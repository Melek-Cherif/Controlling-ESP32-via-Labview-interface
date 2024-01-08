#include <WiFi.h>
#include <ESPmDNS.h>
#include <Esp32WifiManager.h>

//LinxESP32* LinxDevice;
#define LED 13
//#define WIFI_NETWORK "ViZmerald01"
//#define WIFI_PASSWORD "isetso2021"

#define WIFI_NETWORK "TOPNET_F4C0"
#define WIFI_PASSWORD "1qo6f322sh"

bool output_stat = false;
bool input_stat = false;
char On = 'on', off = 'off';
int port = 8888;
WiFiServer server(port);      // Creates a server that listens for incoming connections on the specified port


void connectToWifi() {
  Serial.print("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  if (!MDNS.begin("esp32")) {
    Serial.println("Error starting mDNS");
    return;
  }

  Serial.println(WiFi.localIP());
  server.begin();
}

void setup() {

  //LinxDevice = new LinxESP32();
  //LinxWifiConnection.Start(LinxDevice, 44300);

  pinMode(LED, OUTPUT);

  Serial.begin(9600);
  connectToWifi();
  // Start listening for clients
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);
}

void loop() {

  // listen for incoming clients
  WiFiClient client = server.available();

  if (client) {
    //Client handling code
    if (client.connected())
    {
      Serial.println("Client Connected");
    }

    while (client.connected())
    {
      //input_stat = true;
      //digitalWrite(LED,HIGH);
      while (client.available() > 0)
      {
        // read data from the connected client
        char client_message = client.read();
        if (client_message == off)
        {
          output_stat = false;
          digitalWrite(LED, LOW);
          Serial.write(client_message);
          Serial.println("received msg = OFF");
        }
        if (client_message == On)
        {
          output_stat = true;
          digitalWrite(LED, HIGH);
          Serial.write(client_message);
          Serial.println("received msg = ON");
        }
      }
      
      delay(10); //small delay between polling
      //Serial.println("finish 10ms");

      //send data to the connected client
      while (Serial.available() > 0)
      {
        char server_message = 0;
        if (output_stat == true)
        {
          server_message = 1;
          client.write(server_message);
        }
        else 
        {
          client.write(server_message);
        }
      }
      //Serial.println("finish send data");
    }

    //close the connection to the client
    client.stop();
    Serial.println("Client disconnected");
    output_stat = false;
    //digitalWrite(LED, LOW);
  }

}
