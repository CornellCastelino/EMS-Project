// node2 -EMS Project by Cornell Castelino

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 
//Sensitive data
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

//initialisation
int status = WL_IDLE_STATUS;
IPAddress server(192,168,0,177);  
IPAddress ip(192,168,0,178);

unsigned int localPort = 2390;      // local port to listen 
int holdtime = 5000;
unsigned long mytime = millis();
 
// Initialize the Ethernet client library
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial || holdtime < millis()) {
    ; // wait for serial port to connect or automatically connect after 5s.
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  WiFi.config(ip);//static IP
  
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
  
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, localPort)) {
      Serial.println("connected to server");
    }
}

void loop() {
    if(millis()>mytime + holdtime && client.connected()){
      mytime = millis();
      Serial.println("test: ");
      client.println("Hello");
  }
  // if there are incoming bytes available
  // from the server, read them and print them:
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
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
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

  byte mac[6]; //display mac
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}
