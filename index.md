# Keypad and Fingerprint Lock Box
<!--Replace this text with a brief description (2-3 sentences) of your project. This description should draw the reader in and make them interested in what you've built. You can include what the biggest challenges, takeaways, and triumphs from completing the project were. As you complete your portfolio, remember your audience is less familiar than you are with all that your project entails!
-->
For my project, I built the Keypad and Fingerprint Lockbox. The project has a keypad and fingerprint sensor attached to a box, and once the correct code and a valid fingerprint are put in, the lock on the inside of the box turns, allowing the user to open it. Once the user puts in or retrieves whatever they need, they can close the box and put in the correct code and fingerprint once again to turn the lock and lock the box. The user can also add fingerprints or change the password for their own convenience. 

| **Engineer** | **School** | **Area of Interest** | **Grade** |
|:--:|:--:|:--:|:--:|
| Rushabh T | American High School | Electrical Engineering | Incoming Senior

<!---**Replace the BlueStamp logo below with an image of yourself and your completed project. Follow the guide [here](https://tomcam.github.io/least-github-pages/adding-images-github-p ages-site.html) if you need help.**
-->
![Headstone Image](IMG-1991.jpg)

# Final Milestone

    
          
            
    
<!--
          
          Expand Down
    
    
  
For your final milestone, explain the outcome of your project. Key details to include are:
- What you've accomplished since your previous milestone
- What your biggest challenges and triumphs were at BSE
- A summary of key topics you learned about
- What you hope to learn in the future after everything you've learned at BSE
**Don't forget to replace the text below with the embedding for your milestone video. Go to Youtube, click Share -> Embed, and copy and paste the code to replace what's below.**
-->

<iframe width="560" height="315" src="https://www.youtube.com/embed/JI12loOyLx8" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>


For my third and final milestone, I attached an LCD to my project and attached all of my components to the box. The LCD display allows users to see the options they have available and whether or not everything they have entered everything correctly, working in tandem with the LED on the fingerprint sensor. Once the user turns on the box, the LED prompts the user to put in the code. Once the correct code is put in, the user has the option to either unlock/lock the box, register a new fingerprint in the sensor, or change to code. All of these are displayed on the LCD, allowing the user to see the steps they must take in order to do this. After I got the LCD to work, I moved all the components that were attached to the breadboard and moved them onto a perf board as the components would be soldered on and much harder. Over the course of the milestone, I learned how to properly use an LCD display, how to use a drill, how to use a saw, and the working of a perf board. I also went more in-depth with loops and classes in order to successfully add the features that I desired to add.

A challenge I faced over the course of this milestone was getting the LCD attached to the breadboard. There were many input pins and I kept messing up the wiring, so the LCD stopped working multiple times. This combined with the fact that one of the batteries in my battery pack stopped working so the LCD was not displaying properly. This was an issue as I could not figure out the problem, and kept going through my code and wiring to see if any of those were the issue as I had messed them up multiple times. Eventually, I checked the voltage from the battery pack, and upon realizing it was lower than it should have been, found the issue with the battery. Another issue I faced was the limited space on the LCD screen, leaving me only 16 characters on each line for me to display a message to the user. I was forced to shorten and abbreviate many messages in order to successfully communicate. The process of making the holes in the box for the components was fairly straightforward and was more tedious than challenging. The main issue came when attaching the components to the box as the cables took up much more space than anticipated and I was forced to replace them with shorter cables in order for a user to actually fit stuff in the box. The servo had mounts at the bottom of the servo and I was forced to file them off in order to stick the servo to the side of the box.  The biggest issue I faced was an issue in my code where when setting a new password, the user could add the star symbol to the password. This was a huge issue as the star key was the clear key, so if it was in the password, the user would never be able to put in the password. I realized this bug when trying to put in the new key while the box was locked, leaving me no option but to reach into the box and forcibly turn the servo myself in order to unlock the box. 


While not shown in the milestone, I also went back and added a button that could turn the circuit on and off in order to help save battery power. 



# Code

<details markdown="1"> 

```

c++
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

```

</details>

# Second Milestone
<!--
For your second milestone, explain what you've worked on since your previous milestone. You can highlight:
- Technical details of what you've accomplished and how they contribute to the final goal
- What has been surprising about the project so far
- Previous challenges you fac ed that you overcame
- What needs to be completed before your final milestone 
-->
<iframe width="560" height="315" src="https://www.youtube.com/embed/ojStgfp3zzo" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>
For my second milestone, I have added a fingerprint sensor and battery to my project. This allows the setup to be no longer reliant on my computer as a power source meaning it can be connected to the box. The fingerprint sensor allows the user to scan their fingerprint as another form of verification as well as a way for the user to see if their inputs are right or wrong. The led on the sensor turns red if either the fingerprint or code is incorrect while turning blue if both are correct. The servo works with both, turning if both inputs are correct. A challenge I faced during this milestone was attaching the fingerprint sensor and getting it to work. There were 6 wires attached to the sensor but only 4 were being used, leaving me confused about the purpose of the other 2 until I realized they were useless. Coding it to work was also a struggle as I was not sure what function I was supposed to be used to compare the users fingerprint to the fingerprints of the library until I realized I had to call the entire method and not specific functions. Another issue I faced was running out of space on the Arduino board to plug in the components, but that was easily solved with a breadboard. The next steps will be to add a way to change the passcode, a way to add and remove fingerprints from the library and attach the whole thing to the box. 
 
# Code

<details markdown="1"> 
    

```    
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
  // Serial.println("Ready to enroll a fingerprint!");
  // Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  //  id = int(keypad.getKey());
  // id = readnumber();
  // Serial.println(id);
  // if (id == 0) {  // ID #0 not allowed, try again!
  //   return;
  // }
  // Serial.print("Enrolling ID #");
  // Serial.println(id);
  // while (!getFingerprintEnroll());
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
}
uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
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
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 0, FINGERPRINT_LED_PURPLE);
      Serial.println("put finger on sensor");
      delay(2000);
      finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_PURPLE);
      if (getFingerprintID() == 1) {
        Serial.println("Unlocked");
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 0, FINGERPRINT_LED_BLUE);
        input = "";
        code = "";
        if (pos == 0) {
          for (pos = 0; pos < 90; pos += 3) {  // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            myservo.write(pos);  // tell servo to go to position in variable 'pos'
            delay(15);           // waits 15ms for the servo to reach the position
          }
        } else if (pos == 90) {
          for (pos = 90; pos > 0; pos -= 3) {  // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            myservo.write(pos);  // tell servo to go to position in variable 'pos'
            delay(15);           // waits 15ms for the servo to reach the position
          }
        }
        delay(2500);
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_BLUE);
      } else {
        Serial.println("Don't recognize finerprint");
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 1, FINGERPRINT_LED_RED, 50);
        delay(2500);
        finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 0, FINGERPRINT_LED_RED);
        input = "";
        code = "";
      }
    } else {
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

```

</details>


# First Milestone
<!--For your first milestone, describe what your project is and how you plan to build it. You can include:
- An explanation about the different components of your project and how they will all integrate together
- Technical progress you've made so far
- Challenges you're facing and solving in your future milestones
- What your plan is to complete your project-->
<iframe width="560" height="315" src="https://www.youtube.com/embed/e8Pgml_ho5w" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>
For my first milestone, I attached a functioning keypad and a servo and set a passcode to activate them. This is the first security system and the locking system that I plan to use, leaving only the fingerprint sensor and the installation in the box. I used jumper cables to connect the servo and number pad to the Arduino board. I used C++ to create a matrix that matched every key on the keyboard to provide an output representing the same number when the respective key was pressed. This allowed for a user to input values which were later used to make a code to compare with the password. The only exceptions to this were "*" and "#" which cleared the code and compared it with the password respectively. I then added the servo to function as part of a locking system. When the proper code was put in, the servo would turn 90° which would be used to either lock or unlock the box.
# Code


<details markdown="1"> 
    
```
c++
#include <Keypad.h>
#include <Servo.h>
const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
String password = "1234";
String code = "";
String input = "";
Servo myservo;
int pos = 90;
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
}
void loop() {
  char key = keypad.getKey();  // Read the key
  if (key == '*') {
    input = "";
    code = "";
    Serial.println("cleared");
  }
  else if (key == '#') {
    if (password == input) {
      Serial.println("Unlocked");
      input = "";
      code = "";
      if(pos==0){
        for (pos = 0; pos < 90; pos += 3) {  // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);  // tell servo to go to position in variable 'pos'
          delay(15);           // waits 15ms for the servo to reach the position
        }
      }
      else if (pos==90){
        for (pos = 90; pos > 0; pos -= 3) {  // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);  // tell servo to go to position in variable 'pos'
          delay(15);           // waits 15ms for the servo to reach the position
        }
      }
      }
     else {
      Serial.println("Wrong password");
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
```
</details>

# Starter Project 
<!--For your first milestone, describe what your project is and how you plan to build it. You can include:
- An explanation about the different components of your project and how they will all integrate together
- Technical progress you've made so far
- Challenges you're facing and solving in your future milestones
- What your plan is to complete your project-->
<iframe width="560" height="315" src="https://www.youtube.com/embed/jMDJ2qqvpEo" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>
For my starter, I made the Binary Blaster. The binary blaster displays numbers from 1 to 15 in base 10 in a random , and the user needs to put in the corresponding values in binary. The 4 buttons on the binary blaster represent the first, second, third, and fourth bit in binary. A pressed button represents the bit being 1 and non pressed represents the bit being 0. If the user does not get the correct input within 6 seconds, the game ends and the user can restart. After 15 correct inputs, then the user gets the time they took to solve it and can restart. While waiting for the user to start, the game cycles through the numbers and shows the corresponding binary input by lighting up the buttons on the bottom.
<!--# Schematics 
Here's where you'll put images of your schematics. [Tinkercad](https://www.tinkercad.com/blog/official-guide-to-tinkercad-circuits) and [Fritzing](https://fritzing.org/learning/) are both great resources to create professional schematic diagrams, though BSE recommends Tinkercad because it can be done easily and for free in the browser. 
-->
# Bill of Materials
<!--Here's where you'll list the parts of your project. To add more rows, just copy and paste the example rows below.
Don't forget to place the link of where to buy each component inside the quotation marks in the corresponding row after href =. Follow the guide [here]([url](https://www.markdownguide.org/extended-syntax/)) to learn how to customize this to your project needs. -->

| **Part** | **Note** | **Price** | **Link** |
|:--:|:--:|:--:|:--:|
| Arduino Board | Used to store code as well as input and output pins to send and receive signals. | $29.99 | <a href="https://www.amazon.com/ELEGOO-ATmega2560-ATMEGA16U2-Projects-Compliant/dp/B01H4ZLZLQ/ref=asc_df_B01H4ZLZLQ/?tag=hyprod-20&linkCode=df0&hvadid=309743296044&hvpos=&hvnetw=g&hvrand=13950587571824107519&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-490931309987&psc=1"> Link </a> |
| Keypad | Used to input a code| $11.95 | <a href="https://www.amazon.com/Adafruit-3x4-Phone-style-Matrix-Keypad/dp/B00QSHPCO8/ref=asc_df_B00QSHPCO8/?tag=hyprod-20&linkCode=df0&hvadid=642063549738&hvpos=&hvnetw=g&hvrand=6586475367768961937&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032183&hvtargid=pla-1932365220176&psc=1&gclid=Cj0KCQjwtO-kBhDIARIsAL6LoreDws-fQrbUv8j4FzonZWjF67U0_QuT88VK3-7rFQHX-f-Ga8-MS74aAjhkEALw_wcB"> Link </a> |
| Servo | Turns to act like a lock | $10.00 | <a href="https://www.amazon.com/MMOBIEL-Airplane-Helicopter-Compatible-Raspberry/dp/B09BZTRSDP/ref=asc_df_B09BZTRSDP/?tag=hyprod-20&linkCode=df0&hvadid=632109977210&hvpos=&hvnetw=g&hvrand=17319493360633244579&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032183&hvtargid=pla-1510127627453&psc=1"> Link </a> |
| Jumper Wires | Connects components to Arduino board | $4.61 | <a href="https://www.amazon.com/120pcs-Dupont-Ribbon-Breadboard-arduino/dp/B07KLM9KR1/ref=asc_df_B07KLM9KR1/?tag=hyprod-20&linkCode=df0&hvadid=642170509892&hvpos=&hvnetw=g&hvrand=11961711466041498963&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032183&hvtargid=pla-1954692578081&psc=1&gclid=Cj0KCQjwtO-kBhDIARIsAL6Lorc8F8WBt8xSeZC6phf1oQMX6W9EtIKvnW-9HtXrfy9yawGwdlva4nEaAp7cEALw_wcB"> Link </a> |
| Fingerprint Sensor | Scans users fingerprints | $25.47 | <a href="https://www.amazon.com/Fingerprint-Identification-Capacitive-Recognition-Assistance/dp/B08GKY4RK1/ref=asc_df_B08GKY4RK1/?tag=hyprod-20&linkCode=df0&hvadid=658824471335&hvpos=&hvnetw=g&hvrand=5390830515971170324&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032183&hvtargid=pla-2071100511245&psc=1&gclid=Cj0KCQjwtO-kBhDIARIsAL6Lorc17L5Nd2v9Ny0N6K_8U5f5FNkOVYYsj_PL7KQY3tWn0g1qUpU-6a0aArFrEALw_wcB"> Link </a> |
| Breadboard| Used to hold cables connecting components| $8.75 | <a href="https://www.amazon.com/BB830-Solderless-Plug-BreadBoard-tie-Points/dp/B0040Z4QN8/ref=asc_df_B0040Z4QN8/?tag=hyprod-20&linkCode=df0&hvadid=312400581241&hvpos=&hvnetw=g&hvrand=13618660822161177333&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032183&hvtargid=pla-420088669616&psc=1"> Link </a> |
| LCD display| Shows messages to users | $12.89 | <a href="https://www.amazon.com/SunFounder-Serial-Module-Arduino-Mega2560/dp/B01GPUMP9C/ref=asc_df_B01GPUMP9C/?tag=&linkCode=df0&hvadid=309777534894&hvpos=&hvnetw=g&hvrand=3916194996455184178&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032183&hvtargid=pla-572925702212&ref=&adgrpid=58425267301&th=1"> Link </a> |
| 5 Battery Pack | Holds batteries| $3.24 | <a href="https://www.amazon.com/LAMPVPATH-Battery-Holder-Leads-Wires/dp/B07T7MTRZX/ref=asc_df_B07T7MTRZX/?tag=hyprod-20&linkCode=df0&hvadid=385184478202&hvpos=&hvnetw=g&hvrand=11864053990162520089&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-833432280659&psc=1&tag=&ref=&adgrpid=77282054623&hvpone=&hvptwo=&hvadid=385184478202&hvpos=&hvnetw=g&hvrand=11864053990162520089&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-833432280659"> Link </a> |
| Batteries | Provide an energy source. to the Arduino board | $6.23 | <a href="https://www.amazon.com/AmazonBasics-Performance-Alkaline-Batteries-8-Pack/dp/B00O869QUC/ref=asc_df_B00O869QUC/?tag=hyprod-20&linkCode=df0&hvadid=563645139568&hvpos=&hvnetw=g&hvrand=5406759330328739186&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032183&hvtargid=pla-332122495633&psc=1"> Link </a> |
| Box | Where to attach all the components to as well as where to store objects | $18.99 | <a href="https://www.amazon.com/Unfinished-Hinged-Crafts-Hobbies-Storage/dp/B07QY5M179/ref=asc_df_B07QY5M179/?tag=hyprod-20&linkCode=df0&hvadid=343161308379&hvpos=&hvnetw=g&hvrand=5986731003153187548&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-731109277638&psc=1&tag=&ref=&adgrpid=69167794775&hvpone=&hvptwo=&hvadid=343161308379&hvpos=&hvnetw=g&hvrand=5986731003153187548&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-731109277638"> Link </a> |
| Button | Lets user turn Arduino board on or off in order to conserve battery | $0.80 | <a href="https://www.amazon.com/Twidec-AC250V-AC125V-Momentary-R13-507-5C/dp/B07SVTQ7B9/ref=asc_df_B07SVTQ7B9/?tag=hyprod-20&linkCode=df0&hvadid=385419804644&hvpos=&hvnetw=g&hvrand=6488663868106609588&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-824434418800&psc=1&tag=&ref=&adgrpid=78312795083&hvpone=&hvptwo=&hvadid=385419804644&hvpos=&hvnetw=g&hvrand=6488663868106609588&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-824434418800"> Link </a> |
| Perf Board | Place to hold components so they won't fall out as components move | $0.28 | <a href="https://www.amazon.com/Prototype-Soldering-Universal-Printed-Electronic/dp/B079DN31SW/ref=asc_df_B079DN31SW/?tag=hyprod-20&linkCode=df0&hvadid=242022160290&hvpos=&hvnetw=g&hvrand=11672547180545561597&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9032047&hvtargid=pla-574241827852&psc=1"> Link </a> |



<!--
# Other Resources/Examples
One of the best parts about Github is that you can view how other people set up their own work. Here are some past BSE portfolios that are awesome examples. You can view how they set up their portfolio, and you can view their index.md files to understand how they implemented different portfolio components.
- [Example 1](https://trashytuber.github.io/YimingJiaBlueStamp/)
- [Example 2](https://sviatil0.github.io/Sviatoslav_BSE/)
- [Example 3](https://arneshkumar.github.io/arneshbluestamp/)
To watch the BSE tutorial on how to create a portfolio, click here.
-->
