# ESP32 Psychometry Exam Timer & Logger

A hardware-software integration project designed to help track, time, and log practice sessions for the Psychometry exam. 

This project uses an ESP32 microcontroller with a physical button and buzzer to run the exam timer, which communicates via a serial connection to a Python script on the PC. The Python script logs the test results (section, time taken, total questions, and correct answers) into a locally saved CSV file for progress tracking.

## 🛠 Hardware Requirements
* **ESP32 Microcontroller**
* **Push Button** (Connected to GPIO 19)
* **Active Buzzer** (Connected to GPIO 18)
* **USB Cable** for Serial communication and power

## 💻 Software Architecture
This project is divided into two parts:
1.  **Firmware (C++):** Runs on the ESP32. Handles the exam logic, physical button interrupts, buzzer alerts, and sends formatted data strings over Serial.
2.  **Software (Python):** Runs on the host PC. Listens to the designated COM port, parses the incoming data, saves results to a CSV file, and allows the user to send inputs back to the ESP32 via the terminal.

## 🚀 Setup & Installation

### 1. Flash the ESP32
1. Open `firmware/esp32_timer/esp32_timer.ino` in the Arduino IDE (or VS Code with PlatformIO).
2. Ensure you have the ESP32 board manager installed.
3. Connect your ESP32, select the correct COM port, and upload the code.

### 2. Setup the Python Environment
1. Ensure Python 3.x is installed on your computer.
2. Open a terminal and navigate to the `software/` directory.
3. Install the required `pyserial` library:
   ```bash
   pip install pyserial
Important: Open pc_logger.py and ensure the SERIAL_PORT variable matches the COM port your ESP32 is connected to (e.g., COM3 for Windows or /dev/ttyUSB0 for Linux/Mac).

🎮 Usage
Connect the ESP32 to your PC.

Run the Python script from your terminal:

Bash
python pc_logger.py
The terminal will act as your interface. Follow the prompts to:

Enter the exam duration in minutes.

Select the section (Quantitative, Verbal, or English).

The timer will start on the ESP32.

To stop the timer early, press the physical button wired to GPIO 19.

When the time is up (or stopped early), the buzzer will sound.

Use the PC terminal to enter your total questions and correct answers.

The Python script will automatically log the session into psychometry_scores.csv.

📁 Output Data Format
The Python script generates a CSV file with the following headers:
Section | Time Took (s) | Total Time (s) | Total Questions | Correct Answers
