// ============================================================
// MS101 Makespace – EE Project
// TRANSMITTER (Tx) – Dual Joystick, Arduino Nano
// UART Wired Transmitter → ESP32 Receiver
// ============================================================

#include <Arduino.h>

// ---------- Pin Definitions ----------
int VRx1 = A7;
int VRy1 = A6;
int VRx2 = A1;
int VRy2 = A0;

int SW1 = 12;
int SW2 = 11;

int LED = 2;

// ---------- Config ----------
const int BAUD_RATE = 9600;
const int DEAD_ZONE = 10;
const int TX_INTERVAL_MS = 50;

// ---------- Helpers ----------
// Returns -255 (full left/down) to 0 (centre) to +255 (full right/up)
int mapJoystick(int raw) {
    const int centre = 512;
    if (abs(raw - centre) < DEAD_ZONE) {
        return 0;
    }
    return map(raw, 0, 1023, -255, 255);
}

// 90 deg clockwise axis transform: x' = y, y' = -x
void transformAxes(int &x, int &y) {
    int oldX = x;
    x = y;
    y = -oldX;
}

// ---------- Setup ----------
void setup() {
    pinMode(SW1, INPUT_PULLUP);
    pinMode(SW2, INPUT_PULLUP);
    pinMode(LED, OUTPUT);

    Serial.begin(BAUD_RATE);

    // Blink LED to indicate power-on / ready
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED, HIGH); delay(150);
        digitalWrite(LED, LOW); delay(150);
    }
}

// ---------- Main Loop ----------
void loop() {
    static unsigned long lastTx = 0;
    unsigned long now = millis();

    if (now - lastTx >= TX_INTERVAL_MS) {
        lastTx = now;

        int raw_x2 = analogRead(VRx2);
        int raw_y2 = analogRead(VRy2);

        int jVRx1 = mapJoystick(analogRead(VRx1));
        int jVRy1 = mapJoystick(analogRead(VRy1));
        int jVRx2 = mapJoystick(raw_x2);
        int jVRy2 = mapJoystick(raw_y2);

        Serial.println("Raw Joystick Values:");
        Serial.print(" VRx2: "); Serial.print(raw_x2);
        Serial.print(" VRy2: "); Serial.println(raw_y2);

        transformAxes(jVRx1, jVRy1);
        transformAxes(jVRx2, jVRy2);

        // Differential-drive mix: left joystick → L/R motor speeds
        int L = constrain(jVRy1 + jVRx1, -255, 255);
        int R = constrain(jVRy1 - jVRx1, -255, 255);

        // Right joystick → function axes
        int F1 = jVRx2;
        int F2 = jVRy2;

        // Switch states (active-low)
        int S1 = (digitalRead(SW1) == LOW) ? 1 : 0;
        int S2 = (digitalRead(SW2) == LOW) ? 1 : 0;

        digitalWrite(LED, (S1 || S2) ? HIGH : LOW);

        // Transmit framed ASCII packet: <L,R,F1,F2,S1,S2>\n
        char buf[48];
        int len = snprintf(buf, sizeof(buf), "<%d,%d,%d,%d,%d,%d>\n",
        L, R, F1, F2, S1, S2);
        if (len > 0) {
            Serial.write((const uint8_t *)buf, (size_t)len);
        }
    }
} 

// // ============================================================
// // MS101 Makespace – EE Project
// // TRANSMITTER (Tx) – Dual Joystick, Arduino Nano
// // UART Wired Transmitter → ESP32 Receiver
// // ============================================================

// #include <Arduino.h>

// // ---------- Pin Definitions ----------
// int VRx1 = A7;
// int VRy1 = A6;
// int VRx2 = A1;
// int VRy2 = A0;

// int SW1 = 12;
// int SW2 = 11;

// int LED = 2;

// // ---------- Config ----------
// const int BAUD_RATE = 9600;
// const int DEAD_ZONE = 10;
// const int TX_INTERVAL_MS = 20;

// // ---------- Helpers ----------
// // Returns -255 (full left/down) to 0 (centre) to +255 (full right/up)
// int mapJoystick(int raw) {
// const int centre = 512;
// if (abs(raw - centre) < DEAD_ZONE) {
// return 0;
// }
// return map(raw, 0, 1023, -255, 255);
// }

// // ---------- Setup ----------
// void setup() {
//   pinMode(SW1, INPUT_PULLUP);
//   pinMode(SW2, INPUT_PULLUP);
//   pinMode(LED, OUTPUT);

//   Serial.begin(BAUD_RATE);

//   // Blink LED to indicate power-on / ready
//   for (int i = 0; i < 3; i++) {
//   digitalWrite(LED, HIGH); delay(150);
//   digitalWrite(LED, LOW); delay(150);
//   }
// }

// // ---------- Main Loop ----------
// void loop() {
//   static unsigned long lastTx = 0;
//   unsigned long now = millis();

//   if (now - lastTx >= TX_INTERVAL_MS) {
//   lastTx = now;

//   int jVRx1 = mapJoystick(analogRead(VRx1));
//   int jVRy1 = mapJoystick(analogRead(VRy1));
//   int jVRx2 = mapJoystick(analogRead(VRx2));
//   int jVRy2 = mapJoystick(analogRead(VRy2));

//   // Differential-drive mix: left joystick → L/R motor speeds
//   int L = constrain(jVRy1 + jVRx1, -255, 255);
//   int R = constrain(jVRy1 - jVRx1, -255, 255);

//   // Right joystick → function axes
//   int F1 = jVRx2;
//   int F2 = jVRy2;

//   // Switch states (active-low)
//   int S1 = (digitalRead(SW1) == LOW) ? 1 : 0;
//   int S2 = (digitalRead(SW2) == LOW) ? 1 : 0;

//   digitalWrite(LED, (S1 || S2) ? HIGH : LOW);

//   // Transmit framed ASCII packet: <L,R,F1,F2,S1,S2>\n
//   char buf[48];
//   int len = snprintf(buf, sizeof(buf), "<%d,%d,%d,%d,%d,%d>\n",
//   L, R, F1, F2, S1, S2);
//   Serial.println(buf);

//   // // Transmit framed ASCII packet: <L,R,F1,F2,S1,S2>
//   // Serial.print("<");
//   // Serial.print(L); Serial.print(",");
//   // Serial.print(R); Serial.print(",");
//   // Serial.print(F1); Serial.print(",");
//   // Serial.print(F2); Serial.print(",");
//   // Serial.print(S1); Serial.print(",");
//   // Serial.print(S2);
//   // Serial.println(">");
//   }
// }