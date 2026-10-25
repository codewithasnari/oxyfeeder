/*
  =============================================================================
  Hopper Feed Level Calibration
  =============================================================================

  Measures the hopper with the ultrasonic sensor and works out the two
  numbers the main firmware needs:

      HOPPER_EMPTY_CM   distance to the bottom when the hopper is empty
      HOPPER_FULL_CM    distance to the feed surface when the hopper is full

  The values currently in the firmware (37 and 5) are guesses. Replace them
  with what you measure here or the percentage will never be right.

  This also produces the paired readings for Section III-B of the paper -
  actual fill level versus what the system reports.

  -----------------------------------------------------------------------------
  WIRING
    HC-SR04 VCC  -> 5V LOGIC bus bar
    HC-SR04 GND  -> GND bus bar
    HC-SR04 TRIG -> see TRIG_PIN below
    HC-SR04 ECHO -> see ECHO_PIN below

  Change the two pins below to whichever pair worked in ultrasonic_test.

  -----------------------------------------------------------------------------
  HOW TO USE

    1. Toggle OFF, disconnect the Arduino 5V wire, plug USB, upload
    2. Toggle ON, USB still in. Serial Monitor at 9600, line ending Newline
    3. Mount the sensor in its FINAL position at the top of the hopper,
       facing straight down. Moving it later invalidates the calibration.

    4. EMPTY the hopper completely.  Wait for a steady reading, type:  E
    5. FILL the hopper to the level you call 100%.  Wait, then type:  F
    6. Type  P  to print the constants for the firmware

    7. For the paper's accuracy data, empty it again and add feed in
       known steps - 10%, 20%, 30% and so on by weight or by marks on
       the hopper - recording the actual level against the reported one.

  COMMANDS
    E   mark the current distance as EMPTY
    F   mark the current distance as FULL
    P   print the firmware constants
    R   reset both marks
  =============================================================================
*/

// Confirmed working on these pins 12 Oct 2026 with the replacement sensor.
// Same pins the main firmware uses, so nothing else needs changing.
#define TRIG_PIN 13
#define ECHO_PIN 48

int emptyCm = -1;     // distance when empty, -1 = not yet marked
int fullCm  = -1;     // distance when full
String cmd  = "";

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  delay(600);

  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F("  HOPPER FEED LEVEL CALIBRATION"));
  Serial.println(F("=================================================="));
  Serial.print(F("TRIG = Pin ")); Serial.print(TRIG_PIN);
  Serial.print(F("   ECHO = Pin ")); Serial.println(ECHO_PIN);
  Serial.println();
  Serial.println(F("  E = mark EMPTY     F = mark FULL"));
  Serial.println(F("  P = print values   R = reset"));
  Serial.println();
  Serial.println(F("Mount the sensor in its final position first."));
  Serial.println(F("--------------------------------------------------"));
  Serial.println();
}

// Single ping, centimetres. -1 means no echo.
long pingCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long d = pulseIn(ECHO_PIN, HIGH, 30000);
  if (d == 0) return -1;
  return d / 58;
}

// Median of three, the same filtering the main firmware uses.
// Feed surfaces scatter ultrasound, so single readings can spike.
long readDistance() {
  long r[3];
  for (int i = 0; i < 3; i++) { r[i] = pingCm(); delay(20); }
  for (int i = 0; i < 2; i++)
    for (int j = i + 1; j < 3; j++)
      if (r[i] > r[j]) { long t = r[i]; r[i] = r[j]; r[j] = t; }
  return r[1];
}

// Spread across three pings - a stability indicator. A noisy surface
// gives a wide spread, which is itself worth reporting in the paper.
long readSpread() {
  long lo = 999, hi = -999;
  for (int i = 0; i < 3; i++) {
    long v = pingCm();
    if (v > 0) { if (v < lo) lo = v; if (v > hi) hi = v; }
    delay(20);
  }
  if (hi < 0) return -1;
  return hi - lo;
}

void printBar(int pct) {
  Serial.print(F(" ["));
  for (int i = 0; i < 20; i++) Serial.print(i < pct / 5 ? '#' : '.');
  Serial.print(F("]"));
}

void handleCommand(char c) {
  long d = readDistance();

  if (c == 'E' || c == 'e') {
    if (d <= 0) { Serial.println(F(">> No reading. Cannot mark EMPTY.")); return; }
    emptyCm = d;
    Serial.print(F(">> EMPTY marked at ")); Serial.print(emptyCm); Serial.println(F(" cm"));
  }
  else if (c == 'F' || c == 'f') {
    if (d <= 0) { Serial.println(F(">> No reading. Cannot mark FULL.")); return; }
    fullCm = d;
    Serial.print(F(">> FULL marked at ")); Serial.print(fullCm); Serial.println(F(" cm"));
  }
  else if (c == 'R' || c == 'r') {
    emptyCm = -1; fullCm = -1;
    Serial.println(F(">> Both marks cleared."));
  }
  else if (c == 'P' || c == 'p') {
    Serial.println();
    Serial.println(F("=================================================="));
    if (emptyCm < 0 || fullCm < 0) {
      Serial.println(F("  Mark EMPTY (E) and FULL (F) first."));
    } else if (emptyCm <= fullCm) {
      Serial.println(F("  PROBLEM: empty distance must be LARGER than full."));
      Serial.println(F("  The sensor should be further from the bottom than"));
      Serial.println(F("  from the feed surface. Check it faces straight down."));
    } else {
      Serial.println(F("  Paste these into oxyfeeder_firmware.ino"));
      Serial.println();
      Serial.print(F("  const int HOPPER_EMPTY_CM = "));
      Serial.print(emptyCm); Serial.println(F(";"));
      Serial.print(F("  const int HOPPER_FULL_CM  = "));
      Serial.print(fullCm); Serial.println(F(";"));
      Serial.println();
      Serial.print(F("  Usable span: "));
      Serial.print(emptyCm - fullCm);
      Serial.println(F(" cm"));
      if (emptyCm - fullCm < 10) {
        Serial.println(F("  WARNING: under 10cm of travel gives coarse"));
        Serial.println(F("  resolution. Each cm is a large jump in percent."));
      }
      if (fullCm < 5) {
        Serial.println(F("  WARNING: full distance under 5cm is close to the"));
        Serial.println(F("  2cm blind zone. Mount the sensor higher."));
      }
    }
    Serial.println(F("=================================================="));
    Serial.println();
  }
}

void loop() {
  // commands
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (cmd.length() > 0) { handleCommand(cmd.charAt(0)); cmd = ""; }
    } else {
      cmd += c;
    }
  }

  long d = readDistance();

  Serial.print(F("distance "));
  if (d <= 0) {
    Serial.print(F("  --  NO ECHO"));
  } else {
    if (d < 10) Serial.print(' ');
    Serial.print(d);
    Serial.print(F(" cm"));

    long spread = readSpread();
    Serial.print(F("   spread "));
    Serial.print(spread < 0 ? 0 : spread);
    Serial.print(F(" cm"));

    if (emptyCm > 0 && fullCm > 0 && emptyCm > fullCm) {
      int pct = map(d, emptyCm, fullCm, 0, 100);
      pct = constrain(pct, 0, 100);
      Serial.print(F("   level "));
      if (pct < 100) Serial.print(' ');
      if (pct < 10)  Serial.print(' ');
      Serial.print(pct);
      Serial.print(F("%"));
      printBar(pct);
    } else {
      Serial.print(F("   (mark E and F to see a percentage)"));
    }
  }
  Serial.println();

  delay(700);
}

/*
  =============================================================================
  COLLECTING THE PAPER'S ACCURACY DATA

  Once EMPTY and FULL are marked, empty the hopper and add feed in known
  steps. Record both columns for sheet B2:

      actual %        how much feed you actually added, by weight
                      or by marks you put on the hopper wall
      reported %      what this sketch displays

  Ten points from 0 to 100 is enough. Mean absolute error between the
  two columns is the figure for the paper.

  WATCH THE SPREAD COLUMN. Feed pellets scatter ultrasound, so a full
  hopper often reads noisier than an empty one. If spread grows as the
  hopper fills, that is a genuine finding worth reporting as a limitation
  of ultrasonic sensing on granular material - and it explains why a load
  cell was the original design choice.
  =============================================================================
*/
