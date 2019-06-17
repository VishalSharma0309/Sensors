# Sensors

1. Gravity BME 680
  >Parameters calculated: 
  >Library (Arduino IDE): https://github.com/DFRobot/DFRobot_BME680 
  >Datasheet: https://media.digikey.com/pdf/Data%20Sheets/DFRobot%20PDFs/SEN0248_Web.pdf 
  
2. CJMCU 8128 (integrated sensor)
    2.1 Bosch BMP 280
      >Parameters caluculated:
      >Library (Arduino IDE): https://github.com/adafruit/Adafruit_BMP280_Library
      >Datasheet: https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf
      
     2.2 T.I. HDC 1080
      >Parameters calculated:
      >Library (Arduino IDE): https://github.com/closedcube/ClosedCube_HDC1080_Arduino
      >Datasheet: http://www.ti.com/lit/ds/symlink/hdc1080.pdf
      
     2.3 AMS CCS811
      >Parameters calculated:
      >Library (Arduino IDE): https://github.com/adafruit/Adafruit_CCS811
                             >https://github.com/sparkfun/SparkFun_CCS811_Arduino_Library
      >Datasheet: https://cdn.sparkfun.com/assets/learn_tutorials/1/4/3/CCS811_Datasheet-DS000459.pdf
      
      
3. Grove SPG 30
  >Parameters calculated:
  >Library (Arduino IDE): https://github.com/Seeed-Studio/SGP30_Gas_Sensor (preferred)
                        > https://github.com/adafruit/Adafruit_SGP30
  >Datasheet:
  
4. Grove LDR
  >Parameters calculated:
  >Library (Arduino IDE):
  >Datasheet:
  
  
# Other Important Libraries

1. Time library


## Arduino IDE installation for Linux 
Warning: DON'T USE 'sudo apt-get install arduino'.
Follow the steps mentioned here: https://www.arduino.cc/en/Guide/Linux 

Some important steps to remember:
1. Run 'arduino-linux-setup.sh'.
2. Reboot system.
3. Run 'install.sh'.
