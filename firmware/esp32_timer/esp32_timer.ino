#include <Arduino.h>

const int buzzerPin = 18;
const int buttonPin = 19;
const int freq = 2000;
const int channel = 0;
const int resolution = 8;

enum State { SETUP_TIME, SETUP_SECTION, RUNNING, REPORTING, CHOICE };
State currentState = SETUP_TIME;

unsigned long startTime = 0;
int totalSeconds = 0;
int lastSecond = -1;
int timeTook = 0;
int totalQuestions = 0;
int correctAnswers = 0;
String sectionLabel = "";

void clearSerialBuffer() {
  while (Serial.available() > 0) {
    Serial.read();
    delay(2);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(buzzerPin, channel);
  ledcWrite(channel, 0);
  
  Serial.println("--- PSYCHOMETRY EXAM TIMER ---");
  Serial.println("Enter exam duration in minutes (e.g., 0.5 or 20):");
}

void loop() {
  switch (currentState) {
    case SETUP_TIME:
      if (Serial.available() > 0) {
        float inputMinutes = Serial.parseFloat(); 
        clearSerialBuffer(); 

        if (inputMinutes > 0) {
          totalSeconds = (int)(inputMinutes * 60.0);
          Serial.println("\nSelect Section:");
          Serial.println("A - כמותי");
          Serial.println("B - מילולי");
          Serial.println("C - אנגלית");
          currentState = SETUP_SECTION;
        }
      }
      break;

    case SETUP_SECTION:
      if (Serial.available() > 0) {
        char sectionChoice = Serial.read();
        clearSerialBuffer();

        if (sectionChoice == 'A' || sectionChoice == 'a') {
          sectionLabel = "כמותי";
        } else if (sectionChoice == 'B' || sectionChoice == 'b') {
          sectionLabel = "מילולי";
        } else if (sectionChoice == 'C' || sectionChoice == 'c') {
          sectionLabel = "אנגלית";
        } else {
          sectionLabel = "Unknown";
        }

        Serial.print("Section Selected: "); Serial.println(sectionLabel);
        Serial.println("Starting now... Press the physical button to stop early!");
        
        // Wait a small moment to ensure the button isn't accidentally held down
        delay(500); 
        startTime = millis();
        lastSecond = -1; 
        currentState = RUNNING;
      }
      break;

    case RUNNING:
      {
        int elapsed = (millis() - startTime) / 1000;
        int remaining = totalSeconds - elapsed;
        
        if (remaining >= 0 && remaining != lastSecond) {
          Serial.print("Time Remaining: "); Serial.print(remaining); Serial.println(" seconds");
          lastSecond = remaining;
        }

        // Check if the physical button is pressed to stop early
        if (digitalRead(buttonPin) == LOW) {
          timeTook = elapsed; 
          Serial.println("\n--- TIMER STOPPED EARLY BY USER ---");
          for (int i = 0; i < 3; i++) {
            ledcWrite(channel, 128); delay(150);
            ledcWrite(channel, 0); delay(150);
          }
          Serial.println("How many questions were there in total?");
          currentState = REPORTING;
          break;
        }
        
        if (remaining <= 0) {
          timeTook = totalSeconds; 
          Serial.println("\n--- TIME IS UP ---");
          for (int i = 0; i < 5; i++) {
            ledcWrite(channel, 128); delay(200);
            ledcWrite(channel, 0); delay(200);
          }
          Serial.println("How many questions were there in total?");
          currentState = REPORTING;
        }
      }
      break;

    case REPORTING:
      if (Serial.available() > 0) {
        totalQuestions = Serial.parseInt();
        clearSerialBuffer();
        
        Serial.println("How many did you get correct?");
        while (Serial.available() == 0) { delay(10); } 
        
        correctAnswers = Serial.parseInt();
        clearSerialBuffer();
        
        // Outputting the formatted string for the Python listener to catch
        Serial.println("\n--- RESULTS SENT TO PC ---");
        
        Serial.print("CSV_DATA:");
        Serial.print(sectionLabel); Serial.print(",");
        Serial.print(timeTook); Serial.print(",");
        Serial.print(totalSeconds); Serial.print(",");
        Serial.print(totalQuestions); Serial.print(",");
        Serial.println(correctAnswers);
        
        Serial.println("-------------------------");
        Serial.println("Type 'R' to Reset for a new test, or 'E' to Exit.");
        currentState = CHOICE;
      }
      break;

    case CHOICE:
      if (Serial.available() > 0) {
        char choice = Serial.read();
        clearSerialBuffer();
        
        if (choice == 'R' || choice == 'r') {
          Serial.println("\n--- RESETTING SYSTEM ---");
          Serial.println("Enter exam duration in minutes:");
          currentState = SETUP_TIME;
        } 
        else if (choice == 'E' || choice == 'e') {
          Serial.println("Exiting. Good luck with your psychometry prep!");
          while (1); 
        }
      }
      break;
  }
}
