#include <SimpleBleSerial.h>

SimpleBleSerial bleSerial;
unsigned long lastUpdate = 0;
uint8_t batterySim = 100;

void setup() {
  Serial.begin(115200);
  
  bleSerial.begin("Bateria_BLE");

  Serial.println("Pobieram MAC adres układu...");
  String mac = bleSerial.getDeviceMacAddress();
  Serial.println("Mój adres to: " + mac);

  Serial.println("Czekam na użytkownika, aby załadować interfejs...");
  
  // Wstrzymuje działanie setup(), aż ktoś podłączy telefon. 
  // Jeżeli w ciągu 10 sekund (10000ms) nikt się nie podłączy, program ruszy dalej.
  bool isConnected = bleSerial.waitForConnection(10000);
  
  if (isConnected) {
    Serial.println("Sukces! Ktoś się połączył. Ustawiam baterię startową.");
    bleSerial.setDeviceBatteryLevel(batterySim);
    bleSerial.sendText("Witaj! Twój MAC to: " + mac + "\n");
  } else {
    Serial.println("Minął czas oczekiwania. Działam dalej bez podłączonego klienta.");
  }
}

void loop() {
  // Symulacja rozładowywania baterii co 5 sekund
  if (millis() - lastUpdate > 5000) {
    lastUpdate = millis();
    
    if (batterySim > 0) {
      batterySim--;
    } else {
      batterySim = 100; // ładuj od nowa
    }

    if (bleSerial.isConnected()) {
      Serial.print("Aktualizacja baterii przez BLE na: ");
      Serial.print(batterySim);
      Serial.println("%");
      
      // Aktualizuje standardową usługę baterii w BLE
      bleSerial.setDeviceBatteryLevel(batterySim);
    }
  }
}
