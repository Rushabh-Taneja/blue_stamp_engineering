#include <Keypad.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)

// For UNO and others without hardware serial, we must use software serial...
// pin #12 is IN from sensor (GREEN wire)
// pin #13 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(12, 13);

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

}


//loop


void loop() {

  char key = keypad.getKey();  // Read the key
  if (key == '*') {
    input = "";
    code = "";
    Serial.println("cleared");
  }

  else if (key == '#') {
    if (password == input) {
        input = "";
        code = "";
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 0, FINGERPRINT_LED_PURPLE);

      Serial.println("put finger on sensor");
      delay(2000);
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_PURPLE);

      if (getFingerprintID() == 1) {
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 0, FINGERPRINT_LED_BLUE);
        Serial.println("# to set new passcode");
        Serial.println("* to unlock or lock box");
        Serial.println("0 to add fingerprint");

        char key = '\0';
        while (1 == 1) {
          key = keypad.getKey();

          if (key != '\0' && key != '1' && key != '2' && key != '3' && key != '4' && key != '5' && key != '6' && key != '7' && key != '8' && key != '9') {
            break;
          }
        }

        if (key == '#') {
          Serial.println("Enter what you want the password to be");
          Serial.println("Press # is to confirm");
          Serial.println("Press * is to clear");

          input = "";
          code = "";
          key = '\0';
          while (1 == 1) {
            key = keypad.getKey();

            if (key != '\0') {
              if (key == '#') {
                password = input;
                Serial.println("New password is: " + password);
                input = "";
                break;
              }
              input += key;
              Serial.println(input);

              if (key == '*') {
                input = "";
                code = "";
                Serial.println("cleared");
              }
            }
          }
          Serial.println("New password is: " + password);
          delay(200);
          finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_BLUE);
        }

        else if (key == '*') {

          Serial.println("Unlocked");

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

          delay(2000);
          finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_BLUE);
        }

        else if (key == '0') {

          String placeholder = "";
          int i=0;
          key = '\0';
          Serial.println("put in a number between 1 and 127 for the fingerprint to be saved as");
          Serial.println("press # to save the number");

          while (true) {
            key = keypad.getKey();
            int i = placeholder.toInt();
            
            Serial.println(i);
            Serial.println(placeholder);

            if (key == '#') {
              if (placeholder.toInt() <= 127 && placeholder.toInt() > 0) {
                id = placeholder.toInt();
                while (!getFingerprintEnroll())
                  ;
                break;
              }
              else if(placeholder.toInt() == 0) {
                break;
              }e
              else {
                placeholder = "";
                Serial.println("Not a valid input");
              }
            }
            else if (key == '*') {
              placeholder = "";
            }
            else if(key){
                placeholder += key;
            }
          }
        }
      }

      else {
        Serial.println("Don't recognize finerprint");
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 1, FINGERPRINT_LED_RED, 50);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_RED);

        input = "";
        code = "";
      }

    }

    else {
      Serial.println("Wrong password");
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 1, FINGERPRINT_LED_RED, 50);
      delay(2500);
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_RED);
      input = "";
      code = "";
    }
  }

  // Print if key pressed
  else if (key) {
    code = code + "*";
    input += key;
    Serial.println(code);
  }
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
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
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
    return 1;
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
