#include <FS.h>
#include <LittleFS.h>
#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPI.h>

// --- ESP32-C3 Super Mini Pin Definitions ---
#define PIN_SCL   4   
#define PIN_SDA   6   
#define PIN_CS    7   
#define PIN_DC    10   
#define PIN_RES   3   

// ⚠️ Keeps software timer to prevent freezing
#define PIN_BUSY  5 

#define PIN_MISO  8   

// --- WiFi Credentials ---
const char* ssid = "T&TGuest";
const char* password = "T&TGuest";

// --- Static IP ---
IPAddress local_IP(192, 168, 1, 200); 
IPAddress gateway(192, 168, 1, 1);    
IPAddress subnet(255, 255, 255, 0);   
IPAddress primaryDNS(8, 8, 8, 8);     

WebServer server(80);

// --- UPDATE FLAG ---
bool shouldUpdateScreen = false;

// --- Display Object ---
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
  GxEPD2_420_GDEY042T81(PIN_CS, PIN_DC, PIN_RES, PIN_BUSY)
);

String readFile(const char* path) {
  File file = LittleFS.open(path, "r");
  if (!file) return "";
  String content = file.readString();
  file.close();
  content.trim();
  return content;
}

void writeFile(const char* path, const String& message) {
  File file = LittleFS.open(path, "w");
  if (file) {
    file.print(message);
    file.close();
  }
}

int centerText(const char *text, const GFXfont *font) {
  display.setFont(font);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return (400 - w) / 2;
}

void drawHappyHourMenu();

void setup() {
  Serial.begin(115200);
  delay(2000); 

  Serial.println("\n\n--- ESP32-C3 Super Mini Start ---");
  
  SPI.begin(PIN_SCL, PIN_MISO, PIN_SDA, PIN_CS);

  Serial.println("📶 Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); 
  
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("⚠️ STA Failed to configure");
  }

  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 30) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (!LittleFS.begin(true)) {
    Serial.println("❌ LittleFS Mount Failed");
  } else {
    Serial.println("\n✅ LittleFS mounted");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi");
    Serial.print("🌐 FIXED IP Address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("menu")) {            
      Serial.println("✅ mDNS active: http://menu.local");
      MDNS.addService("http", "tcp", 80);
    }

    server.on("/", []() {
      File f = LittleFS.open("/index.html", "r");
      if (!f) { server.send(404, "text/plain", "index.html not found"); return; }
      server.streamFile(f, "text/html");
      f.close();
    });
    server.on("/script.js", []() {
      File f = LittleFS.open("/script.js", "r");
      if (!f) { server.send(404, "text/plain", "script.js not found"); return; }
      server.streamFile(f, "application/javascript");
      f.close();
    });
    server.on("/style.css", []() {
      File f = LittleFS.open("/style.css", "r");
      if (!f) { server.send(404, "text/plain", "style.css not found"); return; }
      server.streamFile(f, "text/css");
      f.close();
    });
    server.on("/beer.txt", []() {
      File f = LittleFS.open("/beer.txt", "r");
      if (!f) { server.send(404, "text/plain", "beer.txt not found"); return; }
      server.streamFile(f, "text/plain");
      f.close();
    });
    server.on("/food.txt", []() {
      File f = LittleFS.open("/food.txt", "r");
      if (!f) { server.send(404, "text/plain", "food.txt not found"); return; }
      server.streamFile(f, "text/plain");
      f.close();
    });

    server.on("/update", HTTP_POST, []() {
      String beer = server.arg("beer");
      String food = server.arg("food");
      writeFile("/beer.txt", beer);
      writeFile("/food.txt", food);
      
      server.send(200, "text/plain", "Saved! Updating screen...");
      shouldUpdateScreen = true;
    });

    server.begin();
    Serial.println("✅ Web server started");
    Serial.print("🔗 Android Access: http://");
    Serial.println(WiFi.localIP());
  } 

  display.init(115200);
  display.setRotation(0); 
  display.setFullWindow();

  drawHappyHourMenu();
  Serial.println("✅ SETUP COMPLETE. Loop started.");
}

void loop() {
  server.handleClient();
  
  if (shouldUpdateScreen) {
    drawHappyHourMenu();
    shouldUpdateScreen = false;
  }
  
  delay(2); 
}

void drawHappyHourMenu() {
  String beerMenu = readFile("/beer.txt");
  String foodMenu = readFile("/food.txt");

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    // --- Header (Moved up 10px) ---
    const char *header = "TABLES & TAPS";
    int x_header = centerText(header, &FreeSansBold18pt7b);
    display.setFont(&FreeSansBold18pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(x_header, 30); // Was 40
    display.print(header);

    display.drawLine(20, 40, 380, 40, GxEPD_BLACK); // Was 50

    // --- Happy Hour Line (Moved up 10px) ---
    const char *happyHour = "Happy Hour: 3PM - 6PM Mon - Fri Only";
    int x_happy = centerText(happyHour, &FreeMonoBold9pt7b);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(x_happy, 65); // Was 75
    display.print(happyHour);

    // --- Beer Box (Moved up 10px, Height increased by 20px) ---
    // y: 80 (was 90), h: 205 (was 185)
    display.drawRoundRect(20, 80, 170, 205, 6, GxEPD_BLACK); 
    display.setFont(&FreeSans9pt7b);
    display.setCursor(40, 105); // Was 115
    display.print("Draft Beers - $4");

    display.setFont();
    if (beerMenu.length() > 0) {
      int y = 120; // Was 130             
      int lineHeight = 18;      
      int start = 0, end;
      while ((end = beerMenu.indexOf('\n', start)) != -1) {
        String line = beerMenu.substring(start, end);
        display.setCursor(30, y);
        display.print(line);
        y += lineHeight;
        start = end + 1;
      }
      if (start < beerMenu.length()) {
        display.setCursor(30, y);
        display.print(beerMenu.substring(start));
      }
    } else {
      display.setCursor(30, 130);
      display.print("No beer menu found");
    }

    // --- Food Box (Moved up 10px, Height increased by 20px) ---
    // y: 80 (was 90), h: 205 (was 185)
    display.drawRoundRect(210, 80, 170, 205, 6, GxEPD_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(235, 105); // Was 115
    display.print("Food Specials");

    display.setFont();
    if (foodMenu.length() > 0) {
      int y = 120; // Was 130
      int lineHeight = 18;
      int start = 0, end;
      while ((end = foodMenu.indexOf('\n', start)) != -1) {
        String line = foodMenu.substring(start, end);
        display.setCursor(220, y);
        display.print(line);
        y += lineHeight;
        start = end + 1;
      }
      if (start < foodMenu.length()) {
        display.setCursor(220, y);
        display.print(foodMenu.substring(start));
      }
    } else {
      display.setCursor(220, 130);
      display.print("No food menu found");
    }

  } while (display.nextPage());
}