#include <Arduino.h>
#include <Stepper.h>

void enqueue(int state);
int dequeue();
void processServiceQueue();

const int stepsPerRevolution = 2048;

// Will need to adjust this time based on what pump I get
const unsigned long pourTime = 4000;

// Motor driver Pins
#define IN1 14
#define IN2 27
#define IN3 26
#define IN4 25

// Buttons/States
#define state0 15
#define state1 2
#define state2 4
#define state3 18

// Used so we can serice more than 1 state before returning to home
bool zeroPressed = false;
bool onePressed = false;
bool twoPressed = false;
bool threePressed = false;

// Using a linked list as a service queue
struct Node
{
    int state;
    Node *next;
};

Node *head = NULL;
Node *tail = NULL;

// Intializing Stepper Motor
Stepper stepper(stepsPerRevolution, IN1, IN2, IN3, IN4);

unsigned long previousMillis = 0;
bool isProcessing = false;
int currentState = -1;

void setup()
{
    // Serial so we can debug
    Serial.begin(115200);
    // Sets speed using library
    stepper.setSpeed(9);

    // Now setting up buttons using internal pullup resistor
    pinMode(state0, INPUT_PULLUP);
    pinMode(state1, INPUT_PULLUP);
    pinMode(state2, INPUT_PULLUP);
    pinMode(state3, INPUT_PULLUP);
}

void loop()
{
    // From my understanding == low means button is pressed
    if (digitalRead(state0) == LOW && zeroPressed == false)
    {
        // Print statements just for debugging purposes
        printf("State 0 Has been Pressed\n");
        zeroPressed = true;
        enqueue(0);
    }
    if (digitalRead(state1) == LOW && onePressed == false)
    {
        printf("State 1 Has been Pressed\n");
        onePressed = true;
        enqueue(1);
    }
    if (digitalRead(state2) == LOW && twoPressed == false)
    {
        printf("State 2 Has been Pressed\n");
        twoPressed = true;
        enqueue(2);
    }
    if (digitalRead(state3) == LOW && threePressed == false)
    {
        printf("State 3 Has been Pressed\n");
        threePressed = true;
        enqueue(3);
    }

    processServiceQueue();
}

// Pretty standard linked list implementation
// To add new states to queue
void enqueue(int state)
{
    Node *newNode = new Node;
    newNode->state = state;
    newNode->next = NULL;
    if (tail == NULL)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }
}
// Again pretty standard linked list implementation to dequeue
// could possibly be void but unsure
int dequeue()
{
    // Should never get here but just in case
    if (head == NULL)
    {
        // Just incase we see this issue
        printf("Queue is Empty\n");
        return -1;
    }

    Node *temp = head;
    int state = head->state;
    head = head->next;

    if (head == NULL)
    {
        tail = NULL;
    }

    delete temp;
    return state;
}

bool isEmpty()
{
    return head == NULL;
}

// Once queue is added to queue this will iterate through
// the queue and process each state in order
// Might add some logic to make if more efficient
// Like if we are in state 3 and next is 0 but we also have state 2
// Go from state 3 -> 2 -> 0 instead of 3 -> 0 -> 2
void processServiceQueue()
{
    unsigned long currentMillis = millis();

    // Check if currently processing a state
    if (!isProcessing && !isEmpty())
    {
        // Start processing the next state in the queue
        currentState = dequeue();
        Serial.print("Processing state: ");
        Serial.println(currentState);
        stepper.step(200);
        isProcessing = true;
        previousMillis = currentMillis; // Start timing
    }

    // If a state is being processed, check if delay has passed
    if (isProcessing && (currentMillis - previousMillis >= pourTime))
    {
        // Finish processing the state and reset
        Serial.print("Completed state: ");
        Serial.println(currentState);
        isProcessing = false;
        currentState = -1;
    }
}
