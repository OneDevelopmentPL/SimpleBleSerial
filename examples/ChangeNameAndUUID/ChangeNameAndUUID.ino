#include <SimpleBleSerial.h>

SimpleBleSerial bleSerial;

void setup() {
  Serial.begin(115200);
  
  // Opcjonalnie: możemy zmienić domyślne UUID na własne przed startem.
  // Używaj tego, jeśli masz własną aplikację na telefon i chcesz uniknąć konfliktu.
  bleSerial.setUUID(
    "12345678-1234-5678-1234-56789abcdef0", // Service
    "12345678-1234-5678-1234-56789abcdef1", // RX
    "12345678-1234-5678-1234-56789abcdef2"  // TX
  );

  bleSerial.begin("MojeUrzadzenie_V1");
  
  // Wypisz nowe UUID w monitorze portu szeregowego
  bleSerial.printUUID();

  Serial.println("Wyślij 'ZMIEN' przez BLE, aby dynamicznie zmienić nazwę urządzenia na 'Zmienione_BLE'");
}

void loop() {
  if (bleSerial.available()) {
    String msg = bleSerial.readText();
    msg.trim(); // Usunięcie białych znaków (np. \n z terminala)
    
    if (msg == "ZMIEN") {
      Serial.println("Otrzymano komendę zmiany nazwy!");
      
      // Dynamicznie zmienia nazwę i restartuje sygnał rozgłoszeniowy (Advertising),
      // by telefony w pobliżu od razu to zauważyły. 
      // (Spowoduje to tymczasowe rozłączenie aktualnego użytkownika w większości telefonów).
      bleSerial.setName("Zmienione_BLE");
      
      Serial.println("Nazwa została zmieniona. Sprawdź wyszukiwanie Bluetooth na swoim telefonie.");
    }
  }

  delay(20);
}
