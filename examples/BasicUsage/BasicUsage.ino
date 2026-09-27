#include <SimpleBleSerial.h>

SimpleBleSerial bleSerial;

void setup() {
  Serial.begin(115200);
  
  // Inicjalizacja biblioteki
  bleSerial.begin("MójESP32_BLE");

  // Rejestracja zdarzeń
  bleSerial.onConnect([]() {
    Serial.println("Połączono z telefonem!");
  });

  bleSerial.onDisconnect([]() {
    Serial.println("Rozłączono.");
  });

  Serial.println("Oczekuję na połączenie...");
  // Użycie 1. dodatkowej funkcji: waitForConnection
  // Blokuje działanie programu do momentu połączenia z terminalem BLE (lub do timeoutu, tu bez timeoutu)
  bleSerial.waitForConnection(); 
  
  // Użycie 2. dodatkowej funkcji: getDeviceMacAddress
  Serial.print("Adres MAC tego urządzenia to: ");
  Serial.println(bleSerial.getDeviceMacAddress());

  // Użycie 3. dodatkowej funkcji: setDeviceBatteryLevel
  // Pozwala telefonowi zobaczyć ikonę baterii (np. 85%)
  bleSerial.setDeviceBatteryLevel(85);

  // Wysłanie wiadomości powitalnej - autorska metoda
  bleSerial.sendText("Witaj z ESP32!\n");
}

void loop() {
  // Sprawdzamy czy przyszły jakieś dane
  if (bleSerial.available()) {
    // Autorska metoda odczytu całego tekstu na raz
    String msg = bleSerial.readText();
    Serial.print("Otrzymano po BLE: ");
    Serial.println(msg);

    // Użycie 4. dodatkowej funkcji: clearBuffer 
    // W ramach testu - jeśli wyślesz "CLEAR", czyścimy resztki, jeśli jakieś byłyby zablokowane
    if (msg.indexOf("CLEAR") != -1) {
      bleSerial.clearBuffer();
      bleSerial.sendText("Bufor wyczyszczony!\n");
    }
  }

  // Przekazywanie z portu USB na Bluetooth
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    bleSerial.sendText(s + "\n");
  }

  delay(20);
}
