#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_INT 3
#define RFM95_CS 10
#define RFM95_RST 2

#define TXPWR 23
#define FREQ 868.0

#define SERVER_ADDRESS 1

RH_RF95 driver(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  if (manager.init())
  {
    if (!driver.setFrequency(FREQ))
      Serial.println("Unable to set RF95 frequency");
//    if (!driver.setModemConfig(RH_RF95::Bw500Cr45Sf128))
//      Serial.println("Invalid setModemConfig() option");
    driver.setTxPower(TXPWR, false);
    Serial.println("RF95 radio initialized.");
  }
  else
    Serial.println("RF95 radio initialization failed.");
  Serial.print("RF95 max message length = ");
  Serial.println(driver.maxMessageLength());
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  //Serial.print("in loop");
  // Wait for a message addressed to us from the client
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (manager.recvfromAck(buf, &len, &from))
  {
    Serial.print("got request from : 0x");
    Serial.print(from, HEX);
    Serial.print(": ");
    Serial.println((char*)buf);

    // Send a reply back to the originator client
    if (!manager.sendtoWait(data, sizeof(data), from))
    {
      Serial.println("sendtoWait failed");
    }
  }
}
