#include <Adafruit_NeoPixel.h>
#include <RCSwitch.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int senderpin     = 9;
int recpin        = 2;
int buttonpin     = 3;
int neopixel      = 10;
int val           = LOW;
int jobcount      = 0;
int jobcur        = 0;
int counter       = 0;
int cache         = 0;
char device[4]    = "9803";
unsigned long startmillis   = 0;
unsigned long finalmillis   = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
RCSwitch rcSwitch = RCSwitch();
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(12, neopixel, NEO_GRB + NEO_KHZ800);

void  setup()
{
  pinMode(buttonpin, INPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  rcSwitch.enableReceive(0);
  rcSwitch.enableTransmit(senderpin);

  pixels.begin();
  pixels.setBrightness(12);
  pixels.show();

  lcd.setCursor(0, 0);
  lcd.print("Blowjob Trainer");
}
void  loop() {
  String(order);
  if (rcSwitch.available()) {
    order = String(rcSwitch.getReceivedValue());
    if ( (String(order[0]) == "9") && (String(order[1]) == "8") && (String(order[2]) == "0") && (String(order[3]) == "3")  ) {
      order.remove(0, 4);
      jobcount = order.toInt();
      if ( jobcount == 0 )  {
        delay(1000);
        Serial.println("Receive ping send pong");
        rcSwitch.send(990300, 32);
      } else {
      lcd.setCursor(0, 0);
      lcd.print("0 / " + order + " Jobs       ");
      lcd.setCursor(0, 1);
      lcd.print("Wait 60 Seconds     ");

      for (int i = 6; i >= 0; i--) {
        lcd.setCursor(0, 1);
        lcd.print("Wait " + String(i) + " Seconds     ");
        delay(1000);
      }
      lcd.setCursor(0, 0);
      lcd.print("Blowjob Trainer   ");
      lcd.setCursor(0, 1);
      lcd.print("0 / " + order + " Jobs    ");
      jobtraining( jobcount );
      }
    }
    rcSwitch.resetAvailable();
  }
}

void jobtraining(int count) {
  lcd.setCursor(0, 0);
  lcd.print("Blowjob Trainer ");
  lcd.setCursor(0, 1);
  lcd.print("0 / " + String(count) + " Jobs    ");
  finalmillis = millis() + 12500;
  
  while (true)  {
    val=digitalRead(buttonpin);
    if( val == HIGH ) {
      finalmillis = millis() + 3500;
      jobcur++;
      lcd.setCursor(0, 1);
      lcd.print(String(jobcur)+" / " + String(count) + " Jobs    ");
      while ( finalmillis > millis() )  {
        val=digitalRead(buttonpin);
        startmillis = round( ( finalmillis - millis() ) / 1000 );
        if (cache != startmillis) {
          cache = startmillis;
          // lights( startmillis );
        }
      }
      finalmillis = millis() + 3500;
    }
    if ( jobcur >= count ) {
      lcd.setCursor(0, 0);
      lcd.print("Blowjob Trainer    ");
      lcd.setCursor(0, 1);
      lcd.print("Training done.     ");
      jobcur = 0;
      break;
    }
    startmillis = round( ( finalmillis - millis() ) / 1000 );
    if (cache != startmillis) {
       cache = startmillis;
       lights( startmillis );
    }

    if ( startmillis == 0 ) {
      finalmillis = millis() + 3500;
      rcSwitch.send(99031110, 32);
    }
    Serial.println( startmillis );
    
  }
}

void lights(int count)  {
  pixels.clear();
  for (int i = 0; i <= count; i++)  {
    pixels.setPixelColor(i,  pixels.Color(0,255,0));
    pixels.show();
  }
}
