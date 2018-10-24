#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
#define BATTERY_TEMP_SENSORS 3

/* LCD variables */
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* Temperature sensor variables */
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
/* Previously determined addresses of each connected sensor */
DeviceAddress sensor01 = { 0x28, 0xFF, 0x64, 0x1D, 0xF8, 0x65, 0xD2, 0x05 }; 
DeviceAddress sensor02 = { 0x28, 0xFF, 0x64, 0x1D, 0xFB, 0x96, 0xD0, 0x91 };
DeviceAddress sensor03 = { 0x28, 0xFF, 0x64, 0x1D, 0xFB, 0x97, 0xF0, 0x76 };
float temperatures[BATTERY_TEMP_SENSORS];

/* Speed sensor variables */
int speedSensor = 2;
volatile byte counter;
const float pi = 3.1415926535897932384626433832795; 
const float r = 0.1315;  // tyre radius = 131.5mm
int rpm;
int kph;
long timepassed;
long currtime;

/* Timer variables */
long timerPrev, timerCurr;

byte bar1[8] = 
{
        B11100,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11100
};
byte bar2[8] =
{
        B00111,
        B01111,
        B01111,
        B01111,
        B01111,
        B01111,
        B01111,
        B00111
};
byte bar3[8] =
{
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111
};
byte bar4[8] =
{
        B11110,
        B11100,
        B00000,
        B00000,
        B00000,
        B00000,
        B11000,
        B11100
};
byte bar5[8] =
{
        B01111,
        B00111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00011,
        B00111
};
byte bar6[8] =
{
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111
};
byte bar7[8] =
{
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00111,
        B01111
};
byte bar8[8] =
{
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000
};

void setup() {
  initLCD();
  lcd.createChar(1,bar1);
  lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);
  lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);
  lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);
  lcd.createChar(8,bar8);
  initSpeed();
  initBatteryTemp();
  timerPrev = millis();
}

void loop() {
  updateBatteryTemp();
  timerCurr = millis();
  if (timerCurr - timerPrev >= 1000) {
    calcSpeed();
    timerPrev = timerCurr;
  }
  updateLCD();
}

void initLCD(){
  lcd.begin(16, 2);
}

void initBatteryTemp(){
  sensors.begin();
  /*sensor resolution set at 10 byte*/
  sensors.setResolution(sensor01, 10);
  sensors.setResolution(sensor02, 10);
  sensors.setResolution(sensor03, 10);
}

void initSpeed(){
  attachInterrupt(digitalPinToInterrupt(2), isr, RISING);
  pinMode(speedSensor, INPUT);
  counter = 0;
  rpm = 0;
  kph = 0;
  timepassed = 0;
}

void isr(){
  counter++;
}

int calcSpeed(){
  currtime = millis();

  long timedelta = currtime - timepassed;
  timedelta = 30000 / timedelta;
  rpm = timedelta * counter;
  kph = (2*pi*r*rpm*60)/1000;
  counter = 0;
  timepassed = millis();
}

void updateBatteryTemp(){
  sensors.requestTemperatures();
  temperatures[0] = sensors.getTempC(sensor01);
  temperatures[1] = sensors.getTempC(sensor02);
  temperatures[2] = sensors.getTempC(sensor03);
}

void updateLCD(){
  lcd.setCursor(0, 0);
  // print number for speed
  int ten = kph / 10;
  int unit = kph % 10;
  printNumber(ten, 10);
  printNumber(unit, 13); 

  lcd.setCursor(0, 0);
  lcd.print("B ");
  lcd.print(highestBatteryTemperature());
  lcd.print(char(223));
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("M 100");
  lcd.print(char(223));
  lcd.print("C");
}

float highestBatteryTemperature(){
  float maxTemp = temperatures[0];
  for (int i = 1; i < BATTERY_TEMP_SENSORS; i++){
    if (temperatures[i] > maxTemp){
      maxTemp = temperatures[i];
    }
  }
  return maxTemp;
}

void custom0(int col)
{ // uses segments to build the number 0
  lcd.setCursor(col, 0); 
  lcd.write(2);  
  lcd.write(8); 
  lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(2);  
  lcd.write(6);  
  lcd.write(1);
}

void custom1(int col)
{
  lcd.setCursor(col,0);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}

void custom2(int col)
{
  lcd.setCursor(col,0);
  lcd.write(5);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(6);
}

void custom3(int col)
{
  lcd.setCursor(col,0);
  lcd.write(5);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1); 
}

void custom4(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}

void custom5(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1);
}

void custom6(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
}

void custom7(int col)
{
  lcd.setCursor(col,0);
  lcd.write(2);
  lcd.write(8);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}

void custom8(int col)
{
  lcd.setCursor(col, 0); 
  lcd.write(2);  
  lcd.write(3); 
  lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(2);  
  lcd.write(6);  
  lcd.write(1);
}

void custom9(int col)
{
  lcd.setCursor(col, 0); 
  lcd.write(2);  
  lcd.write(3); 
  lcd.write(1);
  lcd.setCursor(col, 1); 
  lcd.write(7);  
  lcd.write(6);  
  lcd.write(1);
}

void printNumber(int value, int col) {
  if (value == 0) {
    custom0(col);
  } if (value == 1) {
    custom1(col);
  } if (value == 2) {
    custom2(col);
  } if (value == 3) {
    custom3(col);
  } if (value == 4) {
    custom4(col);
  } if (value == 5) {
    custom5(col);
  } if (value == 6) {
    custom6(col);
  } if (value == 7) {
    custom7(col);
  } if (value == 8) {
    custom8(col);
  } if (value == 9) {
    custom9(col);
  }      
}  
