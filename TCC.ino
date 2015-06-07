#include <Servo.h>
//#include <PS3BT.h>
#include <PS3USB.h>
#include <SPI.h>
#include <MFRC522.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */
#define RST_PIN     0 //
#define SS_PIN      1 // Problema no pino 10

USB Usb;
//BTD Btd(&Usb);
//PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x0C, 0xBF, 0xEB);
PS3USB PS3(&Usb);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

int servo = 9;
int led = 8;
int pwm1 = 2, ini1 = 3, ini2 = 4,
    pwm2 = 7, ini3 = 5, ini4 = 6;
Servo servoMotor;

int maxVelocidade = 130;
bool limitaVelocidade = false;

void setup()
{
    if (Usb.Init() == -1) {
        return;
        while (1); //halt
    }
    SPI.begin();
    mfrc522.PCD_Init();
    servoMotor.attach(servo);
    pinMode(pwm1, OUTPUT);
    pinMode(ini1, OUTPUT);
    pinMode(ini2, OUTPUT);

    pinMode(pwm2, OUTPUT);
    pinMode(ini3, OUTPUT);
    pinMode(ini4, OUTPUT);
    pinMode(led, OUTPUT);
    startApp();
}

void loop()
{
    Usb.Task();
    if (PS3.PS3Connected) {
        /*if (PS3.getButtonClick(PS)) {
             PS3.disconnect();
        }
        */
        int leftAnalog = PS3.getAnalogHat(LeftHatX);
        if (leftAnalog || leftAnalog == 0) {
            moveServo(map(leftAnalog, 0, 255, 0, 180));
        } else {
            moveServo(90);
        }

        moveMotor(0, false);
        int btR2 = PS3.getAnalogButton(R2);
        int btL2 = PS3.getAnalogButton(L2);
        int btX = PS3.getAnalogButton(CROSS);
        int btQ = PS3.getAnalogButton(SQUARE);
        if (btR2) {
            moveMotor(btR2, true);
        }
        if (btL2) {
            moveMotor(btL2, false);
        }
        if (btX) {
            moveMotor(btX, true);
        }
        if (btQ) {
            moveMotor(btQ, false);
        }

        moveMotor2(0, false);
        if (PS3.getButtonPress(LEFT)) {
            //moveServo(map(0, 0, 255, 0, 180));
            moveMotor2(135, true);
            delay(80);
        }
        if (PS3.getButtonPress(RIGHT)) {
            //moveServo(map(255, 0, 255, 0, 180));
            moveMotor2(135, false);
            delay(80);
        }
    }

    if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
        limitaVelocidade = !limitaVelocidade;
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
    }
}


void moveServo(int graus) {
    if (graus < 15) {
        graus = 15;
    }
    servoMotor.write(graus);
}

void moveMotor2(int number, bool forward) {
    analogWrite(pwm1, number);
    if (number == 0) {
        digitalWrite(ini1, HIGH);
        digitalWrite(ini2, HIGH);
    } else if (forward) {
        digitalWrite(ini1, HIGH);
        digitalWrite(ini2, LOW);
    } else {
        digitalWrite(ini1, LOW);
        digitalWrite(ini2, HIGH);
    }
}

void moveMotor(int number, bool forward) {
    if (limitaVelocidade && forward && number > maxVelocidade) {
        number = maxVelocidade;
    }
    analogWrite(pwm2, number);
    if (number == 0) {
        digitalWrite(ini3, HIGH);
        digitalWrite(ini4, HIGH);
    } else if (forward) {
        digitalWrite(ini3, HIGH);
        digitalWrite(ini4, LOW);
    } else {
        digitalWrite(ini3, LOW);
        digitalWrite(ini4, HIGH);
    }
}

void startApp() {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
}

// void ShowReaderDetails() {
//     // Get the MFRC522 software version
//     byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
//     Serial.print(F("MFRC522 Software Version: 0x"));
//     Serial.print(v, HEX);
//     if (v == 0x91)
//         Serial.print(F(" = v1.0"));
//     else if (v == 0x92)
//         Serial.print(F(" = v2.0"));
//     else
//         Serial.print(F(" (unknown)"));
//     Serial.println("");
//     // When 0x00 or 0xFF is returned, communication probably failed
//     if ((v == 0x00) || (v == 0xFF)) {
//         Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
//     }
// }