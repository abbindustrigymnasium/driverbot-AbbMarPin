#include <PubSubClient.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <Servo.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h" //https://github.com/tzapu/WiFiManager

#define motorPinRightDir 0   //D2
#define motorPinRightSpeed 5 //D1
#define motorPinLeftDir 2
#define motorPinLeftSpeed 4
#define servopin 13

const char *mqtt_server = "maqiatto.com";
const char *topic = "martin.pind@abbindustrigymnasium.se/motor";
const char *topic2 = "martin.pind@abbindustrigymnasium.se/servo";
const char *user = "martin.pind@abbindustrigymnasium.se";
const char *pass = "loaldoaldoawldoakdfigvjosgoshnbos";

WiFiClient espClient;
PubSubClient client(espClient);
Servo servo;

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), user, pass))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, "hello world");
      // ... and resubscribe
      client.subscribe(topic);
      client.subscribe(topic2);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(motorPinRightDir, OUTPUT);
  pinMode(motorPinRightSpeed, OUTPUT);
  pinMode(motorPinLeftDir, OUTPUT);
  pinMode(motorPinLeftSpeed, OUTPUT);

  Serial.begin(115200);
  servo.attach(servopin);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  // wifiManager.resetSettings();

  if (!wifiManager.autoConnect("_Martins AP", "1234567890"))
  {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected to" + WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  int speed = 1024;
  int dir = 0;

  // delay(2200);
  // digitalWrite(motorPinRightDir, dir);
  // analogWrite(motorPinRightSpeed, speed);
  // delay(2200);
  // digitalWrite(motorPinRightDir, 1);
  // analogWrite(motorPinRightSpeed, speed);

  // Drivetest3(motorPinLeftDir, motorPinLeftSpeed);
}

// Handle incomming messages from the broker
void callback(char *topic, byte *payload, unsigned int length)
{
  String topicStr;
  String payloadStr;

  for (int i = 0; topic[i]; i++)
  {
    topicStr += topic[i];
  }

  for (int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }

  // Serial.println("");
  // Serial.print("Message arrived - [");
  // Serial.print(topicStr);
  // Serial.print("] ");
  // Serial.println(payloadStr);

  if (topicStr == "martin.pind@abbindustrigymnasium.se/motor")
  {

    int onoff = payloadStr.substring(1, payloadStr.indexOf(',')).toInt();
    int direction = payloadStr.substring(payloadStr.indexOf(',') + 1, payloadStr.lastIndexOf(',')).toInt();
    int speed = payloadStr.substring(payloadStr.lastIndexOf(',') + 1).toInt();

    // Serial.println(valR);
    // Serial.println(valG);
    // Serial.println(valB);

    // valR = constrain(valR, 0, 255);
    // valG = constrain(valG, 0, 255);
    // valB = constrain(valB, 0, 255);

    // last_color[0] = valR;
    // last_color[1] = valG;
    // last_color[2] = valB;

    // change(valR, valG, valB);

    if (onoff)
    {
      Go(motorPinRightDir, motorPinRightSpeed, direction, speed);
    }
    else
    {
      Go(motorPinRightDir, motorPinRightSpeed, 0, 0);
    }
  }
  else if (topicStr == "martin.pind@abbindustrigymnasium.se/servo")
  {
    int angle = payloadStr.toInt();
    servo.write(angle);
  }
}

void Go(int Dirpin, int Speedpin, int Direction, int Speed)
{
  // if (Direction == 1)
  // Serial.println("Framåt");
  // else
  // Serial.println("Bakåt");

  if (Speed != 0)
  {
    Speed = map(Speed, 0, 100, 500, 1024);
  }
  // Serial.println("Hastighet: " + String(Speed));

  digitalWrite(Dirpin, Direction);
  analogWrite(Speedpin, Speed);
}
