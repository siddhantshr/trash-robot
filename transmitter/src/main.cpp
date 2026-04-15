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
const int BAUD_RATE = 9600; // 9600 bits per second over UART
const int DEAD_ZONE = 10;
const int TX_INTERVAL_MS = 50;

// ---------- Helpers ----------
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

    for (int i = 0; i < 3; i++) {
        digitalWrite(LED, HIGH); delay(150);
        digitalWrite(LED, LOW); delay(150);
    }
}

// ---------- Main Loop ----------
void loop() {
    static unsigned long lastTx = 0; // stores when i last sent the data, initialized to 0 so it sends immediately on startup
    unsigned long now = millis(); // millis returns the number of milliseconds since the program started

    if (now - lastTx >= TX_INTERVAL_MS) { // if enough time has passed since the last transmission ( >= 50ms), then we send the next packet
        lastTx = now;

        int raw_x2 = analogRead(VRx2);
        int raw_y2 = analogRead(VRy2);

        int jVRx1 = mapJoystick(analogRead(VRx1));
        int jVRy1 = mapJoystick(analogRead(VRy1));
        int jVRx2 = mapJoystick(raw_x2);
        int jVRy2 = mapJoystick(raw_y2);

        // Serial.println("Raw Joystick Values:");
        // Serial.print(" VRx2: "); Serial.print(raw_x2);
        // Serial.print(" VRy2: "); Serial.println(raw_y2);

        transformAxes(jVRx1, jVRy1);
        transformAxes(jVRx2, jVRy2);

        // Differential-drive mix: left joystick → L/R motor speeds
        int L = constrain(jVRy1 + jVRx1, -255, 255);
        int R = constrain(jVRy1 - jVRx1, -255, 255);

        // Right joystick → function axes
        int F1 = jVRx2; // belt motor control
        int F2 = jVRy2; // servo control

        // Switch states (active-low)
        int S1 = (digitalRead(SW1) == LOW) ? 1 : 0;
        int S2 = (digitalRead(SW2) == LOW) ? 1 : 0;

        // digitalWrite(LED, (S1 || S2) ? HIGH : LOW); 

        // Transmit framed ASCII packet: <L,R,F1,F2,S1,S2>\n
        char buf[48];
        int len = snprintf(buf, sizeof(buf), "<%d,%d,%d,%d,%d,%d>\n",
        L, R, F1, F2, S1, S2);
        if (len > 0) {
            Serial.write((const uint8_t *)buf, (size_t)len);
        }
    }
}