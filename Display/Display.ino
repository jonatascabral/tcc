/*
 * 4 Digit 7 Segment Display
 * darrencperry
 *
 * Original source code:
 * _8x8 LED matrix
 *
 * (c) 2009 BlushingBoy.net
 */

#define PINS 13
#define ROWS 4
#define COLS 7
#define NUMBERS 10

byte pins[PINS] = {2,3,4,5,6,7,8,9,10,11,12,13};
byte rows[ROWS] = {13,10,9,7};
byte cols[COLS] = {
    3,  // D
    2,  // E
    11, // F
    12, // A
    8,  // B
    5,  // C
    6   // G
};
uint8_t on_off[2] = {HIGH, LOW};

byte numbers[NUMBERS][COLS] = {
    {1,1,1,1,1,1,0}, // zero
    {0,0,0,0,1,1,0}, // one
    {1,1,0,1,1,0,1}, // two
    {1,0,0,1,1,1,1}, // three
    {0,0,1,0,1,1,1}, // four
    {1,0,1,1,0,1,1}, // five
    {1,1,1,1,0,1,1}, // six
    {0,0,0,1,1,1,0}, // seven
    {1,1,1,1,1,1,1}, // eight
    {1,0,1,1,1,1,1}  // nine
};
int rotations = 60;
String rotationsStr;

void setup() {
    for (int i = 0; i < ROWS; i++) {
        pinMode(rows[i], OUTPUT);
    }
    for (int i = 0; i < COLS; i++) {
        pinMode(cols[i], OUTPUT);
    }
}

void allOFF() {
    for (int i = 0; i < ROWS; i++) {
        digitalWrite(rows[i], HIGH);
    }
    for (int i = 0; i < COLS; i++) {
        digitalWrite(cols[i], HIGH);
    }
}

void loop() {
    rotations += 10;
    rotationsStr = String(rotations);
    char rotationsChr[rotationsStr.length() + 1];

    rotationsStr.toCharArray(rotationsChr, rotationsStr.length());
    int k = 0;
    for (int i = rotationsStr.length() - 1; i >= 0; i--) {
        int digit = (int) rotationsChr[i];
        digitalWrite(rows[k], HIGH);
        for(int j=0; j<COLS; j++){
            digitalWrite(cols[j], on_off[ numbers[digit][j] ]);
        }
        ++k;
    }
    delay(1000);
    if (rotations > 200) {
      rotations = 50;
    }
    // allOFF();
}
