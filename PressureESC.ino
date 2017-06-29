#include <Servo.h>
#include <PID_v1.h>

//_______________________________
//
// The Pressure Control Subsystem
// Primarily controls the compressor motor using feedback PID using the pressure sensor
//_______________________________

Servo ESC;

double Setpoint, PressureSensorValue, ESCOutput;
//double Setpoint, ESCOutput;
PID myPID(&PressureSensorValue, &ESCOutput, &Setpoint, 0.5, 0.1, 0, DIRECT);
//PID_ATune aTune(&PressureSensorValue, &ESCOutput);

void pressureESCinit() {
  //Attach ESC to pin 9
  ESC.attach(COMPRESSOR_ESC_PIN);

  //myPID.SetMode(AUTOMATIC);
  //myPID.SetOutputLimits(2000, 2800);
  //Setpoint = point;                             //Looking for resting power

  ESC.writeMicroseconds(INITIAL_ESC_POWER); //initializes the ESC for startup
  delay(3000);

  //LZM
  //ESC.writeMicroseconds(1500);
}

void PIDinit(double point){
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(MINMUM, MAXMUM);
  //Setpoint = point;                             //Looking for resting power
  ESCOutput = point;
}


// Sets the power of the ESC
void setPressureESC(int power) {
  ESC.writeMicroseconds(power);
  //ESC.writeMicroseconds(Setpoint);
}


// Returns the raw value of the pressure sensor
float getPressureSensorValue() {
  return analogRead(PRESSURE_SENSOR_PIN);
}

// Prints Hex data to Serial to be graphed on Bridge Control Panel
void printPressureSensorGraph() {
  int PressureSensorValue = getPressureSensorValue();

  //CODE for Bridge Control Panel to access the PRESSSURE_SENSOR_VALUES
  Serial.println(PressureSensorValue);  
//  Serial.print("C");
//  Serial.write(PressureSensorValue>>8);
//  Serial.write(PressureSensorValue&0xff); 

/*
  //Filtered Data
  float filterData = filteredData(PressureSensorValue);
  Serial.write(int(filterData)>>8);
  Serial.write(int(filterData)& 0xff);

  //  //CODE for Bridge Control Panel to acce1500ss the ESC_OUTPUT_GENERATED_BY_PID
  Serial.write(int(ESCOutput) >> 8);
  Serial.write(int(ESCOutput) & 0xff);

*/
}

//Sets the ESC power
double pidESCcontrol() {
  PressureSensorValue =  filteredData(getPressureSensorValue());
  //myPID.Compute();
  setPressureESC(ESCOutput);
  return PressureSensorValue;
}

