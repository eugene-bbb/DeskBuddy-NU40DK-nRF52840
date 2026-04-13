/*
 * DeskBuddy Complete - Face + Weather + Clock
 * 
 * mode:
 * - mode 0: face with living eyes
 * - mode 1: weather animation window
 * - mode 2: clock + date
 * 
 * button S1 (P0.11) for mode switching
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// custom I2C (P0.07, P0.08)
TwoWire myWire(NRF_TWIM1, NRF_TWIS1, SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, 7, 8);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &myWire, OLED_RESET);

#define BUTTON_PIN 11
#define LED_PIN 13

// ==================================================
// mode management
// ==================================================
int currentMode = 0;
const int MAX_MODES = 3;  // 3 modes!
bool lastButtonState = HIGH;

// ==================================================
// weather data
// ==================================================
float temperature = 23.5;
int humidity = 65;
String weatherCondition = "Clear";

// ==================================================
// time data
// ==================================================
int hours = 12;
int minutes = 0;
int seconds = 0;
String dateStr = "Mon, Apr 14";
unsigned long lastSecondUpdate = 0;

// weather animation frame
int animFrame = 0;
unsigned long lastAnimUpdate = 0;
const int ANIM_SPEED = 100;

// particles
struct Particle {
  float x, y, speed;
  bool active;
};

#define MAX_PARTICLES 15
Particle particles[MAX_PARTICLES];
float cloudX = 0;

// ==================================================
// emotion state (for face mode)
// ==================================================
#define MOOD_NORMAL 0
#define MOOD_HAPPY 1
#define MOOD_SLEEPY 2
#define MOOD_SURPRISED 3
#define MOOD_LOVE 4

int currentMood = MOOD_NORMAL;
unsigned long lastMoodChange = 0;
const unsigned long MOOD_CHANGE_INTERVAL = 15000;

// ==================================================
// eye physics engine
// ==================================================
struct Eye {
  float x, y, w, h;
  float targetX, targetY, targetW, targetH;
  float pupilX, pupilY;
  float targetPupilX, targetPupilY;
  float velX, velY, velW, velH;
  float pVelX, pVelY;
  float k = 0.12, d = 0.60;
  float pk = 0.08, pd = 0.50;
  bool blinking;
  unsigned long lastBlink, nextBlinkTime;
  
  void init(float _x, float _y, float _w, float _h) {
    x = targetX = _x; y = targetY = _y;
    w = targetW = _w; h = targetH = _h;
    pupilX = targetPupilX = pupilY = targetPupilY = 0;
    velX = velY = velW = velH = pVelX = pVelY = 0;
    blinking = false;
    lastBlink = millis();
    nextBlinkTime = millis() + random(2000, 5000);
  }
  
  void update() {
    float ax = (targetX - x) * k;
    float ay = (targetY - y) * k;
    float aw = (targetW - w) * k;
    float ah = (targetH - h) * k;
    
    velX = (velX + ax) * d; velY = (velY + ay) * d;
    velW = (velW + aw) * d; velH = (velH + ah) * d;
    x += velX; y += velY; w += velW; h += velH;
    
    float pax = (targetPupilX - pupilX) * pk;
    float pay = (targetPupilY - pupilY) * pk;
    pVelX = (pVelX + pax) * pd;
    pVelY = (pVelY + pay) * pd;
    pupilX += pVelX; pupilY += pVelY;
  }
};

Eye leftEye, rightEye;
unsigned long lastSaccade = 0;
unsigned long saccadeInterval = 3000;

// ==================================================
// Serial data parsing
// ==================================================
void parseSerialData(String data) {
  // format: "W,15.3,65,Clear" or "T,14:30:25,Mon Apr 14"
  
  if (data.startsWith("W,")) {
    // weather data
    data = data.substring(2); // "W," elimination
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);
    
    if (firstComma > 0 && secondComma > firstComma) {
      temperature = data.substring(0, firstComma).toFloat();
      humidity = data.substring(firstComma + 1, secondComma).toInt();
      weatherCondition = data.substring(secondComma + 1);
      initWeatherAnimation();
    }
  } 
  else if (data.startsWith("T,")) {
    // time data
    data = data.substring(2); // "T," elimination
    int comma = data.indexOf(',');
    
    if (comma > 0) {
      String timeStr = data.substring(0, comma);
      dateStr = data.substring(comma + 1);
      
      // time parsing "14:30:25"
      int colon1 = timeStr.indexOf(':');
      int colon2 = timeStr.indexOf(':', colon1 + 1);
      
      if (colon1 > 0 && colon2 > colon1) {
        hours = timeStr.substring(0, colon1).toInt();
        minutes = timeStr.substring(colon1 + 1, colon2).toInt();
        seconds = timeStr.substring(colon2 + 1).toInt();
        lastSecondUpdate = millis();
      }
    }
  }
}

// ==================================================
// time update (self-counting)
// ==================================================
void updateClock() {
  unsigned long now = millis();
  
  // increasing seconds every 1 second
  if (now - lastSecondUpdate >= 1000) {
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
      }
    }
    lastSecondUpdate = now;
  }
}

// ==================================================
// weather animation (same as before)
// ==================================================
void initWeatherAnimation() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    particles[i].x = random(0, 128);
    particles[i].y = random(-20, 0);
    particles[i].speed = random(2, 5);
    particles[i].active = true;
  }
  cloudX = 0;
  animFrame = 0;
}

void updateWeatherAnimation() {
  animFrame++;
  
  if (weatherCondition == "Rain") {
    for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particles[i].active) {
        particles[i].y += particles[i].speed;
        if (particles[i].y > 45) {
          particles[i].y = random(-10, 0);
          particles[i].x = random(0, 128);
          particles[i].speed = random(2, 5);
        }
      }
    }
  } else if (weatherCondition == "Snow") {
    for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particles[i].active) {
        particles[i].y += particles[i].speed * 0.5;
        particles[i].x += sin(particles[i].y * 0.1) * 0.5;
        if (particles[i].y > 45) {
          particles[i].y = random(-10, 0);
          particles[i].x = random(0, 128);
          particles[i].speed = random(1, 3);
        }
      }
    }
  } else if (weatherCondition == "Clouds") {
    cloudX += 0.3;
    if (cloudX > 128) cloudX = -30;
  }
}

void drawCloud(float x, int y) {
  display.fillCircle(x, y, 8, SSD1306_WHITE);
  display.fillCircle(x + 10, y, 10, SSD1306_WHITE);
  display.fillCircle(x + 20, y, 8, SSD1306_WHITE);
  display.fillCircle(x + 10, y + 5, 12, SSD1306_WHITE);
}

void drawWeatherAnimation() {
  if (weatherCondition == "Clear") {
    int sunX = 64, sunY = 20;
    display.fillCircle(sunX, sunY, 10, SSD1306_WHITE);
    
    for (int i = 0; i < 8; i++) {
      float angle = (i * 45 + animFrame * 2) * DEG_TO_RAD;
      int x1 = sunX + cos(angle) * 12;
      int y1 = sunY + sin(angle) * 12;
      int x2 = sunX + cos(angle) * 18;
      int y2 = sunY + sin(angle) * 18;
      display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    }
    
    if (animFrame % 30 < 15) {
      display.drawPixel(30, 10, SSD1306_WHITE);
      display.drawPixel(90, 15, SSD1306_WHITE);
      display.drawPixel(50, 35, SSD1306_WHITE);
    }
  } else if (weatherCondition == "Rain") {
    drawCloud(50, 10);
    for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particles[i].active) {
        display.drawLine(particles[i].x, particles[i].y, 
                        particles[i].x, particles[i].y + 3, 
                        SSD1306_WHITE);
      }
    }
  } else if (weatherCondition == "Snow") {
    drawCloud(50, 10);
    for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particles[i].active) {
        int x = particles[i].x, y = particles[i].y;
        display.drawPixel(x, y, SSD1306_WHITE);
        display.drawPixel(x-1, y, SSD1306_WHITE);
        display.drawPixel(x+1, y, SSD1306_WHITE);
        display.drawPixel(x, y-1, SSD1306_WHITE);
        display.drawPixel(x, y+1, SSD1306_WHITE);
      }
    }
  } else if (weatherCondition == "Clouds") {
    drawCloud(cloudX, 15);
    drawCloud(cloudX + 50, 25);
    drawCloud(cloudX - 30, 10);
  }
}

void drawTempHumidity() {
  display.drawLine(0, 46, 128, 46, SSD1306_WHITE);
  
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 50);
  display.print((int)temperature);
  
  display.setTextSize(1);
  display.setCursor(35, 50);
  display.print("o");
  display.setCursor(40, 52);
  display.print("C");
  
  display.setTextSize(2);
  display.setCursor(55, 50);
  display.print("/");
  
  display.setTextSize(2);
  display.setCursor(75, 50);
  display.print(humidity);
  
  display.setTextSize(1);
  display.setCursor(105, 55);
  display.print("%");
}

// ==================================================
// drawing clock screen
// ==================================================
void drawClockMode() {
  // date
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 5);
  display.print(dateStr);
  
  display.drawLine(0, 15, 128, 15, SSD1306_WHITE);
  
  // time (large font)
  display.setTextSize(3);
  
  // hour
  char timeStr[3];
  sprintf(timeStr, "%02d", hours);
  display.setCursor(15, 25);
  display.print(timeStr);
  
  // colon (blinking)
  if (seconds % 2 == 0) {
    display.setCursor(50, 25);
    display.print(":");
  }
  
  // minutes
  sprintf(timeStr, "%02d", minutes);
  display.setCursor(65, 25);
  display.print(timeStr);
  
  // seconds (small font)
  display.setTextSize(1);
  display.setCursor(110, 35);
  sprintf(timeStr, "%02d", seconds);
  display.print(timeStr);
  
  // analog clock style decoration
  display.drawCircle(64, 25, 2, SSD1306_WHITE);
}

// ==================================================
// face mode functions
// ==================================================
void randomSaccade() {
  float dx = random(-8, 9);
  float dy = random(-6, 7);
  leftEye.targetPupilX = rightEye.targetPupilX = dx;
  leftEye.targetPupilY = rightEye.targetPupilY = dy;
  saccadeInterval = random(2000, 5000);
}

void checkBlink(Eye &eye) {
  unsigned long now = millis();
  if (!eye.blinking && now > eye.nextBlinkTime) {
    eye.blinking = true;
    eye.lastBlink = now;
  }
  if (eye.blinking) {
    unsigned long blinkDuration = now - eye.lastBlink;
    if (blinkDuration < 100) {
      eye.targetH = 4;
    } else if (blinkDuration < 200) {
      eye.targetH = getMoodEyeHeight();
      eye.blinking = false;
      eye.nextBlinkTime = now + random(2000, 5000);
    }
  }
}

float getMoodEyeWidth() {
  switch(currentMood) {
    case MOOD_SURPRISED: return 40;
    case MOOD_SLEEPY: return 32;
    default: return 36;
  }
}

float getMoodEyeHeight() {
  switch(currentMood) {
    case MOOD_SURPRISED: return 40;
    case MOOD_SLEEPY: return 20;
    case MOOD_HAPPY: return 32;
    default: return 36;
  }
}

void updateMoodEyes() {
  leftEye.targetW = rightEye.targetW = getMoodEyeWidth();
  leftEye.targetH = rightEye.targetH = getMoodEyeHeight();
  
  switch(currentMood) {
    case MOOD_HAPPY:
      leftEye.targetY = rightEye.targetY = 16;
      break;
    case MOOD_SLEEPY:
      leftEye.targetY = rightEye.targetY = 18;
      break;
    default:
      leftEye.targetY = rightEye.targetY = 14;
      break;
  }
}

void changeMood() {
  int newMood = random(0, 5);
  if (newMood == currentMood) newMood = (newMood + 1) % 5;
  currentMood = newMood;
  updateMoodEyes();
}

void drawEye(Eye &eye) {
  int cx = eye.x + eye.w / 2;
  int cy = eye.y + eye.h / 2;
  display.fillEllipse(cx, cy, eye.w / 2, eye.h / 2, SSD1306_WHITE);
  
  if (eye.h > 10) {
    int pupilSize = 8;
    int pupilX = cx + eye.pupilX;
    int pupilY = cy + eye.pupilY;
    int maxOffset = (eye.w / 2) - pupilSize - 2;
    pupilX = constrain(pupilX, cx - maxOffset, cx + maxOffset);
    pupilY = constrain(pupilY, cy - (eye.h / 2) + pupilSize + 2, 
                                cy + (eye.h / 2) - pupilSize - 2);
    display.fillCircle(pupilX, pupilY, pupilSize, SSD1306_BLACK);
    display.fillCircle(pupilX - 3, pupilY - 3, 2, SSD1306_WHITE);
  }
}

void drawMoodParticles() {
  switch(currentMood) {
    case MOOD_HAPPY:
      display.drawPixel(10, 20, SSD1306_WHITE);
      display.drawPixel(11, 19, SSD1306_WHITE);
      display.drawPixel(12, 20, SSD1306_WHITE);
      display.drawPixel(11, 21, SSD1306_WHITE);
      display.drawPixel(116, 20, SSD1306_WHITE);
      display.drawPixel(117, 19, SSD1306_WHITE);
      display.drawPixel(118, 20, SSD1306_WHITE);
      display.drawPixel(117, 21, SSD1306_WHITE);
      break;
    case MOOD_SLEEPY:
      display.setTextSize(1);
      display.setCursor(110, 15);
      display.print("z");
      display.setCursor(115, 10);
      display.print("Z");
      break;
    case MOOD_LOVE:
      for(int i = 0; i < 3; i++) {
        display.drawPixel(62 + i, 50 + i, SSD1306_WHITE);
        display.drawPixel(66 - i, 50 + i, SSD1306_WHITE);
      }
      display.drawPixel(64, 53, SSD1306_WHITE);
      break;
  }
}

void drawFaceMode() {
  drawEye(leftEye);
  drawEye(rightEye);
  drawMoodParticles();
}

// ==================================================
// Setup & Loop
// ==================================================
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  myWire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  
  leftEye.init(18, 14, 36, 36);
  rightEye.init(74, 14, 36, 36);
  
  initWeatherAnimation();
  
  lastSaccade = millis();
  lastMoodChange = millis();
  lastAnimUpdate = millis();
  lastSecondUpdate = millis();
}

void loop() {
  unsigned long now = millis();
  
  // button input
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50);
    currentMode = (currentMode + 1) % MAX_MODES;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    while(digitalRead(BUTTON_PIN) == LOW) delay(10);
    delay(50);
  }
  lastButtonState = buttonState;
  
  // Serial data reception
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    data.trim();
    parseSerialData(data);
  }
  
  // time update (mode 2)
  if (currentMode == 2) {
    updateClock();
  }
  
  // face mode update
  if (currentMode == 0) {
    if (now - lastSaccade > saccadeInterval) {
      randomSaccade();
      lastSaccade = now;
    }
    if (now - lastMoodChange > MOOD_CHANGE_INTERVAL) {
      changeMood();
      lastMoodChange = now;
    }
    checkBlink(leftEye);
    checkBlink(rightEye);
    leftEye.update();
    rightEye.update();
  }
  
  // weather animation update
  if (currentMode == 1 && now - lastAnimUpdate > ANIM_SPEED) {
    updateWeatherAnimation();
    lastAnimUpdate = now;
  }
  
  // drawing screen
  display.clearDisplay();
  
  switch(currentMode) {
    case 0:
      drawFaceMode();
      break;
    case 1:
      drawWeatherAnimation();
      drawTempHumidity();
      break;
    case 2:
      drawClockMode();
      break;
  }
  
  display.display();
  delay(16);
}
