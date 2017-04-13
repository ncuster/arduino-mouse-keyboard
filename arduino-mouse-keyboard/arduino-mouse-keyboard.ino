/*
    arduino-mouse

 */

#include <Button.h>
#include <Mouse.h>
#include <Keyboard.h>

const int rightMouseButton = 7;     // switch to turn on an-d off mouse control (this is our right-mouse click for now)
const int leftMouseButton = 8; // switch to do a leftmouse-click
const int xAxisLeft  = 10;
const int xAxisRight = 12;
const int yAxisUp    = 11;
const int yAxisDown  = 9;
const int ledPin     = 13;        // our built in LED 
const int mouseSpeed = 5;
const int responseDelay = 10;     // response delay of the mouse, in ms

Button rightButton = Button(rightMouseButton, BUTTON_PULLUP_INTERNAL, true, 50);
Button leftButton = Button(leftMouseButton, BUTTON_PULLUP_INTERNAL, true, 50);

boolean mouseIsActive = false;
boolean keyboardIsActive = false;    // whether or not to control the mouse
boolean keyIsDown = false;
int lastSwitchState = LOW;        // previous switch state

void setup() {
    // a little hack so we can set output in setup()
    Serial.begin(9600);
    //while (!Serial) ;
    
    pinMode(ledPin, OUTPUT);                // the LED pin
    pinMode(xAxisLeft, INPUT_PULLUP);
    pinMode(yAxisUp, INPUT_PULLUP);
    pinMode(xAxisRight, INPUT_PULLUP);
    pinMode(yAxisDown, INPUT_PULLUP);
    // take control of the mouse:
    //Keyboard.begin();
}

void loop() {
    // If the button is pressed once, turn on the mouse, pressed again, turn on keyboard, pressed again, turn itself off
    if( rightButton.uniquePress() ) {
        if ( mouseIsActive ) {
            mouseIsActive = false;
            keyboardIsActive = true;
            Mouse.end();
            Keyboard.begin();
        } else if ( keyboardIsActive ) {
            mouseIsActive = false;
            keyboardIsActive = false;
            Mouse.end();
            Keyboard.end();
        } else if ( !mouseIsActive && !keyboardIsActive ) {
            mouseIsActive = true;
            keyboardIsActive = false;
            Mouse.begin();
            Keyboard.end();
        } else {
            Serial.println("We should never get here, problem in keyboard/mouse state checking....");
        }


        /*
        if ( keyboardIsActive ) {
            digitalWrite(ledPin, LOW);
        } else {
            Keyboard.begin();
            digitalWrite(ledPin, HIGH);          
        }
        */

        Serial.print("KeyboardActive:  "); Serial.print(keyboardIsActive); 
        Serial.print(" MouseActive:  "); Serial.println(mouseIsActive);
    }

    
    // if the keyboard control state is active, process keyboard sends from the joystick
    if ( keyboardIsActive ) {
        processKeyboardMovement();
    } else if ( mouseIsActive ) {
        processMouseMovement();
    }
  
    // a delay so the mouse doesn't move too fast:
    delay(responseDelay);
}

// This assumes the mouse is active, that the arduino is actively
// sending mouse events (and not your computer's).
void processKeyboardMovement() {
    int leftState = digitalRead(xAxisLeft);
    int upState = digitalRead(yAxisUp);
    int rightState = digitalRead(xAxisRight);
    int downState = digitalRead(yAxisDown);
    //int clickState = digitalRead(leftButton);

    if ( leftState == 0 ) {
        Keyboard.press('a');
        keyIsDown = true;
    } else if ( rightState == 0 ) {
        Keyboard.press('d');
        keyIsDown = true;
    } else if ( upState == 0 ) {
        Keyboard.press('w');
        keyIsDown = true;
    } else if ( downState == 0 ) {
        Keyboard.press('s');
        keyIsDown = true;
    } else {
        if ( keyIsDown ) {
            Keyboard.releaseAll();
        }
    }
} 

 
// This assumes the mouse is active, that the arduino is actively
// sending mouse events (and not your computer's).
void processMouseMovement() {
    int leftState = digitalRead(xAxisLeft);
    int upState = digitalRead(yAxisUp);
    int rightState = digitalRead(xAxisRight);
    int downState = digitalRead(yAxisDown);
    //int clickState = digitalRead(leftButton);

    // calculate the movement distance based on the button states:
    int  xDistance = (leftState - rightState) * mouseSpeed;
    int  yDistance = (upState - downState) * mouseSpeed;

    // if X or Y is non-zero, move:
    if ( (xDistance != 0) || (yDistance != 0) ) {
        Mouse.move(xDistance, yDistance, 0);
    }

    // Handle button state changes, to support click and drag as well as simple click
    if ( leftButton.stateChanged() ) {
          if ( Mouse.isPressed(MOUSE_LEFT) ) {
              Mouse.release(MOUSE_LEFT);
          } else {
              Mouse.press(MOUSE_LEFT);
          }
    }
}

