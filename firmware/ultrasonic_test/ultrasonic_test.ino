/*
  =============================================================================
  HC-SR04 Ultrasonic Diagnostic  -  tests BOTH pin pairs
  =============================================================================

  Nothing else runs. No LCD, no GSM, no BLE. Just the ultrasonic sensor.

  Each cycle this tries TWO pin pairs, so you can move the sensor's wires
  between them WITHOUT re-uploading:

      PAIR A   TRIG = Pin 13   ECHO = Pin 48      (current build wiring)
      PAIR B   TRIG = Pin 10   ECHO = Pin 11      (known-good spare pins)

  Whichever pair the sensor is actually wired to will report a distance.
  The other will say NO ECHO - that is expected, not a fault.

  -----------------------------------------------------------------------------
  HOW TO USE

    1. Toggle OFF, disconnect the 5V wire from the Arduino 5V pin
       (leave BOTH GND wires connected)
    2. Plug USB, upload this sketch
    3. Toggle ON, USB still plugged in
    4. Serial Monitor at 9600
    5. Hold a flat object - a book works well - about 20cm from the sensor
    6. Watch which pair reports a distance

    Then move TRIG and ECHO to the other pair and watch again.
    No re-upload needed.

  -----------------------------------------------------------------------------
  SENSOR WIRING (unchanged)
    HC-SR04 VCC  -> 5V LOGIC bus bar    (must read 4.8V or higher)
    HC-SR04 GND  -> GND bus bar
    HC-SR04 TRIG -> Pin 13  or  Pin 10
    HC-SR04 ECHO -> Pin 48  or  Pin 11

  Minimum range is 2cm. Touching the sensor reads nothing - hold the
  target at least 10cm away.
  =============================================================================
*/

#define A_TRIG 13
#define A_ECHO 48
#define B_TRIG 10
#define B_ECHO 11

// Pins checked for internal shorts at startup
const uint8_t HEALTH_PINS[] = {13, 48, 10, 11, 22, 23, 24};
const uint8_t HEALTH_COUNT  = sizeof(HEALTH_PINS) / sizeof(HEALTH_PINS[0]);

unsigned long cycle = 0;

void setup() {
  Serial.begin(9600);
  delay(600);

  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F("  HC-SR04 DIAGNOSTIC  -  dual pin pair test"));
  Serial.println(F("=================================================="));

  // ---- pin health ---------------------------------------------------
  // INPUT_PULLUP should read 1 on a healthy pin. A pin reading 0 with
  // nothing attached is shorted to ground - that is how Pin 8 and Pin 9
  // were identified as dead after the 12V incident.
  Serial.println();
  Serial.println(F("PIN HEALTH  (1 = healthy, 0 = shorted to ground)"));
  Serial.println(F("Accurate only with the sensor UNPLUGGED."));
  Serial.print(F("  "));
  for (uint8_t i = 0; i < HEALTH_COUNT; i++) {
    pinMode(HEALTH_PINS[i], INPUT_PULLUP);
  }
  delay(50);
  for (uint8_t i = 0; i < HEALTH_COUNT; i++) {
    Serial.print(F("Pin "));
    Serial.print(HEALTH_PINS[i]);
    Serial.print(F("="));
    Serial.print(digitalRead(HEALTH_PINS[i]));
    Serial.print(F("  "));
  }
  Serial.println();

  // ---- set up both pairs ---------------------------------------------
  pinMode(A_TRIG, OUTPUT); digitalWrite(A_TRIG, LOW);
  pinMode(A_ECHO, INPUT);
  pinMode(B_TRIG, OUTPUT); digitalWrite(B_TRIG, LOW);
  pinMode(B_ECHO, INPUT);

  Serial.println();
  Serial.println(F("PAIR A = TRIG 13 / ECHO 48   (current build wiring)"));
  Serial.println(F("PAIR B = TRIG 10 / ECHO 11   (known-good spares)"));
  Serial.println();
  Serial.println(F("Hold a book about 20cm away. Only the pair you are"));
  Serial.println(F("actually wired to will report a distance."));
  Serial.println(F("--------------------------------------------------"));
  Serial.println();
}

// Fire one trigger pulse and return the echo duration in microseconds.
// 0 means nothing came back within 30ms (about 5 metres).
unsigned long ping(uint8_t trig, uint8_t echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH, 30000);
}

void report(const char* label, uint8_t trig, uint8_t echo) {
  unsigned long d = ping(trig, echo);

  Serial.print(F("  "));
  Serial.print(label);
  Serial.print(F("  TRIG "));
  Serial.print(trig);
  Serial.print(F(" / ECHO "));
  Serial.print(echo);
  Serial.print(F("  ->  "));

  if (d == 0) {
    Serial.print(F("NO ECHO        (echo pin idle = "));
    Serial.print(digitalRead(echo));
    Serial.println(F(")"));
  } else {
    Serial.print(d);
    Serial.print(F(" us  =  "));
    Serial.print(d / 58.0, 1);
    Serial.println(F(" cm"));
  }
}

void loop() {
  cycle++;
  Serial.print(F("cycle "));
  Serial.println(cycle);

  report("PAIR A", A_TRIG, A_ECHO);
  delay(60);                      // let any lingering echo die away
  report("PAIR B", B_TRIG, B_ECHO);

  Serial.println();
  delay(800);
}

/*
  =============================================================================
  READING THE RESULTS

  ONE PAIR READS, THE OTHER SAYS NO ECHO
      Normal. The sensor is only wired to one pair at a time.
      If PAIR B works and PAIR A does not, Pin 13 or Pin 48 is the fault -
      move the ultrasonic to Pin 10 / Pin 11 permanently.

  BOTH PAIRS SAY NO ECHO, ON BOTH WIRINGS
      The pins are not the problem. Work down this list:

      1. VOLTAGE - measure at the sensor's own VCC and GND pins, not the
         bus bar. Needs 4.8V or higher; the HC-SR04 dies around 4.5V.

      2. TRIG AND ECHO SWAPPED - the most common cause. TRIG is the
         Arduino's output, ECHO is its input.

      3. GROUND - continuity from the sensor's GND to the GND bus bar,
         power off. It must beep.

      4. TOO CLOSE - hold the target at least 10cm away.

      5. DEAD SENSOR - try a second HC-SR04 in the same wiring. These
         modules fail often, especially after being fed the wrong voltage.

  A PIN HEALTH LINE SHOWS 0
      That pin is shorted to ground internally - do not use it.
      Re-run with the sensor unplugged to be sure the 0 is the pin
      itself and not the sensor pulling the line down.

  NOTE: the onboard L LED blinking is NORMAL - Pin 13 drives it.
  =============================================================================
*/
