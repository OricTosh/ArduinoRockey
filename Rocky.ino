// Rockey Code - Chris Hills
// Arduino Powered RC Car

#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>
#include <Servo.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

#define TRIGGER_PIN      7    // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN         8    // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE     60   // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define FRONT_MOTOR_EN1  10   // Ardunio pin tied to enable 1
#define FRONT_LOG1       9    // Ardunio pin tied to input 1 (PWM)
#define FRONT_LOG2       6    // Ardunio pin tied to input 2 (PWM)
#define REAR_MOTOR_EN2   4    // Ardunio pin tied to enable 2
#define REAR_LOG1        5    // Ardunio pin tied to input 3 (PWM)
#define REAR_LOG2        3    // Ardunio pin tied to input 4 (PWM)
#define STEERING_SERVO   11   // Ardunio pin tied to servo (PWM)
#define HEARTBEAT        13   // Ardunio pin tied to internal led

Servo steering; // create servo object to control servo

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 50;  // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer = 75; // Holds the next ping time, start at 75ms to give time for the Arduino pins to stabilize.

unsigned long test;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found
int led = HEARTBEAT; 
int pos = 0;    // variable to store the servo position

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

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
  steering.attach(STEERING_SERVO, 36, 60);  // attaches the servo on pin 11 to the servo object
  steering.write(45);  
    
  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
  
  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Found device ");
		Serial.print(i, DEC);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();
		
		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION, DEC);
		
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		 Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
		Serial.println();
	}else{
		Serial.print("Found ghost device at ");
		Serial.print(i, DEC);
		Serial.print(" but could not detect address. Check power and cabling");
	}
  }
   
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}


void loop(void)
{ 
  
  if (Serial.available() > 0) {
    int inByte = Serial.read();
        
    switch (inByte) {
    case 'f':    
      Serial.println("Forward");
      digitalWrite(FRONT_MOTOR_EN1, HIGH); // forward
      analogWrite (FRONT_LOG1, 0);
      analogWrite (FRONT_LOG2, 255); // PWM Values
      
      digitalWrite(REAR_MOTOR_EN2, HIGH); 
      digitalWrite(REAR_LOG1, LOW);
      digitalWrite(REAR_LOG2, HIGH);
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
      Serial.println("Stop");
      digitalWrite(FRONT_MOTOR_EN1, LOW);
      digitalWrite(REAR_MOTOR_EN2, LOW);
      break;
    /*case 'm':    
      Serial.println("Left");
      steering.write(60);
      delay(15);
      break;
    case 'n':
      Serial.println("Right");
      steering.write(36);
      delay(15);
      break;
      */
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
   sensors.requestTemperatures(); // Send the command to get temperatures    
   printTemperature(tempDeviceAddress);  
    
}  //end of loop



void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    Serial.print("Ping: ");
    Serial.print(sonar.convert_cm(sonar.ping_result)); // Ping returned, uS result in ping_result, convert to cm with convert_cm.
    Serial.println("cm");
  }
  // Don't do anything here!
}

 
// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

