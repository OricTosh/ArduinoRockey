
// Ardunio Nano
// Bluetooth Conroller
// LCD Display
// 14/09/13 -- Added Mapped RH Joystick to send comands via BT
// 15/09/13 -- Added dead zone for Joystick + keyPin + Stop motor


#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define rxPin 16 
#define txPin 17

const int RH_POT_X = 18;     // A4 Right Hand X-Axis
const int RH_POT_Y = 19;     // A5 Right Hand Y-Axis
const int LH_POT_X = 21;     // A7 Left Hand X-Axis
const int LH_POT_Y = 20;     // A6 Left Hand Y-Axis
const int motorPin  = 9;     // Rumble motor on D9
const int keyPin    = 11;    // AT Command PIN
const int statePin  = 10;    // Bluetooth connection Status

int valueRH_X = 0;
int valueRH_Y = 0;
int valueLH_X = 0;
int valueLH_Y = 0;


SoftwareSerial BTSerial(rxPin,txPin); 
LiquidCrystal lcd(12, 8, 7, 6, 5, 4);

String incomingString  = "";
boolean stringComplete = false;
boolean hitWall        = false;

void setup()
{
  pinMode(keyPin, OUTPUT);    // For Setting AT Command Mode 
  digitalWrite(keyPin, LOW);  // Set HIGH if Needed
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, HIGH); // Set high due to PNP Transistor 
  
 //establishContact();  // send a byte to establish contact until receiver responds 
    
  // LCD display  
  //Serial.begin(9600);
  BTSerial.begin(9600);
  incomingString.reserve(200);
  lcd.begin(16,2);
  lcd.cursor();
  lcd.print("Rocky Control");
  lcd.setCursor(0,1);
}


void loop(void)
{
  //Serial.println("LOOP");
  //establishContact();
  
  if (BTSerial.available() > 0){
    ReadBTSerial();
  }
  
  delay(500);
  ReadJoystickCmd();
    
  if (hitWall) {
    //incomingString.trim();
    analogWrite(motorPin, 80);   // PWM LOW
    delay(1000);
    analogWrite(motorPin, 255);  // PWM HIGH
    lcd.clear();
    //lcd.setCursor(0,1);
    lcd.print(incomingString);
    lcd.setCursor(1,0);
    incomingString = "";
    stringComplete = false;
    hitWall  = false;
  }
  
  
  // good code    
  if (stringComplete) {
    incomingString.trim();
    //lcd.clear();
    lcd.print(incomingString);
    incomingString = "";
    stringComplete = false;
    //lcd.setCursor(0,6);
  }
  
 
 
 } // End of Loop
  
  
  //analogWrite(motorPin, valueRH_X);
  //lcd.print(valueRH_X);
  //displayVal();
 
// void establishContact() {
  // Serial.println(BTSerial.available());
  // while (BTSerial.available() <= 0) {
  //   lcd.setCursor(0,2);
  //   lcd.print("Conection ?"); 
  //   BTSerial.print('A\n');   // send a capital A
  //   Serial.println("connection");
  //   delay(300);
 // }
// }
 
 void ReadJoystickCmd()
 {
    valueLH_X = analogRead(LH_POT_X);
    valueLH_X = map(valueLH_X, 0, 1230, 0, 100);
    valueRH_Y = analogRead(RH_POT_Y);
    valueRH_Y = map(valueRH_Y, 0, 1230, 0, 100);
  
  //lcd.print(valueLH_X); // debug
  //lcd.setCursor(0,1);
  
  // LH X Joystick values
  if (valueLH_X < 53)
  {
      BTSerial.print("a");
      lcd.setCursor(0,1);
      //lcd.print("Left  ");
      //lcd.setCursor(1,0);
  }
  else if (valueLH_X > 55 )
  {  
      BTSerial.print("d");
      lcd.setCursor(0,1);
      //lcd.print("Right ");
  }
  else
  {
      // center = 50/51
      BTSerial.print("s");
      lcd.setCursor(0,1);
      //lcd.print("Center");
  }
  
  // RH Y Joystick Values
  if (valueRH_Y > 52)
  {
      BTSerial.print("b");
      //lcd.print("  Back");
  }
  else if (valueRH_Y < 49)
  {
      BTSerial.print("f");
      //lcd.print("  FWD ");
  }  
  else 
  {
      //center = 50/51
      BTSerial.print("k");
      //lcd.print("  Stop");
  }
   
   
 }
 
 void ReadBTSerial() {
   while (BTSerial.available()){
     char inChar = (char)BTSerial.read();
     incomingString += inChar;
    if (inChar == 'W'){
       hitWall = true;
    }
     
     if (inChar == '\n')
     {
       stringComplete = true;
     }
      
   }
 }


 
    
  
