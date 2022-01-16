
/******************************************************************************
rgb-plus-buttons.ino
Byron Jacquot @ SparkFun Electronics
1/6/2015

Example to drive the RGB LEDs and scan the buttons of the RGB button pad.

Exercise 3 in a series of 3.
https://learn.sparkfun.com/tutorials/button-pad-hookup-guide/exercise-3-rgb-leds-and-buttons

Development environment specifics:
Developed in Arduino 1.6.5
For an Arduino Mega 2560

This code is released under the [MIT License](http://opensource.org/licenses/MIT).

Distributed as-is; no warranty is given.
******************************************************************************/         
//************  Glove Code Headers **********
#include "MIDIUSB.h"
#include "PitchToNote.h"


// Analog Pins
#define pinBend 39
#define pinAccelX 17
#define pinAccelY 18
#define pinAccelZ 19
#define pinMuxCtl0 15
#define pinMuxCtl1 14

//******* end **********


//config variables
#define NUM_LED_COLUMNS (4)
#define NUM_LED_ROWS (1)
#define NUM_BTN_COLUMNS (4)
#define NUM_BTN_ROWS (1)
#define NUM_COLORS (3)

#define MAX_DEBOUNCE (3)


//*********************** Glove code Variable Definition ******

// Store Bend/Accel values
int bendT = 0;
int bendI = 0;
int bendM = 0;
int bendR = 0;
int accelX = 0;
int accelY = 0;
int accelZ = 0;

// Which finger we're reading
int finger = 0;


#define NUM_BUTTONS  8


const uint16_t pinDPadUp1 = 2;
const uint16_t button2 = 3;
const uint16_t pinDPadRight3 = 4;
const uint16_t button4 = 5;
const uint16_t pinSelect5 = 6;
const uint16_t pinStart6 = 7;
const uint16_t pinB7 = 8;
const uint16_t pinA8 = 9;

const int intensityPot = 0;  //A0 input


const uint16_t buttons[NUM_BUTTONS] = {pinDPadUp1, button2, pinDPadRight3, button4, pinSelect5, pinStart6, pinB7, pinA8};
const byte notePitches[NUM_BUTTONS] = {pitchE7, pitchC7, pitchD7, pitchB6, pitchA4, pitchD4, pitchA6, pitchG6};

uint16_t notesTime[NUM_BUTTONS];
uint16_t pressedButtons = 0x00;
uint16_t previousButtons = 0x00;
uint16_t intensity;


//******************  end ************
//************* varialbes for pad 2 ****
// Global variables
static uint8_t LED_outputs1[NUM_LED_COLUMNS][NUM_LED_ROWS];
static int32_t next_scan1;

static const uint8_t btnselpins1[4]   = {33,34,35,36};
static const uint8_t btnreadpins1[4] = {37};
static const uint8_t ledselpins1[4]   = {26,27,28,29};

// RGB pins for each of 4 rows 
static const uint8_t colorpins1[1][3] = {{30,31,32}};


static int8_t debounce_count1[NUM_BTN_COLUMNS][NUM_BTN_ROWS];

//************* end *********

//********* functions for pad 2 **********


static void setuppins1()
{
    uint8_t i;

    // initialize
    // select lines
    for(i = 0; i < NUM_LED_COLUMNS; i++)
    {
        pinMode(ledselpins1[i], OUTPUT);

        // with nothing selected by default
        digitalWrite(ledselpins1[i], HIGH);
    }

    for(i = 0; i < NUM_BTN_COLUMNS; i++)
    {
        pinMode(btnselpins1[i], OUTPUT);

        // with nothing selected by default
        digitalWrite(btnselpins1[i], HIGH);
    }

    // key return lines
    for(i = 0; i < 4; i++)
    {
        pinMode(btnreadpins1[i], INPUT_PULLUP);
    }

    // LED drive lines
    for(i = 0; i < NUM_LED_ROWS; i++)
    {
        for(uint8_t j = 0; j < NUM_COLORS; j++)
        {
            pinMode(colorpins1[i][j], OUTPUT);
            digitalWrite(colorpins1[i][j], LOW);
        }
    }

    for(uint8_t i = 0; i < NUM_BTN_COLUMNS; i++)
    {
        for(uint8_t j = 0; j < NUM_BTN_ROWS; j++)
        {
            debounce_count1[i][j] = 0;
        }
    }
}

static void scan1()
{
  static uint8_t current = 0;
  uint8_t val;
  uint8_t i, j;

    //run
    digitalWrite(btnselpins1[current], LOW);
    digitalWrite(ledselpins1[current], LOW);
//Serial.println("Low");
    for(i = 0; i < NUM_LED_ROWS; i++)
    {
        uint8_t val = (LED_outputs1[current][i] & 0x03);

        if(val)
        {
            digitalWrite(colorpins1[i][val-1], HIGH);
        }
  }

  delay(1);


  for( j = 0; j < NUM_BTN_ROWS; j++)
  {
    val = digitalRead(btnreadpins1[j]);
//Serial.print(j);
//Serial.print(" = ");
//Serial.println(val);
    if(val == LOW)
    {bitWrite(pressedButtons, current + 4, 1);
      // active low: val is low when btn is pressed
      if( debounce_count1[current][j] < MAX_DEBOUNCE)
      {
        debounce_count1[current][j]++;
        if( debounce_count1[current][j] == MAX_DEBOUNCE )
        {
          Serial.print("Key Down pad 2");
          Serial.println((current * NUM_BTN_ROWS) + j);

          LED_outputs1[current][j]++;
        }
      }
    }
    else
    {bitWrite(pressedButtons, current + 4, 0);
      // otherwise, button is released
      if( debounce_count1[current][j] > 0)
      {
        debounce_count1[current][j]--;
        if( debounce_count1[current][j] == 0 )
        {
          Serial.print("Key Up for pad 2 ");
          Serial.println((current * NUM_BTN_ROWS) + j);
        }
      }
    }
  }// for j = 0 to 3;

  delay(1);

  digitalWrite(btnselpins1[current], HIGH);
  digitalWrite(ledselpins1[current], HIGH);
//Serial.println(" HIGH");
  for(i = 0; i < NUM_LED_ROWS; i++)
  {
    for(j = 0; j < NUM_COLORS; j++)
    {
      digitalWrite(colorpins1[i][j], LOW);
    }
  }

  current++;
  if (current >= NUM_BTN_COLUMNS)
  {
    current = 0;
  }
}


//******************** end ******

// Global variables
static uint8_t LED_outputs[NUM_LED_COLUMNS][NUM_LED_ROWS];
static int32_t next_scan;

static const uint8_t btnselpins[4]   = {7,8,9,10};
static const uint8_t btnreadpins[4] = {25};
static const uint8_t ledselpins[4]   = {3,4,6,5};

// RGB pins for each of 4 rows 
static const uint8_t colorpins[1][3] = {{24,11,12}};


static int8_t debounce_count[NUM_BTN_COLUMNS][NUM_BTN_ROWS];

static void setuppins()
{
    uint8_t i;

    // initialize
    // select lines
    for(i = 0; i < NUM_LED_COLUMNS; i++)
    {
        pinMode(ledselpins[i], OUTPUT);

        // with nothing selected by default
        digitalWrite(ledselpins[i], HIGH);
    }

    for(i = 0; i < NUM_BTN_COLUMNS; i++)
    {
        pinMode(btnselpins[i], OUTPUT);

        // with nothing selected by default
        digitalWrite(btnselpins[i], HIGH);
    }

    // key return lines
    for(i = 0; i < 4; i++)
    {
        pinMode(btnreadpins[i], INPUT_PULLUP);
    }

    // LED drive lines
    for(i = 0; i < NUM_LED_ROWS; i++)
    {
        for(uint8_t j = 0; j < NUM_COLORS; j++)
        {
            pinMode(colorpins[i][j], OUTPUT);
            digitalWrite(colorpins[i][j], LOW);
        }
    }

    for(uint8_t i = 0; i < NUM_BTN_COLUMNS; i++)
    {
        for(uint8_t j = 0; j < NUM_BTN_ROWS; j++)
        {
            debounce_count[i][j] = 0;
        }
    }
}

static void scan()
{
  static uint8_t current = 0;
  uint8_t val;
  uint8_t i, j;

    //run
    digitalWrite(btnselpins[current], LOW);
    digitalWrite(ledselpins[current], LOW);
//Serial.println("Low");
    for(i = 0; i < NUM_LED_ROWS; i++)
    {
        uint8_t val = (LED_outputs[current][i] & 0x03);

        if(val)
        {
            digitalWrite(colorpins[i][val-1], HIGH);
        }
  }

  delay(0);


  for( j = 0; j < NUM_BTN_ROWS; j++)
  {
    val = digitalRead(btnreadpins[j]);
//Serial.print(j);
//Serial.print(" = ");
//Serial.println(val);
    if(val == LOW)
    {bitWrite(pressedButtons, current, 1);
      // active low: val is low when btn is pressed
      if( debounce_count[current][j] < MAX_DEBOUNCE)
      {
        debounce_count[current][j]++;
        if( debounce_count[current][j] == MAX_DEBOUNCE )
        {
          Serial.print("Key Down ");
          Serial.println((current * NUM_BTN_ROWS) + j);

          LED_outputs[current][j]++;
        }
      }
    }
    else
    {bitWrite(pressedButtons, current, 0);
      // otherwise, button is released
      if( debounce_count[current][j] > 0)
      {
        debounce_count[current][j]--;
        if( debounce_count[current][j] == 0 )
        {
          Serial.print("Key Up ");
          Serial.println((current * NUM_BTN_ROWS) + j);
        }
      }
    }
  }// for j = 0 to 3;

  delay(1);

  digitalWrite(btnselpins[current], HIGH);
  digitalWrite(ledselpins[current], HIGH);
//Serial.println(" HIGH");
  for(i = 0; i < NUM_LED_ROWS; i++)
  {
    for(j = 0; j < NUM_COLORS; j++)
    {
      digitalWrite(colorpins[i][j], LOW);
    }
  }

  current++;
  if (current >= NUM_BTN_COLUMNS)
  {
    current = 0;
  }
}

void setup() 
{
  //**************** Glove code setup *******

    for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(pinMuxCtl0, OUTPUT);
    pinMode(pinMuxCtl1, OUTPUT);

    //********* end ********

    //************* setup fo pad 2 ***

    
  // setup hardware
  setuppins1();

  // init global variables
  next_scan1 = millis() + 1;

  for(uint8_t i = 0; i < NUM_LED_ROWS; i++)
  {
    for(uint8_t j = 0; j < NUM_LED_COLUMNS; j++)
    {
      LED_outputs1[i][j] = 0;
    }
  }


//***** end ****
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.print("Starting Setup...");

  // setup hardware
  setuppins();

  // init global variables
  next_scan = millis() + 1;

  for(uint8_t i = 0; i < NUM_LED_ROWS; i++)
  {
    for(uint8_t j = 0; j < NUM_LED_COLUMNS; j++)
    {
      LED_outputs[i][j] = 0;
    }
  }

  Serial.println("Setup Complete.");
}

void loop() {
  //delay(10);
  // put your main code here, to run repeatedly:

  if(millis() >= next_scan)
  {//Serial.println("scan function called");
    next_scan = millis()+1;
    scan();
  }

  //**** loop for pad 2 ****

  
  if(millis() >= next_scan1)
  {//Serial.println("scan function called");
    next_scan1 = millis()+1;
    scan1();
  }

  //***** end ****

  //**** GLove code loop function *****

   switch(finger)
  {
    case 0:
      digitalWrite(pinMuxCtl0, LOW);
      digitalWrite(pinMuxCtl1, LOW);
      delay(10);
      bendT = analogRead(pinBend);
      controlChange(5, 50, map(bendT,300, 330, 0, 127)); //channel 1 (0-15), note = 48 , velocity =64 (64 = normal, 127 = fastest)
      break;
    case 1:
      digitalWrite(pinMuxCtl0, HIGH);
      digitalWrite(pinMuxCtl1, LOW);
      delay(10);
      bendI = analogRead(pinBend);
      controlChange(5, 48, map(bendI,225, 400, 0, 127)); //channel 1 (0-15), note = 48 , velocity =64 (64 = normal, 127 = fastest)
      
      break;
    case 2:
      digitalWrite(pinMuxCtl0, LOW);
      digitalWrite(pinMuxCtl1, HIGH);
      delay(10);
      bendM = analogRead(pinBend);
      controlChange(5, 52, map(bendM,125, 274, 0, 127)); //channel 1 (0-15), note = 48 , velocity =64 (64 = normal, 127 = fastest)
      
      break;
    case 3:
      digitalWrite(pinMuxCtl0, HIGH);
      digitalWrite(pinMuxCtl1, HIGH);
      delay(10);
      bendR = analogRead(pinBend);
      controlChange(5, 53, map(bendR,175, 400, 0, 127)); //channel 1 (0-15), note = 48 , velocity =64 (64 = normal, 127 = fastest)
      
      break;
  }
  
  // increment finger to read
  finger = (finger + 1) % 4;
  
  // read accelerometer
  accelX = analogRead(pinAccelX);
  accelY = analogRead(pinAccelY);
  accelZ = analogRead(pinAccelZ);
 // readButtons();
  playNotes();
  if (accelX>0) {
    controlChange(5, 54, map(accelX,246, 183, 0, 127)); //channel 1 (0-15), note = 48 , velocity =64 (64 = normal, 127 = fastest)
  }
  if (accelY>0) {
     controlChange(5, 55, map(accelY,246, 183, 0, 127)); //channel 1 (0-15), note = 48 , velocity =64 (64 = normal, 127 = fastest)
  }
  if (accelZ>0) {
     controlChange(5, 56, map(accelZ,246, 183, 0, 127)); //channel 1 (0-15), note = 48 , velocity =64 (64 = normal, 127 = fastest)
  }

 //********************* end ********************
}


//*********************  Glove code function definitions *******

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

  

//void readButtons()
//{
  //for (int i = 0; i < NUM_BUTTONS; i++)
  //{
    //if (digitalRead(buttons[i]) == LOW)
    //{
      //bitWrite(pressedButtons, i, 1);
      //delay(2);
   // }
    //else
     // bitWrite(pressedButtons, i, 0);
 // }
//}

void playNotes()
{
  for (int i = 0; i < 8; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitches[i], 100);
        MidiUSB.flush();
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
      }
    }
  }
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// **************** end **********
