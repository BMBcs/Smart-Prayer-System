// for setting timers
#include <millisDelay.h>

// new serial for blutooth
#include <SoftwareSerial.h>
SoftwareSerial BT (10, 11); // BT(rx,tx)  Blutooth :  tx = 10  rx = 11


// a new liquid crystal library to attach the LCD with I2C sheld
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


////// special library for making the Ultrasound sensore more accurate
#include <NewPing.h>


///////// number of FSRs pins - to accept analog data
int s1 = 0;
int s2 = 1;
int s3 = 2;

///////// to read the actual data from the FSRs and the Ultrasound sensor
int fsr1;
int fsr2;
int fsr3;
int distanceSensor;

///////// Setup the LCD with I2C sheild
const int en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2cAddress = 0x27;
LiquidCrystal_I2C lcd (i2cAddress, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);


///////// setup the ultrasonic sensor
#define trigPin  8
#define echoPin  9
#define MAX_DISTANCE  35
NewPing disSensor(trigPin, echoPin, MAX_DISTANCE);


///////////////////////////////////////////////////////////////////////////////////
//keypad

#define key1 33 // increasing button (Yellow)
#define key2 31 // OK button (Green)
#define key3 3  // back Button (Red)
#define key4 2  // offline mode Button (Yellow) 

int key1Reading;
int key2Reading;
//int key3Reading;
//int key4Reading;

////////////////////////// Flags & Counters

bool standing = false;
bool bowing = false;
bool prost1 = false;
bool prost2 = false;
bool sitting = false;

bool firstTashahod = false;
bool finalTashahod = false;

int bigCounter = 0;
int rakahNum = 2;

volatile bool startWork = false;
volatile bool bt_Mode = true;
bool check_online_mode = false; // to return to online mode when press the back button
bool interrupt_check1 = false;
bool interrupt_check2 = false;

bool sahoo = false;
/////////////////////////////


////// to set the timers
millisDelay bowingCounter, sittingCounter;
bool bowingCounter_start = true;
bool sittingCounter_start = true;


////// Blutooth variables
char recieved_Salat_Sympol;


void setup() {

  Serial.begin(9600);
  BT.begin(9600);
  lcd.begin(16, 2);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(key1, INPUT_PULLUP);
  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);
  pinMode(key4, INPUT_PULLUP);

  lcd.setCursor(3, 0);
  lcd.print("Welcome!!");
  delay(3000);

  lcd.clear();
  lcd.print("Pairing        N");
  lcd.setCursor(0, 1);
  lcd.print("OFFline Press'B'");

  attachInterrupt(digitalPinToInterrupt(key3), ISR_backButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(key4), ISR_offlineButton, FALLING);
}

void loop() {

  ////// online Mode
  if (bt_Mode) {

    if (interrupt_check2) {
      check_online_mode = false;
      online_screen_printing();
    }

    if (BT.available()) {
      recieved_Salat_Sympol = BT.read();
      //Serial.println(recieved_Data);

      switch (recieved_Salat_Sympol){

        case 'F': rakahNum = 2;
          bt_Mode = false;
          check_online_mode = true;
          startWork = true;
          salat_Name_Printing("Al-Fajr");

          break;

        case 'D': rakahNum = 4;
          bt_Mode = false;
          check_online_mode = true;
          startWork = true;
          salat_Name_Printing("Al-Dhuhr");

          break;

        case 'A': rakahNum = 4;
          bt_Mode = false;
          check_online_mode = true;
          startWork = true;
          salat_Name_Printing("Al-Asr");

          break;

        case 'M': rakahNum = 3;
          bt_Mode = false;
          check_online_mode = true;
          startWork = true;
          salat_Name_Printing("Al-Maghrib");

          break;

        case 'E': rakahNum = 4;
          bt_Mode = false;
          check_online_mode = true;
          startWork = true;
          salat_Name_Printing("Al-Esha");
          break;

      }

    }
  }

  ////////////////////////////////////// OFFline Mode.
  if (!startWork && (!bt_Mode)) {

    if (interrupt_check1) {
      offline_screen_printing();
    }

    // to read the buttons status
    key1Reading = digitalRead(key1);
    key2Reading = digitalRead(key2);
    //key3Reading = digitalRead(key3);
    //key4Reading = digitalRead(key4);

    delay(200);
    if (!key1Reading) {
      if (rakahNum == 4) {
        rakahNum = 2;
        lcd.setCursor(3, 1);
        lcd.print(rakahNum);
      }
      else {
        rakahNum++;
        lcd.setCursor(3, 1);
        lcd.print(rakahNum);
      }
    }

    if (!key2Reading) {
      startWork = true;
      lcd.clear();
      lcd.print("standing");
      lcd.setCursor(0, 1);
      lcd.print("Done Rakah: 0/");
      lcd.print(rakahNum);


    }
  }
  ////////////////////////////////////////////////////////////////
  // after the user select the number of rakah and press OK

  if (startWork) {

    switch (rakahNum) {

      case 4: fourRakahPrayer();
        break;

      case 3: threeRakahPrayer();
        break;

      case 2: twoRakahPrayer();
        break;
    }
  }

  /////////////////////////////////////////


} // End Loop



//////////////////////////////////////////////////  INTERRUPT SERVICE ROUTINE Functions  ///////////////////////////

void ISR_backButton() {

  if (check_online_mode) {
    startWork = false;
    bt_Mode = true;
    interrupt_check2 = true;
  } else {
    startWork = false;
    interrupt_check1 = true;
  }
}

void ISR_offlineButton() {

  if (bt_Mode) {
    startWork = false;
    bt_Mode = false; // to switch from online to offline
    interrupt_check1 = true; // to activate the printing Function (offline_screen_printing()).
  } else {
    bt_Mode = true;  // to switch from offline to online
    interrupt_check2 = true; // to activate the printing Function (online_screen_printing()).
  }

}

////////////////////////////////////////  Printing Functions ////////////////////////////////////////////////

void online_screen_printing() {

  interrupt_check2 = false;
  lcd.clear();
  //lcd.setCursor(2,0);
  lcd.print("Pairing        N");
  lcd.setCursor(0, 1);
  lcd.print("Offline press'B'");

  // zero all the counters and flags.
  standing = false;
  bowing = false;
  prost1 = false;
  prost2 = false;
  sitting = false;

  firstTashahod = false;
  finalTashahod = false;
  sahoo = false;

  bigCounter = 0;
  bowingCounter_start = true;
  sittingCounter_start = true;
}


void offline_screen_printing() {

  lcd.clear();
  lcd.print("Set Num Rakah  F");
  lcd.setCursor(3, 1);
  lcd.print(rakahNum);
  interrupt_check1 = false;

  // zero all the counters and flags.
  standing = false;
  bowing = false;
  prost1 = false;
  prost2 = false;
  sitting = false;

  firstTashahod = false;
  finalTashahod = false;
  sahoo = false; 

  bigCounter = 0;
  bowingCounter_start = true;
  sittingCounter_start = true;

  // don't forget to zero the counters

}

void salat_Name_Printing(String name) {

  lcd.clear();
  lcd.print("Salat");
  lcd.setCursor(0,1);
  lcd.print(name);
  delay(4000);
  lcd.clear();
  lcd.print("Standing");
  lcd.setCursor(0, 1);
  lcd.print("Done Rakah: 0/");
  lcd.print(rakahNum);

}

void check_sahoo(){

  if(sahoo){ // Initialization
          delay(3000);
          bool prost1 = false;
          bool sitting = false;
          bool prost2 = false;
          
         /// printing
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("Saho");

          // to make the word sahoo blinking in the LCD
          for(int i = 1; i <= 4 ; i++){
            lcd.noDisplay();
            delay(500);
            lcd.display();
            delay(500);
          }

         lcd.setCursor(0,1);
         lcd.print("Prost1    Ps:0/2");
         
         while(sahoo){ // Begin to detect sahoo postures
          fsr3 = analogRead(s3);
          fsr2 = analogRead(s2);
          
          // sahoo prostration 1
          if(fsr3 > 100 && prost1 == false){
            prost1 = true;
          }

          // sahoo sitting
          if(fsr3 < 100 && prost1 == true && sitting == false){
            
            lcd.setCursor(0,1);
            lcd.print("Sitting   Ps:1/2");
            delay(3000); // time for the user to sit
            
            if(fsr2 > 100){
              /// printing
              lcd.setCursor(0,1);
              lcd.print("Prost2  ");
              sitting = true;
            }
          }

          // sahoo prostration 2
          if(sitting && fsr3 > 100 && prost2 == false){
            prost2 = true;
          }

          if(prost1 && prost2 && fsr3 < 100){
            sahoo = false;
            lcd.setCursor(0,1);
            lcd.print("Saho Done Ps:2/2");
          }
        }
      }
}


/*----------------------------------------- Prayers Functions Section -------------------------------*/

void fourRakahPrayer() {

  // read the actual data from the FSRs and the Ultrasound sensor
  fsr1 = analogRead(s1);
  fsr2 = analogRead(s2);
  fsr3 = analogRead(s3);
  distanceSensor = disSensor.ping_cm();

  //standing
                                                                                   // for tashaood
  if (fsr1 > 100 && (standing == false) && (fsr3 < 100) && (distanceSensor == 0) && (fsr2 < 100) ) {

    if (bigCounter == 2 && firstTashahod == false) {         // we add these if statments to prevent printing "Bowing" if the user skip tashhood 1 OR 2
      // to detect sahoo
      sahoo = true;
      Serial.println(" ----------------------------------------------------- T1 Sahooo");
    } else if (bigCounter == 4 && finalTashahod == false) {
      // to detect sahoo
      sahoo = true;
      Serial.println(" ----------------------------------------------------- T2 Sahooo");
    } else {
      delay(4000);
      standing = true;
      lcd.setCursor(0, 0);
      lcd.print("Bowing          ");
      //Serial.println(" ----------------------------------------------------- Standing Done");
    }
  }

  ////////////////////////////////////////////////////

  // bowing detection

if ( standing && (distanceSensor < 100 && distanceSensor != 0) && (bowing == false) ) {

    // start counter to check wether the person bowing or directly prostrating
    if (bowingCounter_start) {
      bowingCounter.start(3000);
      bowingCounter_start = false;
    }

    if (bowingCounter.isFinished()) {
      if (fsr3 < 100 && fsr2 < 100) { // to ensure that the user is not prostrating.
           lcd.setCursor(0, 0);
           lcd.print("Prost     Ps:0/2");
           bowing = true;
           //Serial.println(" ----------------------------------------------------- Bowing Done");
      }else{
        sahoo = true;
        bowingCounter_start = true;
        Serial.println(" ----------------------------------------------------- bow Sahooo");
      }
    }
  }


  //////////////////////////////////////////////////////

  // prostration 1 detection

  if (bowing && fsr3 > 100 && (prost1 == false)) {
    prost1 = true;
    //Serial.println(" ----------------------------------------------------- Prost 1 Done");
  }

  ///////////////////////////////////////////////////////

  // sitting detection

  if (prost1 && fsr3 < 100 && (sitting == false) ) {

    if (sittingCounter_start) {
      sittingCounter.start(4000);
      sittingCounter_start = false;
      lcd.setCursor(0, 0);
      lcd.print("sitting   Ps:1/2");
    }

    if (sittingCounter.isFinished()) {
      if (fsr2 > 100) {
        lcd.setCursor(0, 0);
        lcd.print("prost     Ps:1/2");
        sitting = true;
        //Serial.println(" ----------------------------------------------------- Sitting Done");
      } else {
        sittingCounter_start = true;
        sahoo = true;
        Serial.println(" -----------------------------------------------------Sit Sahooo");
      }
    }
  }

  //////////////////////////////////////////////////////

  // prostration 2 detection

  if ( prost1 && sitting && fsr3 > 100 && (prost2 == false) ) {
    prost2 = true;
    //Serial.println(" ----------------------------------------------------- Prost 2 Done");
  }

  if (prost1 && sitting && fsr3 < 100 && (prost2 == true) ) {

    delay(2000);
    if (bigCounter == 1) {
      lcd.setCursor(0,0);
      lcd.print("Tashahod1 Ps:2/2");
      delay(2000);
    }
    else if (bigCounter == 3) {
      lcd.setCursor(0, 0);
      lcd.print("Tashahod2 Ps:2/2");
      delay(2000);
    }
    else {
      // Serial.println("11111111111111");
      lcd.setCursor(0,0);
      lcd.print("Standing        ");
      delay(3000);
    }
  }

  //////////////////////////////////////////////////////new

  // Frist Tashahod

  if ( bigCounter == 2  && firstTashahod == false && fsr3 < 100) {

    if (fsr2 > 100) {
      firstTashahod = true;
      //Serial.println("2222222222222222");
      lcd.setCursor(0, 0);
      lcd.print("standing        ");
      delay(3000);
    }
  }

  ///////////////////////////////////////////////////////

  // Final Tashahod

  if ( bigCounter == 4  && finalTashahod == false && fsr3 < 100) {
    delay(4000);

    if (fsr2 > 100) {

      finalTashahod = true;
      check_sahoo(); // check weather the user should perform sahoo OR not
      
      delay(5000);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Completed");
      lcd.setCursor(3, 1);
      lcd.print("Prayer");
      delay(5000);

      ISR_backButton(); // we call this function since it does the same purpose that we need here.
      //        startWork = false;
      //
      //        lcd.clear();
      //        lcd.print("Choose Num Rakah");
      //        lcd.setCursor(3,1);
      //        lcd.print(rakahNum);
      //        bigCounter = 0;
      //        firstTashahod == false;

      if (check_online_mode) {
        if (recieved_Salat_Sympol == 'a') {
          BT.write("3");
        } else if (recieved_Salat_Sympol == 'z') { ////// to send the current salat sympol to the mobile app
          BT.write("2");
        } else if (recieved_Salat_Sympol == 'e') {
          BT.write("5");
        }
      }
    }
  }

  ///////////////////////////////////////////////////////

  if ( standing && bowing && prost1 && sitting && prost2 && fsr3 < 100) {

    bigCounter++;
    standing = false;
    bowing = false;
    prost1 = false;
    prost2 = false;
    sitting = false;

    bowingCounter_start = true;
    sittingCounter_start = true;

    //lcd.setCursor(0,0);
    //lcd.print("Complete Rakah  ");
    lcd.setCursor(12, 1);
    lcd.print(bigCounter);

  }
}


/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


void threeRakahPrayer() {

  // read the actual data from the FSRs and the Ultrasound sensor
  fsr1 = analogRead(s1);
  fsr2 = analogRead(s2);
  fsr3 = analogRead(s3);
  distanceSensor = disSensor.ping_cm();
  //Serial.println(distanceSensor);

  //standing
                                                                                   // for tashaood
  if (fsr1 > 100 && (standing == false) && (fsr3 < 100) && (distanceSensor == 0) && (fsr2 < 100) ) {

    if (bigCounter == 2 && firstTashahod == false) {         // we add these if statments to prevent printing "Bowing" if the user skip
      // do nothing
      sahoo = true;
    } else if (bigCounter == 3 && finalTashahod == false) {
      // do nothing
      sahoo = true;
    } else {
      delay(4000); // to delay printing "Bowing" after standing from the tashahood
      standing = true;
      lcd.setCursor(0, 0);
      lcd.print("Bowing          ");
      //Serial.println(" ----------------------------------------------------- Standing Done");
    }
  }




  ////////////////////////////////////////////////////

  // bowing detection
  if ( standing && (distanceSensor < 100 && distanceSensor != 0) && (bowing == false) ) {

    // start counter to check wether the person bowing or directly prostrating
    if (bowingCounter_start) {
      bowingCounter.start(3000);
      bowingCounter_start = false;
    }

    if (bowingCounter.isFinished()) {
      if (fsr3 < 100 && fsr2 < 100) { // to ensure that the user is not prostrating.
           lcd.setCursor(0, 0);
           lcd.print("Prost     Ps:0/2");
           bowing = true;
           //Serial.println(" ----------------------------------------------------- Bowing Done");
      }else{
        sahoo = true;
        bowingCounter_start = true;
        Serial.println(" ----------------------------------------------------- bow Sahooo");
      }
    }
  }

  //////////////////////////////////////////////////////

  // prostration 1 detection

  if (bowing && fsr3 > 100 && (prost1 == false)) {
    prost1 = true;

    //Serial.println(" ----------------------------------------------------- Prost 1 Done");
  }

  ///////////////////////////////////////////////////////

  // sitting detection

  if (prost1 && fsr3 < 100 && (sitting == false) ) {

    if (sittingCounter_start) {
      sittingCounter.start(4000);
      sittingCounter_start = false;
      lcd.setCursor(0, 0);
      lcd.print("sitting   Ps:1/2");
    }

    if (sittingCounter.isFinished()) {
      if (fsr2 > 100) {
        lcd.setCursor(0, 0);
        lcd.print("Prost2    Ps:1/2");
        sitting = true;
        //Serial.println(" ----------------------------------------------------- Sitting Done");
      } else {
        sittingCounter_start = true;
        sahoo= true;
      }
    }
  }

  //////////////////////////////////////////////////////

  // prostration 2 detection

  if ( prost1 && sitting && fsr3 > 100 && (prost2 == false) ) {
    prost2 = true;

    //Serial.println(" ----------------------------------------------------- Prost 2 Done");
  }

  if ( prost1 && sitting && fsr3 < 100 && (prost2 == true)) {
    delay(2000);
    if (bigCounter == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Tashahod1 Ps:2/2"); 
      delay(2000);
      
    } else if (bigCounter == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Tashahod2 Ps:2/2");
      delay(2000);
      
    } else {
      lcd.setCursor(0, 0);
      lcd.print("standing        ");
      delay(3000);
    }
  }

  //////////////////////////////////////////////////////new

  // Frist Tashahd

  if ( bigCounter == 2 && firstTashahod == false && fsr3 < 100) {
    delay(3000);

    if (fsr2 > 100) {
      firstTashahod = true;
      lcd.setCursor(0, 0);
      lcd.print("standing        ");
      delay(3000);
    }
  }

  ///////////////////////////////////////////////////////

  // Final Tashahod

  if ( bigCounter == 3  && finalTashahod == false && fsr3 < 100) {
    delay(4000);

    if (fsr2 > 100) {
      finalTashahod = true;

      check_sahoo(); // check weather the user should perform sahoo OR not 
      delay(5000);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Completed");
      lcd.setCursor(3, 1);
      lcd.print("Prayer");
      delay(5000);
      ISR_backButton();  // we call this function since it does the same purpose that we need here.
      
      if (check_online_mode) {
        BT.write("4");
      }
    }
  }

  ///////////////////////////////////////////////////////

  if ( standing && bowing && prost1 && sitting && prost2 && fsr3 < 100) {

    bigCounter++;
    standing = false;
    bowing = false;
    prost1 = false;
    prost2 = false;
    sitting = false;

    bowingCounter_start = true;
    sittingCounter_start = true;

    lcd.setCursor(12, 1);
    lcd.print(bigCounter);

  }

}


/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void twoRakahPrayer() {

  // read the actual data from the FSRs and the Ultrasound sensor
  fsr1 = analogRead(s1);
  fsr2 = analogRead(s2);
  fsr3 = analogRead(s3);
  distanceSensor = disSensor.ping_cm();
  
  //standing                                                                        // for tashood
  if (fsr1 > 100 && (standing == false) && (fsr3 < 100) && (distanceSensor == 0) && (fsr2 < 100) ) {

    if (bigCounter == 2  && finalTashahod == false) {
      //do nothing
      Serial.println(" -----------------------------------------------------Tashahood sahoo");
      sahoo = true;
    } else {
      standing = true;
      lcd.setCursor(0, 0);
      delay(4000);
      lcd.print("Bowing          ");
      //Serial.println(" ----------------------------------------------------- Standing Done");
    }
  }

  ////////////////////////////////////////////////////

  // bowing detection

  if ( standing && (distanceSensor < 100 && distanceSensor != 0) && (bowing == false) ) {

    // start counter to check wether the person bowing or directly prostrating
    if (bowingCounter_start) {
      bowingCounter.start(3000);
      bowingCounter_start = false;
    }

    if (bowingCounter.isFinished()) {
      if (fsr3 < 100 && fsr2 < 100) { // to ensure that the user is not prostrating.
           lcd.setCursor(0, 0);
           lcd.print("Prost     Ps:0/2");
           bowing = true;
           //Serial.println(" ----------------------------------------------------- Bowing Done");
      }else{
        sahoo = true;
        bowingCounter_start = true;
        Serial.println(" ----------------------------------------------------- bowing Sahooo");
      }
    }
  }

  //////////////////////////////////////////////////////

  // prostration 1 detection

  if (bowing && fsr3 > 100 && (prost1 == false)) {
    prost1 = true;
  }

  ///////////////////////////////////////////////////////

  // sitting detection

  if (prost1 && fsr3 < 100 && (sitting == false) ) {

    if (sittingCounter_start) {
      sittingCounter.start(4000);
      sittingCounter_start = false;
      lcd.setCursor(0, 0);
      lcd.print("sitting   Ps:1/2");
    }

    if (sittingCounter.isFinished()) {
      if (fsr2 > 100) {
        lcd.setCursor(0, 0);
        lcd.print("Prost2    Ps:1/2");
        sitting = true;
        //Serial.println(" ----------------------------------------------------- Sitting Done");
      } else {
        sittingCounter_start = true;
        Serial.println(" ----------------------------------------------------- Stitting Sahooo");
        sahoo = true;
      }
    }
  }

  //////////////////////////////////////////////////////

  // prostration 2 detection

  if ( prost1 && sitting && fsr3 > 100 && (prost2 == false) ) {
    prost2 = true;


    //Serial.println(" ----------------------------------------------------- Prost 2 Done");
  }

  if ( prost1 && sitting && fsr3 < 100 && (prost2 == true)) {
    delay(2000);
    if (bigCounter == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Tashahod  Ps:2/2");
      delay(2000);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("standing        ");
      delay(3000);
    }
  }

  //////////////////////////////////////////////////////new

  //  // Frist Tashahod
  //
  //  if( bigCounter == 2  && firstTashahod == false && fsr3 < 100){
  //      delay(2000);
  //
  //      if(fsr2 > 100){
  //       // firstTashahod = true;
  //        lcd.setCursor(0,0);
  //        lcd.print("1st Tashahod DN");
  //      }
  //    }

  ///////////////////////////////////////////////////////

  // Final Tashahod

  if ( bigCounter == 2  && finalTashahod == false && fsr3 < 100) {
    delay(4000);

    if (fsr2 > 100) {
      finalTashahod = true;
      check_sahoo(); // check weather the user should perform sahoo OR not
      delay(5000);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Completed");
      lcd.setCursor(3, 1);
      lcd.print("Prayer");
      delay(5000);

      ISR_backButton();  // we call this function since it does the same purpose that we need here.
      
      if (check_online_mode) {
      BT.write("1");
      }
    }
  }

  ///////////////////////////////////////////////////////

  if ( standing && bowing && prost1 && sitting && prost2 && fsr3 < 100) {

    bigCounter++;
    standing = false;
    bowing = false;
    prost1 = false;
    prost2 = false;
    sitting = false;

    bowingCounter_start = true;
    sittingCounter_start = true;

    lcd.setCursor(12, 1);
    lcd.print(bigCounter);

  }

}
