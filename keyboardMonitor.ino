

/*aalight led lighting effects for FastLED.

      By: Andrew Tuline
     URL: www.tuline.com
   Email: atuline@gmail.com
  GitHub: https://github.com/atuline
Pastebin: http://pastebin.com/u/atuline
 Youtube: https://www.youtube.com/user/atuline/videos


Date: March, 2015


Introduction:

These effects/visualizations are designed for a single strand/loop of individually addressable LED's using the FastLED library.

Several modes of input are supported, such as local and remote IR support, a debounced button as well as keyboard. This also support for a demo mode.

This is built upon code from several sources, but mainly:

- FastLED library and some examples as found at http://fastled.io
- funkboxing LED for general layout as found at http://funkboxing.com/wordpress/?p=1366
- IR programmability as found at https://github.com/NicoHood/IRLremote


Hardware Setup

- WS2812B or (preferably) APA102 LED strip with data line connected to pin 12. 


Libraries Required

- FastLED library (get 3.1) from https://github.com/FastLED/FastLED

LED Strand Configuration

Keyboard Operation

Once you have compiled the source code, you can open up the monitor and enter keyboard commands. Keyboard commands in the monitor mode include:


Key   Command                         Do I use this
---   -------                         -------------
;     Break                               no
a     Set all to one color by hsv (0-255) yes
b     Set maximum brightness (0-255)      yes
c     clear strip                         no
d     Set delay variable (0-??)           yes
e     Increase/decrease delay (1/2)       yes
h     Set hue variable (0-255)            yes
l     Set single LED value rgb            no  (commented out)
m     Set mode (0-99)                     yes
n     Reverse/forward direction (1/2)     yes
o     Decrease/increase mode (1/2)        yes
Q     Return version number               no
r     Decrease/increase brightness (1/2)  yes
s     Set step variable (0-?)             yes
t     Set saturation variable (0-?)       yes
u     Decrease/increase hue (1/2)         yes
v     Set vitesse turbo -25/25       no (commented out)
y     set of LED's in display and save    Not yet implemented
z     Show LED's                          no



Examples:

m5              // Select mode 5 (with or without spaces separating the command and the argument)
h 80            // Select a hue of 80, which will change the hue of some of the modes
*/


/*------------------------------------------------------------------------------------------
--------------------------------------- Start of variables ---------------------------------
------------------------------------------------------------------------------------------*/


#define qsubd(x, b)  ((x>b)?wavebright:0)                     // A digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Unsigned subtraction macro. if result <0, then => 0.

#define AALIGHT_VERSION 100

#include "FastLED.h"                                          // FastLED library. Preferably the latest copy of FastLED 2.1.
#include <LiquidCrystal.h>                                    // library LCD display

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Fixed definitions cannot change on the fly.
#define LED_DT 6                                             // Serial data pin for WS2801, WS2811, WS2812B or APA102
//#define LED_CK 11                                             // Serial clock pin for WS2801 or APA102.
#define COLOR_ORDER GRB                                       // It's GRB for WS2812B
//#define LED_TYPE APA102
 #define LED_TYPE WS2812B                                      // What kind of strip are you using?
#define NUM_LEDS 300                                            // Number of LED's.

// Initialize changeable global variables.
uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.


int ledMode = 0;                                             // Starting mode is typically 0. Use 99 if no controls available. ###### CHANGE ME #########
int maxMode;                                                  // Maximum number of modes is set later.

// Serial definition
#define SERIAL_BAUDRATE 57600                                 // Or 115200.
#define SERIAL_TIMEOUT 5

byte inbyte;                                                  // Serial input byte
int thisarg;                                                  // Serial input argument


// Generic variables
uint8_t thisdelay = 0;                                        // Standard delay
uint8_t thishue = 0;                                          // Standard hue
uint8_t thissat = 255;                                        // Standard saturation
int thisbright = 0;                                           // Standard brightness
uint8_t thisfade = 224;                                         // Standard fade rate
bool thisdir = 0;                                             // Standard direction

/*
//TurboRed variables
uint8_t vitesse = 25;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
uint8_t startIndex = 1;
extern const CRGBPalette16 RedYelOra;
extern const TProgmemRGBPalette16 RedYelOra_p PROGMEM;
*/
// Generic variables
                                            // Standard direction


// Note that commented out variable are repeated.

// Matrix variables

bool huerot = 0;
uint8_t bgclr = 0;
uint8_t bgbri = 0;

// Two_sin variables-------------------------------------------------------------------------
uint8_t wavebright = 128;                                     // You can change the brightness of the waves/bars rolling across the screen. Best to make them not as bright as the sparkles.
//uint8_t thishue = 0;                                          // You can change the starting hue value for the first wave.
uint8_t thathue = 140;                                        // You can change the starting hue for other wave.
uint8_t thisrot = 0;                                          // You can change how quickly the hue rotates for this wave. Currently 0.
uint8_t thatrot = 0;                                          // You can change how quickly the hue rotates for the other wave. Currently 0.
uint8_t allsat = 255;                                         // I like 'em fully saturated with colour.
//bool thisdir = 0;                                             // A direction variable that will work with inputs.
int8_t thisspeed = 4;                                         // You can change the speed, and can use negative values.
int8_t thatspeed = 4;                                         // You can change the speed, and use negative values.
uint8_t allfreq = 32;                                         // You can change the frequency, thus overall width of bars.
int thisphase = 0;                                            // Phase change value gets calculated.
int thatphase = 0;                                            // Phase change value gets calculated.
uint8_t thiscutoff = 192;                                     // You can change the cutoff value to display this wave. Lower value = longer wave.
uint8_t thatcutoff = 192;                                     // You can change the cutoff value to display that wave. Lower value = longer wave.


// Twinkle variables -----------------------------------------------------------------------
int twinkrate = 100;


// One_sin variables -----------------------------------------------------------------------

// bool thisdir = 0;
// uint8_t bgclr = 0;
// uint8_t bgbri = 0;


// pop_fade variables ----------------------------------------------------------------------
int ranamount = 299;                                           // The higher the number, lowers the chance for a pixel to light up.
// uint8_t thisdelay = 50;                                       // Standard delay value.
bool boolcolours = 1;                                         // True = random, False = array of colours (up to 10)
uint8_t numcolours = 2;                                       // Number of colours in the array to display.
unsigned long colours[10] = {0xff0000, 0x00ff00, 0x0000ff, 0xffffff};  // Just assign the first 3 of these selectable colours.
uint8_t maxbar = 300;                                           // Maximum # of pixels to turn on in a row
uint8_t fadeval = 224;                                        // Fade rate


// three_sin variables ---------------------------------------------------------------------

int wave1=0;
int wave2=0;
int wave3=0;

uint8_t inc1 = 2;
uint8_t inc2 = 1;
uint8_t inc3 = -3;

uint8_t lvl1 = 80;
uint8_t lvl2 = 80;
uint8_t lvl3 = 80;

uint8_t mul1 = 20;
uint8_t mul2 = 25;
uint8_t mul3 = 22;

// rainbow_march variables -----------------------------------------------------------------

uint8_t deltahue = 1;                                         // Hue change between pixels.


// noise16 variables -----------------------------------------------------------------------

//uint8_t thisdelay = 20;                                       // A delay value for the sequence(s)
// bool thisdir = 0;                                          // We can reverse the direction.
uint32_t  x,hue_time;                                         // x & hue_time values
uint8_t octaves=2;       //2                                  // how many octaves to use for the brightness
uint8_t hue_octaves=3;   //3                                  // how many octaves to use for the hue
int xscale=57771;        //57771                              // the 'distance' between points on the x axis
uint32_t hxy = 43213;    //43213                              // not sure about this
int hue_scale=20;        //1                                  // the 'distance' between points for the hue noise
int hue_speed = 1;       //31                                 // how fast we move through hue noise
uint8_t x_speed = 0;     //331                                // adjust this value to move along the x axis between frames
int8_t hxyinc = 3;       //3

// Note that commented out variable are repeated.

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);                        // setup LCD display


// Display functions
#include "matrix.h"
#include "noise16.h"
#include "one_sin.h"
#include "pop_fade.h"
#include "rainbow_march.h"
#include "three_sin.h"
#include "twinkle.h"
#include "two_sin.h"
#include "confetti.h"
#include "sinelon.h"
#include "juggle.h"
#include "demo_modeA.h"
/*------------------------------------------------------------------------------------------
--------------------------------------- Start of code --------------------------------------
------------------------------------------------------------------------------------------*/

void setup() {

  Serial.begin(SERIAL_BAUDRATE);                              // SETUP HARDWARE SERIAL (USB)
  Serial.setTimeout(SERIAL_TIMEOUT);

  
  LEDS.setBrightness(max_bright);                             // Set the generic maximum brightness value.

  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER >(leds, NUM_LEDS); // WS2812B definition
  

  Serial.println("---SETUP COMPLETE---");
  change_mode(ledMode, 1);                                    // Initialize the first sequence
} // setup()



//------------------MAIN LOOP---------------------------------------------------------------
void loop() {
  
  readkeyboard();                                             // Get keyboard input.
  change_mode(ledMode, 0);                                    // Strobe, don't set it.
  show_at_max_brightness_for_power();                         // Power managed display of LED's.
  delay_at_max_brightness_for_power(2.5*thisdelay);           // Power managed FastLED delay.

  //Connexion potards avec propriétés
  thishue = map(analogRead(A0), 0, 1023,0,255);
  thissat = map(analogRead(A1), 0,1023,0,255);
  max_bright = map(analogRead(A2), 0, 1023, 0,255);

  //Affichage valeurs sur écran.
  lcd.print(" H");
  lcd.print(thishue);
  lcd.print(" S"); 
  lcd.print(thissat);
  lcd.print(" B");
  lcd.print(max_bright); 
 
  lcd.print("   ");
  //lcd.setCursor(0, 1);
  
  //lcd.print(" Sat:"); 
  //lcd.print(sat);
  //lcd.print("   ");
  //delay(5);
  lcd.home();

  
//  Serial.println(LEDS.getFPS());                            // Display frames per second in the serial monitor. Disable the delay in order to see how fast/efficient your sequence is.
} // loop()


void change_mode(int newMode, int mc){                        // mc stands for 'Mode Change', where mc = 0 is strobe the routine, while mc = 1 is change the routine

   maxMode = 41;
   if(mc) fill_solid(leds,NUM_LEDS,CRGB(0,0,0));              // Clean up the array for the first time through. Don't show display though, so you may have a smooth transition.

  switch (newMode) {                                          // First time through a new mode, so let's initialize the variables for a given display.

 case  0: if(mc) {fill_solid(leds,NUM_LEDS,CRGB(0,0,0)); LEDS.show();} LEDS.show(); break;              // All off, not animated.
    case  1: if(mc) {fill_solid(leds, NUM_LEDS,CRGB(255,255,255)); LEDS.show();} LEDS.show(); break;              // All on, not animated.
    case  2: if(mc) {thisdelay=20; twinkrate=NUM_LEDS; thishue=0; thissat=255; thisbright=255; thisfade=64; } twinkle(); break;
    case  3: if(mc) {thisdelay=10; thisrot=1; thatrot=1;} two_sin(); break;
    case  4: if(mc) {thisdelay=10; thisrot=0; thisdir=1;} two_sin(); break;
    case  5: if(mc) {thisdelay=10; thatrot=0; thishue=255; thathue=255;} two_sin(); break;
    case  6: if(mc) {thisdelay=10; allfreq=16; thathue=128;} two_sin(); break;
    case  7: if(mc) {thisdelay=10; thiscutoff=96; thishue=196; thatcutoff=240;} two_sin(); break;
    case  8: if(mc) {thisdelay=10; thiscutoff=96; thatcutoff=96; thisrot=1;} two_sin(); break;
    case  9: if(mc) {thisdelay=10; thisspeed= -4; thatspeed= -4;} two_sin(); break;
    case 10: if(mc) {thisdelay=10; thiscutoff=128; thatcutoff=128; wavebright=64;} two_sin(); break;
    case 11: if(mc) {thisdelay=10; wavebright=128; thisspeed=3;} two_sin(); break;
    case 12: if(mc) {thisdelay=10; thisspeed=3; thatspeed=-3;} two_sin(); break;
    case 13: if(mc) {thisdelay=30; thishue=95; bgclr=100; bgbri=10;} matrix(); break;
    case 14: if(mc) {thisdelay=30; thishue=40; thisdir=1; bgclr=75;} matrix(); break;
    case 15: if(mc) {thisdelay=30; thishue=random8(); huerot=1; bgbri=0;} matrix(); break;
    case 16: if(mc) {thisdelay=20; thisrot=1; thiscutoff=254; allfreq=8; bgclr=0; bgbri=10;} one_sin(); break;
    case 17: if(mc) {thisdelay=20; thisrot=0; bgclr=50;} one_sin(); break;
    case 18: if(mc) {thisdelay=20; thishue=255; bgclr=100; bgbri=40;} one_sin(); break;
    case 19: if(mc) {thisdelay=20; allfreq=16; bgbri=0;} one_sin(); break;
    case 20: if(mc) {thisdelay=20; thiscutoff=96; bgclr=200; bgbri=20;} one_sin(); break;
    case 21: if(mc) {thisdelay=20; thiscutoff=128; wavebright=64;} one_sin(); break;
    case 22: if(mc) {thisdelay=40; colours[0]=0xffffff; numcolours=1; boolcolours=0; maxbar=1;} pop_fade(); break;
    case 23: if(mc) {thisdelay=40; colours[1]=0xff0000; numcolours=2; boolcolours=0; maxbar=4;} pop_fade(); break;
    case 24: if(mc) {thisdelay=40; fadeval=192;} pop_fade(); break;
    case 25: if(mc) {thisdelay=40; boolcolours=1; maxbar=1;} pop_fade(); break;
    case 26: if(mc) {thisdelay=40; fadeval=128;} pop_fade(); break;
      if(mc) {thisdelay=40; colours[2]= 0x0000ff; boolcolours=0; numcolours=3; fadeval=192; maxbar=6;} pop_fade(); break;
    case 28: if(mc) {thisdelay=20; mul1=20; mul2=25; mul3=22;} three_sin(); break;
    case 29: if(mc) {thisdelay=20; mul1=5; mul2=8; mul3=7;} three_sin(); break;
    case 30: if(mc) {thisdelay=20; lvl1=220; lvl2=220; lvl3=220;} three_sin(); break;
    case 31: if(mc) {thisdelay=10; thisrot=1; deltahue=5;} rainbow_march(); break;
    case 32: if(mc) {thisdelay=10; thisdir=-1; deltahue=10;} rainbow_march(); break;
    case 33: if(mc) {thisdelay=10; thisrot=5;} rainbow_march(); break;
    case 34: if(mc) {thisdelay=10; thisrot=5; thisdir=-1; deltahue=20;} rainbow_march(); break;
    case 35: if(mc) {thisdelay=10; deltahue=30;} rainbow_march(); break;
    case 36: if(mc) {thisdelay=10; deltahue=2; thisrot=5;} rainbow_march(); break;
    case 37: if(mc) {thisdelay=20; octaves=1; hue_octaves=2; hxy=6000; x=5000; xscale=3000; hue_scale=50; hue_speed=15; x_speed=100;} noise16(); break;
    case 38: if(mc) {thisdelay=20; hxyinc = random16(1,15); octaves=random16(1,3); hue_octaves=random16(1,5); hue_scale=random16(10, 50);  x=random16(); xscale=random16(); hxy= random16(); hue_time=random16(); hue_speed=random16(1,3); x_speed=random16(1,30);} noise16(); break;
    case 39: if(mc) {thisdelay=20; thishue = 20;} confetti(); break;
    case 40: if(mc) {thisdelay=20; thishue = 50;} sinelon(); break;
    case 41: if(mc) {thisdelay=10;} juggle(); break;

    // DEMO MODE
    case 99: demo_modeA(); break;

  } // switch newMode

  ledMode = newMode;
} // change_mode()


//----------------- Hardware Support Functions ---------------------------------------------






void readkeyboard() {                                         // PROCESS HARDWARE SERIAL COMMANDS AND ARGS
  while (Serial.available() > 0) {
  
    inbyte = Serial.read();                                   // READ SINGLE BYTE COMMAND
    Serial.print("Serial read is: ");
    Serial.println(inbyte);
    switch(inbyte) {

      case 59: break;                                         // BREAK IF INBYTE = ';'

      case 97:                                                // "a" - SET ALL TO ONE COLOR BY HSV 0-255
        thisarg = Serial.parseInt();
        thissat = 255;
        thisbright = 255;
        fill_solid_HSV(thisarg, thissat, thisbright);
        break;

      case 98:                                                // "b" - SET MAX BRIGHTNESS to #
        max_bright = Serial.parseInt();
        LEDS.setBrightness(max_bright);
        break;

      case 99:                                                // "c" - CLEAR STRIP
        fill_solid(leds,NUM_LEDS,CRGB(0,0,0));
        break;

      case 100:                                               // "d" - SET DELAY VAR to #
        thisarg = Serial.parseInt();
        thisdelay = thisarg;
        break;

      case 101:                                               // "e1 or e2" - DECREASE OR INCREASE delay by 1
        thisarg = Serial.parseInt();
        if (thisarg == 1) thisdelay--; else thisdelay++;
        if (thisdelay < 0 ) thisdelay = 0;
        break;

      case 104:                                               // "h" - SET HUE VAR to #
        thisarg = Serial.parseInt();
        thishue = thisarg;
        break;

      case 109:                                               // "m" - SET MODE to #
        thisarg = Serial.parseInt();
        change_mode(thisarg,maxMode );
        break;

      case 110:                                               // "n0 or n1" - DIRECTION is 0 or 1
        thisarg = Serial.parseInt();
        if (thisarg <=1 && thisarg >=0 ) thisdir = thisarg;
        break;

      case 111:                                               // "o1 or o2" - DECREASE OR INCREASE MODE by 1
        thisarg = Serial.parseInt();
        if (thisarg == 1) ledMode--;
        if (thisarg == 2) ledMode++;
        if (ledMode < 0) ledMode = 0;
        change_mode(ledMode, maxMode);
        break;

      case 113:                                               // "q" - DISPLAY VERSION NUMBER
        Serial.print(AALIGHT_VERSION);
        break;

      case 114:                                               // "r1 or r2" - DECREASE OR INCREASE BRIGHTNESS by / or * 2
        thisarg = Serial.parseInt();
        if (thisarg == 1) max_bright=max_bright/2; else max_bright=max_bright*2;
        max_bright = constrain(max_bright, 1, 255);
        LEDS.setBrightness(max_bright);
        show_at_max_brightness_for_power();
        break;

      case 116:                                               // "t" - SET SATURATION VAR to #
        thisarg = Serial.parseInt();
        thissat = thisarg;
        break;

      case 117:                                               // "u1 or u2" - DECREASE OR INCREASE HUE by 8
        thisarg = Serial.parseInt();                          // Only works in some CHSV modes
        if (thisarg == 1) thishue=thishue-8; else thishue=thishue+8;
        show_at_max_brightness_for_power();
        break;
        
      /*case 119:                                               // "v" v "-25" v "25" vitesse turbo 
        thisarg = Serial.parseInt();
        vitesse = thisarg;
        break;*/

      case 122:                                               // "z" - COMMAND TO 'SHOW' LEDS
        show_at_max_brightness_for_power();
        break;
    } // switch inbyte
  } // while Serial.available
} // readkeyboard()



//---------------------- LED Utility Functions ---------------------------------------------
 
int wrap(int step) {
  if(step < 0) return NUM_LEDS + step;
  if(step > NUM_LEDS - 1) return step - NUM_LEDS;
  return step;
} // wrap()



void fill_solid_HSV(uint8_t ahue, uint8_t asat, uint8_t abright) {  // Set all LED's to an HSV value.
  for(int i = 0 ; i < NUM_LEDS; i++ ) {
    leds[i] = CHSV(ahue, asat, abright);
  }
}  // fill_solid_HSV()

  


/*
//ajout de la fonction turbo
void TurboRouge(uint8_t vitesse) 
{
  currentPalette = RedYelOra_p;
  currentBlending = NOBLEND;
  
  startIndex = startIndex+vitesse;  
  FillLEDsFromPaletteColors(startIndex);    

  FastLED.show();
  //FastLED.delay(1000 / UPDATES_PER_SECOND);
}


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    
    
    for( int i = 0; i < 150; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, thisbright, currentBlending);
        leds[300-i] = ColorFromPalette( currentPalette, colorIndex, thisbright, currentBlending);
        colorIndex += 1;
    }
}



const TProgmemRGBPalette16 RedYelOra_p PROGMEM =
{
   0xD90000,
   0xD90000,
   0xD90000,
   0xD90000,
   0xD90000,

   0xFF4500,
   0xFF4500,
   0xFF4500,
   0xFF4500,
   0xFF4500, 
   0xFF4500,
  
   0xffff00,
   0xffff00,
   0xffff00,
   0xffff00,
   0xffff00
};
//TurboRouge()
*/
