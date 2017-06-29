#define VERSION     "\n\nGSSI Arduino Bluetooth Protocol \nAuthor: Vinnie \nEmail: savsan60@gmail.com\n\n"

#define    STX          0x02
#define    ETX          0x03
#define    ledPin       13
#define    SLOW         750                            // Datafields refresh rate (ms)
#define    FAST         250                             // Datafields refresh rate (ms)

int initpoint = 0;

byte cmd[8] = {0, 0, 0, 0, 0, 0, 0, 0};                 // bytes received
byte buttonStatus = 0;                                  // first Byte sent to Android device
long previousMillis = 0;                                // will store last time Buttons status was updated
long sendInterval = SLOW;                               // interval between Buttons status transmission (milliseconds)
String displayStatus = "xxxx";                          // message to Android device

bool DEBUG = false;
bool GRAPH = true;
bool PIDbutton = false;

//LZM: for suction motor
float PressureValue;
#define SPEEDLOW 2050
#define SPEEDMID 2060
#define SPEEDHIG 2070

void setup()  {
  Serial.begin(115200);
  Serial1.begin(115200);                                // 57600 = max value for softserial

  Serial.println(VERSION);
  while (Serial1.available())  Serial1.read();        // empty RX buffer
  
  //Initializing Subsystems
  driveTrainInit();
  pressureESCinit();
  //pinMode(5,OUTPUT);
  //pinMode(6,OUTPUT);
  //analogWrite(5,127);
  //analogWrite(6,127);
}

void loop() {
  // Interface to the Bridge Control Panel (So we can graph the sensor data from the pressure sensor)
  if (GRAPH) {
    printPressureSensorGraph();
  }

  // Just check the code for Button #6. If the PIDbutton is "OFF" then the ESC is set to 2000 (power = 0) else PIDButton is "ON" we compute the PID and set it to the ESC directly 
  if (PIDbutton) {
    PressureValue = pidESCcontrol();
  }
  
  // DEBUG
  if (DEBUG) {
    Serial.print(millis());
    Serial.print(" ");
    Serial.println(getPressureSensorValue());
  }


  if (Serial1.available())  {                          // data received from smartphone
    delay(2);
    cmd[0] =  Serial1.read();
    if (cmd[0] == STX)  {
      int i = 1;
      while (Serial1.available())  {
        delay(1);
        cmd[i] = Serial1.read();
        if (cmd[i] > 127 || i > 7)                 break; // Communication error
        if ((cmd[i] == ETX) && (i == 2 || i == 7))   break; // Button or Joystick data
        i++;
      }
      if     (i == 2)          getButtonState(cmd[1]);  // 3 Bytes  ex: < STX "C" ETX >
      else if (i == 7)          getJoystickState(cmd);  // 6 Bytes  ex: < STX "200" "180" ETX >
    }
  }
  sendBlueToothData();
}

void sendBlueToothData()  {
  static long previousMillis = 0;
  long currentMillis = millis();
  if (currentMillis - previousMillis > sendInterval) {  // send data back to smartphone
    previousMillis = currentMillis;

    // Data frame transmitted back from Arduino to Android device:
    // < 0X02   Buttons state   0X01   DataField#1   0x04   DataField#2   0x05   DataField#3    0x03 >
    // < 0X02      "01011"      0X01     "120.00"    0x04     "-4500"     0x05  "Motor enabled" 0x03 >    // example

    Serial1.print((char)STX);                                             // Start of Transmission
    Serial1.print(getButtonStatusString());  Serial1.print((char)0x1);   // buttons status feedback
    Serial1.print(GetdataInt1());            Serial1.print((char)0x4);   // datafield #1
    Serial1.print(GetdataFloat2());          Serial1.print((char)0x5);   // datafield #2
    Serial1.print(displayStatus);                                         // datafield #3
    Serial1.print((char)ETX);                                             // End of Transmission
  }
}

String getButtonStatusString()  {
  String bStatus = "";
  for (int i = 0; i < 6; i++)  {
    if (buttonStatus & (B100000 >> i))      bStatus += "1";
    else                                    bStatus += "0";
  }
  return bStatus;
}

int GetdataInt1()  {              // Data dummy values sent to Android device for demo purpose
  static int i = -30;             // Replace with your own code
  //float PressureValue =  filteredData(getPressureSensorValue());
  i ++;
  if (i > 0)    i = -30;
  return i;
  //return PressureValue;
}

float GetdataFloat2()  {           // Data dummy values sent to Android device for demo purpose
  //static float i = 50;             // Replace with your own code
  float Value;
  Value =  PressureValue;
  //i -= .5;
  //if (i < -50)    i = 50;
  //return i;
  return PressureValue;
}

void getJoystickState(byte data[8])    {
  int joyX = (data[1] - 48) * 100 + (data[2] - 48) * 10 + (data[3] - 48); // obtain the Int from the ASCII representation
  int joyY = (data[4] - 48) * 100 + (data[5] - 48) * 10 + (data[6] - 48);

  //joyX = map(joyX, 100, 300, -90, 90);            //turning default is 90
  //joyY = map(joyY, 100, 300, 0, 179);
  //int leftMotorPower = (3 + joyY - joyX);         //right is left, left is right
  //int rightMotorPower = (2 + 179 - joyY - joyX);

  //setDriveTrainPower(leftMotorPower, rightMotorPower);
  
  joyX = map(joyX, 100, 300, -100, 100);            //turning default is 90
  joyY = map(joyY, 100, 300, 0, 255);
  int leftMotorPower, rightMotorPower;

  if(abs(joyX)<50)
  {
      leftMotorPower = joyY;         //right is left, left is right
      rightMotorPower = joyY;
  }
  else
  {
    leftMotorPower = joyY - joyX;
    rightMotorPower = 255 - joyY - joyX;
  }
  
  //analogWrite(5,leftMotorPower);
  //analogWrite(6,rightMotorPower);
  setDriveTrainPower(leftMotorPower, rightMotorPower);

  if (DEBUG) {
    Serial.print("Joystick position:  ");
    Serial.print(joyX);
    Serial.print(", ");
    Serial.println(joyY);
  }
}

void getButtonState(int bStatus)  {
  switch (bStatus) {
    // -----------------  BUTTON #1  -----------------------
    case 'A':
      buttonStatus |= B000001;        // ON
      buttonStatus &= B001001;        
      Serial.println("\n** Button_1: ON **");
      // your code...
      //if (initpoint == 0)
      //{
        //pressureESCinit(50);
        //PIDinit(43);
        PIDinit(SPEEDLOW);
        initpoint = 1;
      //}
      displayStatus = "LAW <ON>";
      Serial.println(displayStatus);
      //digitalWrite(ledPin, HIGH);
      break;
    case 'B':
      buttonStatus &= B111110;        // OFF
      Serial.println("\n** Button_1: OFF **");
      // your code...
      initpoint = 0;
      displayStatus = "LAW <OFF>";
      Serial.println(displayStatus);
      //digitalWrite(ledPin, LOW);
      break;

    // -----------------  BUTTON #2  -----------------------
    case 'C':
      buttonStatus |= B000010;        // ON
      buttonStatus &= B001010;
      Serial.println("\n** Button_2: ON **");
      // your code...
      //if (initpoint == 0)
      //{
        //pressureESCinit(55);
        //PIDinit(45);
        PIDinit(SPEEDMID);
        initpoint = 1;
      //}
      displayStatus = "MID <ON>";
      Serial.println(displayStatus);
      break;
    case 'D':
      buttonStatus &= B111101;        // OFF
      Serial.println("\n** Button_2: OFF **");
      // your code...
      initpoint = 0;
      displayStatus = "MID <OFF>";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #3  -----------------------
    case 'E':
      buttonStatus |= B000100;        // ON
      buttonStatus &= B001100;
      Serial.println("\n** Button_3: ON **");
      // your code...
      //if (initpoint == 0)
      //{
        //pressureESCinit(60);
        //PIDinit(47);
        PIDinit(SPEEDHIG);
        initpoint = 1;
      //}
      displayStatus = "HIGH <ON>"; // Demo text message
      Serial.println(displayStatus);
      break;
    case 'F':
      buttonStatus &= B111011;      // OFF
      Serial.println("\n** Button_3: OFF **");
      // your code...
      initpoint = 0;
      displayStatus = "HIGH <OFF>";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #4  -----------------------
    case 'G':
      buttonStatus |= B001000;       // ON
      Serial.println("\n** Button_4: ON **");
      // your code...
      //displayStatus = "Datafield update <FAST>";
      //Serial.println(displayStatus);
      //sendInterval = FAST;
      
      PIDbutton = true;
      displayStatus = "Motor Start"; // Demo text message
      Serial.println(displayStatus);
      break;
    case 'H':
      buttonStatus &= B110111;    // OFF
      Serial.println("\n** Button_4: OFF **");
      // your code...
      //displayStatus = "Datafield update <SLOW>";
      //Serial.println(displayStatus);
      //sendInterval = SLOW;

      PIDbutton= false;
      //setPressureESC(1500);
      setPressureESC(2000);
      displayStatus = "Motor Stop";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #5  -----------------------
    case 'I':           // configured as momentary button
      //      buttonStatus |= B010000;        // ON
      Serial.println("\n** Button_5: ++ pushed ++ **");
      // your code...
      displayStatus = "Button5: <pushed>";
      break;
    //   case 'J':
    //     buttonStatus &= B101111;        // OFF
    //     // your code...
    //     break;

    // -----------------  BUTTON #6  -----------------------
    case 'K':
      buttonStatus |= B100000;        // ON
      Serial.println("\n** Button_6: ON **");
      //PIDbutton = true;
      //displayStatus = "Button6 <ON>"; // Demo text message
      break;
    case 'L':
      buttonStatus &= B011111;        // OFF
      Serial.println("\n** Button_6: OFF **");
      //PIDbutton= false;
      //setPressureESC(1500);
      //setPressureESC(2000);
      //displayStatus = "Button6 <OFF>";
      break;
  }
  // ---------------------------------------------------------------
}

