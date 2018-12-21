#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DOWN_BUTTON     8
#define UP_BUTTON       12
#define POWER_BUTTON    13
#define BUZZER          3
#define SERVO           5
#define DEFAULT_SPEED   95                // Servo offset = 5

unsigned long timeLeftHours = 1;
unsigned long prevTimeLeftHours;
unsigned long timerDurationMillis;
unsigned long previousMillis;
int timerOn = false;
int powerButtonState;
int downButtonState;
int upButtonState;
int lastPowerButtonState = LOW;
int lastDownButtonState = LOW;
int lastUpButtonState = LOW;
unsigned long lastPowerDebounceTime = 0;
unsigned long lastDownDebounceTime = 0;
unsigned long lastUpDebounceTime = 0;
unsigned long debounceDelay = 50;         // Delay (in milliseconds) before allowing another button press       

Servo servo;
LiquidCrystal_I2C LCD(0x27, 16, 2);       // Set the LCD address to 0x27 (16 characters, 2 lines)

void setup() {
  pinMode(POWER_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT); 
  pinMode(BUZZER, OUTPUT);
  servo.attach(SERVO);
  LCD.init();
  LCD.backlight();
    
  servo.write(DEFAULT_SPEED);
  displayTime(timeLeftHours);
}

void loop() {
  checkButton(POWER_BUTTON);

  if (timerOn) {
    runTimer();
  } else {
    checkButton(DOWN_BUTTON);
    checkButton(UP_BUTTON);
  }
}

int checkButton(int button) {
  int reading = digitalRead(button);

  switch(button) {
    case POWER_BUTTON:
      if (reading != lastPowerButtonState)
        lastPowerDebounceTime = millis();

      if ((millis() - lastPowerDebounceTime) > debounceDelay) {
        if (reading != powerButtonState) {
          powerButtonState = reading;

          if (powerButtonState == HIGH) {
            if (!timerOn) {
              timerDurationMillis = timeLeftHours * 60 * 60 * 1000;
              timerOn = true;
            } else {
              displayOpeningDoor();
              openDoor();
              timerOn = false;
              displayTime(timeLeftHours);
            }
          }
        }
      }

      lastPowerButtonState = reading;
      break;
      
    case DOWN_BUTTON:
      if (reading != lastDownButtonState)
        lastDownDebounceTime = millis();

      if ((millis() - lastDownDebounceTime) > debounceDelay) {
        if (reading != downButtonState) {
          downButtonState = reading;

          if (downButtonState == HIGH && timeLeftHours > 1) {
            if (!timerOn) {
              timeLeftHours--;
              displayTime(timeLeftHours);
            }
          }
        }
      }

      lastDownButtonState = reading;
      break;
      
    case UP_BUTTON:
      if (reading != lastUpButtonState)
        lastUpDebounceTime = millis();

      if ((millis() - lastUpDebounceTime) > debounceDelay) {
        if (reading != upButtonState) {
          upButtonState = reading;

          if (upButtonState == HIGH && timeLeftHours < 24) {
            if (!timerOn) {
              timeLeftHours++;
              displayTime(timeLeftHours);
            }
          }
        }
      }

      lastUpButtonState = reading;
      break;
   }
}

void runTimer() {
  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) <= timerDurationMillis) { 
    timeLeftHours = ((((timerDurationMillis - (currentMillis - previousMillis)) / 1000) / 60) / 60) + 1;
  } else {
    displayDone();
    playSound();
    openDoor();
    timerOn = false;
    displayTime(timeLeftHours);
  }

  if (prevTimeLeftHours != timeLeftHours) {
    displayTime(timeLeftHours);
    prevTimeLeftHours = timeLeftHours;
  }
}

void displayTime(int time) {
  LCD.clear();
  
  if (timerOn) {  
    LCD.setCursor(0,0);  
    LCD.print("Timer running...");
    LCD.setCursor(0,1);
    LCD.print("Time left: ");
  } else {
    LCD.setCursor(3,0);
    LCD.print("Timer OFF");
    LCD.setCursor(0,1);
    LCD.print("Set timer: ");
  }

  LCD.print(time);
  LCD.setCursor(13, 1);
  if (time == 1) {
    LCD.print("hr");
  } else {
    LCD.print("hrs");
  }
}

void displayDone() {
  LCD.clear();
  
  LCD.setCursor(2, 0);   
  LCD.print("OPENING DOOR");
  
  LCD.setCursor(1,1);
  LCD.print("Fat Cat Time!");
}

void displayClosingDoor() {
  LCD.clear();
  
  LCD.setCursor(2, 0);
  LCD.print("CLOSING DOOR");
  
  LCD.setCursor(1, 1);
  LCD.print("Please wait...");
}

void displayOpeningDoor() {
  LCD.clear();
  
  LCD.setCursor(2, 0);
  LCD.print("OPENING DOOR");
  
  LCD.setCursor(1, 1);
  LCD.print("Please wait...");
}

void playSound() {
  tone(BUZZER, 500);
  delay(4000);
  noTone(BUZZER);
}

void openDoor() {
  servo.write(5);
  delay(5000);
  servo.write(DEFAULT_SPEED);
}

void closeDoor() {
  servo.write(185);
  delay(5000);
  servo.write(DEFAULT_SPEED);
}
