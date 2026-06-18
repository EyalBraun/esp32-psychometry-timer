import serial
import threading
import csv
import os
import sys
import time

# --- CONFIGURATION ---
SERIAL_PORT = 'COM3'  # Update this if your port is different
BAUD_RATE = 115200
CSV_FILENAME = 'psychometry_scores.csv'

# Open the serial port once
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)

# Force a Hardware Reset by toggling DTR
ser.dtr = False
time.sleep(0.1)
ser.dtr = True
time.sleep(0.5)  # Wait for the ESP32 to boot up

# Function to read data from ESP32 and print to screen
def read_from_esp():
    file_exists = os.path.isfile(CSV_FILENAME)
    while True:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                # If we catch the data row, save it
                if line.startswith("CSV_DATA:"):
                    raw_data = line.replace("CSV_DATA:", "")
                    data_list = raw_data.split(',')
                    if len(data_list) == 5:
                        with open(CSV_FILENAME, 'a', newline='', encoding='utf-8-sig') as f:
                            writer = csv.writer(f)
                            if not file_exists:
                                writer.writerow(['Section', 'Time Took (s)', 'Total Time (s)', 'Total Questions', 'Correct Answers'])
                            writer.writerow(data_list)
                        print(f"\n[>>>] Data saved to {CSV_FILENAME}: {data_list}", flush=True)
                else:
                    # Print normal ESP32 output
                    print(line, flush=True)
        except Exception:
            break

# Start reading thread
thread = threading.Thread(target=read_from_esp, daemon=True)
thread.start()

print(f"--- Connected to {SERIAL_PORT}. ESP32 Reset ---", flush=True)
print("Type your inputs here and press Enter:", flush=True)

try:
    while True:
        # Get input from keyboard
        user_input = sys.stdin.readline()
        if user_input:
            ser.write(user_input.encode())
except KeyboardInterrupt:
    print("\nClosing connection...")
    ser.close()
