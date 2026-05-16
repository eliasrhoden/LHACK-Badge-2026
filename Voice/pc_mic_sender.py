import serial
import sounddevice as sd
import numpy as np
import time
import sys

# --- CONFIGURATION ---
# IMPORTANT: Change this to match your ESP32's COM port! 
# You can find this in the Arduino IDE (Tools > Port)
SERIAL_PORT = 'COM3' 
BAUD_RATE = 115200
# ---------------------

try:
    print(f"Attempting to connect to ESP32 on {SERIAL_PORT}...")
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) # Wait for the serial connection to initialize
    print("Successfully connected to ESP32!")
except Exception as e:
    print(f"Error connecting to serial port: {e}")
    print(f"Please ensure your ESP32 is plugged in and the port '{SERIAL_PORT}' is correct.")
    print("Also ensure the Arduino IDE Serial Monitor is CLOSED.")
    sys.exit(1)

def print_sound(indata, outdata, frames, time_info, status):
    """Callback function that receives audio data from the microphone."""
    if status:
        print(status)
    
    # Calculate the volume (Root Mean Square of the audio chunk)
    volume_norm = np.linalg.norm(indata) * 10
    int_volume = int(volume_norm)
    
    # Calculate dominant frequency
    channel_data = indata[:, 0] if indata.ndim > 1 else indata
    fft_result = np.fft.rfft(channel_data)
    magnitude = np.abs(fft_result)
    
    dominant_freq = 0
    if len(magnitude) > 1:
        peak_idx = np.argmax(magnitude[1:]) + 1
        samplerate = sd.default.samplerate if sd.default.samplerate else 44100
        freqs = np.fft.rfftfreq(len(channel_data), 1.0/samplerate)
        dominant_freq = freqs[peak_idx]
        
    int_freq = int(dominant_freq)
    
    try:
        # Send the volume and frequency data
        ser.write(f"{int_volume},{int_freq}\n".encode('utf-8'))
    except Exception as e:
        print(f"Failed to send data: {e}")
        raise sd.CallbackStop()

print("Listening to microphone... Press Ctrl+C to stop.")

try:
    # Start capturing audio from the default microphone
    with sd.Stream(callback=print_sound):
        # Keep the script running forever
        while True:
            time.sleep(0.1)
except KeyboardInterrupt:
    print("\nStopping...")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial port closed.")
