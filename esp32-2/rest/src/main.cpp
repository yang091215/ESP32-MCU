#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

// ==== 定义引脚 ====
#define SOIL_PIN 34         // 土壤湿度模拟输入
#define DHT_PIN 10          // DHT22 数据引脚
#define DHT_TYPE DHT22
#define RGB_PIN 38           // 板载 WS2812 RGB LED 引脚
#define NUM_PIXELS 1

// ==== WiFi 配置 ====
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";

// ==== 实例 ====
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_NeoPixel pixels(NUM_PIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

// ==== 函数定义 ====
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.show();
}

void connectToWiFi() {
  Serial.print("连接到WiFi中: ");
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi 连接成功！");
    Serial.print("IP 地址: ");
    Serial.println(WiFi.localIP());
    setColor(255, 255, 255);  // 白色闪烁表示连接成功
    delay(1000);
  } else {
    Serial.println("\nWiFi 连接失败！");
    setColor(255, 165, 0);  // 橙色表示连接失败
  }
}

// ==== 初始化 ====
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  dht.begin();

  pixels.begin();              // 初始化 RGB
  pixels.setBrightness(10);    // ⭐ 设置亮度（范围：0-255，推荐 10~50）
  setColor(0, 0, 0);           // 初始关闭 RGB 灯

  connectToWiFi();             // 使用多 SSID 轮询连接
}

void loop() {
  // === 土壤湿度读取 ===
  int soilValue = analogRead(SOIL_PIN);
  float voltage = soilValue * 3.3 / 4095.0;

  // === 温湿度读取 ===
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // === 输出与灯光控制 ===
  Serial.print("湿度值: ");
  Serial.print(soilValue);
  Serial.print(" -> ");
  Serial.print(voltage, 2);
  Serial.print("V | ");

  if (soilValue > 3000) {
    Serial.print("Very Dry 🌵 | ");
    setColor(255, 0, 0);    // 红色
  } else if (soilValue > 2000) {
    Serial.print("Normal 💧 | ");
    setColor(0, 255, 0);    // 绿色
  } else {
    Serial.print("Very Wet 🌊 | ");
    setColor(0, 0, 255);    // 蓝色
  }

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("温湿度读取失败 😢");
    setColor(255, 255, 0);  // 黄色警示
  } else {
    Serial.print("温度: ");
    Serial.print(temperature);
    Serial.print("°C | 湿度: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  delay(3000);
}