#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include "utility/socket.h"
#include "config.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
SPI_CLOCK_DIVIDER); // you can change this clock speed but DI

// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

// Wifi definitions
const unsigned long
    dhcpTimeout     = 20L * 1000L, // Max time to wait for address from DHCP
    connectTimeout  = 10L * 1000L, // Max time to wait for server connection
    responseTimeout = 2L * 1000L, // Max time to wait for data from server
    postingInterval = 3L * 1000L;  // Delay between posting data
unsigned long
    currentTime = 0L,
    lastConnectionTime = 0L;
Adafruit_CC3000_Client
  client;        // For WiFi connections
uint32_t ip = 0L, t;
char Wstr[15];
bool has_to_reconnect = false;

/* 

Send a value to a server

*/
bool sendValues(double temp, int W) {
  
  // Connect to numeric IP
  client = cc3000.connectTCP(ip, 80);

  if(client.connected()) { // Success!
    
    // Creating the message for POST
    char message[30 + 1] = "";
    char str_temp[4 + 1] = "";
    char str_power[5 + 1] = "";
    dtostrf(temp, 4, 1, str_temp);
    sprintf(str_power, "%d", W);
  
    Serial.print("Sending POST data... ");
    
    client.fastrprint(F("POST "));
    client.fastrprint(endpoint);
    client.fastrprint(F(" HTTP/1.1\r\n"));
    client.fastrprint(F("Host: "));
    client.fastrprint(host);
    client.fastrprint(F("\r\nConnection: close\r\n"
                       "Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n"
                       "Content-Length: "));
    client.print(encodedLength(str_temp) + encodedLength(str_power) + encodedLength(private_key) + 10);
    client.fastrprint(F("\r\n\r\nt="));
    urlEncode(client, str_temp, false, false);
    client.fastrprint(F("&w="));
    urlEncode(client, str_power, false, false);
    client.fastrprint(F("&key="));
    urlEncode(client, private_key, false, false);

    // Serial.print(F("OK\r\nAwaiting response..."));
    int c = 0;
    // Dirty trick: instead of parsing results, just look for opening
    // curly brace indicating the start of a successful JSON response.
    while(((c = timedRead()) > 0) && (c != '{'));
    if(c == '{') {
      debug(F("done."));
    } else if(c < 0) {
      matrix.print(0xEEEE, HEX);
      matrix.writeDisplay();
      debug(F("timeout."));
    } else {
      matrix.print(0xEBAD, HEX);
      matrix.writeDisplay();
      debug(F("error."));
    }
    client.close();
    return (c == '{');
  } else {
    client.close();
    matrix.print(0xFAC0, HEX);
    matrix.writeDisplay();
    debug(F("Failed to connect."));
    return false;
  }

}


// Read from client stream with a 5 second timeout.  Although an
// essentially identical method already exists in the Stream() class,
// it's declared private there...so this is a local copy.
int timedRead(void) {
  unsigned long start = millis();
  while((!client.available()) && ((millis() - start) < responseTimeout));
  return client.read();  // -1 on timeout
}

/*

Minimalist time server query; adapted from Adafruit Gutenbird sketch,
which in turn has roots in Arduino UdpNTPClient tutorial.

*/
/*
unsigned long getTime(void) {

  uint8_t       buf[48];
  unsigned long ip, startTime, t = 0L;

 debug(F("Locating time server..."));

  // Hostname to IP lookup; use NTP pool (rotates through servers)
  if(cc3000.getHostByName("pool.ntp.org", &ip)) {
    static const char PROGMEM
      timeReqA[] = { 227,  0,  6, 236 },
      timeReqB[] = {  49, 78, 49,  52 };

    debug(F("found\r\nConnecting to time server..."));
    startTime = millis();
    do {
      client = cc3000.connectUDP(ip, 123);
    } while((!client.connected()) &&
            ((millis() - startTime) < connectTimeout));

    if(client.connected()) {
      debug(F("connected!\r\nIssuing request..."));

      // Assemble and issue request packet
      memset(buf, 0, sizeof(buf));
      memcpy_P( buf    , timeReqA, sizeof(timeReqA));
      memcpy_P(&buf[12], timeReqB, sizeof(timeReqB));
      client.write(buf, sizeof(buf));

      debug(F("OK\r\nAwaiting response..."));
      memset(buf, 0, sizeof(buf));
      startTime = millis();
      while((!client.available()) &&
            ((millis() - startTime) < responseTimeout));
      if(client.available()) {
        client.read(buf, sizeof(buf));
        t = (((unsigned long)buf[40] << 24) |
             ((unsigned long)buf[41] << 16) |
             ((unsigned long)buf[42] <<  8) |
              (unsigned long)buf[43]) - 2208988800UL;
       debug(F("success!"));
      }
      client.close();
    }
  }
  if(!t) debug(F("error"));
  return t;
}
*/
// For URL-encoding functions below
static const char PROGMEM hexChar[] = "0123456789ABCDEF";

// URL-encoding output function for Print class.
// Input from RAM or PROGMEM (flash).  Double-encoding is a weird special
// case for Oauth (encoded strings get encoded a second time).
void urlEncode(
  Print      &p,       // EthernetClient, Sha1, etc.
  const char *src,     // String to be encoded
  boolean     progmem, // If true, string is in PROGMEM (else RAM)
  boolean     x2)      // If true, "double encode" parenthesis
{
  uint8_t c;

  while((c = (progmem ? pgm_read_byte(src) : *src))) {
    if(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) ||
       ((c >= '0') && (c <= '9')) || strchr_P(PSTR("-_.~"), c)) {
      p.write(c);
    } else {
      if(x2) p.print("%25");
      else   p.write('%');
      p.write(pgm_read_byte(&hexChar[c >> 4]));
      p.write(pgm_read_byte(&hexChar[c & 15]));
    }
    src++;
  }
}

// Returns would-be length of encoded string, without actually encoding
int encodedLength(char *src) {
  uint8_t c;
  int     len = 0;

  while((c = *src++)) {
    len += (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) ||
            ((c >= '0') && (c <= '9')) || strchr_P(PSTR("-_.~"), c)) ? 1 : 3;
  }

  return len;
}
