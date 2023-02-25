#include<Wire.h>

/*MPU RELATED VARIABLES*/
const int SDA_PIN = 33;
const int  SCL_PIN = 32;
const int MPU_ADDRESS = 0x68;
const int DIVIDE = 100;
int16_t x,y,z; //Use this type, not 'int'

/*LIGHT RELATED VARIABLES*/
const int RED_PIN = 25;
const int GREEN_PIN = 26;
const int BLUE_PIN = 27;

/*MICS VARIABLES*/
int idx = 0;
int states[3] = {0,0,0};
boolean inMotion = false;

/*GETS THE AVERAGE X, Y, Z AND PUTS THEM INTO 'AVERAGES'*/
void getAverageXYZ(float* averages){

  //INITIALIZING VARIABLES
  int16_t myArrayX[DIVIDE];
  int16_t myArrayY[DIVIDE];
  int16_t myArrayZ[DIVIDE];
  float averageX = 0;
  float averageY = 0;
  float averageZ = 0;

  for(int i = 0; i < DIVIDE; i++){ 

    //TELL THE DEVICE THAT YOU ARE GOING TO READ ACCELLEROMETER INFORMATION
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDRESS,6,true); 

    //GETS THE X, Y, Z
    x=Wire.read()<<8|Wire.read();
    y=Wire.read()<<8|Wire.read();
    z=Wire.read()<<8|Wire.read();
    myArrayX[i] = x;
    myArrayY[i] = y;
    myArrayZ[i] = z;
    averageX = 0;
    averageY = 0;
    averageZ = 0;

    //AVERAGES THE X, Y, Z
    for (int j = 0; j < DIVIDE; j++){
      averageX += myArrayX[j];
      averageY += myArrayY[j];
      averageZ += myArrayZ[j];
    }
    averageX /= DIVIDE;
    averageY /= DIVIDE;
    averageZ /= DIVIDE;

    //PUSHES THE AVERAGE X, Y, Z INTO AVERAGES
    float temp[3] = {averageX, averageY, averageZ};
    for(int i = 0; i < 3; i++)
      averages[i] = temp[i];
  }
}

//FINDS THE CURRENT POSITION OF THE DEVICE AND PUTS IT IN THE LIST OF STATES
void pushState(int* states, float* averages){

  //INITIALIZE VARIABLES
  int curr_state;
  float x = averages[0];
  float y = averages[1];
  float z = averages[2];

  //CHECK WHAT STATE IT'S CURRENTLY IN
  if(
    (-3000 < x && x < 3000) &&
    (-3000 < y && y < 3000) &&
    (15000 < z && z < 18000)
    ){                                //State 0: Flat-Face-Up
    curr_state = 0;
    digitalWrite(GREEN_PIN, HIGH);
    Serial.printf("State Recieved: %d\n", curr_state);
    delay(100);
    digitalWrite(GREEN_PIN, LOW);

    
  } else if(
    (-3000 < x && x < 3000) &&
    (-18000 < y && y < -15000) &&
    (-3000 < z && z < 3000)
    ){                                //state 1: Flat-Face-Left
    curr_state = 1;
    digitalWrite(GREEN_PIN, HIGH);
    Serial.printf("State Recieved: %d\n", curr_state);
    delay(200);
    digitalWrite(GREEN_PIN, LOW);

    
  } else if(
    (-3000 < x && x < 3000) &&
    (15000 < y && y < 18000) &&
    (-3000 < z && z < 3000)
    ){                                //state 2: Flat-Face-Right
    curr_state = 2;
    digitalWrite(GREEN_PIN, HIGH);
    Serial.printf("State Recieved: %d\n", curr_state);
    delay(200);
    digitalWrite(GREEN_PIN, LOW);

    
  } else if(
    (-3000 < x && x < 3000) &&
    (-3000 < y && y < 3000) &&
    (-18000 < z && z < -15000)
    ){                                //state 3: Flat-Face-Down
    curr_state = 3;
    digitalWrite(GREEN_PIN, HIGH);
    Serial.printf("State Recieved: %d\n", curr_state);
    delay(200);
    digitalWrite(GREEN_PIN, LOW);

    
  } else if(
    (15000 < x && x < 18000) &&
    (-3000 < y && y < 3000) &&
    (-3000 < z && z < 3000)
    ){                                //state 4: Up-Face-Up
    curr_state = 4;
    digitalWrite(GREEN_PIN, HIGH);
    Serial.printf("State Recieved: %d\n", curr_state);
    delay(200);
    digitalWrite(GREEN_PIN, LOW);

    
  } else {
    curr_state = -1;                  //state -1: Unknown position
    digitalWrite(RED_PIN, HIGH);
    Serial.println("Unknown Position, try to be more steady!");
    delay(200);
    digitalWrite(RED_PIN, LOW);
  }


  /*Algorithm to push the most recent state into
  the array of states and pop the first one*/
  
  int temp[3];
  for(int i = 0; i < 3; i++)
    temp[i-1] = states[i];
  temp[2]=curr_state;
  for(int i = 0; i < 3; i++)
    states[i] = temp[i];
}


//CHECKS TO SEE IF A KNOWN MOTION WAS PERFORMED
void checkForMotion(){
  int s1 = states[0];
  int s2 = states[1];
  int s3 = states[2];

  //quick outs
  if(s1 == 0 && s2 == 0 && s3 == 0) return;    // The motion has not occurred yet
  if(s1 == -1 || s2 == -1 || s3 == -1) return; //If one of the states is unknown, stop
  if(s1==s2)return;                            //None of the motions start with two of the same state

  //CHECK IF RESET MOTION WAS PERFORMED
  if(s1 == 0 && s2 == 1 && s3 == 0){
    
    //TURN OFF THE LIGHT
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);

    //RESET STATES TO ZERO
    states[0] = 0;
    states[1] = 0;
    states[2] = 0;
    Serial.println("Resetting! Please input next motion!");
    return;
  }

  //CHECK IF 'CURL' MOTION WAS PERFORMED
  if(s1 == 0 && s2 == 4 && s3 == 4){
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    Serial.println("Nice Curl!!");
    delay(2000);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    return;
  }

  //CHECK IF 'FLIP' MOTION WAS PERFORMED
  if( s1 == 0 && s2 == 3 && s3 == 3){
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
    Serial.println("Flippin' Pancakes!!");
    delay(2000);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    return;
  }

  //CHECK IF 'FALL' MOTION WAS PERFORMED
  if( s1 == 4 && s2 == 0 && s3 == 0){
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
    Serial.println("Oh No!! You Fell!!");
    delay(2000);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    return;
  }

  //IF THE MOTION PERFORMED IS UNKNOWN THEN TELL THE USER AND ADVISE A MOTION
  Serial.println("Motion Unknown, try curling the device");
}


void setup(){
  /*SETUP MPU*/
  Wire.begin(SDA_PIN, SCL_PIN, 400000);   
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  /*SETUP LIGHT*/
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(115200);  
}

void loop(){
  //INCREASE THE INDEX UP TO 50, AND RESET WHEN IT GETS THERE
  idx = idx<50?idx+1:0;
  
  // MAKES AND FILLS THE ARRAY
  float averages[3]; // 3 for X, Y, Z
  getAverageXYZ(averages);

  //IF THE LOOP HAS HAPPENED A MULTIPLE OF 50 TIMES, THE PERFORM THESE ACTIONS
  if(idx == 49){
    pushState(states, averages);
    checkForMotion();
  }
}


/*                               //LIST OF STATES// 
                                
ZERO: {x|-2000<=x<=2000}, {y|-2000<=y<=2000}, {z|15000<=z<=18000}   //Flat-Face-Up
ONE:  {x|-2000<=x<=2000}, {y|-17000<=y<=-15000}, {z|-2000<=z<=2000} //Flat-Face-Left
TWO:  {x|-2000<=x<=2000}, {y|16000<=y<=15000}, {z|-2000<=z<=2000}   //Flat-Face-Right
THREE:{x|-2000<=x<=2000}, {y|-2000<=y<=2000}, {z|-17000<=z<=-15000} //Flat-Face-Down
FOUR: {x|15000<=x<=18000}, {y|-2000<=y<=2000}, {z|-2000<=z<=2000}   //Up-Rot-0
FIVE: {x|-2000<=x<=2000}, {y|-17000<=y<=-15000}, {z|-2000<=z<=2000} //Up-Rot-Left
SIX:  {x|-2000<=x<=2000}, {y|15000<=y<=18000}, {z|-2000<=z<=2000}   //Up-Rot-Right

*/


/*     //MOTIONS//

RESET: ZERO -> ONE -> ZERO
CURL:  ZERO -> FOUR -> FOUR
FLIP:  ZERO -> THREE -> THREE
FALL:  FOUR -> ZERO -> ZERO

*/
