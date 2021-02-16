/*
[v1.2]
- Send max number of BLE payload size : 20 bytes
- Send data separated by commas

[v1.1]
- Update txValue range : 0 to 100,000
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *pCharacteristic_data;
bool deviceConnected = false;
bool characteristicConnected = false;
int txValue = 0;

String MOI = "";
String PH = "";
String TEMP = "";
String PHOTO = "";
String BAT = "";

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TX "4a154d60-4767-469d-a86e-5a3fc5bbc6ac"


class MyServerCallbacks: public BLEServerCallbacks{
    void onConnect(BLEServer* pServer){
    deviceConnected = true;
    Serial.println("MyServerCallbacks -> BLEServerCallbacks -> onConnect");
  };

  void onDisconnect(BLEServer* pServer){
    deviceConnected = false;
    Serial.println("MyServerCallbacks -> BLEServerCallbacks -> onDisconnect");
  };
};


class MyCharacteristicCallbacks: public BLECharacteristicCallbacks{
    void onRead(BLECharacteristic *pCharacteristic){
    characteristicConnected = true;
    Serial.println("MyServerCallbacks -> BLECharacteristicCallbacks -> onRead");
  };

  void onWrite(BLECharacteristic *pCharacteristic){
    characteristicConnected = false;
    Serial.println("MyServerCallbacks -> BLECharacteristicCallbacks -> onWrite");
  };
};


void setup() {

  Serial.begin(9600);

  // Create BLE Device
  BLEDevice::init("ESP32");

  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic_data = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );  
  pCharacteristic_data->setCallbacks(new MyCharacteristicCallbacks());
 
  // BLE2902 needed to notify
  pCharacteristic_data->addDescriptor(new BLE2902());

  // Start service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client connection to notify ... ");
}

void loop() {
  if (deviceConnected && characteristicConnected){
    /* MAX BLE PAYLOAD SIZE IS 20 BYTES!!!
     * Payload format (total 20 bytes)
     * [Moisture 3 bytes] + , + 
     * [pH 3 bytes] + , + 
     * [Temp 3 bytes] + ,
     * [Photo 3 bytes] + ,
     * [Battery 3 bytes]
     */
    MOI   = String( random(100, 999) );
    PH    = String( random(100, 999) );
    TEMP  = String( random(100, 999) );
    PHOTO = String( random(100, 999) );
    BAT   = String( random(100, 999) );
    String payload = MOI + "," + PH + "," + TEMP + "," + PHOTO + "," + BAT;

    char txString[20];
    payload.toCharArray(txString, 20);

    /* [v1.1]
    txValue = random(0,100000);

    // Conversion of txValue
    char txString[8];
    dtostrf(txValue, 1, 2, txString); 
        // txValue: float
        // 1: length of str
        // 2: num digits after decimal
        // txString: array to store results
    */

    // Setting value to characteristics
    pCharacteristic_data->setValue(txString);

    // Notifying connected client
    pCharacteristic_data->notify();
    Serial.println("Sent value : " + String(txString));
    delay(5000);
  }
}
