#include <MQ2.h>
#include <MQ7.h>
#include <Wire.h>
#include <DHT11.h>
#include <LiquidCrystal_I2C.h>

#define VOLTAGE 5
#define DHT11_PIN A1
#define LIGHT_PIN A0

#define MQ2_PIN A3
#define MQ7_PIN A2


float lpg;
float co;
float smoke;
float humidity;
float temperature;

MQ2 mq2(MQ2_PIN);
DHT11 dht11(DHT11_PIN);
MQ7 mq7(MQ7_PIN, VOLTAGE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  pinMode(LIGHT_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ7_PIN, INPUT);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
}

void loop()
{

  print_temperature_humidity();
  print_gases();
  print_co();
  print_light();
}

void print_gases()
{

  lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LPG:");
  lcd.setCursor(0, 1);
  lcd.print(lpg);
  delay(2600);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO (PPM):");
  lcd.setCursor(0, 1);
  lcd.print(co);
  delay(2600);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smoke:");
  lcd.setCursor(0, 1);
  lcd.print(smoke);
  lcd.print(" %");
  delay(2600);
}

void print_co()
{

  while (!Serial)
  {
    lcd.clear();
    mq7.calibrate();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO - MQ7 (PPM):");
  lcd.setCursor(0, 1);
  lcd.print(mq7.readPpm());
  delay(2600);
}

void print_light()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light:");
  lcd.setCursor(0, 1);
  lcd.print(analogRead(LIGHT_PIN));
  delay(2600);
}

void print_temperature_humidity()
{
  humidity = dht11.readHumidity();
  temperature = dht11.readTemperature();

  if (temperature != -1 && humidity != -1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature (C):");
    lcd.setCursor(0, 1);
    lcd.print(temperature);
    delay(2600);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidity (%):");
    lcd.setCursor(0, 1);
    lcd.print(humidity);
    delay(2600);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dew Point (C):");
    lcd.setCursor(0, 1);
    lcd.print(dewPoint(temperature, humidity));
    delay(2600);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Real Feel (C):");
    lcd.setCursor(0, 1);
    lcd.print(realFeel(temperature, humidity));
    delay(2600);
  }
  else
  {
    lcd.clear();
  }
}

double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO))) - 1);
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1);
  RHS += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

double realFeel(double celsius, double humidity){
  double c1 = -8.78469475556;
  double c2 = 1.61139411;
  double c3 = 2.33854883889;
  double c4 = -0.14611605;
  double c5 = -0.012308094;
  double c6 = -0.0164248277778;
  double c7 = 2.211732e-3;
  double c8 = 7.2546e-4;
  double c9 = -3.582e-6;

  double HI = c1 + c2*celsius + c3*humidity + c4*celsius*humidity + c5*pow(celsius, 2) + c6*pow(humidity,2) + c7*pow(celsius, 2)*humidity + c8*celsius*pow(humidity,2) + c9*pow(celsius,2)*pow(humidity,2);

  return HI;

}