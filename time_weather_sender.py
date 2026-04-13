#!/usr/bin/env python3
"""
Weather + Time to NU40-DK via USB Serial
"""

import serial
import time
import requests
import sys
from datetime import datetime

# 설정
SERIAL_PORT = '/dev/cu.usbmodem1101'  # 띤이 포트로 수정
BAUD_RATE = 115200
UPDATE_INTERVAL = 60

CITY = 'Seoul'

def get_weather():
    """wttr.in에서 날씨 정보"""
    url = f'https://wttr.in/{CITY}?format=j1'
    
    try:
        response = requests.get(url, timeout=10)
        response.raise_for_status()
        data = response.json()
        
        current = data['current_condition'][0]
        temp = float(current['temp_C'])
        humidity = int(current['humidity'])
        condition = current['weatherDesc'][0]['value']
        
        if 'rain' in condition.lower():
            condition = 'Rain'
        elif 'cloud' in condition.lower():
            condition = 'Clouds'
        elif 'clear' in condition.lower() or 'sunny' in condition.lower():
            condition = 'Clear'
        elif 'snow' in condition.lower():
            condition = 'Snow'
        else:
            condition = 'Other'
        
        return temp, humidity, condition
    
    except Exception as e:
        print(f"날씨 가져오기 실패: {e}")
        return None, None, None

def get_time():
    """현재 시간과 날짜"""
    now = datetime.now()
    time_str = now.strftime("%H:%M:%S")
    date_str = now.strftime("%a, %b %d")  # Mon, Apr 14
    return time_str, date_str

def send_weather(ser, temp, humidity, condition):
    """날씨 데이터 전송 (W,온도,습도,상태)"""
    message = f"W,{temp:.1f},{humidity},{condition}\n"
    ser.write(message.encode('utf-8'))
    print(f"날씨 전송: {message.strip()}")

def send_time(ser, time_str, date_str):
    """시간 데이터 전송 (T,시:분:초,날짜)"""
    message = f"T,{time_str},{date_str}\n"
    ser.write(message.encode('utf-8'))
    print(f"시간 전송: {message.strip()}")

def main():
    # Serial 포트 연결
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Serial 포트 연결됨: {SERIAL_PORT}")
        time.sleep(2)
    except serial.SerialException as e:
        print(f"Serial 포트 연결 실패: {e}")
        print("\nSerial 포트 찾기:")
        print("  macOS: ls /dev/cu.usbmodem*")
        print("  Linux: ls /dev/ttyACM*")
        print("  Windows: 장치 관리자에서 COM 포트 확인")
        sys.exit(1)
    
    print(f"날씨는 {UPDATE_INTERVAL}초마다, 시간은 1초마다 업데이트합니다...")
    print("종료: Ctrl+C\n")
    
    last_weather_update = 0
    
    try:
        while True:
            now = time.time()
            
            # 시간은 매초 전송
            time_str, date_str = get_time()
            send_time(ser, time_str, date_str)
            
            # 날씨는 60초마다 전송
            if now - last_weather_update >= UPDATE_INTERVAL:
                temp, humidity, condition = get_weather()
                
                if temp is not None:
                    send_weather(ser, temp, humidity, condition)
                    print(f"  온도: {temp:.1f}°C")
                    print(f"  습도: {humidity}%")
                    print(f"  상태: {condition}")
                else:
                    print("날씨 정보를 가져올 수 없습니다.")
                
                last_weather_update = now
                print()
            
            time.sleep(1)  # 1초마다 시간 업데이트
    
    except KeyboardInterrupt:
        print("\n프로그램 종료")
        ser.close()

if __name__ == '__main__':
    main()
