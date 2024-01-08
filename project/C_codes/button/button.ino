#include <WiFi.h>
#include <ESPmDNS.h>
#include <Esp32WifiManager.h>

// define I/O pins
# define disable 0
# define no_config 1
# define input 2
# define output 3

#define WIFI_NETWORK "Atl.Electrique"
#define WIFI_PASSWORD "Atl.ELC2021"

//#define WIFI_NETWORK "CORAIL-GUEST"
//#define WIFI_PASSWORD "CORAIL-GUEST"
int port = 8888;

char tab_config[40] = {};
char tab_result[40] = {};
char client_msg[20];
char char_pin[5];

bool button_stat = false;
bool LED_stat = false;
String On = "SON",
       Off = "SOFF", get_etat_input = "GSTPIN",
       get_etat_all = "GSTALL", set_config_input = "SCFGIN",
       set_config_output = "SCFGOUT", disable_pin = "SCFGDIS";
String read_button = "read button";
String str_msg = "";
String message_readed = "";


int pin = 0, i = 0;
// Creates a server that listens for incoming connections on the specified port
WiFiServer server(port);

void connectToWifi()
{
  Serial.print("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  if (!MDNS.begin("esp32"))
  {
    Serial.println("Error starting mDNS");
    return;
  }

  Serial.println(WiFi.localIP());
  server.begin();
}

/*void checkButton()
  {
  currentState = digitalRead(button);

  if (lastState == LOW && currentState == HIGH)
    Serial.println("Button pressed");
  LED_stat = true;

  lastState = currentState;
  }*/

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  connectToWifi();
  // Start listening for clients
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);

  initialization();
}


void loop()
{

  // listen for incoming clients
  WiFiClient client = server.available();

  if (client) {
    //Client handling code
    if (client.connected())
    {
      Serial.println("Client Connected\r\n");
    }

    while (client.connected())
    {
      //checkButton();
      message_readed = "";
      i = 0;
      while (client.available() > 0)
      {
        // read data from the connected client
        char client_message = client.read();
        if (client_message != 0)
        {
          message_readed += client_message;
        }
        //message_readed[i] = '\0';
      }

      if (message_readed != "")
      {
        Serial.print(message_readed);
        Serial.write("\n");
        int resultat_msg = decode_msg(message_readed);
        if (resultat_msg == 0)
        {
          Serial.println("trame OK");
        }
        else if (resultat_msg == -1)
        {
          Serial.println("trame not OK");
          client.write("trame not OK\r\n");
        }
        else if (resultat_msg == -2)
        {
          Serial.println("function not defined");
          client.write("function not defined\r\n");
        }
        else if (resultat_msg == -3)
        {
          Serial.println("parameter error");
          client.write("parameter error\r\n");
        }
        else if (resultat_msg == -4)
        {
          Serial.println("CRLF problem");
          client.write("CRLF problem\r\n");
        }

        if (str_msg == set_config_output)
        {
          if (tab_config[pin] != disable)
          {
            pinMode(pin, OUTPUT);
            tab_config[pin] = output;

            itoa(pin, char_pin, 10);
            strcpy(client_msg, "SCFGOUT ");
            strcat(client_msg, char_pin);
            strcat(client_msg, " OK\r\n");
            client.write(client_msg);
            Serial.println("pin set output");
          }
          else Serial.println("configured output pin");
        }
        else if (str_msg == set_config_input)
        {

          if (tab_config[pin] != disable)
          {
            pinMode(pin, INPUT_PULLUP);
            tab_config[pin] = input;
            itoa(pin, char_pin, 10);
            strcpy(client_msg, "SCFGIN ");
            strcat(client_msg, char_pin);
            strcat(client_msg, " OK\r\n");
            client.write(client_msg);
            Serial.println("pin set input");

          }
          else Serial.println("configured input pin");
        }
        else if (str_msg == On )
        {
          if (tab_config[pin] == output)
          {
            tab_result[pin] = 1;
            itoa(pin, char_pin, 10);
            strcpy(client_msg, "SON ");
            strcat(client_msg, char_pin);
            strcat(client_msg, " OK\r\n");
            client.write(client_msg);
            digitalWrite(pin, HIGH);
            Serial.println("LED ON");
          }
          else
          {
            Serial.println("non configured pin");
            client.write("non configured pin\r\n");
          }
        }
        else if (str_msg == Off)
        {
          if (tab_config[pin] == output)
          {
            tab_result[pin] = 2;
            itoa(pin, char_pin, 10);
            strcpy(client_msg, "SOFF ");
            strcat(client_msg, char_pin);
            strcat(client_msg, " OK\r\n");
            client.write(client_msg);
            digitalWrite(pin, LOW);
            Serial.println("LED OFF");
          }
          else
          {
            Serial.println("non configured pin");
            client.write("non configured pin\r\n");
          }
        }
        else if (str_msg == get_etat_input)
        {
          if (tab_config[pin] == input)
          {
            itoa(pin, char_pin, 10);
            strcpy(client_msg, "GSTPIN ");
            strcat(client_msg, char_pin);
            strcat(client_msg, " OK\r\n");
            client.write(client_msg);

            if (digitalRead(pin) == 0)
            {
              Serial.println("input non active");
              client.write("input non active\r\n");
            }
            else
            {
              Serial.println("input active");
              client.write("input active\r\n");
            }
          }
          else
          {
            Serial.println("non configured pin");
            client.write("non configured pin\r\n");
          }
        }
        else if (str_msg == get_etat_all)
        {
          strcpy(client_msg, "GSTPIN ");
          /*for (int a = 0; a < 30; a++)
            {
            getall[a] = tab_result[a];
            }*/
          //strcat(client_msg, tab_result);
          //strcat(client_msg, " OK\r\n");
          
          client.print(getAllResult(tab_result));
        }
        else if (str_msg == disable_pin)
        {
          for (int c = 0; (c < 40 && tab_config[i] != disable); c++)
          {
            setDisablePin(pin);
          }
          Serial.println("input disabled");
        }

        message_readed = "";
      }
      delay(10);
    }

    //close the connection to the client
    client.stop();
    Serial.println("Client disconnected");
    //digitalWrite(13, LOW);
    //digitalWrite(14, LOW);
  }
}

// separate the received message into function and pin number
int decode_msg(String msg)
{
  int space_index = msg.indexOf(" ");
  int CRLF_index = msg.indexOf("\r\n");
  String msg_pin;

  if (space_index < 0)
  {
    return -1;
  }

  if (CRLF_index < 0)
  {
    return -4;
  }

  if (CRLF_index != msg.length() - 2)
  {
    return -1;
  }

  str_msg = msg.substring(0, space_index);
  msg_pin = msg.substring(space_index + 1, CRLF_index);
  pin = msg_pin.toInt();

  if (!(str_msg == On || str_msg == Off || str_msg == get_etat_input || str_msg == get_etat_all
        || str_msg == set_config_input || str_msg == set_config_output || str_msg == disable_pin))
  {
    return -2;
  }
  else if (!(pin > 0 && pin < 37))
  {
    return -3;
  }

  return 0;
}

void setDisablePin(int pin)
{
  tab_config[pin] = disable;
}
void initialization()
{
  for (int i = 0; i < 40 ; i++)
  {
    tab_config[i] = no_config;
    tab_result[i] = disable;
  }

  setDisablePin(20); setDisablePin(24); setDisablePin(28); setDisablePin(29);
  setDisablePin(30); setDisablePin(31); setDisablePin(37); setDisablePin(38);
}

// differentiate the only input gpio
bool onlyInput(int pin)
{
  bool state = false;
  if (pin == 36 | pin == 39 | pin == 34 | pin == 35)
  {
    state = true;
  }
  return state;
}


//Check pin state configured or not
String check_pin_state(int pin)
{
  String result = "";
  if (tab_config[pin] == 3 )
    result = "output";
  else if (tab_config[pin] == 2)
    result = "input";
  else if (tab_config[pin] == 1)
    result = "no config";
  else result = "disable";

  return result;
}

String getAllResult (char tabResult[])

{
  int i = 0;
  String str = "";
  
  str = "GSTALL ";
  for (i = 0; i < 40; i++)
  {
    str += String(tabResult[i], DEC);
  }
  str += " OK\r\n";
  return str;
}
