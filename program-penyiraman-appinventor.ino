//[1] Identitas Pembuat Coding
/**
* [Project 2021]
* 
* Generate by :
*           1.Devandri Suherman
*           Student Research from Electronics Engineering Departement, Bandung State of Polytechnic(1)
* Notes     : Automatic ON-OFF using APP Inventor
* Copyright 2021
*/

#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
//Set WiFi
#define WIFI_SSID "XXXX"
#define WIFI_PASSWORD "XXXXX"
//Set Firebase Database
#define FIREBASE_HOST "XXXXX"
#define FIREBASE_AUTH "XXXXX"
//set pin LED
const int pinon=13; //led on if wifi connect
const int pinoff=27; //led on if wifi !connect
//pompa
const int pinpompa = 22;
String pompa ; // data pompa
//sensor - sensor
//sensor Suhu
#include "DHT.h"
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float t,h;
//sensor sil mouisture
const int soilpin=15;
int kelembaban;
float p;
int inSVdown, inSVup; // bacaan dari database
//fungsi millis()
unsigned long waktuSebelum=0;

void setup() {
Serial.begin(9600);
dht.begin(); // mengkoneksikan DHT
//======== Mengatur pin
pinMode(pinpompa, OUTPUT);
pinMode(pinon, OUTPUT);
pinMode(pinoff, OUTPUT);
//======== Connect to WiFi
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() !=WL_CONNECTED){
Serial.print(".");
digitalWrite(pinoff,HIGH);
digitalWrite(pinon,LOW);
delay(500);
}
Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
//======== Menyalakan lampu Indikator
digitalWrite(pinoff, LOW);
digitalWrite(pinon, HIGH);
//======== Connect to Firebase
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//======== Setting Pompa
Firebase.setString("pompa","Ready");
digitalWrite(pinpompa,HIGH);
}
void loop() {
  inputkelembapan();
  sensorsuhu();
  ngambildata();
  pompakondisi();
}
void inputkelembapan(){
  kelembaban = analogRead(soilpin);
  if(kelembaban<=1200) kelembaban=1200;
  if(kelembaban>=3600) kelembaban=3600; //setdefault 3500
  p = map(kelembaban, 1200, 3500, 100, 0);
  Firebase.setFloat("Tanah",p);
}

void sensorsuhu(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  Firebase.setFloat("Suhu",t);
  Firebase.setFloat("Kelembapan",h);
}

void ngambildata(){
  //======Mengambil data setdown ke Firebase
  String SVdown=Firebase.getString("setdown");
  //====Mengambil data SVdown
  String ambilSVdown="";
  ambilSVdown += SVdown.charAt(2);
  ambilSVdown += SVdown.charAt(3);
  //convert to integer
  inSVdown=ambilSVdown.toInt();
  //======mengambil data setup ke Firebase
  String SVup=Firebase.getString("setup");
  //=====Mengambil data SVup
  String ambilSVup="";
  ambilSVup += SVup.charAt(2);
  ambilSVup += SVup.charAt(3);
  //convert to integer
  inSVup=ambilSVup.toInt();
}

void pompakondisi(){
  int inSVdownCache = 0;
  int inSVupCache=0;
  unsigned long now=0;
  if (millis() - now > 3000){
  now = millis();
  if (inSVdown != inSVdownCache){
  inSVdownCache = inSVdown;
  Serial.println(inSVdown);
  if(p<inSVdown){ //Pemisalan Tanah Kering (Menghidupkan pompa
  digitalWrite(pinpompa,LOW);
  Firebase.setString("pompa","ON"); //Mengirim Kondisi Pompa ON
  }
}
if (inSVup != inSVupCache){
  inSVupCache = inSVup;
  Serial.println(inSVup);
  if(p=inSVup){ // Mematikan pompa
    digitalWrite(pinpompa,HIGH);
    Firebase.setString("pompa","OFF"); //Mengirim Kondisi Pompa OFF
      }
    }
  }
}
  


