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

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;

String MOI = "";
String PH = "";
String TEMP = "";
String PHOTO = "";
String BAT = "";

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks{
    void onConnect(BLEServer* pServer){
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer){
    deviceConnected = false;
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
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
    );
 
  // BLE2902 needed to notify
  pCharacteristic->addDescriptor(new BLE2902());

  // Start service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client connection to notify ... ");
}

void loop() {
  if (deviceConnected){
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
    pCharacteristic->setValue(txString);

    // Notifying connected client
    pCharacteristic->notify();
    Serial.println("Sent value : " + String(txString));
    delay(3000);
  }
}
