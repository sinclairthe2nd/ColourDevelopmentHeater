#include <avr/io.h>
#include <util/delay.h>
//Configutations

#define PORT_7_SEGMENT PORTD
#define DDR_7_SEGMENT DDRD
#define DDR_MULTI DDRB
#define PORTMULTI PORTB

// Defines for the SegmentCases that map the Number to the Output Pins
#define CASE0 0b00000010
#define CASE1 0b11001111
#define CASE2 0b00101001
#define CASE3 0b01001001
#define CASE4 0b11000101
#define CASE5 0b01010001
#define CASE6 0b00010001
#define CASE7 0b11001011
#define CASE8 0b00000000
#define CASE9 0b01000001

int NUMBER = 0;

void initADC()
{
  /* this function initialises the ADC
        ADC Prescaler Notes:
	--------------------
	   ADC Prescaler needs to be set so that the ADC input frequency is between 50 - 200kHz.

           For more information, see table 17.5 "ADC Prescaler Selections" in
           chapter 17.13.2 "ADCSRA – ADC Control and Status Register A"
          (pages 140 and 141 on the complete ATtiny25/45/85 datasheet, Rev. 2586M–AVR–07/10)
           Valid prescaler values for various clock speeds

	     Clock   Available prescaler values
           ---------------------------------------
             1 MHz   8 (125kHz), 16 (62.5kHz)
             4 MHz   32 (125kHz), 64 (62.5kHz)
             8 MHz   64 (125kHz), 128 (62.5kHz)
            16 MHz   128 (125kHz)
           Below example set prescaler to 128 for mcu running at 8MHz
           (check the datasheet for the proper bit values to set the prescaler)
  

            8-bit resolution
            set ADLAR to 1 to enable the Left-shift result (only bits ADC9..ADC2 are available)
            then, only reading ADCH is sufficient for 8-bit results (256 values)
*/
  ADMUX =
            (1 << ADLAR) |     // left shift result
            (1 << REFS1) |     // Sets ref. voltage to 1.1V, bit 1
            (1 << REFS0) |     // Sets ref. voltage to 1.1V, bit 0
            (0 << MUX3)  |     // use ADC2 for input (PB4), MUX bit 3
            (0 << MUX2)  |     // use ADC2 for input (PB4), MUX bit 2
            (1 << MUX1)  |     // use ADC2 for input (PB4), MUX bit 1
            (0 << MUX0);       // use ADC2 for input (PB4), MUX bit 0

  ADCSRA =
            (1 << ADEN)  |     // Enable ADC
            (1 << ADPS2) |     // set prescaler to 64, bit 2
            (1 << ADPS1) |     // set prescaler to 64, bit 1
            (0 << ADPS0);      // set prescaler to 64, bit 0
}

void Display(int NUMBER)
{

//This will map the number in number to PORT_7_Segment very elegantly.
int Segmapper[10] = {CASE0,CASE1,CASE2,CASE3,CASE4,CASE5,CASE6,CASE7,CASE8,CASE9};
PORT_7_SEGMENT = Segmapper[NUMBER];

/*
switch (NUMBER)//Maps the number in number to the display rather crudely (its very flexible and readable though)
      {
         case 0:
         PORT_7_SEGMENT=0b00000010; //0b00000010
         break;

         case 1:
         PORT_7_SEGMENT=0b11001111; //0b11001111
         break;

         case 2:
         PORT_7_SEGMENT=0b00101001; //0b00101001
         break;

         case 3:
         PORT_7_SEGMENT=0b01001001; //0b01001001
         break;

         case 4:
         PORT_7_SEGMENT=0b11000101; //0b11000101
         break;

         case 5:
         PORT_7_SEGMENT=0b01010001; //0b01010001
         break;

         case 6:
         PORT_7_SEGMENT=0b00010001; //0b00010001
         break;

         case 7:
         PORT_7_SEGMENT=0b11001011; //0b11001011
         break;

         case 8:
         PORT_7_SEGMENT=0b00000000; //0b00000000
         break;

         case 9:
         PORT_7_SEGMENT=0b01000001; //0b01000001
         break;
      }
   */   
}


void SevenSegment(int INPUTNUMBER)
{
if(INPUTNUMBER > 9) //Conditioning of the Inputnumber
      {
        for (int a=0; a<10;a++)
            {
            NUMBER=INPUTNUMBER%10; //This extracts the last digit using modulo  
            PORTMULTI = (1 << PORTB1)|(0<< PORTB0); // Turn on the first Display
            Display(NUMBER);
            _delay_ms(1);
            NUMBER=INPUTNUMBER/10;  //This Shaves of the last digit of inputnumber
            PORTMULTI = (1<< PORTB0)|(0<<PORTB1);  // Turn on Second Display
            Display(NUMBER);
            _delay_ms(1);
            }
      }
    else
      {
        NUMBER=INPUTNUMBER;
        PORTMULTI = (1 << PORTB0)|(0<< PORTB1); // Turn on the first Display
        Display(NUMBER);
      }
}


int main(void)
{
   //Setup
   DDR_7_SEGMENT=0b11111111;    //All output
   PORT_7_SEGMENT=0xFF;   //All segments off
   DDR_MULTI=0b11111111;  //All Multiplexing  pins are outputs
   DDRC = 0b00000001;
   PORTC = 0;
   initADC();
   while(1)               //loop forever
   {

 

  
 int temp = 0;        // Temperature
 float linearm = 0.68;    // m for the Calibration equation
 float linearb = 6.25;    // b for the Calibration equation
 int setpoint = 0;

 ADCSRA |= (1 << ADSC);         // start ADC measurement
 while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 
 
 temp = ADCH*linearm;
 temp = temp - linearb;
 SevenSegment(temp);

 if(PINC & (1<<PC1))
    {
    setpoint = 30;    
    }
 else
    {
    setpoint = 38;
    }
 if(temp<setpoint) 
    {
    PORTC = (1<< PORTC0);  // Turn Output on  

    }
    else
    {
    PORTC = (0<< PORTC0);  // Turn Output off      
    }


     
   }
}

