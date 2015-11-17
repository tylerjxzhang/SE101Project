extern "C"{
#include "functions.h"
#include <delay.h>
#include <FillPat.h>
#include <I2CEEPROM.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
}

/* ------------------------------------------------------------ */
/*				Local Type Definitions		*/
/* ------------------------------------------------------------ */
#define WELCOME		0
#define HELP		2
#define GAME		1
#define OVER		3
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

/* ------------------------------------------------------------ */
/*				Global Variables		*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Local Variables			*/
//* ------------------------------------------------------------ */
char	chSwtCur;
char	chSwtPrev;
bool	fClearOled;

/*
 * Car Definitions
 */
 
const unsigned int bitmapHeight =9;  // The height of the bitmap we're about to create
const unsigned int bitmapWidth = 15;   // The width of the bitmap we're about to create
 
 // Now create the bitmap (make sure it has the correct width and height as specified above)
// To make it simple, we're using binary. It's simple - 1 means LED is on. 0=off.
// You can make the bitmap as big as you want (within reason) and draw/write anything you
// want here. Just remember to change the variables above and the array size as you do so.
char bmp[][15] = 
{ 
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0        }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1        }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1        }
  ,
  {
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1        }
  ,
  {
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1        }
  ,
  {
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1        }
  ,
  {
    1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1        }
  ,
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1        }
  ,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0       }
  ,
};
// This bitmap represents the initials "KS" (look at the pattern of 1s
// above to see why.)

// Since this bitmap clearly does not cover the entire 128 by 32 OLED screen
// we need to specify the co-ordinates where we draw the bitmap.
// These co-ordinates correspond to the TOP LEFT of the bitmap,
// so if you wanted to draw this at the very top left of the screen, use (0, 0).
unsigned int x = 0;
unsigned int y = 0;
 

// Define the top left corner of rocket
int	xcoRocketStart 	= 48; //8*6
int	ycoRocketStart	= 11;

int	xcoExhstStart	= 39;
int	ycoExhstStart	= 11;

int	cRocketWidth 	= 24;
int	cRocketHeight 	= 16;

int	cExhstWidth	= 9;
int	cExhstHeight	= 16;

int	fExhstSwt	= 0;

char	rgBMPRocket[] = {
  0xFF, 0x11, 0xF1, 0x11, 0xF1, 0x12, 0x14, 0x18,
  0x90, 0x10, 0x10, 0x10, 0x10, 0x10, 0x90, 0x10,
  0x10, 0xE0, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80,
  0xFF, 0x88, 0x8F, 0x88, 0x8F, 0x48, 0x28, 0x19,
  0x0A, 0x09, 0x08, 0x08, 0x08, 0x09, 0x0A, 0x09,
  0x08, 0x07, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01};

char	rgBMPExhst1[] = {
  0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0F};

char	rgBMPExhst2[] = {
  0x00, 0x80, 0x80, 0xC0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0,
  0x00, 0x01, 0x01, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x0F};


/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */
void DeviceInit();
char CheckSwitches();

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
char* bitmap = formattedBitmap((char*)bmp, bitmapWidth, bitmapHeight);
// ---------------------------------------------------------------------------//

void setup()
{
  DeviceInit();
  
  // Reset OLED (function from functions.cpp)
  //oledReset();
  OrbitOledUpdate();



}

void loop()
{
  char bGameState = 0;
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
  case OVER:
    OverScreen();
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

void WelcomeScreen() {
  
  char welcomeTo[] = {'W','e','l','c','o','m','e',' ','t', 'o'};
  char streetRacer[] = {
  'S','t','r','e','e','t',' ','R','a','c','e','r'};
  //char start[] = {'S','t','a','r','t',' ',' ',' ',' ',' ','H','e','l','p'};
  //char help[] = {'H','e','l','p'};
   
   char a[] = {'a', 'a', 'a', 'a'};
   char b[] = {'b'};
   char c[] = {'c'};
   char d[] = {'d'};
   
   
  /*
   * If applicabe, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;
  } 
  
//    OrbitOledSetCursor(0,0);
//  OrbitOledPutString(a);
//    OrbitOledSetCursor(0,1);
//  OrbitOledPutString(b);
//    OrbitOledSetCursor(0,2);
//  OrbitOledPutString(c);
//    OrbitOledSetCursor(0,3);
//  OrbitOledPutString(help);
//   // OrbitOledSetCursor(9,3);
//   // OrbitOledPutString(help);
  
  /*
   * Display welcome screen
   */
  OrbitOledSetCursor(3,0);
  OrbitOledPutString(welcomeTo);
  
  OrbitOledSetCursor(2,1);
  OrbitOledPutString(streetRacer);  
   
  //OrbitOledMoveTo(0,19);
  //OrbitOledLineTo(127, 19);
   
  //OrbitOledSetCursor(0, 4);
  //OrbitOledPutString(start);
  
}

void HelpScreen(){
  
  // Strings "Tilt the board to move the car"
  // "Press any button to pause"
  char line1[] = {'T', 'i', 'l', 't', ' ', 't', 'h', 'e', ' ', 'b', 'o', 'a', 'r', 'd', ' ', ' ', 't', 'o', ' ', 'm', 'o', 'v', 'e', ' ', 't', 'h', 'e', ' ', 'c', 'a', 'r'};
  char line2[] = {'P', 'r', 'e', 's', 's', ' ', 'a', 'n', 'y', ' ', 'b', 'u', 't', 't', 'o', 'n', ' ', 't', 'o', ' ', 'p', 'a', 'u', 's', 'e'};

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
  
  // Strings "level selection"
  //char levelSelections[] = {};
  
    /*
   * If applicabe, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;
  } 
  
  for(int i=10;i<128;i+=10)
  { 
    OrbitOledClear();
    x=i;
    int numCars=7;
    int xcoors[]={10,40,49,59,79,82,97    };
   int ycoors[]={0,20,20,10,0,0,20    };
      
    for(int j=0;j<numCars;j++){
      int xcoor=x-xcoors[j];
      if(xcoor>=0)
      oledDraw(bitmap, xcoor, ycoors[j], bitmapWidth, bitmapHeight);
      OrbitOledUpdate();
      delay(80);
    }
  }
  oledReset();
  
}

void OverScreen(){
    /*
   * If applicabe, reset OLED
   */
  if(fClearOled == true) {
    OrbitOledClear();
    OrbitOledMoveTo(0,0);
    OrbitOledSetCursor(0,0);
    fClearOled = false;
  } 
}