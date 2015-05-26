// demo: CAN-BUS Shield, receive data with interrupt mode
// when in interrupt mode, the data coming can't be too fast, must >20ms, or else you can use check mode
// loovee, 2014-6-13

#include <SD.h>
#include <SPI.h>
#include "mcp_can.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
const int SD_CS_PIN = 9;
const int RX_LED_PIN = 8;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

void clearSerial()
{
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command  
}

void setup()
{
    pinMode(RX_LED_PIN, OUTPUT);

    Serial.begin(115200);
    clearSerial();

    START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }

    Serial.print("Initializing SD card...");
    
    // see if the card is present and can be initialized:
    if (!SD.begin(SD_CS_PIN)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      return;
    }
    Serial.println("card initialized.");

    attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
}

void MCP2515_ISR()
{
    flagRecv = 1;
}

void loop()
{
    if(flagRecv) 
    {                                   // check if get data

        flagRecv = 0;                   // clear flag

        digitalWrite(RX_LED_PIN, HIGH);

        // iterate over all pending messages
        // If either the bus is saturated or the MCU is busy,
        // both RX buffers may be in use and reading a single
        // message does not clear the IRQ conditon.
        while (CAN_MSGAVAIL == CAN.checkReceive()) 
        {
            File dataFile = SD.open("can-data.txt", FILE_WRITE);
             
            // read data,  len: data length, buf: data buf
            CAN.readMsgBuf(&len, buf);
            int canId = CAN.getCanId();
            
            if (dataFile) {
              dataFile.print(canId);
              dataFile.print("\t");
            }

            // print the data
            for(int i = 0; i<len; i++)
            {
              
              // if the file is available, write to it:
              if (dataFile) {
                dataFile.print(buf[i]);
                dataFile.print("\t");
              }
               
              Serial.print(buf[i]);Serial.print("\t");
            }
            
            dataFile.println();
            dataFile.close();
            Serial.println();
        }
        
        digitalWrite(RX_LED_PIN, LOW);
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
