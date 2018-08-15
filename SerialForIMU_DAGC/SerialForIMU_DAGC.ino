/* Leia antes
   Achar coeficientes para estabilização angular
 */

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
int tetaX = 0;
int tetaY = 0;
float tetaZ = 0;
//-------- testando float
int deltaX = 0;
int deltaY = 0;
float deltaZ = 0;

int deltaFinalX = 0;
int deltaInicialX =0;
int deltaFinalY = 0;
int deltaInicialY =0;
float deltaFinalZ = 0;
float deltaInicialZ =0;

int aux = 0;

//===================================
int n = 0;
int mediaParaCoordenadaInicialX = 0;
int mediaParaCoordenadaInicialY = 0;
long mediaParaCoordenadaInicialZ = 0;
double mmZ=0;
//===================================

// Ganho, pagina 10 datasheet
#define gain 0.00875

// Delay em segundos
#define loopInSec 0.1

// Soma teta com tempo
float tetaTZ = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }

  gyro.enableDefault();
}

void loop() {

  gyro.read();

  if (aux==0)
  {
    deltaInicialX = gyro.g.x;
    deltaInicialY = gyro.g.y;
    deltaInicialZ = gyro.g.z;
  }
  /*
  Serial.print("  ");
  
  Serial.print("G ");
  Serial.print("X: ");
  Serial.print((int)gyro.g.x);
  Serial.print(" Y: ");
  Serial.print((int)gyro.g.y);
  Serial.print(" Z: ");
  Serial.println((int)gyro.g.z);
  */
  
  deltaFinalX = gyro.g.x;
  deltaFinalY = gyro.g.y;
  deltaFinalZ = gyro.g.z;
  Serial.print(" Z: ");
  Serial.print((float)gyro.g.z);
  
  //---------------------- média para media dos deltas iniciais -----------
  n = n + 1;
    mediaParaCoordenadaInicialX = mediaParaCoordenadaInicialX + gyro.g.x*8.75/1000;
    mediaParaCoordenadaInicialY = mediaParaCoordenadaInicialY + gyro.g.y*8.75/1000;
    mediaParaCoordenadaInicialZ = (mediaParaCoordenadaInicialZ + gyro.g.z);
    mmZ = mediaParaCoordenadaInicialZ/n;
  /*
  if (n==1)
  {
     mediaParaCoordenadaInicialZ = gyro.g.z;
  }

  if (n>1)
  {
    mediaParaCoordenadaInicialZ = (mediaParaCoordenadaInicialZ + gyro.g.z)/2;
  }
   */ 
  /*
  Serial.print(" media para x: ");
  Serial.print(mediaParaCoordenadaInicialX/n);
  Serial.print(" media para y: ");
  Serial.print(mediaParaCoordenadaInicialY/n);
  */
  Serial.print(" n: ");
  Serial.print(n);
  Serial.print("\t soma z: ");
  Serial.print(mediaParaCoordenadaInicialZ);
  Serial.print("\t media z: ");
  Serial.print(mmZ);
  //-----------------------------------------------------------------------



  deltaX = ((deltaFinalX - deltaInicialX)*8.75/1000);
  deltaY = ((deltaFinalY - deltaInicialY)*8.75/1000);

  Serial.print(" incial z: ");
  Serial.print(deltaInicialZ);
  deltaZ = (deltaFinalZ - deltaInicialZ)*gain;

  if (deltaZ <= 1.99 and deltaZ >= -1.99)
  {
    deltaZ = 0;
  }
  
  /*
  Serial.print("deltaX: ");
  Serial.print(deltaX);
  Serial.print(" deltaY: ");
  Serial.print(deltaY);
  */
  Serial.print("\t deltaZ: ");
  Serial.print(deltaZ);

  tetaTZ = deltaZ*loopInSec;

  if (tetaTZ <= 0.99 and tetaTZ >= -0.99)
  {
    tetaTZ = 0;
  }
  
  tetaX = tetaX + deltaX;
  tetaY = tetaY + deltaY;
  
  tetaZ += tetaTZ;

  if (tetaX>=360 or tetaX<=-360)
  {
    tetaX=0;
  }
  if (tetaY>=360 or tetaY<=-360)
  {
    tetaY=0;
  }
  if (tetaZ>=360 or tetaZ<=-360)
  {
    tetaZ=0;
  }

  Serial.print("  ");

  /*
  Serial.print("TetaX: ");
  Serial.print(tetaX);
  Serial.print(" TetaY: ");
  Serial.print(tetaY);
  */
  Serial.print("\t tetaZ: ");
  Serial.println(tetaZ);
  
  aux = 1;

  delay(1);
}
