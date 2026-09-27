# SimpleBleSerial

A simple, easy-to-use Bluetooth Low Energy (BLE) Serial wrapper for the ESP32 platform, utilizing the standard Nordic UART Service (NUS).

This library is designed for the Arduino IDE to make communicating with BLE terminal apps (like those on Android or iOS) as simple as using classic `Serial` or `SoftwareSerial`. It inherits from the standard Arduino `Stream` class, so you can seamlessly use familiar methods such as `available()`, `read()`, and `print()`.

[![GitHub](https://img.shields.io/badge/GitHub-OneDevelopmentPL-blue)](https://github.com/OneDevelopmentPL/SimpleBleSerial)

*For Polish documentation, check [README_PL.md](README_PL.md).*

## Features

- **Stream Compatible:** Use `print()`, `println()`, `read()`, `available()` exactly like you do with classic Serial.
- **Nordic UART Service (NUS):** Works out of the box with standard BLE terminal apps.
- **Easy Setup:** Calling `.begin()` automatically configures the device name (default: "MyBLEDevice").
- **Dynamic Configuration:** Supports changing device name (`setName`) and UUIDs (`setUUID`) on the fly.
- **Advertising Control:** Programmatically `startAdvertising()` or `stopAdvertising()` to manage visibility and security.
- **Automatic Chunking:** Seamlessly splits large messages into safe 20-byte chunks to comply with BLE standard limits.
- **Event Handling:** Register custom callbacks via `onConnect()` and `onDisconnect()`.
- **Advanced Features:** 
  - `waitForConnection()` - block execution until a client connects.
  - `getDeviceMacAddress()` - retrieve the physical MAC address of your ESP32.
  - `setDeviceBatteryLevel(uint8_t level)` - dynamically set the battery level (0-100%) visible natively on most mobile devices.
  - `clearBuffer()` - clear the RX buffer manually if needed.

## Installation

1. Download this repository as a `.zip` file.
2. Open Arduino IDE.
3. Go to **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Select the downloaded `.zip` file.
5. Include the library in your code: `#include <SimpleBleSerial.h>`.

## Basic Usage

```cpp
#include <SimpleBleSerial.h>

SimpleBleSerial bleSerial;

void setup() {
  Serial.begin(115200);
  
  bleSerial.begin("MyESP32_BLE");

  bleSerial.onConnect([]() {
    Serial.println("Connected to phone!");
  });

  bleSerial.onDisconnect([]() {
    Serial.println("Disconnected.");
  });
}

void loop() {
  if (bleSerial.available()) {
    String msg = bleSerial.readText();
    Serial.println("Received: " + msg);
  }
}
```

Check out the [examples/](examples/) directory for more comprehensive use cases.

## License
MIT License. Created by [OneDevelopmentPL](https://github.com/OneDevelopmentPL).
