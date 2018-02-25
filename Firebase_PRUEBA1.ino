#include <Adafruit_BMP085.h>  
#include <Wire.h> 
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#define DHTPIN 16
#define DHTTYPE DHT11
Adafruit_BMP085 bmp; 
DHT dht(DHTPIN, DHTTYPE);

#define FIREBASE_HOST "fire1-f885b.firebaseio.com"
#define FIREBASE_AUTH "dYXj75tnbTg18en30DXzZXxC5R0tqkt8PXmgZqGh"
#define WIFI_SSID "leonleon"
#define WIFI_PASSWORD "bbbbbbbb"
//****************************** luminosidad
const long A = 1000;     //Resistencia en oscuridad en KΩ
const int B = 150;        //Resistencia a la luz (10 Lux) en KΩ
const int Rc = 10;       //Resistencia calibracion en KΩ
const int LDRPin = 0;   //Pin del LDR

int V;
int ilum1;
int ilum2;
int ilum3;

void setup() {
   
  
  Serial.begin(9600);
  dht.begin();
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
  /***************************************************************************************** LECTURA DE TEMPERATURA */
Wire.begin(2, 0);   // SCL D3, SDA D4
Wire.setClock(400000); 
if (!bmp.begin()) {
  Serial.println("Error en el sensor");
  while (1) {}
}
  /**************************************************************************************************/

}


void loop() {
  //********************************************************************** PRESION 

  int pressure = bmp.readPressure(); 
  int temperature = bmp.readTemperature();
     Serial.print(" %\n");
  Serial.print("Temperatura: ");
  Serial.println(temperature);
  Serial.print("Presentacion: ");
  Serial.println(pressure/100);
       Serial.print(" %\n");
  delay(1000);
  //********************************************************************** HUMEDAD, TEMPERATURA

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
   Serial.print(" %\n");


//********************************************************************** LUMINOSIDAD

  V = analogRead(17);         
 // Serial.println(V);
 //  ilum = ((long)(1024-V)*A*10)/((long)B*Rc*V);  //usar si LDR entre GND y A0 
 //  ilum = ((long)V*A*10)/((long)B*Rc*(1024-V));   
 //    Serial.println(ilum);
if( V!= 0) {
  if(V <1024){
   ilum3 = ((long)V*A*100)/((long)B*Rc*(1024-V));   
   //  ilum1 = (V*A*10);
   // ilum2= B*Rc*(1024-V);
   // ilum3 = ilum1/ilum2;
  }else{
    V = 1023;
   ilum3 = ((long)V*A*100)/((long)B*Rc*(1024-V));   
  }
 if(ilum3  < 2000){
   Serial.println(ilum3);
 }else{
  ilum3 = 2000;
   Serial.println(ilum3);
 }
}
    
      delay(1000);

      
 //********************************************************************** MANDAR A FIREBASE

      StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["temperature"] = temperature;
        root["pressure"] = pressure;
        root["humidity"] = h;
        root["iluminacion"] = ilum3;
            
        // append a new value to /logDHT
        String name = Firebase.push("/estacion", root);
        // handle error
        if (Firebase.failed()) {
          Serial.print("error al darle push al /estacion");
          Serial.println(Firebase.error());  
          return;
        }
        delay(1000);



/*  Firebase.setString("message", "hello world");
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  */
/*
  // append a new value to /logs
  String name = Firebase.pushInt("logs", n++);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Serial.print("pushed: /logs/");
  Serial.println(name);
  delay(1000);
  */
}


