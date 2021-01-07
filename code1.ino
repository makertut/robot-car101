#include <PS2X_lib.h>  //for v1.6
#define SPEED_1  80
#define SPEED_2  160
#define SPEED_3  200
#define SPEED_4  250

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.
int error = 0; 
byte type = 0;
byte vibrate = 0;

  
int R1PWM_Output = 3; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int R2PWM_Output = 5; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)

int L1PWM_Output = 9; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int L2PWM_Output = 6    ; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)


String inString = "";    // string to hold input
int motorPWM = 0;

int defaultSpeed = 80;
int leftAdd = 20; 
void setup()
{
  Serial.begin(9600);
  pinMode(R1PWM_Output, OUTPUT);
  pinMode(R2PWM_Output, OUTPUT);

  pinMode(L1PWM_Output, OUTPUT);
  pinMode(L2PWM_Output, OUTPUT);

  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
 error = ps2x.config_gamepad(13,11,10,12, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 
 if(error == 0){
   Serial.println("Found Controller, configured successful");
   Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
  Serial.println("holding L1 or R1 will print out the analog stick values.");
  Serial.println("Go to www.billporter.info for updates and to report bugs.");
 }
   
  else if(error == 1)
   Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
   Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  else if(error == 3)
   Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
   
   //Serial.print(ps2x.Analog(1), HEX);
   
   type = ps2x.readType(); 
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");
       break;
       case 1:
        Serial.println("DualShock Controller Found");
       break;
       case 2:
         Serial.println("GuitarHero Controller Found");
       break;
     }
  
}
 
void loop()
{
    int pwm = readSerialValue();

    if( pwm != -1){

       if( pwm > 255) pwm = 255;
       else if( pwm < -255) pwm = 0;

       motorPWM = pwm;
    }

    if( pwm == 50){
      
       motorPWM =defaultSpeed ;
       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM);
       
    }else if(pwm == 150) {
       
       pwm =50;
       motorPWM =defaultSpeed ;
       
       analogWrite(R2PWM_Output, 0);
       analogWrite(R1PWM_Output, motorPWM);

       analogWrite(L2PWM_Output, 0);
       analogWrite(L1PWM_Output, motorPWM);
       
    }else if( pwm == 0){

       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM);
    }
 
 
    // forward rotation
   
  //   analogWrite(RPWM_Output, 0);
  //   analogWrite(LPWM_Output, motorPWM);

  if(error == 1) //skip loop if no controller found
  return; 
  
 if(type == 2){ //Guitar Hero Controller
  
 }
 else { //DualShock Controller
  
    ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))                   //will be TRUE as long as button is pressed
         Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
         Serial.println("Select is being held");
         
         
     if(ps2x.Button(PSB_PAD_UP)) {         //will be TRUE as long as button is pressed
         Serial.print("Up held this hard: ");
         Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
         runForward(); 
      }
      
      if(ps2x.Button(PSB_PAD_RIGHT)){
        Serial.print("Right held this hard: ");
        Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);

        turnRight();
      }
      if(ps2x.Button(PSB_PAD_LEFT)){
       Serial.print("LEFT held this hard: ");
        Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);

        turnLeft();
      }
      if(ps2x.Button(PSB_PAD_DOWN)){
       Serial.print("DOWN held this hard: ");
       Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
       runBackward();
      }   

     if(ps2x.ButtonReleased(PSB_PAD_UP) || 
        ps2x.ButtonReleased(PSB_PAD_DOWN) ||
        ps2x.ButtonReleased(PSB_PAD_LEFT) ||
        ps2x.ButtonReleased(PSB_PAD_RIGHT)){
            stopRun();
        }
    
      vibrate = ps2x.Analog(PSAB_BLUE);        //this will set the large motor vibrate speed based on 
                                              //how hard you press the blue (X) button    
    
    if (ps2x.NewButtonState())               //will be TRUE if any button changes state (on to off, or off to on)
    {
     
       
         
        if(ps2x.Button(PSB_L3))
         Serial.println("L3 pressed");
        if(ps2x.Button(PSB_R3))
         Serial.println("R3 pressed");

        if(ps2x.Button(PSB_L1)){
           Serial.println("L1 pressed SPEED 1");
           defaultSpeed = SPEED_1;
        }
                 
        if(ps2x.Button(PSB_L2)){
           Serial.println("L2 pressed SPEED 2");
           defaultSpeed = SPEED_2;
        }

        
        if(ps2x.Button(PSB_R1)){
           Serial.println("R1 pressed SPEED 3");
           defaultSpeed = SPEED_3;
         
        }
        
        if(ps2x.Button(PSB_R2)){
           Serial.println("R2 pressed SPEED 4");
           defaultSpeed = SPEED_4;
         
        }
        if(ps2x.Button(PSB_GREEN))
         Serial.println("Triangle pressed");
         
    }   
         
    
    if(ps2x.ButtonPressed(PSB_RED))             //will be TRUE if button was JUST pressed
         Serial.println("Circle just pressed");
         
    if(ps2x.ButtonReleased(PSB_PINK))             //will be TRUE if button was JUST released
         Serial.println("Square just released");     
    
    if(ps2x.NewButtonState(PSB_BLUE))            //will be TRUE if button was JUST pressed OR released
         Serial.println("X just changed");    
    
    
   
    
    
 }
 
 
 delay(50);

}

int readSerialValue()
{
     int v= -1;
  // Read serial input:
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char
      // and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string,
    // then the string's value:
    if (inChar == '\n') {
      Serial.print("String: ");
      Serial.println(inString);
      v = inString.toInt();
      Serial.print("Value:");
      Serial.println(v);
      
           
      // clear the string for new input:
      inString = "";
    }
  }
   return v;
}
void runForward(){
       
       motorPWM =defaultSpeed ;
       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM);
       
}
void runBackward(){
       
       motorPWM =defaultSpeed ;
       analogWrite(R2PWM_Output, 0);
       analogWrite(R1PWM_Output, motorPWM);

       analogWrite(L2PWM_Output, 0);
       analogWrite(L1PWM_Output, motorPWM);
       
}
void turnLeft(){

       motorPWM =defaultSpeed ;
       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L2PWM_Output, 0);
       analogWrite(L1PWM_Output, motorPWM+leftAdd);
  
}
void turnRight(){

       motorPWM =defaultSpeed ;
       analogWrite(R2PWM_Output, 0);
       analogWrite(R1PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM+leftAdd);
  
}
void stopRun(){
       motorPWM =0;
       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM);
}
