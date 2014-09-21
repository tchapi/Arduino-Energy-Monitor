void loop() {

  /**************************************/
  /*              SAMPLING              */
  /**************************************/
  strip.setPixelColor(1,255,255,255);
  strip.show();

  double Irms = calcIrms(1480);  // Calculate Irms only
  int W = (int)(Irms*VOLTAGE);
  itoa(W, Wstr,10); // Copies the value in a string for sending the value

  double temp = getTemperature(4000);  // Calculate T

  if (DEBUG) { 
    Serial.print("Apparent Instant Power : ");
    Serial.print(W);  // Apparent power
    Serial.print(" W (");
    Serial.print(Irms);  // Irms
    Serial.println("A) ");

    Serial.print("Apparent Temperature : ");
    Serial.print(temp);  // Apparent temp
    Serial.println("C");
  }
  
  // Not sampling anymore
  strip.setPixelColor(1,0,0,0);
  strip.show();
  
  if (first_loop == false) { // First loop, we skip displaying
    
    /**************************************/
    /*          DISPLAYING POWER          */
    /**************************************/
    // We got 6 LEDS so let's compute the number that we need to lit
    if (W < 2) {
      nb_leds = 0;
    } else {
      nb_leds = ceil(W / (MAX_CURRENT * VOLTAGE) * 6); 
    }
    
    // lit them !
    for(int k=2;k<8; k++) {
      if (k<nb_leds + 2) {// + 2 since two leds are for wifi and sampling
        strip.setPixelColor(k,colors[k]);
      }  else {
        strip.setPixelColor(k,0,0,0);
      }
    }
    
    strip.show();
  
    /**************************************/
    /*          DISPLAYING TEMP           */
    /**************************************/
    // Round up
    int temp_display = floor(temp*10);
    // 4 digit display
    matrix.writeDigitNum(1, (temp_display % 1000) / 100);
    matrix.writeDigitNum(3, (temp_display % 100) /10, true);
    matrix.writeDigitNum(4, temp_display % 10);
    
    // We need to call this to display changes in the strip and the matrix:
    matrix.writeDisplay();
  
    /**************************************/
    /*               SENDING              */
    /**************************************/
    strip.setPixelColor(0,255,255,255);
    strip.show();
    
    // if ten seconds have passed since your last connection,
    // then connect again and send data:
  /*  if (millis() - lastConnectionTime > postingInterval) {
      sendValue(Wstr);
      //blinkLed(LED_WIFI);
      lastConnectionTime = millis();
    }
  */
    strip.setPixelColor(0,0,0,0);
    strip.show();

  }
  
  /* DELAY FOR SAMPLING */
  delay(samplingInterval);
  first_loop = false;

}

