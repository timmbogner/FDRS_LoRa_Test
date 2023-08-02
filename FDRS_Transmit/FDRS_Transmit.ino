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

// #define RADIOLIB_MODULE SX1262  // T3S3
// #define USE_SX126X
// #define LORA_SS 7
// #define LORA_RST 8
// #define LORA_DIO 33
// #define LORA_BUSY 34
// #define CUSTOM_SPI
// #define LORA_SPI_SCK 5
// #define LORA_SPI_MISO 3
// #define LORA_SPI_MOSI 6


#define RADIOLIB_MODULE SX1276  // ESP32 SX1276 (TTGO LoRa32 V1)
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO 26
#define LORA_BUSY RADIOLIB_NC

#define FDRS_LORA_TXPWR 17

#define FDRS_LORA_FREQUENCY 915.0
#define FDRS_LORA_BANDWIDTH 125.0
#define FDRS_LORA_SF 7
#define FDRS_LORA_CR 5
#define FDRS_LORA_SYNCWORD 0x12

#define RADIOLIB_DEBUG

#include <RadioLib.h>

#ifdef CUSTOM_SPI
#ifdef ESP32
//SPIClass SPI(HSPI);
#endif  // ESP32
RADIOLIB_MODULE radio = new Module(LORA_SS, LORA_DIO, LORA_RST, LORA_BUSY, SPI);
#else
RADIOLIB_MODULE radio = new Module(LORA_SS, LORA_DIO, LORA_RST, LORA_BUSY);
#endif  // CUSTOM_SPI



void setup() {
  Serial.begin(115200);
  //  delay(5000);
  Serial.println("Hello, world!");


#ifdef CUSTOM_SPI
#ifdef ESP32
  SPI.begin(LORA_SPI_SCK, LORA_SPI_MISO, LORA_SPI_MOSI);
#endif  // ESP32
#ifdef ARDUINO_ARCH_RP2040
  SPI1.setRX(LORA_SPI_MISO);
  SPI1.setTX(LORA_SPI_MOSI);
  SPI1.setSCK(LORA_SPI_SCK);
  SPI1.begin(false);
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
  radio.setCRC(false);


}

void loop() {
  Serial.print(F("[SX1278] Transmitting packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  // NOTE: transmit() is a blocking method!
  //       See example SX127x_Transmit_Interrupt for details
  //       on non-blocking transmission method.
  int state = radio.startTransmit("Hello World!");

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    int state = radio.transmit(byteArr, 8);
  */

  if (state == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F(" success!"));

  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }

  // wait for a second before transmitting again
  delay(1000);
}
