#ifndef SIMPLE_BLE_SERIAL_H
#define SIMPLE_BLE_SERIAL_H

#include <Arduino.h>
#include <Stream.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

typedef void (*ConnectionCallback)(void);

class SimpleBleSerial : public Stream, public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    SimpleBleSerial();
    ~SimpleBleSerial();

    // 1. Podstawowe funkcje (wg specyfikacji)
    void begin(String deviceName = "MyBLEDevice");
    void setName(String name);
    void setUUID(String serviceUUID, String rxUUID, String txUUID);
    
    // Kontrola nadajnika
    void startAdvertising();
    void stopAdvertising();
    
    // Narzedzia
    int getRSSI();
    void printUUID();
    
    // Obsługa zdarzeń
    void onConnect(ConnectionCallback callback);
    void onDisconnect(ConnectionCallback callback);
    
    // Autorski Standard Przetwarzania Danych (BleSerial)
    bool isConnected();
    void sendText(const String& text);
    void sendNumbers(int number);
    void sendNumbers(float number);
    void sendBytes(uint8_t byte);
    void sendBytes(const uint8_t* array, size_t length);
    String readText();
    String readSpecifiedText(const String& keyword);

    // ==========================================
    // 4 DODATKOWE FUNKCJE OD TWÓRCY (EXTRA FEATURES)
    // ==========================================
    
    // 1. Zwraca fizyczny adres MAC modułu Bluetooth (przydatne do filtracji i whitelistów)
    String getDeviceMacAddress();
    
    // 2. Czyści cały bufor odbiorczy (przydatne przy przepełnieniach lub resecie komunikacji)
    void clearBuffer();
    
    // 3. Dodaje standardowy serwis baterii BLE i ustawia jej poziom (widoczne natywnie na telefonach w pasku)
    void setDeviceBatteryLevel(uint8_t level); 
    
    // 4. Aktywnie blokuje program i czeka na połączenie z opcjonalnym limitem czasu (timeout) - idealne do setup()
    bool waitForConnection(uint32_t timeout_ms = 0);

    // ==========================================

    // Dziedziczenie ze Stream (natywna składnia Arduino)
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    // Callbacks wewnetrzne BLE (nie wywolywac recznie)
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
    void onWrite(BLECharacteristic *pCharacteristic) override;

private:
    BLEServer* pServer = nullptr;
    BLEService* pService = nullptr;
    BLECharacteristic* pTxCharacteristic = nullptr;
    BLECharacteristic* pRxCharacteristic = nullptr;
    
    // Battery Service (Dodatek)
    BLEService* pBatteryService = nullptr;
    BLECharacteristic* pBatteryCharacteristic = nullptr;

    String currentName;
    String SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"; // Nordic UART Service
    String RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; // NUS RX
    String TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"; // NUS TX

    bool deviceConnected = false;

    ConnectionCallback connectCallback = nullptr;
    ConnectionCallback disconnectCallback = nullptr;

    String rxBuffer = "";
    
    void processChunking(const uint8_t* buffer, size_t size);
};

#endif // SIMPLE_BLE_SERIAL_H
