
#include <Arduino.h>
    int decimalPrecision = 2;               // decimal places for all values shown in LED Display & Serial Monitor

    
    int ThermistorPin = A2;                 // The analog input pin for measuring temperature
    float voltageDividerR1 = 10000;         // Resistor value in R1 for voltage devider method 
    float BValue = 3470;                    // The B Value of the thermistor for the temperature measuring range
    float RT1 = 10000;                        // Thermistor resistor rating at based temperature (25 degree celcius)
    float T1 = 298.15;                      /* Base temperature T1 in Kelvin (default should be at 25 degree)*/
    float R2 ;                              /* Resistance of Thermistor (in ohm) at Measuring Temperature*/
    float T2 ;                              /* Measurement temperature T2 in Kelvin */
    
    float a ;                               /* Use for calculation in Temperature*/
    float b ;                               /* Use for calculation in Temperature*/
    float c ;                               /* Use for calculation in Temperature*/
    float d ;                               /* Use for calculation in Temperature*/
    float e = 2.718281828 ;                 /* the value of e use for calculation in Temperature*/
    
    float tempSampleRead  = 0;               /* to read the value of a sample including currentOffset1 value*/
    float tempLastSample  = 0;               /* to count time for each sample. Technically 1 milli second 1 sample is taken */
    float tempSampleSum   = 0;               /* accumulation of sample readings */
    float tempSampleCount = 0;               /* to count number of sample. */
    float tempMean ;                         /* to calculate the average value from all samples, in analog values*/ 





float readTemp(){
    /* 1- Temperature Measurement */
    while(true){
    float temp;
    if(millis() >= tempLastSample + 1)                                                                  /* every 1 milli second taking 1 reading */
        {
            tempSampleRead = analogRead(ThermistorPin);                                                 /* read analog value from sensor */
            tempSampleSum = tempSampleSum+tempSampleRead;                                               /* add all analog value for averaging later*/
            tempSampleCount = tempSampleCount+1;                                                        /* keep counting the sample quantity*/
            tempLastSample = millis();                                                                  /* reset the time in order to repeat the loop again*/
        }

    if(tempSampleCount == 1000)                                                                         /* after 1000 sample readings taken*/
        {
            tempMean = tempSampleSum / tempSampleCount;                                                 /* find the average analog value from those data*/
            R2 = (voltageDividerR1*tempMean)/(1023-tempMean);                                           /* convert the average analog value to resistance value*/
    
            a = 1/T1;                                                                                   /* use for calculation */
            b = log10(RT1/R2);                                                                           /* use for calculation */
            c = b / log10(e);                                                                           /* use for calculation */
            d = c / BValue ;                                                                            /* use for calculation */
            T2 = 1 / (a- d);
            temp = T2 - 273.15;
            /* the measured temperature value based on calculation (in Kelvin) */
            
            tempSampleSum = 0;                                                                          /* reset all the total analog value back to 0 for the next count */
            tempSampleCount = 0;
            return temp;/* reset the total number of samples taken back to 0 for the next count*/
        }
    }
        return -1;
    

  
}
