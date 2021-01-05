## arduino_esp32_ble_v2
Arduino script (.ino) to assign ESP32 as a Bluetooth Low Energy (BLE) server and broacast data. <br>
<br>
React Native app serving as a client to ESP32 (server), can be found here: <br>
https://github.com/onehwengineer/rn_ble_manager_v2 <br>
Data broadcasted from ESP32 is displayed real-time.<br>
This app does NOT work in Simulator -> **App must be run in an actual device.**<br>


## Arduino Board Settings
**Setting up the board for the first time**
- Arduino -> Preferences -> Settings tab
- Under Additional Boards Manager URL, paste: https://dl.espressif.com/dl/package_esp32_index.json
- Tools -> Board -> Boards Manager -> Install `esp32 by Espressif Systems` (v1.0.4 as of 20201202)
- Tools -> Board -> ESP32 Arduino -> Select `ESP32 Dev Module`

**Board settings**<br>
<img src="https://user-images.githubusercontent.com/60368973/103686254-64f34600-4f43-11eb-921b-c179343b015a.png" width="400">

## Serial Monitor -> Baud Rate 9600

## BLE Max Payload Size is 20 Bytes
