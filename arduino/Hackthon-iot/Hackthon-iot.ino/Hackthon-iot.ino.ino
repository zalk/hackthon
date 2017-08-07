//#include <TimedAction.h>
#include <ArduinoJson.h>


// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.


#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 40
#define BUTTON_Y 100
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 75
#define TEXT_W 220
#define TEXT_H 240
#define TEXT_TSIZE 3
#define TEXT_TCOLOR ILI9341_MAGENTA
#define TEXTPOS_X 10
#define TEXTPOS_Y 75
// the data (phone #) we store in the textfield
#define TEXT_LEN 12
char textfield[TEXT_LEN+1] = "";
uint8_t textfield_i=0;

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define MINPRESSURE 10
#define MAXPRESSURE 1000


#define TS_MINX 100
#define TS_MAXX 920

#define TS_MINY 70
#define TS_MAXY 900
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65

// We have a status line for like, is FONA working
#define KEY_X 100
#define KEY_Y 100

// We have a status line for like, is FONA working
#define INFO_X 90
#define INFO_Y 20

//#include <Wire.h>
#include <UnoWiFiDevEd.h>

#define CONNECTOR "mqtt"
#define TOPIC_UP "/refresh"
#define TOPIC "/accounts/AE3F5"
#define DEVICE_ID "AE3F5"

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

Adafruit_GFX_Button buttons[2];
/* create 15 buttons, in classic candybar phone style */
char buttonlabels[2][5] = {"Key", "UPD." };
uint16_t buttoncolors[2] = { ILI9341_RED,ILI9341_DARKGREEN};



int updateInfo=0;


int acctNumber=0;
char accountNames[5][10];
char balances[5][5];
char lastTra[5][10];

StaticJsonBuffer<200> jsonBuffer;

                             
void setup(void) {
  Ciao.begin();
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();
  uint16_t identifier = 0x9341;
  tft.begin(identifier);
  createButton();

  

}
void loop(void) {

 checkButton();
 
 if(updateInfo ==1){
   Serial.println("updateInfo");
   bankDisplay();
   updateInfo =0;
 }
 
}

void createButton(void){
  uint16_t identifier=0x9341;  
  tft.begin(identifier);

  tft.setRotation(0);
  tft.fillScreen(BLACK);

  buttons[0].initButton(&tft,40, 
                40,    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[0], ILI9341_WHITE,
                  buttonlabels[0], BUTTON_TEXTSIZE);
  buttons[0].drawButton(); 
  buttons[1].initButton(&tft,160, 
                 40,    // x, y, w, h, outline, fill, text
                  120,BUTTON_H, ILI9341_WHITE, buttoncolors[1], ILI9341_WHITE,
                 buttonlabels[1], BUTTON_TEXTSIZE); 
  buttons[1].drawButton();

  
  // create 'text field'
 tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);
 delay(200);
 }

// or charstring
void status(char *msg) {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(msg);
}

void keyDisplay() {
  tft.setRotation(1);
  tft.fillRect(INFO_X, INFO_Y, 220, 190, ILI9341_BLACK);
  //tft.fillRect(KEY_X, KEY_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(KEY_X, KEY_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.print(DEVICE_ID);
  Serial.println("key printed");
  Serial.println(DEVICE_ID);
  delay(3000);
  tft.fillRect(INFO_X, INFO_Y, 190, 190, ILI9341_BLACK);
  tft.setRotation(0);
}

void bankDisplay() {
  //set the display zone
  tft.setRotation(1);
  tft.setCursor(INFO_X, INFO_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(INFO_X, INFO_Y);
  if (acctNumber==0){
    tft.println("Nothing to display");
    tft.setCursor(INFO_X, INFO_Y+30);
    tft.print("Please refresh");  
  }else{
    tft.print("Acct: ");
    tft.println("hardcode");
    tft.setCursor(INFO_X, INFO_Y+30);
    tft.print("bal:");
    tft.println("10");
    tft.setCursor(INFO_X, INFO_Y+60);
    tft.print("last:");
    tft.println("hardcode");
   
  }
   tft.setRotation(0);
}


  
void checkButton(void){
  /*TSPoint p;
  p = ts.getPoint(); 
  */
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

   if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
   }
   
  // go thru all the buttons, checking if they were pressed
  for (uint8_t b=0; b<2; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      //Serial.print("Pressing: "); Serial.println(b);
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b=0; b<2; b++) {
    if (buttons[b].justReleased()) {
      // Serial.print("Released: "); Serial.println(b);
      buttons[b].drawButton();  // draw normal
    }
    
    if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);  // draw invert!
  
      // key button
        if (b == 0) {
          status("Key");
          //fona.hangUp();
          keyDisplay();
          
        }
        
        // refresh
        if (b == 1) {

          Serial.print("refreshing ");
      Serial.print(textfield);
        refresh();
          //fona.callPhone(textfield);
        }
        updateInfo=1;
     
          status('\0');
   
    }

}

}

void refresh(void){
  
    //Ciao.write(CONNECTOR, TOPIC_UP, "{\"action\": \"refresh\", \"DEVICE\": \"AE3F5\"}");
    Ciao.write(CONNECTOR, TOPIC_UP, "{fsfsdf}");
    delay(200); // wait for replay
    receiveAccts();
    
    //screen set up
    
    createButton();
 
}

void receiveAccts(void){
  CiaoData data = Ciao.read(CONNECTOR, TOPIC);
 
    if (!data.isEmpty()){
        String message = data.get(2);
        Serial.println(message);
        JsonObject& root = jsonBuffer.parseObject(message);
        if (!root.success()) {
          Serial.println("parseObject() failed");
          return;
         }
         acctNumber= sizeof(root["account"])/sizeof(root["account"][0]);
     
         

         
     } else{
    Serial.println("no update");
   }
  
}








