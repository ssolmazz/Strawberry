

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/////////////*/

        

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
        return -1;
    

  
}






//************************* User Defined Variables ********************************************************//


//##################################################################################
//-----------  Do not Replace R1 with a resistor lower than 300 ohms    ------------
//##################################################################################


int R1 = 1000;
int Ra = 25; //Resistance of powering Pins
int ECPin = A0;
int ECGround = A1;
int ECPower = A4;



float PPMconversion = 0.7;


//*************Compensating for temperature ************************************//
//The value below will change depending on what chemical solution we are measuring
//0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring




//********************** Cell Constant For Ec Measurements *********************//
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K = 1.16;


#define ONE_WIRE_BUS 10          // Data wire For Temp Probe is plugged into pin 10 on the Arduino


float Temperature = 10;
float EC = 0;
float EC25 = 0;
int ppm = 0;


float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;




//*********************************Setup - runs Once and sets pins etc ******************************************************//
void setup()
{
  Serial.begin(9600);
  //pinMode(TempProbeNegative , OUTPUT ); //seting ground pin as output for tmp probe
  //digitalWrite(TempProbeNegative , LOW );//Seting it to ground so it can sink current
  //pinMode(TempProbePossitive , OUTPUT );//ditto but for positive
  //digitalWrite(TempProbePossitive , HIGH );
  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT); //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT); //setting pin for sinking current
  digitalWrite(ECGround, LOW); //We can leave the ground connected permanantly

  delay(100);// gives sensor time to settle
  //sensors.begin();
  delay(100);
  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor *********//
  // Consule Read-Me for Why, or just accept it as true
  R1 = (R1 + Ra); // Taking into acount Powering Pin Resitance

  Serial.println("ElCheapo Arduino EC-PPM measurments");
  Serial.println("By: Michael Ratcliffe  Mike@MichaelRatcliffe.com");
  Serial.println("Free software: you can redistribute it and/or modify it under GNU ");
  Serial.println("");
  Serial.println("Make sure Probe and Temp Sensor are in Solution and solution is well mixed");
  Serial.println("");
  Serial.println("Measurments at 5's Second intervals [Dont read Ec morre than once every 5 seconds]:");


};
//******************************************* End of Setup **********************************************************************//




//************************************* Main Loop - Runs Forever ***************************************************************//
//Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop
void loop()
{


  Temperature = readTemp();
  if (Temperature != -1){

    GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
    PrintReadings();  // Cals Print routine [below main loop]

  }


}
//************************************** End Of Main Loop **********************************************************************//




//************ This Loop Is called From Main Loop************************//
void GetEC() {


  //*********Reading Temperature Of Solution *******************//
  //sensors.requestTemperatures();// Send the command to get temperatures
  //Temperature = 20;
  //Temperature=sensors.getTempCByIndex(0); //Stores Value in Variable




  //************Estimates Resistance of Liquid ****************//
  digitalWrite(ECPower, HIGH);
  raw = analogRead(ECPin);
  raw = analogRead(ECPin); // This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(ECPower, LOW);




  //***************** Converts to EC **************************//
  Vdrop = (Vin * raw) / 1024.0;
  Rc = (Vdrop * R1) / (Vin - Vdrop);
  Rc = Rc - Ra; //acounting for Digital Pin Resitance
  EC = 1000 / (Rc * K);


  //*************Compensating For Temperaure********************//
  EC25  =  EC / (1 + TemperatureCoef * (Temperature - 25.0));
  ppm = (EC25) * (PPMconversion * 1000);


  ;
}
//************************** End OF EC Function ***************************//




//***This Loop Is called From Main Loop- Prints to serial usefull info ***//
void PrintReadings() {
  Serial.print("Rc: ");
  Serial.print(Rc);
  Serial.print(" EC: ");
  Serial.print(EC25);
  Serial.print(" Simens  ");
  Serial.print(ppm);
  Serial.print(" ppm  ");
  Serial.print(Temperature);
  Serial.println(" *C ");


};
