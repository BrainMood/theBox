/*********************************************************************/
/*                             - TheBox -                            */
/*                                                                   */
/*             DHT22 - Temperature and humidity sensor               */
/*                       uSD reader logging sys                      */
/*                                Lux                                */
/*                                                                   */
/*             Contacts:                                             */
/*                      Alessio Aka BrainDev                         */
/*                         Ivan B  Aka KillerBM                      */
/*                            Gepp Aka UserK                         */
/*                                                                   */
/*             PinOUT  DHT22                                         */
/*                        Vcc  - 5 V                                 */
/*                         DATA - 2                                  */
/*                                                                   */
/*             PinOUT  Blue                                          */
/*                        RX   -  11                                 */
/*                        TX   -  10                                 */
/*                                                                   */
/*             PinOUT  GAS                                           */
/*                        A0   -  A0                                 */
/*                                                                   */
/*             PinOUT  LUX                                           */
/*                       DATA  -  A1                                 */
/*                                                                   */
/*                                                                   */
/*********************************************************************/

/*                                                                   */
/*********************************************************************/


#include "DHT.h"
#include "SoftwareSerial.h"

// Update Current Version
#define VERSION 1.01

#define DHTPIN 2     // what digital pin we're connected to
#define LUX_PIN 1
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial blu(10, 11); // RX, TX

int luxVal = 0;
float temperature =0.0,humidity=0.0;
boolean printCSV = false;
boolean printSerial = false;
boolean verbosity = false;
int verbosityLevel = 2;

// Strings Log
char temp[13];
char hum[13];
char lux[13];
char timeK[13];

String tempT;
String luxT;
String humT;
String timeT;

// Timers Countera
long timerLoop, timer0, timer1, timer2;
int TLperiod = 5;
long int periodeTimerLoopMillis = 0;
long int SdTimerPeriod = 5000;

void setupDht()
{
  dht.begin();
  pinMode(0,INPUT);
}

void initTimer()
{
  timerLoop = 0; 
  timer0 = 0;
  timer1 = 0;
  timer2 = 0;
}

void setup() 
{
  Serial.begin(115200);
  blu.begin(115200);
  setupDht();

  // This should be the last one
  initTimer();
}

void loop()
{
  updateTimer();
  sensorRoutine();
  SerialRoutine();
}

double convertSec2millisTimerL(int a)
{
  periodeTimerLoopMillis = 1000*a;
  return periodeTimerLoopMillis;
}

int convertMillis2Min(float a)
{
  return  a/1000;
}

double convertMin2Millis(double a)
{
  return a*1000;
}

//  #SENSOR #ROUTINE
void sensorRoutine()
{
  convertSec2millisTimerL(TLperiod);
  if ( timerLoop % periodeTimerLoopMillis == 0)
  {
    readLux();
    getTempHum();
    printData();
  }
  
}

// Update timers
void updateTimer()
{
  timerLoop = millis();
  if (verbosityLevel >=8)
  {
    Serial.println("[Sakura] Timer updates");
  }
}

// # LUX
void readLux()
{
  luxVal = analogRead(LUX_PIN);  
  if (verbosityLevel >= 3)
     Serial.println("Ok LUx");
}

// #TEMP #HUMIDITY #HUM
void getTempHum()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();  
  if (verbosityLevel >= 3)
     Serial.println("Ok Temp");
}

void printData()
{
  if (verbosity)
  {
    if (printSerial)
    {
      printNormal();
    }
    else if (printCSV)
    {
      printCSVData();
    }
  }
}

void printNormal()
{
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" LUX:  ");
  Serial.println(luxVal);  


  
  blu.print("Humidity: ");
  blu.print(humidity);
  blu.print(" %\t");
  blu.print("Temperature: ");
  blu.print(temperature);
  blu.print(" LUX:  ");
  blu.println(luxVal);  

  
}

void printCSVData()
{
  Serial.print("H,");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(luxVal);
  Serial.print(",");
  Serial.println("Z");


  
  blu.print("H,");
  blu.print(humidity);
  blu.print(",");
  blu.print(temperature);
  blu.print(",");
  blu.print(luxVal);
  blu.print(",");
  blu.println("Z");
}

// #SERIAL
void SerialRoutine()
{
  // TODO 
  if (Serial.available())
  {
    //Serial.println(" received ");
    char t = Serial.read();
    if (t == 'c')
    {
      printCSV = !printCSV;
      if (printCSV)
        Serial.println("- CSV mode = TRUE");
      else
        Serial.println("- CSV mode = FALSE");
    }
    else if (t == 'n')
    {
      printSerial = !printSerial;
      if (printSerial)
        Serial.println("- Normal mode = TRUE");
      else
        Serial.println("- Normal mode = FALSE");
    }    
    else if (t == '+')
    {
      verbosityLevel++;
      Serial.println("- Verbosity level changed :]\n");
      Serial.print(verbosityLevel);
    }    
    else if (t == '-')
    {
      verbosityLevel--;
      Serial.println("- Verbosity level changed :]\n");
      Serial.print(verbosityLevel);
    }    
    else if (t == 'p')
    {
      verbosity = !verbosity;
      if (verbosity) {
        Serial.println("- Verbosity = TRUE");
        blu.println("- Verbosity = TRUE");
      } else {
        Serial.println("- Verbosity = FALSE");
        blu.println("- Verbosity = FALSE");
      }
    } 
    else if (t == 'h')
    {
      Serial.println("Help:");
      
      
      Serial.println("+/- : adjust verbosity level");
      Serial.println("c : toggle CSV mode");
      Serial.println("n : toggle Normal mode");
      Serial.println("s : toggle SD verbosity");
      Serial.println("p : toggle general verbosity");    

      
      blu.println("Help:");
      
      
      blu.println("+/- : adjust verbosity level");
      blu.println("c : toggle CSV mode");
      blu.println("n : toggle Normal mode");
      blu.println("s : toggle SD verbosity");
      blu.println("p : toggle general verbosity");    

     
    }        
  }
} 

void initLog()
{
  tempT = dtostrf(temperature,5,2,temp);
  luxT = dtostrf(luxVal,5,2,lux);
  timeT = dtostrf(convertMillis2Min(millis()),5,2,timeK);
  humT = dtostrf(humidity,5,2,hum);
}

