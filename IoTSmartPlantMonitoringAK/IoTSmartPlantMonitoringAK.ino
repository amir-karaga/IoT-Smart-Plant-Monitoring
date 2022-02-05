#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

//za DHT11
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

#define FIREBASE_HOST "iotprojectamir-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "c6dUKUa7pz8a7nRtvMCYBtpfUENez5GloY1RZOrN"
#define WIFI_SSID "Amir2000"
#define WIFI_PASSWORD "nemasifre"

FirebaseData firebaseData;
bool indikator;
bool komanda_prev=false;
int komanda=0;

void setup() {

  //za DHT11
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  
  Serial.begin(115200);
   pinMode(D7, OUTPUT); 
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int sensor=0;
int n = 0;
int tempZrakaParametar=0;
int vlagaZrakaParametar=0;
int vlagaZemljeParametar=0;

void loop() {

  Firebase.getInt(firebaseData, "Parametri/parametarTempZraka");
  tempZrakaParametar=firebaseData.floatData();
   Serial.print(tempZrakaParametar);
  Firebase.getInt(firebaseData, "Parametri/parametarVlazZemlje");
  vlagaZemljeParametar=firebaseData.floatData();
  Serial.print(vlagaZemljeParametar);
  Firebase.getInt(firebaseData, "Parametri/parametarVlazZraka");
  vlagaZrakaParametar=firebaseData.floatData();
  Serial.print(vlagaZrakaParametar);
  //citanje DHT11
    delay(delayMS);
  
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  Firebase.setFloat(firebaseData,"Zemljiste/temperaturaZraka", event.temperature);
  
  
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  Firebase.setFloat(firebaseData,"Zemljiste/vlaznostZraka", event.relative_humidity);

  sensor=analogRead(A0);
  Serial.print(sensor);
  Firebase.setFloat(firebaseData,"Zemljiste/vlaznostZemljista",sensor);

  if(sensor>vlagaZemljeParametar || event.temperature>tempZrakaParametar && event.relative_humidity<vlagaZrakaParametar )
  {
    Firebase.setInt(firebaseData,"Zemljiste/komanda",1);
  }
  else
  {
    Firebase.setInt(firebaseData,"Zemljiste/komanda",0);
  }
  
  if(Firebase.getInt(firebaseData, "Zemljiste/komanda"))
  {
    komanda=firebaseData.intData();
    if(komanda==1)
    {
      digitalWrite(D7,LOW);
    Serial.println("Relej uključen : ");
    Serial.print(komanda); 
    }
    else {
                digitalWrite(D7,HIGH);
                Serial.println("Relej isključen : ");
                Serial.print(komanda); 
         }
      
    }
  else
  {
     Serial.println(firebaseData.errorReason());
  }
  delay(100);
  }
  
