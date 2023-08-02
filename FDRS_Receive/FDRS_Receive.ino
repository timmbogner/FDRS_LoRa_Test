// #define RADIOLIB_MODULE SX1262  // Pico SX1262 (Waveshare)
// #define USE_SX126X
// #define LORA_SS 3
// #define LORA_RST 15
// #define LORA_DIO 20
// #define LORA_BUSY 2
// #define CUSTOM_SPI
// #define LORA_SPI_SCK 10
// #define LORA_SPI_MISO 12
// #define LORA_SPI_MOSI 11

// #define RADIOLIB_MODULE SX1262  // ESP32 SX1262 (Heltec)
// #define LORA_SS 8
// #define LORA_RST 12
// #define LORA_DIO 14
// #define LORA_BUSY 13
// #define USE_SX126X

// #define RADIOLIB_MODULE SX1276  // ESP32 SX1276 (TTGO LoRa32 V1)
// #define LORA_SS 18
// #define LORA_RST 14
// #define LORA_DIO 26
// #define LORA_BUSY RADIOLIB_NC

#define RADIOLIB_MODULE SX1278  // ESP8266 SX1278
#define LORA_SS    15
#define LORA_RST   4
#define LORA_DIO   5
#define LORA_BUSY  RADIOLIB_NC
//#define USE_SX126X

#define FDRS_LORA_TXPWR 17   // LoRa TX power in dBm (: +2dBm - +17dBm (for SX1276-7) +20dBm (for SX1278))
#define FDRS_LORA_FREQUENCY 433.0
#define FDRS_LORA_BANDWIDTH 125.0
#define FDRS_LORA_SF 7
#define FDRS_LORA_CR 5
#define FDRS_LORA_SYNCWORD 0x12

#include <RadioLib.h>

#ifdef CUSTOM_SPI
#ifdef ESP32
//SPIClass SPI(HSPI);
#endif  // ESP32
RADIOLIB_MODULE radio = new Module(LORA_SS, LORA_DIO, LORA_RST, LORA_BUSY, SPI);
#else
RADIOLIB_MODULE radio = new Module(LORA_SS, LORA_DIO, LORA_RST, LORA_BUSY);
#endif  // CUSTOM_SPI

void printLoraPacket(uint8_t *p, int size)
{
  printf("Printing packet of size %d.", size);
  for (int i = 0; i < size; i++)
  {
    if (i % 2 == 0)
      printf("\n%02d: ", i);
    printf("%02X ", p[i]);
  }
  printf("\n");
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  receivedFlag = true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, world!");


#ifdef CUSTOM_SPI
#ifdef ESP32
  SPI.begin(LORA_SPI_SCK, LORA_SPI_MISO, LORA_SPI_MOSI);
#endif  // ESP32
#ifdef ARDUINO_ARCH_RP2040
  SPI.setRX(LORA_SPI_MISO);
  SPI.setTX(LORA_SPI_MOSI);
  SPI.setSCK(LORA_SPI_SCK);
  SPI.begin(false);
#endif  //ARDUINO_ARCH_RP2040
#endif  // CUSTOM_SPI

#ifdef USE_SX126X
  int state = radio.begin(FDRS_LORA_FREQUENCY, FDRS_LORA_BANDWIDTH, FDRS_LORA_SF, FDRS_LORA_CR, FDRS_LORA_SYNCWORD, FDRS_LORA_TXPWR);
#else
  int state = radio.begin(FDRS_LORA_FREQUENCY, FDRS_LORA_BANDWIDTH, FDRS_LORA_SF, FDRS_LORA_CR, FDRS_LORA_SYNCWORD, FDRS_LORA_TXPWR, 8, 0);
#endif

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib initialization successful!");
  } else {
    Serial.println("RadioLib initialization failed, code " + String(state));
  }
#ifdef USE_SX126X
  radio.setDio1Action(setFlag);
#else
  radio.setDio0Action(setFlag, RISING);
#endif
  radio.setCRC(false);

  // start listening for LoRa packets
  Serial.print(F("[SX1278] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }
}


void loop() {
  // check if the flag is set
  if (receivedFlag) {
    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    // String str;
    // int state = radio.readData(str);

    // you can also read received data as byte array
      int packetSize = radio.getPacketLength();
      byte byteArr[255];
      int state = radio.readData(byteArr, packetSize);
      //printLoraPacket(byteArr, packetSize);


    if (state == RADIOLIB_ERR_NONE) {
      // packet was successfully received
      Serial.println(F("[SX1278] Received packet!"));

      // print data of the packet
      // Serial.print(F("[SX1278] Data:\t\t"));
      // Serial.println(str);

      // print RSSI (Received Signal Strength Indicator)
      Serial.print(F("[SX1278] RSSI:\t\t"));
      Serial.print(radio.getRSSI());
      Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[SX1278] SNR:\t\t"));
      Serial.print(radio.getSNR());
      Serial.println(F(" dB"));

  

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("[SX1278] CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("[SX1278] Failed, code "));
      Serial.println(state);
    }

    // put module back to listen mode
    //radio.startReceive();
  }
}