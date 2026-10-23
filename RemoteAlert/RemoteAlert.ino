#define RECEIVER_PIN D1
#define BUZZER_PIN   D2
#define LED_PIN      D5
#define RESET_BUTTON D6

// =====================================================
// DEBUG MODE
// =====================================================

// true  = buzzer disabled
// false = buzzer enabled
#define DEBUG_MODE false


// =====================================================
// SETTINGS
// =====================================================

const unsigned long IDLE_LED_INTERVAL  = 1000;
const unsigned long ALARM_LED_INTERVAL = 150;
const unsigned long BUZZER_INTERVAL    = 80;


// =====================================================
// VARIABLES
// =====================================================

bool alarmActive = false;

bool ledState = false;
bool buzzerState = false;

unsigned long lastLedTime = 0;
unsigned long lastBuzzerTime = 0;


// =====================================================
// SETUP
// =====================================================

void setup() {

  // RX26A is active LOW
  pinMode(RECEIVER_PIN, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Button connected between D6 and GND
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  // Everything starts OFF
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  Serial.println();
  Serial.println("RX26A LATCHED ALARM");

  if (DEBUG_MODE) {
    Serial.println("DEBUG MODE: BUZZER DISABLED");
  } else {
    Serial.println("NORMAL MODE: BUZZER ENABLED");
  }
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  unsigned long now = millis();


  // =====================================================
  // RESET BUTTON
  // =====================================================

  if (digitalRead(RESET_BUTTON) == LOW) {

    alarmActive = false;

    ledState = false;
    buzzerState = false;

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("ALARM RESET");

    // Wait for button release
    while (digitalRead(RESET_BUTTON) == LOW) {
      delay(10);
    }

    delay(50);
  }


  // =====================================================
  // REMOTE TRIGGER
  // =====================================================

  // RX26A pulls D1 LOW when triggered
  if (digitalRead(RECEIVER_PIN) == LOW) {

    if (!alarmActive) {

      alarmActive = true;

      Serial.println("REMOTE TRIGGERED!");
      Serial.println("ALARM LATCHED");

      // Start LED immediately
      ledState = true;
      digitalWrite(LED_PIN, HIGH);

      // Start buzzer only if NOT in debug mode
      if (!DEBUG_MODE) {
        buzzerState = true;
        digitalWrite(BUZZER_PIN, HIGH);
      }

      lastLedTime = now;
      lastBuzzerTime = now;
    }
  }


  // =====================================================
  // ALARM MODE
  // =====================================================

  if (alarmActive) {

    // ---------------------------------
    // FAST LED
    // ---------------------------------

    if (now - lastLedTime >= ALARM_LED_INTERVAL) {

      lastLedTime = now;

      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }


    // ---------------------------------
    // BUZZER
    // ---------------------------------

    if (!DEBUG_MODE) {

      if (now - lastBuzzerTime >= BUZZER_INTERVAL) {

        lastBuzzerTime = now;

        buzzerState = !buzzerState;

        digitalWrite(BUZZER_PIN, buzzerState);
      }
    }
    else {

      // Make absolutely sure buzzer stays OFF
      digitalWrite(BUZZER_PIN, LOW);
      buzzerState = false;
    }
  }


  // =====================================================
  // IDLE MODE
  // =====================================================

  else {

    // Slowly flash LED

    if (now - lastLedTime >= IDLE_LED_INTERVAL) {

      lastLedTime = now;

      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);
    }

    // Buzzer always OFF when idle
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
  }
}