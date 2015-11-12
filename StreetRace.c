extern "C"{
#include <delay.h>
#include <FillPat.h>
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


void setup()
{
  DeviceInit();



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
  //char empty[] {' ',' ',' ',' ',' ',' '};
  char start[] = {'S','t','a','r','t',' ',' ',' ',' ',' ','H','e','l','p'};
  char help[] = {'H','e','l','p'};
   
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
 // OrbitOledSetCursor(3,0);
  //OrbitOledPutString(welcomeTo);
  
  OrbitOledSetCursor(2,1);
  OrbitOledPutString(streetRacer);  
   
  //OrbitOledMoveTo(0,19);
  //OrbitOledLineTo(127, 19);
   
  OrbitOledSetCursor(0, 4);
  OrbitOledPutString(start);
  
}

void HelpScreen(){
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

void GameScreen(){
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
