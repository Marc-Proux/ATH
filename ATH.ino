/*
 * 
 * File name : ATH.ino
 * 
 * Description :  Retrieves and processes the information sent by the 
 *                application in order to display it on the ATH.
 * 
 * Coder : Marc PROUX
 * 
 */

#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define pixWidth 128  //Defines the number of pixels wide
#define pixHeight 64 //Defines the number of pixels heigh
#define resetPin -1  //Define de reset Pin (-1 for none)
#define i2cAdress 0x3C  //Sets the address of the i2C display
Adafruit_SSD1306 ecran(pixWidth, pixHeight, &Wire, resetPin);


#define rxPin 2  //Set the pin 2 as RX
#define txPin 3  //Set the pin 4 as TX
#define baudrate 9600  //Set the HC05 baudrate to 9600
#define state 4  // Set the HC05 state pin to 4

SoftwareSerial hc05(rxPin , txPin);
String entry; //Create the Entry variable that will retrieve the HC05 message

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(state, INPUT);
  Serial.begin(19200);
  hc05.begin(baudrate);

  // =====================================
  //        Initialization Display
  // =====================================
  ecran.begin(SSD1306_SWITCHCAPVCC, i2cAdress);
  ecran.clearDisplay(); // Clear the display buffer
  ecran.display(); // Display the buffer on screen
  ecran.setTextColor(WHITE, BLACK); // Set the font color as Black (Pix off) and the Text color to White (Pix on)

}

// =====================================
//               Main loop
// =====================================

void loop() {
  while (hc05.available()){ // While HC05 sebding information to Arduino
    entry += char(hc05.read()); // Add the char sent by the HC05 to the entry string
    Serial.println(entry);
  }
  //Serial.println("End");
  delay(200);
  if (entry!=""){
    sms(); // Call the notif function
    Serial.println("Begin sms");
  }
  bt_state(); // Call the bt_state function
}

// =====================================
//            Sms function
// Will process the entry information to
//      display it on the screen
// =====================================

void sms(){
  String sender = entry.substring(5, entry.indexOf("0x6D")); // Get the id of the sender located between code 0x73 and 0x6D
  String message = entry.substring(entry.indexOf("0x6D")+5); // get the id of the sender located after the code 0x6D
  Serial.println(sender);                                                          
  Serial.println(message);
  clear_text_area(); // Call the clear_text_area function
  ecran.setCursor(0, 15);
  ecran.print(">>> ");
  ecran.print(sender); // Place the sender name in the screen buffer
  ecran.setCursor(0, 30);
  ecran.print(message); // Place the message in the screen buffer
  ecran.display(); // Display the buffer on screen
  delay(5000);
  clear_text_area(); // Call the clear_text_screen function
  entry=""; // Clear the entry variable
  
}

// =====================================
//          bt_state function
//   Checks if the HC05 is connected to 
//    the phone and blocks the program 
//              otherwise
// =====================================

void bt_state() {
  if (digitalRead(state) == 0) { // If the HC05 state is LOW, this means that it is not connected
    ecran.clearDisplay();
    ecran.display();
    ecran.setCursor(0, 32);
    ecran.print("Connect to phone"); // Asks to user to connect the system
    Serial.println("Connect to phone");
    ecran.display();
    while (1) { // Infinite that loop // Infinite loop that blocks the program until the module is reconnected
      if (digitalRead(state) == 1) {  // If the HC05 state is HIGH, this means that it is not connected
        ecran.setCursor(0, 32);
        ecran.clearDisplay();
        ecran.display();
        ecran.print("Connected"); // Notifies the user that they are back online
        Serial.println("Connected");
        ecran.display();
        delay(2000);
        clear_text_area();
        break; // Exits the infinite loop
      }
    }
  }
}

// =====================================
//           clear_text_area
// Clear the part of the screen intended 
//     for displaying notifications
// =====================================

void clear_text_area() {
  for (int width = 0; width < 128; width++) {
    for (int height = 15; height < 64; height++) { 
      ecran.drawPixel(width, height, BLACK); // Turns all pixels in the selected area off
    }
  }
  ecran.display();
}
