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
#include <SPI.h>
#include <SD.h>

File myFile;

// Update Current Version
#define VERSION 1.16 

#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

int luxVal;
float temperature,humidity;
boolean printCSV = false;
boolean printSerial = false;
boolean verbosity = false;
int verbosityLevel = 2;
boolean printSD = false;

const int chipSelect = 10;

// Strings Log
char temp[13];
char hum[13];
char lux[13];
char timeK[13];

String tempT;
String luxT;
String humT;
String timeT;
String messageToSdWrite;
String FILE2OPEN = "logBox.txt";

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

void setupSD()
{
  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(4)) 
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("[OK]");
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
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
  setupDht();
  setupSD();

  // Yhis should be the last one
  initTimer();
}

void loop()
{
  // First
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
  
  if ( timerLoop % SdTimerPeriod == 0)
  {
    writeSdToRoutine();
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

// #SD #SDWRITE
void writeSdToRoutine()
{  
  if (printSD)
  {
    // The following function must be called
    initLog();


    if (verbosityLevel >=6)
    {
      Serial.print("[Sakura] writing to sd: ");
      Serial.print("H," + humT + "," + tempT + "," + luxT + "," + timeT + ",Z");
    }
    
    //int timeK = convertMillis2Min(millis());
    //Concat string to send
    messageToSdWrite = "H," + humT + "," + tempT + "," + luxT + "," + timeT + ",Z";
    
    sdWrite(messageToSdWrite);
  
    // timerSec = micros()-secRoutine;
    // lastTimeToRead = micros();
    readSdFile();
  }
}

// # LUX
void readLux()
{
  luxVal = analogRead(0);  
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
}

void sdWrite(String message)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(FILE2OPEN, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) 
  {
    myFile.println(message);
    // close the file:
    myFile.close();
    Serial.println("[Sakura] Wrote to file.");
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("[Sakura] ERROR opening");
  }   
}

void readSdFile()
{
  // re-open the file for reading:
  myFile = SD.open(FILE2OPEN);
  if (myFile) 
  {
    Serial.println("[Sakura] Content:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) 
    {
      Serial.write(myFile.read());
    }
    // close the file:c
    
    myFile.close();
    Serial.println();
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
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
      if (verbosity)
        Serial.println("- Verbosity = TRUE");
      else
        Serial.println("- Verbosity = FALSE");
    }      
    else if (t == 's')
    {
      Serial.println("- Toggle SDCARD verbosity ... [OK]");
      printSD = !printSD;
      if (printSD)
        Serial.println("- SD = TRUE");
      else
        Serial.println("- SD = FALSE");
    }
    else if (t == 'h')
    {
      Serial.println("Help:");
      
      
      Serial.println("+/- : adjust verbosity level");
      Serial.println("c : toggle CSV mode");
      Serial.println("n : toggle Normal mode");
      Serial.println("s : toggle SD verbosity");
      Serial.println("p : toggle general verbosity");      
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

