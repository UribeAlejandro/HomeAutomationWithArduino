#include <MQ2.h>
#include <MQ7.h>
#include <Wire.h>
#include <DHT11.h>
#include <LiquidCrystal_I2C.h>

#define VOLTAGE 5
#define DHT11_PIN 2

#define MQ2_PIN A0
#define MQ7_PIN A1
#define LIGHT_PIN A2

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