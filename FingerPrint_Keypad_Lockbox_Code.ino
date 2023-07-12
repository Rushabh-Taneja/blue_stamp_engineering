#include <Keypad.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
#include <FastLED.h>

#define LED_PIN 26
#define NUM_LEDS 60
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


#define UPDATES_PER_SECOND 100


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)

// For UNO and others without hardware serial, we must use software serial...
// pin #12 is IN from sensor (GREEN wire)
// pin #13 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(18, 19);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1


#endif

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
String password = "1234";
String code = "";
String input = "";



Servo myservo;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

int pos = 0;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 6, 7, 8, 9 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 3, 4, 5 };     //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
bool i = true;


void setup() {

  Serial.begin(9600);
  myservo.attach(11);  // attaches the servo on pin 11 to the servo object
  myservo.write(pos);
  //while (!Serial)
  ;  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  lcd.begin(16, 2);
  // set up the LCD's number of columns and rows:

  Hello();
}
//loop


void loop() {

  char key = keypad.getKey();  // Read the key


  if (key == '*') {
    input = "";
    code = "";
    lcd.clear();
    lcd.print("cleared");
    Serial.println("cleared");
  }

  else if (key == '#') {
    if (password == input) {
      input = "";
      code = "";
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 0, FINGERPRINT_LED_PURPLE);

      lcd.clear();
      lcd.print("put finger on");
      lcd.setCursor(0, 1);
      lcd.print("sensor ");
      // lcd.setCursor(0, 1);



      Serial.println("put finger on sensor");
      delay(2000);

      if (getFingerprintID() == 20) {
        lcd.clear();
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_PURPLE);

        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 0, FINGERPRINT_LED_BLUE, 200);

        Serial.println("# to set new code");
        Serial.println("* to unlock or lock box");
        Serial.println("0 to add fingerprint");

        key = '\0';
        long startMillis = millis();

        int l = 1;
        while (true) {
          key = keypad.getKey();

          if (key == '0' || key == '#' || key == '*') {
            break;
          }
          long currentMillis = millis();
          if (((currentMillis - startMillis) % 9000) < 3000) {
            if (l == 1) {
              lcd.clear();
              lcd.println("Press # to set            ");
              lcd.setCursor(0, 1);
              lcd.println("new Code              ");
              // while(((currentMillis - startMillis) % 9000) < 3000){

              // }
              l = 2;
            }
          } else if (((currentMillis - startMillis) % 9000) < 6000) {
            if (l == 2) {
              lcd.clear();
              lcd.println("Press * to lock           ");
              lcd.setCursor(0, 1);
              lcd.println("or unlock box            ");
              //
              l = 3;
            }
            // }
          } else if (((currentMillis - startMillis) % 9000) < 9000) {
            if (l == 3) {

              lcd.clear();
              lcd.println("Press 0 to add        ");
              lcd.setCursor(0, 1);
              lcd.println("fingerprint             ");
              l = 1;
            }
          }
        }


        if (key == '#') {
          lcd.clear();
          lcd.println("Enter Code          ");
          lcd.setCursor(0, 1);
          lcd.println("#-confirm*-clear");

          Serial.println("Press # is to confirm");
          Serial.println("Press * is to clear");

          input = "";
          key = '\0';
          while (true) {
            key = keypad.getKey();
            if (key != '\0') {
              input += key;
              lcd.clear();
              lcd.print(input);
              break;
            }
          }
          while (true) {
            key = keypad.getKey();

            if (key != '\0') {
              if (key == '#') {
                password = input;
                Serial.println("New Code is: " + password);
                lcd.clear();
                lcd.println("New Code is:       ");
                lcd.setCursor(0, 1);
                lcd.println(password);
                Hello();

                input = "";
                break;
              }
              input += key;
              Serial.println(input);
              lcd.clear();
              lcd.print(input);


              if (key == '*') {
                input = "";
                Serial.println("cleared");
                lcd.clear();
                lcd.println("cleared");
                lcd.setCursor(0, 1);
                lcd.println("Enter new code");
                input="";
              }
            }
          }
          Serial.println("New code is: " + password);
          delay(200);
          finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_BLUE);
        }

        else if (key == '*') {
          if (i) {
            Serial.println("Unlocked");
            lcd.clear();
            lcd.print("Unlocked");
            i = false;

          } else {
            Serial.println("Locked");
            lcd.clear();
            lcd.print("Locked");
            i = true;
          }

          lock();
          delay(2000);
          finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_BLUE);
          Hello();
        }

        else if (key == '0') {

          code = "";

          key = '\0';
          Serial.println("put in a number between 1 and 127 for the fingerprint to be saved as");
          lcd.clear();
          //   lcd.print("put number 1-127 to save finger as");
          lcd.print("save finger as #");
          lcd.setCursor(0, 1);
          lcd.print("from 1-127");

          Serial.println("press # to save");

          while (true) {
            key = keypad.getKey();

            Serial.println(code);

            if (key == '#') {
              if (code.toInt() <= 127 && code.toInt() > 0) {
                id = code.toInt();
                lcd.clear();
                lcd.print("Put Finger on");
                lcd.setCursor(0, 1);
                lcd.print("Sensor");
                while (!getFingerprintEnroll())
                  ;
                lcd.clear();
                lcd.print("Finger");
                lcd.setCursor(0, 1);
                lcd.print("Registered");
                code = "";
                Hello();
                break;
              } else if (code.toInt() == 0) {
                break;
              } else {
                code = "";
                Serial.println("Not a valid input");
                lcd.print("Invalid Input");
                lcd.setCursor(0, 1);
                lcd.print("Reneter Code");
              }
            } else if (key == '*') {
              code = "";
            } else if (key) {
              code += key;
              lcd.clear();
              lcd.print(code);
              lcd.setCursor(0, 1);
              lcd.print("Press # to save");
            }
          }
        }
      }

      else {
        Serial.println("Don't recognize finerprint");
        lcd.clear();
        lcd.print("Don't recognize");
        lcd.setCursor(0, 1);
        lcd.print("fingerprint");

        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 1, FINGERPRINT_LED_RED, 50);
        Hello();
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_RED);

        input = "";
        code = "";
      }

    }

    else {
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 1, FINGERPRINT_LED_RED, 50);
      Serial.println("Wrong Code");
      lcd.clear();
      lcd.print("Wrong Code");
      Hello();

      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_RED);
      input = "";
      code = "";
    }
  }

  // Print if key pressed
  else if (key) {
    code += "*";
    Serial.println(key);
    Serial.println(code);
    Serial.println(code.length());
    // input += key;
    input = input + key;
    Serial.println(input);
    Serial.println(input.length());
    Serial.println();

    lcd.clear();
    lcd.print(code);
  }
}


uint8_t lock() {
  if (pos == 0) {
    for (pos = 0; pos < 90; pos += 3) {  // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);  // tell servo to go to position in variable 'pos'
      delay(15);           // waits 15ms for the servo to reach the position
    }
  }

  else if (pos == 90) {
    for (pos = 90; pos > 0; pos -= 3) {  // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);  // tell servo to go to position in variable 'pos'
      delay(15);           // waits 15ms for the servo to reach the position
    }
  }
}

uint8_t Hello() {
  delay(2000);
  lcd.clear();
  lcd.println("Hello                      ");
  lcd.setCursor(0, 1);
  lcd.println("Enter Code              ");
  finger.LEDcontrol(FINGERPRINT_LED_OFF);
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  lcd.clear();
  lcd.print("Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  lcd.clear();
  Serial.println("Place same finger again");
  lcd.print("Place Finger ");
  lcd.setCursor(0, 1);
  lcd.print("Again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }


  return true;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    return 20;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}
