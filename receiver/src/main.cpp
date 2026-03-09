// ============================================================
//  MS101 Makespace – EE Project
//  RECEIVER (Rx) – ESP32
// ============================================================

#include <Arduino.h>

// ---------- UART ----------
#define RX_PIN 16
#define BAUD_RATE 9600

// ---------- L298N Pins ----------
#define ENA 12
#define IN1 14
#define IN2 27

#define IN3 26
#define IN4 25
#define ENB 33

// ---------- PWM ----------
#define CH_A 0
#define CH_B 1
#define PWM_FREQ 1000
#define PWM_RES 8

// ---------- Packet buffer ----------
char buffer[64];
int bufIndex = 0;

// ---------- Control variables ----------
int leftSpeed, rightSpeed, F1, F2, S1, S2;


// ============================================================
// Motor drive
// ============================================================

void drive(int speed, int IN_A, int IN_B, int pwmChannel)
{
    speed = constrain(speed, -255, 255);

    if (speed > 0)
    {
        digitalWrite(IN_A, HIGH);
        digitalWrite(IN_B, LOW);
    }
    else if (speed < 0)
    {
        digitalWrite(IN_A, LOW);
        digitalWrite(IN_B, HIGH);
        speed = -speed;
    }
    else
    {
        digitalWrite(IN_A, LOW);
        digitalWrite(IN_B, LOW);
    }

    ledcWrite(pwmChannel, speed);
}


// ============================================================
// Parse packet
// ============================================================

void parsePacket(char *pkt)
{
    sscanf(pkt, "<%d,%d,%d,%d,%d,%d>",
           &leftSpeed,
           &rightSpeed,
           &F1,
           &F2,
           &S1,
           &S2);
}


// ============================================================
// Setup
// ============================================================

void setup()
{
    Serial.begin(115200);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, -1);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    ledcSetup(CH_A, PWM_FREQ, PWM_RES);
    ledcSetup(CH_B, PWM_FREQ, PWM_RES);

    ledcAttachPin(ENA, CH_A);
    ledcAttachPin(ENB, CH_B);
}


// ============================================================
// Main loop
// ============================================================

void loop()
{
    while (Serial2.available())
    {
        char c = Serial2.read();

        if (c == '\n')
        {
            buffer[bufIndex] = '\0';

            parsePacket(buffer);

            parsePacket(buffer);

            // Serial.println("Hi");
            Serial.printf("L:%d R:%d F1:%d F2:%d S1:%d S2:%d\n",
            leftSpeed, rightSpeed, F1, F2, S1, S2);

            drive(leftSpeed, IN1, IN2, CH_A);
            drive(rightSpeed, IN3, IN4, CH_B);

            bufIndex = 0;
        }
        else
        {
            if (bufIndex < sizeof(buffer) - 1)
                buffer[bufIndex++] = c;
        }
    }
}