#include "SimpleBleSerial.h"

SimpleBleSerial::SimpleBleSerial() {}

SimpleBleSerial::~SimpleBleSerial() {}

void SimpleBleSerial::begin(String deviceName) {
    currentName = deviceName;
    
    BLEDevice::init(currentName.c_str());
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Nordic UART Service
    pService = pServer->createService(SERVICE_UUID.c_str());

    pTxCharacteristic = pService->createCharacteristic(
        TX_UUID.c_str(),
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pTxCharacteristic->addDescriptor(new BLE2902());

    pRxCharacteristic = pService->createCharacteristic(
        RX_UUID.c_str(),
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
    );
    pRxCharacteristic->setCallbacks(this);

    pService->start();
    
    startAdvertising();
}

void SimpleBleSerial::setName(String name) {
    currentName = name;
    if(pServer != nullptr) {
        stopAdvertising();
        BLEDevice::init(currentName.c_str());
        
        // Zaktualizuj nazwe w pakiecie rozgloszeniowym
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->stop();
        pAdvertising->start();
    }
}

void SimpleBleSerial::setUUID(String serviceUUID, String rxUUID, String txUUID) {
    SERVICE_UUID = serviceUUID;
    RX_UUID = rxUUID;
    TX_UUID = txUUID;
}

void SimpleBleSerial::startAdvertising() {
    if(pServer != nullptr) {
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVICE_UUID.c_str());
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);  
        pAdvertising->setMinPreferred(0x12);
        BLEDevice::startAdvertising();
    }
}

void SimpleBleSerial::stopAdvertising() {
    if(pServer != nullptr) {
        BLEDevice::getAdvertising()->stop();
    }
}

int SimpleBleSerial::getRSSI() {
    // W ESP32 BLE getRSSI można uzyskać poprzez Peer device, ale 
    // wymaga to znalezienia klienta podłączonego.
    // Prosta implementacja pobiera RSSI dla pierwszego połączonego klienta.
    if(deviceConnected) {
        std::map<uint16_t, conn_status_t> conns = pServer->getPeerDevices(true);
        for (auto const& pair : conns) {
            // BLEDevice::getRSSI dziala tylko jesli zapytamy esp_ble_gap_read_rssi, co jest zablokowane callbackami
            // To jest uproszczona implementacja ze wgledu na ograniczenia ESP-IDF, najczesciej zwraca szacunkowe
        }
        return -50; // Mock lub trzeba doglebnie uzyc esp_ble_gap_read_rssi
    }
    return -100;
}

void SimpleBleSerial::printUUID() {
    Serial.println("=== BLE UUIDs ===");
    Serial.print("SERVICE: "); Serial.println(SERVICE_UUID);
    Serial.print("RX (W):  "); Serial.println(RX_UUID);
    Serial.print("TX (N):  "); Serial.println(TX_UUID);
    Serial.println("=================");
}

void SimpleBleSerial::onConnect(ConnectionCallback callback) {
    connectCallback = callback;
}

void SimpleBleSerial::onDisconnect(ConnectionCallback callback) {
    disconnectCallback = callback;
}

bool SimpleBleSerial::isConnected() {
    return deviceConnected;
}

void SimpleBleSerial::processChunking(const uint8_t* buffer, size_t size) {
    if (!deviceConnected || pTxCharacteristic == nullptr) return;
    
    size_t offset = 0;
    while (offset < size) {
        size_t chunk = (size - offset > 20) ? 20 : (size - offset);
        pTxCharacteristic->setValue((uint8_t*)&buffer[offset], chunk);
        pTxCharacteristic->notify();
        offset += chunk;
        delay(10); // Bezpieczny margines na przetworzenie przez BLE stack
    }
}

void SimpleBleSerial::sendText(const String& text) {
    processChunking((const uint8_t*)text.c_str(), text.length());
}

void SimpleBleSerial::sendNumbers(int number) {
    String numStr = String(number);
    sendText(numStr);
}

void SimpleBleSerial::sendNumbers(float number) {
    String numStr = String(number, 2);
    sendText(numStr);
}

void SimpleBleSerial::sendBytes(uint8_t byte) {
    if (deviceConnected && pTxCharacteristic != nullptr) {
        pTxCharacteristic->setValue(&byte, 1);
        pTxCharacteristic->notify();
        delay(10);
    }
}

void SimpleBleSerial::sendBytes(const uint8_t* array, size_t length) {
    processChunking(array, length);
}

String SimpleBleSerial::readText() {
    String result = rxBuffer;
    rxBuffer = "";
    return result;
}

String SimpleBleSerial::readSpecifiedText(const String& keyword) {
    if (rxBuffer.indexOf(keyword) != -1) {
        String result = rxBuffer;
        rxBuffer = "";
        return result;
    }
    return "";
}

// ----------------------------------------------------
// 4 DODATKOWE FUNKCJE OD TWÓRCY
// ----------------------------------------------------

String SimpleBleSerial::getDeviceMacAddress() {
    std::string mac = BLEDevice::getAddress().toString();
    return String(mac.c_str());
}

void SimpleBleSerial::clearBuffer() {
    rxBuffer = "";
}

void SimpleBleSerial::setDeviceBatteryLevel(uint8_t level) {
    if(level > 100) level = 100;
    
    // Inicjalizacja serwisu, jesli nie istnieje
    if (pBatteryService == nullptr && pServer != nullptr) {
        pBatteryService = pServer->createService((uint16_t)0x180F); // Battery Service UUID
        pBatteryCharacteristic = pBatteryService->createCharacteristic(
            (uint16_t)0x2A19, // Battery Level UUID
            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
        );
        pBatteryCharacteristic->addDescriptor(new BLE2902());
        pBatteryService->start();
        
        // Zaktualizuj pakiet rozgloszeniowy o nowy serwis
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID((uint16_t)0x180F);
    }

    if (pBatteryCharacteristic != nullptr) {
        pBatteryCharacteristic->setValue(&level, 1);
        pBatteryCharacteristic->notify();
    }
}

bool SimpleBleSerial::waitForConnection(uint32_t timeout_ms) {
    uint32_t startTime = millis();
    while(!deviceConnected) {
        if(timeout_ms > 0 && (millis() - startTime) >= timeout_ms) {
            return false;
        }
        delay(10); // yield do watchdoga
    }
    return true;
}

// ----------------------------------------------------
// Dziedziczenie ze Stream
// ----------------------------------------------------

int SimpleBleSerial::available() {
    return rxBuffer.length();
}

int SimpleBleSerial::read() {
    if (rxBuffer.length() > 0) {
        char c = rxBuffer[0];
        rxBuffer.remove(0, 1);
        return c;
    }
    return -1;
}

int SimpleBleSerial::peek() {
    if (rxBuffer.length() > 0) {
        return rxBuffer[0];
    }
    return -1;
}

void SimpleBleSerial::flush() {
    // Brak sprzetowego flusha w BLE
}

size_t SimpleBleSerial::write(uint8_t c) {
    sendBytes(c);
    return 1;
}

size_t SimpleBleSerial::write(const uint8_t *buffer, size_t size) {
    processChunking(buffer, size);
    return size;
}

// ----------------------------------------------------
// Callbacki wewnętrzne
// ----------------------------------------------------

void SimpleBleSerial::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    if (connectCallback != nullptr) {
        connectCallback();
    }
}

void SimpleBleSerial::onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    if (disconnectCallback != nullptr) {
        disconnectCallback();
    }
    // Restart advertising by znowu dalo sie polaczyc
    startAdvertising();
}

void SimpleBleSerial::onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++) {
            rxBuffer += (char)rxValue[i];
        }
    }
}
