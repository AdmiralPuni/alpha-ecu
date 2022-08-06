//initialize i2c lcd 16x2
//Library version:1.1
#include <Arduino.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define HALL_SENSOR_PIN A0

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const unsigned int HALL_TRESHOLD = 200;
const unsigned int TIRE_RADIUS_CM = 45;
const unsigned int HALL_MAGNET_COUNT = 4;
const unsigned float PI = 3.14159265;
bool hall_state = false;


unsigned int tire_rpm = 0;
unsigned int speed_kmh = 0;
unsigned int hall_count = 0;
const unsigned int UPDATE_INTERVAL_MS = 200;

unsigned long int last_update_ms = 0;

void setup(){
  lcd.init();
  lcd.backlight();

  //set hall interrupt pin to input
  pinMode(HALL_SENSOR_PIN, INPUT);

  last_update_ms = millis();
}

void loop(){
  unsigned int hall_value = analogRead(HALL_SENSOR_PIN);
  if (millis() - last_update_ms > UPDATE_INTERVAL_MS){
    int speed_kmh = int(calc_speed_kmh(hall_count));
    

    //reset hall count
    hall_count = 0;
  }

  if(hall_value < HALL_TRESHOLD){
    if(!hall_state){
      hall_count++;
      hall_state = true;
    }
  }
  else{
    hall_state = false;
  }
}

void lcd.println(String text, int row){
  lcd.setCursor(0,row);
  //print 16 empty characters to clear the previous text
  lcd.print("                ");
  lcd.print(text);
}

float calc_speed_kmh(int hall_count){
  unsigned float tire_rpm = hall_count * (1 / HALL_MAGNET_COUNT);
  unsigned float travelled = tire_rpm * (PI * TIRE_RADIUS_CM);

  //adjust against update interval
  float interval_adjustment = 1000 / UPDATE_INTERVAL_MS;

  unsigned float speed_cm_per_sec = travelled * interval_adjustment;
  //for an approximate result, divide the speed value by 27.778
  unsigned float speed_kmh = speed_cm_per_sec / 27.7778;

  return speed_kmh;
}

void update_display(int speed_kmh, int tire_rpm, int debug_val, int fuel_level){
  lcd.println("SPD: " + String(speed_kmh) + " DBG:" + String(debug_val));
  lcd.println("RPM: " + String(tire_rpm) + " FUE: " + String(fuel_level));
}