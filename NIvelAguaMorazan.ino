#define SensorVibracion A0 // SensorVibracion en el pin 7
#define Trigger A1 // Trigger en el pin 4
#define Echo 8 // Echo en el pin 3

#include <SD.h>
#include <DHT.h>
#include <DHT_U.h>
#include <RTClib.h>
#include <LowPower.h>

RTC_DS3231 rtc;
float hum = 0;
float temp = 0;
const byte HYTS = 9;
const byte SDIN = 10;
unsigned int readPeriod = 1;
unsigned int writePeriod = 1;
bool vibracion = false;
bool leer = true;

//unsigned int writeTime = 0;
//unsigned int readTime = 0;

//int tiltSensorPreviousValue = 0;
int tiltSensorCurrentValue = 0;
////long lastTimeMoved = 0;
////int shakeTime = 50;

File lectura;
DHT DHTsensor(HYTS, DHT22);
long distancia;
void setup() {
  // put your setup code here, to run once:
//Serial.begin (9600);
//Serial.println("Inicio");
if (! rtc.begin()) {
    //Serial.println("No hay un módulo RTC");
    while (1);
  }
 //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //DateTime now = rtc.now();
    //Serial.println(now.hour()); 
    // Serial.println("listo2"); 
  pinMode (SensorVibracion, INPUT);
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);
  digitalWrite(SensorVibracion, HIGH);

  DHTsensor.begin();
  SD.begin(SDIN);

  pinMode(HYTS, INPUT);
  pinMode(SDIN, OUTPUT);

}

void loop() {
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  //LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); 
  unsigned long inicio = millis();
  //leer=true;
  rtc.begin();
  DHTsensor.begin();
  SD.begin(SDIN);
  unsigned long timeEcho = millis();
  //while(timeEcho -millis()<50){
    long t;
    tiltSensorCurrentValue = digitalRead( SensorVibracion); 
    if (tiltSensorCurrentValue){
      vibracion = true;
    }
    digitalWrite(Trigger, HIGH); //Mandamos un pulso a Trigger
    delayMicroseconds(10);
  //definimos segun las especificaciones tecnicas del componente
    digitalWrite(Trigger, LOW);
  
    t=pulseIn(Echo, HIGH);  //encontramos el ancho del pulso
    distancia=t/59;  //hacemos la relacion de la distancia en cm segun el tiempo
  
    //Serial.println(distancia); 
  //if ( leer ) {
    hum = 0;
    temp = 0;
    readValues();
    writeSD();
  //}
  long wait=millis()-inicio;
  while(wait<2000){
    LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);
    wait+=15;
  }
}



void readValues() {
  
  if ( isnan(DHTsensor.readHumidity()) != 1 )
        hum = DHTsensor.readHumidity();           // Se lee la humedad
  if ( isnan(DHTsensor.readTemperature()) != 1 )
        temp = DHTsensor.readTemperature();       // Se lee la temperatura

}

void writeSD() {

  lectura = SD.open("datos.txt", FILE_WRITE);

  DateTime now = rtc.now();
  if( lectura ){
    //Serial.println("SD"); 
    // Escribe el valor de humedad sin espacios decimales
    lectura.print(now.hour());
    lectura.print(":");
    lectura.print(now.minute());
    lectura.print(":");
    lectura.print(now.second());
    lectura.print(",");
    lectura.print(now.day());
    lectura.print("/");
    lectura.print(now.month());
    lectura.print("/");
    lectura.print(now.year());
    lectura.print(",");
    lectura.print(hum, 1);
    lectura.print("%");
    lectura.print(",");

    // Escribe el valor de temperatura con un espacio decimal
    lectura.print(temp, 2);
    lectura.print("°C");
    lectura.print(",");
    lectura.print(distancia, 1);
    lectura.print("cm");
    lectura.print(",");
    lectura.print(vibracion);
    lectura.println();

  vibracion=false;
 
 }

  lectura.close();
}
