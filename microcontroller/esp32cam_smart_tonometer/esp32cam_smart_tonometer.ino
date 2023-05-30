#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <HTTPClient.h>

const String apiHost = "http://192.168.0.132:8080";
const char* ssid = "***";
const char* password = "***";

#define BACKGROUND TFT_BLACK

#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 15
#define BUTTON_PADDING_X 6
#define BUTTON_PADDING_Y 3
#define BUTTON_MARGIN_Y 5
#define BUTTON1_Y 72

#define WIFI_STATUS_BOTTOM_MARGIN 6

#define LEFT "LEFT"
#define RIGHT "RIGHT"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const int BUTTON2_Y = (BUTTON_HEIGHT + (BUTTON_PADDING_Y * 2) + BUTTON1_Y + BUTTON_MARGIN_Y);
const int button1Y = BUTTON1_Y - BUTTON_PADDING_Y;
const int button2Y = BUTTON2_Y - BUTTON_PADDING_Y;
const int buttonWidth = BUTTON_WIDTH + (BUTTON_PADDING_X * 2);
const int buttonHeight = BUTTON_HEIGHT + (BUTTON_PADDING_Y * 2);

struct button {
  byte pin;
  byte state = HIGH;
  byte previousState = HIGH;
  byte lastState = HIGH;

  button(byte pin) : pin(pin) {}

  void initialize() {
    pinMode(pin, INPUT_PULLUP);
  }

  bool isPressed() {
    state = digitalRead(pin);
    previousState = lastState;
    lastState = state;

    return previousState == LOW && state == HIGH;
  }
};

button leftButton = button(13); //1 // 16
button rightButton = button(3);
TFT_eSPI tft = TFT_eSPI();

int tftCenterX;
int tftHeight;
int tftWidth;
int buttonX;

void drawButtonLeft(String text, bool isSelected) {
  tft.setTextSize(2);
  if (isSelected) {
    tft.setTextColor(TFT_WHITE);
    tft.fillRect(buttonX, button1Y, buttonWidth, buttonHeight, TFT_RED);
    tft.drawString(text, tftCenterX + 1, BUTTON1_Y);
  } else {
    tft.setTextColor(TFT_RED);
    tft.fillRect(buttonX, button1Y, buttonWidth, buttonHeight, BACKGROUND);
    tft.drawRect(buttonX, button1Y, buttonWidth, buttonHeight, TFT_RED);
    tft.drawString(text, tftCenterX + 1, BUTTON1_Y);
  }
}

void drawButtonRight(String text, bool isSelected) {
  tft.setTextSize(2);
  if (isSelected) {
    tft.setTextColor(TFT_WHITE);
    tft.fillRect(buttonX - 1, button2Y, buttonWidth, buttonHeight, TFT_BLUE);
    tft.drawString(text, tftCenterX, BUTTON2_Y);
  } else {
    tft.setTextColor(TFT_BLUE);
    tft.fillRect(buttonX - 1, button2Y, buttonWidth, buttonHeight, BACKGROUND);
    tft.drawRect(buttonX - 1, button2Y, buttonWidth, buttonHeight, TFT_BLUE);
    tft.drawString(text, tftCenterX, BUTTON2_Y);
  }
}

void drawStatus(String status, short color) {
  int textWidth = (1 + status.length()) * 3;
  int textX = tftCenterX - textWidth;

  tft.setTextDatum(BL_DATUM);

  tft.fillRect(0, tftHeight - 26, tftWidth, 26, BACKGROUND);

  tft.setTextSize(1);
  tft.setTextColor(color, TFT_BLACK);
  tft.drawString(" " + status, textX, tftHeight - WIFI_STATUS_BOTTOM_MARGIN);
  tft.fillCircle(textX, tftHeight - WIFI_STATUS_BOTTOM_MARGIN - 4, 2, color);

  tft.setTextDatum(TC_DATUM);
}

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if (psramFound()) {
    Serial.println("psram has been found");
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    Serial.println("psram has not been found");
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }

  sensor_t* sensor = esp_camera_sensor_get();

  sensor->set_vflip(sensor, 1);
  sensor->set_hmirror(sensor, 1);
  sensor->set_wb_mode(sensor, 3);
  sensor->set_contrast(sensor, 2);
  sensor->set_saturation(sensor, 2);
  sensor->set_raw_gma(sensor, 1);

  return true;
}

void setup() {
  Serial.begin(9600);

  #pragma region disabling Brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  #pragma endregion

  pinMode(4, OUTPUT);
  // ledcSetup(0, 5000, 8);
  // ledcAttachPin(4, 0);
  // ledcWrite(0, 0);

  leftButton.initialize();
  rightButton.initialize();

  tft.init();
  tft.setRotation(0);

  tftWidth = tft.width();
  tftHeight = tft.height();
  tftCenterX = tftWidth / 2;
  buttonX = tftCenterX - (BUTTON_WIDTH / 2) - BUTTON_PADDING_X;

  tft.fillScreen(BACKGROUND);

  tft.setTextSize(2);

  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_CYAN);

  tft.drawString("SMART", tftCenterX, 8);
  tft.drawString("TONOMETER", tftCenterX, 26);

  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW);

  tft.drawString("SELECT HAND", tftCenterX, 52);

  tft.setTextSize(2);

  drawButtonLeft(LEFT, false);
  drawButtonRight(RIGHT, false);

  drawStatus("starting", TFT_CYAN);

  if (!initCamera()) {
    drawStatus("cam error", TFT_RED);
    return;
  }
  
  WiFi.begin(ssid, password);
  long connectionStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - connectionStart > 10000) {
      drawStatus("network off", TFT_RED);
      WiFi.disconnect();
      return;
    }
    
    drawStatus("connecting", TFT_CYAN);
    delay(500);
  }

  drawStatus("checking", TFT_CYAN);

  HTTPClient http;
  http.begin(apiHost + "/status");

  int httpCode = http.GET();
  if (httpCode <= 0) {
    drawStatus("server off", TFT_RED);
    http.end();
    return;
  }

  drawStatus("ready", TFT_GREEN);
  http.end();
}

void sendMeasurement(String hand) {
  drawStatus("taking photo", TFT_CYAN);

  Serial.println("sendMeasurement: taking photo");

  delay(400);

  digitalWrite(4, HIGH);
  delay(200);

  camera_fb_t* photo = esp_camera_fb_get();
  digitalWrite(4, LOW);

  Serial.println("sendMeasurement: took photo, size " + String(photo->len));

  drawStatus("sending photo", TFT_CYAN);

  Serial.println("sendMeasurement: before http");

  HTTPClient http;

  Serial.println("sendMeasurement: before http.begin");

  http.begin(apiHost + "/measurement?hand=" + hand);

  Serial.println("sendMeasurement: before http.POST");

  int httpCode = http.POST(photo->buf, photo->len);

  Serial.println("sendMeasurement: after http.POST");
  
  if (httpCode != 200) {
    drawStatus("sending error", TFT_RED);
  } else {
    drawStatus("sent photo", TFT_GREEN);
  }

  http.end();

  esp_camera_fb_return(photo);

  drawButtonLeft(LEFT, false);
  drawButtonRight(RIGHT, false);
}

void loop() {
  if (leftButton.isPressed()) {
    Serial.println("left button pressed");
    drawButtonLeft(LEFT, true);
    drawButtonRight(RIGHT, false);
    sendMeasurement("LEFT");
  } else if (rightButton.isPressed()) {
    Serial.println("right button pressed");
    drawButtonLeft(LEFT, false);
    drawButtonRight(RIGHT, true);
    sendMeasurement("RIGHT");
  }
}
