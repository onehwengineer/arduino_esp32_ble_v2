## arduino_esp32_ble_v2
Arduino script (.ino) to assign ESP32 as a Bluetooth Low Energy (BLE) server and broacast data. <br>
<br>
React Native app serving as a client to ESP32 (server), can be found here: <br>
https://github.com/onehwengineer/rn_ble_manager_v2 <br>
Data broadcasted from ESP32 is displayed real-time.<br>
This app does NOT work in Simulator -> **App must be run in an actual device.**<br>

> **Change Logs** <br>
> [v4] <br>
>   - Functional POC1 test script <br>
>   - BLE data in raw bytes format (NOT string as previous) <br>  

> [v2p3] <br>
>   - Defined 4 characteristics per A's UUIDs <br>
>   - Included characteristic idenitifier in Characteristic Callback function <br>

> [v2p2] <br>
> Two callback functions defined :
>   - [1] One at Server (Device/Profile) level -> this callback function should define boolean : `deviceConnected` in `loop()` function
>     - This allows users to first makes a click in App to connect to ESP (**Device/Profile & Service**)
>   - [2] Second at Characteristic level -> this callback function should define boolean : `characteristicConnected` in `loop()` function
>     - This allows users to make another click afterwards to connect to a **Characteristic** --> ESP starts broadcasting Data
>   - When booleans from `deviceConnected && characteristicConnected` are satisfied, then Data is broadcasted
>   - **From my app, a single click to connect takes care of both [1] & [2]**

## Arduino Board Settings
**Setting up the board for the first time**
- Arduino -> Preferences -> Settings tab
- Under Additional Boards Manager URL, paste: https://dl.espressif.com/dl/package_esp32_index.json
- Tools -> Board -> Boards Manager -> Install `esp32 by Espressif Systems` (v1.0.4 as of 20201202)
- Tools -> Board -> ESP32 Arduino -> Select `ESP32 Dev Module`

**Board settings**<br>
<img src="https://user-images.githubusercontent.com/60368973/103705769-7eef5180-4f60-11eb-8fcd-9c70f18a5c09.png" width="400">

## Serial Monitor -> Baud Rate 9600

## BLE Max Payload Size is 20 Bytes
