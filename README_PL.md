# SimpleBleSerial

Prosta w użyciu biblioteka do komunikacji Bluetooth Low Energy (BLE) dla platformy ESP32, wykorzystująca uniwersalny profil Nordic UART Service (NUS).

Biblioteka ta została stworzona z myślą o środowisku Arduino IDE, aby maksymalnie uprościć komunikację z aplikacjami typu BLE Terminal (na systemach Android i iOS). Zachowuje się dokładnie tak samo, jak klasyczny port szeregowy po kablu USB. Dziedziczy po wbudowanej klasie `Stream`, dzięki czemu możesz używać dobrze znanych metod takich jak `available()`, `read()` czy `print()`.

[![GitHub](https://img.shields.io/badge/GitHub-OneDevelopmentPL-blue)](https://github.com/OneDevelopmentPL/SimpleBleSerial)

*English documentation is available in [README.md](README.md).*

## Główne funkcje

- **Zgodność ze Stream:** Używaj `print()`, `println()`, `read()`, `available()` dokładnie tak, jak w standardowym `Serial`.
- **Nordic UART Service (NUS):** Działa od razu (Out-Of-The-Box) ze wszystkimi standardowymi aplikacjami BLE na telefon.
- **Szybki start:** Wywołanie `.begin()` automatycznie konfiguruje i uruchamia urządzenie (domyślnie pod nazwą "MyBLEDevice").
- **Dynamiczna konfiguracja:** Obsługuje zmianę nazwy urządzenia (`setName`) i identyfikatorów UUID (`setUUID`) w trakcie pracy programu.
- **Kontrola rozgłaszania:** Włączaj i wyłączaj widoczność układu dla innych telefonów za pomocą `startAdvertising()` / `stopAdvertising()`.
- **Automatyczne pakietowanie (Chunking):** Biblioteka sama dzieli długie wiadomości tekstowe na bezpieczne, 20-bajtowe paczki zgodne z limitami technologii BLE.
- **Zdarzenia połączeń:** Rejestruj własne funkcje zwrotne za pomocą `onConnect()` i `onDisconnect()`.
- **4 autorskie funkcje dodatkowe:** 
  - `waitForConnection()` - wstrzymuje program dopóki telefon się nie połączy (opcjonalny limit czasu).
  - `getDeviceMacAddress()` - zwraca fizyczny adres MAC układu ESP32.
  - `setDeviceBatteryLevel(uint8_t level)` - ustawia poziom baterii (0-100%), który telefony będą w stanie pokazać natywnie na pasku stanu.
  - `clearBuffer()` - ręczne czyszczenie bufora odbiorczego.

## Instalacja

1. Pobierz to repozytorium jako plik `.zip`.
2. Otwórz Arduino IDE.
3. Przejdź do **Szkic (Sketch)** -> **Dołącz bibliotekę (Include Library)** -> **Dodaj bibliotekę .ZIP (Add .ZIP Library...)**.
4. Wybierz pobrany plik `.zip`.
5. Dołącz bibliotekę w swoim kodzie: `#include <SimpleBleSerial.h>`.

## Podstawowe użycie

```cpp
#include <SimpleBleSerial.h>

SimpleBleSerial bleSerial;

void setup() {
  Serial.begin(115200);
  
  bleSerial.begin("MojESP32_BLE");

  bleSerial.onConnect([]() {
    Serial.println("Polaczono z telefonem!");
  });

  bleSerial.onDisconnect([]() {
    Serial.println("Rozlaczono.");
  });
}

void loop() {
  if (bleSerial.available()) {
    String msg = bleSerial.readText(); // Autorska metoda wczytania wszystkiego na raz
    Serial.println("Odebrano: " + msg);
  }
}
```

Zajrzyj do folderu [examples/](examples/) po więcej rozbudowanych przykładów kodu.

## Licencja
MIT License. Autorem jest [OneDevelopmentPL](https://github.com/OneDevelopmentPL).
