#include "TinyWireS.h"

/*
  ATtiny85 I2C Firmware for Sharp gp2y1010 Dust Sensor

  (C) Ricardo Carrasco Cursach - 2014


  This firmware returns a IEEE-754 compilant float value corresponding to the analog reading of
  the Sharp PG2Y1010 Dust Sensor.
  
  It must receive a Ox00 byte as command to start reading, it will return 4 bytes after at least
  more than 900 msec in little-endian order.
  
  The result is the average of at least 88 Samples taken in 900 msecs
  
                     ----
  RST             --|°   |--    VIN
  DUST_READ / A3  --|    |--    SCL
  DUST_LED  / 4   --|    |--    1 / LED_PIN
  GND             --|    |--    SDA
                     ----
  
  Based on works by:  
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  * Sharp sensor reading based on examples by Chris Nafis (C) 2012
    http://www.howmuchsnow.com/arduino/airquality/

  * USIi2c Library "TinyWireS" based on a Don Blake's post at AVR freaks. 2011 BroHogan - brohoganx10 at gmail dot com
  
  * arduino-tiny core Libraries
    https://code.google.com/p/arduino-tiny/
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
*/


#define I2C_SLAVE_ADDR  0x25  // i2c slave address

#define DUST_OUT       A3     // Dust sensor output on pin 2 (ADC3)
#define DUST_LED        4     // Dust LED control on pin 3 (PB4)
#define LED_PIN         1     // Led Indicator

#define M_SECS          900   // Time to be consumed on reading sensor
                              // (1000 mSec ~ 99 Samples with Arduino UNO @16 MHz)

void setup(){
  pinMode(DUST_LED,OUTPUT);
  pinMode(DUST_OUT,INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode
}

/*
  void loop()
  
  Checks if an I2C command is received.
  
  On command 0x00 will call getReading for M_SECS time
  and then send the obtained value as a little-endian
  single presicion float number representing Volts taken
  from the analog input through the I2C Bus with 4 Bytes.
*/

void loop(){
  if (TinyWireS.available()){
    byte command = TinyWireS.receive();
    switch(command){
      case 0x00:
        
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        digitalWrite(LED_PIN, HIGH);
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        
        // Reading Value in Volts
        float f = getReading(M_SECS);
        
        // Sending float as bytes
        TinyWireS.send( *((uint8_t *)&f ) );
        TinyWireS.send( *((uint8_t *)&f + 1) );
        TinyWireS.send( *((uint8_t *)&f + 2) );
        TinyWireS.send( *((uint8_t *)&f + 3)  );
        
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        digitalWrite(LED_PIN, LOW);
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        
      break;
    }
  }
}


/*
  float getReading(int msec)
  
  Return the value in volts of all the samples taken in the given milliseconds
  1000 msec gives arround 99 samples on arduino UNO @ 16 MHz
  
*/
float getReading(int msec){

  int delayTime=280;
  int delayTime2=40;
  int offTime=9680;
  
  boolean keep_reading = true;
  int i = 0;
  long startTime = millis(); 
  long data = 0;
 
  // Keep reading until time is out
  while(keep_reading){
    
    i=i+1; // Count Samples
    
    digitalWrite(DUST_LED,LOW); // power on the LED
    delayMicroseconds(delayTime);
    
    data += analogRead(DUST_OUT); // read the dust value
    
    delayMicroseconds(delayTime2);
    digitalWrite(DUST_LED,HIGH); // turn the LED off
    delayMicroseconds(offTime);

    if((millis() - startTime > msec)) { keep_reading = false; }
  }

  // Divide the sum of values by the number of samples (average)
  // Then convert the given value to volts and returns.
  return ( (float)(data/i) * 5.0 ) / 1024.0;
}

