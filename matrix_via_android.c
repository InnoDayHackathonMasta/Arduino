// The source for the Android application can be found at the following link: https://github.com/Lauszus/ArduinoBlinkLED
// The code for the Android application is heavily based on this guide: http://allaboutee.com/2011/12/31/arduino-adk-board-blink-an-led-with-your-phone-code-and-explanation/ by Miguel

#include <Usb.h>
#include <adk.h>

// defines 5x7 ascii characters 0x20-0x7F (32-127)
unsigned char font[59][5] = {
        {0x00,0x00,0x00,0x00,0x00}, //
        {0x00,0x00,0xfa,0x00,0x00}, // !
        {0x00,0xe0,0x00,0xe0,0x00}, // "
        {0x28,0xfe,0x28,0xfe,0x28}, // #
        {0x24,0x54,0xfe,0x54,0x48}, // $
        {0xc4,0xc8,0x10,0x26,0x46}, // %
        {0x6c,0x92,0xaa,0x44,0x0a}, // &
        {0x00,0xa0,0xc0,0x00,0x00}, // '
        {0x00,0x38,0x44,0x82,0x00}, // (
        {0x00,0x82,0x44,0x38,0x00}, // )
        {0x10,0x54,0x38,0x54,0x10}, // *
        {0x10,0x10,0x7c,0x10,0x10}, // +
        {0x00,0x0a,0x0c,0x00,0x00}, // ,
        {0x10,0x10,0x10,0x10,0x10}, // -
        {0x00,0x06,0x06,0x00,0x00}, // .
        {0x04,0x08,0x10,0x20,0x40}, // /
        {0x7c,0x8a,0x92,0xa2,0x7c}, // 0
        {0x00,0x42,0xfe,0x02,0x00}, // 1
        {0x42,0x86,0x8a,0x92,0x62}, // 2
        {0x84,0x82,0xa2,0xd2,0x8c}, // 3
        {0x18,0x28,0x48,0xfe,0x08}, // 4
        {0xe4,0xa2,0xa2,0xa2,0x9c}, // 5
        {0x3c,0x52,0x92,0x92,0x0c}, // 6
        {0x80,0x8e,0x90,0xa0,0xc0}, // 7
        {0x6c,0x92,0x92,0x92,0x6c}, // 8
        {0x60,0x92,0x92,0x94,0x78}, // 9
        {0x00,0x6c,0x6c,0x00,0x00}, // :
        {0x00,0x6a,0x6c,0x00,0x00}, // ;
        {0x00,0x10,0x28,0x44,0x82}, // <
        {0x28,0x28,0x28,0x28,0x28}, // =
        {0x82,0x44,0x28,0x10,0x00}, // >
        {0x40,0x80,0x8a,0x90,0x60}, // ?
        {0x4c,0x92,0x9e,0x82,0x7c}, // @
        {0x7e,0x88,0x88,0x88,0x7e}, // A
        {0xfe,0x92,0x92,0x92,0x6c}, // B
        {0x7c,0x82,0x82,0x82,0x44}, // C
        {0xfe,0x82,0x82,0x44,0x38}, // D
        {0xfe,0x92,0x92,0x92,0x82}, // E
        {0xfe,0x90,0x90,0x80,0x80}, // F
        {0x7c,0x82,0x82,0x8a,0x4c}, // G
        {0xfe,0x10,0x10,0x10,0xfe}, // H
        {0x00,0x82,0xfe,0x82,0x00}, // I
        {0x04,0x02,0x82,0xfc,0x80}, // J
        {0xfe,0x10,0x28,0x44,0x82}, // K
        {0xfe,0x02,0x02,0x02,0x02}, // L
        {0xfe,0x40,0x20,0x40,0xfe}, // M
        {0xfe,0x20,0x10,0x08,0xfe}, // N
        {0x7c,0x82,0x82,0x82,0x7c}, // O
        {0xfe,0x90,0x90,0x90,0x60}, // P
        {0x7c,0x82,0x8a,0x84,0x7a}, // Q
        {0xfe,0x90,0x98,0x94,0x62}, // R
        {0x62,0x92,0x92,0x92,0x8c}, // S
        {0x80,0x80,0xfe,0x80,0x80}, // T
        {0xfc,0x02,0x02,0x02,0xfc}, // U
        {0xf8,0x04,0x02,0x04,0xf8}, // V
        {0xfe,0x04,0x18,0x04,0xfe}, // W
        {0xc6,0x28,0x10,0x28,0xc6}, // X
        {0xc0,0x20,0x1e,0x20,0xc0}, // Y
        {0x86,0x8a,0x92,0xa2,0xc2}, // Z
};

// Col = Spalte!
int active_col = 0;
byte leds[5][7];

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[13]= {-1, 14, 15, 16, 17, 18, 2, 3, 4, 5, 6, 7, 8};

// col[xx] of leds = pin yy on led matrix
int cols[5] = {pins[1], pins[2], pins[3], pins[4], pins[5]};

// row[xx] of leds = pin yy on led matrix
int rows[8] = {pins[6], pins[7], pins[8], pins[9], pins[10], pins[11], pins[12], pins[13]};

USB Usb;
ADK adk(&Usb,"JoeKickass", // Manufacturer Name
             "AndroidApkTakeTwo", // Model Name
             "ADK example application", // Description (user-visible string)
             "1.0", // Version
             "http://joekickass.se", // URL (web page to visit if no installed apps support the accessory)
             "0000000012345678"); // Serial Number (optional)

void clearLeds() {
  // Clear display array
  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 7; y++) {
      leds[x][y] = 0;
    }
  }
}

void setPattern(int charnum) {
  
  charnum = charnum -32;
  
  unsigned char shiftedchar;
  
  Serial.print("\r\nCurrent char=");
  Serial.print( charnum );
  
  for (int x = 0; x < 5; x++) {
    unsigned char currentcharatcol = font[charnum][x];
    Serial.print(currentcharatcol);
    for (int y = 0; y < 7; y++) {
      shiftedchar = currentcharatcol << y;
      shiftedchar = shiftedchar & 0x80;
      if (shiftedchar == 0) {
        leds[x][6-y]=0;
      } else {
        leds[x][6-y]=1;
      } 
      Serial.print("\r\nledarray ");
      Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      Serial.print(leds[x][y]); 
    }
  }
}

// reads out led array and displays it on the screen
void refresh_display() {
  
  digitalWrite(cols[active_col], LOW);  // Turn whole previous column off
  
  //Serial.print("\r\nActivel Col:");
  
  active_col++;
  if (active_col > 4) {
    active_col = 0;
  }
  
//    Serial.print(active_col);

  
  for (int y = 0; y < 8; y++) {
    if (leds[active_col][y] != 0) {
      digitalWrite(rows[y], HIGH);  // Turn on this led
    }
    else {
      digitalWrite(rows[y], LOW); // Turn off this led
    }
  }
  digitalWrite(cols[active_col], HIGH); // Turn whole column on at once (for equal lighting times)
}


void setup()
{
  Serial.begin(115200);
  Serial.print("\r\nHackathon ADK Start");
  if (Usb.Init() == -1) {
    Serial.print("\r\nOSCOKIRQ failed to assert");
    while(1); //halt
  }

  // Init LEDs as output
  for (int i = 1; i <= 12; i++) {
  pinMode(pins[i], OUTPUT);
  }

  // init cols and rows with LOW
  for (int i = 0; i < 5; i++) {
    digitalWrite(cols[i], LOW);
  }

  for (int i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW);
  }

  clearLeds();
  
  leds[0][6] = 0;
  leds[0][5] = 1;
  leds[0][4] = 1;
  leds[0][3] = 1;
  leds[0][2] = 1;
  leds[0][1] = 1;
  leds[0][0] = 0;

  leds[1][6] = 1;
  leds[1][5] = 0;
  leds[1][4] = 0;
  leds[1][3] = 0;
  leds[1][2] = 0;
  leds[1][1] = 0;
  leds[1][0] = 1;
  
  leds[2][6] = 1;
  leds[2][5] = 0;
  leds[2][4] = 0;
  leds[2][3] = 1;
  leds[2][2] = 0;
  leds[2][1] = 0;
  leds[2][0] = 1;
  
  leds[3][6] = 1;
  leds[3][5] = 0;
  leds[3][4] = 0;
  leds[3][3] = 0;
  leds[3][2] = 0;
  leds[3][1] = 0;
  leds[3][0] = 1;
  
  leds[4][6] = 0;
  leds[4][5] = 1;
  leds[4][4] = 1;
  leds[4][3] = 1;
  leds[4][2] = 1;
  leds[4][1] = 1;
  leds[4][0] = 0;  
  
}

void loop()
{    
  Usb.Task();
  if(adk.isReady()) {
    uint8_t msg[1];
    uint16_t len = sizeof(msg);
    uint8_t rcode = adk.RcvData(&len, msg);
    if(rcode && rcode != hrNAK)
      USBTRACE2("Data from Android rcv. :", rcode);
    if(len > 0) {
      Serial.print(F("\r\nData Packet: "));
      Serial.print(msg[0]);
      setPattern(msg[0]);
    }
    // Only Display Characters, when adk is ready
  } 
  
      refresh_display();
  
}

