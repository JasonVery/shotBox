#include <Arduino.h>
#include <Stepper.h>
#include <math.h>
void addToServiceQueue(int drinkID);
void sortDrinks(int drinkServiceQueue[], int drinkServiceQueueLength);
void serviceRequests();
void resetService();
void disableStepper();
void testMotor();
void testPump();

// Steps of our current motor I believe is 2048
const int STEPS = 2048;

// Creating instance of our motor with pins its attached to
Stepper stepper(STEPS, 14, 26, 27, 25);
// Simple array of the pins so I can turn off and on when needed so it doesnt overheat
const int stepperPins[4] = {14, 26, 27, 25};

// Current pin setup so i dont forget
//  In1 14
//  In2 27
//  In3 26
//  In4 25

// Buttons/Glass locations pins they are connected to
#define drinkZero 15
#define drinkOne 2
#define drinkTwo 0
#define drinkThree 4

// Start Button
#define startService 5

// pump pins
#define pump1 21
#define pump2 22

// Used so we can serice more than 1 state before returning to home
bool zeroPressed = false;
bool onePressed = false;
bool twoPressed = false;
bool threePressed = false;
bool startPressed = false;

// Array to hold each button that gets pressed
int drinkServiceQueue[4];
int drinkServiceQueueLength = 0;

void setup()
{
    // Serial so we can debug
    Serial.begin(115200);

    // Now setting up buttons using internal pullup resistor thanks google
    pinMode(drinkZero, INPUT_PULLUP);
    pinMode(drinkOne, INPUT_PULLUP);
    pinMode(drinkTwo, INPUT_PULLUP);
    pinMode(drinkThree, INPUT_PULLUP);
    pinMode(startService, INPUT_PULLUP);

    // Settin speed of motor will need some tuning
    stepper.setSpeed(7);

    // Setting up pump pins
    pinMode(pump1, OUTPUT);
    pinMode(pump2, OUTPUT);

    // Just making sure queue is empty
    resetService();

    Serial.println("Setup Complete");
}

void loop()
{
    // From my understanding == low means button is pressed
    if (digitalRead(drinkZero) == LOW && zeroPressed == false)
    {
        // Print statements just for debugging purposes
        Serial.println("Drink 0 Has been Pressed");
        zeroPressed = true;
        addToServiceQueue(256);
    }
    if (digitalRead(drinkOne) == LOW && onePressed == false)
    {
        Serial.println("Drink 1 Has been Pressed");
        onePressed = true;
        addToServiceQueue(512);
    }
    if (digitalRead(drinkTwo) == LOW && twoPressed == false)
    {
        Serial.println("Drink 2 Has been Pressed");
        twoPressed = true;
        addToServiceQueue(768);
    }
    if (digitalRead(drinkThree) == LOW && threePressed == false)
    {
        Serial.println("Drink 3 Has been Pressed");
        threePressed = true;
        addToServiceQueue(1024);
    }
    if (digitalRead(startService) == LOW && startPressed == false)
    {
        startPressed = true;
        Serial.println("Start Button Pressed, will begin serivce");
        serviceRequests();
    }
}

// Adds drink postion to service queue once pressed, will be servied once main button is pressed
// When we add to queue we are actually adding the amount of steps to move the motor
void addToServiceQueue(int drinkID)
{
    drinkServiceQueue[drinkServiceQueueLength] = drinkID;
    drinkServiceQueueLength++;
    Serial.print("Drink ");
    Serial.print(drinkID);
    Serial.println(" Has been added to queue");
}

// Simple Bubble sort function, We want to have the motor service the queue from Highest to lowest drink pos
void sortDrinks(int drinkServiceQueue[], int drinkServiceQueueLength)
{
    for (int i = 0; i < drinkServiceQueueLength - 1; i++)
    {
        for (int j = 0; j < drinkServiceQueueLength - i - 1; j++)
        {
            if (drinkServiceQueue[j] < drinkServiceQueue[j + 1])
            {
                int temp = drinkServiceQueue[j];
                drinkServiceQueue[j] = drinkServiceQueue[j + 1];
                drinkServiceQueue[j + 1] = temp;
            }
        }
    }
    Serial.println("Drinks have been sorted highest to lowest");
    for (int i = 0; i < drinkServiceQueueLength; i++)
    {
        Serial.print(drinkServiceQueue[i]);
        Serial.print(" ");
    }
}

// Main function to move the motor to fill glasses
void serviceRequests()
{
    if (drinkServiceQueueLength == 0)
    {
        Serial.println("No drinks to pour");
        return;
    }

    // First we are sorting to the highest to lowest drink
    sortDrinks(drinkServiceQueue, drinkServiceQueueLength);
    // Keeps track of current position of motor
    int currentPos = 0;
    int totalSteps = 0;

    // Array with our buttons, we are changing the logic slightley so drinks will only get
    // Serviced if their position button remains pressed
    int drinkButtons[4] = {drinkZero, drinkOne, drinkTwo, drinkThree};
    // Now we are going to move the motor to each drink in the queue
    for (int i = 0; i < drinkServiceQueueLength; i++)
    {
        int nextDrink = drinkServiceQueue[i];
        // This will calculate the button index- basically just maps the steps to the physical button
        int buttonID = (nextDrink / 256) - 1;

        // If the button does not remain pressed it will jsut skip
        if (digitalRead(drinkButtons[buttonID]) == HIGH)
        {
            continue;
        }

        // First drink will be the furthest away and we want positive steps
        if (i == 0)
        {
            // Current position will be 0 at this point so just add
            totalSteps = nextDrink;
            stepper.step(-totalSteps);
        }
        else
        {
            // All other drinks will need negative steps
            totalSteps = nextDrink - currentPos;
            stepper.step(abs(totalSteps));
        }
        // Updating our current position
        currentPos = nextDrink;
        // Delaying at each position this will have to be updated once pump is added
        delay(2000);
        testPump();
    }

    // Queue should be fully serviced now
    // Returning home
    stepper.step(currentPos);
    currentPos = 0;
    drinkServiceQueueLength = 0;
    Serial.println("Drinks have been poured");
    disableStepper();
    resetService();
}

// This function just resets the buttons so we can service them again
void resetService()
{
    zeroPressed = false;
    onePressed = false;
    twoPressed = false;
    threePressed = false;
    startPressed = false;

    drinkServiceQueueLength = 0;
    for (int i = 0; i < 4; i++)
    {
        drinkServiceQueue[i] = 0;
    }
}

// Just simply turns off motor when in home position so we dont overheat
void disableStepper()
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(stepperPins[i], LOW);
    }
    Serial.println("Stepper Disabled");
}

void testMotor()
{
    stepper.step(200); // Move forward
    delay(2000);
    Serial.println("Moving backward 200 steps");
    stepper.step(-200); // Move backward
    delay(2000);
}

void testPump()
{
    digitalWrite(pump1, HIGH);
    digitalWrite(pump2, HIGH);
    delay(3000);
    digitalWrite(pump1, LOW);
    digitalWrite(pump2, LOW);
    delay(2000);
}
