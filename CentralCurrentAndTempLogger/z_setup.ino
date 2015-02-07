void setup() {

  Serial.begin(9600); 

  debug(F("-- Current and Temperature Sensor --")); 

  pinMode(CT_PIN,INPUT);
  pinMode(T_PIN,INPUT);
 
  matrix.begin(0x70);
  matrix.setBrightness(0);
  matrix.println(); // starts with nothing
  matrix.writeDisplay();
  
  strip.begin();
  strip.setPixelColor(0, white); // SAMPLING
  strip.setPixelColor(1, white); // WIFI
  strip.setPixelColor(2, blue);
  strip.setPixelColor(3, turquoise);
  strip.setPixelColor(4, green);
  strip.setPixelColor(5, yellow);
  strip.setPixelColor(6, orange);
  strip.setPixelColor(7, red);
  strip.setBrightness(0); // Initialize all pixels to no color
  strip.show(); // Initialize all pixels to no color

//  if (DEBUG) { 
//    // In the primary coil
//    float peak_current_primary = MAX_CURRENT * sqrt(2); // in A
//
//    // So in the secondary coil :
//    float peak_current_secondary = peak_current_primary / NB_TURNS; // in A
//
//    // Arduino running at 5V (?), so :
//    int SUPPLYVOLTAGE = readVcc(); 
//    float burden_resistor_value = ((SUPPLYVOLTAGE/1000.0) / 2) / peak_current_secondary; // in Ohm
//
//    Serial.print(" Peak Current (primary) [A]: " );
//    Serial.println(peak_current_primary);
//    Serial.print(" Peak Current (secondary) [A]: " );
//    Serial.println(peak_current_secondary);
//    Serial.print(" Internal Supply Voltage [V]: ");
//    Serial.println(SUPPLYVOLTAGE/1000.0);
//    Serial.print(" Burden Resistor [Ohm]: ");
//    Serial.println(burden_resistor_value); // 235,70 Ohm ?
//    Serial.println("");
//
//  }

  setup_wifi();

  strip.setBrightness(2);
  strip.show();
  
}

bool setup_wifi() {
  // Initialize the CC3000 
  if(!cc3000.begin()) {
    matrix.print(0xE, HEX);
    matrix.writeDisplay();
    hang(F("Unable to initialize the Wifi module."));
  }

  // debug(F("Deleting old connection profiles ... "));
  if(!cc3000.deleteProfiles()) {
    matrix.print(0xEE, HEX);
    matrix.writeDisplay();
    hang(F("Unable to delete profiles."));
  }

  // Attempting to connect to an access point 
  if(!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    matrix.print(0xEEE, HEX);
    matrix.writeDisplay();
    hang(F("Connection to WPA failed."));
  }

  for(t=millis(); !cc3000.checkDHCP() && ((millis() - t) < dhcpTimeout); delay(100));
  if(!cc3000.checkDHCP()) {
    matrix.print(0xEEEE, HEX);
    matrix.writeDisplay();
    hang(F("Could not obtain an IP."));
  }
  
  // Here, we are connected and ready to roll !
  
  t = millis();
  while((0L == ip) && ((millis() - t) < connectTimeout)) {
    if(cc3000.getHostByName((char *)host, &ip)) break;
    delay(1000);
  }
  if(0L == ip) {
    hang(F("Failed to retrieve host IP"));
  }
 
}

