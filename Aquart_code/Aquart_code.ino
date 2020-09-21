/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This sketch shows how to write values to Virtual Pins

  NOTE:
  BlynkTimer provides SimpleTimer functionality:  
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "N5oAgEdLbzuWNazBPYCeUWQqbTYYaeNQ";

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "IOT-SNIST";
//char pass[] = "snist@iot123";

char ssid[] = "aaryan";
char pass[] = "aaryan12";


//char ssid[] = "wifi@hone";
//char pass[] = "murthy115";
BlynkTimer timer;

WidgetLED led1(V5);
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"


byte sensorInterrupt = D4; 
byte sensorPin       = D4;

byte relay = D5;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount; 

float flowRate;
unsigned int flowMilliLitres;
float totalMilliLitres;

unsigned long oldTime;

int threshold;
float percent;

int state;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.

  BLYNK_WRITE(V1)
{
  threshold = param.asInt(); // assigning incoming value from pin V1 to a variable
    Serial.print ( "Threshold limit = ");
    Serial.print(threshold);
    Serial.println(" Liters");
  // process received value
}

BLYNK_WRITE(V0)
{
  int state = param.asInt(); // assigning incoming value from pin V1 to a variable
    Serial.print("Tap is : ");
    Serial.println(state);
  // process received value

   if (state == 1)
   {
    digitalWrite(relay, HIGH);
    Serial.println("ON");
   }

   else if (state == 0)
   {
    digitalWrite(relay, LOW);
    Serial.println("OFF");
   }
}


void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  {
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
       
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
   
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
   
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
   
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    percent = ((totalMilliLitres/ 1000) / threshold) * 100;
     
    unsigned int frac;


    Serial.print("Tap is : ");
    Serial.println(state);
    
  
   
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL");

    Serial.print("  Percent Used: ");             // Output separator
    Serial.print(percent);
    Serial.println(" %");

    Blynk.virtualWrite(V3, flowMilliLitres);
    Blynk.virtualWrite(V4, (totalMilliLitres/ 1000));
   if (percent < 100)
   {
    Blynk.virtualWrite(V2, percent);
   }
    if (percent >= 100)
    {
      Blynk.virtualWrite(V0, 0);
      Blynk.virtualWrite(V2, 100);
      digitalWrite( relay , LOW);
    }

    if (percent < 50  )
    {
      led1.setColor(BLYNK_GREEN);
    }
    
    if (percent >= 50 && percent < 80 )
    {
      led1.setColor(BLYNK_YELLOW);
    }
    
    if (percent >80 && percent <100 )
    {
      led1.setColor(BLYNK_RED);
    }

//    if( percent <55 && percent > 45)
//    {
//    Blynk.tweet(" 50% of your daily quota has been used ");
//    Serial.println(" Tweeted 50% notification");
//    }
//
//    if( percent <85 && percent > 75)
//    {
//    Blynk.tweet(" 80% of your daily quota has been used. Carefull utilization of water is prescribed. ");
//    Serial.println(" Tweeted 80% notification");
//    }
//
//    if( percent <105 && percent > 95)
//    {
//    Blynk.tweet(" 100% of your daily quota has been used. To switch the tap back ON change the Threshold limit.");
//    Serial.println(" Tweeted 100% notification");
//    }
    

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
   
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }

}


void setup()
{

    Serial.begin(115200);
   
    // We have an active-low LED attached
 
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  percent           = 0.0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  // Debug console

  led1.on();  

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

pinMode( relay , OUTPUT);
  
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
   
}

void pulseCounter() 
{
  // Increment the pulse counter
  pulseCount++;
}
