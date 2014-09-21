// Pin used to sense current
#define CT_PIN A0
// Pin used to sense temperature
#define T_PIN A2

#define MAX_CURRENT 15.0 // 60 A / Abonnement EDF 3kW
#define VOLTAGE 230.0 // 230 V
#define NB_TURNS 2000 // 2000 turns, ratio 1:2000
#define I_CAL 8.51 // Calibration I
#define I_OFFSET 0.02 
// WHERE calibration_value = ( i(measured = max*sqrt(2) / i(sensor = measured/2000) ) / R(burden)

const unsigned long samplingInterval = 2L * 1000L; //delay between samples

// We need to declare that global
int lastSampleI, sampleI;
double lastFilteredI, filteredI, sumI,sqI,Irms;

/*
  Calculates sampled I at RMS for a number of samples
*/
double calcIrms(int NUMBER_OF_SAMPLES) {

  // We need a correct voltage reference
  int SUPPLYVOLTAGE = readVcc();

  sumI = 0;
  
  for (int n = 0; n < NUMBER_OF_SAMPLES; n++)
  {
    // Store previous values
    lastSampleI = sampleI;
    lastFilteredI = filteredI;

    // Sample
    sampleI = analogRead(CT_PIN);
    
    // Apply a digital high pass filters to remove 2.5V DC offset (centered on 0V).
    filteredI = 0.996*(lastFilteredI + sampleI - lastSampleI);

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum 
    sumI += sqI;
  }

  double I_RATIO = I_CAL *((SUPPLYVOLTAGE/1000.0) / (ADC_COUNTS));
  Irms = max(0, I_RATIO * sqrt(sumI / NUMBER_OF_SAMPLES) - I_OFFSET); 

  return Irms;
}

/*
  Gets temperature for a number of samples
*/
double getTemperature(int NUMBER_OF_SAMPLES) {

  // We need a correct voltage reference
  int SUPPLYVOLTAGE = readVcc();

  unsigned long sumT =0;
  int sampleT = 0;

  for (int n = 0; n < NUMBER_OF_SAMPLES; n++)
  {
    // Sample
    sampleT = analogRead(T_PIN);
    sumT += sampleT;
  }

  double T_RATIO = ((SUPPLYVOLTAGE/1000.0) / (ADC_COUNTS));
  // MCP 9700 : 500mV = 0°C and 10mV / °C (http://ww1.microchip.com/downloads/en/DeviceDoc/21942e.pdf)
  double Temp = (T_RATIO * sumT / NUMBER_OF_SAMPLES - 0.5)/0.01; 

  return Temp;
}

