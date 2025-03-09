#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include <BLE2902.h>
#include "driver/ledc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LED_PIN 2  // Choose your GPIO pin
#define LEDC_CHANNEL_0 0  // Choose 0-15
#define LEDC_TIMER_8_BIT 8  // Choose 1-14 bits resolution
#define LEDC_BASE_FREQ 5000  // 5000 Hz frequency

// UUIDs for BLE service and characteristics
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Receiving data
#define CHARACTERISTIC_UUID_TX "beb5483e-36e1-4688-b7f5-ea07361b26a9" // Sending data

BLEServer *pServer = nullptr;
BLECharacteristic *rxCharacteristic;
BLECharacteristic *txCharacteristic;
bool deviceConnected = false;

// The provided values MUST be correct [intensity >= 0 and <= 255]
void start_animation(int pin, int blinks) {
    while (blinks > 0) {
        digitalWrite(pin, HIGH);
        
        delay(2000);
        
        digitalWrite(pin, LOW);
        
        delay(2000);
        
        blinks--;
    }
}


int execute_bluetooth_command(char command[]) {
    char command_ex[4];
    char value[4];
    
    strncpy(command_ex, command, 3);
    command_ex[3] = '\0';
    strncpy(value, command + 4, 3);
    value[3] = '\0';
    
    if (strcmp(command_ex, "ra1") == 0) {
        int intensity = atoi(value);
        if (intensity < 0 || intensity > 255) {
            return 0;
        }
        
        start_animation(LED_PIN, intensity);
        return 1;
    } else {
        return 0;
    }
}

// Callback to handle connection/disconnection events
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("Device connected!");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("Device disconnected. Restarting advertising...");
        delay(5000);  // Prevent GATT 133 by delaying restart
        BLEDevice::startAdvertising();
    }
};

// Callback to handle received BLE data
class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        int length = pCharacteristic->getValue().length(); // Get message length
        if (length == 0) return;  // Exit if empty

        const uint8_t* data = pCharacteristic->getData();  // Get raw bytes

        // Create a char array with space for null terminator
        char char_array[length + 1];

        // Copy the data
        for(int i = 0; i < length; i++) {
            char_array[i] = (char)data[i];
        }

        // Add null terminator
        char_array[length] = '\0';

        if (strlen(char_array) != 9) {
          txCharacteristic->setValue("Incorrect command format, please use: <cmd> <integer> (the integer needs to be padded, like 006 for 6)");
          txCharacteristic->notify();
          return;
        }

        int result = execute_bluetooth_command(char_array);

        if (result == 0) {
          txCharacteristic->setValue("Incorrect command, please use one of the available ones.");
          txCharacteristic->notify();
          return;
        } else {
          txCharacteristic->setValue("Command Executed.");
          txCharacteristic->notify();
          return;
        }
    }
};

void setup() {
    Serial.begin(115200);

    // Configure LED PWM functionalitites
    //ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT);
  
    // Attach the channel to the GPIO to be controlled
    //ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize BLE
    BLEDevice::init("ESP32_BLE_LED");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create RX (write) characteristic
    rxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE
    );
    rxCharacteristic->setCallbacks(new MyCallbacks());

    // Create TX (notify) characteristic
    txCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    txCharacteristic->addDescriptor(new BLE2902());

    // Start service and advertising
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // Helps with iPhone connections
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("BLE Server is ready and advertising...");
}

void loop() {
    delay(1000);  // Allow BLE events to process
}

/* Program to control LED (ON/OFF/DIMMING) from ESP32 using Serial Bluetooth
 * Now supports PWM dimming with commands "dimX" where X is 0-255
 */