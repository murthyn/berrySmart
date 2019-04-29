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

//----------SERVER GLOBAL VARIABLES----------//
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
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

#define SERVICE_UUID        "336de7fe-34a3-4e9b-a527-d143bacd4579"
#define CHARACTERISTIC_UUID "c7098593-62e0-4368-9e26-ad68242cda59" //UUID for ESP 2->3
static BLEUUID serviceUUID("336de7fe-34a3-4e9b-a527-d143bacd4579");
static BLEUUID    charUUID("f9f89149-d81e-434b-8cb7-6b296a68b545"); //UUID for ESP 1->2



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
    strncpy(dataBuffer[endBuffer], (char*)pData, sizeof(dataBuffer[endBuffer]));
    endBuffer++;
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
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void clientSetup() {
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Server & Client application...");

  // Create the BLE Device
  BLEDevice::init("esp2");

  // Instantiate timer
  receiveTime = millis();

  // Set up server
  serverSetup();

  // Set up client
  clientSetup();
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
//        delay(500); // give the bluetooth stack the chance to get things ready
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

void loop() {
    // receive
    if (endBuffer == 0 || (millis() - receiveTime) < MAX_RECEIVE_TIME) {
      loopReceive();
    }
    // send
    else {
      loopSend();
      // done sending, time to receive
      receiveTime = millis();
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
