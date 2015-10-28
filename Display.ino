/*
 * 4 Digit 7 Segment Display
 * darrencperry
 *
 * Original source code:
 * _8x8 LED matrix
 *
 * (c) 2009 BlushingBoy.net
 */


//With this pin setup you can connect pin 1 through 12 on the display to pins 2-13 on Arduino
byte pins[16] = {2,3,4,5,6,7,8,9,10,11,12,13};
byte rows[4] = {13,10,9,7};
byte cols[7] = {
    3,  // D
    2,  // E
    11, // F
    12, // A
    8,  // B
    5,  // C
    6   // G
};

byte numbers[11][7] = {
  {1,1,1,1,1,1,0}, //zero
  {0,0,0,0,1,1,0}, //one
  {1,1,0,1,1,0,1}, //two
  {1,0,0,1,1,1,1}, //three
  {0,0,1,0,1,1,1}, //four
  {1,0,1,1,0,1,1}, //five
  {1,1,1,1,0,1,1}, //six
  {0,0,0,1,1,1,0}, //seven
  {1,1,1,1,1,1,1}, //eight
  {1,0,1,1,1,1,1}, //nine
  {0,0,0,0,0,0,0}  //off
};

void setup() {
  for (int i = 0; i < 16; i++)
    pinMode(pins[i], OUTPUT);

    // digitalWrite(rows[0], HIGH);

    // digitalWrite(cols[0], LOW);
    // digitalWrite(cols[1], LOW);
    // digitalWrite(cols[2], LOW);
    // digitalWrite(cols[3], LOW);
    // digitalWrite(cols[4], HIGH);
    // digitalWrite(cols[5], LOW);
    // digitalWrite(cols[6], LOW);
}

void allOFF() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(rows[i], LOW);
        for (int j; j < 7; j++) {
            digitalWrite(cols[j], HIGH);
        }
    }
}

int digit = 0;
void loop() {
    digit = 0;
    for (int i = 0; i < 4; i++) {
        digitalWrite(rows[i], HIGH);
        for (int k = 0; k < 11; k++) {
            for (int j = 0; j < 7; j++) {
                digit = numbers[k][j];
                if (digit == 1) {
                    digitalWrite(cols[j], LOW);
                } else {
                    digitalWrite(cols[j], HIGH);
                }
            }
            delay(1000);
        }
    }
    allOFF();
}
