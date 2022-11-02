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

int j = 0;

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
    unsigned long timeAllowed = 5000;
    bool winLose = 0;
    while(score<99)
    {
      delay(1000);
      randomSeed(analogRead(SeedPin));
      long RandNumber = random(3); //Will spit out random number 0-2 which will determine actions
      executeCommand(RandNumber); //This command will tell the user what to do, and display a song, and the hold LED

      bool passFail = waitForUserAction(RandNumber,timeAllowed); 
      digitalWrite(LEDHold,LOW);
      if(passFail)
      {
        score = score+1;
        if(score == 10) //change to 99 in final version
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("You Win!");
          lcd.setCursor(0,1);
          lcd.print(String("Score: " + String(score)));

          LEDCelebrate();
          ResetInitial = digitalRead(Start); //also changed this one
          j = 0;
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
          delay(timeAllowed/3);
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
        youLoseTone();
        delay(3000);
        digitalWrite(LEDFail,LOW);
        ResetInitial = digitalRead(Start);  //was reset it
        j = 0;
        break;
      }
      timeAllowed = timeAllowed-30;
    }
  }
  else
  {
    //Tell user to hit reset button to start game on LED
    
    if(j==0)
    {
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print("Press Start"); //20 characters
      j = 1;
    }
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
  unsigned long startTime = millis();
  unsigned long currentTime = startTime;

  int resetState = digitalRead(ResetIt);
  int lastResetState = resetState;

  int pullState = digitalRead(PullIt);
  int lastPullState = pullState;

  int bumpState = digitalRead(BumpIt);
  int lastBumpState = bumpState;
  int i = 0;
  switch(randNumber)
  {
    case 0:  //if reset it
      while(i<2) //will iterate until i is incremented 2 times (two switches occur)
      {
        resetState = digitalRead(ResetIt); //update states
        pullState = digitalRead(PullIt);
        bumpState = digitalRead(BumpIt);
        currentTime = millis();
        if(timeAllowed<(currentTime- startTime)) //fail if run out of time
        {
          return 0;
        }
        if(resetState != lastResetState)
        {
          lastResetState = resetState; //update the previous state
          i = i + 1; //iterate
        }
        if(pullState != lastPullState) //if pulled while seeking reset, fail
        {
          return 0;
        }
        if(bumpState != lastBumpState)//if bumped while seeking reset fail
        {
          return 0; //
        }
      }
      return 1;
      break;
    case 1:  //if pull it
      while(i<2) //iterate until 2 switches happen
      {
        resetState = digitalRead(ResetIt); //update States
        pullState = digitalRead(PullIt);
        bumpState = digitalRead(BumpIt);
        currentTime = millis();
        if(timeAllowed<(currentTime- startTime)) //fail if run out of time
        {
          return 0;
        }
        if(resetState != lastResetState) //if reset while seeking pull, fail
        {
          return 0;
        }
        if(pullState != lastPullState) //if pulled correctly
        {
          lastPullState = pullState; //updated previous state
          i = i + 1; //increment
        }
        if(bumpState != lastBumpState)//if bumped while seeking pull fail
        {
          return 0; //
        }
      }
      return 1;
      break;
    default: //if bump it
      while(i<2) //iterate until 2 switches happen
      {
        resetState = digitalRead(ResetIt); //update States
        pullState = digitalRead(PullIt);
        bumpState = digitalRead(BumpIt);
        currentTime = millis();
        if(timeAllowed<(currentTime- startTime)) //fail if run out of time
        {
          return 0;
        }
        if(resetState != lastResetState)
        {
          return 0; //reset while seeking bump
        }
        if(pullState != lastPullState) //if pulled while seeking bump, fail
        {
          return 0;
        }
        if(bumpState != lastBumpState)//if bumped 
        {
          lastBumpState = bumpState; //
          i = i + 1;
        }
      }
      return 1;
      break;

  }
  return 0;
}


void LEDCelebrate() //for winning game
{
  youWinTone();
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

  digitalWrite(LEDFail,LOW);
  delay(500);
}

void youLoseTone()
{
  tone(Speaker,523); //C5
  delay(100);
  noTone(Speaker);

  tone(Speaker,622);  //Eb5
  delay(100);
  noTone(Speaker);

  tone(Speaker,784); //G5
  delay(100);
  noTone(Speaker);

  tone(Speaker,740); //F#5
  delay(100);
  noTone(Speaker);

  tone(Speaker,587); //D5
  delay(100);
  noTone(Speaker);

  tone(Speaker,494); //B4
  delay(100);
  noTone(Speaker);

  tone(Speaker,466); //Bb4
  delay(200);
  noTone(Speaker);

  tone(Speaker,554); //Db5
  delay(200);
  noTone(Speaker);

  tone(Speaker,698); //F5
  delay(200);
  noTone(Speaker);

  tone(Speaker,932); //Bb5
  delay(400);
  noTone(Speaker);
}

void youWinTone()
{
  tone(Speaker,523); //C5
  delay(100);
  noTone(Speaker);

  tone(Speaker, 587); //D5
  delay(100);
  noTone(Speaker);

  tone(Speaker,659);  //E5
  delay(100);
  noTone(Speaker);

  tone(Speaker,784); //G5
  delay(100);
  noTone(Speaker);

  tone(Speaker,659);  //E5
  delay(100);
  noTone(Speaker);

  tone(Speaker,587); //D5
  delay(100);
  noTone(Speaker);

  tone(Speaker,523); //C5
  delay(100);
  noTone(Speaker);

  tone(Speaker,523); //C5
  delay(100);
  noTone(Speaker);

  tone(Speaker,587); //D5
  delay(100);
  noTone(Speaker);

  tone(Speaker,659);  //E5
  delay(100);
  noTone(Speaker);

  tone(Speaker,784); //G5
  delay(100);
  noTone(Speaker);

  tone(Speaker,659);  //E5
  delay(100);
  noTone(Speaker);

  tone(Speaker,587); //D5
  delay(100);
  noTone(Speaker);

  tone(Speaker,523); //C5
  delay(100);
  noTone(Speaker);

  tone(Speaker,523); //C5
  delay(100);
  noTone(Speaker);

  tone(Speaker,587); //D5
  delay(100);
  noTone(Speaker);

  tone(Speaker,659);  //E5
  delay(100);
  noTone(Speaker);

  tone(Speaker,784); //G5
  delay(200);
  noTone(Speaker);

  tone(Speaker,698);  //F5
  delay(200);
  noTone(Speaker);

  tone(Speaker,659); //E5
  delay(200);
  noTone(Speaker);

  tone(Speaker,1026); //C6
  delay(500);
  noTone(Speaker);

  



  
}
