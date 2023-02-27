#include <SimpleKalmanFilter.h>
#include <Wire.h>  // Wire library - used for I2C communication
#include <LiquidCrystal_I2C.h>
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs
SimpleKalmanFilter simpleKalmanFilter(2, 2, 0.01);
unsigned long heart_micros = 500000;
unsigned long t= micros();


unsigned long prevTime, curTime;
int y;
int monitor;
int an_input = A1;
int input = 8;
bool skip = false;
float avg_value;
#define MCP4725_ADDR 0x60

//const long SERIAL_REFRESH_TIME = 100;
//long refresh_time;
const long SAMPLE_RATE = 5000; // 5 seconds
const long OUTPUT_RATE = 500; // 0.5 seconds
const long SAMPLE_COUNT = 5;
float avg_list[SAMPLE_COUNT];
int idx = 0;
unsigned long sample_time;
unsigned long output_time;
float total = 0.0;
int count = 0;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600); 
  lcd.begin();
  lcd.setBacklight((uint8_t)1);
      // First row
  lcd.print("fari bolod");

  // Second row
  lcd.setCursor(0,1);
  lcd.print("Razin sagol");  
  Wire.begin(); // Initiate the Wire library
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);
  pinMode(input, INPUT);
  prevTime = micros();
  sample_time = millis();
  output_time = millis();
}

void print_pulse(int x){
 Serial.print("H:1, L:0, P:"); Serial.println(x);
}


void loop() {
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = X_out/256*10; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = Y_out/256*10;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/256*10;
  float avg = abs(X_out)+abs(Y_out)+abs(Z_out)-10;
  float estimated_value = simpleKalmanFilter.updateEstimate(abs(avg));
  // if (millis() > refresh_time) {
  //Serial.print("H:12,L:0,S:");
  //Serial.println(estimated_value,4);
  // Serial.println();
  //   refresh_time = millis() + SERIAL_REFRESH_TIME;
  // // }
  // if (millis() > sample_time + SAMPLE_RATE) {
  //   total += estimated_value;
  //   count++;
  //   sample_time = millis();
  //   if (count > SAMPLE_COUNT) {
  //     total -= avg_list[idx];
  //     avg_list[idx++] = estimated_value;
  //     idx = idx % SAMPLE_COUNT;
  //   }  
  // }
  // if (millis() > output_time + OUTPUT_RATE) {
  //   avg_value = total/count;
  //   //Serial.print("High:10, Low:0, Signal:");
  //   Serial.print(avg_value, 4);
  //   Serial.println();
  //   output_time = millis();
  // }
  heart_micros = 500000/(1+estimated_value/6);
  if (micros() - t > heart_micros) {
      t = micros();
      print_pulse(1);
    } else {
      print_pulse(0);
    }

  // Wire.beginTransmission(MCP4725_ADDR);
  // Wire.write(64);                     // cmd to update the DAC
  // //Serial.println(y);
  // heart(50*(1+avg_value/6), 30);
  // Wire.write(y >> 4); // high byte
  // Wire.write(y & 0x0F); // low byte
  // Wire.endTransmission();
  // monitor = analogRead(an_input);
  //Serial.print("High:1020, Low:0, Signal:");
  //Serial.println(monitor);  


}

// void heart(int bpm, int block_rate){
//   curTime = micros();
//   int r = random(0, 100);
//   float val = 60000000/bpm;
//   //Serial.println(val/1000);
//   if (digitalRead(input)){
//     if (curTime - prevTime >= val/2) {
//       if (r < block_rate) skip = true;
//       else skip = false;
//       prevTime = micros();
//     } else if (!skip) {
//       y = (int)abs(4095*(sin(2*PI*(curTime - prevTime)/val)));
//       if (y >= 600){
//         y = (int)abs(4095*(sin(2*PI*(curTime - prevTime)/val)))-500;
//       } else y = 0;
//     } else y = 0;
//   } else y = (int)0;
// }

