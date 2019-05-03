/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/

// Receives information from ESP1, sends to ESP3

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h> //Connect to WiFi Network

/* TODOs:
 * - Change name of client UUID
 * - Change type of client UUID to match server
 * - Create sublibraries to hold helper functions
 * - Add if statements for setupClient and setupServer (instead of one setup) - put in library
 * - Add global states for server and client
 */

//----------BUFFER/GENERAL GLOBAL VARIABLES----------//
char dataBuffer [30][30]; // [Number of Strings][Max Size of Strings]
int endBuffer = 0;
bool isReceiving = true;
float receiveTime;
const float MAX_RECEIVE_TIME = 3000.0;
#define CLIENT 0
#define SERVER 1
int state = CLIENT;

//----------SERVER GLOBAL VARIABLES----------//
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEService* pService = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
uint32_t counter = 0;

//----------CLIENT GLOBAL VARIABLES----------//
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;


//----------SERVER & CLIENT UUIDs----------//

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "df8f99f2-7c85-422a-91f6-73abc1fb40bd"
#define CHARACTERISTIC_UUID "c7098593-62e0-4368-9e26-ad68242cda59" //UUID for ESP 2->3
static BLEUUID serviceUUID("336de7fe-34a3-4e9b-a527-d143bacd4579");
static BLEUUID    charUUID("f9f89149-d81e-434b-8cb7-6b296a68b545"); //UUID for ESP 1->2


//----------POSTING----------//
 
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.

const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

char network[] = "6s08";  //SSID for 6.08 Lab
char password[] = "iesc6s08"; //Password for 6.08 Lab
//char network[] = "iPhone (2)";  //SSID for 6.08 Lab
//char password[] = "hello123"; //Password for 6.08 Lab

const uint8_t LOOP_PERIOD = 15000; //milliseconds
uint32_t primary_timer = 0;


//----------SERVER & CLIENT CALLBACKs----------//

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
      Serial.println("onConnect -------------------------------");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
    // add to dataBuffer
    char* dataA = (char*)pData;
    char* nullArray = {"\0"};
    dataA[length] = *nullArray;
//    int address = (char*)pData + length;
//    *address = "\0";

    // droptail for congestion control
    if (endBuffer < sizeof(dataBuffer)/sizeof(char)) {
      strncpy(dataBuffer[endBuffer], (char*)pData, sizeof(dataBuffer[endBuffer]));
      endBuffer++;
    }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("onConnect (CLIENT) *******************");
  }
  

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

//----------CLIENT HELPER FUNCTIONS----------//

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
      
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

//----------SETUP HELPER FUNCTIONS----------//
void serverSetup() {
  Serial.println("STARTING INIT (server)");
  BLEDevice::init("esp2");
  Serial.println("FINISHING INIT (server)");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  Serial.println("finished creating server (server)");
  pServer->setCallbacks(new MyServerCallbacks());

  Serial.println("HERE");
  
  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);
  deviceConnected = false;
  Serial.println("HERE2");

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  Serial.println("HERE3");
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());
  Serial.println("HERE4");
  // Start the service
  pService->start();
  Serial.println("HERE5");
  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  Serial.println("HERE6");
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void clientSetup() {
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  Serial.println("STARTING INIT (client)");
  BLEDevice::init("esp2");
  Serial.println("FINISHING INIT (client)");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void wifiSetup() {
  // WIFI
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 6) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Server & Client application...");

  // Instantiate timer
  receiveTime = millis();

  if (state == SERVER) {
    // Set up server
    serverSetup();
  }
  else {
    // Set up client
    clientSetup();
  }

  wifiSetup();
}

//----------UNSETUP HELPER FUNCTIONS----------//

void serverUnsetup() {
  Serial.println("STARTING DE-INIT (server)");
  BLEDevice::deinit(true);
  Serial.println("FINISHED DE-INIT (server)");
  pServer = NULL;
  pCharacteristic = NULL;
  deviceConnected = false;
  oldDeviceConnected = false;
  value = 0;
  counter = 0;
}

void clientUnsetup() {
  Serial.println("STARTING DE-INIT (client)");
  BLEDevice::deinit(true);
  Serial.println("FINISHED DE-INIT (client)");
  doConnect = false;
  connected = false;
  doScan = false;
  pRemoteCharacteristic = NULL;
  myDevice = NULL;
}

//----------LOOP HELPER FUNCTIONS----------//

void loopReceive() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    //Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(1000); // Delay a second between loops.
}

void loopSend() {
  // notify changed value
    if (deviceConnected) {
      Serial.println("LOOP SEND DEVICE CONNECTED");
        for (int i = 0; i < endBuffer; i++) {
          char val[100];
          sprintf(val, dataBuffer[i]);
          pCharacteristic->setValue(val);
          pCharacteristic->notify();
          delay(1000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        }
        // reset buffer
        endBuffer = 0;
        for(int i = 0; i < 10; i++){
          dataBuffer[i][0] = '\0';
        }
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
//        pServer->startAdvertising(); // restart advertising
//        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

void loopPost() {
    Serial.println("Posting!");

    for (int i = 0; i < endBuffer; i++) {
      char body[200]; //for body;
      sprintf(body, "text=%s",dataBuffer[i]);

      int body_len = strlen(body); //calculate body length (for header reporting)
      Serial.println(body);
      sprintf(request_buffer, "POST http://608dev.net/sandbox/sc/garciag/blueberry_Handler.py HTTP/1.1\r\n");
      strcat(request_buffer, "Host: 608dev.net\r\n");
      strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
      strcat(request_buffer, "\r\n"); //new line from header to body
      strcat(request_buffer, body); //body
      strcat(request_buffer, "\r\n"); //header
      Serial.println(request_buffer);
      do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    } 

    endBuffer = 0;
    for(int i = 0; i < 30; i++){
      dataBuffer[i][0] = '\0';
//      for(int j = 0; j < 30; j++) {
//         dataBuffer[i][j] = '\0';
//      }
    }
}

void loop() {

    if (state == CLIENT) {
      loopReceive();
    }
    else {
      loopSend();
      // done sending, time to receive
      receiveTime = millis();
    }

   
    if (endBuffer == 0 || (millis() - receiveTime) < MAX_RECEIVE_TIME) {
      // switch from SERVER to CLIENT
      if (state == SERVER) {
        serverUnsetup();
        clientSetup();
        
      }
      state = CLIENT;
    }
    else {
      // switch from CLIENT to SERVER
//      if (state == CLIENT) {
//        clientUnsetup();
//        serverSetup();
//      }
//      state = SERVER;
        state = CLIENT;
    }

    // posting
    if (endBuffer > 5) {
      loopPost();
    }

    // debugging
    Serial.println("BUFFER");
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < sizeof(dataBuffer[0]); j++) {
        Serial.print(dataBuffer[i][j]);
      }
      Serial.println("");
    }
    Serial.println("END BUFFER");
    Serial.println(endBuffer);
    Serial.println("DEVICE CONNECTED");
    Serial.println(deviceConnected);
}

// POSTING HELPER FUNCTIONS

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
