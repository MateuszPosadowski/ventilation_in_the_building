#include "DHT.h"          // biblioteka DHT
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
 
#define DHTPIN D5          // numer pinu sygnałowego
#define DHTTYPE DHT11     // typ czujnika (DHT11). Jesli posiadamy DHT22 wybieramy DHT22
                                             

DHT dht(DHTPIN, DHTTYPE); // definicja czujnika
const char* ssid = "Orange_Swiatlowod_55A0"; //  here router or wifi SSID
const char* password = "35F56YMHPSSA"; // here router or wifi password

const char* serverName = "http://192.168.1.206/ventilation_systems/czujnik3.php";

int pin2 = D3; // numer pinu czujnika jakości powietrza
int pin1 = D2; // numer pinu czujnika jakości powietrza
unsigned long duration1;
unsigned long duration2;

unsigned long starttime;
unsigned long sampletime_ms = 3000;//sampe 1s ;
unsigned long lowpulseoccupancy1 = 0;
unsigned long lowpulseoccupancy2 = 0;
float ratio1 = 0;
float ratio2 = 0;
float concentration1 = 0;
float concentration2 = 0;


int value;


WiFiClient client;

void setup()
{
  Serial.begin(9600);     // otworzenie portu szeregowego
  dht.begin();            // inicjalizacja czujnika
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  starttime = millis();//get the current time;
   

}
 
void loop()
{
  //odczyt jakości powietrza

    duration1 = pulseIn(pin1, LOW);
    duration2 = pulseIn(pin2, LOW);
    lowpulseoccupancy1 += duration1;
    lowpulseoccupancy2 += duration2;

    Serial.print(lowpulseoccupancy1);
    Serial.print(" lowpulseoccupancy1");

    
    Serial.print(lowpulseoccupancy2);
    Serial.print(" lowpulseoccupancy2");
    
    ratio1 = lowpulseoccupancy1/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration1 = 1.1*pow(ratio1,3)-3.8*pow(ratio1,2)+520*ratio1+0.62; // using spec sheet curve

    ratio2 = lowpulseoccupancy2/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration2 = 1.1*pow(ratio2,3)-3.8*pow(ratio2,2)+520*ratio2+0.62; // 



 
       
    Serial.print("concentration1 = ");
    Serial.print(concentration1);
    Serial.print(" pcs/0.01cf  -  ");

    Serial.print("concentration2 = ");
    Serial.print(concentration2);
    Serial.println(" pcs/0.01cf  -  ");
    Serial.println("   ");
    lowpulseoccupancy1 = 0;
    lowpulseoccupancy2 = 0;
    starttime = millis();
  
    Serial.print("PM10 ");
    Serial.print(concentration1,3);
    Serial.print("PM25 ");
    Serial.print(concentration2,3);
    Serial.println("   ");

    lowpulseoccupancy1 = 0;
    lowpulseoccupancy2 = 0;
    starttime = millis();
  

  // Odczyt temperatury i wilgotności powietrza
  float t = dht.readTemperature();
  float h = dht.readHumidity();
 
  // Sprawdzamy czy są odczytane wartości
  if (isnan(t) || isnan(h))
  {
    // Jeśli nie, wyświetlamy informację o błędzie
    Serial.println("Blad odczytu danych z czujnika");
  } else
  {
    // Jeśli tak, wyświetlamy wyniki pomiaru
    Serial.print("Wilgotnosc: ");
    Serial.print(h);
    Serial.print(" % ");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C");
  }
   
   if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    
     
    //http://192.168.1.16/testcode/dht11.php?humidity=44.00&temperature=23.40&air_quality=5000
    String httpRequestData = "?humidity=" + String(h)
                          + "&temperature=" + String(t)
                          + "&air_quality=" + String(concentration1);
                          
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    String serverPath = serverName + httpRequestData;
    http.begin(client, serverPath);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  delay(30000);
  //Send an HTTP POST request every 30 seconds
   
  
}
