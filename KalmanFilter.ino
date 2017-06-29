// Filter temperature sensor readings using the Kalman process

const int sensorPin = A0;  // named constant for the pin the sensor is connected to

// kalman variables
float varVolt =  .543221;  // variance determined using excel and reading samples of raw sensor data
float varProcess = .5e-6;
float Pc = 0.0;
float G = 0.0;
float P = 1.0;
float Xp = 0.0;
float Zp = 0.0;
float Xe = 0.0;

//void setup() {
//  // open a serial connection to display values
//  Serial.begin(9600);
//}

float filteredData(float voltage) {
  //float sensorVal = analogRead(sensorPin);     // read the value on AnalogIn pin 0 and store it in a variable
  //float voltage = sensorVal * 5.0 / 1023.0;  // convert the ADC reading to voltage

  // kalman process
  Pc = P + varProcess;
  G = Pc/(Pc + varVolt);    // kalman gain
  P = (1-G)*Pc;
  Xp = Xe;
  Zp = Xp;
  Xe = G*(voltage-Zp)+Xp;   // the kalman estimate of the sensor voltage


  return Xe;
//  serialFloatPrint(voltage);
//  serialFloatPrint(Xe);
//  Serial.println();
//  delay(20);
}

// function to print out floats in HEX
void serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  Serial.print("#");
  for(int i=0; i<4; i++) {
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
    char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;
    Serial.print(c1);
    Serial.print(c2);
  }
}
