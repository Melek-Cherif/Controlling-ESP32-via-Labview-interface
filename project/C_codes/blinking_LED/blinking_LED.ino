#include <WiFi.h>
#include <ESPmDNS.h>

#define LED 13
//#define WIFI_NETWORK "CORAIL-GUEST"
//#define WIFI_PASSWORD "CORAIL-GUEST"

#define WIFI_NETWORK "ViZmerald01"
#define WIFI_PASSWORD "isetso2021"

int port = 8888;   
WiFiServer server(port); 
bool LED_STATE = true;

int ConnectToWiFi()
{
  Serial.print("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
    }
    
    if (!MDNS.begin("esp32"))
    {
      Serial.println("Error starting mDNS");
      return -1;
    }

      Serial.println(WiFi.localIP());
      server.begin();
      return 0;
} 

void setup() 
{
  // put your setup code here, to run once:
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  ConnectToWiFi();
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);
  
}

void loop() 
{
  // put your main code here, to run repeatedly:
    WiFiClient client = server.available();
  
  if (client) {
    //Client handling code
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
  } 

    if (LED_STATE == true)
  {
    digitalWrite(LED, HIGH);
  }
  else digitalWrite(LED, LOW);
  
  
  while(client.connected())
    {      
      while(client.available()>0)
      {
        // read data from the connected client
       char client_message = client.read();
       Serial.write(client_message);   
       }
      
      delay(10); //small delay between polling
      
      //send data to the connected client
      while(Serial.available()>0)
      {
        int server_message = 0 ;
        if (LED_STATE == true)
        {
          server_message = 1;
        }
        client.write(server_message);
      }
      //Serial.println("finish send data");
      
    }
    
    //close the connection to the client
    client.stop();
    //Serial.println("Client disconnected");

}
