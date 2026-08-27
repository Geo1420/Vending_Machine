# Vending_Machine

An automation project for a vending machine controlled by an Arduino Mega / ESP32, with numeric code + RFID authentication and mechanical product control using steppers and servos.

## Project purpose

The vending machine allows:

- entering a numeric code through the keypad
- validating access via RFID
- controlling product release through steppers
- opening a cabinet / compartment through servo motors
- displaying information on an I2C LCD
- monitoring temperature and humidity using the DHT11
- emitting sound signals through a buzzer

## Components used

### Main controller

- Arduino Mega (or compatible board)

### Display

- 16x2 I2C LCD
- I2C address: 0x27

### User input

- 3x4 keypad
- 4 rows × 3 columns

### Authentication

- MFRC522 RFID module
- Connected through SPI

### Actuators

- 2 NEMA 17 stepper motors
- 4 servos for opening compartments or releasing products

### Sensors

- DHT11 for temperature and humidity

### Sound and indication

- Buzzer
- Green LED
- Red LED

## Main pin map

### RFID MFRC522

- SS/SDA: 53
- RST: 48
- MOSI: 51
- MISO: 50
- SCK: 52
- VCC: 3.3V
- GND: GND

### I2C LCD

- SDA: 20
- SCL: 21
- VCC: 5V
- GND: GND

### 3x4 keypad

- Row 1: 30
- Row 2: 31
- Row 3: 32
- Row 4: 33
- Col 1: 34
- Col 2: 35
- Col 3: 36

### Stepper 1

- STEP: 22
- DIR: 23
- EN: 24

### Stepper 2

- STEP: 25
- DIR: 26
- EN: 27

### Servo 1-4

- Servo 1: 40
- Servo 2: 41
- Servo 3: 42
- Servo 4: 43

### Buzzer

- Pin: 8

### DHT11

- Data: 2

### LEDs

- Green LED: 6
- Red LED: 7

## Logical structure of the project

The project works in several stages:

1. The user enters a numeric code on the keypad.
2. The code is validated in software.
3. If the code is valid, the user is prompted to bring an RFID card near the reader.
4. The card is checked against stored values.
5. If the card is valid, access is granted.
6. The steppers are activated to move the product-distribution mechanism.
7. The corresponding servo is activated according to the entered code.
8. Temperature and humidity are displayed on the LCD.
9. Sound signals are emitted for success or error conditions.

## Main workflow

### Activation sequence

- The user enters the code
- The code is validated
- The message "Scan card" is displayed
- RFID reads the card UID
- UID is compared with values stored in `defcard[]`
- If there is a match, the following are activated:
  - steppers
  - corresponding servo
  - confirmation buzzer

### Error handling

- Invalid code → displays "Code invalid" and emits an error sound
- Invalid card → displays "Access denied" and emits an error sound
- RFID blocked or not responding → the RFID module is reset and the system continues

## Libraries used

- `SPI.h`
- `MFRC522.h`
- `Keypad.h`
- `Wire.h`
- `LiquidCrystal_I2C.h`
- `AccelStepper.h`
- `Servo.h`
- `DHT.h`

## Usage precautions

- The MFRC522 RFID module must be powered at 3.3V, not 5V.
- The stepper driver must be connected correctly to the power supply and to the common GND.
- For A4988/TB6600 drivers, it is recommended to use a separate motor power supply, with a common ground.
- Servos and steppers require a sufficiently powerful supply as they draw more current.
- The I2C LCD does not use digital pins; it communicates through I2C (SDA/SCL).

## Possible improvements

- saving codes and cards in EEPROM memory
- adding an admin mode / register new cards
- control through a mobile application
- sales statistics and stock tracking
- centralized monitoring of the machine

## Notes

This project combines access management, physical product distribution, and environment monitoring. It is suitable for small vending machines or laboratory prototypes.

## How to use it

1. Power on the system.
2. Enter the valid code using the keypad.
3. Bring an authorized RFID card close to the reader.
4. The system verifies access and activates the release mechanism.
5. The LCD will display the current status, and the buzzer will indicate success or failure.

## Author

Project created for the automation of a vending machine.
