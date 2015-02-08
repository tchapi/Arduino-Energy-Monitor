void loop() {

  /**************************************/
  /*              SAMPLING              */
  /**************************************/
  strip.setPixelColor(1,255,255,255);
  strip.show();

  double Irms = calcIrms(1480);  // Calculate Irms only
  int W = (int)(Irms*VOLTAGE);

  double temp = getTemperature(5000);  // Calculate T
// 
//  if (DEBUG) { 
//    Serial.print("Apparent Instant Power : ");
//    Serial.print(W);  // Apparent power
//    Serial.print(" W (");
//    Serial.print(Irms);  // Irms
//    Serial.println("A) ");
//
//    Serial.print("Apparent Temperature : ");
//    Serial.print(temp);  // Apparent temp
//    Serial.println("C");
//  }
  
  // Not sampling anymore
  strip.setPixelColor(1,0,0,0);
  strip.show();
  
  if (first_loop == false) { // On the first loop, we skip displaying
    
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
    
    matrix.clear(); // Clears just in case

    // Round up
    int temp_display = floor(temp*10);
    // 4 digit display
    matrix.writeDigitNum(1, (temp_display % 1000) / 100);
    matrix.writeDigitNum(3, (temp_display % 100) /10, true);
    matrix.writeDigitNum(4, temp_display % 10);
    
    // We need to call this to display changes in the matrix:
    matrix.writeDisplay();
  
    /**************************************/
    /*               SENDING              */
    /**************************************/

    // if ten seconds have passed since your last connection,
    // then connect again and send data:
    if (millis() - lastConnectionTime > postingInterval || millis() < lastConnectionTime /* millis() will overflow after 50days approx */) {
      strip.setPixelColor(0,255,255,255);
      strip.show();
      bool result = sendValues(temp, W); // Actually send to the server
    
      // Display a different color depending on the result
      strip.setPixelColor(0,(result == false)?red:green);
      strip.show();
      lastConnectionTime = millis();
      delay(250);
      
      if (result == false) {
        // Trying to reconnect
        strip.setPixelColor(0,orange);
        strip.show();
        setup_wifi();
        lastConnectionTime = millis();
      }
      
      // Led off
      strip.setPixelColor(0,0,0,0);
      strip.show();
    }

  } else {
    strip.setPixelColor(1,green);
    strip.show();
  }
  
  /* DELAY FOR SAMPLING */
  delay(samplingInterval);
  first_loop = false;

}

