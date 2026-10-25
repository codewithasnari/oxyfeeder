# OxyFeeder — Complete Wiring Guide

> # ⚠️ SUPERSEDED — USE REV 4 INSTEAD
> ### → [`wiring-guide-rev4.md`](wiring-guide-rev4.md) · Bisaya: [`WIRING_GUIDE_BISAYA_REV4.md`](../guide/WIRING_GUIDE_BISAYA_REV4.md)
>
> This file is Rev 3, kept for history. **Three things below are wrong:**
>
> 1. **Servo on the 5V POWER rail** — the DS S020B is a **6.0–7.4V** servo and will not run on 5.1V. It needs its own 7.4V converter.
> 2. **Buzzer as 2-pin** — it is a **3-pin module** (VCC · GND · I/O).
> 3. **Fuse plan (5A/3A/5A/2A)** — replaced by **3A/3A/5A/2A/5A** now that the servo has its own branch.
>
> Everything else in this file still matches the firmware, but read Rev 4 — it carries these fixes plus troubleshooting from the first build attempt.


**For:** whoever is wiring the unit
**Date:** Sept 22, 2026 · **Rev 3**
**Assumes:** all modules are disconnected and laid out — a fresh build, not a repair
**Bisaya version:** [`guide/WIRING_GUIDE_BISAYA.md`](../guide/WIRING_GUIDE_BISAYA.md) — same section numbers, so "Section 7.12" means the same wire in both files

> ### This guide matches the firmware
> Every pin below was checked against `firmware/oxyfeeder_firmware/oxyfeeder_firmware.ino` and
> `firmware/esp32_communicator/esp32_communicator.ino` on Sept 22, 2026.
>
> **Older documents disagree with this one and are wrong.** `CLAUDE.md`, `docs/hardware-overview.md`,
> and `docs/sensor-list.md` still describe the load cell, pins A0/A1, motor pins 10/11, and the logic
> level shifter. None of that is in the firmware anymore. If two documents disagree, this one wins.

---

## 0. READ FIRST — THE 6 RULES

Break any one of these and you kill a board. Four Arduino pins are already dead because of rule 1.

| # | NEVER | WHY |
|---|-------|-----|
| 1 | **Never** connect 12V directly to the Arduino 5V pin | This is what killed A0, A1, Pin 8, and Pin 9 |
| 2 | **Never** use **A0, A1, Pin 8, Pin 9, Pin 2** | Dead. They read nothing. Don't retry them. |
| 3 | **Never** power the ESP32 from the Arduino 5V pin | This is the cause of the brownouts. ESP32 gets its own rail now. |
| 4 | **Never** plug USB into the Arduino while main power is ON | Two power sources fight each other |
| 5 | **Never** connect a buck converter output before setting it to 5.1V | If it ships at 12V, everything downstream dies |
| 6 | **Never** use a logic level shifter for the data line | Two boards were tested, neither worked on UART. Use the divider. |

---

## 1. PARTS & TOOLS

### Components
- [ ] Arduino Mega 2560
- [ ] ESP32 Dev Module
- [ ] ESP32-CAM
- [ ] TFT LCD 4.0" ST7796S (480x320)
- [ ] DO Sensor (DFRobot SEN0237)
- [ ] Ultrasonic Sensor (HC-SR04) — **only one is used**
- [ ] RTC Module DS3231 (+ CR2032 coin cell)
- [ ] Voltage Sensor (25V module)
- [ ] ~~Load Cell + HX711~~ — **NOT USED** (replaced by the ultrasonic sensor)
- [ ] SIM800L V2 (the blue board)
- [ ] Servo Motor
- [ ] DC Motor
- [ ] L298N Motor Driver
- [ ] Fan
- [ ] Buzzer
- [ ] Solar Panel
- [ ] Solar Charge Controller — **use the MPPT.** The PWM one is a spare, don't use it.
- [ ] Battery 12V
- [ ] Toggle Switch
- [ ] 6-way Fusebox
- [ ] Buck Converter **5A** (x1)
- [ ] Buck Converter **3A** (x1)

### Supplies
- [ ] **Terminal block / bus bar — x3** (GND rail, 5V LOGIC rail, 5V POWER rail)
- [ ] Resistor **1K — x3** (voltage divider)
- [ ] Resistor **10K — x2** (pull-downs)
- [ ] Capacitor **1000uF 16V — x1** (for the SIM800L)
- [ ] Wire: **18AWG** (power), **22AWG** (signal)
- [ ] Heat shrink / electrical tape
- [ ] **Multimeter** — REQUIRED. Do not start without one.
- [ ] Label tape / marker

### Fuses
- [ ] 5A x2, 3A x1, 2A x1

---

## 2. CONFIRM EVERYTHING IS DISCONNECTED

You are starting fresh, so this is a verification pass, not a teardown. It still takes two minutes and it still matters.

- [ ] **2.1** — Toggle switch is OFF
- [ ] **2.2** — Battery is disconnected from the charge controller
- [ ] **2.3** — Solar panel is disconnected from the charge controller
- [ ] **2.4** — Probe the fusebox with the multimeter. Must read **0V**. If there is any voltage, stop and find out why.
- [ ] **2.5** — No module has any wire still attached to it — including the ones you think are fine
- [ ] **2.6** — All modules laid out separately on the table, not stacked, not touching
- [ ] **2.7** — Set the HX711 and the load cell aside, in a different box if you can. They are not part of this build and you should not be tempted by them later.

---

## 3. BUILD THE BUS BARS (RAILS)

This is the centre of the whole system. Three terminal blocks.

- [ ] **3.1** — Mount the 3 terminal blocks in the enclosure, away from the motor and away from heat
- [ ] **3.2** — Label them with tape and marker:
  - Block 1 = **GND** (black tape)
  - Block 2 = **5V LOGIC** (red tape)
  - Block 3 = **5V POWER** (yellow tape)
- [ ] **3.3** — Photograph the labels

### Why two 5V rails?

The servo and the GSM module draw sudden large current — the SIM800L hits 2A when it sends a text. If they share a rail with the ESP32 and the LCD, the voltage dips and the ESP32 resets. So they are kept apart.

| Rail | Converter | What connects to it |
|------|-----------|---------------------|
| **5V LOGIC** | Buck **5A** | Arduino, ESP32, TFT LCD, RTC, DO sensor, Ultrasonic, Voltage sensor |
| **5V POWER** | Buck **3A** | Servo, SIM800L, ESP32-CAM |
| **GND** | — | EVERYTHING. No exceptions. |

> **THE MOST IMPORTANT RULE:** every single GND must land on the GND bus bar. If even one is missing,
> sensor readings go random and data stops reaching the app.

---

## 4. POWER CHAIN (Solar → Fusebox)

```
Solar Panel
   |
   v
Charge Controller (MPPT)   <-- connect the BATTERY first, not the panel
   |
   +--> Battery 12V
   |
   +--> LOAD output
          |
          v
      Toggle Switch (master ON/OFF)
          |
          v
      6-way Fusebox
```

- [ ] **4.1** — Charge Controller **BATTERY terminal** → Battery
  - `Controller BAT+ → Battery +`
  - `Controller BAT- → Battery -`
  - **Battery goes on first. Never the panel first** — an MPPT controller needs to see the battery
    voltage to know what system it is charging.
- [ ] **4.2** — Charge Controller **PV terminal** → Solar Panel
  - `Controller PV+ → Panel +`
  - `Controller PV- → Panel -`
  - Cover the panel with cloth while wiring
- [ ] **4.3** — Charge Controller **LOAD+** → Toggle Switch terminal 1
- [ ] **4.4** — Toggle Switch terminal 2 → **Fusebox positive input**
- [ ] **4.5** — Charge Controller **LOAD-** → **GND bus bar**
- [ ] **4.6** — **Fusebox ground** → **GND bus bar**

### Fuse assignments

| Slot | Fuse | Feeds |
|------|------|-------|
| 1 | **5A** | Buck Converter 5A (input) |
| 2 | **3A** | Buck Converter 3A (input) |
| 3 | **5A** | L298N Motor Driver 12V |
| 4 | **2A** | Fan 12V |
| 5 | — | spare (empty) |
| 6 | — | spare (empty) |

- [ ] **4.7** — Insert the 4 fuses into the correct slots

---

## 5. SET THE BUCK CONVERTERS

> **STOP. This is the most dangerous step in the build.** Get it wrong and everything dies at once.
> Buck converters often arrive from the shop set to 12V output. If you wire that straight to the 5V
> rail, you lose the Arduino, ESP32, LCD, and every sensor in one go.

### Converter 5A (LOGIC rail)

- [ ] **5.1** — Connect the **INPUT only**:
  - `Fusebox Slot 1 (+) → Converter IN+`
  - `GND bus bar → Converter IN-`
- [ ] **5.2** — **Leave the OUTPUT connected to nothing. Completely free.**
- [ ] **5.3** — Turn the toggle switch ON
- [ ] **5.4** — Multimeter probes on the converter's **OUT+** and **OUT-**
- [ ] **5.5** — Turn the small potentiometer (the blue square) with a screwdriver. Slowly. It takes many turns.
- [ ] **5.6** — Stop at **5.10V** (5.05V – 5.15V is fine)
- [ ] **5.7** — Toggle OFF, then ON again. Measure again. **Must still read 5.10V.** If it drifted, the
  pot is loose or the converter is faulty — replace it.

### Converter 3A (POWER rail)

- [ ] **5.8** — Connect the **INPUT only**:
  - `Fusebox Slot 2 (+) → Converter IN+`
  - `GND bus bar → Converter IN-`
- [ ] **5.9** — Repeat 5.3 through 5.7. Same target: **5.10V**
- [ ] **5.10** — Turn the toggle switch OFF

### Only now connect the outputs to the rails

- [ ] **5.11** — `Converter 5A OUT+ → 5V LOGIC bus bar` (18AWG)
- [ ] **5.12** — `Converter 5A OUT- → GND bus bar`
- [ ] **5.13** — `Converter 3A OUT+ → 5V POWER bus bar` (18AWG)
- [ ] **5.14** — `Converter 3A OUT- → GND bus bar`

---

## 6. ARDUINO MEGA

- [ ] **6.1** — `5V LOGIC bus bar → Arduino 5V pin`
- [ ] **6.2** — `GND bus bar → Arduino GND` (use both GND pins, to be safe)
- [ ] **6.3** — **Do not use the Vin pin. Leave it empty.**

### USB RULE — read this

When uploading new firmware to the Arduino:

1. Toggle switch OFF
2. **Disconnect the 5V wire from the Arduino 5V pin**
3. Plug in USB → upload
4. Unplug USB
5. Reconnect the 5V wire
6. Toggle switch ON

**This is normal:** the LCD goes white when the Arduino is running on USB. It is not broken. It comes back on main power.

---

## 7. ESP32 (Data Bridge)

The ESP32 has its own power now. **No longer from the Arduino.**

### Power
- [ ] **7.1** — `5V LOGIC bus bar → ESP32 VIN`
- [ ] **7.2** — `GND bus bar → ESP32 GND`

### Data: Arduino → ESP32 (Voltage Divider)

The Arduino outputs 5V; the ESP32 only tolerates 3.3V. It needs a divider.

```
Arduino Pin 1 (TX0)
        |
      [1K]
        |
        +--------------------> ESP32 GPIO26
        |
      [1K]
        |
      [1K]
        |
    GND bus bar
```

- [ ] **7.3** — From `Arduino Pin 1 (TX0)`, solder one **1K resistor**
- [ ] **7.4** — At the far end of that resistor, make a junction (one solder point where wires meet)
- [ ] **7.5** — From the junction, solder **two 1K resistors in series**, then to the **GND bus bar**
- [ ] **7.6** — From the junction, run a wire → `ESP32 GPIO26`
- [ ] **7.7** — With power ON and the Arduino running, measure the junction: should read about **3V**

> **Do not use a logic level shifter here.** Two boards were tested; neither worked for UART.
> Only the divider works.

### Commands: ESP32 → Arduino (GPIO pulses)

- [ ] **7.8** — `ESP32 GPIO12 (D12) → Arduino Pin 17` (Feed command) — direct wire
- [ ] **7.9** — `ESP32 GPIO13 (D13) → Arduino Pin 3` (SMS command) — direct wire
- [ ] **7.10** — Pull-down: `Arduino Pin 17 → 10K resistor → GND bus bar`
- [ ] **7.11** — Pull-down: `Arduino Pin 3 → 10K resistor → GND bus bar`

> **10K, not 1K.** A 1K pull-down is too strong — it drags the signal down and the trigger never fires.
>
> **Don't use ESP32 D2 or D5** — weak output. **Don't use Arduino Pin 2** — dead.

### ⚠️ Commands: ESP32 → Arduino (Serial — DON'T FORGET THIS ONE)

Besides the two GPIO pulses above, there is **one more serial wire** that is easy to miss.
The phone number, the feeding schedules, and the replies from the app all travel through it.

| FROM | TO |
|------|-----|
| **ESP32 ** | Arduino **Pin 19 (RX1)** |

- [ ] **7.12** — `ESP32 GPIO22 → Arduino Pin 19` — direct wire, **no resistor, no divider**

> **Why no divider here?** This is 3.3V going into a 5V Arduino input, which is safe. A divider is only
> needed in the other direction, 5V into 3.3V (that's Pin 1 → GPIO26).
>
> **If you forget this wire:** live data and Feed Now still work, but **schedules and the phone number
> will not save** from the app.

---

## 8. TFT LCD (ST7796S — Direct SPI)

| Wire # | FROM | TO |
|--------|------|-----|
| 1 | **5V LOGIC bus bar** | LCD **VCC** |
| 2 | **GND bus bar** | LCD **GND** |
| 3 | Arduino **Pin 52** | LCD **SCK** |
| 4 | Arduino **Pin 51** | LCD **SDI (MOSI)** |
| 5 | Arduino **Pin 40** | LCD **CS** |
| 6 | Arduino **Pin 38** | LCD **DC** |
| 7 | Arduino **3.3V** | LCD **LED** |
| 8 | Arduino **3.3V** | LCD **RST** |

- [ ] **8.1** — Connect the 8 wires above
- [ ] **8.2** — The LCD mounts on the **lid (outside)** of the enclosure — use long wire, but **max 20cm** for SPI

> **Do not** connect LED or RST to 5V. **3.3V only.**
> **Do not** power the LCD from the Arduino 5V pin. Bus bar only.

---

## 9. SENSORS

### DO Sensor (DFRobot SEN0237)

| FROM | TO |
|------|-----|
| **5V LOGIC bus bar** | DO board **VCC** (red) |
| **GND bus bar** | DO board **GND** (black) |
| DO board **Signal** (blue) | Arduino **A3** |

- [ ] **9.1** — **A3, not A1.** A1 is dead and reads a flat 0.

### Voltage Sensor

| FROM | TO |
|------|-----|
| Voltage sensor **signal (S)** | Arduino **A2** |
| Voltage sensor **GND (–)** | **GND bus bar** |
| Voltage sensor **input terminal** | Battery + and – (so it reads the battery) |

- [ ] **9.2** — **A2, not A0.** A0 is dead.

### RTC DS3231

| FROM | TO |
|------|-----|
| **5V LOGIC bus bar** | RTC **VCC** |
| **GND bus bar** | RTC **GND** |
| Arduino **Pin 20 (SDA)** | RTC **SDA** |
| Arduino **Pin 21 (SCL)** | RTC **SCL** |

- [ ] **9.3** — Connect
- [ ] **9.4** — **Check the coin cell (CR2032).** Measure it: must be **2.8V or higher**. If it's low,
  replace it. If it's dead, the clock resets to zeros on every power cut.

### HX711 + Load Cell — ⛔ DO NOT WIRE (NO LONGER USED)

> **Skip this.** The load cell has been **replaced by the ultrasonic sensor** for feed level.
> In the firmware every HX711 line is commented out (`// #include <HX711.h>`).
> If you wire it, **nothing happens** — no code reads it.
>
> Set the HX711 and the load cell aside. Do not connect them to the Arduino.

- [ ] **9.5** — Confirm there is **no** wire on Arduino Pin 10 or Pin 11. Both must be empty.

### Ultrasonic Sensor (HC-SR04) — this is the feed level sensor now

> **The firmware uses exactly one sensor**, not two. This is what replaced the load cell for
> measuring the feed level in the hopper.

| FROM | TO |
|------|-----|
| **5V LOGIC bus bar** | HC-SR04 **VCC** |
| **GND bus bar** | HC-SR04 **GND** |
| Arduino **Pin 13** | HC-SR04 **TRIG** |
| Arduino **Pin 48** | HC-SR04 **ECHO** |

- [ ] **9.6** — Connect the 4 wires. **Pin 13 (TRIG) and Pin 48 (ECHO)** — this is what the firmware uses.
- [ ] **9.7** — Mount the sensor at the **top of the hopper, facing straight down**, level and unobstructed
- [ ] No voltage divider needed on ECHO — the Mega is a 5V board, so it's fine
- [ ] **NOTE:** the Arduino's onboard LED (L) will blink — it's tied to Pin 13. That's normal, not a fault.

> **If you have a second ultrasonic sensor:** set it aside. The firmware supports only one.

---

## 10. SIM800L V2 (the blue board)

> This is the hungriest module in the system. It pulls up to 2A when sending a text.
> That's why it goes on the **POWER rail**, not LOGIC.

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
  - Short leg (the one with the white stripe beside it) = **–** → GND
  - **Backwards and the capacitor explodes. Check it twice.**
- [ ] **10.3** — Insert the SIM card. **The slot is loose** — put a small piece of paper behind the SIM
  so the contacts stay tight.
- [ ] **10.4** — Connect the antenna. Never power it on without the antenna.

> The SIM800L V2 (blue) has an onboard regulator — 5V direct is fine. No diodes needed.

---

## 11. MOTORS, FAN, BUZZER

### L298N Motor Driver + DC Motor

| FROM | TO |
|------|-----|
| **Fusebox Slot 3 (12V)** | L298N **12V** |
| **GND bus bar** | L298N **GND** |
| Arduino **Pin 4** | L298N **IN1** |
| Arduino **Pin 5** | L298N **IN2** |
| Arduino **Pin 12** | L298N **ENA** |
| L298N **OUT1 / OUT2** | DC Motor (two wires) |

- [ ] **11.1** — Connect
- [ ] **11.2** — **Remove the L298N 5V-enable jumper** (the small blue jumper next to the 12V terminal)
  - Why: we are not using its onboard 5V regulator
  - **Never connect the L298N 5V pin to the Arduino**

> **Note:** IN1=Pin 4 and IN2=Pin 5. Older docs say Pin 10 and Pin 11 — that is out of date and
> will not work with the current firmware.

### Servo Motor

| FROM | TO |
|------|-----|
| **5V POWER bus bar** | Servo **red (VCC)** |
| **GND bus bar** | Servo **brown/black (GND)** |
| Arduino **Pin 6** | Servo **orange/yellow (signal)** |

- [ ] **11.3** — **POWER rail, not LOGIC** — sharing a rail makes the ESP32 reset every time the servo moves

### Fan

- [ ] **11.4** — If it's a **12V fan**: `Fusebox Slot 4 (+) → Fan +`, `GND bus bar → Fan –`
- [ ] **11.5** — If it's a **5V fan**: `5V POWER bus bar → Fan +`, `GND bus bar → Fan –`
- [ ] The fan blows **outward** (exhaust). Check the arrow on the side of the fan.

### Buzzer

| FROM | TO |
|------|-----|
| Arduino **Pin 7** | Buzzer **+** |
| **GND bus bar** | Buzzer **–** |

- [ ] **11.6** — The buzzer mounts on the **lid (outside)**
- [ ] NOTE: currently a 12V buzzer, so it sounds weak. Waiting on a 5V active buzzer.

---

## 12. ESP32-CAM

Standalone. No data wire to the Arduino — WiFi only.

| FROM | TO |
|------|-----|
| **5V POWER bus bar** | ESP32-CAM **5V** |
| **GND bus bar** | ESP32-CAM **GND** |

- [ ] **12.1** — Two wires. Done.
- [ ] **12.2** — **Do not connect IO0 to GND** — that is only for uploading firmware

---

## 13. COMPLETE PIN TABLE (Arduino Mega)

Print this and tape it next to the Arduino.

| Arduino Pin | Goes to |
|-------------|---------|
| **5V** | 5V LOGIC bus bar |
| **GND** (x2) | GND bus bar |
| **3.3V** | LCD LED + LCD RST |
| **Pin 1 (TX0)** | Voltage divider → ESP32 GPIO26 |
| **Pin 3** | ESP32 D13 (SMS cmd) + 10K pull-down |
| **Pin 4** | L298N IN1 |
| **Pin 5** | L298N IN2 |
| **Pin 6** | Servo signal |
| **Pin 7** | Buzzer + |
| **Pin 10** | *(empty — HX711 no longer used)* |
| **Pin 11** | *(empty — HX711 no longer used)* |
| **Pin 12** | L298N ENA |
| **Pin 13** | Ultrasonic TRIG |
| **Pin 14 (TX3)** | SIM800L RXD |
| **Pin 15 (RX3)** | SIM800L TXD |
| **Pin 17** | ESP32 D12 (Feed cmd) + 10K pull-down |
| **Pin 19 (RX1)** | ESP32 GPIO22 (schedules / phone number) |
| **Pin 20 (SDA)** | RTC SDA |
| **Pin 21 (SCL)** | RTC SCL |
| **Pin 38** | LCD DC |
| **Pin 40** | LCD CS |
| **Pin 48** | Ultrasonic ECHO |
| **Pin 51 (MOSI)** | LCD SDI |
| **Pin 52 (SCK)** | LCD SCK |
| **A2** | Voltage sensor signal |
| **A3** | DO sensor signal |

### DEAD PINS — NEVER USE

| Pin | Condition |
|-----|-----------|
| **A0** | Internal leakage — readings are wrong |
| **A1** | Dead — reads a flat 0 |
| **Pin 8** | Shorted to ground |
| **Pin 9** | Shorted to ground |
| **Pin 2** | Dead |
| **Vin** | Don't use — the 5V pin is the only power input |

### ESP32 pin summary

| ESP32 Pin | Goes to |
|-----------|---------|
| **VIN** | 5V LOGIC bus bar |
| **GND** | GND bus bar |
| **GPIO26** | Voltage divider junction (data from Arduino TX0) |
| **GPIO12 (D12)** | Arduino Pin 17 (Feed) |
| **GPIO13 (D13)** | Arduino Pin 3 (SMS) |
| **GPIO22** | Arduino Pin 19 / RX1 (schedules, phone number) |

**Dead ESP32 pins:** D2, D5 (weak output), GPIO4 and GPIO17 (UART TX broken)

---

## 14. POWER-UP TEST — ONE MODULE AT A TIME

> **Do not switch everything on at once.** One at a time. Measure the rail at each step.
> If it drops below **4.80V**, **STOP** — you have found the problem.

Set up: multimeter probes on the **5V bus bar** and the **GND bus bar**.

| # | Do this | Expected | ✓ |
|---|---------|----------|---|
| **T1** | Disconnect ALL modules from the rails. Converters only. Switch ON. | **5.10V** on both rails | [ ] |
| **T2** | Connect the **Arduino** only. ON. | **4.95V or higher.** Arduino LED lights. | [ ] |
| **T3** | Add the **ESP32**. ON. | **4.90V or higher.** ESP32 LED **steady**, not flickering. | [ ] |
| **T4** | Add the **LCD**. ON. | **4.85V or higher.** LCD shows a display. | [ ] |
| **T5** | Add the **RTC, DO, Ultrasonic, Voltage sensor**. ON. | **4.85V or higher** | [ ] |
| **T6** | Add the **SIM800L**. ON. | POWER rail **4.80V or higher.** Network LED blinks (slow blink = registered) | [ ] |
| **T7** | Add the **Servo + L298N**. ON. Test a movement. | Small dip while moving, but **the ESP32 must not reset** | [ ] |
| **T8** | Add the **ESP32-CAM + Fan**. ON. | Everything stable, nothing flickering | [ ] |

**If a step fails:** go back one step, disconnect what you just added, and check that module's wiring. Do not continue until it passes.

---

## 15. FUNCTION TEST (after the power test passes)

- [ ] **F1** — LCD powers on and shows a display (not white, not black)
- [ ] **F2** — ESP32 LED is steady
- [ ] **F3** — Voltage sensor reads on the LCD (should be about 12–13V)
- [ ] **F4** — RTC shows a time on the LCD (not 00:00)
- [ ] **F5** — Feed level reads (from the ultrasonic, not the load cell)
- [ ] **F6** — App connects over BLE ("OxyFeeder")
- [ ] **F7** — Data reaches the app (DO, feed, battery)
- [ ] **F8** — "Feed Now" in the app → the servo moves
- [ ] **F9** — "Test SMS" in the app → a text arrives
- [ ] **F10** — Camera stream shows in the app

---

## 16. TROUBLESHOOTING

| Symptom | Cause | Do this |
|---------|-------|---------|
| ESP32 LED flickering | Not enough current, or a loose GND | Measure 5V LOGIC while it flickers. Check the GND bus bar — is every screw tight? |
| LCD is white | Arduino is on USB | Normal. Unplug USB, run on main power only. |
| LCD is black, nothing on it | Not enough 5V, or a loose SPI wire | Measure VCC **at the LCD itself**, not at the bus bar. Check all 6 SPI wires. |
| Rail voltage sags | Wire gauge too thin | Replace with 18AWG from the converter to the bus bar |
| **DO = 0.00** | **Probe is dry — no electrolyte** | **Not a wiring problem.** Still waiting on NaOH 0.5 mol/L. Don't hunt for a wiring fault. |
| **RTC = 00:00** | Dead coin cell, or not visible on I2C | Measure the CR2032 (2.8V+). Run an I2C scanner — it should find **0x68**. |
| ESP32 resets when the servo moves | They share a rail | Move the servo to the **POWER rail** |
| No data in the app | Divider broken, or GND not common | Measure the junction: should be about **3V** while the Arduino is sending |
| Feed Now doesn't work | Loose pull-down or wrong pin | Check: 10K from Pin 17 → GND. Check the wire from ESP32 D12. |
| Schedules / phone number won't save | **The GPIO22 → Pin 19 wire is missing** | This is the wire everyone forgets. See step 7.12. |
| No SMS | No SIM, loose SIM, or not enough current | Put paper behind the SIM. Check the 1000uF cap. |
| Converter getting hot | Overloaded | Split the load. Check each module is on the correct rail. |
| Arduino resets randomly | Loose 5V wire or GND | Tighten everything. Measure 5V right at the Arduino pin. |

---

## 17. FINAL CHECK — WHEN YOU'RE DONE

- [ ] Every wire is tight, none loose
- [ ] Every GND lands on the GND bus bar
- [ ] No wire touches the motor or any hot surface
- [ ] All bus bars are labelled
- [ ] Photographs taken of the finished wiring (for reference if something breaks)
- [ ] The **LCD, toggle switch, and buzzer** are on the **lid (outside)**
- [ ] Everything else is **inside** the enclosure
- [ ] Cable glands are tight, no way for water to get in

---

## 18. FOR WHOEVER DOES THE FIRMWARE (not wiring, but don't forget)

1. **`dispenseFeed()` must stay BLOCKING** (delay-based). This came from the old HX711 problem — it
   disabled interrupts and wrecked the servo PWM. Even though the HX711 is gone, the blocking code is
   still what's in the firmware. Don't change it until it's been tested.
2. Pin 17 and Pin 3 must be **`INPUT`**, not `INPUT_PULLUP` — a pullup fights the pull-down resistor.
3. Serial baud: **9600** (Arduino → ESP32)
4. ESP32 Serial2: **RX = GPIO26**, no TX on that port
5. 15-second startup delay before GPIO commands are read (prevents false triggers on boot)
6. DO calibration: the debug print is already in place (`[DO CAL] Raw ADC: X | Voltage: X.XXXX V`) —
   ready for when the electrolyte arrives
7. The **load cell / HX711 is commented out** in the firmware — the ultrasonic (Pin 13/48) is the feed
   level sensor now
8. Hopper calibration lives in `HOPPER_EMPTY_CM = 37` and `HOPPER_FULL_CM = 5` — adjust to the real hopper
9. The ESP32 sends commands to Arduino **Pin 19 (RX1)** from **GPIO22** — not only GPIO pulses

---

## 19. DOCUMENT SYNC

This file and [`guide/WIRING_GUIDE_BISAYA.md`](../guide/WIRING_GUIDE_BISAYA.md) are the same guide in two
languages and use **identical section numbers**. If you change a wire in one, change it in the other and
bump the Rev number in both headers.

| Rev | Date | Change |
|-----|------|--------|
| 1 | — | Original (superseded — had the wrong ultrasonic pins and still included the load cell) |
| 2 | Sept 16, 2026 | Ultrasonic moved to Pin 13/48, load cell dropped, GPIO22 → Pin 19 wire added |
| 3 | Sept 22, 2026 | English version added; rewritten for a fresh build instead of a teardown; pin table re-verified against firmware; L298N pin note and the schedules-won't-save troubleshooting row added |
