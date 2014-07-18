/*
  Touch Screen Keypad 
  Uses the Seeed Studio TFT touch screen to display a numerical keypad.
  Features include display box, numerical keypad, enter (checks password)
  and clear (clears password).
  Created by Richard Kirkpatrick, 21 June 2014
  
  Future Upgrades:
    - Make Keypad into a Class
*/

// Define libraries
#include <stdint.h>  
#include <TouchScreen.h>  // Library for the Seeed Studio TFT Touch Shield 
#include <TFT.h>      // Library for the Seeed Studio TFT Touch Shield 
#include <Password.h> // http://playground.arduino.cc/Code/Password
#include <TouchScreenKeypad.h> // Library for keypad
#include <TouchScreenButtons.h>
#include <TouchScreenGeometry.h>
#include <TouchSceenStrings.h>

#ifdef SEEEDUINO
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 14   // can be a digital pin, this is A0
  #define XP 17   // can be a digital pin, this is A3 
#endif

#ifdef MEGA
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 54   // can be a digital pin, this is A0
  #define XP 57   // can be a digital pin, this is A3 
#endif 

//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1
#define TS_MINX 140
#define TS_MAXX 900
#define TS_MINY 120
#define TS_MAXY 940

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
 

TouchScreenKeypad keypad; // Initializes the keypad

void setup() 
{  
  // Initializes TFT library
 Tft.init(); 
 keypad.drawKeypad();
 
 // Set the password
 char* myPassword = "1234";
 keypad.setPassword(myPassword);
}

void loop() 
{
  // A point objects holds x, y, and z coordinates
  Point p = ts.getPoint(); 
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);
  
  int userInput = -1; // -1 means no button is pressed
  if (p.z > ts.pressureThreshhold) 
    userInput = keypad.getButton(p.x, p.y); 
  
  // Highlights the button that was pressed
  keypad.displayButton(userInput);
  
  // Complete the neccessary action based on the user's input
  char textKeypad[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'C', '0', 'E'}; // Text for the buttons
  if (userInput >= 0 && userInput != 9 && userInput != 11) {
    keypad.displayText(textKeypad[userInput]);
    thePassword.append(textKeypad[userInput]); // Append the number to the password
  } else if (userInput == 9) {
    keypad.resetPassword();
  } else if (userInput == 11) {
    keypad.enterPassword();
  }
  delay(10);
}




