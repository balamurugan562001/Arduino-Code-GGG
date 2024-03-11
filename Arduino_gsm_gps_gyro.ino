#include<Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Configure software serial port
SoftwareSerial SIM800L(7, 8); 
SoftwareSerial GPS_SoftSerial(4, 3);
TinyGPSPlus gps;

const int MPU_addr=0x68;
int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
volatile float minutes, seconds;
volatile int degree, secs, mins;
int minVal=225;
int maxVal=1000;
 
double x;
double y;
double z;
 
void setup()
{
SIM800L.begin(19200);
Wire.begin();
Wire.beginTransmission(MPU_addr);
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission(true);
Serial.begin(9600);
GPS_SoftSerial.begin(9600);
}
void loop(){
Wire.beginTransmission(MPU_addr);
Wire.write(0x3B);
Wire.endTransmission(false);
Wire.requestFrom(MPU_addr,14,true);
AcX=Wire.read()<<8|Wire.read();
AcY=Wire.read()<<8|Wire.read();
AcZ=Wire.read()<<8|Wire.read();
int xAng = map(AcX,minVal,maxVal,-90,90);
int yAng = map(AcY,minVal,maxVal,-90,90);
int zAng = map(AcZ,minVal,maxVal,-90,90);
 
x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
 
Serial.print("AngleX= ");
Serial.println(x);
 
Serial.print("AngleY= ");
Serial.println(y);
 
Serial.print("AngleZ= ");
Serial.println(z);
Serial.println("-----------------------------------------");
delay(400);
smartDelay(1000);
  unsigned long start;
  double lat_val, lng_val, alt_m_val;
  bool loc_valid, alt_valid;
  lat_val = gps.location.lat(); 
  loc_valid = gps.location.isValid(); 
  lng_val = gps.location.lng();
  alt_m_val = gps.altitude.meters(); 
  alt_valid = gps.altitude.isValid(); 

  if (!loc_valid)
  {
    Serial.print("Latitude : ");
    Serial.println("*****");
    Serial.print("Longitude : ");
    Serial.println("*****");
    delay(4000);
  }
  else
  {
    Serial.println("GPS READING: ");

    Serial.print("Latitude in Decimal Degrees : ");
    Serial.println(lat_val, 6);


    Serial.print("Longitude in Decimal Degrees : ");
    Serial.println(lng_val, 6);
    delay(4000);
  }
if(z<300 && z>60)
{
  //sendSMS("+919345466434", "Alert");
  SIM800L.print("AT+CMGF=1\r");
  delay(100);
  
  SIM800L.print("AT+CMGS=\"");
  SIM800L.print("+919345466434");
  SIM800L.println("\"");
  delay(100);
  
  SIM800L.print("Alert");
  SIM800L.print(lat_val);
  SIM800L.print(lng_val);
  delay(100);
  
  SIM800L.write(0x1A); // Ctrl+Z
  delay(100);
}
}
/*void sendSMS(const char* phoneNumber, const char* message)
{
  SIM800L.print("AT+CMGF=1\r");
  delay(100);
  
  SIM800L.print("AT+CMGS=\"");
  SIM800L.print(phoneNumber);
  SIM800L.println("\"");
  delay(100);
  
  SIM800L.print(message);
  SIM800L.print(lat_val);
  SIM800L.print(lng_val);
  delay(100);
  
  SIM800L.write(0x1A); // Ctrl+Z
  delay(100);
}*/

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (GPS_SoftSerial.available()) 
    gps.encode(GPS_SoftSerial.read());
  } while (millis() - start < ms);
}
