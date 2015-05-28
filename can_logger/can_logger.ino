


// demo: CAN-BUS Shield, receive data with interrupt mode
// when in interrupt mode, the data coming can't be too fast, must >20ms, or else you can use check mode
// loovee, 2014-6-13

#include <SD.h>
#include <SPI.h>
#include <Time.h>
#include <QueueArray.h>
#include "mcp_can.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
const int SD_CS_PIN = 9;
const int RX_LED_PIN = 8;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

unsigned char len = 0;
unsigned char buf[8];
char str[20];

struct Message
{
    int id;
    char data[8];
    String asciiData;
};

QueueArray <Message> commonQueue;
QueueArray <Message> current;

void clearSerial()
{
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command  
}

void setup()
{
    setTime(5, 0, 0, 9, 1, 1977);
    time_t time = now();
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
    Serial.println(time);

    Serial.print("Initializing SD card...");
    
    // see if the card is present and can be initialized:
    if (!SD.begin(SD_CS_PIN)) {
      Serial.println("Card failed, or not present");

    Serial.println("card initialized.");
    
    time_t time2 = now();
    Serial.println(time2);

      // don't do anything more:
      return;
    }

    attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
}

void MCP2515_ISR()
{
    digitalWrite(RX_LED_PIN, HIGH);

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
  
        // create log entry struct with current timestamp
        Message m = { canId, buf, String(s, HEX) };
  
        // is the message already in the common list
        
        // yes - remove the current entry and replace with new one
        
        // no - add to current list
  
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

void loop()
{
    // clear screen
    
    // print current list
    
    // loop through current list and expire old entries
    
    // loop through common list and expire old entries
  
  
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
