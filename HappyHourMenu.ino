#include <FS.h>
#include <LittleFS.h>
#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>  // ✅ Added for .local domain support

// --- Pin Definitions ---
#define CS_PIN   15
#define DC_PIN   4
#define RES_PIN  5
#define BUSY_PIN 16

// --- WiFi Credentials ---
const char* ssid = "T&TGuest";
const char* password = "T&TGuest";

ESP8266WebServer server(80);

// --- Display Object ---
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
  GxEPD2_420_GDEY042T81(CS_PIN, DC_PIN, RES_PIN, BUSY_PIN)
);

String readFile(const char* path) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.printf("⚠️ Failed to open %s\n", path);
    return "";
  }
  String content = file.readString();
  file.close();
  content.trim();
  return content;
}

int centerText(const char *text, const GFXfont *font) {
  display.setFont(font);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return (400 - w) / 2;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n📶 Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 30) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi");
    Serial.print("🌐 IP Address: ");
    Serial.println(WiFi.localIP());

    // --- Start mDNS ---
    if (MDNS.begin("menu")) {            // use http://menu.local
      Serial.println("✅ mDNS responder started: http://menu.local");
    } else {
      Serial.println("⚠️ Error setting up mDNS responder!");
    }

    // --- Mount LittleFS and list files ---
    if (!LittleFS.begin()) {
      Serial.println("❌ LittleFS Mount Failed");
    } else {
      Serial.println("✅ LittleFS mounted");
      Dir dir = LittleFS.openDir("/");
      while (dir.next()) {
        Serial.print("  ");
        Serial.println(dir.fileName());
      }
      Serial.println("📁 End of list.\n");
    }

    // --- Web server routes ---
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

    // Endpoints to save posted text
    server.on("/saveBeer", HTTP_POST, []() { 
      File f = LittleFS.open("/beer.txt", "w");
      if (!f) { server.send(500, "text/plain", "failed to open beer.txt"); return; }
      f.print(server.arg("plain"));
      f.close();
      drawHappyHourMenu();            // refresh display immediately
      server.send(200, "text/plain", "Saved");
    });
    server.on("/saveFood", HTTP_POST, []() { 
      File f = LittleFS.open("/food.txt", "w");
      if (!f) { server.send(500, "text/plain", "failed to open food.txt"); return; }
      f.print(server.arg("plain"));
      f.close();
      drawHappyHourMenu();            // refresh display immediately
      server.send(200, "text/plain", "Saved");
    });

    server.begin();
    Serial.println("✅ Web server started");
    Serial.println("🔗 Open in browser: ");
    Serial.print("   http://");
    Serial.println(WiFi.localIP());
    Serial.println("or http://menu.local (if your system supports mDNS)");
  }
  else {
    Serial.println("\n❌ WiFi connection failed.");
    Serial.print("Status: ");
    Serial.println(WiFi.status());
    // Still attempt to mount FS so you can debug locally via Serial
    if (!LittleFS.begin()) {
      Serial.println("❌ LittleFS Mount Failed");
    } else {
      Serial.println("✅ LittleFS mounted (no WiFi)");
      Dir dir = LittleFS.openDir("/");
      while (dir.next()) {
        Serial.print("  ");
        Serial.println(dir.fileName());
      }
      Serial.println("📁 End of list.\n");
    }
  }

  // Initialize display at the end so it doesn't fight Serial during WiFi prints
  display.init(115200);
  display.setRotation(2);
  display.setFullWindow();

  // Draw the menu once at boot
  drawHappyHourMenu();
}

void loop() {
  server.handleClient(); // ✅ Required for webpage response
  MDNS.update();
}

// --- Display Function ---
void drawHappyHourMenu() {
  Serial.println("🔍 Debugging file reads...");

  File beer = LittleFS.open("/beer.txt", "r");
  if (!beer) {
    Serial.println("⚠️ Could not open /beer.txt");
  } else {
    Serial.println("✅ Opened /beer.txt");
    Serial.println("Contents:");
    while (beer.available()) {
      String line = beer.readStringUntil('\n');
      line.trim();
      Serial.println(line);
    }
    beer.close();
  }

  File food = LittleFS.open("/food.txt", "r");
  if (!food) {
    Serial.println("⚠️ Could not open /food.txt");
  } else {
    Serial.println("✅ Opened /food.txt");
    Serial.println("Contents:");
    while (food.available()) {
      String line = food.readStringUntil('\n');
      line.trim();
      Serial.println(line);
    }
    food.close();
  }

  Serial.println("🔍 End of debug output.\n");

  String beerMenu = readFile("/beer.txt");
  String foodMenu = readFile("/food.txt");

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    // --- Header ---
    const char *header = "TABLES & TAPS";
    int x_header = centerText(header, &FreeSansBold18pt7b);
    display.setFont(&FreeSansBold18pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(x_header, 40);
    display.print(header);

    display.drawLine(20, 50, 380, 50, GxEPD_BLACK);

    // --- Happy Hour ---
    const char *timeText = "Happy Hour: 3 PM - 6 PM";
    const char *daysText = "Mon - Fri Only";
    int x_time = centerText(timeText, &FreeMonoBold9pt7b);
    int x_days = centerText(daysText, &FreeMonoBold9pt7b);

    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(x_time, 75);
    display.print(timeText);

    display.setCursor(x_days, 95);
    display.print(daysText);

    // --- Beers ---
    display.drawRoundRect(20, 110, 170, 160, 6, GxEPD_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(45, 130);
    display.print("Draft Beers - $4");

    display.setFont();
    if (beerMenu.length() > 0) {
      int y = 155;
      int lineHeight = 20;
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
      display.setCursor(30, 155);
      display.print("No beer menu found");
    }

    // --- Food ---
    display.drawRoundRect(210, 110, 170, 160, 6, GxEPD_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(240, 130);
    display.print("Food Specials");

    display.setFont();
    if (foodMenu.length() > 0) {
      int y = 155;
      int lineHeight = 20;
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
      display.setCursor(220, 155);
      display.print("No food menu found");
    }

    display.drawLine(20, 275, 380, 275, GxEPD_BLACK);

    const char *footer = "CHEERS!";
    int x_footer = centerText(footer, &FreeMonoBold9pt7b);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(x_footer, 295);
    display.print(footer);

  } while (display.nextPage());
}
