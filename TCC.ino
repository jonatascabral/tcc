#include <Servo.h>
#include <PS3BT.h>
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
int pinReset = 49,               // RFID RST
    pinSDA = 53,                 // RFID SDA(SS)

    pinDirecao = 2,             // Servo
    pinLed = 17,                 // Led de alertas

    pinFrente = 3,              // OUT 3
    pinTras = 4;                // OUT 4

uint16_t maxVelocidade = 50,         // Velocidade maxima ao ler a tag @TODO: definir velocidades para cada tag/cartao lido
         velocidadeDefault = 125,    // Velocidade maxima para economizar bateria
         velocidadeDefaultBack = 60;

bool limitaVelocidade = false,  // Define se deve limitar a velocidade
     aceso = false;

Servo servoMotor;
MFRC522 mfrc522(pinSDA, pinReset);

/**
 * USB Host Library
 * Arduino UNO:
 * - Pinos 11 a 13 sao de uso exclusivo da lib SPI
 * - Pino 10 NAO PODE SER USADO, ele eh exclusivo para comunicacao da Shield USB HOST com o Arduino
 */
USB Usb;

/**
 * PS3 Bluetooth mode
 * Como usar:
 * Conecte o adaptador e aguarde 5 seg;
 * Conecte o controle com o cabo, aguarde 5 seg, ligue o controle e desconecte o cabo;
 * Conecte o adaptador novamente;
 * Espere sincronizar;
 */
// BTD Btd(&Usb);
// PS3BT PS3(&Btd);

// PS3 Cable mode
PS3USB PS3(&Usb); // Caso use a versao bluetooth comente esta linha


void virar(int graus) {
    if (graus < 15) {
        graus = 15;
    }
    servoMotor.write(graus);
}

void acelerar(uint16_t velocidade, bool forward = true) {
    if (velocidade > velocidadeDefault && forward) {
        velocidade = velocidadeDefault;
    } else if (velocidade > velocidadeDefaultBack && !forward) {
        velocidade = velocidadeDefaultBack;
    }
    if (limitaVelocidade && forward && velocidade > maxVelocidade) {
        velocidade = maxVelocidade;
    }
    if (forward) {
        analogWrite(pinFrente, velocidade);
    } else {
        analogWrite(pinTras, velocidade);
    }
}

void blink() {
    digitalWrite(pinLed, HIGH);
    delay(100);
    digitalWrite(pinLed, LOW);
    delay(100);
    digitalWrite(pinLed, HIGH);
    delay(100);
    digitalWrite(pinLed, LOW);
}


void setup() {
    Serial.begin(9600);
    if (Usb.Init() == -1) {
        Serial.write("Erro ao iniciar driver USB");
        while(1);
    }

    // Start SPI e RFID
    SPI.begin();
    mfrc522.PCD_Init();

    servoMotor.attach(pinDirecao);
    pinMode(pinLed,      OUTPUT);

    pinMode(pinFrente,   OUTPUT);
    pinMode(pinTras,     OUTPUT);

    // Blink Led, app start OK
    blink();

    Serial.write("Setup finalizado");
}

void loop() {
    Usb.Task();
    if (PS3.PS3Connected) {
        if (PS3.getButtonClick(PS)) {
            // PS3.release(); // USB Mode
            // PS3.disconnect(); // Bluetooth Mode
        }
        // Acelerar Frente e Tras R2 / L2
        uint16_t btR2 = PS3.getAnalogButton(R2);
        uint16_t btL2 = PS3.getAnalogButton(L2);
        acelerar(btR2);
        acelerar(btL2, false);

        // Acelerar Frente e Tras X / Q
        uint16_t btX = PS3.getAnalogButton(CROSS);
        uint16_t btQ = PS3.getAnalogButton(SQUARE);
        acelerar(btX);
        acelerar(btQ, false);

        // Virar direcionais
        if (PS3.getButtonPress(LEFT)) {
            virar(map(0, 0, 255, 0, 180));
            // virar(velocidadeMotor, 1);
            // delay(80);
        } else if (PS3.getButtonPress(RIGHT)) {
            virar(map(255, 0, 255, 0, 180));
            // virar(velocidadeMotor, 2);
            // delay(80);
        } else {
            // Virar Analogico esquerdo
            uint16_t leftAnalog = PS3.getAnalogHat(LeftHatX);
            virar(map(leftAnalog, 0, 255, 0, 180));
            // virar(leftAnalog, 3);
        }


        if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
            limitaVelocidade = !limitaVelocidade;
            blink();
            delay(700);
        }
        delay(40);

        if (limitaVelocidade && !aceso) {
            digitalWrite(pinLed, HIGH);
            aceso = true;
        } else if (!limitaVelocidade && aceso) {
            digitalWrite(pinLed, LOW);
            aceso = false;
        }
    }
}