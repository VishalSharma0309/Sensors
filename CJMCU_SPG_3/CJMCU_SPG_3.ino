#include <Arduino.h>

#include "sensirion_common.h"
#include "sgp30.h"



#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "Algo8.ai";  //  your network SSID (name)
const char pass[] = "stockbay1234";       // your network password

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)


WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);


#include <time.h>
#include <math.h>

#define TIME_MSG_LEN 11 // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER 'T' // Header tag for serial time sync message
#define TIME_REQUEST 7 // ASCII bell character requests a time sync message


/*#define LIGHT_SENSOR A0//Grove - Light Sensor is connected to A0 of Arduino
const int ledPin=12;                 //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue=10;         //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
float Rsensor; //Resistance of sensor in K*/


#include "HDC1080JS.h"
#include "Wire.h"


#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

#include <WiFiClient.h>;

#include <ThingSpeak.h>;

WiFiClient client;

unsigned long myChannelNumber = 799923; //Your Channel Number (Without Brackets)

const char * myWriteAPIKey = "VQSO57IJ51VMS51Q"; //Your Write API Key




HDC1080JS tempsensor;
Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void setup() {
  Serial.begin(115200);
  
  //Serial.println("CCS811 Reading CO2 and VOC");
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }
  //calibrate temperature sensor
  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);

  //Serial.println(F("BMP280 test"));

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); 
  Wire.begin();
  Wire.setClock(400000); //set clock speed for I2C bus to maximum allowed for HDC1080
  //Serial.begin(9600);  // start serial for output

  tempsensor = HDC1080JS();
  tempsensor.config();

  //pinMode(ledPin,OUTPUT);            //Set the LED on Digital 12 as an OUTPUT
  //Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);
  //Serial.println("TimeNTP Example");
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  ThingSpeak.begin(client);

  //Serial.print("IP number assigned by DHCP is ");
  //Serial.println(WiFi.localIP());
  //Serial.println("Starting UDP");
  Udp.begin(localPort);
  //Serial.print("Local port: ");
  //Serial.println(Udp.localPort());
  //Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;
  Serial.begin(115200);
  Serial.println("serial start!!");

 /*For wio link!*/
  #if defined(ESP8266)
          pinMode(15,OUTPUT);
          digitalWrite(15,1);
          Serial.println("Set wio link power!");
          delay(500);
  #endif
  /*Init module,Reset all baseline,The initialization takes up to around 15 seconds, during which
all APIs measuring IAQ(Indoor air quality ) output will not change.Default value is 400(ppm) for co2,0(ppb) for tvoc*/
  while (sgp_probe() != STATUS_OK) {
         Serial.println("SGP failed");
         while(1);
    }
    /*Read H2 and Ethanol signal in the way of blocking*/
    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                            &scaled_h2_signal);
    if (err == STATUS_OK) {
        Serial.println("get ram signal!");
    } else {
         Serial.println("error reading signals"); 
    }
     err = sgp_iaq_init();
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop() {
  //Serial.print(time.now());
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
    }
  }
  
  Serial.print(",");
  if(ccs.available()){
    float temp = ccs.calculateTemperature();
    if(!ccs.readData()){
      Serial.print(String(ccs.geteCO2())+","+String(ccs.getTVOC())+","+String(temp));
    }
    else{
      Serial.println("Sensor read ERROR!");
      ccs.readData();
    }
  }
    Serial.print(",");
   //Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    //Serial.println(" *C");

    Serial.print(",");
    
    //Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    //Serial.println(" Pa");

    //Serial.print(F("Approx altitude = "));
    //Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    //Serial.println(" m");

    //Serial.println();
    tempsensor.readTempHumid();
  
  float temp = tempsensor.getTemp();
  float humid = tempsensor.getRelativeHumidity();

  //Serial.print("T=");
  Serial.print(",");
  Serial.print(temp);
  //Serial.print("C, RH=");
  Serial.print(",");
  Serial.print(humid);


  Serial.print(",");
  //Serial.println("%");
  //Serial.print(",");
    //int sensorValue = analogRead(LIGHT_SENSOR); 
    //Rsensor = (float)(1023-sensorValue)*10/sensorValue;
    //Serial.println("the analog read data is ");
    //Serial.println(sensorValue);
    //Serial.println("the sensor resistance is ");
    //Serial.println(Rsensor,DEC);//show the ligth intensity on the serial monitor;
    //Serial.print(",");
    //Serial.println(time(void*start));
    s16 err=0;
  u16 tvoc_ppb, co2_eq_ppm;
  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
        if (err == STATUS_OK) {
             //Serial.print("tVOC  Concentration:");
             Serial.print(tvoc_ppb);
             //Serial.println("ppb");

             Serial.print(",");
              //Serial.print("CO2eq Concentration:");
              Serial.println(co2_eq_ppm);
               //Serial.println("ppm");
        } else {
             Serial.println("error reading IAQ values\n"); 
        }
    
    //ThingSpeak.setField(1, digitalClockDisplay());
ThingSpeak.setField(2, String(ccs.geteCO2()));
ThingSpeak.setField(3, bmp.readPressure());
ThingSpeak.setField(4, bmp.readTemperature());
ThingSpeak.setField(5, humid);
ThingSpeak.setField(6, String(ccs.getTVOC()));
ThingSpeak.setField(7, co2_eq_ppm);
ThingSpeak.setField(8, tvoc_ppb);
ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    delay(2000);
  //delay(500);
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  //Serial.println();
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  //Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  //Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  //Serial.print(ntpServerName);
  //Serial.print(": ");
  //Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      //Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
