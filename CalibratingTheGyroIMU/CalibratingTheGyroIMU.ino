/*
The sensor outputs provided by the library are the raw 16-bit values
obtained by concatenating the 8-bit high and low gyro data registers.
They can be converted to units of dps (degrees per second) using the
conversion factors specified in the datasheet for your particular
device and full scale setting (gain).

Example: An L3GD20H gives a gyro X axis reading of 345 with its
default full scale setting of +/- 245 dps. The So specification
in the L3GD20H datasheet (page 10) states a conversion factor of 8.75
mdps/LSB (least significant bit) at this FS setting, so the raw
reading of 345 corresponds to 345 * 8.75 = 3020 mdps = 3.02 dps.
*/

#include <Wire.h>
#include <L3G.h>

L3G gyro;


int n = 0;
//<<<>>>>>>>>>>>>>>> mudar todos defines depois
// Número de contagens para calibrar gyro
const long sampleNumber = 200;
//#define sampleNumber 1000

double averageNumberToZ = 0;
double averageNumberToY = 0;
double averageNumberToX = 0;

double noiseZ = 0;
double noiseY = 0;
double noiseX = 0;


unsigned long time = 0;

double rateZ = 0;
double rateY = 0;
double rateX = 0;

const long sampleToTime = 10;
//#define sampleToTime 10

double prevRateZ = 0;
double prevRateY = 0;
double prevRateX = 0;

double angleZ = 0;
double angleY = 0;
double angleX = 0;

const double gainZ = 0.00838;
//#define gainZ 0.04363
const double gainY = 0.00945;
//#define gainY 0.1
const double gainX = 0.00981;
//#define gainX 0.1

int testZ = 0;
int testY = 0;
int testX = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }

  gyro.enableDefault();

  //-------------------------------------------------------------
  // Calibração do offset
  for (n=0;n<sampleNumber; n++)
  {
    gyro.read();
    
    averageNumberToZ += (int)gyro.g.z;
    averageNumberToY += (int)gyro.g.y;
    averageNumberToX += (int)gyro.g.x;

    Serial.print("\t Z: ");
    Serial.print(averageNumberToZ);
    Serial.print("\t Y: ");
    Serial.print(averageNumberToY);
    Serial.print("\t X: ");
    Serial.print(averageNumberToX);
    Serial.println();
    
  }

  averageNumberToZ = averageNumberToZ/sampleNumber;
  averageNumberToY = averageNumberToY/sampleNumber;
  averageNumberToX = averageNumberToX/sampleNumber;
  
  //Calculos para quantidade de ruído para evitar drifting do gyro
  for(n=0;n<sampleNumber;n++)
  {
    gyro.read();
    
    if (gyro.g.z - averageNumberToZ > noiseZ)
      noiseZ = gyro.g.z - averageNumberToZ;
    else if(gyro.g.z - averageNumberToZ < -noiseZ)
      noiseZ = -gyro.g.z - averageNumberToZ;
      
    if (gyro.g.y - averageNumberToY> noiseY)
      noiseY = gyro.g.y - averageNumberToY;
    else if(gyro.g.y - averageNumberToY < -noiseY)
      noiseY = -gyro.g.y - averageNumberToY;
      
    if (gyro.g.x - averageNumberToX > noiseX)
      noiseX = gyro.g.x- averageNumberToX;
    else if(gyro.g.x - averageNumberToX < -noiseX)
      noiseX = -gyro.g.x - averageNumberToX;
  }

  // Soma com mais 1 para termos um range maior no ruído
  noiseZ = noiseZ/100 + 1;
  noiseY = noiseY/100 + 1;
  noiseX = noiseX/100 + 1;


  // Repitir 200 vezes e tirar média para ver qual é o melhor número para ser usado
  Serial.println();
  Serial.print("Numero para calibracao Z: ");
  Serial.print(averageNumberToZ);
  Serial.print ("\t Nivel do ruido Y: ");
  Serial.print(noiseZ);
  Serial.println();
  
  Serial.print("Numero para calibracao Y: ");
  Serial.print(averageNumberToY);
  Serial.print ("\t Nivel do ruido Y: ");
  Serial.print(noiseY);
  Serial.println();

  Serial.print("Numero para calibracao X: ");
  Serial.print(averageNumberToX);
  Serial.print ("\t Nivel do ruido X: ");
  Serial.print(noiseX);
  Serial.println();
  Serial.println();
  //------------------------------------------------------------------------

  time = millis() +2000;
  delay(2000);
}


   

void loop() 
{
  //if(millis() - time > sampleToTime)
  {
    //time = millis();
    gyro.read(); //Leitura dos dados do gyroscope

    // Leitura menos o offset vezes o ganho
    rateZ = ((int)gyro.g.z - averageNumberToZ)*gainZ;
    rateY = ((int)gyro.g.y - averageNumberToY)*gainY;
    rateX = ((int)gyro.g.x - averageNumberToX)*gainX;

    
    Serial.print("\t");
    Serial.print((millis() - time)); //Tempo do loop
    Serial.print("\t");
      
    if(rateZ >= noiseZ or rateZ<= -noiseZ)
    {
      // teste <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<     
      angleZ += (((double)(prevRateZ + rateZ)*(millis() -time))/2000);
    }
    if(rateY >= noiseY or rateY<= -noiseY)
    {
      angleY += (((double)(prevRateY + rateY)*(millis() -time))/2000);
    }
    if(rateX >= noiseX or rateX<= -noiseX)
    {
      angleX += (((double)(prevRateX + rateX)*(millis() -time))/2000);
    }
    time = millis();

    prevRateZ = rateZ;
    prevRateY = rateY;
    prevRateX = rateX;
    
    // recalibração da média sem movimento
    // testar proxima vez que ir no lago <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // testado
    ///*
    if((int)gyro.g.z >= (averageNumberToZ-15) and (int)gyro.g.z <= (averageNumberToZ+15))
    {
      testZ = (averageNumberToZ + (int)gyro.g.z)/2;
      ///*
      Serial.print("mZ: ");
      Serial.print(averageNumberToZ);
      Serial.print("\t");
      //*/
      averageNumberToZ = testZ;
    }
    if((int)gyro.g.y >= (averageNumberToY-15) and (int)gyro.g.y <= (averageNumberToY+15))
    {
      testY = (averageNumberToY + (int)gyro.g.y)/2;
      /*
      Serial.print("mY: ");
      Serial.print(averageNumberToY);
      Serial.print("\t");
      */
      averageNumberToY = testY;
    }
    if((int)gyro.g.x >= (averageNumberToX-15) and (int)gyro.g.x <= (averageNumberToX+15))
    {
      testX = (averageNumberToX + (int)gyro.g.x)/2;
      /*
      Serial.print("mX: ");
      Serial.print(averageNumberToX);
      Serial.print("\t");
      */
      averageNumberToX = testX;
    }
    //*/
    
    /*
     * Faz com que o angulos variado fiquem entre 0 e 359.
     */
    if (angleZ < 0)
    {
      angleZ += 360;
    }
    else if(angleZ >= 360)
    {
      angleZ -=360;
    }
    
    if (angleY < 0)
    {
      angleY += 360;
    }
    else if(angleY >= 360)
    {
      angleY -=360;
    }
    
    if (angleX < 0)
    {
      angleX += 360;
    }
    else if(angleX >= 360)
    {
      angleX -=360;
    }


    
    ///*
    Serial.print("\t angleZ: ");
    Serial.print(angleZ);
    ///*
    Serial.print("\t prevRateZ: ");
    Serial.print(prevRateZ);
    Serial.print("\t rateZ: ");
    Serial.print(rateZ);
    //*/
    // Now you have to calibrate gains in X and Y rates. >>>>>>>>>>>>>>>>>>>>>>>>>>
    /*
    Serial.print("\t angleY: ");
    Serial.print(angleY);
    /*
    Serial.print("\t prevRateY: ");
    Serial.print(prevRateY);
    Serial.print("\t rateY: ");
    Serial.print(rateY);
    //*/
    /*
    Serial.print("\t angleX: ");
    Serial.print(angleX);
    /*
    Serial.print("\t prevRateX: ");
    Serial.print(prevRateX);
    Serial.print("\t rateX: ");
    Serial.print(rateX);
    //*/
    
    Serial.println();
  }
  delay(0);
}
