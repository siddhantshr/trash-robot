#include <Arduino.h>

#define VRx1 A7
#define VRy1 A6
#define VRx2 A1
#define VRy2 A0
#define SW1 12
#define SW2 11

void setup() {
    Serial.begin(115200);

    pinMode(SW1, INPUT_PULLUP);
    pinMode(SW2, INPUT_PULLUP);
}

int convert(int v) {
    int mid = 512;

    if (v > mid + 30)
      return map(v, mid + 30, 1023, 0, 255);

    if (v < mid - 30)
      return map(v, mid - 30, 0, 0, -255);

    return 0;
}

void loop() {
    int rawX1 = analogRead(VRx1);
    int rawY1 = analogRead(VRy1);
    int rawX2 = analogRead(VRx2);
    int rawY2 = analogRead(VRy2);
    

    int turn = convert(rawX1);
    int forward = convert(rawY1);

    int func1 = convert(rawX2);
    int func2 = convert(rawY2);

    int leftSpeed = forward + turn;
    int rightSpeed = forward - turn;

    leftSpeed = constrain(leftSpeed, -255, 255);
    rightSpeed = constrain(rightSpeed, -255, 255);

    int s1 = !digitalRead(SW1);
    int s2 = !digitalRead(SW2);

    Serial.print("<");
    Serial.print(leftSpeed);
    Serial.print(",");
    Serial.print(rightSpeed);
    Serial.print(",");
    Serial.print(func1);
    Serial.print(",");
    Serial.print(func2);
    Serial.print(",");
    Serial.print(s1);
    Serial.print(",");
    Serial.print(s2);
    Serial.println(">");

    delay(50);
}