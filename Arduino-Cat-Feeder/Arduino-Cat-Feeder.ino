#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DOWN_BUTTON     8
#define UP_BUTTON       12
#define POWER_BUTTON    13
#define MODE_BUTTON     7
#define BUZZER          3
#define SERVO           10
#define DEFAULT_SPEED   95                // Servo offset = 5

unsigned long timeLeftHours = 1;
unsigned long intervalHours = 1;
unsigned long prevTimeLeftHours;
unsigned long timerDurationMillis;
unsigned long previousMillis = 0;
int timerOn = false;
int mode = 1;
int powerButtonState;
int modeButtonState;
int downButtonState;
int upButtonState;
int lastPowerButtonState = LOW;
int lastModeButtonState = LOW;
int lastDownButtonState = LOW;
int lastUpButtonState = LOW;
unsigned long lastPowerDebounceTime = 0;
unsigned long lastModeDebounceTime = 0;
unsigned long lastDownDebounceTime = 0;
unsigned long lastUpDebounceTime = 0;
unsigned long debounceDelay = 50;         // Delay (in milliseconds) before allowing another button press       

Servo servo;
LiquidCrystal_I2C LCD(0x27, 16, 2);       // Set the LCD address to 0x27 (16 characters, 2 lines)

void setup() {
  pinMode(POWER_BUTTON, INPUT);
  pinMode(MODE_BUTTON, INPUT);
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
    checkButton(MODE_BUTTON);
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
              intervalHours = timeLeftHours;
              timerDurationMillis = timeLeftHours * 60 * 60 * 1000;
              timerOn = true;
              displayTime(timeLeftHours);
            } else {
              if (mode == 3)
                dispenseFood();

              timerOn = false;
              timeLeftHours = 1;
              displayTime(timeLeftHours);
            }
          }
        }
      }

      lastPowerButtonState = reading;
      break;

    case MODE_BUTTON:
      if (reading != lastModeButtonState)
        lastModeDebounceTime = millis();

      if ((millis() - lastModeDebounceTime) > debounceDelay) {
        if (reading != modeButtonState) {
          modeButtonState = reading;

          if (modeButtonState == HIGH) {
            if (!timerOn) {
              if (mode == 3) mode = 0;
              mode++;
              
              displayTime(timeLeftHours);
            }
          }
        }
      }

      lastModeButtonState = reading;   
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
    previousMillis = currentMillis;
    playSound();
    dispenseFood();
    if (mode == 2) {
      timeLeftHours = intervalHours;
      timerDurationMillis = timeLeftHours * 60 * 60 * 1000;
    } else {
      timerOn = false;
      timeLeftHours = 1;
    }
    
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
    LCD.print("Timer:ON  Mode:");
    LCD.print(mode);
    LCD.setCursor(0,1);
    LCD.print("Time left:<");
  } else {
    LCD.setCursor(0,0);
    LCD.print("Timer:OFF Mode:");
    LCD.print(mode);
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

void dispenseFood() {
  displayDispensingFood();
  playSound();
  
  servo.write(105);
  delay(5325);
  servo.write(DEFAULT_SPEED);
}

void displayDispensingFood() {
  LCD.clear();
  
  LCD.setCursor(0, 0);
  LCD.print("DISPENSING FOOD");
  
  LCD.setCursor(1, 1);
  LCD.print("Please wait...");
}

void playSound() {
  tone(BUZZER, 950);
  delay(800);
  tone(BUZZER, 750);
  delay(800);
  tone(BUZZER, 850);
  delay(800);
  tone(BUZZER, 550);
  delay(1000);
  noTone(BUZZER);
  delay(500);
  tone(BUZZER, 550);
  delay(800);
  tone(BUZZER, 850);
  delay(800);
  tone(BUZZER, 950);
  delay(800);
  tone(BUZZER, 750);
  delay(1000);
  noTone(BUZZER);
}
