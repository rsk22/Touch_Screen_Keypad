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
#include <Password.h> // http://www.arduino.cc/playground/uploads/Code/Password.zip
#include <TouchScreenGeometry.h>  // Library for drawing shapes for the keypad
#include <TouchScreenStrings.h> // Library for drawing strings for the keypad
#include <TouchScreenButtons.h> // Library for drawing buttons for the keypad


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

// Pre-processor constants
#define widthKeypad 45
#define heightKeypad 45
#define widthTextBlock 216
#define heightTextBlock 35
#define xstartTextBlock 15
#define ystartTextBlock 15
#define fontSize 2
#define noColumns 3
#define noRows 4

// Global constants
const int xstartKeypad[] = {15, 95, 175};           // x-min for keypads
const int ystartKeypad[] = {65, 127, 189, 251};     // y-min for keypads
const int xendKeypad[] = {65, 145, 225};            // x-max for keypads
const int yendKeypad[] = {115, 177, 239, 301};      // y-min for keypads
const int xstartKeypadText[] = {35, 115, 195};      // x-coordinate for keypad numbers 
const int ystartKeypadText[] = {85, 145, 209, 271}; // y-coordinate for keypad numbers

// Global variables 
char textKeypad[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'C', '0', 'E'}; // Text for the buttons
Password password = Password("1234"); // Sets the password
int xPosition = 20;  // Used for positioning the text on the touch pad

// Global instances from TouchScreenGeometry.h and TouchScreenString.h
Button button[12];
TouchScreenChar buttonText[12]; 

void setup() 
{
  // Initializes TFT library
  Tft.init(); 
  
  // Sets the xstart, ystart, width, and height of the textblock and then draws it
  Rectangle textBlock(15, 15, 216, 35, WHITE, BLACK); 
  textBlock.draw(); 

  // Sets the coordinates and sizes of the keypad buttons, and sets all the values for the buttons' text
  for(int y = 0; y < noRows; y++) {
    for(int x = 0; x < noColumns; x++) {
      button[noColumns * y + x].setValues(xstartKeypad[x], ystartKeypad[y], widthKeypad, heightKeypad);
      buttonText[noColumns * y + x].setValues(textKeypad[noColumns * y + x], xstartKeypadText[x], ystartKeypadText[y], 1, WHITE);
    }
  }
    
  // Draws the keypads and their text
  for(int x = 0; x < 12; x++) { 
    button[x].draw(); 
    buttonText[x].drawText(); 
  }
}

void loop() 
{
  int userInput = getButton(); // Gets the user input
  displayButton(userInput);  // Highlights the pressed button
  
  // Complete the neccessary action based on the user's input
  if (userInput >= 0 && userInput != 9 && userInput != 11) {
    displayText(userInput); // Display the number in the textblock
    password.append(textKeypad[userInput]); // Append the number to the password
  } else if (userInput == 9) {
    resetPassword();
  } else if (userInput == 11) {
    enterPassword();
  }
}

int getButton() // Returns the button number.
{
  // A point objects holds x, y, and z coordinates
  Point p = ts.getPoint(); 
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY,TS_MAXY, 320, 0);
  
  if (p.z > ts.pressureThreshhold) {
    for(int y = 0; y < noRows; y++) {
      for(int x = 0; x < noColumns; x++) {
        if ((p.x > xstartKeypad[x] && p.x < xendKeypad[x]) && (p.y > ystartKeypad[y] && p.y < yendKeypad[y])) 
          return noColumns * y + x;
      }
    }
  }
 
 // If no button is presed, return -1 signifying no button was pressed
 return -1; 
}

void displayButton(int buttonNumber)
{
  // Only allow valid buttons to be displayed to prevent errors
  if (buttonNumber >= 0 && buttonNumber <= 12) { 
      button[buttonNumber].buttonDisplay(); 
      buttonText[buttonNumber].textButtonDisplay();
  }
}

void displayText(int i) // Displays the text to the screen
{
  // Ensures text input is not beyond width of screen and only a valid button.
  if (xPosition > 200) {
    // If true, it resets the input and notifies user.
    clearTextblock();
    Tft.drawString("Too many inputs", 65, 30, 1, WHITE);
    delay(2500);
    resetPassword();
  } else {
    Tft.drawChar(textKeypad[i], xPosition, 25, fontSize, WHITE);
    xPosition += 20;
  } 
}

void resetPassword()  // Clears the screen, resets the cursor and resets the password
{
  clearTextblock();
  xPosition = 20;
  password.reset();  
  delay(100);
}

void enterPassword() // Clears screen and evalulates the password.
{
  clearTextblock();
  if (password.evaluate())
    Tft.drawString("Success", 70, 25, 2, WHITE);
  else
    Tft.drawString("Wrong", 80, 25, 2, WHITE);
  delay(1000);
  password.reset();
  clearTextblock();
  xPosition = 20;
}

void clearTextblock() 
{
 Tft.fillRectangle(xstartTextBlock + 1, ystartTextBlock + 1, widthTextBlock - 2, heightTextBlock - 2, BLACK);
}
