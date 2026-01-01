/*
 * Project: Bluetooth Mobile Robot (Rover Module)
 * Source: Sem 2 Project Report [cite: 10, 161]
 * Description: Non-blocking smooth acceleration/deceleration control using millis().
 */

#include <SoftwareSerial.h>

// --- Pin Definitions ---
const int bluetoothTx = 10; // Arduino TX -> BT RX [cite: 165]
const int bluetoothRx = 11; // Arduino RX -> BT TX [cite: 165]

// Left Motor Group (Motor A on L298N)
const int enLeftMotors = 9;  // Enable Pin (PWM) [cite: 168]
const int inLeft1 = 8;       // Input 1 [cite: 169]
const int inLeft2 = 7;       // Input 2 [cite: 169]

// Right Motor Group (Motor B on L298N)
const int enRightMotors = 3; // Enable Pin (PWM) [cite: 172]
const int inRight3 = 5;      // Input 3 [cite: 173]
const int inRight4 = 4;      // Input 4 [cite: 175]

// --- Bluetooth Object ---
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx); [cite: 178]

// --- Global Variables ---
char receivedChar;
int currentUserSpeed = 0;   // 0-100 [cite: 181]
int currentPwmSpeed = 0;    // 0-255 [cite: 182]
int targetPwmSpeed = 0;     // 0-255 [cite: 183]

// Velocity Profiling Config (Smooth Acceleration)
const int accelerationStep = 5;   // PWM increment per step [cite: 184]
const int accelerationDelay = 20; // Time (ms) between steps [cite: 184]
unsigned long lastAccelerationTime = 0;

enum CarDirection {
    STOPPED, FORWARD, BACKWARD, LEFT, RIGHT,
    FORWARD_LEFT, FORWARD_RIGHT, BACK_LEFT, BACK_RIGHT
};

CarDirection currentDirection = STOPPED;

// --- Function Prototypes ---
void updateMotorSpeeds();
int mapUserSpeedToPwm(int userSpeed);
void setTargetSpeed(int targetUserSpeed);
void handleSpeedTransition();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void moveForwardLeft();
void moveForwardRight();
void moveBackLeft();
void moveBackRight();
void stopCar();

// --- Setup ---
void setup() {
    Serial.begin(9600);
    bluetooth.begin(9600); 
    
    pinMode(enLeftMotors, OUTPUT);
    pinMode(inLeft1, OUTPUT);
    pinMode(inLeft2, OUTPUT);
    pinMode(enRightMotors, OUTPUT);
    pinMode(inRight3, OUTPUT);
    pinMode(inRight4, OUTPUT);

    stopCar();
    Serial.println("System Ready: Bluetooth Mobile Robot");
}

// --- Main Loop ---
void loop() {
    // 1. Non-Blocking Command Parsing
    if (bluetooth.available()) {
        receivedChar = bluetooth.read();
        switch (receivedChar) {
            case 'F': moveForward(); break; [cite: 234]
            case 'B': moveBackward(); break; [cite: 237]
            case 'L': turnLeft(); break; [cite: 240]
            case 'R': turnRight(); break; [cite: 243]
            case 'G': moveForwardLeft(); break; [cite: 246]
            case 'I': moveForwardRight(); break; [cite: 249]
            case 'H': moveBackLeft(); break; [cite: 252]
            case 'J': moveBackRight(); break; [cite: 255]
            case 'S': 
            case 'D': stopCar(); break; [cite: 259]
            // Speed Commands
            case '0': setTargetSpeed(0); break;
            case '1': setTargetSpeed(10); break;
            case '2': setTargetSpeed(20); break;
            case '3': setTargetSpeed(30); break;
            case '4': setTargetSpeed(40); break;
            case '5': setTargetSpeed(50); break;
            case '6': setTargetSpeed(60); break;
            case '7': setTargetSpeed(70); break;
            case '8': setTargetSpeed(80); break;
            case '9': setTargetSpeed(90); break;
            case 'q': setTargetSpeed(100); break; [cite: 294]
        }
    }

    // 2. Non-Blocking Velocity Profiling
    handleSpeedTransition(); [cite: 301]
}

// --- Helper Functions ---

int mapUserSpeedToPwm(int userSpeed) {
    return map(userSpeed, 0, 100, 0, 255); [cite: 306]
}

void updateMotorSpeeds() {
    int leftPWM = currentPwmSpeed;
    int rightPWM = currentPwmSpeed;

    // Differential Drive Logic for Smooth Turns
    if (currentDirection == FORWARD_LEFT || currentDirection == BACK_LEFT) {
        leftPWM = currentPwmSpeed / 2; // Slow down inner wheel [cite: 320]
    } else if (currentDirection == FORWARD_RIGHT || currentDirection == BACK_RIGHT) {
        rightPWM = currentPwmSpeed / 2; [cite: 322]
    } else if (currentDirection == STOPPED) {
        leftPWM = 0;
        rightPWM = 0;
    }

    analogWrite(enLeftMotors, leftPWM);
    analogWrite(enRightMotors, rightPWM);
}

void setTargetSpeed(int targetUserSpeed) {
    if (targetUserSpeed < 0) targetUserSpeed = 0;
    if (targetUserSpeed > 100) targetUserSpeed = 100;
    
    currentUserSpeed = targetUserSpeed;
    targetPwmSpeed = mapUserSpeedToPwm(targetUserSpeed);
    
    if (targetPwmSpeed == 0) {
        stopCar();
    }
}

// THE CORE ALGORITHM: Soft-Start / Soft-Stop [cite: 23, 116]
void handleSpeedTransition() {
    if (currentPwmSpeed != targetPwmSpeed) {
        // Check if enough time has passed (Non-blocking delay) [cite: 119]
        if (millis() - lastAccelerationTime >= accelerationDelay) {
            if (targetPwmSpeed > currentPwmSpeed) {
                currentPwmSpeed += accelerationStep;
                if (currentPwmSpeed > targetPwmSpeed) currentPwmSpeed = targetPwmSpeed;
            } else {
                currentPwmSpeed -= accelerationStep;
                if (currentPwmSpeed < targetPwmSpeed) currentPwmSpeed = targetPwmSpeed;
            }
            updateMotorSpeeds();
            lastAccelerationTime = millis();
        }
    }
}

// --- Motor Logic ---

void moveForward() {
    currentDirection = FORWARD;
    digitalWrite(inLeft1, HIGH); digitalWrite(inLeft2, LOW);
    digitalWrite(inRight3, HIGH); digitalWrite(inRight4, LOW);
    updateMotorSpeeds();
}

void moveBackward() {
    currentDirection = BACKWARD;
    digitalWrite(inLeft1, LOW); digitalWrite(inLeft2, HIGH);
    digitalWrite(inRight3, LOW); digitalWrite(inRight4, HIGH);
    updateMotorSpeeds();
}

void turnLeft() {
    currentDirection = LEFT;
    // Pivot Turn (Wheels rotate opposite)
    digitalWrite(inLeft1, LOW); digitalWrite(inLeft2, HIGH);
    digitalWrite(inRight3, HIGH); digitalWrite(inRight4, LOW);
    updateMotorSpeeds();
}

void turnRight() {
    currentDirection = RIGHT;
    // Pivot Turn
    digitalWrite(inLeft1, HIGH); digitalWrite(inLeft2, LOW);
    digitalWrite(inRight3, LOW); digitalWrite(inRight4, HIGH);
    updateMotorSpeeds();
}

void moveForwardLeft() {
    currentDirection = FORWARD_LEFT;
    digitalWrite(inLeft1, HIGH); digitalWrite(inLeft2, LOW);
    digitalWrite(inRight3, HIGH); digitalWrite(inRight4, LOW);
    updateMotorSpeeds();
}

void moveForwardRight() {
    currentDirection = FORWARD_RIGHT;
    digitalWrite(inLeft1, HIGH); digitalWrite(inLeft2, LOW);
    digitalWrite(inRight3, HIGH); digitalWrite(inRight4, LOW);
    updateMotorSpeeds();
}

void moveBackLeft() {
    currentDirection = BACK_LEFT;
    digitalWrite(inLeft1, LOW); digitalWrite(inLeft2, HIGH);
    digitalWrite(inRight3, LOW); digitalWrite(inRight4, HIGH);
    updateMotorSpeeds();
}

void moveBackRight() {
    currentDirection = BACK_RIGHT;
    digitalWrite(inLeft1, LOW); digitalWrite(inLeft2, HIGH);
    digitalWrite(inRight3, LOW); digitalWrite(inRight4, HIGH);
    updateMotorSpeeds();
}

void stopCar() {
    currentDirection = STOPPED;
    digitalWrite(inLeft1, LOW); digitalWrite(inLeft2, LOW);
    digitalWrite(inRight3, LOW); digitalWrite(inRight4, LOW);
    setTargetSpeed(0);
}
