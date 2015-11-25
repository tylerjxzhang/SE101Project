#include "functions.h"
#include <delay.h>
#include <FillPat.h>
#include <I2CEEPROM.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>

/* ------------------------------------------------------------ */
/*				Local Type Definitions		*/
/* ------------------------------------------------------------ */
#define WELCOME		0
#define HELP		2
#define GAME		1
#define SURVIVAL	3
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

/* ------------------------------------------------------------ */
/*				Local Variables			*/
/* ------------------------------------------------------------ */
char	chSwtCur;
char	chSwtPrev;
bool	fClearOled;

/*
 * Car Definitions
 */

const unsigned int bitmapHeight = 9;  // The height of the bitmap of the car
const unsigned int bitmapWidth = 15;   // The width of the bitmap of the car

// Bitmap of the cars coming from the oppsite direction
char bmp[][15] = 
{ 
  {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0          }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1          }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1          }
  ,
  {
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1          }
  ,
  {
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1          }
  ,
  {
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1          }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1          }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1          }
  ,
  {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0         }
  ,
};

int xcoCarStart = 112;
int ycoCarStart = 10;

int xcoCarCur = xcoCarStart;
int ycoCarCur = ycoCarStart;

int carHeight = 9;
int carWidth= 15;

// Bitmap of the car
char bmpCar[][15] = 
{ 
  {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0          }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1          }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1          }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1          }
  ,
  { 
    1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1          }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1          }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1          }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1          }
  ,
  {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0         }
  ,
};

/*
 * Bitmaps of other displayed items
 */
char lv[][20]={
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,
  {
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1  }
  ,};

char one[][20]={
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
  ,
  {
    0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  }
  ,
  {
    0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  }
  ,
  {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,};

char two [][20]={
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0  }
  ,
  {
    0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0  }
  ,
  {
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0  }
  ,
  {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0  }
  ,
  {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0  }
  ,
  {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0  }
  ,
  {
    0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0  }
  ,
  {
    0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,};

char three[][20]={
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0  }
  ,
  {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0  }
  ,
  {
    0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0  }
  ,
  {
    0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0  }
  ,
  {
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0  }
  ,
  {
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0  }
  ,
  {
    0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0  }
  ,
  {
    0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0  }
  ,
  {
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0  }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  }
  ,};
  
char oneOpposite[][20] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

char twoOpposite[][20] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1},
{1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1},
{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1},
{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

char threeOpposite[][20] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
{1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
{1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1},
{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1},
{1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};
  

// These co-ordinates correspond to the TOP LEFT of the bitmap
unsigned int x = 0;
unsigned int y = 0;


/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */
void DeviceInit();
char CheckSwitches();
int collision(int mainx, int mainy, int x, int y);
char potentiometer();
char bGameState = 0;
int gameStart = 0;
int level;
int *xcoors;
int *ycoors;

// ---------------------------------------------------------------------------//
// This is the sneaky part... actually drawing your bitmap as a 2D array is
// very hard, so I helped you out by writing a admittedly hard-to-understand
// function to convert the bitmap to bytes that the booster pack can understand.
char* formattedBitmap(char* input, unsigned int width, unsigned int height)
{

  unsigned int h = ceil(height / 8.0);
  char *output = (char*)calloc(h * width, sizeof(char));
  char b, temp;
  for (unsigned int hbyte = 0; hbyte < h; ++hbyte) {
    for (unsigned int i = 0; i < width; ++i) {
      b = 0;
      for (unsigned int j = 0; j < ((height - hbyte * 8)/8 ? 8 : (height%8)); ++j) {
        temp = input[(8*hbyte+j)*width+i];
        if (temp) b |= 1 << j;
      }

      output[hbyte*width+i]|=b;
    }
  }
  return output;
}

// Convert all the 2D arrays to actual bitmaps
char* bitmap = formattedBitmap((char*)bmp, bitmapWidth, bitmapHeight);
char* carBitmap = formattedBitmap ((char*)bmpCar, carWidth, carHeight);
char* LvBitmap = formattedBitmap ((char*)lv, 20, 30);
char* oneBitmap = formattedBitmap ((char*)one, 20, 20);
char* twoBitmap = formattedBitmap ((char*)two, 20, 20);
char* threeBitmap = formattedBitmap ((char*)three, 20, 20);
char* oneOppositeBitmap = formattedBitmap((char*)oneOpposite, 20, 20);
char* twoOppositeBitmap = formattedBitmap((char*)twoOpposite, 20, 20);
char* threeOppositeBitmap = formattedBitmap((char*)threeOpposite, 20, 20);

// ---------------------------------------------------------------------------//

void setup()
{
  DeviceInit();

  // Reset OLED (function from functions.cpp)
  oledReset();
  OrbitOledUpdate();
}

void loop()
{
  
  volatile uint32_t ui32Loop;

  bGameState = CheckSwitches();
  for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
  {
  }

  switch(bGameState) {

  case WELCOME:
    WelcomeScreen();
    break;
  case HELP:
    HelpScreen();
    break;
  case GAME:
    GameScreen();
    break;
  case SURVIVAL:
    SurvivalScreen();
    break;
  default:
    WelcomeScreen();
    break;
  }

}

/* ------------------------------------------------------------ */
/***	DeviceInit
 **
 **	Parameters:
 **		none
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Initialize I2C Communication, and GPIO
 */
void DeviceInit()
{
  /*
   * First, Set Up the Clock.
   * Main OSC		  -> SYSCTL_OSC_MAIN
   * Runs off 16MHz clock -> SYSCTL_XTAL_16MHZ
   * Use PLL		  -> SYSCTL_USE_PLL
   * Divide by 4	  -> SYSCTL_SYSDIV_4
   */
  SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_4);

  /*
   * Enable and Power On All GPIO Ports
   */
  //SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOA | SYSCTL_PERIPH_GPIOB | SYSCTL_PERIPH_GPIOC |
  //						SYSCTL_PERIPH_GPIOD | SYSCTL_PERIPH_GPIOE | SYSCTL_PERIPH_GPIOF);

  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOA );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOB );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOC );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOD );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOE );
  SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOF );
  /*
   * Pad Configure.. Setting as per the Button Pullups on
   * the Launch pad (active low).. changing to pulldowns for Orbit
   */
  GPIOPadConfigSet(SWTPort, SWT1 | SWT2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(BTN1Port, BTN1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPadConfigSet(BTN2Port, BTN2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(LED1Port, LED1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED2Port, LED2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED3Port, LED3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED4Port, LED4, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  /*
   * Initialize Switches as Input
   */
  GPIOPinTypeGPIOInput(SWTPort, SWT1 | SWT2);

  /*
   * Initialize Buttons as Input
   */
  GPIOPinTypeGPIOInput(BTN1Port, BTN1);
  GPIOPinTypeGPIOInput(BTN2Port, BTN2);

  /*
   * Initialize LEDs as Output
   */
  GPIOPinTypeGPIOOutput(LED1Port, LED1);
  GPIOPinTypeGPIOOutput(LED2Port, LED2);
  GPIOPinTypeGPIOOutput(LED3Port, LED3);
  GPIOPinTypeGPIOOutput(LED4Port, LED4);

  /*
   * Enable ADC Periph
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

  GPIOPinTypeADC(AINPort, AIN);

  /*
   * Enable ADC with this Sequence
   * 1. ADCSequenceConfigure()
   * 2. ADCSequenceStepConfigure()
   * 3. ADCSequenceEnable()
   * 4. ADCProcessorTrigger();
   * 5. Wait for sample sequence ADCIntStatus();
   * 6. Read From ADC
   */
  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCSequenceEnable(ADC0_BASE, 0);

  /*
   * Initialize the OLED
   */
  OrbitOledInit();

  /*
   * Reset flags
   */
  chSwtCur = 0;
  chSwtPrev = 0;
  fClearOled = true;

}

/* ------------------------------------------------------------ */
/***	CheckSwitches()
 **
 **	Parameters:
 **		none
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Return the state of the Switches
 */
char CheckSwitches() {

  long 	lSwt1;
  long 	lSwt2;

  chSwtPrev = chSwtCur;

  lSwt1 = GPIOPinRead(SWT1Port, SWT1);
  lSwt2 = GPIOPinRead(SWT2Port, SWT2);

  chSwtCur = (lSwt1 | lSwt2) >> 6;

  if(chSwtCur != chSwtPrev) {
    fClearOled = true;
  }

  return chSwtCur;

}

/* ------------------------------------------------------------ */
/***    Welcome Screen
 ** 
 **	Parameters:
 **		none
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Display a welcome screen of the game
 **
 */

void WelcomeScreen() {

  char streetRacer[] = {
    'S','t','r','e','e','t',' ','R','a','c','e','r'  };

  /*
   * If applicabe, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;
  } 

  /*
   * Display welcome screen
   */
  OrbitOledSetCursor(2,1);
  OrbitOledPutString(streetRacer);  

  OrbitOledMoveTo(0,19);
  OrbitOledLineTo(127, 19);

  for (int i = 7; i < 108; i += 20)
    oledDraw(bitmap, i, 22, bitmapWidth, bitmapHeight);
}

/* ------------------------------------------------------------ */
/***	HelpScreen
 **
 **	Parameters:
 **		none
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Display a string that tells the player how to play the game
 */
void HelpScreen(){

  char line1[] = {
    'T', 'i', 'l', 't', ' ', 't', 'h', 'e', ' ', 'b', 'o', 'a', 'r', 'd', ' ', ' ', 't', 'o', ' ', 'm', 'o', 'v', 'e', ' ', 't', 'h', 'e', ' ', 'c', 'a', 'r'  };
  char line2[] = {
    'P', 'r', 'e', 's', 's', ' ', 'a', 'n', 'y', ' ', 'b', 'u', 't', 't', 'o', 'n', ' ', 't', 'o', ' ', 'p', 'a', 'u', 's', 'e'  };

  /*
   * If applicabe, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;
  } 

  OrbitOledSetCursor(0,0);
  OrbitOledPutString(line1);

  OrbitOledSetCursor(0,2);
  OrbitOledPutString(line2); 
}

void GameScreen(){
  
  long 			lBtn1;
  long 			lBtn2;
  
  int levelOneX[] = {10,89,129,187,273,336,400,478,532,590};
  int levelOneY[] = {0,0,10,20,0,20,10,10,20,0};
  int levelTwoX[] = {0,94,157,197,273,362,405,460,542,590,637,697,738,788,814,893,950,997,1067,1099};
  int levelTwoY[] = {0,10,0,20,10,10,20,0,20,0,10,0,10,20,10,20,0,10,0,0,10};
  int levelThreeX[] = {0,30,70,120,175,250,295,340,382,399,429,458,486,529,560,590,631,660,690,723,842,931,990,1067,1100,1130,1300,1429,1450,1460};
  int levelThreeY[] = {0,10,5,1,20,10,10,20,10,0,10,0,0,0,11,10,0,0,3,10,0,17,6,0,18,12,10,20,0,0};
  int numCars;
  
  // Accelerometer variables
  char chPwrCtlReg = 0x2D;
  char chY0Addr = 0x34;

  char 	rgchReadAccl[] = { 0, 0, 0 };
  char 	rgchWriteAccl[] = {0, 0 };
  
  short dataY;

  int	yDirThreshPos = 50;
  int	yDirThreshNeg = -50;

  bool fDir = true;
  bool carDrawn = false;
  
  int switchChange = 0;
  int collided = 0;
  
 
  /*
   * If applicable, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;
  }
  
      /*
     * Enable I2C Peripheral
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
     * Set I2C GPIO pins
     */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
     * Setup I2C
     */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    /* Initialize the Accelerometer
     *
     */
    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    rgchWriteAccl[0] = chPwrCtlReg;
    rgchWriteAccl[1] = 1 << 3;		// sets Accl in measurement mode
    I2CGenTransmit(rgchWriteAccl, 1, WRITE, ACCLADDR);
  
  if (gameStart == 0)
  {
      /* 
       * Level selection
       */
      // Draw the bitmaps of the three numbers
      oledDraw(LvBitmap, 0, 1, 20, 30);
      oledDraw(oneBitmap, 35, 5, 20, 20);
      oledDraw(twoBitmap, 70, 5, 20, 20);
      oledDraw(threeBitmap, 105, 5, 20, 20);
    
      // Use the potentiometer to choose the levels
      char potent=potentiometer();
      if(potent>='A'&& potent<='E')
        oledDraw(oneOppositeBitmap, 35, 5, 20, 20);
      else if(potent>='F'&& potent<='J')
        oledDraw(twoOppositeBitmap, 70, 5, 20, 20);
      else if(potent>='K'&& potent<='P')
        oledDraw(threeOppositeBitmap, 105, 5, 20, 20);
     
      /*
       * Check BTN states. If any button is pressed, start the game with selected levels
       */ 
      lBtn1 = GPIOPinRead(BTN1Port, BTN1);
      lBtn2 = GPIOPinRead(BTN2Port, BTN2);
    
      if(lBtn1 == BTN1 || lBtn2 == BTN2) {
         if(potent>='A'&& potent<='E')
            level = 1;
         else if(potent>='F'&& potent<='J')
            level = 2;
         else if(potent>='K'&& potent<='P')
            level = 3;
         
        gameStart = 1;
        //OrbitOledUpdate();
      }
      
      OrbitOledUpdate();
  }
  
  else
  {
      numCars = level * 10;
         if (level == 1)
        {
          xcoors = levelOneX;
          ycoors = levelOneY;
        }
        else if (level == 2)
        {
            xcoors = levelTwoX;
            ycoors = levelOneY;
        }
        else
        {
            xcoors = levelThreeX;
            ycoors = levelThreeY;
        }
      for(int i = -15; i < xcoors[level*10-1] + 128 && !collided; i += 6)
      { 
        
        OrbitOledClear();
        x=i;
    
        /*___________________________________________________________________*/
        /*
         * Draw the starting car
         */
        if (!carDrawn){
          oledDraw(carBitmap, xcoCarCur, ycoCarCur, carWidth, carHeight);
          OrbitOledUpdate();
    
          carDrawn = true;
        }
    
        /*
         * Read accelerometer information
         */
        rgchReadAccl[0] = chY0Addr;
        I2CGenTransmit(rgchReadAccl, 2, READ, ACCLADDR);
    
        dataY = (rgchReadAccl[2] << 8) | rgchReadAccl[1];
    
        /*
         * Check and see if Accel is positive or negative and set fDir accordingly
         */
        if(dataY > 0 && dataY > yDirThreshNeg) {
          fDir = false;
    
          if(ycoCarCur <= (crowOledMax - 14)) {
            ycoCarCur+=3;
          }
        }
    
        else if(dataY < 0 && dataY < yDirThreshPos) {
          fDir = true;
    
          if(ycoCarCur >= 0) {
            ycoCarCur-=3;
          }
        }
    
        CarMove(xcoCarCur, ycoCarCur);
        
         
    
        /*_________________________________________________________________*/
    
       for(int j = 0; j < numCars && !collided; j++){
          int xcoor = x - xcoors[j];
          if(xcoor >= 0 && xcoor <= 128)
            oledDraw(bitmap, xcoor, ycoors[j], bitmapWidth, bitmapHeight);
    
          if(collision(xcoCarCur, ycoCarCur, xcoor, ycoors[j]))
          {
            collided = 1;
            gameStart = 0;
            break;
          }
    
          OrbitOledUpdate();
    
          bGameState = CheckSwitches(); 
    
        /*
         * If applicabe, reset OLED
         */
        if(fClearOled == true) {
          switchChange = 1;
          OrbitOledClear();
          OrbitOledMoveTo(0,0);
          OrbitOledSetCursor(0,0);
          fClearOled = false;
        }
    
          if (switchChange == 1||collided==1)
          {
            OrbitOledClear();
            break;
          }
        }
          
        delay(70);
        
          if (switchChange == 1)
          {
            OrbitOledClear();
            break;
          }
          if (collided == 1)
          {
            gameStart = 0;
            OrbitOledClear();
            oledDraw(oneOppositeBitmap, 35, 5, 20, 20);
            OrbitOledUpdate();
            delay(1500);
            OrbitOledClear();
            break; 
          }
      }
      if (switchChange == 0  && collided == 0)
         {
            delay(500);
            gameStart = 0;
            OrbitOledClear();
            oledDraw(twoOppositeBitmap, 35, 5, 20, 20);
            OrbitOledUpdate();
            delay(2000);
            OrbitOledClear();
          }
  }
}

/* ------------------------------------------------------------ */
/***	CarMove
 **
 **	Parameters:
 **		xcoUpdate, ycoUpdate
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Moves the car to the specified coordinates
 */
void CarMove (int xcoUpdate, int ycoUpdate) {
  OrbitOledMoveTo(xcoUpdate, ycoUpdate);
  OrbitOledPutBmp(carWidth, carHeight, carBitmap);

  OrbitOledUpdate();
}

/* ------------------------------------------------------------ */
/***	SurvivalScreen
 **
 **	Parameters:
 **		none
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Survival mode of the game
 */
void SurvivalScreen(){
  /*
   * If applicabe, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;
  } 

  OrbitOledUpdate();
}

/*
 * Checks if there is a collision
 */
int collision(int mainx, int mainy, int x, int y)
{
  if(fabs(mainx-x)<15&&fabs(mainy-y)<9)
    return 1;
  return 0;
}


/**
 *   Returns 'A'-'P' depending on the position of potentiometer ('A' for leftmost and 'P' for right most), total of 16 return values possible
 **/
char potentiometer()
{
  uint32_t	ulAIN0;

  char		cMSB = 0x00;

  ADCProcessorTrigger(ADC0_BASE, 0);

  while(!ADCIntStatus(ADC0_BASE, 0, false));

  ADCSequenceDataGet(ADC0_BASE, 0, &ulAIN0);

  cMSB = (0xF00 & ulAIN0) >> 8;
  char a = (cMSB > -1) ? 'A' + (cMSB - 0) : '0' + cMSB;

  return a;
}
