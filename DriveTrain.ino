#include<Servo.h>

//_______________________________
//
// The Drive train Subsystem
// Primarily controls the drivetrain motors for driving the robot
//_______________________________


//Servo leftMotor;
//Servo rightMotor;

void driveTrainInit() {
  //leftMotor.attach(LEFT_MOTOR_PIN);
  //rightMotor.attach(RIGHT_MOTOR_PIN);
  pinMode(LEFT_MOTOR_PIN,OUTPUT);
  pinMode(RIGHT_MOTOR_PIN,OUTPUT);
  analogWrite(LEFT_MOTOR_PIN,127); // Output PWM 500Hz
  analogWrite(RIGHT_MOTOR_PIN,127);
}

// Setting power to drivetrain
// 90- 0rpm
// 180 full speed (forward)
// 0 full speed (reverse)
void setDriveTrainPower(int leftMotorPower, int rightMotorPower) {

  //Serial.println("Setting Power");
  
  //leftMotor.write(leftMotorPower);
  //rightMotor.write(rightMotorPower);
  analogWrite(LEFT_MOTOR_PIN,leftMotorPower);
  analogWrite(RIGHT_MOTOR_PIN,rightMotorPower);
}


