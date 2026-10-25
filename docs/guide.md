# Your Document Playbook: When to Call Each File

---

## 1. system_research.md

**Think of this as:** The “Project Bible” or the “Scientific Foundation.”  
**Core Purpose:** Explains the fundamental *why* and the core operational logic of the project, based on the original research.  
**When to Call It:**  
- When you need to generate high-level summaries or introductory text (like for a README.md).  
- When onboarding a new team member and want the AI to explain the project's purpose.  
- When brainstorming a major new feature and ensuring it aligns with the project's core objectives.  

**Example Prompt:**  
> "Generate a README for the firmware folder. Use the core concepts from @docs/system_research.md to explain the system's input-process-output logic and its main purpose."

---

## 2. create-prd.md (Product Requirements Document)

**Think of this as:** The “Feature Blueprint” or the “User’s Wishlist.”  
**Core Purpose:** Defines what the final product should do from a user’s perspective — includes all features, user stories, and success metrics.  
**When to Call It (Very Frequently):**  
- When building any UI (most common use case).  
- When implementing the logic for a specific feature (e.g., alerts).  
- When writing test cases to verify a feature works as expected.  

**Example Prompt:**  
> "Based on the Key Features and User Stories in @docs/create-prd.md, create the UI for the 'Settings' screen. It needs controls for setting the feeding schedule and the low-oxygen alert threshold."

---

## 3. wiring-guide-rev4.md ⭐ (and guide/WIRING_GUIDE_BISAYA_REV4.md)

**Think of this as:** The “Build Manual” — the physical truth of the machine.
**Core Purpose:** Step-by-step instructions for wiring the unit from bare components, plus the
authoritative pin table, the dead-pin list, the staged power-up test, and troubleshooting.
**Language:** `docs/wiring-guide-rev4.md` is English, `guide/WIRING_GUIDE_BISAYA_REV4.md` is Bisaya.
Same content, same section numbers — "Section 7.12" is the same wire in both.
**Visual companion:** the blueprint artifact — power tree, Arduino signal map, resistor detail drawings.

> **Rev 3 (`wiring-guide.md`, `WIRING_GUIDE_BISAYA.md`) is superseded** and kept only for history.
> It has the servo on 5V (it needs 7.4V), a 2-pin buzzer, and the old fuse plan. Always use **Rev 4**.

> **⚠️ This is the ONLY correct pin reference.** It is checked against the firmware. The documents
> below (hardware-overview.md, sensor-list.md) still describe the load cell, pins A0/A1, motor pins
> 10/11, and the logic level shifter — all removed from the design. **When they disagree with the
> wiring guide, the wiring guide is right.**

**When to Call It:**
- When wiring or rewiring the hardware.
- When you need to know which pin anything is actually on.
- When writing or changing firmware that touches a pin.
- When debugging "the app shows no data" or "Feed Now does nothing."

**Example Prompt:**
> "Using the pin table in @docs/wiring-guide.md, check that every `#define` in
> firmware/oxyfeeder_firmware/oxyfeeder_firmware.ino matches the wiring, and list any mismatch."

---

## 4. hardware-overview.md ⚠️ OUT OF DATE

**Think of this as:** The “System Architecture Map.”  
**Core Purpose:** Explains how all major hardware components connect and communicate at a high level.  

> **⚠️ The pin mappings in this file are wrong.** Use it only for the high-level block diagram and the
> data-flow narrative. For any actual pin, use `wiring-guide.md`.

**When to Call It:**  
- When writing the communication protocol between the Arduino and the ESP32.  
- When creating data models (classes) in your Flutter app to represent data coming from the device.  
- When debugging issues where one part of the system isn’t talking to another.  

**Example Prompt:**  
> "Using the signal flow diagram described in @docs/hardware-overview.md, write a Dart class named OxyFeederStatus that can model the complete data packet sent from the ESP32 to the Flutter app."

---

## 5. sensor-list.md ⚠️ OUT OF DATE

**Think of this as:** The “Technical Spec Sheet.”  
**Core Purpose:** Provides specific, low-level technical details for each sensor (voltage, interface, etc.).  

> **⚠️ Still lists the load cell as the feed-level sensor.** It was replaced by an ultrasonic HC-SR04.
> Use this file for voltage ranges and interface types only — **never for pin numbers.**

**When to Call It:**  
- When writing the actual firmware code to read data from a specific sensor.  
- When you need to know which pin to connect a sensor to. → **use `wiring-guide.md` instead**
- When debugging inaccurate sensor readings and need to check the specs.  

**Example Prompt:**  
> "I'm writing the Arduino code to read from the Gravity DO Sensor. Using the details from @docs/sensor-list.md, provide the complete function to read the analog value and convert it to an mg/L reading. Reference the correct voltage for the sensor."

---

## 6. power-system.md

**Think of this as:** The “Energy and Safety Guide.”  
**Core Purpose:** Explains how the system is powered and what safety measures are in place.  
**When to Call It:**  
- When writing firmware code to monitor battery voltage.  
- When implementing low-power or battery-saving features.  
- When creating the battery status UI in the Flutter app.  

**Example Prompt:**  
> "Based on the safety recommendations in @docs/power-system.md, write the firmware logic that sends a 'Low Battery' alert when voltage drops below 11.5V and then puts the device into deep sleep mode to protect the battery."

---

## 7. generate-task.md & process-task-list.md

**Think of these as:** “Project Management Docs.”  
**Core Purpose:** Help you and your team stay organized. These are less frequently used for generating *application code.*  
**When to Call It:**  
- When you want the AI to act as a project manager.  

**Example Prompt:**  
> "Review @docs/generate-task.md and tell me what the next three tasks are for the 'Flutter Dev' role. Provide a brief summary of each."

---

### Summary Cheat Sheet

| **Document Name**       | **Use For...**                           | **Keywords**                                         |
|--------------------------|------------------------------------------|------------------------------------------------------|
| **wiring-guide.md** ⭐    | **Pins, wiring & the physical build**    | *pin, wire, connect, rewire, GPIO, rail, voltage*    |
| system_research.md       | The “Big Picture” & Core Logic           | *Why, purpose, concept, overview*                    |
| create-prd.md            | Features, UI & User Needs                | *UI, screen, feature, alert, user*                   |
| hardware-overview.md ⚠️  | Data flow only — its pins are WRONG      | *Communication, protocol, data model*                |
| sensor-list.md ⚠️        | Sensor specs only — its pins are WRONG   | *Voltage, interface, driver*                         |
| power-system.md          | Battery Logic & Power Features           | *Battery, power, voltage, sleep mode, safety*        |
| generate-task.md         | Planning & Task Management               | *Tasks, next steps, schedule, plan*                  |
