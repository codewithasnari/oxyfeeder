# OxyFeeder — Kompletong Wiring Guide (Rev 4)

**Para kang kinsa:** ang mag-wire sa system
**Petsa:** Sept 26, 2026 · **Rev 4**
**Kahimtang:** bag-ong build — bulag na ang tanang module
**Gipulihan ang:** [`WIRING_GUIDE_BISAYA.md`](WIRING_GUIDE_BISAYA.md) — gitipigan para sa history, pero **kini na ang tama**
**English nga bersyon:** [`docs/wiring-guide-rev4.md`](../docs/wiring-guide-rev4.md) — parehas ang numero sa section
**Blueprint (drawing):** https://claude.ai/artifact/DF9rpHHmpvLzvZ8Vr99HAr

---

> ## UNSAY NAUSAB GIKAN SA REV 3 — basaha ni bisag nabasa na nimo ang Rev 3
>
> Tulo ka butang ang nadiskobrehan sa unang pagsulay og build. Sayop ang Rev 3 sa tanan niini.
>
> | # | Ang Rev 3 nag-ingon | Ang Rev 4 nag-ingon | Ngano |
> |---|---------------------|---------------------|-------|
> | 1 | Servo sa **5V POWER** rail | Servo naa nay **kaugalingong 7.4V** rail | Ang DS S020B **6.0–7.4V DC** ang spec. Dili gyud mogana sa 5.1V. |
> | 2 | Buzzer 2 ka pin | Buzzer **3 ka pin nga module** (VCC · GND · I/O) | Giilisan na ang 12V buzzer og 5V active module |
> | 3 | Fusebox: 5A, 3A, 5A, 2A | **3A, 3A, 5A, 2A, 5A** | Nausab ang tanang load tungod sa bag-ong servo rail |
>
> **Tulo na ka rail karon**, dili duha. Ug **tulo na ka buck converter**, dili duha.

---

## 0. BASAHA UNA — ANG 6 KA BAWAL

Kung usa ra ani ang malapas, maguba ang board. Naa nay 4 ka patay nga pin tungod sa bawal #1.

| # | BAWAL | NGANO |
|---|-------|-------|
| 1 | **AYAW** i-sumpay ang 12V direkta sa 5V pin sa Arduino | Mao ni ang nakapatay sa A0, A1, Pin 8, Pin 9 |
| 2 | **AYAW** gamita ang **A0, A1, Pin 8, Pin 9, Pin 2** | Patay na. Walay basa. Ayaw na sulayi. |
| 3 | **AYAW** i-power ang ESP32 gikan sa Arduino 5V pin | Mao ni ang hinungdan sa brownout |
| 4 | **AYAW** ug plug sa USB sa Arduino samtang naka-ON ang main power | Mag-away ang duha ka power source |
| 5 | **AYAW** i-sumpay ang buck converter output kung wala pa na-set ang boltahe | Kung 12V diay, mamatay tanan |
| 6 | **AYAW** gamita ang logic level shifter para sa data | Duha na ka board ang gi-test, pareho dili mogana |

**Bawal #7, bag-o sa Rev 4:** AYAW i-taas ang 5V POWER rail ngadto sa 7.4V para sa servo. Ang SIM800L
ug ESP32-CAM naa ana nga rail ug 5V ra sila. Kaugalingong converter ang servo.

---

## 1. LISTAHAN SA GAMIT

### Components
- [ ] Arduino Mega 2560
- [ ] ESP32 Dev Module
- [ ] ESP32-CAM
- [ ] TFT LCD 4.0" ST7796S (480×320)
- [ ] DO Sensor (DFRobot SEN0237)
- [ ] Ultrasonic Sensor (HC-SR04) — **usa ra ang gamiton**
- [ ] RTC Module DS3231 **+ CR2032 coin cell** (permi walay sulod ang clip inig palit)
- [ ] Voltage Sensor (25V module)
- [ ] SIM800L V2 (asul nga board)
- [ ] **Servo — DS S020B, 20kg, 6.0–7.4V** (tan-awa ang §11)
- [ ] DC Motor
- [ ] L298N Motor Driver
- [ ] Fan
- [ ] **Buzzer — 3 ka pin nga active module** (VCC · GND · I/O)
- [ ] Solar Panel
- [ ] Solar Charge Controller — **gamita ang MPPT.** Spare ra ang PWM.
- [ ] Battery 12V
- [ ] Toggle Switch
- [ ] 6-way blade Fusebox
- [ ] Buck Converter **5A ×1** — para sa servo rail
- [ ] Buck Converter **3A ×2** — para sa LOGIC ug POWER rail

> ⛔ **Load Cell + HX711 — WALA NA GIGAMIT.** Gipulihan sa ultrasonic. Naka-comment out na ang tanang
> HX711 code sa firmware. Kung i-wire nimo, walay mahitabo. Ibutang sa laing kahon.

### Kinahanglan andamon
- [ ] **Terminal block / bus bar — ×4** (GND, 5V LOGIC, 5V POWER, 7.4V SERVO)
- [ ] Resistor **1K — ×3** (voltage divider, §7.3–7.5)
- [ ] Resistor **10K — ×2** (pull-down, §7.10–7.11)
- [ ] Capacitor **1000uF 16V — ×1** (para sa SIM800L)
- [ ] Wire: **18AWG** (power), **22AWG** (signal)
- [ ] Heat shrink / electrical tape
- [ ] **Multimeter** — KINAHANGLAN GYUD. Ayaw sugdi kung wala.
- [ ] Label tape / marker

### Fuse (blade type)

| Amps | Kolor | Pila |
|------|-------|------|
| **5A** | tan | 2 |
| **3A** | violet | 2 |
| **2A** | grey | 1 |

> Naay kolor ang blade fuse. Kung makakita kag **pula**, 10A na — sayop sa tanang slot dinhi.

### 5 ra ka resistor ang tibuok build
3× 1K para sa divider, 2× 10K para sa pull-down. Ang uban tanan direktang wire.
Mahimo pod nga 10K ang divider (ang ratio ang importante, dili ang value) — pero ayaw pagsagol og
value **sulod** sa divider, ug 10K gyud ang pull-down.

---

## 2. SIGUROHA NGA BULAG NA ANG TANAN

- [ ] **2.1** — OFF ang toggle switch
- [ ] **2.2** — Bulag ang battery sa charge controller
- [ ] **2.3** — Bulag ang solar panel sa charge controller
- [ ] **2.4** — Sukda ang fusebox. Dapat **0V**.
- [ ] **2.5** — Walay module nga naay nahibilin nga wire
- [ ] **2.6** — I-lain ang tanang module, dili nagsapaw
- [ ] **2.7** — Ibutang ang HX711 ug load cell sa laing kahon

---

## 3. HIMOA ANG BUS BARS (RAIL)

**Upat** na ka terminal block karon.

- [ ] **3.1** — Ibutang ang 4 ka terminal block sa enclosure, layo sa motor ug sa init
- [ ] **3.2** — Labeli gamit tape ug marker:
  - Block 1 = **GND** (itom nga tape)
  - Block 2 = **5V LOGIC** (pula nga tape)
  - Block 3 = **5V POWER** (dalag nga tape)
  - Block 4 = **7.4V SERVO** (purple nga tape)
- [ ] **3.3** — Kuhaa og litrato ang labels

### Ang mga rail

| Rail | Boltahe | Converter | Kinsa naka-sumpay |
|------|---------|-----------|-------------------|
| **5V LOGIC** | 5.10V | Buck **3A** | Arduino, ESP32, TFT LCD, RTC, DO, Ultrasonic, Buzzer |
| **5V POWER** | 5.10V | Buck **3A** | SIM800L, ESP32-CAM |
| **7.4V SERVO** | 7.4V | Buck **5A** | Servo ra |
| **GND** | — | — | TANAN. Walay exception. |

### Ngano bulag-bulag

- **LOGIC vs POWER:** mo-hulbot og ~2A ang SIM800L inig text. Mo-ubos ang boltahe sa maong wire. Kung
  parehas silag wire sa ESP32, mo-reset ang ESP32 ug mawala ang Bluetooth.
- **SERVO bulag gyud:** 2–3A ang servo inig stall, ug 7.4V ang gikinahanglan niya — maguba ang mga 5V
  nga module ana. Bulag nga boltahe, bulag nga converter.

> ### ⚠️ ANG PINAKAIMPORTANTE NGA BALAOD
> Ang TANAN nga GND kinahanglan mosumpay sa GND bus bar. Kung usa ra ang wala, mag-random ang sensor
> readings ug dili mo-abot ang data sa app. **Walay resistor nga isulod sa ground wire.**

---

## 4. POWER CHAIN — Solar → Fusebox

```
Solar Panel
   │
   ▼
Charge Controller (MPPT)   ← ang BATTERY i-sumpay UNA, dili ang panel
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
  - **Ang battery UNA gyud, dili ang panel** — kinahanglan mabasa sa MPPT ang battery voltage
- [ ] **4.2** — Charge Controller **PV terminal** → Solar Panel
  - `PV+ → Panel +` · `PV- → Panel -`
  - Taboni ang panel og tela samtang nag-wire
- [ ] **4.3** — Charge Controller **LOAD+** → Toggle Switch (tunga / COM terminal)
- [ ] **4.4** — Toggle Switch pikas terminal → **Fusebox positive input**
- [ ] **4.5** — Charge Controller **LOAD-** → **GND bus bar**
- [ ] **4.6** — **Fusebox ground** → **GND bus bar**

### Fuse assignments — NAUSAB SA REV 4

| Slot | Fuse | Kolor | Padulong asa | Tinuod nga load sa 12V |
|------|------|-------|--------------|------------------------|
| **1** | **3A** | violet | Buck 3A → **5V LOGIC** | ~0.3A |
| **2** | **3A** | violet | Buck 3A → **5V POWER** | ~1.1A |
| **3** | **5A** | tan | **L298N** motor driver 12V | ~2–3A |
| **4** | **2A** | grey | **Fan** 12V | ~0.3A |
| **5** | **5A** | tan | Buck 5A → **7.4V SERVO** | ~2A inig stall |
| **6** | — | — | spare — bakante | — |

- [ ] **4.7** — Isulod ang 5 ka fuse sa tama nga slot

> **Ngano 5A ang slot 5, dili 2A:** ang 20kg servo mo-hulbot og ~3A sa 7.4V inig stall = ~2A sa 12V
> nga side. Mabugto ang 2A nga fuse taliwala sa feeding.
>
> **Walay kaugalingong slot ang voltage sensor.** 0.3 mA ra siya — i-sumpay ang measuring input sa
> fusebox positive input bar (tan-awa ang §9).

---

## 5. I-SET ANG BUCK CONVERTERS

> ### 🔴 HUNONG — pinaka-delikado nga step
> Ang buck converter usahay 12V ang output pag-abot gikan sa tindahan. Kung diretso nimo i-sumpay sa
> rail, mamatay ang Arduino, ESP32, LCD ug tanang sensor. **I-set ang boltahe samtang bakante ang
> output.**

**Boltahe ra ang ma-adjust sa imong converter.** Ang "3A" ug "5A" kay rating sa module, dili setting.
Ilhi pinaagi sa chip: **LM2596 ≈ 3A** (gamay), **XL4015 ≈ 5A** (dako, naay heatsink).

### Converter A — 3A → LOGIC rail
- [ ] **5.1** — **INPUT ra**: `Fusebox Slot 1 (+) → IN+` · `GND bus bar → IN-`
- [ ] **5.2** — **Ayaw i-sumpay ang OUTPUT bisag asa.**
- [ ] **5.3** — ON ang toggle switch
- [ ] **5.4** — Multimeter probe sa **OUT+** ug **OUT-**
- [ ] **5.5** — Tuyuka ang gamay nga asul nga potentiometer. Hinay-hinay, daghan tuyok.
- [ ] **5.6** — Hunonga sa **5.10V** (5.05–5.15V okay ra)
- [ ] **5.7** — OFF, ON balik. Sukda usab — **dapat 5.10V gihapon.** Kung nausab, ilisi ang converter.

### Converter B — 3A → POWER rail
- [ ] **5.8** — Input ra: `Fusebox Slot 2 (+) → IN+` · `GND bus bar → IN-`
- [ ] **5.9** — Balika ang 5.3–5.7. Target **5.10V**

### Converter C — 5A → SERVO rail
- [ ] **5.10** — Input ra: `Fusebox Slot 5 (+) → IN+` · `GND bus bar → IN-`
- [ ] **5.11** — Balika ang 5.3–5.7. Target **7.4V**
  - *Ang 6.0V sakto pod ug mas hinay sa servo (22 kg·cm imbes 28 kg·cm). Pareho mogana — dili
    kinahanglan sa feed gate ang kusog kaayo nga torque.*
- [ ] **5.12** — OFF ang toggle switch

### Karon pa i-sumpay ang output
- [ ] **5.13** — `Converter A OUT+ → 5V LOGIC bar` · `OUT- → GND bar` (18AWG)
- [ ] **5.14** — `Converter B OUT+ → 5V POWER bar` · `OUT- → GND bar` (18AWG)
- [ ] **5.15** — `Converter C OUT+ → 7.4V SERVO bar` · `OUT- → GND bar` (18AWG)

> **Susiha kaduha una ani:** LOGIC ug POWER **5.10V**, SERVO **7.4V**. Kung mabaylo ang servo converter
> ngadto sa 5V rail, 7.4V ang mosulod sa ESP32 ug SIM800L.

---

## 6. ARDUINO MEGA

- [ ] **6.1** — `5V LOGIC bus bar → Arduino 5V pin`
- [ ] **6.2** — `GND bus bar → Arduino GND` (gamita ang duha ka GND pin)
- [ ] **6.3** — **Ayaw gamita ang Vin pin. Bakante gyud na.**

### USB RULE
Inig upload og firmware: **1.** OFF ang toggle → **2.** tanggala ang 5V wire sa Arduino 5V pin →
**3.** plug USB, upload → **4.** tanggala ang USB → **5.** ibalik ang 5V → **6.** ON ang toggle.

**Normal ni:** mo-puti ang LCD kung naka-USB ang Arduino. Dili guba — mo-balik ra inig main power.

---

## 7. ESP32 — DATA BRIDGE

### Power
- [ ] **7.1** — `5V LOGIC bus bar → ESP32 VIN`
- [ ] **7.2** — `GND bus bar → ESP32 GND` — **direktang wire, WALAY resistor**

### Data: Arduino → ESP32 (voltage divider)

```
Arduino Pin 1 (TX0)   5V
        │
      [1K]
        │
        ├──────────────▶ ESP32 GPIO26   ≈3.3V   ← ang JUNCTION ang sumpayan
        │
      [1K]
        │
      [1K]
        │
    GND bus bar
```

- [ ] **7.3** — Gikan sa `Arduino Pin 1 (TX0)`, i-solder ang usa ka **1K resistor**
- [ ] **7.4** — Sa pikas tumoy, himoa nga **junction** (usa ka tulbok nga sumpayan)
- [ ] **7.5** — Gikan sa junction, i-solder ang **duha ka 1K nga sunod-sunod**, unya sa **GND bus bar**
- [ ] **7.6** — Gikan sa junction, wire → `ESP32 GPIO26`
- [ ] **7.7** — ON ang power, nagdagan ang Arduino, sukda ang junction: dapat mga **3.3V**

> 🔴 **Ang junction ang sumpayan, dili ang Arduino pin.** Kung ang duha ka resistor padulong sa ground
> naa sa Pin 1 imbes sa junction, hapit **5V** ang makita sa GPIO26 ug maguba ang ESP32. Kung 5V ang
> imong nasukod, hunong ug himoa pag-usab ang divider.

### Commands: ESP32 → Arduino (GPIO pulse)

```
ESP32 GPIO12:   ________┌──────────┐________
                  LOW   │   HIGH   │   LOW
                        │← 500ms  →│
```

- [ ] **7.8** — `ESP32 GPIO12 (D12) → Arduino Pin 17` (Feed) — direktang wire
- [ ] **7.9** — `ESP32 GPIO13 (D13) → Arduino Pin 3` (SMS) — direktang wire
- [ ] **7.10** — Pull-down: `Arduino Pin 17 → 10K → GND bus bar`
- [ ] **7.11** — Pull-down: `Arduino Pin 3 → 10K → GND bus bar`

Ang pull-down **mosanga** gikan sa signal wire — dili siya sulod sa wire:

```
❌ SAYOP — mahuyang ang signal:
   GPIO12 ●───[10K]───● Pin 17

✅ TAMA — mosanga padulong sa ground:
   GPIO12 ●─────┬─────● Pin 17
                │
              [10K]
                │
            GND bus bar
```

Kung wala ni, mo-**float** ang pin — mahimong antenna, mo-dawat og noise gikan sa motor ug kuryente,
ug mo-basa og random nga HIGH. Sangputanan: mo-feed og random alas-3 sa kaadlawon.

> **10K gyud, dili 1K.** 3.0V ang kinahanglan sa Mega para HIGH, ug 3.3V ra ang gikan sa ESP32 — 0.3V
> ra ang lugar. Ang 10K mo-bilin og ~3.27V; ang 1K mo-birada paubos sa ~3.14V ug dili mo-trigger.
>
> **Ayaw gamita ang ESP32 D2 ug D5** (luya). **Ayaw gamita ang Arduino Pin 2** (patay).

### ⚠️ Serial: ESP32 → Arduino — ANG WIRE NGA PERMI MAKALIMTAN

| GIKAN | PADULONG |
|-------|----------|
| **ESP32 GPIO22 (D22)** | Arduino **Pin 19 (RX1)** |

- [ ] **7.12** — `ESP32 GPIO22 → Arduino Pin 19` — direktang wire, **walay resistor, walay divider**

> **Ngano walay divider:** 3.3V ni padulong sa 5V nga input — okay ra. Ang divider kinahanglan ra kung
> 5V padulong sa 3.3V (Pin 1 → GPIO26).
>
> **Kung makalimtan ni:** mogana gihapon ang live data ug ang Feed Now, pero **dili mo-save ang
> schedules ug phone number.** Usahay `SCL` ang nakasulat sa board imbes `D22` — parehas ra na.

---

## 8. TFT LCD — ST7796S, DIRECT SPI

| # | GIKAN | PADULONG |
|---|-------|----------|
| 1 | **5V LOGIC bus bar** | LCD **VCC** |
| 2 | **GND bus bar** | LCD **GND** |
| 3 | Arduino **Pin 52** | LCD **SCK** |
| 4 | Arduino **Pin 51** | LCD **SDI (MOSI)** |
| 5 | Arduino **Pin 40** | LCD **CS** |
| 6 | Arduino **Pin 38** | LCD **DC / RS** |
| 7 | Arduino **3.3V** | LCD **LED** |
| 8 | Arduino **3.3V** | LCD **RST** |

- [ ] **8.1** — Sumpaya ang 8 ka wire
- [ ] **8.2** — Ang LCD naa sa **lid (gawas)** — mahimong taas nga wire, pero **max 20cm** para sa SPI

> 🔴 **DC = Pin 38, SCK = Pin 52.** Kini ang pinakasayon nga mabaylo sa tibuok build. Kung itom ang
> display, susiha ni una.
>
> **Ayaw** i-sumpay ang LED ug RST sa 5V — **3.3V gyud.** **Ayaw** i-power ang LCD gikan sa Arduino 5V pin.

---

## 9. SENSORS

### DO Sensor — DFRobot SEN0237

| GIKAN | PADULONG |
|-------|----------|
| **5V LOGIC bus bar** | DO board **VCC** (pula) |
| **GND bus bar** | DO board **GND** (itom) |
| DO board **Signal** (asul) | Arduino **A3** |

- [ ] **9.1** — **A3 gyud, dili A1.** Patay ang A1, 0 gyud ang basa.

### Voltage Sensor — 25V module

Passive ni nga module — duha ka resistor ra, walay chip. Walay kinahanglan nga logic power. Ang screw
terminal nga "VCC" kay **ang boltahe nga sukdon**, dili power input.

```
 ┌──────────────────────────────────┐
 │  SCREW TERMINAL      HEADER PINS │
 │  ┌─────┬─────┐      ┌───┬───┬───┐│
 │  │ VCC │ GND │      │ S │ + │ – ││
 │  └─────┴─────┘      └───┴───┴───┘│
 │   ↑ ang 12V nga          ↑ output │
 │     sukdon                 sa A2  │
 └──────────────────────────────────┘
```

| GIKAN | PADULONG |
|-------|----------|
| Fusebox positive input bar (12V) | Screw terminal **VCC** |
| **GND bus bar** | Screw terminal **GND** |
| Header **S** | Arduino **A2** |
| Header **–** | **GND bus bar** |
| Header **+** | *walay sumpay — bakante* |

- [ ] **9.2** — **A2 gyud, dili A0.** Patay ang A0.
- [ ] **9.3** — **Importante ang duha ka ground.** Ang screw GND mo-kompleto sa divider; ang header –
  mo-hatag og common reference sa Arduino. Kung kulang ang usa, basura ang mabasa sa A2.

> **Sayop ang basa sa sinugdan — normal na.** Ang `VOLTAGE_RATIO = 4.57` sa firmware gi-calibrate sa
> **daan, guba** nga module. Tan-awa ang §18 kung unsaon pag-calibrate. Ayaw ni pangitaag sayop sa wiring.

### RTC DS3231

| GIKAN | PADULONG |
|-------|----------|
| **5V LOGIC bus bar** | RTC **VCC** |
| **GND bus bar** | RTC **GND** |
| Arduino **Pin 20 (SDA)** | RTC **SDA** |
| Arduino **Pin 21 (SCL)** | RTC **SCL** |

- [ ] **9.4** — Sumpaya ang 4 ka wire
- [ ] **9.5** — **Susiha ang CR2032 coin cell** — dapat **2.8V pataas**. Ang coin cell kay ang lapad,
  lingin nga battery sa clip sa likod. Permi **bakante** ang clip sa bag-ong module. Kung wala ni,
  mo-00 ang oras kada patay sa kuryente ug dili mo-gana ang schedules.

### Ultrasonic HC-SR04 — ang feed level sensor

| GIKAN | PADULONG |
|-------|----------|
| **5V LOGIC bus bar** | HC-SR04 **VCC** |
| **GND bus bar** | HC-SR04 **GND** |
| Arduino **Pin 13** | HC-SR04 **TRIG** |
| Arduino **Pin 48** | HC-SR04 **ECHO** |

- [ ] **9.6** — Sumpaya ang 4 ka wire
- [ ] **9.7** — Ibutang sa **taas nga bahin sa hopper, nag-atubang paubos**, patag ug walay babag
- [ ] **9.8** — Siguroha nga **WALAY** wire sa Pin 10 ug Pin 11 (daan nga HX711 pins)

> Dili kinahanglan og divider sa ECHO — 5V pod ang Mega. Normal nga mo-kislap ang onboard **L** LED
> kay naka-sumpay sa Pin 13. Usa ra ka sensor ang gisuportahan sa firmware.

---

## 10. SIM800L V2 — asul nga board

Pinakagutom sa kuryente — mo-abot og **2A** inig text. Mao nga POWER rail siya, dili LOGIC.

| GIKAN | PADULONG |
|-------|----------|
| **5V POWER bus bar** | SIM800L **VCC** |
| **GND bus bar** | SIM800L **GND** |
| SIM800L **TXD** | Arduino **Pin 15 (RX3)** |
| SIM800L **RXD** | Arduino **Pin 14 (TX3)** |

- [ ] **10.1** — Sumpaya ang 4 ka wire
- [ ] **10.2** — **I-solder ang 1000uF capacitor** tapat sa VCC ug GND **sa SIM800L board mismo**
  - Taas nga paa = **+** → VCC
  - Mubo nga paa (naay puti nga guhit) = **–** → GND
  - 🔴 **Kung mabaliktad, mobuto ang capacitor. Susiha kaduha.**
- [ ] **10.3** — Isulod ang SIM card. **Luag ang slot** — butangi og gamay nga papel sa likod
- [ ] **10.4** — I-sumpay ang antenna. **Ayaw ON kung walay antenna.**

> Ang V2 (asul) naay onboard regulator — 5V diretso, okay ra. Walay diode nga kinahanglan.

---

## 11. MOTORS, SERVO, FAN, BUZZER

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
- [ ] **11.2** — **Tanggala ang L298N 5V-enable jumper** (gamay nga asul nga jumper tapad sa 12V
  terminal). **Ayaw gyud i-sumpay ang L298N 5V pin sa Arduino.**

> 🔴 **IN1 = Pin 4, IN2 = Pin 5, ENA = Pin 12.** Walay IN3 sa firmware — ang ikatulo nga wire kay
> **ENA**. Ang daan nga papel nag-ingon Pin 10/11, luma na na. Kung mabalhin og usa ka slot, dili
> mo-lihok ang motor.

### Servo — DS S020B · ⚠️ NAUSAB SA REV 4

**Spec: 20kg · 6.0–7.4V DC · IP67 · 25T spline.** **Dili** ni 5V nga servo. Sa 5.1V, mo-utro-utro,
dili mo-hold sa posisyon, o dili gyud mo-lihok.

| GIKAN | PADULONG |
|-------|----------|
| **7.4V SERVO bus bar** | Servo **PULA** (VCC) |
| **GND bus bar** | Servo **ASUL** (GND) |
| Arduino **Pin 6** | Servo **PURPLE** (signal) |

- [ ] **11.3** — Sumpaya ang 3 ka wire
- [ ] **11.4** — ⚠️ **Susiha ang kolor sa imong aktuwal nga servo una mo-ON.** Sa standard nga 3-pin
  servo connector, ang **tunga nga pin kay VCC permi** ug dapat pula. Kung mabaliktad ang VCC ug
  signal, maguba ang servo.

> **5V gihapon ang signal sa Pin 6** bisag 7.4V ang supply sa servo. Ang servo mo-basa sa gitas-on sa
> pulse, dili sa boltahe. Ang standard nga 500–2500µs gikan sa `Servo.h` mao ray iyang gikinahanglan —
> **walay usbon sa firmware.**
>
> **Torque:** 7.4V → 28 kg·cm · 6.0V → 22 kg·cm. Dili kinahanglan sa feed gate ang tibuok torque.

### Fan
- [ ] **11.5** — **12V nga fan:** `Fusebox Slot 4 (+) → Fan +` · `GND bus bar → Fan –`
- [ ] **11.6** — **5V nga fan:** `5V POWER bus bar → Fan +` · `GND bus bar → Fan –`
- [ ] Ang fan mo-huyop **pagawas** (exhaust). Susiha ang arrow sa kilid.

### Buzzer — 3 ka pin nga module · ⚠️ NAUSAB SA REV 4

Basaha ang nakasulat sa board — lain-lain ang pagkahan-ay sa pin depende sa brand.

| Buzzer pin | PADULONG |
|------------|----------|
| **VCC** (o `+`, `5V`) | **5V LOGIC bus bar** |
| **GND** (o `–`, `G`) | **GND bus bar** |
| **I/O** (o `S`, `SIG`, `IN`) | Arduino **Pin 7** |

- [ ] **11.7** — Sumpaya ang 3 ka wire
- [ ] **11.8** — Ang buzzer naa sa **lid (gawas)**
- [ ] **11.9** — **Siguroha nga ACTIVE nga buzzer.** Sumpaya ang VCC ug GND, unya ihikap ang I/O sa 5V:
  - **Padayon nga beep** → active ✅ mogana ang firmware
  - **Usa ka click unya hilom** → passive ⚠️ kinahanglan usbon ang firmware ngadto sa `tone()`

> Ang firmware mo-gamit og `digitalWrite(HIGH)` — **active** ra nga buzzer ang motingog ana. Kasagaran
> sa 3-pin nga module kay active. ~30mA ra — kaya sa LOGIC rail.

---

## 12. ESP32-CAM

Standalone. Walay data wire sa Arduino — WiFi ra.

| GIKAN | PADULONG |
|-------|----------|
| **5V POWER bus bar** | ESP32-CAM **5V** |
| **GND bus bar** | ESP32-CAM **GND** |

- [ ] **12.1** — Duha ra ka wire. Human.
- [ ] **12.2** — **Ayaw i-sumpay ang IO0 sa GND** — para ra na sa pag-upload og firmware

---

## 13. KOMPLETO NGA PIN TABLE — ARDUINO MEGA

I-print ni ug ibutang tapad sa Arduino.

| Arduino Pin | Padulong | Nota |
|-------------|----------|------|
| **5V** | 5V LOGIC bus bar | dili Vin |
| **GND** ×2 | GND bus bar | gamita ang duha |
| **3.3V** | LCD LED + LCD RST | dili 5V |
| **Pin 1 (TX0)** | divider → ESP32 GPIO26 | 3× 1K |
| **Pin 3** | ESP32 D13 (SMS cmd) | + 10K pull-down |
| **Pin 4** | L298N IN1 | |
| **Pin 5** | L298N IN2 | |
| **Pin 6** | Servo signal (purple) | servo VCC sa 7.4V rail |
| **Pin 7** | Buzzer **I/O** | 3-pin module |
| **Pin 10** | *(bakante — wala na ang HX711)* | |
| **Pin 11** | *(bakante — wala na ang HX711)* | |
| **Pin 12** | L298N ENA | |
| **Pin 13** | Ultrasonic TRIG | mo-kislap ang L LED — normal |
| **Pin 14 (TX3)** | SIM800L RXD | |
| **Pin 15 (RX3)** | SIM800L TXD | |
| **Pin 17** | ESP32 D12 (Feed cmd) | + 10K pull-down |
| **Pin 19 (RX1)** | ESP32 GPIO22 | schedules / phone number |
| **Pin 20 (SDA)** | RTC SDA | |
| **Pin 21 (SCL)** | RTC SCL | |
| **Pin 38** | LCD **DC** | dili Pin 52 |
| **Pin 40** | LCD CS | |
| **Pin 48** | Ultrasonic ECHO | |
| **Pin 51 (MOSI)** | LCD SDI | |
| **Pin 52 (SCK)** | LCD **SCK** | dili Pin 38 |
| **A2** | Voltage sensor S | |
| **A3** | DO sensor signal | |

### PATAY NGA PIN — AYAW GYUD GAMITA

| Pin | Kahimtang |
|-----|-----------|
| **A0** | Internal leakage — sayop ang basa |
| **A1** | Patay — 0 gyud |
| **Pin 8** | Shorted sa ground |
| **Pin 9** | Shorted sa ground |
| **Pin 2** | Patay |
| **Vin** | Ayaw gamita — 5V pin ra |

### ESP32 pin summary

| ESP32 Pin | Padulong |
|-----------|----------|
| **VIN** | 5V LOGIC bus bar |
| **GND** | GND bus bar — **direkta, walay resistor** |
| **GPIO26** | Voltage divider junction |
| **GPIO12 (D12)** | Arduino Pin 17 (Feed) |
| **GPIO13 (D13)** | Arduino Pin 3 (SMS) |
| **GPIO22 (D22)** | Arduino Pin 19 / RX1 (schedules, phone number) |

**Patay nga ESP32 pin:** D2, D5 (luya), GPIO4 ug GPIO17 (guba ang UART TX).

---

## 14. POWER-UP TEST — ISA-ISA

> **AYAW i-ON tanan dungan.** Sukda kada step. Kung mo-ubos sa **4.80V** ang 5V rail, **HUNONG**.

| # | Buhaton | Dapat makita | ✓ |
|---|---------|--------------|---|
| **T1** | Bulag tanang module. Converters ra. ON. | LOGIC **5.10V** · POWER **5.10V** · SERVO **7.4V** | [ ] |
| **T2** | Sumpaya ang **Arduino** ra. ON. | LOGIC **4.95V pataas**. Mo-sidlak ang LED. | [ ] |
| **T3** | Dugang ang **ESP32**. ON. | LOGIC **4.90V pataas**. ESP32 LED **steady**. | [ ] |
| **T4** | Dugang ang **LCD**. ON. | LOGIC **4.85V pataas**. Mo-display ang LCD. | [ ] |
| **T5** | Dugang ang **RTC, DO, Ultrasonic, Voltage sensor, Buzzer**. ON. | LOGIC **4.85V pataas** | [ ] |
| **T6** | Dugang ang **SIM800L**. ON. | POWER **4.80V pataas**. Mo-blink ang network LED. | [ ] |
| **T7** | Dugang ang **Servo**. ON. Pa-lihoka. | SERVO rail **7.0V pataas** samtang naglihok, **dili mo-reset ang ESP32** | [ ] |
| **T8** | Dugang ang **L298N + motor**. ON. Pa-dagana. | Walay rail nga mo-ubos sa 4.80V | [ ] |
| **T9** | Dugang ang **ESP32-CAM + Fan**. ON. | Tanan stable | [ ] |

**Kung napakyas ang usa ka step:** balik sa miaging step, tanggala ang bag-ong gi-sumpay, susiha ang
wire ana. Ayaw og padayon hangtod matul-id.

---

## 15. FUNCTION TEST

- [ ] **F1** — Mo-ON ang LCD ug naay display (dili puti, dili itom)
- [ ] **F2** — Steady ang ESP32 LED
- [ ] **F3** — Naay basa ang voltage sensor sa LCD (~12–13V human sa calibration, §18)
- [ ] **F4** — Naay oras ang RTC sa LCD (dili 00:00)
- [ ] **F5** — Naay basa ang feed level (gikan sa ultrasonic)
- [ ] **F6** — Ma-connect ang app sa BLE ("OxyFeeder")
- [ ] **F7** — Mo-abot ang data sa app (DO, feed, battery)
- [ ] **F8** — "Feed Now" sa app → mo-lihok ang servo **ug** mo-tuyok ang motor
- [ ] **F9** — "Test SMS" sa app → makadawat og text
- [ ] **F10** — Makita ang camera stream sa app
- [ ] **F11** — Dugangi og schedule sa app, i-power-cycle ang ESP32, susiha kung na-save gihapon
      *(test ni sa GPIO22 → Pin 19 nga wire)*

---

## 16. TROUBLESHOOTING

| Sintomas | Hinungdan | Buhaton |
|----------|-----------|---------|
| **Patay ang ESP32, okay ang Arduino** | Naka-12V/Vin ang Arduino — walay gipamatud-an bahin sa 5V rail | Sukda ang **LOGIC rail → GND**. 0V = walay converter output nga naka-sumpay. Susiha pod ang continuity sa ESP32 GND → GND bar. |
| **Naay resistor sa ESP32 GND** | Sayop nga wiring | 🔴 Tanggala. Walay resistor sa ground wire. |
| Nagkurap ang ESP32 LED | Kulang kuryente, o luag ang GND | Sukda ang LOGIC samtang nagkurap. Susiha ang tanang GND screw. |
| Mo-reset ang ESP32 inig lihok sa servo | Parehas silag rail ang servo | Kaugalingong **7.4V converter** ang servo |
| **Mo-utro-utro ang servo / dili mo-lihok** | **Ubos sa 6.0V ang supply** | Sukda ang SERVO rail — dapat 6.0–7.4V. Dili gyud mogana sa 5.1V. |
| Mag-jitter ang servo samtang nag-idle | Normal sa digital servo | Mo-detach ang firmware human sa feed — pasagdi |
| Puti ang LCD | Naka-USB ang Arduino | Normal. Tanggala ang USB. |
| Itom ang LCD, walay gikan | **Nabaylo ang DC ug SCK**, o kulang 5V | DC = **Pin 38**, SCK = **Pin 52**. Sukda ang VCC **sa LCD mismo**. |
| Dili mo-dagan ang motor | Nabalhin og usa ang IN1/IN2/ENA | IN1=**4**, IN2=**5**, ENA=**12**. Walay IN3. |
| **DO = 0.00** | **UGA ang probe — walay electrolyte** | **Dili ni wiring problem.** Naghulat pa og NaOH 0.5 mol/L. |
| **RTC = 00:00** | Patay o wala ang coin cell | Sukda ang CR2032 (2.8V+). I2C scanner dapat makakita og **0x68**. |
| Sayop kaayo ang battery % | Daan pa ang `VOLTAGE_RATIO` | I-calibrate — tan-awa ang §18 |
| Walay data sa app | Sayop ang divider, o dili common ang GND | Ang junction dapat **≈3.3V**. Kung 5V, himoa pag-usab. |
| Dili mo-gana ang Feed Now | Luag ang pull-down o sayop ang pin | 10K sa Pin 17 → GND. Susiha ang wire gikan sa ESP32 D12. |
| **Dili mo-save ang schedules / phone number** | **Wala ang GPIO22 → Pin 19** | Ang wire nga permi makalimtan. Tan-awa ang §7.12. |
| Mo-click ra ang buzzer, dili mo-beep | **Passive** diay nga buzzer | Kinahanglan `tone()` sa firmware, dili `digitalWrite` |
| Walay SMS | Walay SIM, luag ang SIM, kulang kuryente | Papel sa likod sa SIM. Susiha ang 1000uF cap ug ang antenna. |
| Mabugto ang fuse inig feeding | Gamay ra ang slot 5 | **5A** ang kinahanglan sa servo branch, dili 2A |
| Init kaayo ang converter | Sobra ang load | Susiha kung tama ang rail sa kada module |
| Mo-reset ang Arduino kada karon ug unya | Luag nga 5V o GND | Hugti tanan. Sukda ang 5V direkta sa pin. |

---

## 17. FINAL CHECK

- [ ] Tanan nga wire hugot, walay luag
- [ ] Tanan nga GND naa sa GND bus bar — ug **walay resistor sa bisan unsang ground wire**
- [ ] LOGIC = 5.10V · POWER = 5.10V · **SERVO = 7.4V** (sukda una i-sumpay ang servo)
- [ ] Walay wire nga naghikap sa motor o sa init nga bahin
- [ ] Naka-label ang upat ka bus bar
- [ ] Naka-litrato ang wiring
- [ ] Ang **LCD, toggle switch ug buzzer** naa sa **lid (gawas)**
- [ ] Ang uban tanan naa sa **sulod** sa enclosure
- [ ] Hugot ang cable gland, walay tubig nga makasulod

---

## 18. PARA SA MAG-FIRMWARE

**Walay usbon nga pin.** Kini nga guide gihimo **gikan** sa firmware, mao nga kung mosunod ka niini,
tugma na ang tanang `#define`. I-flash lang. Pero tulo ka calibration value ang kinahanglan tan-awon:

### 1. 🔴 `VOLTAGE_RATIO` — halos sigurado nga sayop na
`oxyfeeder_firmware.ino` linya 137: `const float VOLTAGE_RATIO = 4.57;`
Gi-calibrate ni sa **daan, guba** nga voltage sensor. Lahi ang resistor sa bag-ong module.

1. Sukda ang tinuod nga battery voltage gamit multimeter
2. Basaha ang gi-report sa Serial Monitor (9600 baud)
3. `bag-ong ratio = 4.57 × (tinuod ÷ gi-report)`
4. Usba ang linya 137, i-flash pag-usab

### 2. Hopper calibration
Linya 117–118: `HOPPER_EMPTY_CM = 37` · `HOPPER_FULL_CM = 5`. Sukda ang aktuwal nga hopper ug usba.

### 3. SMS cooldown testing mode pa
Linya 186: `SMS_COOLDOWN = 10000` (10 segundo). Okay sa bench; taasi ni una i-deploy o mahurot ang load.

### Dili usbon
1. `dispenseFeed()` kinahanglan **BLOCKING** gihapon (delay-based) — gikan sa daan nga HX711 issue
2. Ang Pin 17 ug Pin 3 kinahanglan `INPUT`, **dili** `INPUT_PULLUP` — mag-away sa pull-down
3. Serial baud **9600** (Arduino → ESP32)
4. ESP32 Serial2: **RX = GPIO26**, walay TX
5. 15-second startup delay una mobasa og GPIO command
6. Andam na ang DO calibration debug print para inig abot sa electrolyte
7. Naka-comment out ang load cell / HX711 — ultrasonic na ang feed level
8. Ang 7.4V nga supply sa servo **walay** kinahanglan nga usbon sa firmware

### Nahibaloan nga bug (dili wiring — ayaw ni pangitaag sayop sa wire)
- **Wala gigamit ang feed duration.** Mo-send ang app og `FEED:5` pero `dispenseFeed(2)` ang gidagan sa
  Arduino. 2 segundo permi. Parehas pod sa scheduled feeds.
- **Dili mo-survive sa reboot ang phone number nga walay `+`.** Ang `loadPhoneNumberFromEEPROM()`
  modawat ra og data nga nagsugod sa `+`, pero local format ang default ug ang hint sa app. Kung mo-save
  ka nga walay `+`, mobalik sa default inig patay sa kuryente.

---

## 19. PAG-SYNC SA DOKUMENTO

| File | Pinulongan | Kahimtang |
|------|------------|-----------|
| `guide/WIRING_GUIDE_BISAYA_REV4.md` | Bisaya | ✅ **karon — kini nga file** |
| `docs/wiring-guide-rev4.md` | English | ✅ karon, parehas ang section |
| Blueprint artifact | Drawing | ✅ karon |
| `guide/WIRING_GUIDE_BISAYA.md` | Bisaya | ⚠️ Rev 2/3 — gitipigan para history |
| `docs/wiring-guide.md` | English | ⚠️ Rev 3 — gitipigan para history |
| `CLAUDE.md`, `hardware-overview.md`, `sensor-list.md` | — | ❌ luma kaayo ang pin |

Kung usbon nimo ang usa ka wire, usba pod sa uban ug taasi ang Rev.

| Rev | Petsa | Giusab |
|-----|-------|--------|
| 1 | — | Orihinal — sayop ang ultrasonic pin, naa pa ang load cell |
| 2 | Sept 16, 2026 | Ultrasonic → Pin 13/48, gitangtang ang load cell, gidugang ang GPIO22 → Pin 19 |
| 3 | Sept 22, 2026 | English nga bersyon; gi-usab para sa bag-ong build; gi-susi ang pin batok sa firmware |
| **4** | **Sept 26, 2026** | **Gibalhin ang servo sa kaugalingong 7.4V rail (6.0–7.4V ang DS S020B); 3-pin nga module na ang buzzer; bag-ong fuse plan para sa 5 ka branch; kompleto na ang voltage sensor wiring uban ang duha ka ground; gidugang ang troubleshooting gikan sa unang build** |
