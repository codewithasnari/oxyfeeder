# OxyFeeder — Kompletong Wiring Guide (Bag-ong Wiring)

**Para kang kinsa:** ang mag-wire sa system
**Petsa:** Sept 27, 2026 · **Rev 3**
**Status sa daan nga wiring:** GUBA — kinahanglan tanggalon TANAN ug sugdan pag-usab

> ### 🔴 REV 3 — GAMITA NI, AYAW ANG DAAN NGA KOPYA
> **Rev 2 (Sept 16):** ultrasonic = **Pin 13/48** (dili 22/23/24/25, ug **usa ra**) · HX711 **ayaw na i-wire** · dugang nga wire **ESP32 GPIO22 → Arduino Pin 19**
> **Rev 3 (Sept 27):** buzzer **5V active na** (naa na, dili na 12V) · DO sensor **calibrated na** · dugang **smoothing capacitor** sa rail
>
> ⚠️ **Kung naay lain nga AI o lain nga listahan nga mo-ingon og "DO sensor → A1" — SAYOP NA.**
> **PATAY ang A1.** Ang DO sensor naa sa **A3**.

---

## 0. BASAHA UNA — ANG 6 KA BAWAL

Kung usa ra ani ang malapas, maguba ang board. Naa nay 4 ka patay nga pin sa Arduino tungod ani.

| # | BAWAL | NGANO |
|---|-------|-------|
| 1 | **AYAW** i-sumpay ang 12V direkta sa 5V pin sa Arduino | Mao ni ang nakapatay sa A0, A1, Pin 8, Pin 9 |
| 2 | **AYAW** gamita ang **A0, A1, Pin 8, Pin 9, Pin 2** | Patay na. Walay basa. Ayaw na sulayi. |
| 3 | **AYAW** i-power ang ESP32 gikan sa Arduino 5V pin | Mao ni ang hinungdan sa brownout karon |
| 4 | **AYAW** ug plug sa USB sa Arduino samtang naka-ON ang main power | Mag-away ang duha ka power source |
| 5 | **AYAW** i-sumpay ang buck converter output kung wala pa na-set og 5.1V | Kung 12V diay ang output, mapatay ang tanan |
| 6 | **AYAW** gamita ang logic level shifter para sa data | Duha na ka board ang gi-test, pareho dili mogana |

---

## 1. LISTAHAN SA GAMIT

### Components (naa na)
- [ ] Arduino Mega 2560
- [ ] ESP32 Dev Module
- [ ] ESP32-CAM
- [ ] TFT LCD 4.0" ST7796S (480x320)
- [ ] DO Sensor (DFRobot SEN0237)
- [ ] Ultrasonic Sensor (HC-SR04) — **usa ra ang gamiton**
- [ ] RTC Module DS3231
- [ ] Voltage Sensor
- [ ] ~~Load Cell + HX711~~ — **DILI NA GAMITON** (gipulihan sa ultrasonic)
- [ ] SIM800L V2 (asul nga board)
- [ ] Servo Motor
- [ ] DC Motor
- [ ] L298N Motor Driver
- [ ] Fan
- [ ] Buzzer
- [ ] Solar Panel
- [ ] Solar Charge Controller — **PWM (BOSCA)** ang aktuwal nga gigamit. Terminal labels: `S1+/S1-` (panel), `S2+/S2-` (spare, 2nd panel), `B+/B-` (battery), `L+/L-` (LOAD output — mao ni ang "bulb icon" terminal). Mas maayo unta ang MPPT (mas efficient ang charging), pero PWM ra ang naa, ug okay ra ni — walay safety issue, ang LOAD output parehas ra ug uga sa laing bahin sa system.
- [ ] Battery 12V
- [ ] Toggle Switch
- [ ] 6-way Fusebox
- [ ] Buck Converter **5A** (1 ka buok)
- [ ] Buck Converter **3A** (1 ka buok)

### Kinahanglan andamon
- [ ] **Terminal block / bus bar — 3 ka buok** (GND rail, 5V LOGIC rail, 5V POWER rail)
- [ ] Resistor **1K — 3 ka buok** (voltage divider)
- [ ] Resistor **10K — 2 ka buok** (pull-down)
- [ ] Capacitor **1000uF 16V — 1 ka buok** (para sa SIM800L)
- [ ] Capacitor **470uF 16V — 2 ka buok** (smoothing sa kada 5V rail)
- [ ] Wire **18AWG** (power) ug **22AWG** (signal)
- [ ] Heat shrink / electrical tape
- [ ] **Multimeter** — KINAHANGLAN GYUD. Ayaw sugdi kung wala.
- [ ] Label tape / marker

---

## 2. TEARDOWN — TANGGALA TANAN

- [ ] **2.1** — OFF ang toggle switch
- [ ] **2.2** — Tanggala ang battery sa charge controller (negative una, unya positive)
- [ ] **2.3** — Tanggala ang solar panel sa charge controller
- [ ] **2.4** — Sukda ang fusebox gamit multimeter. Dapat **0V**. Kung naay boltahe, ayaw og padayon.
- [ ] **2.5** — Tanggala ang TANANG wire sa tanang module. Tanan gyud. Ayaw pag-pili.
- [ ] **2.6** — I-lain ang tanang module sa lamesa, linis, dili nagsapaw

---

## 3. HIMOA ANG BUS BARS (RAIL / EXTENSION)

Kini ang sentro sa tanan. Tulo ka terminal block.

- [ ] **3.1** — Ibutang ang 3 ka terminal block sa enclosure. Layo sa motor ug sa init.
- [ ] **3.2** — Labeli gamit tape ug marker:
  - Block 1 = **GND** (itom nga tape)
  - Block 2 = **5V LOGIC** (pula nga tape)
  - Block 3 = **5V POWER** (dalag nga tape)
- [ ] **3.3** — Kuhaa og litrato ang labels

### Ngano duha ka 5V rail?

Ang servo ug ang GSM mo-hatag og kalit nga dako nga current (ang SIM800L mo-abot og 2A inig text). Kung parehas silag rail sa ESP32 ug LCD, mo-ubos ang boltahe ug mo-reset ang ESP32. Mao nga bulag sila.

| Rail | Converter | Kinsa naka-sumpay |
|------|-----------|-------------------|
| **5V LOGIC** | Buck **5A** | Arduino, ESP32, TFT LCD, RTC, DO, Ultrasonic, Voltage sensor |
| **5V POWER** | Buck **3A** | Servo, SIM800L, ESP32-CAM |
| **GND** | — | TANAN. Walay exception. |

> **PINAKAIMPORTANTE:** ang TANAN nga GND kinahanglan mosumpay sa GND bus bar. Kung usa ra ang wala, mag-random ang sensor readings ug dili mo-gana ang data sa app.

---

## 4. POWER CHAIN (Solar → Fusebox)

```
Solar Panel
   |
   v
Charge Controller (PWM)   <-- ang BATTERY i-sumpay UNA, dili ang panel
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

- [ ] **4.1** — Charge Controller **B+ / B-** (battery terminal) → Battery
  - `Controller B+ → Battery +`
  - `Controller B- → Battery -`
  - **Ang battery ang UNA gyud i-sumpay. Dili ang panel.**
- [ ] **4.2** — Charge Controller **S1+ / S1-** (panel terminal) → Solar Panel
  - `Controller S1+ → Panel +`
  - `Controller S1- → Panel -`
  - Bakante ra ang S2+/S2- (para sa 2nd panel, wala ta gigamit)
  - Taboni ang panel og tela samtang nag-wire
- [ ] **4.3** — Charge Controller **L+** (bulb icon / LOAD output) → Toggle Switch terminal 1
- [ ] **4.4** — Toggle Switch terminal 2 → **Fusebox positive input**
- [ ] **4.5** — Charge Controller **L-** (bulb icon / LOAD output) → **GND bus bar**
- [ ] **4.6** — **Fusebox ground** → **GND bus bar**

### Fuse assignments

| Slot | Fuse | Padulong asa |
|------|------|--------------|
| 1 | **5A** | Buck Converter 5A (input) |
| 2 | **3A** | Buck Converter 3A (input) |
| 3 | **5A** | L298N Motor Driver 12V |
| 4 | **2A** | Fan 12V |
| 5 | — | spare (bakante) |
| 6 | — | spare (bakante) |

- [ ] **4.7** — Isulod ang 4 ka fuse sa tama nga slot

---

## 5. I-SET ANG BUCK CONVERTERS

> **STOP. Kini ang pinaka-delikado nga step. Kung mali, tanan maguba.**
> Ang buck converter usahay 12V ang output pag-abot gikan sa tindahan. Kung diretso nimo i-sumpay sa 5V rail, mamatay ang Arduino, ESP32, LCD, tanan.

### Converter 5A (LOGIC rail)

- [ ] **5.1** — Sumpaya ang **INPUT** ra:
  - `Fusebox Slot 1 (+) → Converter IN+`
  - `GND bus bar → Converter IN-`
- [ ] **5.2** — **AYAW pa i-sumpay ang OUTPUT bisag asa. Bakante gyud.**
- [ ] **5.3** — ON ang toggle switch
- [ ] **5.4** — Multimeter probe sa **OUT+** ug **OUT-** sa converter
- [ ] **5.5** — Tuyuka ang gamay nga potentiometer (asul nga kwadrado) gamit screwdriver. Hinay-hinay. Daghan tuyok ang kinahanglan.
- [ ] **5.6** — Hunonga inig abot og **5.10V** (5.05V – 5.15V okay ra)
- [ ] **5.7** — OFF ang toggle. ON balik. Sukda usab. **Dapat 5.10V gihapon.** Kung nausab, luag ang pot o guba ang converter — ilisi.

### Converter 3A (POWER rail)

- [ ] **5.8** — Sumpaya ang **INPUT** ra:
  - `Fusebox Slot 2 (+) → Converter IN+`
  - `GND bus bar → Converter IN-`
- [ ] **5.9** — Balika ang 5.3 hangtod 5.7. Target pod: **5.10V**
- [ ] **5.10** — OFF ang toggle switch

### Karon pa i-sumpay ang output sa rails

- [ ] **5.11** — `Converter 5A OUT+ → 5V LOGIC bus bar` (18AWG)
- [ ] **5.12** — `Converter 5A OUT- → GND bus bar`
- [ ] **5.13** — `Converter 3A OUT+ → 5V POWER bus bar` (18AWG)
- [ ] **5.14** — `Converter 3A OUT- → GND bus bar`
- [ ] **5.15** — **Smoothing capacitor:** i-solder ang **470uF** tapat sa kada 5V bus bar (+ sa 5V, – sa GND)
  - Usa sa LOGIC rail, usa sa POWER rail
  - Taas nga paa = **+**. Mubo nga paa (naay puti nga guhit) = **–**. **Kung mabaliktad, mobuto.**
  - Kini ang mo-absorb sa kalit nga current spike — mao ni ang mo-pugong sa pag-reset sa ESP32

---

## 6. ARDUINO MEGA

- [ ] **6.1** — `5V LOGIC bus bar → Arduino 5V pin`
- [ ] **6.2** — `GND bus bar → Arduino GND` (gamita ang duha ka GND pin, para sigurado)
- [ ] **6.3** — **Ayaw gamita ang Vin pin. Bakante gyud na.**

### USB RULE — basaha ni

Kung mag-upload og bag-ong firmware sa Arduino:

1. OFF ang toggle switch
2. **Tanggala ang 5V wire sa Arduino 5V pin**
3. Plug ang USB → upload
4. Tanggala ang USB
5. Ibalik ang 5V wire
6. ON ang toggle switch

**Normal ni:** mo-puti ang LCD kung naka-USB ang Arduino. Dili na guba. Mo-balik ra inig main power.

---

## 7. ESP32 (Data Bridge)

Ang ESP32 naa nay kaugalingong kuryente karon. **Dili na gikan sa Arduino.**

### Power
- [ ] **7.1** — `5V LOGIC bus bar → ESP32 VIN`
- [ ] **7.2** — `GND bus bar → ESP32 GND`

### Data: Arduino → ESP32 (Voltage Divider)

Ang Arduino 5V ang output, ang ESP32 3.3V ra ang kaya. Kinahanglan og divider.

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

- [ ] **7.3** — Gikan sa `Arduino Pin 1 (TX0)`, i-solder ang **1K resistor**
- [ ] **7.4** — Sa pikas tumoy sa resistor, himoa nga junction (usa ka tulbok nga sumpayan)
- [ ] **7.5** — Gikan sa junction, i-solder ang **duha ka 1K nga sunod-sunod** (series), unya sa **GND bus bar**
- [ ] **7.6** — Gikan sa junction, wire → `ESP32 GPIO26`
- [ ] **7.7** — Sukda ang junction inig ON (Arduino nagdagan): dapat mga **3V**

> **AYAW gamita ang logic level shifter dinhi.** Duha na ka board ang gi-test, pareho dili mogana sa UART. Ang divider ra ang mogana.

### Commands: ESP32 → Arduino (GPIO pulse)

- [ ] **7.8** — `ESP32 GPIO12 (D12) → Arduino Pin 17` (Feed command) — direktang wire
- [ ] **7.9** — `ESP32 GPIO13 (D13) → Arduino Pin 3` (SMS command) — direktang wire
- [ ] **7.10** — Pull-down: `Arduino Pin 17 → 10K resistor → GND bus bar`
- [ ] **7.11** — Pull-down: `Arduino Pin 3 → 10K resistor → GND bus bar`

### ⚠️ Commands: ESP32 → Arduino (Serial — AYAW NI KALIMTI)

Gawas sa duha ka GPIO pulse sa taas, naa pay **usa ka serial wire** nga kinahanglan.
Dinhi moagi ang **phone number, feeding schedules, ug ang mga reply** gikan sa app.

| GIKAN | PADULONG |
|-------|----------|
| **ESP32 GPIO22** | Arduino **Pin 19 (RX1)** |

- [ ] **7.12** — `ESP32 GPIO22 → Arduino Pin 19` — direktang wire, **walay resistor, walay divider**

> **Ngano walay divider diri?** 3.3V ang gikan sa ESP32 padulong sa 5V nga Arduino input — okay ra na,
> dili makadaot. Ang divider kinahanglan ra kung 5V padulong sa 3.3V (sama sa Pin 1 → GPIO26).
>
> **Kung makalimtan ni nga wire:** mogana gihapon ang live data ug ang Feed Now,
> pero **dili mo-save ang schedules ug ang phone number** gikan sa app.

> **10K gyud, dili 1K.** Ang 1K sobra ka kusog, mo-birada sa signal paubos ug dili mo-trigger.
>
> **Ayaw gamita ang D2 ug D5 sa ESP32** — luya na ang output. **Ayaw gamita ang Arduino Pin 2** — patay.

---

## 8. TFT LCD (ST7796S — Direct SPI)

| Wire # | GIKAN | PADULONG |
|--------|-------|----------|
| 1 | **5V LOGIC bus bar** | LCD **VCC** |
| 2 | **GND bus bar** | LCD **GND** |
| 3 | Arduino **Pin 52** | LCD **SCK** |
| 4 | Arduino **Pin 51** | LCD **SDI (MOSI)** |
| 5 | Arduino **Pin 40** | LCD **CS** |
| 6 | Arduino **Pin 38** | LCD **DC** |
| 7 | Arduino **3.3V** | LCD **LED** |
| 8 | Arduino **3.3V** | LCD **RST** |

- [ ] **8.1** — Sumpaya ang 8 ka wire sa taas
- [ ] **8.2** — Ang LCD naa sa **lid (gawas)** sa enclosure — gamit og taas nga wire, pero **max 20cm** para sa SPI

> **Ayaw** i-sumpay ang LED ug RST sa 5V. **3.3V gyud.**
> **Ayaw** i-power ang LCD gikan sa Arduino 5V pin. Bus bar gyud.

---

## 9. SENSORS

### DO Sensor (DFRobot SEN0237)

| GIKAN | PADULONG |
|-------|----------|
| **5V LOGIC bus bar** | DO board **VCC** (pula) |
| **GND bus bar** | DO board **GND** (itom) |
| DO board **Signal** (asul) | Arduino **A3** |

- [ ] **9.1** — **A3 gyud, dili A1.** Patay ang A1, 0 gyud ang basa.

### Voltage Sensor

| GIKAN | PADULONG |
|-------|----------|
| Voltage sensor **signal (S)** | Arduino **A2** |
| Voltage sensor **GND (–)** | **GND bus bar** |
| Voltage sensor **input terminal** | Battery + ug – (para masukod ang battery) |

- [ ] **9.2** — **A2 gyud, dili A0.** Patay ang A0.

### RTC DS3231

| GIKAN | PADULONG |
|-------|----------|
| **5V LOGIC bus bar** | RTC **VCC** |
| **GND bus bar** | RTC **GND** |
| Arduino **Pin 20 (SDA)** | RTC **SDA** |
| Arduino **Pin 21 (SCL)** | RTC **SCL** |

- [ ] **9.3** — Sumpaya
- [ ] **9.4** — **Susiha ang coin cell (CR2032).** Sukda: dapat **2.8V pataas**. Kung ubos, ilisi. Kung uga, mo-00 gihapon ang oras kada patay sa kuryente.

### HX711 + Load Cell — ⛔ AYAW NA I-WIRE (WALA NA GIGAMIT)

> **Laktawi ni nga step.** Ang load cell **gi-ilisan na sa ultrasonic sensor** para sa feed level.
> Sa firmware, naka-comment out na ang tanang HX711 code (`// #include <HX711.h>`).
> Kung i-wire nimo ni, **walay mahitabo** — walay mobasa niini.
>
> Ipadaplin lang ang HX711 ug ang load cell. Ayaw i-sumpay sa Arduino.

- [ ] **9.5** — Siguroha nga **WALAY** wire sa Arduino Pin 10 ug Pin 11. Dapat bakante.

### Ultrasonic Sensor (HC-SR04) — kini na ang feed level sensor

> **USA ra ka sensor ang gigamit sa firmware**, dili duha. Kini ang mipuli sa load cell
> para sa pagsukod sa feed level sa hopper.

| GIKAN | PADULONG |
|-------|----------|
| **5V LOGIC bus bar** | HC-SR04 **VCC** |
| **GND bus bar** | HC-SR04 **GND** |
| Arduino **Pin 13** | HC-SR04 **TRIG** |
| Arduino **Pin 48** | HC-SR04 **ECHO** |

- [ ] **9.6** — Sumpaya ang 4 ka wire. **Pin 13 (TRIG) ug Pin 48 (ECHO)** — mao ni ang naa sa firmware.
- [ ] **9.7** — Ibutang ang sensor sa **taas nga bahin sa hopper, nag-atubang paubos**, patag ug walay babag
- [ ] Dili kinahanglan og voltage divider sa ECHO — 5V pod ang Arduino Mega, okay ra
- [ ] **NOTE:** normal nga mo-kislap ang onboard LED (L) sa Arduino — ang Pin 13 naka-sumpay ana. Dili na guba.

> **Kung naa kay ikaduha nga ultrasonic:** ipadaplin lang. Usa ra ang gisuportahan sa firmware karon.

---

## 10. SIM800L V2 (Asul nga board)

> Kini ang pinakagutom sa kuryente. Mo-abot og 2A inig text. Mao nga **POWER rail** siya, dili LOGIC.

| GIKAN | PADULONG |
|-------|----------|
| **5V POWER bus bar** | SIM800L **VCC** |
| **GND bus bar** | SIM800L **GND** |
| SIM800L **TXD** | Arduino **Pin 15 (RX3)** |
| SIM800L **RXD** | Arduino **Pin 14 (TX3)** |

- [ ] **10.1** — Sumpaya ang 4 ka wire
- [ ] **10.2** — **I-solder ang 1000uF capacitor** tapat sa VCC ug GND **sa SIM800L board mismo** (dili sa bus bar)
  - Taas nga paa = **+** → VCC
  - Mubo nga paa (naay puti nga guhit sa kilid) = **–** → GND
  - **Kung mabaliktad, mobuto ang capacitor. Susiha kaduha.**
- [ ] **10.3** — Isulod ang SIM card. **Luag ang slot** — butangi og gamay nga papel sa likod sa SIM para hugot ang contact.
- [ ] **10.4** — I-sumpay ang antenna. Ayaw ON kung walay antenna.

> Ang SIM800L V2 (asul) naay onboard regulator — 5V diretso, okay ra. Walay diode nga kinahanglan.

---

## 11. MOTORS UG FAN

### L298N Motor Driver + DC Motor

| GIKAN | PADULONG |
|-------|----------|
| **Fusebox Slot 3 (12V)** | L298N **12V** |
| **GND bus bar** | L298N **GND** |
| Arduino **Pin 4** | L298N **IN1** |
| Arduino **Pin 5** | L298N **IN2** |
| Arduino **Pin 12** | L298N **ENA** |
| L298N **OUT1 / OUT2** | DC Motor (duha ka wire) |

- [ ] **11.1** — Sumpaya
- [ ] **11.2** — **TANGGALA ang jumper sa L298N 5V-enable** (ang gamay nga asul nga jumper tapad sa 12V terminal)
  - Ngano: dili nato gamiton ang onboard 5V regulator niya
  - **Ayaw gyud i-sumpay ang L298N 5V pin sa Arduino**

### Servo Motor

| GIKAN | PADULONG |
|-------|----------|
| **5V POWER bus bar** | Servo **pula (VCC)** |
| **GND bus bar** | Servo **brown/itom (GND)** |
| Arduino **Pin 6** | Servo **orange/dalag (signal)** |

- [ ] **11.3** — **POWER rail gyud, dili LOGIC** — mo-reset ang ESP32 kung parehas silag rail

### Fan

- [ ] **11.4** — Kung **12V nga fan**: `Fusebox Slot 4 (+) → Fan +`, `GND bus bar → Fan –`
- [ ] **11.5** — Kung **5V nga fan**: `5V POWER bus bar → Fan +`, `GND bus bar → Fan –`
- [ ] Ang fan mo-huyop **pagawas** sa enclosure (exhaust). Susiha ang arrow sa kilid sa fan.

### Buzzer

| GIKAN | PADULONG |
|-------|----------|
| Arduino **Pin 7** | Buzzer **+** |
| **GND bus bar** | Buzzer **–** |

- [ ] **11.6** — Ang buzzer naa sa **lid (gawas)** sa enclosure
- [ ] Gamita ang **5V active buzzer** (gi-ilisan na ang daan nga 12V panel buzzer)

---

## 12. ESP32-CAM

Standalone ni. Walay data wire sa Arduino — WiFi ra.

| GIKAN | PADULONG |
|-------|----------|
| **5V POWER bus bar** | ESP32-CAM **5V** |
| **GND bus bar** | ESP32-CAM **GND** |

- [ ] **12.1** — Duha ra ka wire. Human.
- [ ] **12.2** — **Ayaw i-sumpay ang IO0 sa GND** — para ra na sa pag-upload og firmware

---

## 13. KOMPLETO NGA PIN TABLE (Arduino Mega)

I-print ni ug ibutang tapad sa Arduino.

| Arduino Pin | Padulong |
|-------------|----------|
| **5V** | 5V LOGIC bus bar |
| **GND** (x2) | GND bus bar |
| **3.3V** | LCD LED + LCD RST |
| **Pin 1 (TX0)** | Voltage divider → ESP32 GPIO26 |
| **Pin 3** | ESP32 D13 (SMS cmd) + 10K pull-down |
| **Pin 4** | L298N IN1 |
| **Pin 5** | L298N IN2 |
| **Pin 6** | Servo signal |
| **Pin 7** | Buzzer + |
| **Pin 10** | *(bakante — HX711 wala na gigamit)* |
| **Pin 11** | *(bakante — HX711 wala na gigamit)* |
| **Pin 13** | Ultrasonic TRIG |
| **Pin 12** | L298N ENA |
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

### PATAY NGA PIN — AYAW GYUD GAMITA

| Pin | Kahimtang |
|-----|-----------|
| **A0** | Internal leakage — sayop ang basa |
| **A1** | Patay — 0 gyud ang basa |
| **Pin 8** | Shorted sa ground |
| **Pin 9** | Shorted sa ground |
| **Pin 2** | Patay |
| **Vin** | Ayaw gamita — 5V pin ra ang gamiton |

### ESP32 pin summary

| ESP32 Pin | Padulong |
|-----------|----------|
| **VIN** | 5V LOGIC bus bar |
| **GND** | GND bus bar |
| **GPIO26** | Voltage divider junction (data gikan sa Arduino TX0) |
| **GPIO12 (D12)** | Arduino Pin 17 (Feed) |
| **GPIO13 (D13)** | Arduino Pin 3 (SMS) |
| **GPIO22** | Arduino Pin 19 / RX1 (schedules, phone number) |

**Patay nga ESP32 pin:** D2, D5 (luya na ang output), GPIO4 ug GPIO17 (guba ang UART TX)

---

## 14. POWER-UP TEST — SUNOD-SUNOD

> **AYAW i-ON tanan dungan.** Isa-isa. Kada step, sukda ang rail gamit multimeter.
> Kung mo-ubos sa **4.80V**, **HUNONG** — didto ang problema.

Andama: multimeter probe sa **5V bus bar** ug **GND bus bar**.

| # | Buhaton | Dapat makita | ✓ |
|---|---------|--------------|---|
| **T1** | Tanggala TANANG module sa rails. Converters ra ang naka-sumpay. ON. | **5.10V** duha ka rail | [ ] |
| **T2** | Sumpaya ang **Arduino** ra. ON. | **4.95V pataas.** Mo-sidlak ang Arduino LED. | [ ] |
| **T3** | Dugang ang **ESP32**. ON. | **4.90V pataas.** Ang ESP32 LED **steady** — dili nagkurap-kurap. | [ ] |
| **T4** | Dugang ang **LCD**. ON. | **4.85V pataas.** Mo-display ang LCD. | [ ] |
| **T5** | Dugang ang **RTC, DO, Ultrasonic, Voltage sensor**. ON. | **4.85V pataas** | [ ] |
| **T6** | Dugang ang **SIM800L**. ON. | POWER rail **4.80V pataas.** Mo-blink ang network LED (hinay nga blink = naka-network na) | [ ] |
| **T7** | Dugang ang **Servo + L298N**. ON. Test og lihok. | Mo-ubos gamay inig lihok, pero **dili mo-reset ang ESP32** | [ ] |
| **T8** | Dugang ang **ESP32-CAM + Fan**. ON. | Tanan stable, walay nagkurap | [ ] |

**Kung naay step nga napakyas:** balik sa miaging step, tanggala ang bag-ong gi-sumpay, ug susiha ang wire ana. Ayaw og padayon hangtod matul-id.

---

## 15. FUNCTION TEST (human sa power test)

- [ ] **F1** — Mo-ON ang LCD ug naay display (dili puti, dili itom)
- [ ] **F2** — Steady ang ESP32 LED
- [ ] **F3** — Naay basa ang Voltage sensor sa LCD (dapat mga 12–13V)
- [ ] **F4** — Naay oras ang RTC sa LCD (dili 00:00)
- [ ] **F5** — Naay basa ang feed level (gikan sa ultrasonic, dili load cell)
- [ ] **F6** — Ma-connect ang app sa BLE ("OxyFeeder")
- [ ] **F7** — Mo-abot ang data sa app (DO, feed, battery)
- [ ] **F8** — "Feed Now" sa app → mo-lihok ang servo
- [ ] **F9** — "Test SMS" sa app → makadawat og text
- [ ] **F10** — Makita ang camera stream sa app

---

## 16. TROUBLESHOOTING

| Sintomas | Hinungdan | Buhaton |
|----------|-----------|---------|
| Nagkurap ang ESP32 LED | Kulang ang kuryente, o luag ang GND | Sukda ang 5V LOGIC samtang nagkurap. Susiha ang GND bus bar — hugot ba tanan? |
| Puti ang LCD | Naka-USB ang Arduino | Normal. Tanggala ang USB, gamit main power ra. |
| Itom ang LCD, walay gikan | Kulang 5V o luag ang SPI wire | Sukda ang VCC **sa LCD mismo**, dili sa bus bar. Susiha ang 6 ka SPI wire. |
| Ubos ang boltahe sa rail | Gagmay ra ang wire | Ilisi og 18AWG gikan sa converter ngadto sa bus bar |
| **DO = 0.00** | Naka-wire sa **A1** imbes **A3** | **Susiha ni una.** Patay ang A1 — 0 gyud ang basa bisag unsa. Balhina sa **A3**. |
| DO naay basa pero sayop | Nawad-an og electrolyte ang probe | Na-calibrate na ni (zero solution + air). Kung mo-drift, i-recalibrate. |
| **RTC = 00:00** | Patay ang coin cell, o dili makita sa I2C | Sukda ang CR2032 (2.8V pataas). I-run ang I2C scanner — dapat makita ang **0x68**. |
| Mo-reset ang ESP32 inig lihok sa servo | Parehas silag rail | Ibalhin ang servo sa **POWER rail** |
| Walay data sa app | Nabuak ang voltage divider, o dili common ang GND | Sukda ang junction: dapat **mga 3V** samtang nag-send ang Arduino |
| Dili mo-gana ang Feed Now | Luag ang pull-down o sayop ang pin | Susiha: 10K sa Pin 17 → GND. Susiha ang wire gikan sa ESP32 D12. |
| Walay SMS | Walay SIM, luag ang SIM, o kulang kuryente | Butangi og papel sa likod sa SIM. Susiha ang 1000uF cap. |
| Init kaayo ang converter | Sobra ang load | Bahina ang load. Susiha kung tama ang rail sa kada module. |
| Mo-reset ang Arduino kada karon ug unya | Luag nga 5V wire o GND | Hugti tanan. Sukda ang 5V direkta sa Arduino pin. |

---

## 17. FINAL CHECK — HUMAN NA

- [ ] Tanan nga wire hugot, walay luag
- [ ] Tanan nga GND naa sa GND bus bar
- [ ] Walay wire nga naghikap sa motor o sa init nga bahin
- [ ] Naka-label ang tanang bus bar
- [ ] Naka-litrato ang wiring (para reference kung maguba)
- [ ] Ang **LCD, toggle switch, ug buzzer** naa sa **lid (gawas)**
- [ ] Ang tanan nga uban naa sa **sulod** sa enclosure
- [ ] Ang cable gland hugot, walay tubig nga makasulod

---

## 18. PARA SA MAG-FIRMWARE (dili wiring, pero ayaw kalimti)

1. **`dispenseFeed()` kinahanglan BLOCKING** (delay-based). Kini gikan sa daan nga HX711 issue (mo-disable siya sa interrupts ug mo-guba sa servo PWM). Bisag wala na ang HX711, ang blocking nga code mao gihapon ang naa sa firmware — ayaw usba kung dili pa ma-test.
2. Ang Pin 17 ug Pin 3 kinahanglan **`INPUT`**, dili `INPUT_PULLUP` — mag-away sa pull-down resistor.
3. Serial baud: **9600** (Arduino → ESP32)
4. ESP32 Serial2: **RX = GPIO26**, walay TX
5. 15-second startup delay sa wala pa mo-basa ang GPIO commands (para dili mag-false trigger)
6. DO calibration: naa nay debug print (`[DO CAL] Raw ADC: X | Voltage: X.XXXX V`) — andam na inig abot sa electrolyte
7. Ang **load cell/HX711 naka-comment out na** sa firmware — ang ultrasonic (Pin 13/48) na ang feed level sensor
8. Ang hopper calibration naa sa `HOPPER_EMPTY_CM = 37` ug `HOPPER_FULL_CM = 5` — usba ni sumala sa aktuwal nga hopper
9. Ang ESP32 mo-send og commands sa Arduino **Pin 19 (RX1)** gikan sa **GPIO22** — dili lang GPIO pulse
