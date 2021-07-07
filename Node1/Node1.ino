// node1 -EMS Project by Cornell Castelino

#include <SPI.h>
#include <WiFiNINA.h>
int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

IPAddress NodeIP = IPAddress(192, 168, 0, 177);//<------------
IPAddress MyIP = IPAddress( 192, 168, 0, 178); //<------------

unsigned int localPort = 80;      // local port to listen on

char packetBuffer[256]; //buffer to hold incoming packet
int parameters[2][1] = {{0},// { power value } add more parameters like voltage and current
  {0}
};
char buff[255];             // buffer for incoming values

int holdtime = 10000;
unsigned long mytime = millis();

//behaves both as a server and a client
WiFiServer server(localPort);
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  //  while (!Serial||holdtime<millis()) {
  //    ; // wait for serial port to connect or connect after 10s
  //  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");// should change to send msg to computer/router
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
    Serial.println(fv);
  }
  WiFi.config(MyIP);//<------------
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();


  Serial.println("\nData collection in progress.. | Communicating with other nodes");
  // if you get a connection, report back via serial:
  server.begin();//starts listening
}



void loop() {

  if (millis() > mytime + holdtime) {
    mytime = millis();
    Serial.println("Node 1 is running");
  }

//SERVER MODE
  
  // if there's data available, read a packet
  WiFiClient Myclients = server.available();
  int nodeIndex = 0;//change location
  if (Myclients) {

    //other nodes connect to current node to exchange data.
    Serial.println("new client");

    int variableNum = 0;
    int buffIndex = 0;
    bool NodeFound = false;
    while (Myclients.connected()) {
      if (Myclients.available()) {

        //making a parse function to read and return values to char arrays/ints
        char c = Myclients.read();      //read character
        Serial.write(c);

        if (c == ',' || c == '\n') {          //change variable for other parameters
          if (buff + '\0' == "ACK") {
            Serial.println("received ACK from Node");
            break;
          }
          else if (!NodeFound) {
            Serial.print("Node found:");
            nodeIndex = atoi(buff + '\0');// sets which node we are communicating with.. can be done with IPaddress but making it simpler for now.
            Serial.println(nodeIndex);
            NodeFound = true;
          } else {                                               //inserts values(pow,V,I etc..) into an int array
            parameters[nodeIndex][variableNum] = atoi(buff + '\0');
            variableNum++;
          }
        } else if (c == '\n') {                                  //breaks if we reach end of sentence
          // you're starting a new line
          Myclients.println("ACK");                             //ACK
          break;
        } else {                                                 //inserts new character into the buffer
          buff[buffIndex] = c;
          buffIndex++;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
//SERVER OUTPUT
    Serial.print("power from node");
    Serial.print(nodeIndex);
    Serial.print(": ");
    Serial.println(parameters[nodeIndex][0]);
    // close the connection:
    Myclients.stop();
    Serial.println("client disconnected");
    Serial.println("___________________");
  }
    delay(50);

//CLIENT MODE
    //current nodes connects with other nodes to exchange data.
    if (client.connect(NodeIP, localPort)) {
      Serial.println("connected to Node2");
      if (millis() > mytime + holdtime && client.connected()) {
        mytime = millis();
        Serial.println("Sending Data to Node2...");//<------------
        client.println("1,524");                   //<------------
      }
    }
    boolean currentLineIsBlank = true;
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
      if (c == '\n' && currentLineIsBlank) {
        break;
      }
      if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
      } else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }

    //if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting from node1."); //<------------
      client.stop();
    }
  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}
