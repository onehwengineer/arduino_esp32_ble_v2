/*
    Version 1.1 (3/3/2021)
    - Added LED on
  
    Version 1.0
    BLE test purpose. Data is artifically generated. Not from sensor.
*/
/* REFERENCES
 * BLEDevice.h : https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEDevice.h
 * BLEServer.h : https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEServer.h
 * BLE Class Reference : http://www.neilkolban.com/esp32/docs/cpp_utils/html/annotated.html
*/


String firmware_v = "7.8";

// ESP32 PIN ASSIGNMENT
#define ESP32_MOIST 35          /* ADC2 cannot be used w/ Wifi */
#define ESP32_PH 32
#define ESP32_TEMP 33
#define ESP32_PHOTO 36
#define EPS32_BAT 39

#define ESP32_VGND 14
#define ESP32_TOUCH 4

#define ESP32_LED_R 5
#define ESP32_LED_G 18
#define ESP32_LED_B 19
#define ESP32_LED_Y 21

// BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE HIERARCHY : PROFILE -> SERVICE(S) -> CHARACTERISTIC(S)
#define PROFILE_DEV_NAME  "xyz:Homerun.ai"
#define SERVICE_UUID      "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARA_UUID_DEVICE "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARA_UUID_SSID   "165fa2cf-f22e-4e81-b74b-2a22d3753977"
#define CHARA_UUID_PASS   "3020f983-7faf-4207-bd40-a1bc8a620d7e"
#define CHARA_UUID_DATA   "4a154d60-4767-469d-a86e-5a3fc5bbc6ac"
BLECharacteristic *pChara_data;   // Declare here -> used in loop() to assign values
BLECharacteristic *pChara_device; // Declare here -> used in loop() to assign values
BLECharacteristic *pChara_ssid;   // Declare here -> used in loop() to assign values
BLECharacteristic *pChara_pass;   // Declare here -> used in loop() to assign values
  // Generating UUIDs: https://www.uuidgenerator.net/
  
// CONNECTION BOOLS
bool deviceConnected = false;
bool charactConnected = false;

// BLE DATA PLACEHOLDER
uint8_t mydata[10];
float VMOIST = 0;
float VPH = 0;
float TEMP_C = 0;
float VPhoto = 0;
float VBat = 0;

// ADC Calibration
#include <esp_adc_cal.h>



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// LED ON
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void LED_on() {
  digitalWrite(ESP32_LED_R, HIGH);
  delay(100);
  digitalWrite(ESP32_LED_G, HIGH);
  delay(100);
  digitalWrite(ESP32_LED_B, HIGH);
  delay(100);
  digitalWrite(ESP32_LED_Y, HIGH);
  delay(100);
  digitalWrite(ESP32_LED_R, LOW);
  delay(100);
  digitalWrite(ESP32_LED_G, LOW);
  delay(100);
  digitalWrite(ESP32_LED_B, LOW);
  delay(100);
  digitalWrite(ESP32_LED_Y, LOW);
}



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
    charactConnected = false; // When disconnected, also set this as false
    Serial.println("MyServerCallbacks -> BLEServerCallbacks -> onDisconnect");
    Serial.println("Disconnected from SERVER");
    Serial.println("");
  };
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// CALLBACK (CHARACTERISTIC)
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class MyCharaCallbacks: public BLECharacteristicCallbacks{
    
    void onRead(BLECharacteristic *pCharacteristic){
    charactConnected = true;
    Serial.println("MyServerCallbacks -> BLECharacteristicCallbacks -> onRead");
    Serial.println("Connected to CHARACTERISTIC, with UUID");
    
    BLEUUID charaUUID = pCharacteristic->getUUID();
    std::string strUUID = charaUUID.toString();
    String mystrUUID = strUUID.c_str();
    Serial.println(mystrUUID);

    if (mystrUUID.equals(CHARA_UUID_DATA)) {
      Serial.println("This is DATA CHARACTERISTIC");
      Serial.println("");
    } else if (mystrUUID.equals(CHARA_UUID_DEVICE)) {
      Serial.println("This is DEVICE CHARACTERISTIC");
      Serial.println("");
    } else if (mystrUUID.equals(CHARA_UUID_SSID)) {
      Serial.println("This is SSID CHARACTERISTIC");
      Serial.println("");
    } else if (mystrUUID.equals(CHARA_UUID_PASS)) {
      Serial.println("This is PASS CHARACTERISTIC");
      Serial.println("");
    } else {
      Serial.println("This is UNKNOWN CHARACTERISTIC");
      Serial.println("");
    }

    delay(10);
    LED_on();
  };

  void onWrite(BLECharacteristic *pCharacteristic){
    charactConnected = false;
    Serial.println("MyServerCallbacks -> BLECharacteristicCallbacks -> onWrite");
    Serial.println("Disconnected from CHARACTERISTIC");
  };
};



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// BLE START
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void BLEStart() {
  
  // Create BLE Device
  BLEDevice::init( PROFILE_DEV_NAME );  

  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // Attach Server callback

  // Create BLE Service
  BLEService *pService = pServer->createService( SERVICE_UUID ); 

  // Create BLE Characteristic(s)
  // Charact [1]
  pChara_data = pService->createCharacteristic(
                    CHARA_UUID_DATA,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pChara_data->setCallbacks(new MyCharaCallbacks()); // Attach Characteristic callback

  // Charact [2]
  pChara_device = pService->createCharacteristic(
                    CHARA_UUID_DEVICE,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pChara_device->setCallbacks(new MyCharaCallbacks()); // Attach Characteristic callback
  
  // Charact [3]
  pChara_ssid = pService->createCharacteristic(
                    CHARA_UUID_SSID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pChara_ssid->setCallbacks(new MyCharaCallbacks()); // Attach Characteristic callback

  // Charact [4]
  pChara_pass = pService->createCharacteristic(
                    CHARA_UUID_PASS,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
  );
  pChara_pass->setCallbacks(new MyCharaCallbacks()); // Attach Characteristic callback
  delay(10);

 // BLE2902 needed to Notify
  pChara_data->addDescriptor(new BLE2902());
  pChara_device->addDescriptor(new BLE2902());
  pChara_ssid->addDescriptor(new BLE2902());
  pChara_pass->addDescriptor(new BLE2902());
    /* 
     * I think this descriptor library is used by react native ...
     * Without it, I can't initiate startNotification() from react native 
     */ 
  delay(10);

  // Start BLE Service
  pService->start();
  delay(10);

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
// ANALOG READ
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Command to see the REF_VOLTAGE: espefuse.py --port /dev/ttyUSB0 adc_info
// or dc2_vref_to_gpio(25)
#define REF_VOLTAGE 1100
esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;

// ========= analogRead_cal =========
int analogRead_cal(uint8_t channel, adc_atten_t attenuation) {
  adc1_channel_t channelNum;

  switch (channel) {
    case (36):
      channelNum = ADC1_CHANNEL_0;
      break;
    case (39):
      channelNum = ADC1_CHANNEL_3;
      break;
    case (34):
      channelNum = ADC1_CHANNEL_6;
      break;
    case (35):
      channelNum = ADC1_CHANNEL_7;
      break;
    case (32):
      channelNum = ADC1_CHANNEL_4;
      break;
    case (33):
      channelNum = ADC1_CHANNEL_5;
      break;
  }
  adc1_config_channel_atten(channelNum, attenuation);
  return esp_adc_cal_raw_to_voltage(analogRead(channel), adc_chars);
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SENSOR READ
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void sensorRead() {
  digitalWrite(ESP32_VGND, HIGH);  // Enable sensor
  delay(100);   // For PH/MOIST/TEMP/PHOT Even 1ms is OK (0ms NG)
  
  analogRead_cal(36, ADC_ATTEN_DB_11);

  float VMOIST = (float) analogRead_cal(ESP32_MOIST, ADC_ATTEN_DB_11);
  float VPH = (float) analogRead_cal(ESP32_PH, ADC_ATTEN_DB_11);
  float VTEMP = (float) analogRead_cal(ESP32_TEMP, ADC_ATTEN_DB_11);
  float VPhoto = (float) analogRead_cal(ESP32_PHOTO, ADC_ATTEN_DB_11);
  float Bat = (float) analogRead_cal(EPS32_BAT, ADC_ATTEN_DB_11);
  VBat = Bat * 2;
  TEMP_C = (VTEMP - 400) / 19.5 * 10;

  //digitalWrite(ESP32_VGND, LOW);  // Disable snesor

  mydata[0] = uint16_t(VBat) & 255;
  mydata[1] = (uint16_t(VBat) >> 8) & 255;
  mydata[2] = uint16_t(VMOIST) & 255;
  mydata[3] = (uint16_t(VMOIST) >> 8) & 255;
  mydata[4] = uint16_t(VPH) & 255;
  mydata[5] = (uint16_t(VPH) >> 8) & 255;
  mydata[6] = uint16_t(VPhoto) & 255;
  mydata[7] = (uint16_t(VPhoto) >> 8) & 255;
  mydata[8] = uint16_t(TEMP_C) & 255;
  mydata[9] = (uint16_t(TEMP_C) >> 8) & 255;

  // Setting value to characteristics
  pChara_data->setValue(mydata, 10);

  // Notifying connected client
  pChara_data->notify();
  
  Serial.println("Sent value : ");
  Serial.print("VMOIST=");
  Serial.println(VMOIST);
  Serial.print("VPH=");
  Serial.println(VPH);
  Serial.print("VTEMP=");
  Serial.println(VTEMP);
  Serial.print("TEMP_C=");
  Serial.println(TEMP_C / 10);
  Serial.print("VPhoto=");
  Serial.println(VPhoto);
  Serial.print("VBat=");
  Serial.println(VBat);
  Serial.println("");
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SETUP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() {
  Serial.begin(19200);
  while (!Serial);

  Serial.println("Start of setup");
  
  BLEStart();

  pinMode(ESP32_VGND, OUTPUT);
  pinMode(ESP32_TOUCH, INPUT);
    
  pinMode(ESP32_PH, INPUT);
  pinMode(ESP32_MOIST, INPUT);
  pinMode(ESP32_TEMP, INPUT);
  pinMode(ESP32_PHOTO, INPUT);
  pinMode(EPS32_BAT, INPUT);
  
  pinMode(ESP32_LED_R, OUTPUT);
  pinMode(ESP32_LED_G, OUTPUT);
  pinMode(ESP32_LED_B, OUTPUT);
  pinMode(ESP32_LED_Y, OUTPUT);
  digitalWrite(ESP32_LED_R, LOW);
  digitalWrite(ESP32_LED_G, LOW);
  digitalWrite(ESP32_LED_B, LOW);
  digitalWrite(ESP32_LED_Y, LOW);
  
  // -- Fixed for the all adc1 ---
  // 4095 for 12-bits -> VDD_A / 4095 = 805uV  too jittery
  // 2047 for 11-bits -> 3.9 / 2047 = 1.9mV looks fine
  /*
     Set the resolution of analogRead return values. Default is 12 bits (range from 0 to 4095).
     If between 9 and 12, it will equal the set hardware resolution, else value will be shifted.
     Range is 1 - 16
  */
  analogReadResolution(12);
  
  // Calibration function
  esp_adc_cal_value_t val_type =
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, REF_VOLTAGE, adc_chars);
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// LOOP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {
  if (deviceConnected && charactConnected){
    sensorRead();
    delay(5000); 
  }
}
