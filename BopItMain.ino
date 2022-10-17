#include <LiquidCrystal.h>

//ResetIt output pin
const int ResetIt = 7;
//PullIt output pin
const int  PullIt = 8;
//Accel output Pin
//const int  BumpItX = A3;
//const int  BumpItY =A2;
//const int  BumpItZ = A1;
const int BumpIt = A0;

const int Start = A3;
//LCD pins
const int  LCDRS  =12;  //on website 12  real 12 spreadsheet
const int  LCDEnable = 10;  //web 11 real 10
const int  LCDD4 = 6  ;  //web 5 real 5  MOVE TO 6 PWM WITH SAME FREQ
const int  LCDD5 = 0 ;   //web 4 real 4 moved from 4 to 0
const int  LCDD6 = 9 ;  //web 3  real 9 good
const int  LCDD7 = 2;   //web 2 real 2 good
//pwm: 3 4 5 9 10 11   we need 
//Speaker Pin

//For tone speaker
const int  Speaker = 11;
//LED pins
const int  LEDFail = 4;
const int  LEDHold = 1;
const int  LEDWin  = 13;
//const int  LEDPwr  5

const int  SeedPin =A4;

const int bumpMaxAmplitude = 10000; //place holder value for max acceleration change once expirimentally determined


//LCD instance?
LiquidCrystal lcd(LCDRS,LCDEnable,LCDD4,LCDD5,LCDD6,LCDD7);


void setup() {
  //Setting up pins as input and output
  //INPUTS:
  //Reset Button
  pinMode(ResetIt,INPUT); //Add 10k pull down resistor
  //Cartridge
  pinMode(PullIt,INPUT);  //Generally high and circuit complete so we want pull down resistor 10k
  //Accelerometer (See if we really need to put pull up resistors on these pins)
  //pinMode(BumpItX, INPUT);
  //pinMode(BumpItY, INPUT);
  //pinMode(BumpItZ, INPUT);
  pinMode(BumpIt,INPUT);
  //LCD pins
  pinMode(Start,INPUT);


  //Speaker
  pinMode(Speaker, OUTPUT);
  digitalWrite(Speaker,LOW);
  //LEDS
  pinMode(LEDFail,OUTPUT);
  digitalWrite(LEDFail,LOW);
  pinMode(LEDHold,OUTPUT);
  digitalWrite(LEDHold,LOW);
  pinMode(LEDWin,OUTPUT);
  digitalWrite(LEDWin,LOW);
  //pinMode(LEDPwr,OUTPUT);
  //digitalWrite(LEDPwr,HIGH);
  pinMode(SeedPin,INPUT);
  

  //LCD setup
  lcd.begin(16,2);

  
}
int ResetInitial = digitalRead(Start); //changed to bump it for debugging
void loop() {
  //if reset button is hit run game, if not display "Hit reset button to start game"
  
  if(digitalRead(Start)!=ResetInitial) //was reset it
  {
    //run game
    
    int score = 0;
    unsigned long timeAllowed = 8000;
    bool winLose = 0;
    while(score<99)
    {
      delay(1000);
      randomSeed(analogRead(SeedPin));
      long RandNumber = random(3); //Will spit out random number 0-2 which will determine actions
      executeCommand(RandNumber); //This command will tell the user what to do, and display a song, and the hold LED
      delay(250);
      bool passFail = waitForUserAction(RandNumber,timeAllowed); 
      digitalWrite(LEDHold,LOW);
      if(passFail)
      {
        score = score+1;
        if(score == 99)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("You Win!");
          lcd.setCursor(0,1);
          lcd.print(String("Score: " + String(score)));

          LEDCelebrate();
          ResetInitial = digitalRead(Start); //also changed this one
          break;
        }
        else
        {
          //display LCD with score, FIGURE THIS OUT
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Good Job!");
          lcd.setCursor(0,1);
          lcd.print(String("Score: " + String(score)));
          digitalWrite(LEDWin,HIGH);
          delay(timeAllowed);
          digitalWrite(LEDWin,LOW);
        }
      }
      else
      {
        //You lose, display score
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("You Lose!");
        lcd.setCursor(0,1);
        lcd.print(String("Score: " + String(score)));
        digitalWrite(LEDFail,HIGH);
        delay(3000);
        digitalWrite(LEDFail,LOW);
        ResetInitial = digitalRead(Start);  //was reset it
        break;
      }
      timeAllowed = timeAllowed-30;
    }
  }
  else
  {
    //Tell user to hit reset button to start game on LED
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Press Start"); //20 characters
  }

}


//This function (executeCommand) will display the action, make an audible noise, and set hold LED to high

void executeCommand(long RandNumber) //PLAY AROUND WITH LENGTH OF TONES
{
  digitalWrite(LEDHold, HIGH); 
  lcd.setCursor(0,0);
  switch (RandNumber){
    case 0:  //0 is Reset it
      lcd.clear();
      lcd.print("Reset It!");
      //Make Melody:
      tone(Speaker, 261); //middle c
      delay(100);
      noTone(Speaker);
      tone(Speaker, 329); //E4
      delay(100);
      noTone(Speaker);
      tone(Speaker,392); //G4
      delay(100);
      noTone(Speaker);
      break;
    case 1:
      lcd.clear();
      lcd.print("Pull It!");
      //Make Melody:
      tone(Speaker, 392 );  //G4
      delay(100);
      noTone(Speaker);
      tone(Speaker, 311  );   //Eb4
      delay(100);
      noTone(Speaker);
      tone(Speaker, 261  );   //C4
      delay(100);
      noTone(Speaker);
      break;
    default:
      lcd.clear();
      lcd.print("Bump It!");
      //Make Melody:
      tone(Speaker,698  );  //F5
      delay(75);
      noTone(Speaker);
      tone(Speaker, 494  );  //B4
      delay(75);
      noTone(Speaker);
      tone(Speaker, 587  );  //D5
      delay(75);
      noTone(Speaker);
      tone(Speaker, 392);  //G4
      delay(75);
      noTone(Speaker);
      break;
  }
    

}

bool waitForUserAction(long randNumber, unsigned long timeAllowed)
{
  unsigned long StartTime = millis(); //get the time
  
  lcd.begin(16,2);
  switch(randNumber)
  {
    case 0:  //reset it
      delay(50);
      int lastSwitchState = digitalRead(ResetIt); //Records current and previous switch states
      delay(50);
      int currentSwitchState = lastSwitchState;
      

      //below is tests for pressing wrong button/bump

      int lastPull0State = digitalRead(PullIt);
      int currentPull0State = lastPull0State;

      int lastBump0State = digitalRead(BumpIt);
      int currentBump0State = lastBump0State;
      //int xResult0 = 0;
      //int yResult0 = 0;
      //int zResult0 = 0;
      
      while(1) //inf loop
      {
        currentSwitchState = digitalRead(ResetIt); //reads reset it
        if(currentSwitchState != lastSwitchState)  //if the switch got moved, then...
        {
          
          break;
        }
        currentPull0State = digitalRead(PullIt);
        if(lastPull0State != currentPull0State) //if accidently pull it exit fail
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("F P during R"); //debugging
          delay(2000); //debugging
          return 0;
        }
        currentBump0State - digitalRead(BumpIt);
        if(currentBump0State != lastBump0State)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("F B during R");
          delay(2000);
          return 0;
        }
        //test if accidentily bump it
        //xResult0 = analogRead(BumpItX);
        //yResult0 = analogRead(BumpItY);
        //zResult0 = analogRead(BumpItZ);
        //int netResult0 = sqrt(pow(xResult0,2)+pow(yResult0,2)+pow(zResult0,2));
        //if(netResult0>=bumpMaxAmplitude)
        //{
        //  lcd.clear();  //debugging
        //  lcd.setCursor(0,0);
          //lcd.print("F B during R"); //debugging

          //delay(2000); //debugging
        //  return 0;
        //}
        unsigned long CurrentTime = millis(); //if not flipped in time, record current time
        unsigned long deltaTime = CurrentTime - StartTime; //find net time since start of task
        if(deltaTime>timeAllowed) //if net time since start of task is greater than allowed time, break out of the loop without changing flag
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("F too slow"); //debugging
          delay(2000); //debugging
          return 0;
        }
      }

      //now on to the flipping the switch again, most code will be copied.
      

      lastSwitchState = digitalRead(ResetIt);
      currentSwitchState = lastSwitchState;

      while(1)
      {
        currentSwitchState = digitalRead(ResetIt);
        if(currentSwitchState != lastSwitchState)
        {
          return 1; //switch activated, return success
        }
        currentPull0State = digitalRead(PullIt);
        if(lastPull0State != currentPull0State) //if accidently pull it exit fail
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("F P during R"); //debugging
          delay(2000); //debugging
          return 0;
        }
        currentBump0State = digitalRead(BumpIt);
        if(lastBump0State != currentBump0State)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("F B during R");
          delay(2000);
          return 0;
        }
        //xResult0 = analogRead(BumpItX); //if accidentally bump it
        //yResult0 = analogRead(BumpItY);
        //zResult0 = analogRead(BumpItZ);
        //int netResult0 = sqrt(pow(xResult0,2)+pow(yResult0,2)+pow(zResult0,2));
        //if(netResult0>=bumpMaxAmplitude)
       // {
       //   lcd.clear();  //debugging
         // lcd.setCursor(0,0);
          //lcd.print("F B during R"); //debugging
          //delay(2000); //debugging
          //return 0;
        //}
        unsigned long CurrentTime = millis();
        unsigned long deltaTime = CurrentTime - StartTime;
        if(deltaTime>timeAllowed)
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("F too long"); //debugging
          delay(2000); //debugging
          return 0; //too much time as elapsed return fail
        }
      }

      break;
    case 1:  //pull it

      int lastPullState = digitalRead(PullIt); //Records current and previous switch states
      int currentPullState = lastPullState;

      //error if wrong action taken
      int lastReset1State = digitalRead(ResetIt);
      int currentReset1State = lastReset1State;

      int lastBump1State = digitalRead(BumpIt);
      int currentBump1State = lastBump1State;
      
      //int xResult1 = 0;
      //int yResult1 = 0;
      //int zResult1 = 0;
      
      while(1) //inf loop
      {
        currentPullState = digitalRead(PullIt); //reads Pull it
        if(currentPullState != lastPullState)  //if the switch got moved, then...
        {
           // completion of first half of task
           
          break;
        }
        currentReset1State = digitalRead(ResetIt);
        if(lastReset1State != currentReset1State) //if accidently reset it exit fail
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("F R during P"); //debugging
          delay(2000); //debugging
          return 0;
        }
        currentBump1State = digitalRead(BumpIt);
        if(lastBump1State != currentBump1State)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("F B during P");
          delay(2000);
          return 0;
        }
        //xResult1 = analogRead(BumpItX);
        //yResult1 = analogRead(BumpItY);
        //zResult1 = analogRead(BumpItZ);
        //int netResult1 = sqrt(pow(xResult1,2)+pow(yResult1,2)+pow(zResult1,2));
       // if(netResult1>=bumpMaxAmplitude)  //if accidentaly bump
        //{
         // lcd.clear();  //debugging
         // lcd.setCursor(0,0);
         // lcd.print("F B during P"); //debugging
         // delay(2000); //debugging
         // return 0;
       // }
        unsigned long CurrentTime = millis(); //if not flipped in time, record current time
        unsigned long deltaTime = CurrentTime - StartTime; //find net time since start of task
        if(deltaTime>timeAllowed) //if net time since start of task is greater than allowed time, break out of the loop without changing flag
        {
          lcd.clear();  //debugging
          lcd.print("too long "); //debugging
          delay(2000); //debugging
          return 0;
        }
      }

      //now on to the flipping the switch again, most code will be copied.
      

      lastPullState = digitalRead(PullIt);
      currentPullState = digitalRead(PullIt);

      while(1)
      {
        currentPullState = digitalRead(PullIt);
        if(currentPullState != lastPullState)
        {
          return 1; //switch activated, return success
        }
        currentReset1State = digitalRead(ResetIt);
        if(lastReset1State != currentReset1State) //if accidently reset it exit fail
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("F R during P"); //debugging
          delay(2000); //debugging
          return 0;
        }
        currentBump1State = digitalRead(BumpIt);
        if(lastBump1State != currentBump1State)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("F B during P");
          delay(2000);
          return 0;
        }
        //xResult1 = analogRead(BumpItX);
        //yResult1 = analogRead(BumpItY);
        //zResult1 = analogRead(BumpItZ);
        //int netResult1 = sqrt(pow(xResult1,2)+pow(yResult1,2)+pow(zResult1,2));
       // if(netResult1>=bumpMaxAmplitude)  //if accidentaly bump
        //{
         // lcd.clear();  //debugging
          //lcd.setCursor(0,0);
          //lcd.print("F B during P"); //debugging
         // delay(2000); //debugging
        //  return 0;
       // }
        unsigned long CurrentTime = millis();
        unsigned long deltaTime = CurrentTime - StartTime;
        if(deltaTime>timeAllowed)
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("too long"); //debugging
          delay(2000); //debugging
          return 0; //too much time as elapsed return fail
        }
      }

      break;

    default:  //bump it
      //int xResult = 0;
      //int yResult = 0;
      //int zResult = 0;
      int currentBumpState = digitalRead(BumpIt);
      int lastBumpState = currentBumpState;
      
      int lastReset2State = digitalRead(ResetIt); //Records current and previous reset states
      int currentReset2State = digitalRead(ResetIt);

      int lastPull2State = digitalRead(PullIt); //Records current and previous pull states
      int currentPull2State = digitalRead(PullIt);

      while(1)
      {
        //xResult = analogRead(BumpItX);
        //yResult = analogRead(BumpItY);
        //zResult = analogRead(BumpItZ);


        //below is testing for if pull or reset it is triggered
        currentPull2State = digitalRead(PullIt);
        currentReset2State = digitalRead(ResetIt);

        if(currentPull2State != lastPull2State)  //if triggered then did wrong action and exit fail
        {
          lcd.clear();  //debugging
          lcd.print("F P during B"); //debugging
          delay(2000); //debugging
          return 0;
        }
        if(lastReset2State != currentReset2State)
        {
          lcd.clear();  //debugging
          lcd.print("F R during B"); //debugging
          delay(2000); //debugging
          return 0;
        }


        //int netResult = sqrt(pow(xResult,2)+pow(yResult,2)+pow(zResult,2)); //net analog data
        //if(netResult>= bumpMaxAmplitude)
        //{
        //  return 1;
        //}
        currentBumpState = digitalRead(BumpIt);
        if(lastBumpState!=currentBumpState)
        {
          break;
        }
        unsigned long CurrentTime = millis();
        unsigned long deltaTime = CurrentTime - StartTime;
        if(deltaTime>timeAllowed)
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("too long"); //debugging
          delay(2000); //debugging
          return 0;
        }
      }

      lastBumpState = digitalRead(BumpIt);
      currentBumpState = lastBumpState;
      while(1)
      {
        currentBumpState = digitalRead(BumpIt);
        if(currentBumpState!=lastBumpState)
        {
          return 1;
        }
        currentPull2State = digitalRead(PullIt);
        currentReset2State = digitalRead(ResetIt);
        if(currentPull2State != lastPull2State)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("F P during B");
          delay(2000);
          return 0;

        }
        if(currentReset2State != lastReset2State)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("F R during B");
          delay(2000);
          return 0;
        }
        unsigned long CurrentTime = millis();
        unsigned long deltaTime = CurrentTime - StartTime;
        if(deltaTime>timeAllowed)
        {
          lcd.clear();  //debugging
          lcd.setCursor(0,0);
          lcd.print("too long"); //debugging
          delay(2000); //debugging
          return 0;
        }

      }

    break;
  }
}


void LEDCelebrate() //for winning game
{
  digitalWrite(LEDWin,HIGH);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,HIGH);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,HIGH);
  delay(500);

  digitalWrite(LEDWin,HIGH);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,HIGH);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,HIGH);
  delay(500);

  digitalWrite(LEDWin,HIGH);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,HIGH);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,HIGH);
  delay(500);

  digitalWrite(LEDWin,HIGH);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,HIGH);
  digitalWrite(LEDFail,LOW);
  delay(500);

  digitalWrite(LEDWin,LOW);
  digitalWrite(LEDHold,LOW);
  digitalWrite(LEDFail,HIGH);
  delay(500);
}