#define SensorVibracion A0 // SensorVibracion en el pin 7
#define Trigger A1 // Trigger en el pin 4
#define Echo 8 // Echo en el pin 3

#include <SD.h>
#include <DHT.h>
#include <DHT_U.h>

#include <RTClib.h>
RTC_DS3231 rtc;
float hum = 0;
float temp = 0;
const byte HYTS = 9;
const byte SDIN = 10;
unsigned int readPeriod = 1;
unsigned int writePeriod = 1;
bool vibracion = false;
unsigned int writeTime = 0;
unsigned int readTime = 0;

int tiltSensorPreviousValue = 0;
int tiltSensorCurrentValue = 0;
long lastTimeMoved = 0;
int shakeTime = 50;

File lectura;
DHT DHTsensor(HYTS, DHT22);
long distancia;
void setup() {
  // put your setup code here, to run once:
Serial.begin (9600);
if (! rtc.begin()) {
    Serial.println("No hay un módulo RTC");
    while (1);
  }
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
  // put your main code here, to run repeatedly:
long t; // TIEMPO QUE DEMORA EN LLEGAR EL ECO
 // distancia en centimetros
unsigned long inicio = millis();

tiltSensorCurrentValue = digitalRead( SensorVibracion);
   
   
digitalWrite(Trigger, HIGH); //Mandamos un pulso a Trigger
delayMicroseconds(10);
//definimos segun las especificaciones tecnicas del componente
digitalWrite(Trigger, LOW);

t=pulseIn(Echo, HIGH);  //encontramos el ancho del pulso
distancia=t/59;  //hacemos la relacion de la distancia en cm segun el tiempo
//
//if (distancia<200){ //si la distancia es menor a 10 cm
//  digitalWrite (led, HIGH); //Se enciende el led 
//   Serial.print("Distancia: " ); 
//      Serial.print(distancia); // se imprime el valor de la distancia en el monitor serial
//      Serial.println(); 
//      delay(100);
//}else{ 
//    digitalWrite(led, LOW);  // Se mantiente 
//    
//      Serial.print("Distancia: " ); 
//      Serial.print(distancia); // se imprime el valor de la distancia en el monitor serial
//      Serial.println(); 
//      delay(100);
//    
//  }

  

  if ( readTime > readPeriod*1000 ) {
    
    readValues();
    readTime = 0;
  }

  if ( writeTime > writePeriod*1000 ) {
    
    writeSD();
    hum = 0;
    temp = 0;
    writeTime = 0;
  }
  unsigned long total = millis() - inicio;
  
  writeTime += total;
  readTime += total;
}



void readValues() {
  
   if (tiltSensorPreviousValue != tiltSensorCurrentValue) {
//          lastTimeMoved = millis();
//          tiltSensorPreviousValue = tiltSensorCurrentValue;
            vibracion= true;
   }
  if ( isnan(DHTsensor.readHumidity()) != 1 )
        hum = DHTsensor.readHumidity();           // Se lee la humedad
  if ( isnan(DHTsensor.readTemperature()) != 1 )
        temp = DHTsensor.readTemperature();       // Se lee la temperatura

  // Se guarda la velocidad y dirección de la ráfaga
  // Se actualiza con cada reinicio del sistema
//   if (millis() - lastTimeMoved < shakeTime) {
//        vibracion= true;
//    }
//  else { vibracion=false;
//  }
}

void writeSD() {

  lectura = SD.open("datos.csv", FILE_WRITE);

  DateTime now = rtc.now();
  if( lectura ){
    Serial.println("SD"); 
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
    // Escribe el valor de velocidad del viento con un espacio decimal
    
    // Escribe el valor de dirección del viento

    // Escribe el valor de velocidad de la ráfaga con un espa
    
    // Escribe el valor de dirección de la ráfaga con un espacio decimal
 
  }
  else{
    Serial.println("sin SD"); 
  }

  lectura.close();
}
