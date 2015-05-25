#include <Servo.h>
#include <PS3BT.h>
#include <PS3USB.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

USB Usb;
BTD Btd(&Usb);
//PS3BT PS3(&Btd);
PS3USB PS3(&Usb);

int servo = 8;
int pwm1 = 2, ini1 = 3, ini2 = 4, pwm2 = 5, ini3 = 6, ini4 = 7;
Servo servoMotor;


void setup()
{
    servoMotor.attach(servo);
    pinMode(pwm1, OUTPUT);
    pinMode(ini1, OUTPUT);
    pinMode(ini2, OUTPUT);

    pinMode(pwm2, OUTPUT);
    pinMode(ini3, OUTPUT);
    pinMode(ini4, OUTPUT);
    Serial.begin(115200);
    if (Usb.Init() == -1) {
        Serial.println("Error in USB lib");
        return;
        while (1); //halt
    }
    Serial.println("USB lib started");
}

void loop()
{
    Usb.Task();
    if (PS3.PS3Connected) {
        if (PS3.getButtonClick(PS)) {
            //PS3.disconnect();
        }
       int leftAnalog = PS3.getAnalogHat(LeftHatX);
        if (leftAnalog || leftAnalog == 0) {
            moveServo(map(leftAnalog, 0, 255, 0, 180));
        } else {
            moveServo(90);
        }

        int btR2 = PS3.getAnalogButton(R2);
        int btL2 = PS3.getAnalogButton(L2);
        if (btR2) {
            moveMotor(btR2, true);
            delay(40);
        } else if (btL2) {
            moveMotor(btL2, false);
            delay(40);
        } else {
            moveMotor(0, true);
        }

        int btX = PS3.getAnalogButton(CROSS);
        int btQ = PS3.getAnalogButton(SQUARE);
        if (btX) {
            moveMotor(btX, true);
            delay(40);
        } else if (btQ) {
            moveMotor(btQ, false);
            delay(40);
        } else {
            moveMotor(0, true);
        }

        if (PS3.getButtonPress(LEFT)) {
            moveServo(map(0, 0, 255, 0, 180));
            delay(40);
        }
        if (PS3.getButtonPress(RIGHT)) {
            moveServo(map(255, 0, 255, 0, 180));
            delay(40);
        }
    }
}


void moveServo(int graus) {
    if (graus < 15) {
        graus = 15;
    }
    servoMotor.write(graus);
}

void moveMotor(int number, bool forward) {
    if (number == 0) {
        analogWrite(pwm1, number);
        digitalWrite(ini1, HIGH);
        digitalWrite(ini2, HIGH);

        analogWrite(pwm2, number);
        digitalWrite(ini3, HIGH);
        digitalWrite(ini4, HIGH);
    } else if (forward) {
        analogWrite(pwm1, number);
        digitalWrite(ini1, LOW);
        digitalWrite(ini2, HIGH);

        analogWrite(pwm2, number);
        digitalWrite(ini3, LOW);
        digitalWrite(ini4, HIGH);
    } else {
        analogWrite(pwm1, number);
        digitalWrite(ini1, HIGH);
        digitalWrite(ini2, LOW);

        analogWrite(pwm2, number);
        digitalWrite(ini3, HIGH);
        digitalWrite(ini4, LOW);
    }
}
