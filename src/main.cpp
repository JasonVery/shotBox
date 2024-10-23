#include <Arduino.h>

void moveToState(int state);
void moveMotor(int steps);
void moveToHomeState(int steps);

// Stepper motor pins
//*May need to be changed later after prototype
int stepPins[] = {14, 27, 26, 25};

// Button pins, each button corresponds to a state
//*May need to be changed later after prototype
int state0 = 15;
int state1 = 2;
int state2 = 4;
int state3 = 18;

// Since the buttons will be held low when a glass is on top of button
// We need to keep track of which button has been pressed so it only registers one press
bool zeroPressed = false;
bool onePressed = false;
bool twoPressed = false;
bool threePressed = false;

// Keeps track if motor is active or not
// ie when its in a button position it will be active
// If in home state it will be inactive
bool activeMotor = false;
void setup()
{
    // Serial for Debugging
    Serial.begin(115200);
    // Setting up out motor pins
    for (int i = 0; i < 4; i++)
    {
        pinMode(stepPins[i], OUTPUT);
        // Make sure pins are all working
        Serial.println("Pin: " + String(stepPins[i]) + " good to go");
    }
    // Now setting up buttons using internal pullup resistor (heard about it online dont know what it does)
    pinMode(state0, INPUT_PULLUP);
    pinMode(state1, INPUT_PULLUP);
    pinMode(state2, INPUT_PULLUP);
    pinMode(state3, INPUT_PULLUP);
}

// Main loop to control prog
void loop()
{
    // From my understanding == low means button is pressed
    if (digitalRead(state0) == LOW && zeroPressed == false)
    {
        // Print statements just for debugging purposes
        printf("State 0 Has been Pressed\n");
        zeroPressed = true;
        moveToState(0);
    }
    if (digitalRead(state1) == LOW && onePressed == false)
    {
        printf("State 1 Has been Pressed\n");
        onePressed = true;
        moveToState(1);
    }
    if (digitalRead(state2) == LOW && twoPressed == false)
    {
        printf("State 2 Has been Pressed\n");
        twoPressed = true;
        moveToState(2);
    }
    if (digitalRead(state3) == LOW && threePressed == false)
    {
        printf("State 3 Has been Pressed\n");
        threePressed = true;
        moveToState(3);
    }
}

// This function is called when the button is pressed
// And calls the moveMotor Function to move to that state
void moveToState(int state)
{
    switch (state)
    {
    case 0:
        Serial.println("Going to State 0");
        moveMotor(100);
        delay(5000);
        moveToHomeState(100);
        zeroPressed = false;
        break;

    case 1:
        Serial.println("Going to State 1");
        moveMotor(200);
        delay(5000);
        moveToHomeState(200);
        onePressed = false;
        break;

    case 2:
        Serial.println("Going to State 2");
        moveMotor(300);
        delay(5000);
        moveToHomeState(300);
        twoPressed = false;
        break;

    case 3:
        Serial.println("Going to State 3");
        moveMotor(400);
        delay(5000);
        moveToHomeState(400);
        threePressed = false;
        break;

    // Should never get here
    default:
        break;
    }
}

//This function moves motor counterclockwise from
//home state to selected state through button press
void moveMotor(int steps)
{
  //Step sequence is from gpt seems like easiest way to implement 
    int stepSequence[4][4] = {
        {1, 0, 0, 1}, 
        {1, 1, 0, 0}, 
        {0, 1, 1, 0}, 
        {0, 0, 1, 1}  
    };

    
    for (int step = 0; step < steps; step++)
    {
        for (int i = 3; i >= 0; i--)
        { 
            for (int pin = 0; pin < 4; pin++)
            {
                digitalWrite(stepPins[pin], stepSequence[i][pin]);
            }
            //Seems to be the sweet spot for motor speed
            delay(3);
        }
    }

    // Turn off all motor pins so we dont use power when not in use
    for (int pin = 0; pin < 4; pin++)
    {
        digitalWrite(stepPins[pin], LOW);
    }
}

//This fucntion moves motor clockwise from selected state to home
void moveToHomeState(int steps)
{
    int stepSequence[4][4] = {
        {1, 0, 0, 1}, 
        {1, 1, 0, 0}, 
        {0, 1, 1, 0}, 
        {0, 0, 1, 1}  
    };

    // Moves motor clockwise back to home state
    for (int step = 0; step < steps; step++)
    {
        for (int i = 0; i < 4; i++)
        { 
            for (int pin = 0; pin < 4; pin++)
            {
                digitalWrite(stepPins[pin], stepSequence[i][pin]);
            }
            //Sweet spot
            delay(3);
        }
    }

    // Turn off all motor pins
    for (int pin = 0; pin < 4; pin++)
    {
        digitalWrite(stepPins[pin], LOW); 
    }

    Serial.println("Returned to Home State");
}
