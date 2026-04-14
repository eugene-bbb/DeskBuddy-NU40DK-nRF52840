# DeskBuddy - Cute Animated Desktop Companion

> 귀여운 애니메이션 데스크탑 친구

Build your own DeskBuddy using the **NU40-DK** (nRF52840-based board by NUCODE) with an OLED display.

**NU40-DK**(NUCODE의 nRF52840 기반 보드)와 OLED 디스플레이로 나만의 DeskBuddy를 만들어보세요.

You can check the video!
> 링크 추가 예정

![Project Status](https://img.shields.io/badge/status-working-brightgreen)
![Platform](https://img.shields.io/badge/platform-nRF52840-blue)

---

## ✨ Features / 기능

**3 Modes** (switch with S1 button) / **3가지 모드** (S1 버튼으로 전환)

### 👁️ Mode 0: Animated Face / 살아있는 얼굴
- Living eyes with smooth physics engine / 부드러운 물리 엔진 기반 눈동자
- Auto-blinking (random intervals) / 자동 깜빡임 (랜덤 간격)
- Eye movements following a natural pattern / 자연스러운 눈동자 움직임
- Emotion changes every 15 seconds / 15초마다 감정 변화
  - Happy 😊 / Sleepy 😴 / Surprised 😲 / Love ❤️ / Normal 😐

### 🌤️ Mode 1: Weather Window / 날씨 창문
Real-time weather animations / 실시간 날씨 애니메이션:
- ☀️ **Clear** / **맑음**: Rotating sun with rays / 회전하는 태양
- ☔ **Rain** / **비**: Clouds with falling raindrops / 빗방울 떨어지는 구름
- ❄️ **Snow** / **눈**: Snowflakes floating down / 내리는 눈송이
- ☁️ **Clouds** / **흐림**: Moving clouds / 움직이는 구름
- Temperature and humidity display / 온도와 습도 표시

### 🕐 Mode 2: Clock / 시계
- Current time with blinking colon / 깜빡이는 콜론의 현재 시간
- Date display / 날짜 표시
- Runs independently without PC / PC 연결 없이 독립 작동

---

## 🛠️ Hardware Requirements / 하드웨어 요구사항

- **NU40-DK** (nRF52840 development board by NUCODE)
  - NUCODE의 nRF52840 개발 보드
- **128x64 OLED Display** (SSD1306, I2C)
  - Model tested / 테스트된 모델: GM009605 V4.3
- **USB Cable** (for programming and serial communication)
  - 프로그래밍 및 시리얼 통신용 USB 케이블
- Optional / 선택사항: Tactile switch (S1 already built-in on NU40-DK)
  - 택트 스위치 (NU40-DK에 내장됨)

### Wiring / 배선

```
OLED          NU40-DK
────────────────────
GND    →      GND
VCC    →      3.3V
SCK    →      P0.08 (I2C SCL)
SDA    →      P0.07 (I2C SDA)

Button S1: P0.11 (built-in / 내장)
LED1:      P0.13 (built-in / 내장)
```

⚠️ **Important / 중요**: VCC must be connected to **3.3V**, not 5V!
VCC는 반드시 **3.3V**에 연결하세요 (5V 아님!)

---

## 📦 Software Requirements / 소프트웨어 요구사항

### Arduino IDE Setup / Arduino IDE 설정

1. **Install Arduino IDE** / **Arduino IDE 설치**
   - Download from / 다운로드: https://www.arduino.cc/en/software

2. **Add NUCODE Board Support** / **NUCODE 보드 지원 추가**
   - Open Arduino IDE / Arduino IDE 실행
   - Go to / 이동: `File → Preferences` / `파일 → 환경설정`
   - Add this URL to "Additional Board Manager URLs":
   - "추가 보드 매니저 URLs"에 이 URL 추가:
     ```
     https://raw.githubusercontent.com/Nucode01/Adafruit_nRF52_Arduino/refs/heads/master/package_nuduino_index.json
     ```
   - Go to / 이동: `Tools → Board → Boards Manager` / `도구 → 보드 → 보드 매니저`
   - Search for "NUCODE" and install / "NUCODE" 검색 후 설치

3. **Install Libraries** / **라이브러리 설치**
   - `Sketch → Include Library → Manage Libraries` / `스케치 → 라이브러리 포함하기 → 라이브러리 관리`
   - Install these libraries / 다음 라이브러리 설치:
     - **Adafruit GFX Library**
     - **Adafruit SSD1306**

### Python Setup (for Weather & Time) / Python 설정 (날씨 및 시간용)

```bash
# Install Python 3 (if not already installed)
# Python 3 설치 (미설치 시)
# macOS: brew install python3
# Windows: Download from python.org

# Install required packages / 필요한 패키지 설치
pip3 install pyserial requests
```

---

## 🚀 Quick Start / 빠른 시작

### 1. Upload Arduino Code / Arduino 코드 업로드

1. Open `deskbuddy_complete.ino` in Arduino IDE
   - Arduino IDE에서 `deskbuddy_complete.ino` 열기
2. Select board / 보드 선택: `Tools → Board → NUCODE NU40-DK`
   - `도구 → 보드 → NUCODE NU40-DK`
3. Select port / 포트 선택: `Tools → Port → /dev/cu.usbmodem...` (your port / 사용자 포트)
   - `도구 → 포트 → /dev/cu.usbmodem...`
4. Click **Upload** / **업로드** 클릭

### 2. Run Python Script (Optional) / Python 스크립트 실행 (선택사항)

For weather and time features / 날씨 및 시간 기능 사용:

```bash
# Edit the script first / 먼저 스크립트 편집
nano time_weather_sender_fixed.py

# Change these lines / 다음 줄 수정:
SERIAL_PORT = '/dev/cu.usbmodem1101'  # Your port / 사용자 포트
CITY = 'Seoul'  # Your city / 사용자 도시

# Run the script / 스크립트 실행
python3 time_weather_sender_fixed.py
```

**Finding your serial port / 시리얼 포트 찾기:**
- macOS: `ls /dev/cu.usbmodem*`
- Linux: `ls /dev/ttyACM*`
- Windows: Check Device Manager → COM ports
  - 장치 관리자 → COM 포트 확인

### 3. Use It! / 사용하기!

- Press **S1 button** to switch modes
  - **S1 버튼**을 눌러 모드 전환
- Face mode works without PC connection
  - 얼굴 모드는 PC 연결 없이 작동
- Weather & Clock modes need Python script running
  - 날씨 및 시계 모드는 Python 스크립트 실행 필요

---

## 📖 How It Works / 작동 원리

### Data Flow / 데이터 흐름

```
┌──────────────┐  Weather API   ┌─────────────┐  USB Serial  ┌──────────┐
│  wttr.in     │ ─────────────→ │   Python    │ ───────────→ │ NU40-DK  │
│  (Free API)  │                │   Script    │              │   OLED   │
│  (무료 API)  │                │  (스크립트)  │              │          │
└──────────────┘                └─────────────┘              └──────────┘
                                      ↓
                                Time: Every 1s / 시간: 1초마다
                                Weather: Every 60s / 날씨: 60초마다
```

### Serial Protocol / 시리얼 프로토콜

**Weather data / 날씨 데이터:**
```
W,23.5,65,Clear
```
Format / 형식: `W,temperature,humidity,condition`
형식: `W,온도,습도,상태`

**Time data / 시간 데이터:**
```
T,14:30:25,Mon, Apr 14
```
Format / 형식: `T,HH:MM:SS,date_string`
형식: `T,시:분:초,날짜문자열`

---

## ⚙️ Customization / 커스터마이징

### Change Weather Update Interval / 날씨 업데이트 간격 변경

Edit / 편집: `time_weather_sender_fixed.py`
```python
UPDATE_INTERVAL = 300  # Update every 5 minutes / 5분마다 업데이트
```

### Change City / 도시 변경

Edit / 편집: `time_weather_sender_fixed.py`
```python
CITY = 'Tokyo'  # Change to your city / 원하는 도시로 변경
CITY = 'Busan'  # 부산
CITY = 'New York'  # 뉴욕
```

### Change Emotion Interval / 감정 변화 간격 변경

Edit / 편집: `deskbuddy_complete.ino`
```cpp
const unsigned long MOOD_CHANGE_INTERVAL = 30000;  // 30 seconds / 30초
```

### Adjust Animation Speed / 애니메이션 속도 조절

Edit / 편집: `deskbuddy_complete.ino`
```cpp
const int ANIM_SPEED = 150;  // Slower animation (default: 100)
                              // 느린 애니메이션 (기본값: 100)
```

---

## 🐛 Troubleshooting / 문제 해결

### OLED doesn't turn on / OLED가 켜지지 않음
- Check wiring (especially VCC to 3.3V, not 5V!)
  - 배선 확인 (특히 VCC는 3.3V에, 5V 아님!)
- Check I2C address (0x3C or 0x3D)
  - I2C 주소 확인 (0x3C 또는 0x3D)
- Verify OLED model is SSD1306
  - OLED 모델이 SSD1306인지 확인

### Serial port busy error / 시리얼 포트 사용 중 오류
- Close Arduino IDE Serial Monitor
  - Arduino IDE 시리얼 모니터 닫기
- Make sure no other program is using the port
  - 다른 프로그램이 포트를 사용하지 않는지 확인
- Try / 시도: `sudo killall -9 cu` (macOS/Linux)

### Weather not updating / 날씨가 업데이트 안 됨
- Check internet connection / 인터넷 연결 확인
- Python script must be running / Python 스크립트가 실행 중이어야 함
- Check serial port in Python script matches Arduino port
  - Python 스크립트의 시리얼 포트가 Arduino 포트와 일치하는지 확인

### Weather shows "Other" / 날씨가 "Other"로 표시됨
- Use `time_weather_sender_fixed.py` instead of original
  - 원본 대신 `time_weather_sender_fixed.py` 사용
- This version has better weather condition detection
  - 이 버전은 날씨 상태 인식이 개선됨

### Button not working / 버튼이 작동 안 함
- Built-in S1 button is on P0.11 / 내장 S1 버튼은 P0.11
- Check if LED1 toggles when pressing button
  - 버튼 누를 때 LED1이 토글되는지 확인

---

## 📝 Credits / 크레딧

- Original DeskBuddy concept: [Edison Science Corner](https://www.youtube.com/@EdisonScienceCorner)
  - 원본 DeskBuddy 컨셉
- Hardware / 하드웨어: NUCODE NU40-DK (nRF52840)
- Weather API: [wttr.in](https://wttr.in) (free, no API key needed / 무료, API 키 불필요)
- Libraries / 라이브러리: Adafruit GFX, Adafruit SSD1306
