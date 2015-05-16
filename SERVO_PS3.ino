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
//USBHub Hub1(&Usb); // Some dongles have a hub inside

//BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two wa\ys */
//PS3BT PS3(&Btd); // This will just create the instance
//PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x0C, 0xBF, 0xEB); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch
PS3USB PS3(&Usb);

byte led = 4;
byte servo = 6;
boolean ligado = false, manterPiscando = false;
Servo motor;


void setup()
{
    Serial.begin(115200);
    pinMode(led, OUTPUT);
    motor.attach(servo);
    if (Usb.Init() == -1) {
        digitalWrite(led, HIGH);
        return;
        while (1); //halt
    }
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
}

void loop()
{
    Usb.Task();
    motor.write(90);
    if (PS3.PS3Connected) {
        if (PS3.getButtonClick(CROSS)) {
            if (!ligado) {
                digitalWrite(led, HIGH);
            } else {
                digitalWrite(led, LOW);
            }
            ligado = !ligado;
        }
        if (PS3.getButtonClick(CIRCLE)) {
            digitalWrite(led, LOW);
            ligado = false;
        }
        if (PS3.getButtonClick(PS)) {
            //PS3.disconnect();
        }
        if (PS3.getAnalogHat(LeftHatX) > 180) {
            motor.write(map(PS3.getAnalogHat(LeftHatX), 0, 255, 0, 180));
            delay(30);
	}
        if (PS3.getAnalogHat(LeftHatX) < 45) {
            int graus = map(PS3.getAnalogHat(LeftHatX), 0, 255, 0, 180);
            if (graus < 15) {
                graus = 15;
            }
            motor.write(graus);
            delay(30);
	}
    }
}

