#include <SimpleBleSerial.h>

SimpleBleSerial bleSerial;

void setup() {
  Serial.begin(115200);
  
  bleSerial.begin("Filtr_BLE");
  Serial.println("Uruchomiono moduł. Oczekuję na komendy...");
}

void loop() {
  if (bleSerial.available()) {
    // Odczytujemy specjalnie wybraną komendę.
    // readSpecifiedText zwróci wartość TYLKO wtedy, gdy tekst zawiera słowo kluczowe "LED".
    // Jeżeli ktoś wyśle np. "MOTOR ON", funkcja zwróci pusty ciąg.
    String msg = bleSerial.readSpecifiedText("LED");
    
    if (msg.length() > 0) {
      Serial.print("Otrzymano prawidłową komendę sterującą: ");
      Serial.println(msg);

      if (msg.indexOf("ON") != -1) {
        bleSerial.sendText("Włączam diodę!\n");
        // Tu logika włączania diody LED
      } 
      else if (msg.indexOf("OFF") != -1) {
        bleSerial.sendText("Wyłączam diodę!\n");
        // Tu logika wyłączania diody LED
      }
    } else {
      // Wyczyść bufor, jeśli przyszło coś, co nas nie interesuje (żeby się nie zapchał)
      bleSerial.clearBuffer();
    }
  }

  delay(20);
}
