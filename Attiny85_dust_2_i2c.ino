#include "TinyWireS.h"

/*
  i2cset -y 0 0x25 0x00; sleep 1; i2cget -y 0 0x25 0x00; i2cget -y 0 0x25 0x00; i2cget -y 0 0x25 0x00; i2cget -y 0 0x25 0x00

  0x00
  0xc0
  0x3c
  0x40

  0x403cc000
  2.9492188 <--- IEEE 754 Float http://www.h-schmidt.net/FloatConverter/IEEE754.html
  
                     ----
  RST             --|°   |--    VIN
  DUST_READ / A3  --|    |--    SCL
  DUST_LED  / 4   --|    |--    1 / LED_PIN
  GND             --|    |--    SDA
                     ----
  
*/


#define I2C_SLAVE_ADDR  0x25 // i2c slave address

#define DUST_READ       A3  // Dust read on pin 2 (ADC3)
#define DUST_LED         4  // Dust LED control on pin 3 (PB4)

#define DELAY_TIME1     28  // Wait 28 us before read sensor value (cf : datasheet)
#define DELAY_TIME2     10  // Wait 10 ms before reread sensor value (cf : datasheet)

#define SAMPLES         40  // How many samples to take

#define LED_PIN         1

int readings[SAMPLES];

void setup(){
  pinMode(DUST_LED,OUTPUT);
  pinMode(DUST_READ,INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode
}

void loop(){
  if (TinyWireS.available()){
    byte command = TinyWireS.receive();
    switch(command){
      case 0x00:
        
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        digitalWrite(LED_PIN, HIGH);
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        
        // Take [SAMPLE] Times samples and push them to array
        for(int i=0; i<SAMPLES; i++){
          digitalWrite(DUST_LED,LOW);     //++
          delayMicroseconds(DELAY_TIME1); //++
          readings[i] = analogRead(DUST_READ);
          digitalWrite(DUST_LED,HIGH);    //++
          delay(DELAY_TIME2);             //++
        }


        //------------------------------------//        
        //Cutting edges -10 ... 10+ over 40
        boolean clean = true;        
        for(int i=0; i<SAMPLES-1; i++){
          clean = true;
          for(int j=1; j<SAMPLES; j++){
            if(readings[i] > readings[j]){
              int aux = readings[j];
              readings[j] = readings[i];
              readings[i] = aux;
              clean = false;
            }
          }
          if(clean){ break; }
        }

        float r = 0.0;
        for(int i=10; i<SAMPLES-10; i++){
          r += readings[i];
        }
        r = r / (SAMPLES-20);
        
        //------------------------------------//
        
        // Obtaining Arithmetic Media on variable r
        /*
        float r = 0.0;
        for(int i=0; i<SAMPLES; i++){
          r += readings[i];
        }
        r = r / SAMPLES;
        */
        
        // Value to Volts
        float f = 0.00000000;
        f = (r * 5.0)/1024.0;
        // Sending float as bytes
        TinyWireS.send( *((uint8_t *)&f + 3) );
        TinyWireS.send( *((uint8_t *)&f + 2) );
        TinyWireS.send( *((uint8_t *)&f + 1) );
        TinyWireS.send( *((uint8_t *)&f)  );
        
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        digitalWrite(LED_PIN, LOW);
        // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
        
      break;
    }
  }
}

