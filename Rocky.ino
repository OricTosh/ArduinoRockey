// Rockey Code - Chris Hills
// Arduino Powered RC Car
// 12th Aug 2012
// Updated 02/04/2013

#include <NewPing.h>
#include <Servo.h>

const int TRIGGER_PIN = 7;     // Arduino pin tied to trigger pin on ping sensor.
const int ECHO_PIN =  8;       // Arduino pin tied to echo pin on ping sensor.
const int MAX_DISTANCE = 60;  // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
const int safeDis  = 15;      // 1/4 distance

const int FRONT_MOTOR_EN1 = 10;  // Ardunio pin tied to enable 1
const int FRONT_LOG1 = 9;        // Ardunio pin tied to input 1 (PWM)
const int FRONT_LOG2 = 6;        // Ardunio pin tied to input 2 (PWM)
const int REAR_MOTOR_EN2 = 4;    // Ardunio pin tied to enable 2
const int REAR_LOG1 = 5;         // Ardunio pin tied to input 3 (PWM)
const int REAR_LOG2 = 3;         // Ardunio pin tied to input 4 (PWM)
const int STEERING_SERVO = 11;   // Ardunio pin tied to servo (PWM)
const int centAng = 56;          // Straight Ahead
const int HEARTBEAT = 13;        // Ardunio pin tied to internal led
const int sensorPin = A0;         // TM36 Temp Sensor data pin 

int LEFT_LOCK = 40;
int RIGHT_LOCK = 72;
int angle = 56;            // servo for loop postion
int ANGLE_STEP = 2;        // Turn in 2 degree increments
int samples[8];            // Take 8 temp readings
int i;                    // for loop counter
int tempReading = 0;      // temp readings
int advTemp;

#define aref_voltage 3.3

Servo steeringServo;            // create servo object to control servo
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);     // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 25;    // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer = 75;   // Holds the next ping time, start at 75ms to give time for the Arduino pins to stabilize.

unsigned long test;

int led = HEARTBEAT; 

void setup(void)
{
  pinMode(led, OUTPUT);
  pinMode(FRONT_MOTOR_EN1, OUTPUT);
  pinMode(FRONT_LOG1, OUTPUT);
  pinMode(FRONT_LOG2, OUTPUT);
  pinMode(REAR_MOTOR_EN2, OUTPUT);
  pinMode(REAR_LOG1, OUTPUT);
  pinMode(REAR_LOG2, OUTPUT);
  
  // start serial port
  Serial.begin(9600);
  steeringServo.attach(STEERING_SERVO,1400,1514);  // attaches the servo on pin 11 to the servo object
  steeringServo.write(centAng); // center the servo 
  analogReference(EXTERNAL); 
} 
    
void loop(void)
{ 
  for(i = 0; i<=7; i++){
    tempReading = analogRead(sensorPin);
    float voltage = (aref_voltage*tempReading)/ 1024.0;
    float temperatureC = (voltage-0.5)*10;
    samples[i] = temperatureC;
    advTemp = advTemp + samples[i];
    //Serial.print(temperatureC); 
    //Serial.println(" AdvTemp ") + advTemp;
    delay(1000);
  }
  
  advTemp = advTemp/8.0;
  Serial.print(advTemp); Serial.println(" degrees C");
  
  //float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  //Serial.print(temperatureF); Serial.println(" degrees F");
 delay(500);
   
  if (Serial.available() > 0) {
    int ch = Serial.read();
        
    switch (ch) {
    case 'f':    
      Serial.println("Forward");
      digitalWrite(FRONT_MOTOR_EN1, HIGH);   // forward
      analogWrite (FRONT_LOG1, 0);
      analogWrite (FRONT_LOG2, 255);         // PWM Values
      
      digitalWrite(REAR_MOTOR_EN2, HIGH); 
      digitalWrite(REAR_LOG1, LOW);
      digitalWrite(REAR_LOG2, HIGH);
      break;
    case '2':
      Serial.println("2WD Selected");
      digitalWrite(REAR_MOTOR_EN2, LOW);
      break;      
    case '4':
      Serial.println("4WD Selected");
      digitalWrite(REAR_MOTOR_EN2, HIGH);
      break; 
    case 'b':    
      Serial.println("Backward");
      digitalWrite(FRONT_MOTOR_EN1, HIGH);
      digitalWrite(FRONT_LOG1, HIGH);
      digitalWrite(FRONT_LOG2, LOW);
      
      digitalWrite(REAR_MOTOR_EN2, HIGH); 
      digitalWrite(REAR_LOG1, HIGH);
      digitalWrite(REAR_LOG2, LOW);
      break;
    case 'k':    
      Serial.println("STOP");
      digitalWrite(FRONT_MOTOR_EN1, LOW);
      digitalWrite(REAR_MOTOR_EN2, LOW);
      break;
    case 'a':    
      Serial.println("LEFT");
        if(angle > LEFT_LOCK)
        {
          angle = angle - ANGLE_STEP;  // step left
        }
        else
        {
          angle = LEFT_LOCK;
        }
        steeringServo.write(angle);
      break;
    case 'd':
      Serial.println("RIGHT");
        if(angle < RIGHT_LOCK - ANGLE_STEP)
        {
          angle = angle + ANGLE_STEP; // step right
        }
        else
        {
          angle = RIGHT_LOCK;
        }
        steeringServo.write(angle);
      break;
    case 's':
      Serial.println("STRIAGHT");
      angle = centAng;
      steeringServo.write(angle);
      //delay(pingSpeed);
      break;
    default:
      Serial.println("NO INPUT");
    }    
  }
   
    
  //heartbeat
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(pingSpeed); 
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(pingSpeed);               
  
  // Ping Sensor
  if (millis() >= pingTimer) // pingSpeed milliseconds since last ping, do another ping.
  { 
    pingTimer += pingSpeed;    // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
   // function to show temperatures
   // call sensors.requestTemperatures() to issue a global temperature 
   // request to all devices on the bus
   //sensors.requestTemperatures(); // Send the command to get temperatures    
   //printTemperature(tempDeviceAddress);  
    
}  //end of loop


void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
      if (sonar.convert_cm(sonar.ping_result) < safeDis)
      {
        Serial.println("STOP WALL");
        //digitalWrite(FRONT_MOTOR_EN1, LOW);
        //digitalWrite(REAR_MOTOR_EN2, LOW);
        //digitalWrite(FRONT_MOTOR_EN1, HIGH);
        analogWrite(FRONT_LOG1, 191);
        analogWrite(FRONT_LOG2, 191);
        analogWrite(REAR_LOG1, 191);
        analogWrite(REAR_LOG2, 191);
        
        delay(30000);
        digitalWrite(FRONT_MOTOR_EN1, LOW);
        digitalWrite(REAR_MOTOR_EN2, LOW);
        
        //digitalWrite(FRONT_LOG1, HIGH);
        //digitalWrite(FRONT_LOG2, LOW);
        //digitalWrite(REAR_MOTOR_EN2, HIGH); 
        //digitalWrite(REAR_LOG1, HIGH);
        //digitalWrite(REAR_LOG2, LOW);
             
        
      }           
        
    Serial.print("Ping: ");
    Serial.print(sonar.convert_cm(sonar.ping_result)); // Ping returned, uS result in ping_result, convert to cm with convert_cm.
    Serial.println("cm");
  }
  // Don't do anything here!
}

 
// function to print a device address
//void printAddress(DeviceAddress deviceAddress)
//{
  //for (uint8_t i = 0; i < 8; i++)
  //{
    //if (deviceAddress[i] < 16) Serial.print("0");
    //Serial.print(deviceAddress[i], HEX);
  //}
//}

