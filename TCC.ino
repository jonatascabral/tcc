#include <Servo.h>
#include <PS3BT.h>
#include <PS3USB.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SevSeg.h>

// #ifdef dobogusinclude
// #include <spi4teensy3.h>
// #endif

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
int pinReset = 49,  // RFID RST
    pinSDA = 53,    // RFID SDA(SS)

    pinDirecao = 48, // Servo
    pinFarol1 = 34,  // Farol 1
    pinFarol2 = 35,  // Farol 2

    pinFrente = 2,  // OUT 3
    pinTras = 3;    // OUT 4

byte numDigits = 4, // Numero de digitos do display
    digitPins[] = { // Pinos dos digitos
        36, // D1
        37, // D2
        38, // D3
        39  // D4
    },
    segmentPins[] = {  // Pinos dos segmentos
        40, // A
        41, // B
        42, // C
        43, // D
        44, // E
        45, // F
        46, // G
        47  // dep
    };

uint16_t velocidadeDefault = 100,    // Velocidade maxima para economizar bateria
         velocidadeAtual = 0,
         maxVelocidade = velocidadeDefault;

bool aceso = false;

SevSeg sevseg;
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
BTD Btd(&Usb);
PS3BT PS3(&Btd);

// PS3 Cable mode
// PS3USB PS3(&Usb); // Caso use a versao bluetooth comente esta linha

void mostrarRotacao(int velocidade = 0) {
    sevseg.setNumber(velocidade, 4);
    sevseg.refreshDisplay();
}

void virar(int graus) {
    servoMotor.write(graus);
    delay(10);
}

void frente(uint16_t velocidade) {
    if (velocidade > maxVelocidade) {
        velocidade = maxVelocidade;
    }
    analogWrite(pinTras, 0);
    analogWrite(pinFrente, velocidade);
    mostrarRotacao(velocidade);
    delay(10);
}

void tras(uint16_t velocidade) {
    if (velocidade > maxVelocidade) {
        velocidade = maxVelocidade;
    }
    analogWrite(pinFrente, 0);
    analogWrite(pinTras, velocidade);
    mostrarRotacao(velocidade);
    delay(10);
}

void parar() {
    analogWrite(pinTras, 0);
    analogWrite(pinFrente, 0);
    mostrarRotacao();
    delay(10);
}

void piscarFarol() {
    uint16_t on = HIGH, off = LOW;
    if (aceso) {
        on = off;
        off = HIGH;
    }
    digitalWrite(pinFarol1, on);
    digitalWrite(pinFarol2, on);
    delay(100);
    digitalWrite(pinFarol1, off);
    digitalWrite(pinFarol2, off);
    delay(100);
    digitalWrite(pinFarol1, on);
    digitalWrite(pinFarol2, on);
    delay(100);
    digitalWrite(pinFarol1, off);
    digitalWrite(pinFarol2, off);
}

void farol() {
    uint16_t funcao = HIGH;
    if (aceso) {
        funcao = LOW;
    }
    digitalWrite(pinFarol1, funcao);
    digitalWrite(pinFarol2, funcao);
    aceso = !aceso;
}

/**
 * mfrc522.PICC_IsNewCardPresent() should be checked before
 * @return the card UID
 */
unsigned long getID() {
    if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
        return -1;
    }
    unsigned long hex_num;
    hex_num =  mfrc522.uid.uidByte[0] << 24;
    hex_num += mfrc522.uid.uidByte[1] << 16;
    hex_num += mfrc522.uid.uidByte[2] <<  8;
    hex_num += mfrc522.uid.uidByte[3];
    mfrc522.PICC_HaltA(); // Stop reading
    return hex_num;
}

void limitaVelocidade(unsigned long cardUid) {
    switch (cardUid) {
        case 4294938010:
          maxVelocidade = 160;
          break;
        case 4294938965:
          maxVelocidade = 40;
          break;
        case 4294964565:
          maxVelocidade = 120;
          break;
        case 4294937124:
          maxVelocidade = 180;
          break;
        case 9509:
          maxVelocidade = 90;
          break;
        case 16788:
          maxVelocidade = 60;
          break;
    }
}

void setup() {
    Serial.begin(115200);
    if (Usb.Init() == -1) {
        Serial.write("Erro ao iniciar driver USB");
        while(1);
    }

    // Inicia o display
    sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
    sevseg.setBrightness(10);

    // Start SPI e RFID
    SPI.begin();
    mfrc522.PCD_Init();

    servoMotor.attach(pinDirecao);
    pinMode(pinFarol1, OUTPUT);
    pinMode(pinFarol2, OUTPUT);

    pinMode(pinFrente, OUTPUT);
    pinMode(pinTras,   OUTPUT);

    Serial.write("Setup finalizado");
}

void loop() {
    Usb.Task();
    if (PS3.PS3Connected) {
        if (PS3.getButtonClick(PS)) {
            PS3.disconnect(); // Bluetooth Mode
        }
        // Acelerar Frente e Tras R2 / L2
        uint16_t btR2 = PS3.getAnalogButton(R2);
        uint16_t btL2 = PS3.getAnalogButton(L2);

        // Acelerar Frente e Tras X / Q
        uint16_t btX = PS3.getAnalogButton(CROSS);
        uint16_t btQ = PS3.getAnalogButton(SQUARE);
        if (btR2) {
            frente(btR2);
        } else if (btX) {
            frente(btX);
        } else if (PS3.getButtonPress(R2) || PS3.getButtonPress(CROSS)) {
            frente(velocidadeDefault);
        } else if (btL2) {
            tras(btL2);
        } else if (btQ) {
            tras(btQ);
        } else if (PS3.getButtonPress(L2) || PS3.getButtonPress(SQUARE)) {
            tras(velocidadeDefault);
        } else {
            parar();
        }

        // Virar direcionais
        if (PS3.getButtonPress(LEFT)) {
            virar(map(0, 0, 255, 0, 180));
        } else if (PS3.getButtonPress(RIGHT)) {
            virar(map(255, 0, 255, 0, 180));
        } else {
            // Virar Analogico esquerdo
            uint16_t leftAnalog = PS3.getAnalogHat(LeftHatX);
            if (leftAnalog > 0) {
                virar(map(leftAnalog, 0, 255, 0, 180));
            } else {
                virar(0);
            }
        }

        if (PS3.getButtonClick(R1)) {
            farol();
        }
        if (PS3.getButtonClick(L1)) {
            piscarFarol();
        }

        if (mfrc522.PICC_IsNewCardPresent()) {
            unsigned long uid = getID();
            if(uid != -1){
                limitaVelocidade(uid);
            }
        }
    } else {
        virar(90);
        parar();
    }
    delay(10);
}
