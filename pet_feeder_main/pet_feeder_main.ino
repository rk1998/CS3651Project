
#include <Wire.h>
#include <SparkFunDS1307RTC.h>

#include <HX711.h>
#include <Stepper.h>
#include <LiquidCrystal.h>

#define PRINT_USA_DATE
#define MOTOR_STEPS 2038
#define MOTOR_SPEED 7

//LCD Display pins
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
const int scale_dout = 3;
const int scale_clk = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//motor pins
const int motor1 = 7;
const int motor2 = 6;
const int motor3 = 5;
const int motor4 = 4;

//button pins
const int setWeightButton = 14;
const int setTimeButton = 15;
const int manualFeedButton = 16;
const int increaseButton = 19;
const int decreaseButton = 18;

Stepper motor = Stepper(MOTOR_STEPS, motor1, motor3, motor2, motor4);
HX711 scale;


float feedVal = 20; //in grams
int feed1hour = 13;
int feed1minute = 45;
int feed2hour = 14;
int feed2minute = 30;

float scale_calibration_factor = 561400;
float scale_zero_factor = 3705; 
boolean bowl_full = false;
boolean stepper_direction = true;

//volatile int virtualPosition = 0; `
void setup() {
  rtc.begin();
  //rtc.setTime(30, 29, 15, 3, 26, 11, 19);
  //rtc.autoTime();
  Serial.begin(9600);
  lcd.begin(16, 2);
  //lcd.print("Curr Time: ");
  pinMode(setWeightButton, INPUT_PULLUP);
  pinMode(setTimeButton, INPUT_PULLUP);
  pinMode(increaseButton, INPUT_PULLUP);
  pinMode(decreaseButton, INPUT_PULLUP);
  pinMode(manualFeedButton, INPUT_PULLUP);
  scale.begin(scale_dout, scale_clk);
  scale.set_scale(scale_calibration_factor);
  //zero out scale and set offset to account for the weight of the bowl
  scale.tare();
  long zero_factor = scale.read_average(10);
  scale.set_offset(zero_factor);
  //scale.tare();
  motor.setSpeed(MOTOR_SPEED);
}

void printDigits(int digits){   // utility function for digital clock display: prints leading 0
   if(digits < 10) {
      lcd.print('0');
   }
   lcd.print(digits);
 }


void loop() {
  static int8_t lastSecond = -1;
  static long virtualPosition = 0;
  rtc.update();
  int currHour = rtc.hour();
  int currMinute = rtc.minute();

  if(rtc.second() != lastSecond) {
    printTime();
    lastSecond = rtc.second();
  }

  
  //logic for handling Set Time Button press
  if(digitalRead(setTimeButton) == LOW) {
    lcd.clear();
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.print("Set 1st Feed Hr:");
    virtualPosition = feed1hour;
    do {
      lcd.setCursor(2, 1);
      delay(400); // delay is used to allow time to read presses from increase or decrease buttons
      if(digitalRead(increaseButton) == LOW){
          virtualPosition++;
          if(virtualPosition > 23) {
              virtualPosition = 0;
          }
      } else if(digitalRead(decreaseButton) == LOW) {
          virtualPosition--;
          if(virtualPosition < 0) {
            virtualPosition = 23;
          }
      }
      Serial.println(virtualPosition);
      //print out current time
      feed1hour = virtualPosition;
      lcd.setCursor(2, 1);
      printDigits(feed1hour);
      lcd.print(":" + String(feed1minute));
      //lcd.print(String(feed1hour) + ":" + String(feed1minute));
    }while((digitalRead(setTimeButton)));
    lcd.noBlink();
    delay(1000);
    lcd.clear();
    
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.print("Set 1st Feed Min:");
//    lcd.setCursor(2, 1);
//    lcd.print(String(feed1hour) + ":" + String(feed1minute));
    lcd.blink();
    do {
      lcd.setCursor(5, 1);
      delay(400);
      virtualPosition = feed1minute;
      if(digitalRead(increaseButton) == LOW) {
         virtualPosition++;
         if(virtualPosition > 59) {
            virtualPosition = 0;
         }
      } else if(digitalRead(decreaseButton) == LOW) {
        virtualPosition--;
        if(virtualPosition < 0) {
          virtualPosition = 59;
        }
      }
      feed1minute = virtualPosition;
      lcd.setCursor(2, 1);
      printDigits(feed1hour);
      lcd.setCursor(4, 1);
      lcd.print(":");
      //lcd.print(String(feed1hour) + ":");
      lcd.setCursor(5, 1);
      printDigits(feed1minute);
      //lcd.print(String(feed1minute));
    }while(digitalRead(setTimeButton));
    lcd.noBlink();
    delay(1000);

    lcd.clear();
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.print("Set 2nd Feed Hr:");
    virtualPosition = feed2hour;
    do {
      lcd.setCursor(2, 1);
      delay(400);
      if(digitalRead(increaseButton) == LOW){
          virtualPosition++;
          if(virtualPosition > 23) {
              virtualPosition = 0;
          }
      } else if(digitalRead(decreaseButton) == LOW) {
          virtualPosition--;
          if(virtualPosition < 0) {
            virtualPosition = 23;
          }
      }
      Serial.println(virtualPosition);
      feed2hour = virtualPosition;
      lcd.setCursor(2, 1);
      printDigits(feed2hour);
      lcd.print(":" + String(feed2minute));
    }while((digitalRead(setTimeButton)));
    lcd.noBlink();
    delay(1000);
    lcd.clear();
    
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.print("Set 2nd Feed Min:");
    lcd.blink();
    do {
      lcd.setCursor(5, 1);
      delay(400);
      virtualPosition = feed2minute;
      if(digitalRead(increaseButton) == LOW) {
         virtualPosition++;
         if(virtualPosition > 59) {
            virtualPosition = 0;
         }
      } else if(digitalRead(decreaseButton) == LOW) {
        virtualPosition--;
        if(virtualPosition < 0) {
          virtualPosition = 59;
        }
      }
      feed2minute = virtualPosition;
      lcd.setCursor(2, 1);
      printDigits(feed2hour);
      lcd.setCursor(4, 1);
      lcd.print(":");
      //lcd.print(String(feed1hour) + ":");
      lcd.setCursor(5, 1);
      printDigits(feed2minute);
    }while(digitalRead(setTimeButton));
    lcd.noBlink();
    delay(1000);
    lcd.clear();
    
  }

  //similar logic is used to set the weight
  if(digitalRead(setWeightButton) == LOW) {
    lcd.clear();
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.print("Set Feed Qty(g): ");
    virtualPosition = feedVal;
    do {
      lcd.setCursor(2, 1);
      delay(400);
      if(digitalRead(increaseButton) == LOW){
          virtualPosition++;
          if(virtualPosition > 150) {
              virtualPosition = 0;
          }
      } else if(digitalRead(decreaseButton) == LOW) {
          virtualPosition--;
          if(virtualPosition < 0) {
            virtualPosition = 1;
          }
      }
      Serial.println(virtualPosition);
      feedVal = virtualPosition;
      lcd.setCursor(2, 1);
      printDigits(feedVal);
    }while(digitalRead(setWeightButton));
    lcd.noBlink();
    delay(1000);
    lcd.clear();
  }

  //start feed cycle if the manual feed button is pressed
  if(digitalRead(manualFeedButton) == LOW) {
    feedCycle();
  }
  //start feed cycle if current time matches one of the two user set times
  if((currHour == feed1hour) && (currMinute == feed1minute) && !bowl_full) {
    feedCycle();
  } else if((currHour == feed2hour) && (currMinute == feed2minute) && !bowl_full) {
    feedCycle();
  }

  //read current weight of food in the bowl. this will prevent
  //the feed cycle from starting if manual feed is pressed
  //or if one of the feed times comes up
  float current_weight = (scale.get_units(2)/0.453592)*1000;
  Serial.println("CURRENT WEIGHT: " + String(current_weight));
  if(current_weight >= .80 * feedVal) {
    bowl_full = true;
  } else {
    bowl_full = false;
  }
}

void feedCycle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Let's Eat!");
  delay(500);
  motor.setSpeed(MOTOR_SPEED);
  while(true) {
   //read current weight from scale
   float current_weight = (scale.get_units(10)/0.453592)*1000; //get weight in grams
   Serial.println("WEIGHT: " + String(current_weight));
   lcd.setCursor(0,1);
   lcd.print("Curr Weight: " + String(current_weight));
   if(current_weight >= .80 * feedVal) {
    //lcd.print("Curr Weight: " + String(current_weight));
    bowl_full = true;
    delay(2000);
    break;
   }
   //turn 3/4ths of a revolution
   motor.step(-1529);
//   if(stepper_direction == true) {
//    motor.step(-1529);
//   } else {
//     motor.step(1529);
//   }
//   stepper_direction = !stepper_direction;
//   Serial.println(stepper_direction);
  }
  lcd.clear();
}

void printTime()
{
  lcd.setCursor(0, 0);
  //Serial.print(String(rtc.hour()) + ":"); // Print hour
  lcd.print(String(rtc.hour()) + ":");
  if (rtc.minute() < 10) {
    Serial.print('0'); // Print leading '0' for minute
    lcd.print('0');
  }
  //Serial.print(String(rtc.minute()) + ":"); // Print minute
  lcd.print(String(rtc.minute()) + ":");
  if (rtc.second() < 10) {
    Serial.print('0'); // Print leading '0' for second
    lcd.print('0');
  }
  //Serial.print(String(rtc.second())); // Print second
  lcd.print(String(rtc.second()));

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) {
      //Serial.print(" PM"); // Returns true if PM
      lcd.print(" PM");
    }
    else {
      //Serial.print(" AM");
      lcd.print(" AM");
    }
  }

  lcd.setCursor(0, 1);
  lcd.print("1)");
  lcd.print(String(feed1hour) + ":");
  if(feed1minute < 10) {
    lcd.print("0");
  }
  lcd.print(String(feed1minute) + " ");
  
  lcd.setCursor(8, 1);
  lcd.print("2)");
  lcd.print(String(feed2hour) + ":");
  if(feed2minute < 10) {
    lcd.print("0");
  }
  lcd.print(String(feed2minute) + " ");
  

  
  
  Serial.print(" | ");

  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
#ifdef PRINT_USA_DATE
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
#else
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
                 String(rtc.month()) + "/"); // Print month
#endif
  Serial.println(String(rtc.year()));        // Print year
}
