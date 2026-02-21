# 🍻 HappyHourMenu

## 📝 Description
**HappyHourMenu** is a connected E-ink display project designed for local bars (or home bars!) to display daily specials, happy hour menus, or fun messages. 

Unlike traditional screens, the **E-ink display** mimics the look of paper, is readable in direct sunlight, and consumes almost no power when static. The menu is updated wirelessly via a user-friendly web interface hosted directly on the microcontroller—no app or external internet connection required!

## ✨ Features
- **Wireless Updates:** Change the menu instantly from any smartphone or computer connected to the network.
- **Web Interface:** Custom HTML/CSS/JS dashboard hosted on the ESP32/ESP8266.
- **E-Ink Display:** Crisp, paper-like readability and ultra-low power consumption.
- **Persistent Storage:** Menu items are saved to the device's flash memory (using LittleFS), so they aren't lost if power is cut.
- **Dual Support:** Codebases available for both **ESP32** and **ESP8266** platforms.

## 🛠️ Hardware Required
- **Microcontroller:** ESP32 or ESP8266 NodeMCU.
- **Display:** E-Ink / E-Paper Display module (supported by GxEPD/Adafruit libraries).
- **Power:** USB cable or LiPo battery.

## 🔧 Technologies Used
- **C++** (Arduino Framework)
- **JavaScript** (Frontend logic)
- **CSS** (Styling the web interface)
- **HTML** (Web interface structure)
- **LittleFS** (Filesystem for serving web files)

## 📦 Installation

### 1. Clone the Repository
```sh
git clone https://github.com/jelawless/HappyHourMenu.git
cd HappyHourMenu
```

### 2. Software Requirements
Before compiling, ensure you have the following installed in your Arduino IDE:
*   **ESP32 or ESP8266 Board Manager** installed.
*   **LittleFS Filesystem Uploader Plugin** (Crucial for uploading the HTML/CSS files).
*   **Required Libraries:**
    *   `ESPAsyncWebServer`
    *   `AsyncTCP` (for ESP32) or `ESPAsyncTCP` (for ESP8266)
    *   `GxEPD2` or `Adafruit_GFX` (depending on your display driver)

### 3. Uploading Filesystem (LittleFS)
**⚠️ Important:** This project uses **LittleFS** to store the web interface. You must upload the `data` folder to your board before the code will work.

1.  Open the project folder for your board (e.g., `HappyHourMenu/ESP32`).
2.  In Arduino IDE, select your board and port.
3.  Click **Tools** > **ESP32 LittleFS Data Upload** (or ESP8266 equivalent).
4.  Wait for the files to upload to the flash memory.

### 4. Flash the Firmware
1.  Open the `.ino` file in Arduino IDE.
2.  Update the `ssid` and `password` variables with your Wi-Fi credentials.
3.  Click **Upload** to flash the code to your microcontroller.

## 🚀 Usage
1.  Power on the device.
2.  Connect your phone or laptop to the same Wi-Fi network.
3.  Open a browser and navigate to the IP address printed in the Serial Monitor (e.g., `http://192.168.1.123`).
4.  Type in your new menu items and hit **Update**.
5.  Watch the E-ink screen refresh with your new Happy Hour specials!

## ❤️ Support
Thank you for checking out HappyHourMenu! If you find it useful for your bar or next party, consider giving it a ⭐ star on GitHub!

