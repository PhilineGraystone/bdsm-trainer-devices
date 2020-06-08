#include <Wire.h>
#include <VL53L0X.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#define NUMPIXELS 12
#define PIN       D3

const char* SSID        = "<WLAN SID>";
const char* PASSWORD    = "<WLAN PASSWORD>";
const char* MQTT_BROKER = "<MQTT IP>";
const char* MQTT_USER   = "punisher";
const char* MQTT_PASS   = "punisher";
const char* MQTT_CLIENT = "torturedevice02";

long lastMsg = 0;
char msg[50];
char serialmsg[50];
String para;
String para2;
int value = 0;
unsigned long timercount = 0;
volatile int seconds;
volatile int counter       = 0;
int jobcount      = 0;
int jobcur        = 0;
int cache         = 0;
unsigned long startmillis   = 0;
unsigned long finalmillis   = 0;

VL53L0X sensor;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();

  setup_wifi();

  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);

  pixels.begin();
  pixels.setBrightness(12);
  pixels.show();

  lcd.setCursor(0, 0);
  lcd.print("Blowjob Trainer");

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message [");
  Serial.print(topic);
  Serial.print("] ");
  char msg[length + 1];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';
  Serial.println();

  para   = String(msg);
  para.trim();
  para2 = getValue(para, ',', 0);
  para  = getValue(para, ',', 1);

  seconds = para.toInt();
  counter = para2.toInt();
  snprintf(serialmsg, 50, "Blowjob Trainer starts in %u seconds with %u sucks", seconds, counter);
  Serial.println(serialmsg);
  if ( counter != 0 ) {
    lcd.setCursor(0, 0);
    lcd.print("0 / " + String(counter) + " Jobs       ");
    lcd.setCursor(0, 1);
    lcd.print("Wait " + String(seconds) + " Seconds     ");
    
    for (int i = 0; i <= seconds; i++) {
      value = seconds - i;
      lcd.setCursor(0, 1);
      lcd.print("Wait " + String(value) + " Seconds     ");
      delay(1000);
    }
    lcd.setCursor(0, 0);
    lcd.print("Blowjob Trainer ");
    lcd.setCursor(0, 1);
    lcd.print("0 / " + String(counter) + " Jobs    ");
    jobtraining( counter );
    finalmillis = millis() + 12500;
  }

  snprintf (msg, 50, "BlowJob Done! -  %ld", millis());
  client.publish("punisher/devices/torturedevice02/response", msg);
}

void jobtraining(int count) {
  lcd.setCursor(0, 0);
  lcd.print("Blowjob Trainer ");
  lcd.setCursor(0, 1);
  lcd.print("0 / " + String(count) + " Jobs       ");
  finalmillis = millis() + 12500;

  while (true)  {
    if ( sensor.readRangeContinuousMillimeters() < 40 ) {
      Serial.println( sensor.readRangeContinuousMillimeters() );
      finalmillis = millis() + 3500;
      jobcur++;
      lcd.setCursor(0, 1);
      lcd.print(String(jobcur) + " / " + String(count) + " Jobs       ");
      while ( finalmillis > millis() )  {
        startmillis = round( ( finalmillis - millis() ) / 1000 );
        if (cache != startmillis) {
          cache = startmillis;
          // lights( startmillis );
        }
        yield();
        client.loop();
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
      client.publish("punisher/functions/shock", "0,10");
      delay(100);
      Serial.println("PUNISH SHOCK");
    }
    Serial.println( startmillis );
    client.loop();
    yield();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting MQTT...");
    if (!client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS)) {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
  client.subscribe("punisher/functions/blowjob");
  client.subscribe("punisher/devices/torturedevice02/blowjob");
  Serial.println("MQTT Connected...");
}

void lights(int count)  {
  pixels.clear();
  for (int i = 0; i <= count; i++)  {
    pixels.setPixelColor(i,  pixels.Color(0, 255, 0));
    pixels.show();
  }
}

void loop() {
  //  lcd.setCursor(0,0);
  //  lcd.print(sensor.readRangeContinuousMillimeters());

  if (!client.connected()) {
    reconnect();
  }
  if (timercount == 0)  {
    snprintf (msg, 50, "availabe -  %ld", millis());
    client.publish("punisher/devices/torturedevice02/available", msg);
    timercount = 10000;
  }
  timercount--;
  client.loop();
}
