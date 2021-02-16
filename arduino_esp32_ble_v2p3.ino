
/* REFERENCES
 * BLEDevice.h : https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEDevice.h
 * BLEServer.h : https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEServer.h
 * BLE Class Reference : http://www.neilkolban.com/esp32/docs/cpp_utils/html/annotated.html
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE HIERARCHY : PROFILE -> SERVICE(S) -> CHARACTERISTIC(S)
#define PROFILE_DEV_NAME    "xyz:Homerun.ai"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACT_UUID_DATA   "4a154d60-4767-469d-a86e-5a3fc5bbc6ac"
#define CHARACT_UUID_DEVICE "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACT_UUID_SSID   "165fa2cf-f22e-4e81-b74b-2a22d3753977"
#define CHARACT_UUID_PASS   "3020f983-7faf-4207-bd40-a1bc8a620d7e"
BLECharacteristic *pCharact_data;   // Declare here -> used in loop() to assign values
BLECharacteristic *pCharact_device; // Declare here -> used in loop() to assign values
BLECharacteristic *pCharact_ssid;   // Declare here -> used in loop() to assign values
BLECharacteristic *pCharact_pass;   // Declare here -> used in loop() to assign values
  // Generating UUIDs: https://www.uuidgenerator.net/

// CONNECTION BOOLS
bool deviceConnected = false;
bool charactConnected = false;

// BLE DATA PLACEHOLDERS
String MOI = "";
String PH = "";
String TEMP = "";
String PHOTO = "";
String BAT = "";



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// CALLBACK (SERVER)
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class MyServerCallbacks: public BLEServerCallbacks{
    void onConnect(BLEServer* pServer){
    deviceConnected = true;
    Serial.println("MyServerCallbacks -> BLEServerCallbacks -> onConnect");
    Serial.println("Connected to SERVER");
    Serial.println("");
  };

  void onDisconnect(BLEServer* pServer){
    deviceConnected = false;
    Serial.println("MyServerCallbacks -> BLEServerCallbacks -> onDisconnect");
    Serial.println("Disconnected from SERVER");
    Serial.println("");
  };
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// CALLBACK (CHARACTERISTIC)
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class MyCharactCallbacks: public BLECharacteristicCallbacks{
    void onRead(BLECharacteristic *pCharacteristic){
    charactConnected = true;
    Serial.println("MyServerCallbacks -> BLECharacteristicCallbacks -> onRead");
    Serial.println("Connected to CHARACTERISTIC, with UUID");
    
    BLEUUID charaUUID = pCharacteristic->getUUID();
    std::string strUUID = charaUUID.toString();
    String mystrUUID = strUUID.c_str();
    Serial.println(mystrUUID);

    if (mystrUUID.equals(CHARACT_UUID_DATA)) {
      Serial.println("This is DATA CHARACTERISTIC");
      Serial.println("");
    } else if (mystrUUID.equals(CHARACT_UUID_DEVICE)) {
      Serial.println("This is DEVICE CHARACTERISTIC");
      Serial.println("");
    } else if (mystrUUID.equals(CHARACT_UUID_SSID)) {
      Serial.println("This is SSID CHARACTERISTIC");
      Serial.println("");
    } else if (mystrUUID.equals(CHARACT_UUID_PASS)) {
      Serial.println("This is PASS CHARACTERISTIC");
      Serial.println("");
    } else {
      Serial.println("This is UNKNOWN CHARACTERISTIC");
      Serial.println("");
    }
  };

  void onWrite(BLECharacteristic *pCharacteristic){
    charactConnected = false;
    Serial.println("MyServerCallbacks -> BLECharacteristicCallbacks -> onWrite");
    Serial.println("Disconnected from CHARACTERISTIC");
  };
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SETUP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() {

  Serial.begin(9600);

  // Create BLE Device
  BLEDevice::init( PROFILE_DEV_NAME );

  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // Attach Server callback

  // Create BLE Service
  BLEService *pService = pServer->createService( SERVICE_UUID );

  // Create BLE Characteristic(s)
  // Charact [1]
  pCharact_data = pService->createCharacteristic(
                    CHARACT_UUID_DATA,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pCharact_data->setCallbacks(new MyCharactCallbacks()); // Attach Characteristic callback

  // Charact [2]
  pCharact_device = pService->createCharacteristic(
                    CHARACT_UUID_DEVICE,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pCharact_device->setCallbacks(new MyCharactCallbacks()); // Attach Characteristic callback
  
  // Charact [3]
  pCharact_ssid = pService->createCharacteristic(
                    CHARACT_UUID_SSID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pCharact_ssid->setCallbacks(new MyCharactCallbacks()); // Attach Characteristic callback

  // Charact [4]
  pCharact_pass = pService->createCharacteristic(
                    CHARACT_UUID_PASS,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pCharact_pass->setCallbacks(new MyCharactCallbacks()); // Attach Characteristic callback

  /* BLECharacteristic *pCharact_data = pService->createCharacteristic(
                    CHARACT_UUID_DATA,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  ); */
      
  // BLE2902 needed to Notify
  pCharact_data->addDescriptor(new BLE2902());
  pCharact_device->addDescriptor(new BLE2902());
  pCharact_ssid->addDescriptor(new BLE2902());
  pCharact_pass->addDescriptor(new BLE2902());
    /* 
     * I think this descriptor library is used by react native ...
     * Without it, I can't initiate startNotification() from react native 
     */

  // Start BLE Service
  pService->start();

  // Start BLE Advertising - Method 1 (all work)
  pServer->getAdvertising()->start();

  // Start BLE Advertising - Method 2 (all work)
  /* BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start(); */

  // Start BLE Advertising - Method 3 (all work)
  /* BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising(); */
  

  delay(3000);
  Serial.println("Waiting for a client to connect ... ");
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {
  if (deviceConnected && charactConnected){
    
    /* MAX BLE PAYLOAD SIZE IS 20 BYTES!!!
     * Payload format (total 20 bytes) = 
     * [Moisture 3 bytes] + , + [pH 3 bytes] + , + [Temp 3 bytes] + , +
     * [Photo 3 bytes] + , + [Battery 3 bytes]
     */
    MOI   = String( random(1, 999) );
    PH    = String( random(1, 999) );
    TEMP  = String( random(1, 999) );
    PHOTO = String( random(1, 999) );
    BAT   = String( random(1, 999) );
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
    pCharact_data->setValue(txString);

    // Notifying connected client
    pCharact_data->notify();
    Serial.println("Sent value : " + String(txString));
    delay(5000);
  }
}
