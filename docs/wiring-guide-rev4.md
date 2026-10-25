# OxyFeeder — Complete Wiring Guide (Rev 4)

**For:** whoever is wiring the unit
**Date:** Sept 26, 2026 · **Rev 4**
**Assumes:** all modules disconnected and laid out — a fresh build
**Replaces:** [`wiring-guide.md`](wiring-guide.md) (Rev 3) — that file is kept for history, but **this one is correct**
**Bisaya version:** [`guide/WIRING_GUIDE_BISAYA_REV4.md`](../guide/WIRING_GUIDE_BISAYA_REV4.md) — same section numbers
**Visual blueprint:** https://claude.ai/artifact/DF9rpHHmpvLzvZ8Vr99HAr

---

> ## What changed from Rev 3 — read this even if you read Rev 3
>
> Three things were discovered during the first build attempt. Rev 3 is wrong on all three.
>
> | # | Rev 3 said | Rev 4 says | Why |
> |---|---|---|---|
> | 1 | Servo on the **5V POWER** rail | Servo on its own **7.4V** rail | The DS S020B is spec'd **6.0–7.4V DC**. It will not run properly on 5.1V. |
> | 2 | Buzzer is 2-pin (Pin 7 + GND) | Buzzer is a **3-pin module** (VCC · GND · I/O) | The 12V buzzer was replaced with a 5V active module |
> | 3 | Fusebox: 5A, 3A, 5A, 2A | **3A, 3A, 5A, 2A, 5A** | Adding the servo rail changed every branch load |
>
> There are now **three 5V-class rails**, not two, and **three buck converters**, not two.

---

## 0. READ FIRST — THE 6 RULES

Break any one of these and you kill a board. Four Arduino pins are already dead because of rule 1.

| # | NEVER | WHY |
|---|-------|-----|
| 1 | **Never** connect 12V directly to the Arduino 5V pin | This is what killed A0, A1, Pin 8, and Pin 9 |
| 2 | **Never** use **A0, A1, Pin 8, Pin 9, Pin 2** | Dead. They read nothing. Don't retry them. |
| 3 | **Never** power the ESP32 from the Arduino 5V pin | This is the cause of the brownouts. The ESP32 gets its own rail. |
| 4 | **Never** plug USB into the Arduino while main power is ON | Two power sources fight each other |
| 5 | **Never** connect a buck converter output before setting its voltage | If it ships at 12V, everything downstream dies |
| 6 | **Never** use a logic level shifter for the data line | Two boards were tested, neither worked on UART. Use the divider. |

**Rule 7, new in Rev 4:** never raise the 5V POWER rail to feed the servo. The SIM800L and ESP32-CAM live on that rail and are 5V parts. The servo gets its own converter.

---

## 1. PARTS & TOOLS

### Components
- [ ] Arduino Mega 2560
- [ ] ESP32 Dev Module
- [ ] ESP32-CAM
- [ ] TFT LCD 4.0" ST7796S (480×320)
- [ ] DO Sensor (DFRobot SEN0237)
- [ ] Ultrasonic Sensor (HC-SR04) — **only one is used**
- [ ] RTC Module DS3231 **+ CR2032 coin cell** (often sold with the clip empty)
- [ ] Voltage Sensor (25V module)
- [ ] SIM800L V2 (the blue board)
- [ ] **Servo — DS S020B, 20kg, 6.0–7.4V** (see §11)
- [ ] DC Motor
- [ ] L298N Motor Driver
- [ ] Fan
- [ ] **Buzzer — 3-pin active module** (VCC · GND · I/O)
- [ ] Solar Panel
- [ ] Solar Charge Controller — **use the MPPT.** The PWM one is a spare.
- [ ] Battery 12V
- [ ] Toggle Switch
- [ ] 6-way blade Fusebox
- [ ] Buck Converter **5A ×1** — for the servo rail
- [ ] Buck Converter **3A ×2** — for the LOGIC and POWER rails

> ⛔ **Load Cell + HX711 — NOT USED.** Replaced by the ultrasonic sensor. Every HX711 line in the
> firmware is commented out. Wiring it does nothing. Put it in a different box.

### Supplies
- [ ] **Terminal block / bus bar — ×4** (GND, 5V LOGIC, 5V POWER, 7.4V SERVO)
- [ ] Resistor **1K — ×3** (voltage divider, §7.3–7.5)
- [ ] Resistor **10K — ×2** (pull-downs, §7.10–7.11)
- [ ] Capacitor **1000uF 16V — ×1** (for the SIM800L)
- [ ] Wire: **18AWG** (power), **22AWG** (signal)
- [ ] Heat shrink / electrical tape
- [ ] **Multimeter** — REQUIRED. Do not start without one.
- [ ] Label tape / marker

### Fuses (blade type)

| Amps | Colour | Qty |
|------|--------|-----|
| **5A** | tan | 2 |
| **3A** | violet | 2 |
| **2A** | grey | 1 |

> Blade fuses are colour-coded. If you see **red**, that's a 10A — wrong for every slot in this build.

### Only 5 resistors in the entire build
3× 1K for the divider, 2× 10K for the pull-downs. Everything else is a direct wire.
You may use 10K for the divider instead (the ratio is what matters, not the value) — but never mix
values *within* the divider, and the pull-downs must be 10K.

---

## 2. CONFIRM EVERYTHING IS DISCONNECTED

- [ ] **2.1** — Toggle switch is OFF
- [ ] **2.2** — Battery is disconnected from the charge controller
- [ ] **2.3** — Solar panel is disconnected from the charge controller
- [ ] **2.4** — Probe the fusebox with the multimeter. Must read **0V**.
- [ ] **2.5** — No module has any wire still attached — including the ones you think are fine
- [ ] **2.6** — All modules laid out separately, not stacked, not touching
- [ ] **2.7** — Put the HX711 and load cell in a different box so nobody reaches for them later

---

## 3. BUILD THE BUS BARS (RAILS)

**Four** terminal blocks now.

- [ ] **3.1** — Mount the 4 terminal blocks in the enclosure, away from the motor and away from heat
- [ ] **3.2** — Label them with tape and marker:
  - Block 1 = **GND** (black tape)
  - Block 2 = **5V LOGIC** (red tape)
  - Block 3 = **5V POWER** (yellow tape)
  - Block 4 = **7.4V SERVO** (purple tape)
- [ ] **3.3** — Photograph the labels

### The rails

| Rail | Volts | Converter | What connects to it |
|------|-------|-----------|---------------------|
| **5V LOGIC** | 5.10V | Buck **3A** | Arduino, ESP32, TFT LCD, RTC, DO sensor, Ultrasonic, Buzzer |
| **5V POWER** | 5.10V | Buck **3A** | SIM800L, ESP32-CAM |
| **7.4V SERVO** | 7.4V | Buck **5A** | Servo only |
| **GND** | — | — | EVERYTHING. No exceptions. |

### Why they're separated

- **LOGIC vs POWER:** the SIM800L yanks ~2A when it sends a text. That dip travels down whatever wire
  it's on. If the ESP32 shares that wire it browns out and resets, and Bluetooth drops.
- **SERVO on its own:** a 20kg servo pulls 2–3A at stall, and it needs 7.4V, which would destroy the
  5V parts. Separate voltage *and* separate converter.

> ### ⚠️ THE MOST IMPORTANT RULE
> Every single GND lands on the GND bus bar. If even one is missing, sensor readings go random and
> data stops reaching the app. **No resistor ever goes in a ground wire.**

---

## 4. POWER CHAIN — Solar → Fusebox

```
Solar Panel
   │
   ▼
Charge Controller (MPPT)   ← connect the BATTERY first, not the panel
   │
   ├──▶ Battery 12V
   │
   └──▶ LOAD output
          │
          ▼
      Toggle Switch (master ON/OFF)
          │
          ▼
      6-way Fusebox
```

- [ ] **4.1** — Charge Controller **BATTERY terminal** → Battery
  - `BAT+ → Battery +` · `BAT- → Battery -`
  - **Battery first, never the panel first** — an MPPT controller reads battery voltage to know what
    system it is charging
- [ ] **4.2** — Charge Controller **PV terminal** → Solar Panel
  - `PV+ → Panel +` · `PV- → Panel -`
  - Cover the panel with cloth while wiring
- [ ] **4.3** — Charge Controller **LOAD+** → Toggle Switch (middle / COM terminal)
- [ ] **4.4** — Toggle Switch other terminal → **Fusebox positive input**
- [ ] **4.5** — Charge Controller **LOAD-** → **GND bus bar**
- [ ] **4.6** — **Fusebox ground** → **GND bus bar**

### Fuse assignments — CHANGED IN REV 4

| Slot | Fuse | Colour | Feeds | Real load at 12V |
|------|------|--------|-------|------------------|
| **1** | **3A** | violet | Buck 3A → **5V LOGIC** | ~0.3A |
| **2** | **3A** | violet | Buck 3A → **5V POWER** | ~1.1A |
| **3** | **5A** | tan | **L298N** motor driver 12V | ~2–3A |
| **4** | **2A** | grey | **Fan** 12V | ~0.3A |
| **5** | **5A** | tan | Buck 5A → **7.4V SERVO** | ~2A at stall |
| **6** | — | — | spare — leave empty | — |

- [ ] **4.7** — Insert the 5 fuses into the correct slots

> **Why slot 5 is 5A, not 2A:** a 20kg servo at stall draws ~3A at 7.4V, which is ~2A on the 12V input
> side. A 2A fuse would blow in the middle of a feed.
>
> **The voltage sensor does not get a slot.** It draws 0.3 mA — tap its measuring input off the
> fusebox positive input bar (see §9).

---

## 5. SET THE BUCK CONVERTERS

> ### 🔴 STOP — the most dangerous step in the build
> Buck converters often arrive set to 12V output. Wire that to a rail and you lose the Arduino,
> ESP32, LCD and every sensor at once. **Set the voltage with the output disconnected.**

**Your converters only adjust volts.** The "3A" and "5A" are the module's fixed current rating, not a
setting. Tell them apart by the chip: **LM2596 ≈ 3A** (smaller), **XL4015 ≈ 5A** (bigger, heatsink).

### Converter A — 3A → LOGIC rail

- [ ] **5.1** — Connect the **INPUT only**: `Fusebox Slot 1 (+) → IN+` · `GND bus bar → IN-`
- [ ] **5.2** — **Leave the OUTPUT connected to nothing.**
- [ ] **5.3** — Toggle switch ON
- [ ] **5.4** — Multimeter probes on **OUT+** and **OUT-**
- [ ] **5.5** — Turn the small blue potentiometer slowly. It takes many turns.
- [ ] **5.6** — Stop at **5.10V** (5.05–5.15V is fine)
- [ ] **5.7** — Toggle OFF, then ON. Measure again — **must still read 5.10V.** If it drifted, the pot
  is loose or the converter is faulty. Replace it.

### Converter B — 3A → POWER rail
- [ ] **5.8** — Input only: `Fusebox Slot 2 (+) → IN+` · `GND bus bar → IN-`
- [ ] **5.9** — Repeat 5.3–5.7. Target **5.10V**

### Converter C — 5A → SERVO rail
- [ ] **5.10** — Input only: `Fusebox Slot 5 (+) → IN+` · `GND bus bar → IN-`
- [ ] **5.11** — Repeat 5.3–5.7. Target **7.4V**
  - *6.0V is also in spec and gentler on the servo (22 kg·cm instead of 28 kg·cm). Either works — a
    feed gate does not need full torque.*
- [ ] **5.12** — Toggle switch OFF

### Only now connect the outputs

- [ ] **5.13** — `Converter A OUT+ → 5V LOGIC bar` · `OUT- → GND bar` (18AWG)
- [ ] **5.14** — `Converter B OUT+ → 5V POWER bar` · `OUT- → GND bar` (18AWG)
- [ ] **5.15** — `Converter C OUT+ → 7.4V SERVO bar` · `OUT- → GND bar` (18AWG)

> **Triple-check before this step:** LOGIC and POWER read **5.10V**, SERVO reads **7.4V**. Swapping the
> servo converter onto a 5V rail puts 7.4V into the ESP32 and SIM800L.

---

## 6. ARDUINO MEGA

- [ ] **6.1** — `5V LOGIC bus bar → Arduino 5V pin`
- [ ] **6.2** — `GND bus bar → Arduino GND` (use both GND pins)
- [ ] **6.3** — **Do not use the Vin pin. Leave it empty.**

### USB RULE

When uploading firmware: **1.** Toggle OFF → **2.** disconnect the 5V wire from the Arduino 5V pin →
**3.** plug USB, upload → **4.** unplug USB → **5.** reconnect 5V → **6.** toggle ON.

**Normal:** the LCD goes white when the Arduino runs on USB. Not broken — it returns on main power.

---

## 7. ESP32 — DATA BRIDGE

### Power
- [ ] **7.1** — `5V LOGIC bus bar → ESP32 VIN`
- [ ] **7.2** — `GND bus bar → ESP32 GND` — **direct wire, no resistor**

### Data: Arduino → ESP32 (voltage divider)

The Arduino outputs 5V; the ESP32 tolerates 3.3V. The signal passes **through** the first resistor —
that is what drops the voltage.

```
Arduino Pin 1 (TX0)   5V
        │
      [1K]
        │
        ├──────────────▶ ESP32 GPIO26   ≈3.3V   ← tap the JUNCTION
        │
      [1K]
        │
      [1K]
        │
    GND bus bar
```

- [ ] **7.3** — From `Arduino Pin 1 (TX0)`, solder one **1K resistor**
- [ ] **7.4** — At the far end of that resistor, make a **junction** (one solder point where wires meet)
- [ ] **7.5** — From the junction, solder **two 1K resistors in series**, then to the **GND bus bar**
- [ ] **7.6** — From the junction, run a wire → `ESP32 GPIO26`
- [ ] **7.7** — Power ON with the Arduino running, measure the junction: must read about **3.3V**

> 🔴 **Tap the junction, not the Arduino pin.** If the two ground-side resistors hang off Pin 1 instead
> of the junction, GPIO26 sees nearly **5V** and the ESP32 input is damaged. If you measure 5V there,
> stop and rebuild the divider.

### Commands: ESP32 → Arduino (GPIO pulses)

The ESP32 taps a pin to get the Arduino's attention, like a doorbell. Press **Feed Now** in the app and
GPIO12 goes HIGH for half a second.

```
ESP32 GPIO12:   ________┌──────────┐________
                  LOW   │   HIGH   │   LOW
                        │← 500ms  →│
```

- [ ] **7.8** — `ESP32 GPIO12 (D12) → Arduino Pin 17` (Feed) — direct wire
- [ ] **7.9** — `ESP32 GPIO13 (D13) → Arduino Pin 3` (SMS) — direct wire
- [ ] **7.10** — Pull-down: `Arduino Pin 17 → 10K → GND bus bar`
- [ ] **7.11** — Pull-down: `Arduino Pin 3 → 10K → GND bus bar`

The pull-down **branches off** the signal wire — it is not in it:

```
❌ WRONG — signal gets weakened:
   GPIO12 ●───[10K]───● Pin 17

✅ RIGHT — resistor branches to GND:
   GPIO12 ●─────┬─────● Pin 17
                │
              [10K]
                │
            GND bus bar
```

Without it the pin **floats** — it acts as an antenna, picks up motor and mains noise, and reads
randomly HIGH. That means random feeding at 3am.

> **10K, not 1K.** The Mega needs ≥3.0V to read HIGH and the ESP32 only outputs 3.3V — 0.3V of
> headroom. 10K leaves ~3.27V at the pin; 1K drags it to ~3.14V and the trigger fails intermittently.
>
> **Don't use ESP32 D2 or D5** (weak output). **Don't use Arduino Pin 2** (dead).

### ⚠️ Serial: ESP32 → Arduino — THE WIRE EVERYONE FORGETS

| FROM | TO |
|------|-----|
| **ESP32 GPIO22 (D22)** | Arduino **Pin 19 (RX1)** |

- [ ] **7.12** — `ESP32 GPIO22 → Arduino Pin 19` — direct wire, **no resistor, no divider**

> **No divider needed:** this is 3.3V into a 5V input, which is safe. A divider is only needed the
> other way (5V into 3.3V — that's Pin 1 → GPIO26).
>
> **If you forget this wire:** live data and Feed Now still work, but **schedules and the phone number
> never save** from the app. Some boards print this pin as `SCL` instead of `D22` — same pin.

---

## 8. TFT LCD — ST7796S, DIRECT SPI

| # | FROM | TO |
|---|------|-----|
| 1 | **5V LOGIC bus bar** | LCD **VCC** |
| 2 | **GND bus bar** | LCD **GND** |
| 3 | Arduino **Pin 52** | LCD **SCK** |
| 4 | Arduino **Pin 51** | LCD **SDI (MOSI)** |
| 5 | Arduino **Pin 40** | LCD **CS** |
| 6 | Arduino **Pin 38** | LCD **DC / RS** |
| 7 | Arduino **3.3V** | LCD **LED** |
| 8 | Arduino **3.3V** | LCD **RST** |

- [ ] **8.1** — Connect the 8 wires
- [ ] **8.2** — LCD mounts on the **lid (outside)** — long wire is fine but **max 20cm** for SPI

> 🔴 **DC = Pin 38, SCK = Pin 52.** These two are the easiest pair in the build to swap. If the display
> stays black, check this first.
>
> **Never** connect LED or RST to 5V — **3.3V only.** **Never** power the LCD from the Arduino 5V pin.

---

## 9. SENSORS

### DO Sensor — DFRobot SEN0237

| FROM | TO |
|------|-----|
| **5V LOGIC bus bar** | DO board **VCC** (red) |
| **GND bus bar** | DO board **GND** (black) |
| DO board **Signal** (blue) | Arduino **A3** |

- [ ] **9.1** — **A3, not A1.** A1 is dead and reads a flat 0.

### Voltage Sensor — 25V module

This module is **passive** — two resistors, no chip. It needs no logic power. The screw terminal
labelled "VCC" is **the voltage being measured**, not a power input.

```
 ┌──────────────────────────────────┐
 │  SCREW TERMINAL      HEADER PINS │
 │  ┌─────┬─────┐      ┌───┬───┬───┐│
 │  │ VCC │ GND │      │ S │ + │ – ││
 │  └─────┴─────┘      └───┴───┴───┘│
 │   ↑ 12V being          ↑ output   │
 │     measured             to A2    │
 └──────────────────────────────────┘
```

| FROM | TO |
|------|-----|
| Fusebox positive input bar (12V) | Screw terminal **VCC** |
| **GND bus bar** | Screw terminal **GND** |
| Header **S** | Arduino **A2** |
| Header **–** | **GND bus bar** |
| Header **+** | *nothing — leave empty* |

- [ ] **9.2** — **A2, not A0.** A0 is dead.
- [ ] **9.3** — **Both grounds matter.** The screw GND completes the divider; the header – gives the
  Arduino a common reference. Miss either and A2 reads garbage.

> **Expect a wrong reading at first.** `VOLTAGE_RATIO = 4.57` in the firmware was calibrated against
> the *old, damaged* module. See §18 for how to recalibrate. Don't chase it as a wiring fault.

### RTC DS3231

| FROM | TO |
|------|-----|
| **5V LOGIC bus bar** | RTC **VCC** |
| **GND bus bar** | RTC **GND** |
| Arduino **Pin 20 (SDA)** | RTC **SDA** |
| Arduino **Pin 21 (SCL)** | RTC **SCL** |

- [ ] **9.4** — Connect the 4 wires
- [ ] **9.5** — **Check the CR2032 coin cell** — must measure **2.8V or higher**. A coin cell is the flat
  round battery in the clip on the back. New modules often ship with the clip **empty**. Without it the
  clock resets to zeros on every power cut and schedules stop firing.

### Ultrasonic HC-SR04 — the feed level sensor

| FROM | TO |
|------|-----|
| **5V LOGIC bus bar** | HC-SR04 **VCC** |
| **GND bus bar** | HC-SR04 **GND** |
| Arduino **Pin 13** | HC-SR04 **TRIG** |
| Arduino **Pin 48** | HC-SR04 **ECHO** |

- [ ] **9.6** — Connect the 4 wires
- [ ] **9.7** — Mount at the **top of the hopper, facing straight down**, level and unobstructed
- [ ] **9.8** — Confirm **nothing** is wired to Pin 10 or Pin 11 (the old HX711 pins)

> No divider needed on ECHO — the Mega is a 5V board. The onboard **L** LED will blink because it's tied
> to Pin 13. Normal, not a fault. The firmware supports **one** sensor only.

---

## 10. SIM800L V2 — the blue board

The hungriest module in the system — up to **2A** when sending a text. That's why it's on the POWER
rail, not LOGIC.

| FROM | TO |
|------|-----|
| **5V POWER bus bar** | SIM800L **VCC** |
| **GND bus bar** | SIM800L **GND** |
| SIM800L **TXD** | Arduino **Pin 15 (RX3)** |
| SIM800L **RXD** | Arduino **Pin 14 (TX3)** |

- [ ] **10.1** — Connect the 4 wires
- [ ] **10.2** — **Solder the 1000uF capacitor** across VCC and GND **on the SIM800L board itself**
  (not on the bus bar)
  - Long leg = **+** → VCC
  - Short leg (white stripe beside it) = **–** → GND
  - 🔴 **Backwards and the capacitor explodes. Check twice.**
- [ ] **10.3** — Insert the SIM card. **The slot is loose** — put a small piece of paper behind the SIM
- [ ] **10.4** — Connect the antenna. **Never power it on without the antenna.**

> The V2 (blue) board has an onboard regulator — 5V direct is fine, no diodes needed.

---

## 11. MOTORS, SERVO, FAN, BUZZER

### L298N Motor Driver + DC Motor

| FROM | TO |
|------|-----|
| **Fusebox Slot 3 (12V)** | L298N **12V** |
| **GND bus bar** | L298N **GND** |
| Arduino **Pin 4** | L298N **IN1** |
| Arduino **Pin 5** | L298N **IN2** |
| Arduino **Pin 12** | L298N **ENA** |
| L298N **OUT1 / OUT2** | DC Motor (two wires) |

- [ ] **11.1** — Connect the wires above
- [ ] **11.2** — **Remove the L298N 5V-enable jumper** (small blue jumper beside the 12V terminal).
  **Never connect the L298N 5V pin to the Arduino.**

> 🔴 **IN1 = Pin 4, IN2 = Pin 5, ENA = Pin 12.** There is no IN3 in this firmware — the third wire is
> **ENA**. Older docs say Pin 10/11, which is out of date. Getting this shifted by one is a common
> mistake and the motor simply won't run.

### Servo — DS S020B · ⚠️ CHANGED IN REV 4

**Spec: 20kg · 6.0–7.4V DC · IP67 · 25T spline.** This is **not** a 5V servo. At 5.1V it will stutter,
fail to hold position, or not move at all.

| FROM | TO |
|------|-----|
| **7.4V SERVO bus bar** | Servo **RED** (VCC) |
| **GND bus bar** | Servo **BLUE** (GND) |
| Arduino **Pin 6** | Servo **PURPLE** (signal) |

- [ ] **11.3** — Connect the 3 wires
- [ ] **11.4** — ⚠️ **Verify the wire colours against your actual servo before powering on.** On a
  standard 3-pin servo connector the **middle pin is always VCC** and should be the red one. Reverse
  VCC and signal and you damage the servo.

> **The signal wire stays 5V logic on Pin 6** regardless of the servo's supply voltage. Servos read
> pulse *width*, not voltage. This one accepts the standard 500–2500µs that `Servo.h` sends, so **no
> firmware change is needed.**
>
> **Torque:** 7.4V → 28 kg·cm · 6.0V → 22 kg·cm. A feed gate doesn't need full torque; 6.0V is gentler.

### Fan

- [ ] **11.5** — **12V fan:** `Fusebox Slot 4 (+) → Fan +` · `GND bus bar → Fan –`
- [ ] **11.6** — **5V fan:** `5V POWER bus bar → Fan +` · `GND bus bar → Fan –`
- [ ] The fan blows **outward** (exhaust). Check the arrow on its side.

### Buzzer — 3-pin module · ⚠️ CHANGED IN REV 4

Read the silkscreen labels — pin order varies by brand, so don't go by position.

| Buzzer pin | TO |
|------------|-----|
| **VCC** (or `+`, `5V`) | **5V LOGIC bus bar** |
| **GND** (or `–`, `G`) | **GND bus bar** |
| **I/O** (or `S`, `SIG`, `IN`) | Arduino **Pin 7** |

- [ ] **11.7** — Connect the 3 wires
- [ ] **11.8** — The buzzer mounts on the **lid (outside)**
- [ ] **11.9** — **Verify it's an ACTIVE buzzer.** Connect VCC and GND, then briefly touch I/O to 5V:
  - **Continuous beep** → active ✅ the firmware works as written
  - **One click then silence** → passive ⚠️ the firmware must be changed to use `tone()`

> The firmware drives it with plain `digitalWrite(HIGH)`, which only makes sound on an **active**
> buzzer. Most 3-pin modules are active. Draw is ~30mA — the LOGIC rail handles it fine.

---

## 12. ESP32-CAM

Standalone. No data wire to the Arduino — WiFi only.

| FROM | TO |
|------|-----|
| **5V POWER bus bar** | ESP32-CAM **5V** |
| **GND bus bar** | ESP32-CAM **GND** |

- [ ] **12.1** — Two wires. Done.
- [ ] **12.2** — **Do not connect IO0 to GND** — that's only for uploading firmware

---

## 13. COMPLETE PIN TABLE — ARDUINO MEGA

Print this and tape it next to the board.

| Arduino Pin | Goes to | Note |
|-------------|---------|------|
| **5V** | 5V LOGIC bus bar | never Vin |
| **GND** ×2 | GND bus bar | use both pins |
| **3.3V** | LCD LED + LCD RST | never 5V |
| **Pin 1 (TX0)** | divider → ESP32 GPIO26 | 3× 1K |
| **Pin 3** | ESP32 D13 (SMS cmd) | + 10K pull-down |
| **Pin 4** | L298N IN1 | |
| **Pin 5** | L298N IN2 | |
| **Pin 6** | Servo signal (purple) | servo VCC on 7.4V rail |
| **Pin 7** | Buzzer **I/O** | 3-pin module |
| **Pin 10** | *(empty — HX711 not used)* | |
| **Pin 11** | *(empty — HX711 not used)* | |
| **Pin 12** | L298N ENA | |
| **Pin 13** | Ultrasonic TRIG | onboard L LED blinks — normal |
| **Pin 14 (TX3)** | SIM800L RXD | |
| **Pin 15 (RX3)** | SIM800L TXD | |
| **Pin 17** | ESP32 D12 (Feed cmd) | + 10K pull-down |
| **Pin 19 (RX1)** | ESP32 GPIO22 | schedules / phone number |
| **Pin 20 (SDA)** | RTC SDA | |
| **Pin 21 (SCL)** | RTC SCL | |
| **Pin 38** | LCD **DC** | not Pin 52 |
| **Pin 40** | LCD CS | |
| **Pin 48** | Ultrasonic ECHO | |
| **Pin 51 (MOSI)** | LCD SDI | |
| **Pin 52 (SCK)** | LCD **SCK** | not Pin 38 |
| **A2** | Voltage sensor S | |
| **A3** | DO sensor signal | |

### DEAD PINS — NEVER USE

| Pin | Condition |
|-----|-----------|
| **A0** | Internal leakage — readings wrong |
| **A1** | Dead — reads flat 0 |
| **Pin 8** | Shorted to ground |
| **Pin 9** | Shorted to ground |
| **Pin 2** | Dead |
| **Vin** | Don't use — the 5V pin is the only power input |

### ESP32 pin summary

| ESP32 Pin | Goes to |
|-----------|---------|
| **VIN** | 5V LOGIC bus bar |
| **GND** | GND bus bar — **direct, no resistor** |
| **GPIO26** | Voltage divider junction (data from Arduino TX0) |
| **GPIO12 (D12)** | Arduino Pin 17 (Feed) |
| **GPIO13 (D13)** | Arduino Pin 3 (SMS) |
| **GPIO22 (D22)** | Arduino Pin 19 / RX1 (schedules, phone number) |

**Dead ESP32 pins:** D2, D5 (weak output), GPIO4 and GPIO17 (UART TX broken).

---

## 14. POWER-UP TEST — ONE MODULE AT A TIME

> **Do not switch everything on at once.** Measure at each step. If a 5V rail drops below **4.80V**,
> **STOP** — you've found the problem.

| # | Do this | Expected | ✓ |
|---|---------|----------|---|
| **T1** | All modules disconnected from rails. Converters only. ON. | LOGIC **5.10V** · POWER **5.10V** · SERVO **7.4V** | [ ] |
| **T2** | Connect the **Arduino** only. ON. | LOGIC **4.95V+**. Arduino LED lights. | [ ] |
| **T3** | Add the **ESP32**. ON. | LOGIC **4.90V+**. ESP32 LED **steady**, not flickering. | [ ] |
| **T4** | Add the **LCD**. ON. | LOGIC **4.85V+**. LCD shows a display. | [ ] |
| **T5** | Add **RTC, DO, Ultrasonic, Voltage sensor, Buzzer**. ON. | LOGIC **4.85V+** | [ ] |
| **T6** | Add the **SIM800L**. ON. | POWER **4.80V+**. Network LED blinks slowly = registered. | [ ] |
| **T7** | Add the **Servo**. ON. Command a movement. | SERVO rail holds **7.0V+** while moving, **ESP32 must not reset** | [ ] |
| **T8** | Add the **L298N + motor**. ON. Run it. | No rail sags below 4.80V | [ ] |
| **T9** | Add the **ESP32-CAM + Fan**. ON. | Everything stable, nothing flickering | [ ] |

**If a step fails:** go back one, disconnect what you just added, and check that module's wiring. Do
not continue until it passes.

---

## 15. FUNCTION TEST

- [ ] **F1** — LCD powers on and shows a display (not white, not black)
- [ ] **F2** — ESP32 LED is steady
- [ ] **F3** — Voltage sensor reads on the LCD (~12–13V after calibration, see §18)
- [ ] **F4** — RTC shows a time on the LCD (not 00:00)
- [ ] **F5** — Feed level reads (from the ultrasonic)
- [ ] **F6** — App connects over BLE ("OxyFeeder")
- [ ] **F7** — Data reaches the app (DO, feed, battery)
- [ ] **F8** — "Feed Now" in the app → servo moves **and** the motor spins
- [ ] **F9** — "Test SMS" in the app → a text arrives
- [ ] **F10** — Camera stream shows in the app
- [ ] **F11** — Add a schedule in the app, power-cycle the ESP32, confirm it still saved *(tests the
      GPIO22 → Pin 19 wire)*

---

## 16. TROUBLESHOOTING

| Symptom | Cause | Do this |
|---------|-------|---------|
| **ESP32 completely dead, Arduino fine** | The Arduino was on 12V/Vin so it proves nothing about the 5V rail | Measure **LOGIC rail → GND**. 0V = no converter output wired to it. Also continuity-test ESP32 GND → GND bar. |
| **ESP32 GND has a resistor in it** | Miswire | 🔴 Remove it. No resistor ever goes in a ground wire. |
| ESP32 LED flickering | Not enough current, or a loose GND | Measure LOGIC while it flickers. Check every GND screw. |
| ESP32 resets when the servo moves | Servo sharing a rail | Servo must be on its **own 7.4V converter** |
| **Servo buzzes / won't hold / won't move** | **Supply below 6.0V** | Measure the SERVO rail — must be 6.0–7.4V. It will not work on 5.1V. |
| Servo jitters at rest | Normal for a digital servo between commands | The firmware detaches it after each feed — ignore |
| LCD is white | Arduino on USB | Normal. Unplug USB, use main power. |
| LCD is black, nothing on it | **DC and SCK swapped**, or low 5V | DC = **Pin 38**, SCK = **Pin 52**. Measure VCC **at the LCD**, not the bar. |
| Motor doesn't run | IN1/IN2/ENA shifted by one | IN1=**4**, IN2=**5**, ENA=**12**. There is no IN3. |
| **DO = 0.00** | **Probe is dry — no electrolyte** | **Not a wiring problem.** Waiting on NaOH 0.5 mol/L. |
| **RTC = 00:00** | Dead or missing coin cell | Measure the CR2032 (2.8V+). I2C scanner should find **0x68**. |
| Battery % obviously wrong | `VOLTAGE_RATIO` still set for the old module | Recalibrate — see §18 |
| No data in the app | Divider wrong, or GND not common | Junction must read **≈3.3V**. 5V there = rebuild the divider. |
| Feed Now does nothing | Loose pull-down or wrong pin | 10K from Pin 17 → GND. Check the wire from ESP32 D12. |
| **Schedules / phone number won't save** | **GPIO22 → Pin 19 wire missing** | The wire everyone forgets. See §7.12. |
| Buzzer clicks but doesn't beep | It's a **passive** buzzer | Firmware needs `tone()` instead of `digitalWrite` |
| No SMS | No SIM, loose SIM, or not enough current | Paper behind the SIM. Check the 1000uF cap. Check antenna. |
| A fuse blows during feeding | Slot 5 undersized | Servo branch needs **5A**, not 2A |
| Converter getting hot | Overloaded | Check each module is on the correct rail |
| Arduino resets randomly | Loose 5V or GND | Tighten everything. Measure 5V right at the pin. |

---

## 17. FINAL CHECK

- [ ] Every wire is tight, none loose
- [ ] Every GND lands on the GND bus bar — and **no ground wire contains a resistor**
- [ ] LOGIC = 5.10V · POWER = 5.10V · **SERVO = 7.4V** (verify before the servo is connected)
- [ ] No wire touches the motor or any hot surface
- [ ] All four bus bars are labelled
- [ ] Photographs taken of the finished wiring
- [ ] **LCD, toggle switch and buzzer** are on the **lid (outside)**
- [ ] Everything else is **inside** the enclosure
- [ ] Cable glands tight, no way for water to get in

---

## 18. FIRMWARE NOTES

**No pin changes are needed.** This guide was written *from* the firmware, so wiring to it means the
`#define`s already match. Flash as-is. But three calibration values need attention:

### 1. 🔴 `VOLTAGE_RATIO` — almost certainly wrong now
`oxyfeeder_firmware.ino` line 137: `const float VOLTAGE_RATIO = 4.57;`
Calibrated against the **old, damaged** voltage sensor. A new module has different resistors.

1. Measure actual battery voltage with the multimeter
2. Read what the Serial Monitor reports (9600 baud)
3. `new ratio = 4.57 × (actual ÷ reported)`
4. Edit line 137, re-flash

### 2. Hopper calibration
Lines 117–118: `HOPPER_EMPTY_CM = 37` · `HOPPER_FULL_CM = 5`.
Measure with the ultrasonic mounted and adjust to your real hopper.

### 3. SMS cooldown still in test mode
Line 186: `SMS_COOLDOWN = 10000` (10 seconds). Fine on the bench; raise it before deployment or a
sustained alert will burn through load credits.

### Things that don't change
1. `dispenseFeed()` must stay **BLOCKING** (delay-based) — legacy of the HX711 interrupt problem
2. Pin 17 and Pin 3 must be `INPUT`, **not** `INPUT_PULLUP` — a pullup fights the pull-down resistor
3. Serial baud **9600** (Arduino → ESP32)
4. ESP32 Serial2: **RX = GPIO26**, no TX on that port
5. 15-second startup delay before GPIO commands are read (prevents false triggers on boot)
6. DO calibration debug print is already in place, ready for when the electrolyte arrives
7. Load cell / HX711 is commented out — ultrasonic is the feed level sensor
8. The servo's 7.4V supply needs **no** firmware change — servos read pulse width, not voltage

### Known bugs (not wiring — don't chase them as faults)
- **Feed duration is ignored.** The app sends `FEED:5` but the Arduino runs a hardcoded
  `dispenseFeed(2)`. Every feed is 2 seconds. Scheduled feeds lose their duration the same way.
- **Phone numbers without `+` don't survive a reboot.** `loadPhoneNumberFromEEPROM()` only accepts
  stored data starting with `+`, but the app hint and firmware default use local format. Save without
  `+`, power-cycle, and it silently reverts to the default.

---

## 19. DOCUMENT SYNC

| File | Language | Status |
|------|----------|--------|
| `docs/wiring-guide-rev4.md` | English | ✅ **current — this file** |
| `guide/WIRING_GUIDE_BISAYA_REV4.md` | Bisaya | ✅ current, same section numbers |
| Blueprint artifact | Diagrams | ✅ current |
| `docs/wiring-guide.md` | English | ⚠️ Rev 3 — kept for history, superseded |
| `guide/WIRING_GUIDE_BISAYA.md` | Bisaya | ⚠️ Rev 2/3 — kept for history, superseded |
| `CLAUDE.md`, `hardware-overview.md`, `sensor-list.md` | — | ❌ badly out of date on pins |

Change a wire in one current file, change it in the others and bump the Rev in each.

| Rev | Date | Change |
|-----|------|--------|
| 1 | — | Original — wrong ultrasonic pins, still had the load cell |
| 2 | Sept 16, 2026 | Ultrasonic → Pin 13/48, load cell dropped, GPIO22 → Pin 19 added |
| 3 | Sept 22, 2026 | English version; rewritten for a fresh build; pins re-verified against firmware |
| **4** | **Sept 26, 2026** | **Servo moved to its own 7.4V rail (DS S020B is 6.0–7.4V); buzzer rewired as a 3-pin module; fuse plan redone for 5 branches; voltage sensor wiring completed with both grounds; troubleshooting rows added from the first build attempt** |
