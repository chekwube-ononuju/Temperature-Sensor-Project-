#include <Servo.h>
#include <DHT.h>

// Define DHT sensor type and pin
#define DHTPIN 2          // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11     // DHT11 or DHT22

DHT dht(DHTPIN, DHTTYPE);

// Define RGB LED pins
const int redPin = 9;          // Red channel of RGB LED
const int greenPin = 10;       // Green channel of RGB LED
const int bluePin = 11;        // Blue channel of RGB LED
const int servoPin = 6;        // Servo motor connected to Digital Pin 6

Servo myServo;

// Temperature thresholds (in Celsius)
const float T1 = 30.0; // Threshold for AC
const float T2 = 18.0; // Threshold for HEAT

// Define color gradient (10 colors from Purple to Red)
struct Color {
  int red;
  int green;
  int blue;
};

const int numColors = 10;
Color colorSteps[numColors] = {
  {128, 0, 128}, // Purple
  {153, 0, 102},
  {179, 0, 76},
  {204, 0, 51},
  {229, 0, 25},
  {255, 0, 0},   // Red
  {255, 51, 0},
  {255, 102, 0},
  {255, 153, 0},
  {255, 204, 0}  // Light Red/Orange
};

void setup() {
  // Initialize serial communication for plotting
  Serial.begin(9600);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize RGB LED pins as OUTPUT
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // Initialize Servo
  myServo.attach(servoPin);
  myServo.write(90); // Neutral position
  
  // Print thresholds for reference
  Serial.println("Temperature Thresholds:");
  Serial.print("T1 (AC On): ");
  Serial.println(T1);
  Serial.print("T2 (HEAT On): ");
  Serial.println(T2);
}

void loop() {
  // Read temperature from DHT sensor
  float temperature = readTemperature();
  
  // Check if reading was successful
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Map temperature to RGB color
  setColorBasedOnTemperature(temperature);
  
  // Control servo based on temperature thresholds
  controlServo(temperature);
  
  // Send data to Serial for plotting
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, LED Color: ");
  Serial.print(getColorName(temperature));
  Serial.print(", Servo Position: ");
  Serial.println(getServoPosition(temperature));
  
  delay(1000); // Wait for 1 second before next reading
}

float readTemperature() {
  // Read temperature as Celsius
  float tempC = dht.readTemperature();
  return tempC;
}

void setColorBasedOnTemperature(float temp) {
  // Define temperature range for mapping
  float minTemp = 10.0; // Minimum expected temperature
  float maxTemp = 40.0; // Maximum expected temperature
  
  // Clamp temperature within range
  if (temp < minTemp) temp = minTemp;
  if (temp > maxTemp) temp = maxTemp;
  
  // Determine which color step to use
  int step = map(temp, minTemp, maxTemp, 0, numColors - 1);
  
  // Set RGB LED color
  analogWrite(redPin, colorSteps[step].red);
  analogWrite(greenPin, colorSteps[step].green);
  analogWrite(bluePin, colorSteps[step].blue);
}

void controlServo(float temp) {
  if (temp > T1) {
    // Turn servo to "AC" position (e.g., 0 degrees)
    myServo.write(0);
  }
  else if (temp < T2) {
    // Turn servo to "HEAT" position (e.g., 180 degrees)
    myServo.write(180);
  }
  else {
    // Neutral position (e.g., 90 degrees)
    myServo.write(90);
  }
}

String getColorName(float temp) {
  // Simple mapping for demonstration purposes
  if (temp >= 30.0) return "Red";
  else if (temp >= 27.0) return "Orange-Red";
  else if (temp >= 24.0) return "Orange";
  else if (temp >= 21.0) return "Yellow-Orange";
  else if (temp >= 18.0) return "Yellow";
  else if (temp >= 15.0) return "Yellow-Green";
  else if (temp >= 12.0) return "Green";
  else if (temp >= 9.0) return "Blue-Green";
  else if (temp >= 6.0) return "Blue";
  else return "Purple";
}

String getServoPosition(float temp) {
  if (temp > T1) return "AC";
  else if (temp < T2) return "HEAT";
  else return "Neutral";
}

