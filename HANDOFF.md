# OxyFeeder — Handoff

**Last updated:** 26 October 2026
**For:** anyone (human or AI assistant) picking this project up cold on a different machine.

Read this file first. It tells you what works, what does not, what was learned the hard way,
and what to do next. Everything here was verified against real hardware, not assumed.

---

## 1. State of play — read this table first

| Subsystem | Status | Notes |
|---|---|---|
| **Arduino Mega firmware** | ✅ Working | Boots to `SYSTEM READY`, main loop runs |
| **TFT LCD (ST7796S)** | ✅ Working | Was white-screening; cause was an upstream hang, see §5 |
| **SMS via SIM800L** | ✅ **WORKING** | Took four stacked faults to fix, see §4. Texts arrive. |
| **DC motor + L298N** | ✅ Working | Spins only on feed command, as designed |
| **Servo (hopper gate)** | ✅ Working | Needs its own **7.4V** rail — it is a 6.0–7.4V unit |
| **Voltage sensor** | ✅ Working | Recalibrated 15 Oct, `VOLTAGE_RATIO = 4.99` |
| **Ultrasonic (feed level)** | ✅ Working | **Original sensor was dead.** Replaced 12 Oct. Pins 13/48. |
| **DO sensor** | ⚠️ Reads, **uncalibrated** | Needs electrolyte + two-point calibration. See §6. |
| **RTC DS3231** | ❌ **Module faulty** | Hangs the I²C bus. Firmware now survives it — see §5. |
| **ESP32 BLE bridge** | ✅ Working | App connects, data flows |
| **ESP32-CAM** | ⚠️ Untested recently | Hotspot `OxyFeeder-CAM` / pass `oxyfeeder123` |
| **Flutter app** | ✅ Working | Dashboard, settings, Feed Now, Test SMS all functional |

> ⚠️ **The team previously believed "we only need to fix the text feature."**
> That is **no longer true** — SMS was fixed on 14 October. What actually remains is
> DO calibration, the feed-duration bug, the RTC replacement, and the paper. See §7.

---

## 2. What the machine is

Solar-powered floating fish feeder with dissolved-oxygen monitoring, for a capstone project
at University of Mindanao Digos College.

```
Sensors ──▶ Arduino Mega 2560 ──(TX0, voltage divider)──▶ ESP32 ──BLE──▶ Flutter app
                   │                                        │
                   ├─▶ TFT LCD, buzzer, SIM800L (SMS)        └─▶ GPIO pulses back to Arduino
                   └─▶ L298N + DC motor, servo gate
ESP32-CAM ──WiFi──▶ Flutter app (independent of everything else)
```

**Repo layout**

| Path | Contains |
|---|---|
| `app/` | Flutter mobile app (MVVM + Provider) |
| `firmware/oxyfeeder_firmware/` | Arduino Mega production firmware — **the main one** |
| `firmware/esp32_communicator/` | ESP32 BLE bridge |
| `firmware/camera_server/` | ESP32-CAM WiFi stream |
| `firmware/*_test/` | Single-purpose diagnostic sketches |
| `docs/` | Guides, data sheets, paper material |
| `guide/` | Bisaya-language wiring guide |

---

## 3. ⚠️ Documentation: which files to trust

Several documents contradict each other. This is the precedence order:

| File | Trust | Why |
|---|---|---|
| **`docs/wiring-guide-rev4.md`** | ✅ **AUTHORITATIVE** | Verified against firmware. Use this for every pin. |
| **`guide/WIRING_GUIDE_BISAYA_REV4.md`** | ✅ Authoritative | Same content in Bisaya, identical section numbers |
| `docs/wiring-guide.md` (Rev 3) | ⚠️ Superseded | Servo on 5V (wrong), 2-pin buzzer, old fuse plan |
| `guide/WIRING_GUIDE_BISAYA.md` | ⚠️ Superseded | Rev 2/3 |
| `CLAUDE.md` | ❌ **BADLY STALE** | Load cell, A0/A1, motor pins 10/11, logic level shifter — none of this is real any more |
| `docs/hardware-overview.md` | ❌ Stale pins | Concepts fine, pin numbers wrong |
| `docs/sensor-list.md` | ❌ Stale | Still lists the load cell |
| `docs/roadmap.md` | ❌ Stale | Status table contradicts its own body |

**If two documents disagree, Rev 4 wins.**

---

## 4. The SMS saga — what it took (14 Oct)

Four separate faults stacked on top of each other. Worth reading, because the debugging
pattern will repeat.

| # | Fault | Symptom | Fix |
|---|---|---|---|
| 1 | **RTC hung the I²C bus** | Firmware never reached `loop()`, so SMS commands were never read **and** the LCD never initialised | Disconnected RTC; then added a timeout guard (§5) |
| 2 | **SIM card contact** | `AT+CPIN?` → `ERROR` | Paper shim behind the SIM — the V2 board's slot is loose |
| 3 | **Not attached to a network** | `+CREG: 0,0` then `0,2` forever | Needed operator selection. `AT+COPS=0`, then it found SMART |
| 4 | **Echo overflowed the UART buffer** | Firmware reported `SMS FAILED` on sends that worked | `ATE0` before the AT sequence |

**Network facts discovered:**
- SIM is **SMART / TNT**. `AT+COPS=?` showed `SMART (51503)` available, `Globe (51502)` forbidden.
- Signal `AT+CSQ` ≈ **11–14** — usable.
- **Registration takes 2–3 minutes from cold boot.** SMS will not work before that.
- Lock the operator so it does not hunt on every boot:
  ```
  AT+COPS=1,2,"51503"
  ```

**Diagnostic tool:** `firmware/gsm_passthrough/gsm_passthrough.ino` gives raw AT access.
Serial Monitor must be **9600** with line ending **Both NL & CR**. Type `~` to send Ctrl+Z,
`` ` `` to send ESC.

---

## 5. Firmware changes, September–October

All in `firmware/oxyfeeder_firmware/oxyfeeder_firmware.ino`.

### 🔴 RTC I²C timeout guard — the most important one

A damaged DS3231 holds SDA/SCL low. `Wire` has no timeout on AVR, so `rtc.begin()` blocked
**forever**. `setup()` never finished, which meant the LCD never initialised, the GSM never
initialised, and `loop()` never ran. One hang presented as three unrelated faults.

```cpp
Wire.setWireTimeout(25000, true);        // 25 ms, reset bus on timeout
Wire.beginTransmission(0x68);
bool rtcResponds = (Wire.endTransmission() == 0);   // probe before handing to the library
```

Boot now continues with `RTC DS3231: NOT FOUND - continuing without it`.
**Requires Arduino AVR core ≥ 1.8.0** for `setWireTimeout`.

### Other changes

| Change | Why |
|---|---|
| `ATE0` before the SMS sequence | Module echo was overflowing the 64-byte `Serial3` buffer and destroying the `+CMGS:` confirmation |
| `normalizePhoneNumber()` | Converts `09xxxxxxxxx` → `+639xxxxxxxxx` automatically, on boot and on set |
| Default number → `+639550717546` | Networks reject local format for `AT+CMGS` |
| EEPROM accepts a leading digit | Previously only `'+'` was accepted, so local-format numbers were silently discarded on every reboot |
| **Combined alert SMS** | One text naming every active problem instead of one per problem. Re-sends when the *set* of alerts changes, otherwise hourly. |
| **Low-DO SMS added** | Previously low oxygen only buzzed — no text. This made the paper's Objective 2 untrue. |
| Feed confirmation SMS | After each feed. Disable with `#define SMS_ON_FEED false` |
| `VOLTAGE_RATIO = 4.99` | Calibrated 15 Oct: multimeter 12.22 V vs firmware-reported 11.20 V |
| Battery scale → **11.8–12.7 V** | Was 11.0–14.4 V. **14.4 V is the charging voltage, not a full battery** — under the old numbers a fully charged battery could never read above 50%. |
| CSCA/CSQ/COPS diagnostics | Printed when a send or registration fails |

---

## 6. Known bugs — still open

### 🔴 Feed duration is ignored
The app sends `FEED:5`, but the ESP32 only pulses a GPIO (which carries no duration), and the
Arduino runs a hardcoded `dispenseFeed(2)`. **Every feed is 2 seconds**, scheduled or manual.
The duration dropdown in Settings does nothing.

**Fix:** have the ESP32 send `FEED:n` over the existing **GPIO22 → Pin 19** serial wire, and
add `FEED:` to the accepted prefixes in the Arduino's `processIncomingCommands()` (it currently
accepts only `PHONE:` and `THRESHOLD:` from Serial1). Touches both firmwares.

### 🟠 Scheduling does not survive a power cut
`checkFeedingSchedule()` on the Arduino is **dead code** — `shouldFeed` is never set true.
Scheduling actually runs on the **ESP32**, using `millis()` seeded by `SYNC_TIME` when the
app connects.

Consequence: after any power cut, **someone must open the app once** or no schedule fires.
The RTC is not used for scheduling at all.

⚠️ The paper claims RTC-based scheduling that works "even when the mobile device was
disconnected." **That is not true of the current firmware.** Either move scheduling onto the
RTC or reword the claim.

### 🟡 DO sensor uncalibrated
`readDissolvedOxygen()` uses a guessed linear map (`voltage / 3.0 * 20.0`). Needs a real
two-point calibration — see §7.

### 🟠 App: every reading arrives twice
`real_bluetooth_service.dart` enables BLE notifications (line ~310) **and** starts a 2-second
`Timer.periodic` polling fallback (line ~326) at the same time. In `flutter_blue_plus` a
`read()` also fires `onValueReceived`, so each poll emits the same status through
`_statusStreamController` twice — once from the read, once from the notification.

Visible as a jittery chart and a history log with duplicated rows. **Keep the polling only as
a fallback**: cancel the timer on the first successful notification, or drop it entirely now
that notifications are confirmed working.

### 🟠 App: `dispose()` writes to closed streams
```dart
void dispose() {
  _pollingTimer?.cancel();
  disconnect();                      // async, NOT awaited
  _statusStreamController.close();   // runs before disconnect finishes
  _connectionStateController.close();
}
```
`disconnect()` pushes a state update after the controllers are closed → `Bad state: Cannot add
event after closing`. Make `dispose()` async and `await disconnect()` first.

### 🟡 App: Sensors screen shows LIVE before any data
`_lastUpdate` is initialised to `DateTime.now()` at construction, and `isReceivingData` is
`now - _lastUpdate < 10s`. For the first 10 seconds after launch the screen reports live data
**with no hardware connected**. Initialise it to a sentinel instead.

### 🟡 App: dashboard DO colours ignore the user's threshold
`dashboard_screen.dart` hardcodes `4.0` and `6.0` for the colour and the CRITICAL/WARNING
label (lines ~204, ~217) — while line ~464 of the same file tells the user the alert is set to
`settingsVm.minDissolvedOxygen`. **The screen contradicts itself.** Change the threshold in
Settings and the colours do not move.

### 🟡 App: About screen is hardcoded
`Version 1.0.0` / `Build 2024.01.24`. Wrong date, and worth fixing before the defence since
panels do look at it.

### 🟡 Camera auto-discovery is half-built
`camera_server.ino` broadcasts its IP over UDP on port **5556** every few seconds
(`DISCOVERY_PORT`). **Nothing in the app listens for it** — `camera_screen.dart` just uses a
hand-typed IP saved in `SharedPreferences`, defaulting to `oxyfeeder-cam.local`. Either add a
`RawDatagramSocket` listener on 5556 or stop claiming auto-discovery.

### ⚪ Build config not release-ready
- `applicationId` is still `com.example.oxyfeeder_app` (`android/app/build.gradle.kts:24`)
- release builds are signed with the **debug** key (`:37`) — fine for sideloading to the panel,
  not for any store
- `http: ^0.13.6` is a major version behind

### 🟡 `SMS_COOLDOWN` still in test mode
10 seconds, marked "testing mode". Raise it before deployment or a sustained alert will burn
through load credits.

---

## 7. Next steps, in priority order

### 1. Calibrate the DO sensor
The last sensor still producing meaningless numbers.

- Get **sodium sulfite** (Na₂SO₃) — zero reference, ~2 g in 100 mL, stir, wait 5 min. **Mix fresh each session**; once its oxygen is consumed it stops reading zero.
- Get **electrolyte** for the probe if it is still dry.
- Open `docs/DO_calibration.xlsx` — type the air temperature, it looks up the reference DO, you enter the two voltages, and it writes the firmware lines for you.
- Voltages come from the `[DO CAL] Raw ADC: X | Voltage: X.XXXX V` line already in the firmware.

> ⚠️ **Open disagreement on the reference value.** The team has been using **13.5 mg/L** for
> 100% saturation. That value corresponds to water at about **3 °C**. At Digos ambient
> (26–32 °C) the physically correct value is **7.3–8.1 mg/L**. Using 13.5 inflates every
> reading by roughly 70% — which is why the app screenshot in the paper shows 11.4 mg/L
> marked OPTIMAL (that is supersaturated water, not a normal pond), and why the low-DO alarm
> would never fire.
>
> `DO_logging_6days.xlsx` keeps the reference in **one cell** (`Summary!B3`, currently 13.5)
> so it can be changed without redoing any data. The `Sat Table` tab has the correct
> temperature lookup. **Raise this with the adviser before the defence.**

### 2. Fix the feed-duration bug
See §6. Small change, affects both firmwares, makes a visible app feature actually work.

### 3. Replace the RTC module
It is faulty (pulls I²C low). Firmware now survives it, but **no scheduled feeding works
without it**. Order a DS3231 and a fresh CR2032 (must read 2.8 V+).

### 4. Gather the paper's data
`docs/DO_logging_6days.xlsx` — 6 day tabs, four sampling intervals each (20 s, 5 min,
hourly 7 AM–5 PM, and a custom table), with mean / SD / %Sat computed automatically.

**Start the battery log first** — it needs days of elapsed time and nothing else is blocking it.

| Section | Data needed | Blocked? |
|---|---|---|
| III-A Feeding | 20 trials, weight per cycle | by the duration bug |
| III-B Sensors | DO / feed level / battery accuracy | DO by calibration |
| III-C Alerts | 20 SMS, delivery rate + latency | ✅ ready |
| III-D Power | hourly battery voltage, several days | ✅ ready — **start now** |
| III-E Beta testing | 8 weighted criteria, respondent scores | ✅ ready, needs people |

### 5. Finish the paper
See §9.

---

## 8. Gotchas that cost us days

Read these before touching the hardware.

### Dead pins — do not use
| Board | Dead |
|---|---|
| Arduino Mega | **A0, A1, Pin 2, Pin 8, Pin 9**, and `Vin` |
| ESP32 | **D2, D5** (weak output), **GPIO4, GPIO17** (UART TX broken) |

Killed by a 12 V-into-5 V incident. They read nothing. Do not retry them.

### The USB rule
**USB in → disconnect the Arduino's 5V wire. GND never comes off.**
Two power sources fighting each other. The LCD going white on USB is **normal**, not a fault.

For tests needing both the Serial Monitor and rail-powered peripherals: 5V wire off, GND on,
USB in, toggle switch **ON**.

### Three rails, one ground
| Rail | Volts | Feeds |
|---|---|---|
| 5V LOGIC | 5.10 V | Arduino, ESP32, LCD, RTC, DO, ultrasonic, buzzer |
| 5V POWER | 5.10 V | SIM800L, ESP32-CAM |
| **7.4V SERVO** | 7.4 V | Servo only — it is a 6.0–7.4 V unit |

**Many positives, one ground.** 26 ground wires all land on the single GND bar.
No resistor ever goes in a ground wire (this once left the ESP32 unable to power on).

### The wire everyone forgets
**ESP32 GPIO22 → Arduino Pin 19 (RX1).** Without it, live data and Feed Now still work, but
**schedules and the phone number never save** from the app. Direct wire, no resistor.

### The 1000 µF capacitor
Must be soldered **on the SIM800L board itself**, across its own VCC/GND pins — not on the bus
bar. It supplies the 2 A transmit burst locally. On the bus bar it cannot do its job.

### Ultrasonic and feed pellets
Pellets scatter ultrasound. Expect reading spread to grow as the hopper fills. This is a real
limitation of ultrasonic sensing on granular material and is worth reporting rather than
hiding — it is also why a load cell was the original design.

---

## 9. The paper — outstanding issues

`docs/paper-additions.md` holds drafted replacement text. Most of it has been applied. What
remains:

| Issue | Severity |
|---|---|
| **No Section III (Results and Discussion)** | 🔴 Drafted in `paper-additions.md`, never pasted in |
| **No Section IV (Conclusion)** | 🔴 Same |
| **No Abstract, no Index Terms** | 🔴 Required by IEEE format |
| Trade-off matrix arithmetic | 🟠 Reports 3.105 / 2.509; the printed weights and scores give **2.66 / 2.08**. Re-derive or show the formula — a reviewer will recompute it. |
| "Onboard LED indicators" | 🟠 **No LED code exists.** Remove from text and Figure 7, or implement it. |
| "Feed Confirmations" SMS | 🟠 Now true — implemented Oct |
| Low-oxygen SMS in Objective 2 | ✅ Now true — implemented Oct |
| "RTC-based scheduling" | 🟠 Not how it works. See §6. |
| Buck converter described as singular, 5 V | 🟠 There are **three**, one at 7.4 V |
| Figure 9 showing 11.4 mg/L | 🟠 Supersaturated — an artefact of the 13.5 reference |
| B.6 says "Design 1" | 🟡 Should be "Design A" |
| B.5 claims A beat B on cost and sustainability | 🟡 Both are **ties** in the table |

---

## 10. Serial commands (main firmware)

Serial Monitor, **9600**, line ending **Newline** or **Both NL & CR**.

| Command | Does |
|---|---|
| `TEST_SMS:1` | Send a test SMS — prints every AT stage |
| `GET_PHONE:1` | Show the stored number |
| `PHONE:+639XXXXXXXXX` | Set the SMS number |
| `FEED:5` | Manual feed *(note: always runs 2 s — see §6)* |
| `TIME:14:30:00` | Set the RTC |
| `THRESHOLD:DO,4.5` | Set a threshold |

⚠️ These need a **colon**. Raw AT commands do **not** work here — use the passthrough sketch.

---

## 11. Diagnostic sketches

| Sketch | Use |
|---|---|
| `firmware/gsm_passthrough/` | Raw AT access to the SIM800L |
| `firmware/ultrasonic_test/` | Tests **both** pin pairs (13/48 and 10/11) plus pin health, no re-upload needed to switch |
| `firmware/hopper_calibration/` | Marks empty/full, prints `HOPPER_EMPTY_CM` / `HOPPER_FULL_CM` |
| `firmware/sim800l_test/` | Older SIM800L test |
| `firmware/bench_test_diagnostic/` | Multi-component bench test (uses the **old** pinout — treat with care) |

---

## 12. Credentials and identifiers

| Item | Value |
|---|---|
| BLE device name | `OxyFeeder` |
| BLE service UUID | `0000abcd-0000-1000-8000-00805f9b34fb` |
| BLE data characteristic | `0000abce-...` (notify) |
| BLE command characteristic | `0000abcf-...` (write) |
| ESP32-CAM hotspot | `OxyFeeder-CAM` / `oxyfeeder123` |
| Camera URL | `http://oxyfeeder-cam.local/stream` |
| SMS recipient | `+639550717546` |
| Network operator | SMART, numeric `51503` |

---

## 13. Hardware reference — quick pin table

Full detail in `docs/wiring-guide-rev4.md` §13.

| Arduino pin | Connects to |
|---|---|
| 5V / GND ×2 | 5V LOGIC rail / GND bar |
| 3.3V | LCD LED + RST |
| Pin 1 (TX0) | divider → ESP32 GPIO26 |
| Pin 3 | ESP32 D13 (SMS cmd) + 10K pull-down |
| Pin 4 / 5 / 12 | L298N IN1 / IN2 / **ENA** (there is no IN3) |
| Pin 6 | Servo signal (purple) |
| Pin 7 | Buzzer I/O (3-pin module) |
| Pin 13 / 48 | Ultrasonic TRIG / ECHO |
| Pin 14 / 15 | SIM800L RXD / TXD |
| Pin 17 | ESP32 D12 (feed cmd) + 10K pull-down |
| Pin 19 (RX1) | **ESP32 GPIO22** — schedules + phone number |
| Pin 20 / 21 | RTC SDA / SCL |
| Pin 38 / 40 / 51 / 52 | LCD DC / CS / MOSI / SCK |
| A2 / A3 | Voltage sensor / DO sensor |

---

## 14. Loose ends and things never verified

Honest list of what is *not* settled. Do not assume any of these were checked.

**Solar charging was never confirmed.** The battery calibration was fixed (§5) so the reported
percentage is now right, but nobody ever watched the pack actually *rise* on panel power. Leave
it in sun with a multimeter on the battery terminals for an hour. A lead-acid pack on charge
reads ~13.6–14.4 V; resting-full is 12.7 V. If it never climbs, the MPPT or the panel wiring is
the problem, not the firmware.

**L298N 5V jumper.** The guide says remove the onboard 5V-regulator jumper when feeding the
board 12 V from the fuse box. With the jumper removed the L298N's own logic must be fed 5 V
from the logic rail — if neither is done the driver is dead and the motor simply never turns.
**Check this physically before debugging motor code.** It is not recorded anywhere which way
the board is currently set.

**Motor spinning continuously (resolved, but know the cause).** Early on the DC motor ran
non-stop without any feed command. The cause was **the Arduino not running at all** — with no
sketch driving them, IN1/IN2 float and the driver sees a valid direction. If it happens again,
check the Arduino is alive *first*; it is not a motor-control bug.

**`STDEV.S` showed 0 in the team's own sheet** despite a visible range of 2.2 mg/L. Diagnosis
given at the time: the cells are text, not numbers (left-aligned, often from pasting). Fix is
`Data → Text to Columns → Finish` on the column. **Never confirmed fixed** — recheck before
putting any standard deviation in the paper.

**The Excel files were hand-built as raw OOXML** (`[Content_Types].xml` + `xl/*.xml`, zipped
with explicit forward-slash entry names) because there was no Python on the build machine.
They open correctly, but if you need to regenerate or heavily restructure them, install
`openpyxl` and rebuild rather than editing the XML by hand. Note that PowerShell's
`Compress-Archive` writes **backslash** entry names and produces a file Excel rejects.

**`nul` file (fixed in this PR).** The repo contained a file literally named `nul`, a reserved
device name on Windows. It made `git clone` fail outright with `error: invalid path 'nul'` —
the repo was **unclonable on any Windows machine**. Removed in commit `ec4903a` via git
plumbing (`ls-tree` → `mktree` → `commit-tree`), since `git rm` cannot touch a path Windows
will not let git check out. A `.gitattributes` was added at the same time to stop CRLF churn.

**Still tracked but probably shouldn't be:** `OxyFeeder_servo_test_and_restore.zip` and the
`*_FINAL_BACKUP.ino` files. Harmless, but they confuse "which firmware is current" — the
answer is always `firmware/oxyfeeder_firmware/oxyfeeder_firmware.ino`.

**Diagrams from the October sessions were published as private claude.ai artifacts** (wiring
guide, step-by-step blueprint, build sequence, logic rail, power branches, data sheets, worked
examples). They are **private to the original account and will not open on another device**.
Everything in them is also in `docs/wiring-guide-rev4.md`; treat the markdown as the record and
regenerate diagrams if needed.

**The app's event log exports to CSV** — a data source for the paper that is easy to forget.
It is a cleaner provenance trail than hand-typing readings into the spreadsheet.

---

## 15. If you are an AI assistant picking this up

1. Read `docs/wiring-guide-rev4.md` before answering anything about pins. **Ignore `CLAUDE.md`** — it is stale.
2. The firmware is the source of truth. Check the `#define`s rather than trusting any document.
3. SMS **works**. Do not re-debug it unless it actually fails.
4. The DO reference-value disagreement in §7 is unresolved and matters — do not quietly pick a side.
5. Hardware questions are usually **power or connections**, not code. Ask for a multimeter reading before theorising.
6. The team is working toward a capstone defence. Honest limitations beat invented precision — a stated limitation is a strength in a paper, fabricated data is not.
7. **§14 is the honest list.** Several things were never verified — solar charging, the L298N
   jumper, the `STDEV.S` fix. Do not report them as working.
8. The app-layer bugs in §6 were found by reading the code, **not** by reproducing them on a
   device. They are real in the source; confirm the symptom before spending time on a fix.
