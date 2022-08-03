#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <PubSubClient.h>

#define ledPin 5
#define ldrPin A0

String Status;

const char* ssid = ""; //ssid network
const char* password = ""; //password network

const char* mqtt_server = "broker.hivemq.com"; //server cloud
int port = 1883; //port cloud network

const char *mqtt_user = ""; //username cloud
const char *mqtt_pass = ""; //password cloud

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMillis = 0;
char celc[4];

void setup_wifi() {
  delay(100);
  //connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp = (char)payload[i];
  }
  if(messageTemp == "1"){
    digitalWrite(ledPin, HIGH); //topic mqtt out
    Serial.print("On");
  }
  else if(messageTemp == "0"){
    digitalWrite(ledPin, LOW);//topic mqtt out
    Serial.print("Off");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX); //Client ID
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("test1");
      Serial.println(clientId);
      //delay(1000);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);
  pinMode(ledPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  reconnect();
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  int sensorValue = analogRead(ldrPin);   // read the input on analog pin 0

//  float voltage = sensorValue * (5.0 / 1023.0);   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)

  if (sensorValue >= 80) {
    digitalWrite(ledPin,HIGH);
    Status="Kosong";
  } else {
    digitalWrite(ledPin,LOW);
    Status="Terisi";
  }
  client.publish("test1",Status.c_str());
  Serial.println(Status);
  Serial.println(sensorValue);
  client.loop();
  delay(1000);
}
