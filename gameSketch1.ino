#include "pitches.h"

//defs for pins
#define TOP_SWITCH1_IN 2 //d (interrupt)
#define TOP_SWITCH2_IN 3 //d (interrupt)
#define BOT_SWITCH1_IN A4 //(analog as digital)
#define BOT_SWITCH2_IN A5 //(analog as digital)
#define START_BUTTON_IN 4 //d
#define PHOTO1_IN A2 //a
#define PHOTO2_IN A3 //a

#define ENABLE_LASERS_OUT 7 //d
#define START_BLINK_OUT 8 //d
#define LIGHT1_OUT 12 //d
#define LIGHT2_OUT 13 //d
#define GUN_FB1_OUT A0 //(analog as digital)
#define GUN_FB2_OUT A1 //(analog as digital)
#define MOTOR1_1_OUT 10 //pwm
#define MOTOR1_2_OUT 11 //pwm
#define MOTOR2_1_OUT 5 //pwm
#define MOTOR2_2_OUT 6 //pwm
#define AUDIO_OUT 9 //a

//defs for THRESHOLDs 
#define PHOTO_THRESHOLD1 256 //TODO
#define PHOTO_THRESHOLD2 256 //TODO
#define MOTOR_THRESHOLD 256 //TODO

//defs for melody sizes
#define MEL1_SIZE 5
#define MEL2_SIZE 5

//var for game started
boolean gameStart = false;

//var for debugging
boolean debugging = true;

//var arr for melodies
int melody1[MEL1_SIZE] = {
  NOTE_C3, NOTE_E3, NOTE_G3, NOTE_G3, NOTE_C4};
int melody2[MEL2_SIZE] = {
  NOTE_C3, NOTE_DS3, NOTE_G3, NOTE_G3, NOTE_C4};

//var arr for durations
int noteDurations1[MEL1_SIZE] = {4, 4, 4, 4, 4};
int noteDurations2[MEL2_SIZE] = {4, 4, 4, 4, 4};

void setup() {

  //initialize inputs and outputs
  pinMode(TOP_SWITCH1_IN, INPUT);
  digitalWrite(TOP_SWITCH1_IN, HIGH);
  pinMode(TOP_SWITCH2_IN, INPUT);
  digitalWrite(TOP_SWITCH2_IN, HIGH);
  pinMode(BOT_SWITCH1_IN, INPUT);
  digitalWrite(BOT_SWITCH1_IN, HIGH);
  pinMode(BOT_SWITCH2_IN, INPUT);
  digitalWrite(BOT_SWITCH2_IN, HIGH);
  pinMode(PHOTO1_IN, INPUT);
  pinMode(PHOTO2_IN, INPUT);
  pinMode(START_BUTTON_IN, INPUT);
  digitalWrite(START_BUTTON_IN, HIGH);

  pinMode(ENABLE_LASERS_OUT, OUTPUT);
  pinMode(START_BLINK_OUT, OUTPUT);
  pinMode(LIGHT1_OUT, OUTPUT);
  pinMode(LIGHT2_OUT, OUTPUT);
  pinMode(GUN_FB1_OUT, OUTPUT);
  pinMode(GUN_FB2_OUT, OUTPUT);
  pinMode(MOTOR1_1_OUT, OUTPUT);
  pinMode(MOTOR1_2_OUT, OUTPUT);
  pinMode(MOTOR2_1_OUT, OUTPUT);
  pinMode(MOTOR2_2_OUT, OUTPUT);
  pinMode(AUDIO_OUT, OUTPUT); 

  //enable interrupt for end-of-game - 0 = (pin 2)
  attachInterrupt(0, intFuncTop1, FALLING);
  attachInterrupt(1, intFuncTop2, FALLING);

  if (debugging) {
    Serial.begin(9600);  // i can haz serial plz?
  }

  activeWait();
}

void activeWait() {
  //DEBUG
  if (debugging) {
    Serial.println("activeWait() entered");
  }

  while (!gameStart) {
    int startButtonState = digitalRead(START_BUTTON_IN);

    //DEBUG
    if (debugging) {
      Serial.println("StartButtonState = " + startButtonState);
    }

    //if start button on, set game start and enable lasers
    if (HIGH != startButtonState) {
      gameStart = true;
      digitalWrite(ENABLE_LASERS_OUT, HIGH);

      //DEBUG
      if (debugging) {
        Serial.println("Starting game");
      }
    } else {
      //else blink
      digitalWrite(START_BLINK_OUT, HIGH);
      delay(3000);
      digitalWrite(START_BLINK_OUT, LOW);
      delay(3000);
    }
  }
  //Once we're out of this loop, it returns and leaps to loop()
}

void loop() {
  //DEBUG
  if (debugging) {
    Serial.println("loop() entered");
  }
  
  while (gameStart) {
    //read from photo inputs
    int photo1State = analogRead(PHOTO1_IN);
    photo1State = map(photo1State, 0, 1023, 0, 255);
    int photo2State = analogRead(PHOTO2_IN);
    photo2State = map(photo2State, 0, 1023, 0, 255);

    //DEBUG
    if (debugging) {
      Serial.println("Photo1 state: " + photo1State);
      Serial.println("Photo2 state: " + photo2State);
    }

    //if photo input 1
    if (photo1State > PHOTO_THRESHOLD1) {
      //Give gun feedback
      digitalWrite(GUN_FB1_OUT, HIGH);
      delay(5000);
      digitalWrite(GUN_FB1_OUT, LOW);

      //DEBUG
      if (debugging) {
        Serial.println("Writing to motor_1_1_out at : " + MOTOR_THRESHOLD);
      }

      //motor 1 up
      analogWrite(MOTOR1_1_OUT, MOTOR_THRESHOLD);
      delay(2000);
      analogWrite(MOTOR1_1_OUT, LOW);
    }

    //if photo input 2
    if (photo2State > PHOTO_THRESHOLD2) {
      //Give gun feedback
      digitalWrite(GUN_FB2_OUT, HIGH);
      delay(5000);
      digitalWrite(GUN_FB2_OUT, LOW);

      //DEBUG
      if (debugging) {
        Serial.println("Writing to motor_2_1_out at : " + MOTOR_THRESHOLD);
      }

      //motor 2 up
      analogWrite(MOTOR2_1_OUT, MOTOR_THRESHOLD);
      delay(2000);
      analogWrite(MOTOR2_1_OUT, LOW);
    }
  }
  
  //If we're no longer in the gameStart loop, go back to waiting.
  //Waiting will return here, which loops back up
  activeWait();
}

void intFuncTop1() {
  //DEBUG
  if (debugging) {
    Serial.println("Interrupt called.");
  }

  //We've hit one of the top switches
  //Disable the lasers
  digitalWrite(ENABLE_LASERS_OUT, LOW);

  //DEBUG
  if (debugging) {
    Serial.println("Player 1 wins");
  }

  //Light up that player's light
  digitalWrite(LIGHT1_OUT, HIGH);

  //Play shiny music
  playMusic(1);

  //Send both players back to bottom    
  returnToHome();

  //Disable light
  digitalWrite(LIGHT1_OUT, LOW);

  //Reset game state
  gameStart = false;
}

void intFuncTop2() {
  //DEBUG
  if (debugging) {
    Serial.println("Interrupt called.");
  }

  //We've hit one of the top switches
  //Disable the lasers
  digitalWrite(ENABLE_LASERS_OUT, LOW);

  //DEBUG
  if (debugging) {
    Serial.println("Player 2 wins");
  }

  //Light up that player's light
  digitalWrite(LIGHT2_OUT, HIGH);

  //Play shiny music
  playMusic(2);

  //Send both players back to bottom    
  returnToHome();

  //Disable light
  digitalWrite(LIGHT2_OUT, LOW);

  //Reset game state
  gameStart = false;
}


void returnToHome() {
  //DEBUG
  if (debugging) {
    Serial.println("Return to home called.");
  }

  boolean done = false;
  int botSwitch1State = 0;
  int botSwitch2State = 0;

  while (!done) {
    botSwitch1State = digitalRead(BOT_SWITCH1_IN);
    botSwitch2State = digitalRead(BOT_SWITCH2_IN);

    //DEBUG
    if (debugging) {
      Serial.println("Bottom switch 1 state: " + botSwitch1State);
      Serial.println("Bottom switch 2 state: " + botSwitch2State);
    }

    if (HIGH == botSwitch1State) {
      //DEBUG
      if (debugging) {
        Serial.println("Writing to motor1_2_out at : " + MOTOR_THRESHOLD);
      }

      analogWrite(MOTOR1_2_OUT, MOTOR_THRESHOLD);
    }

    if (HIGH == botSwitch2State) {
      //DEBUG
      if (debugging) {
        Serial.println("Writing to motor2_2_out at : " + MOTOR_THRESHOLD);
      }

      analogWrite(MOTOR2_2_OUT, MOTOR_THRESHOLD);
    }

    if (HIGH == botSwitch1State && HIGH == botSwitch2State) {
      //DEBUG
      if (debugging) {
        Serial.println("Done moving to home");
      }

      analogWrite(MOTOR2_2_OUT, LOW);
      analogWrite(MOTOR1_2_OUT, LOW);
      done = true;
    }
  }

}

void playMusic(int player) {
  if (1 == player) {
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < MEL1_SIZE; thisNote++) {

      // to calculate the note duration, take one second 
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000/noteDurations1[thisNote];
      tone(8, melody1[thisNote],noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);
    }
  } 
  else if (2 == player) {
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < MEL2_SIZE; thisNote++) {

      // to calculate the note duration, take one second 
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000/noteDurations2[thisNote];
      tone(8, melody1[thisNote],noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);
    }
  }
}

