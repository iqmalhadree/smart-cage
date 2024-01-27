#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include "DHT.h"
#include "string"

//network connection
const char *WIFI_SSID = "Hotspot"; //your WiFi SSID
const char *WIFI_PASSWORD = "iqmalhadree99"; // your password
const char *MQTT_SERVER = "35.239.179.174"; // your VM instance public IP address
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "smart-cage"; // MQTT topic
const char *TEMP_TOPIC = "smart-cage/temperature";
const char *MOIST_TOPIC = "smart-cage/moisture";
const char *FAN_TOPIC = "smart-cage/fan";
const char *FEED_TOPIC = "smart-cage/feeder";
WiFiClient espClient;
PubSubClient client(espClient);

//Used Pins
const int servoPin = 14; 
const int relayPin = 48;   
const int dht11Pin = 4; 
const int moisturePin = A2;
const int DHT_TYPE = DHT11;

//device initialization
Servo catFeeder;
DHT dht(dht11Pin, DHT_TYPE);

//global variable
int currentMoisture;
int initialMoisture;

//Connecting to wifi
void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client")){
    Serial.println("Connected to MQTT server");
    client.subscribe(FAN_TOPIC);
    client.subscribe(FEED_TOPIC);
    }
    else
    {
    Serial.print("Failed, rc=");
    Serial.print(client.state());
    Serial.println(" Retrying in 5 seconds...");
    delay(5000);
    }
  }
}

void FanCallback(char* FAN_TOPIC, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(FAN_TOPIC);
  Serial.print("] ");
  String message = "";
  for (int i=0;i<length;i++) {
    message = message + (char)payload[i];
  }
  Serial.println(message);
  if (message == "On" || message == "on" || message == "ON"){
    digitalWrite(relayPin, true); //turn on the fan
  }
  else {digitalWrite(relayPin, false);}
}

void FeedCallback(char* FEED_TOPIC, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(FEED_TOPIC); 
  Serial.print("] ");
  String message = "";
  for (int i=0;i<length;i++) {
    message = message + (char)payload[i];
  }
  Serial.println(message);
  if (message == "Feed" || message == "feed" || message == "FEED"){
    catFeeder.write(180);
    delay(1000);
    catFeeder.write(0);
    //flip open the pet feeder
  }
  else {catFeeder.write(0);}
}

void setup() {
  Serial.begin(300);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(FanCallback); //listen to user for turning on/off the fan
  client.setCallback(FeedCallback); //listen to user for feeding cat

  dht.begin();
  catFeeder.attach(servoPin);
  catFeeder.write(0);
  initialMoisture = analogRead(moisturePin); // facilitate changes in moisture of sand cat
  pinMode(relayPin, OUTPUT);
  pinMode(moisturePin, INPUT);
}

void loop()
{
  if (!client.connected()){
    reconnect();
  }
  client.loop();
  delay(5000); // adjust the delay according to your requirements
  
  float t = readTemperature();
  float temp_threshold = 30.00;
  if (t >= temp_threshold){
    digitalWrite(relayPin, true);
  }
  else {digitalWrite(relayPin, false);}

  float m = readMoisture();
  if (m > 0.5){
    Serial.println("Your pet has peed");
    client.publish(MOIST_TOPIC, "Your pet has peed");
  }
  else {Serial.println("Moisture percentage below 0.1");}
}

float readTemperature(){
  float temperature = dht.readTemperature();
  char payload[10];
  sprintf(payload, "%.2f", temperature);
  Serial.println(temperature);
  client.publish(TEMP_TOPIC, payload);

  return temperature;
}

float readMoisture(){
  currentMoisture = analogRead(moisturePin);
  float percentage = currentMoisture/initialMoisture;
  return percentage;
}
