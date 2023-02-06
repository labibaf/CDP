#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLk7xRr6Wl"
#define BLYNK_DEVICE_NAME "MONITORING"
#define BLYNK_AUTH_TOKEN "eqHMz4nCryXFtvppXz54QBpfKKiDsNLU"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// dht11
#include <DHT.h>
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

BlynkTimer timer;

// water level
#define waterPin 35
#define powerPin 32
int sensorValue=0;
float tinggiAir=0;
int nilaiMax=1400;
float panjangSensor=4.0;

// LiquidCrystal I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// BH1750
#include <BH1750.h>
BH1750 bh1750;

// Load Cell
#include <HX711.h>
#define DATA_PIN  26
#define CLOCK_PIN 27
HX711 hx711;
volatile float units;

// relay
int relaysuhu=19;
int relaycahaya=18;
int relaywater=16;

// servo
#include <ESP32Servo.h>
#define pinServo 33
int pos=0;
Servo servo;
int i=0;

// timer
unsigned long TimerL;
unsigned long TimerAwal=0;
unsigned long IntervalTimer=1000;
unsigned long detik,menit,jam,hari;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Test";
char pass[] = "12121212";

int temperature,humidity;
float lux;

// ----------------------------------------

void DHT_Read(){
  temperature=dht.readTemperature();
  humidity=dht.readHumidity();

  Blynk.virtualWrite(V0,temperature);
  Blynk.virtualWrite(V1,humidity);
}

void WATER_Read(){
  digitalWrite(powerPin,HIGH);
  delay(10);
  sensorValue=analogRead(waterPin);
  digitalWrite(powerPin,LOW);
  tinggiAir=sensorValue*panjangSensor/nilaiMax;
  Blynk.virtualWrite(V3,tinggiAir);
}

void LUX_Read(){
  lux = bh1750.readLightLevel(); 
  Blynk.virtualWrite(V2,lux);
}

void HX711_Read(){
  units = hx711.get_units();
  Blynk.virtualWrite(V4,units);
}

void SendToBlynk(){
  DHT_Read();
  WATER_Read();
  LUX_Read();
  HX711_Read();
}

void LCD_WRITE(){
  lcd.setCursor(0, 0);
  lcd.print("Temp =");
  lcd.print(temperature);
  lcd.setCursor(8, 0);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Air  =");
  lcd.print(tinggiAir);
  lcd.setCursor(10, 1);
  lcd.print("cm");
  lcd.setCursor(11, 0);
  lcd.print("Humi =");
  lcd.print(humidity);
  lcd.setCursor(19, 0);
  lcd.print("%");
  lcd.setCursor(0,2);
  lcd.print("Lux  =");
  lcd.print(lux);
  lcd.setCursor(10,2);
  lcd.print("lx");
  lcd.setCursor(0, 3);
  lcd.print("Makanan =");
  lcd.print(units);
  lcd.setCursor(13, 3);
  lcd.print("kg");  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  dht.begin();

  pinMode(powerPin,OUTPUT);
  digitalWrite(powerPin, LOW);

  lcd.begin();
  Wire.begin(21,22);
  bh1750.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire);  

  hx711.begin(DATA_PIN, CLOCK_PIN);
  hx711.set_scale(-383490.00);
  hx711.tare();

  // relay
  pinMode(relaycahaya,OUTPUT);
  pinMode(relaysuhu,OUTPUT);
  pinMode(relaywater,OUTPUT);

  // servo
  servo.attach(pinServo);

  timer.setInterval(1000L,SendToBlynk);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();
  LCD_WRITE();
  Pewaktu();
  KondisiCahaya();
  KondisiSuhu();
  KondisiAir();
  KondisiBerat();
}

void Pewaktu(){
  TimerL = millis();
  if (TimerL - TimerAwal >= IntervalTimer)
  {
    TimerAwal = TimerL;
    detik - millis();
    detik++;
    
  }
  if (detik==60){
    detik = 0;
    menit++;
    
  }
  if (menit==60){
    menit = 0;
    jam++;
    
  }
  if (jam==24){
    jam = 0;
    hari++; 
  }
}

void KondisiSuhu(){
  int suhu=temperature;
  if(hari<=3){ //PEMBACAAN 1
    if(suhu >= 31 && suhu <= 33){ //PEMBACAAN 1
      digitalWrite(relaysuhu,HIGH);
    }
    else{
      digitalWrite(relaysuhu,LOW);
    }
  }
  else if(hari >=4 && hari <=7 ) //PEMBACAAN 2
  {
    if(suhu >= 31 && suhu <= 32) //PEMBACAAN 2
    {
    digitalWrite(relaysuhu,HIGH);
    }
  else{
    digitalWrite(relaysuhu,LOW);
  }
  }
  else if(hari >=8 && hari <=14 )//PEMBACAAN 3
  {
    if(suhu >= 28 && suhu <= 30) //PEMBACAAN 3
    {
    digitalWrite(relaysuhu,HIGH);
    }
  else{
    digitalWrite(relaysuhu,LOW);
    }
  }
  else if(hari >=15 && hari <=21) //PEMBACAAN 4
  {
   if(suhu >= 26 && suhu <= 28) //PEMBACAAN 4
    {
    digitalWrite(relaysuhu,HIGH);
    }
  else{
    digitalWrite(relaysuhu,LOW);
    }
  }
  else if(hari >=22 && hari >=98)//PEMBACAAN 5
  {
    if(suhu >=20 && suhu <= 25) //PEMBACAAN 5
  {
  digitalWrite(relaysuhu,HIGH);
  }
 else{
  digitalWrite(relaysuhu,LOW);
  }
 }
}

void KondisiCahaya(){
 if(hari<=3){ //PEMBACAAN 1
    if(jam >= 23){ //PEMBACAAN 1
      if(lux >= 20 && lux <= 40){
      digitalWrite(relaycahaya,HIGH);
    }
    else{
      digitalWrite(relaycahaya,LOW);
     }
    }
  }
  else if(hari >=4 && hari <=7 ) //PEMBACAAN 2
  {
    if(jam >= 22) //PEMBACAAN 2
    {
      if(lux >= 15 && lux <= 30){
    digitalWrite(relaycahaya,HIGH);
    }
  else{
    digitalWrite(relaycahaya,LOW);
    }
   }
  }
   else if(hari >=8 && hari <=14 ) //PEMBACAAN 3
  {
    if(jam >= 20) //PEMBACAAN 3
    {
      if(lux >= 10 && lux <= 20){
    digitalWrite(relaycahaya,HIGH);
    }
  else{
    digitalWrite(relaycahaya,LOW);
     }
    }
  }
 else if(hari >=15 && hari <=21 ) //PEMBACAAN 4
  {
    if(jam >= 18) //PEMBACAAN 4
    {
      if(lux >= 5 && lux <= 10){
    digitalWrite(relaycahaya,HIGH);
    }
  else{
    digitalWrite(relaycahaya,LOW);
    }
   }
  }
   else if(hari >=22 && hari <=24 ) //PEMBACAAN 5
  {
    if(jam >= 16) //PEMBACAAN 5
    {
      if(lux >= 5 && lux <= 10){
    digitalWrite(relaycahaya,HIGH);
    }
  else{
    digitalWrite(relaycahaya,LOW);
     }
    }
  }
   else if(hari >=25 && hari <=28 ) //PEMBACAAN 6
  {
    if(jam >= 14) //PEMBACAAN 6
    {
      if(lux >= 5 && lux <= 10){
    digitalWrite(relaycahaya,HIGH);
    }
  else{
    digitalWrite(relaycahaya,LOW);
   }
    }
  }
   else if(hari >=29 && hari <=35 ) //PEMBACAAN 7
  {
    if(jam >= 12) //PEMBACAAN 7
    {
      if (lux >= 5 && lux <= 10){
    digitalWrite(relaycahaya,HIGH);
    }
  else{
    digitalWrite(relaycahaya,LOW);
     }
    }
  }
   else if(hari >=36 && hari <=98 ) //PEMBACAAN 8
  {
    if(jam >= 10) //PEMBACAAN 8
    {
      if(lux >= 5 && lux <= 10){
    digitalWrite(relaycahaya,HIGH);
    }
  else{
    digitalWrite(relaycahaya,LOW);
     }
    }
  }
}


void KondisiAir(){
    if (tinggiAir <= 1) {                
      digitalWrite (relaywater,LOW);            
    }
     else if (tinggiAir >=4) {
      digitalWrite (relaywater,HIGH);
    }   
   
}

void KondisiBerat(){
if(hari<=7){
    i=0.22/3;
  if (jam==7&&jam==15&&jam==19){
        if (units <=i){
         servo.write(90);
         }
        else if (units >=i){
           servo.write(0);
           }
        }}
else if(hari<=14){
    i=0.34/3;
  if (jam==7&&jam==15&&jam==19){
       if (units <=i){
        servo.write(90);
       }
       else if (units >=i){
        servo.write(0);
        }
     }}
else if(hari<=21){
    i=0.44/3;
  if (jam==7&&jam==15&&jam==19){
        if (units <=i){
        servo.write(90);
      }
        else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=28){
   i=0.56/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
      servo.write(0);
      }
    }}
else if(hari<=35){
    i=0.7/3;
 if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=42){
    i=0.82/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=49){
    i=0.94/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }} 
else if(hari<=56){
    i=1.02/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      } 
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=63){
    i=1.1/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=70){
    i=1.16/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=77){
    i=1.2/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=84){
    i=1.28/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=91){
    i=1.3/3;
  if (jam==7&&jam==15&&jam==19){
       if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=98){
    i=1.36/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
else if(hari<=105){
    i=1.4/3;
  if (jam==7&&jam==15&&jam==19){
      if (units <=i){
        servo.write(90);
      }
      else if (units >=i){
        servo.write(0);
      }
    }}
}


