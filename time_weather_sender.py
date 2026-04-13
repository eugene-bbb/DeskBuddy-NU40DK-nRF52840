#!/usr/bin/env python3
"""
Weather + Time to NU40-DK via USB Serial
Improved weather condition detection
"""

import serial
import time
import requests
import sys
from datetime import datetime

# settings
SERIAL_PORT = '/dev/cu.usbmodem1101'  # fix as needed (e.g. /dev/ttyACM0 on Linux, COM3 on Windows, /dev/cu.usbmodem* on macOS)
BAUD_RATE = 115200
UPDATE_INTERVAL = 60

CITY = 'Seoul'

def get_weather():
    """weather information from wttr.in"""
    url = f'https://wttr.in/{CITY}?format=j1'
    
    try:
        response = requests.get(url, timeout=10)
        response.raise_for_status()
        data = response.json()
        
        current = data['current_condition'][0]
        temp = float(current['temp_C'])
        humidity = int(current['humidity'])
        condition = current['weatherDesc'][0]['value'].lower()
        
        print(f"original weather: {condition}")  # for debugging
        
        # 더 정확한 날씨 분류
        if 'rain' in condition or 'drizzle' in condition or 'shower' in condition:
            condition = 'Rain'
        elif 'snow' in condition or 'sleet' in condition:
            condition = 'Snow'
        elif 'cloud' in condition or 'overcast' in condition:
            condition = 'Clouds'
        elif 'clear' in condition or 'sunny' in condition or 'fair' in condition:
            condition = 'Clear'
        elif 'mist' in condition or 'fog' in condition or 'haze' in condition:
            condition = 'Clouds'  # show clouds for mist, fog, and haze
        elif 'thunder' in condition or 'storm' in condition:
            condition = 'Rain'  # show rain for thunder and storm
        else:
            # set clear as default if no match found
            condition = 'Clear'
        
        return temp, humidity, condition
    
    except Exception as e:
        print(f"Failed to get weather information: {e}")
        return None, None, None

def get_time():
    """Get current time and date"""
    now = datetime.now()
    time_str = now.strftime("%H:%M:%S")
    date_str = now.strftime("%a, %b %d")  # Mon, Apr 14
    return time_str, date_str

def send_weather(ser, temp, humidity, condition):
    """Send weather data (W,temperature,humidity,condition)"""
    message = f"W,{temp:.1f},{humidity},{condition}\n"
    ser.write(message.encode('utf-8'))
    print(f"Weather sent: {message.strip()}")

def send_time(ser, time_str, date_str):
    """Send time data (T,time,date)"""
    message = f"T,{time_str},{date_str}\n"
    ser.write(message.encode('utf-8'))
    print(f"Time sent: {message.strip()}")

def main():
    # Serial port connection
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Serial port connected: {SERIAL_PORT}")
        time.sleep(2)
    except serial.SerialException as e:
        print(f"Failed to connect to serial port: {e}")
        print("\nFinding serial port:")
        print("  macOS: ls /dev/cu.usbmodem*")
        print("  Linux: ls /dev/ttyACM*")
        print("  Windows: Check COM ports in Device Manager")
        sys.exit(1)
    
    print(f"Weather is updated every {UPDATE_INTERVAL} seconds, and time is updated every 1 second...")
    print("exit: Ctrl+C\n")
    
    last_weather_update = 0
    
    try:
        while True:
            now = time.time()
            
            # sending time is updated every second
            time_str, date_str = get_time()
            send_time(ser, time_str, date_str)
            
            # weather is updated every 60 seconds
            if now - last_weather_update >= UPDATE_INTERVAL:
                temp, humidity, condition = get_weather()
                
                if temp is not None:
                    send_weather(ser, temp, humidity, condition)
                    print(f"  temperature: {temp:.1f}°C")
                    print(f"  humidity: {humidity}%")
                    print(f"  condition: {condition}")
                else:
                    print("Failed to get weather information.")
                
                last_weather_update = now
                print()
            
            time.sleep(1)  # update time every 1 second
    
    except KeyboardInterrupt:
        print("\nProgram exited")
        ser.close()

if __name__ == '__main__':
    main()
