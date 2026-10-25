/*
  =============================================================================
  SIM800L AT Passthrough — Diagnostic Sketch
  =============================================================================

  Purpose: talk to the SIM800L directly, with nothing else running.
  Whatever you type in the Serial Monitor goes straight to the module, and
  whatever the module says comes straight back. No sensors, no BLE, no LCD.

  -----------------------------------------------------------------------------
  WIRING (unchanged from the main build)
    SIM800L TXD  -> Arduino Pin 15 (RX3)      <- crossed, not straight
    SIM800L RXD  -> Arduino Pin 14 (TX3)
    SIM800L VCC  -> 5V POWER bus bar
    SIM800L GND  -> GND bus bar
    1000uF capacitor across VCC and GND, ON THE MODULE ITSELF
    Antenna connected BEFORE powering on

  -----------------------------------------------------------------------------
  HOW TO USE

  1. Toggle switch OFF
  2. Disconnect the 5V wire from the Arduino's 5V pin (leave GND connected)
  3. Plug in USB, upload this sketch
  4. Leave USB connected, toggle switch ON
     (Arduino runs on USB; SIM800L runs on the 5V POWER rail — it needs the
      rail's current capability, USB cannot supply a 2A transmit burst)
  5. Serial Monitor: 9600 baud, line ending = "Newline"
  6. Type the commands below, one at a time

  -----------------------------------------------------------------------------
  COMMANDS TO TRY, IN ORDER

    AT              -> OK                     module is alive
    AT+CPIN?        -> +CPIN: READY           SIM detected and unlocked
    AT+CSQ          -> +CSQ: 18,0             signal: first number, 10+ is good
                                              (99 means no signal at all)
    AT+CREG?        -> +CREG: 0,1  or  0,5    registered to the network
                                              (0,2 = still searching)
    AT+CSCA?        -> +CSCA: "+639..."       message centre number
                                              (blank = SMS will never send)
    AT+CMGF=1       -> OK                     switch to SMS text mode

  Seeing your command repeated back is NORMAL — the module echoes by default.
  What matters is whether a real answer follows it.

  -----------------------------------------------------------------------------
  SENDING A TEST SMS BY HAND

    AT+CMGF=1
    AT+CMGS="+639550717546"
      wait for the >  prompt, then type your message, then press Ctrl+Z
      (in the Arduino Serial Monitor, send the character 26 — or just use
       TEST_SMS:1 in the main firmware once AT commands are confirmed working)

  =============================================================================
*/

void setup() {
  Serial.begin(9600);    // USB to the computer
  Serial3.begin(9600);   // TX3 = Pin 14, RX3 = Pin 15  -> SIM800L

  delay(2000);           // let the module finish booting

  Serial.println();
  Serial.println(F("=============================================="));
  Serial.println(F("  SIM800L AT PASSTHROUGH"));
  Serial.println(F("=============================================="));
  Serial.println(F("Serial Monitor must be set to:"));
  Serial.println(F("  9600 baud   |   Line ending: Newline"));
  Serial.println();
  Serial.println(F("Type:  AT          expect: OK"));
  Serial.println(F("Then:  AT+CPIN?    expect: +CPIN: READY"));
  Serial.println(F("Then:  AT+CSQ      expect: +CSQ: <10 or more>,0"));
  Serial.println(F("Then:  AT+CREG?    expect: +CREG: 0,1  or  0,5"));
  Serial.println();
  Serial.println(F("Your command being repeated back is normal (echo)."));
  Serial.println(F("What matters is the answer that follows it."));
  Serial.println(F("----------------------------------------------"));
  Serial.println();
}

void loop() {
  // SIM800L -> computer
  while (Serial3.available()) {
    Serial.write(Serial3.read());
  }

  // computer -> SIM800L
  while (Serial.available()) {
    char c = Serial.read();

    // The Arduino IDE 2.x Serial Monitor cannot send control characters, and
    // AT+CMGS needs Ctrl+Z (character 26) to actually send the message.
    // Type  ~  on its own line to stand in for Ctrl+Z.
    // Type  `  on its own line to send ESC (27) and abort a stuck > prompt.
    if (c == '~') {
      Serial3.write(26);
      Serial.println(F("\r\n[sent Ctrl+Z]"));

      // Swallow the CR/LF the Serial Monitor appends after the '~'.
      // Without this the module receives an empty command line straight
      // after the message terminator and replies with a bare ERROR.
      delay(20);
      while (Serial.available()) {
        char nxt = Serial.peek();
        if (nxt == '\r' || nxt == '\n') Serial.read();
        else break;
      }
    } else if (c == '`') {
      Serial3.write(27);
      Serial.println(F("\r\n[sent ESC - aborted]"));
    } else {
      Serial3.write(c);
    }
  }
}
