#include "pitches.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Global Var
#define contra 9 //Define the pin that controls the contrast of the screen
#define bri 10 //Define the pin the controls the brightness of the screen

//Touch
int TouchSensor = A0;

// These variables are for timing purposes
int kk = 1;
int time_out = 1;
bool state_on = 0;


//Content Input
int displayStatus = 0;
String Name = "Buddy!";
String Task = "Do HW1";
double over = 240000; //Static FOCUS time after time lapse
double player1Time = 240000;    // Timer 1 - FOCUS
double player2Time = 60000;    // Timer 2 - BREAK

//Timer
int player1Minutes = 0;
int player1Seconds = 0;
double player1LastCheck = millis();

int player2Minutes = 0;
int player2Seconds = 0;
double player2LastCheck = millis();

bool isPlayer1Turn = true;
bool gameOver = false;
int Trigger = 0;
int Alarm1 = 0;
int Alarm2 = 0;

//Touch Instructions
int tapOne = 0;
int tapTwo = 0;
int tapThree = 0;

void setup() {
  
//Touch
pinMode(TouchSensor, INPUT);
pinMode(7, OUTPUT);
pinMode(13, OUTPUT);

// initialize the serial communications:
Serial.begin(9600);

lcd.begin(16, 2); //Tell the LCD that it is a 16x2 LCD
pinMode(contra, OUTPUT); //set pin 9 to OUTPUT
pinMode(bri, OUTPUT); //Set pin 10 to OUTPUT

digitalWrite(contra, LOW); /*outputs no power to the contrast pin. This lets you see the words*/
analogWrite(bri, 200); //Outputs full power to the screen brightness LED

//TIMER
  UpdatePlayer1Time();
  UpdatePlayer2Time();  
}

void loop() 
{ 
  //Touch Enabled
  //TOUCH Trigger-----------------------------------------------------
  time_out++; // iterate
  int touch_state = digitalRead(TouchSensor);
  //lcd.print(touch_state);

  // if statement asking how many times it has been triggered:
  if (touch_state==1 and state_on == 0) {
    kk++;
    state_on = 1;
    time_out = 1;
    delay(50);
  } else if (state_on==1 and touch_state == 0) {
    state_on = 0;
    delay(50);
  }
  if (time_out>70){ // change this to change how quickly the touch sensor must be touched to register changes
    
    if (kk==2){                       //For single tap - FOCCUS timer                                        
      StartPlayer1Turn();    
      Trigger = 1;
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      lcd.print("Task: ");
      lcd.print(Task);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      Alarm1 = 0;
     } 
     
     else if (kk==3){                //For two taps - BREAK timer 
      StartPlayer2Turn();
      Trigger = 1;
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      lcd.print("Task: ");
      lcd.print(Task);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      Alarm2 = 0;
   } 
   
   else if (kk==4){                       //For three taps - Complete Task

      Trigger = 0;                      // Stop the timer
      
      //TASK COMPLETE

      if(Alarm1 == 0 && Alarm2 == 0)
     { 
       //Calculate the time saved
      double totalTime = player1Time + player2Time;
     // double totalLastTime = player1LastCheck + player2LastCheck;
     // double newTime = totalTime - totalLastTime;  
      int newMinute;
      int newSeconds;
      //Separate out the minutes and seconds from the time value (which is in milliseconds).
      newMinute = floor(totalTime / 60000);
      newSeconds = floor(totalTime / 1000) - newMinute * 60;
 
      //Print the minutes, then a colon, then the seconds.
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      lcd.print("Yass! You saved:");
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(newMinute);
      lcd.print(":");
      if (newSeconds < 10) {
         lcd.print(0);
      }
      lcd.print(newSeconds);
      lcd.print(" mins.");

      tone(8, NOTE_FS7, 100);
      digitalWrite(13, HIGH);
      digitalWrite(7, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      digitalWrite(7, LOW);
      delay(200);
      digitalWrite(13, HIGH);
      digitalWrite(7, HIGH);
     }
     
     if(gameOver)
     {
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      lcd.print("Hey, task done");
      //lcd.print(Name);
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("over ");
      int newMinute1;
      int newSeconds1;
      //Separate out the minutes and seconds from the time value (which is in milliseconds).
      newMinute1 = floor(over / 60000);
      newSeconds1 = floor(over / 1000) - newMinute1 * 60;
      lcd.print(newMinute1);
      lcd.print(":");
          if (newSeconds1 < 10) {
           lcd.print(0);
          }
      lcd.print(newSeconds1);
      lcd.print(" mins.");
      tone(8, NOTE_FS2, 100);
      digitalWrite(13, LOW);
      digitalWrite(7, LOW);
      
     }   
     // RestartGame();        // Reset

     
    }   kk = 1;
    time_out = 1;
    delay(10);
  }
  delay(2); // short pause between reads
  //TOUCH Trigger-----------------------------------------------------


//Whichever player is currently active--update their clock.
  if (Trigger == 1)             //Update timer after touch.
  {
    if (isPlayer1Turn) {
      UpdatePlayer1Time();
    } else {
     UpdatePlayer2Time();
    }
  }
  
  if(digitalRead(TouchSensor)==HIGH)
  { 
    analogWrite(6, 255); //haptic  
    displayStatus = 1;
  }


  else if(digitalRead(TouchSensor)==LOW)
  {
      digitalWrite(6, LOW);

      //ALARM
      if (Alarm1 == 1)
      {
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,0);
        lcd.print("Task: ");
        //lcd.print(Task);
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("FOCUS time over!");
      tone(8, NOTE_B5, 25);
      analogWrite(6, 225);
      digitalWrite(7, HIGH);
      delay(200);
      analogWrite(6, LOW);
      digitalWrite(7, LOW);
      delay(200);
      tone(8, NOTE_B5, 25);
      analogWrite(6, 225);
      digitalWrite(7, HIGH);
      delay(200);
      analogWrite(6, LOW);
      digitalWrite(7, LOW);
      noTone(8); 
     }

     if(Alarm2 ==1)
     {
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,0);
        lcd.print("Task: ");
        //lcd.print(Task);
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("BREAK time over!");
      tone(8, NOTE_B5, 25);
      analogWrite(6, 225);
      digitalWrite(13, HIGH);
      delay(200);
      analogWrite(6, LOW);
      digitalWrite(13, LOW);
      delay(200);
      tone(8, NOTE_B5, 25);
      analogWrite(6, 225);
      digitalWrite(13, HIGH);
      delay(200);
      analogWrite(6, LOW);
      digitalWrite(13, LOW);
      noTone(8); 
    }

      //INTRO                  
      if (millis() > 1500 && millis() < 2500) { 
      lcd.setCursor(0,1);                         
      lcd.print("NEURON, ");
      lcd.print(Name);                      // print user name
      
      tone(8, NOTE_C4, 25);
      analogWrite(6, 225);
      digitalWrite(7, HIGH);
      digitalWrite(13, HIGH);
      delay(200);
      analogWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(13, LOW);
      delay(200);
      tone(8, NOTE_C4, 25);
      analogWrite(6, 225);
      digitalWrite(7, HIGH);
      digitalWrite(13, HIGH);
      delay(200);
      analogWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(13, LOW);
      delay(1000);
      noTone(8);
            for (int l = 0; l < 16; l++) { //For loop. Repeating 16 times
                lcd.scrollDisplayRight(); //Scroll whole screen to the right once
                delay(50); //Slight delay for animation
            }
            for (int l = 0; l < 16; l++){
                lcd.scrollDisplayLeft();
                //delay(90);
            }
            //delay(500);
      }
      else if (millis() > 3500 && millis() < 6500)
      {
          if (displayStatus == 0)
        {
          lcd.setCursor(0,0);
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("Tap once to:");
          delay(300);
          tapOne++;
          if (tapOne == 1)
          {
            digitalWrite(6, HIGH); 
          }
          lcd.setCursor(0,1);                         
          lcd.print("                ");
          lcd.setCursor(0,1);
          lcd.print("Start FOCUS time");
          delay(200);
        }
      }
      else if (millis() > 6500 && millis() < 9500)
      {
             if (displayStatus == 0)
        {
          lcd.setCursor(0,0);
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("Tap twice to:");
          delay(300);
          tapTwo++;
          if (tapTwo == 1)
          {
              digitalWrite(6, HIGH);
              delay(100);
              digitalWrite(6, LOW);
              delay(100);
              digitalWrite(6, HIGH);
          }
          lcd.setCursor(0,1);                         
          lcd.print("                ");
          lcd.setCursor(0,1);
          lcd.print("Start BREAK time");  
          delay(200);      
        }
      }
      else if (millis() > 9500 && millis() < 12500)
      {
             if (displayStatus == 0)
        {
          lcd.setCursor(0,0);
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("Tap thrice when:");
          delay(300);
          tapThree++;
          if (tapThree == 1)
          {
              digitalWrite(6, HIGH);
              delay(100);
              digitalWrite(6, LOW);
              delay(100);
              digitalWrite(6, HIGH);
              delay(100);
              digitalWrite(6, LOW);
              delay(100);
              digitalWrite(6, HIGH);
          }
          lcd.setCursor(0,1);                         
          lcd.print("                ");
          lcd.setCursor(0,1);
          lcd.print("DONE with a task");  
          delay(200);      
        }
      }
       else if (millis() > 12500 && millis() < 15500)
      {
             if (displayStatus == 0)
        {
          lcd.setCursor(0,0);
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("Your task:");
          delay(300);
          lcd.setCursor(0,1);                         
          lcd.print("                ");
          lcd.setCursor(0,1);
          lcd.print(Task);
          delay(200);  
        }
      }
      else if (millis() > 15500 && millis() < 18500)
      {
             if (displayStatus == 0)
        {
          digitalWrite(7, LOW);
          lcd.setCursor(0,1);                         
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("Your FOCUS time:");
          delay(300);
          lcd.setCursor(0,1);
          lcd.print(player1Minutes);
          lcd.print(":");
          if (player1Seconds < 10) {
           lcd.print(0);
          }
          lcd.print(player1Seconds);
          digitalWrite(7, HIGH);     
          delay(200);
          digitalWrite(7, LOW);
        }
      }
       else if (millis() > 18500 && millis() < 21500)
      {
             if (displayStatus == 0)
        {
          digitalWrite(13, LOW);
          lcd.setCursor(0,1);                         
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("Your BREAK time:");
          delay(300);
          lcd.setCursor(0,1);
          lcd.print(player2Minutes);
          lcd.print(":");
          if (player2Seconds < 10) {
             lcd.print(0);
          }
          lcd.print(player2Seconds);
          digitalWrite(13, HIGH);
          delay(200);
          digitalWrite(13, LOW);
        }
      }
      else if (millis() > 21500 && millis() < 24500)
      {
             if (displayStatus == 0)
        {
          lcd.setCursor(0,0);
          lcd.print("                ");
          lcd.setCursor(0,0);
          lcd.print("Tap to:");
          delay(300);
           lcd.setCursor(0,1);                         
          lcd.print("                ");
          lcd.setCursor(0,1);
          lcd.print("Begin your task");
          delay(200);
          displayStatus = 1;        
        }
      }
      else {
        if (displayStatus == 0)
        {
          lcd.setCursor(0,0);
          lcd.print("Welcome to");
        }
      }
  }
}

void UpdatePlayer1Time() {
  //Tell the LCD screen that we're going to be writing on the first line, where player one's time is displayed.
  lcd.setCursor(0,1);
  if(Trigger==1){
    lcd.print("FOCUS time ");
    }
  //Update the variable that contains the number of milliseconds left on player one's timer.
  // millis() is a built-in function that returns the number of milliseconds that have elapsed
  //since the Arduino was turned on. We check how many milliseconds  have elapsed since last time
  //we ran this function.
  player1Time -= ((millis() - player1LastCheck));
  //If the player is out of time, they lose and the game is over.
  if (player1Time <= 0) {
    gameOver = true;
    Alarm1++;
    return;
  }
  

  player1LastCheck = millis();
  //Separate out the minutes and seconds from the time value (which is in milliseconds).
  player1Minutes = floor(player1Time / 60000);
  player1Seconds = floor(player1Time / 1000) - player1Minutes * 60;
 
  //Print the minutes, then a colon, then the seconds.
  if(Trigger==1){
    lcd.print(player1Minutes);
  lcd.print(":");
  if (player1Seconds < 10) {
    lcd.print(0);
  }
  lcd.print(player1Seconds);
  }
  
}

void UpdatePlayer2Time() {
  lcd.setCursor(0, 1);
  if(Trigger==1){
    lcd.print("BREAK time ");
    }
  player2Time -= ((millis() - player2LastCheck));
  if (player2Time <= 0) {
    gameOver = true;
    Alarm2++;
    return;
  }
  player2LastCheck = millis();
  player2Minutes = floor(player2Time / 60000);
  player2Seconds = floor(player2Time / 1000) - player2Minutes * 60;

  if (Trigger == 1) {
  lcd.print(player2Minutes);
  lcd.print(":");
  if (player2Seconds < 10) {
    lcd.print(0);
  }
  lcd.print(player2Seconds);
  
  }
  
}


void StartPlayer1Turn () {
    if (isPlayer1Turn) {
      if(Trigger==1)            //Trigger timer after touch.
      {
      return;
      }
    }
  tone(8, NOTE_B5, 5);
         digitalWrite(7, HIGH);
         digitalWrite(13, LOW);
         digitalWrite(7, LOW);
         delay(200);
         tone(8, NOTE_C4, 5);
         digitalWrite(7, HIGH);
  isPlayer1Turn = true;
  player1LastCheck = millis();
}


void StartPlayer2Turn () {
    if (!isPlayer1Turn) {
    return;
    }
     tone(8, NOTE_C4, 5);
        digitalWrite(7, LOW);
        digitalWrite(13, HIGH);
        digitalWrite(13, LOW);
        delay(200);
        tone(8, NOTE_B5, 5);
        digitalWrite(13, HIGH);
  isPlayer1Turn = false;
  player2LastCheck = millis();
}


//void RestartGame() {
//  StartPlayer1Turn();
//  player1Time = 300000;
//  player2Time = 300000;
//  gameOver = false;
//  UpdatePlayer1Time();
//  UpdatePlayer2Time();
//}


