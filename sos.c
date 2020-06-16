// XTAL = 4Mhz, Prescaler = 4, 1Khz frequency PWM waveform

#include <p18f4580.h>
#include <delays.h>

#define gled PORTCbits.RC5 //green LED
#define yled PORTCbits.RC4 //yellow LED
#define rled PORTCbits.RC3 //red LED

#define s1 PORTBbits.RB0 //Switch 1
#define s2 PORTBbits.RB1 //Switch 2
#define s3 PORTBbits.RB2 //Switch 3
#define s4 PORTBbits.RB3 //Switch 4

#define LCD_DATA    PORTD               //LCD data port
#define en          PORTEbits.RE2       // enable signal
#define rw          PORTEbits.RE1       // read/write signal
#define rs          PORTEbits.RE0       // register select signal

#pragma config WDT = OFF				// System setting - Watchdog Timer OFF
#pragma config LVP = OFF				// System setting - Low Voltage Programming OFF
#pragma config PBADEN = OFF			   	// System setting - PORTB<4:0> pins are configured as digital I/O on Reset 
#pragma config OSC = HS                 // Configure Oscilitator
#pragma config PWRT = OFF

void lcd_init(void);     //Function to initialise the LCD 
void lcd_order(unsigned char cmd); //Function to pass command to the LCD
void lcd_char(unsigned char data);  //Function to write character to the LCD
void lcd_write(static char *str);//Function to write string to the LCD
void msdelay (unsigned int time);  //Function to generate delay

unsigned int adcresultget(void);  //Function to Get ADC result after conversion
void convertgo(void);   //Function to Start of Conversion

void init_adc0(void);     //Function to initialize the ADC
void init_adc1(void);     //Function to initialize the ADC

void main()
{
    char info[] = "temp:";
    char info2[] = "made by asmy.dev";
    unsigned char i, thousands,hundreds,tens,ones;
    unsigned int adc_val, k=0,adc_val11;
    unsigned long volt, adc_val3;
    float v1, adc_val1,adc_val2;
    int result,m, j;
    float mV, Temp, Press;
    unsigned int speed;

    while (s1 == 0);

    TRISB = 1;                  // ALL PORT B Input

    TRISC = 0;                  // ALL Port C Output
    TRISE = 0;                  // ALL PORT E Output
    TRISD = 0;                  // ALL PORT D Output

    TRISAbits.TRISA0 = 1;       // RA0 is input
    TRISAbits.TRISA1 = 1;       // RA1 is input
    TRISAbits.TRISA2 = 1;       // RA2 is input

    TRISCbits.TRISC2=0;			// Make PWM pin Output
    CCP1CON = 0;				// Clear CCP1CON
    TMR2 = 0;					// Clear TMR2
    T2CON = 0x01;				// Prescaler = 4
    PR2 = 249;				    // PR2 Value

    ADCON1 = 0x0F;        //Configuring the PORTE pins as digital I/O

    init_adc0();    // Init ADC peripheral
    lcd_init();    // Init LCD Module
    lcd_order (0x8F);   // Goto First line, 16th place of LCD
    msdelay(1);
    lcd_order (0x07);   // Display shift left

    while (1)
    {

        if(k==0) {

            init_adc0();               // Init ADC peripheral
            lcd_order(0x01);      // clear LCD
            msdelay(1);
            lcd_order(0x06);      // Shift curser right
            msdelay(1);
            lcd_order (0x80);   // Goto first line, 0th place of LCD
            lcd_write(info); // Display Message
            convertgo();   //Trigger conversion
            adc_val= adcresultget();//Get the ADC output by polling GO bit
            
            volt = (long) adc_val*500.0; //Convert Binary result into temperature
            adc_val = volt /1024.0;  
            lcd_order (0x85);   //Goto 5th place on first line of LCD
            i = adc_val/100 ;///Get the hundreds place
            hundreds = i + 0x30;  // Convert it to ASCII
            lcd_char (hundreds);  //Display hundreds place
            

            i = ((adc_val)%100)/10; //Get the tens place
            tens = i + 0x30;   // Convert it to ASCII
            lcd_char (tens);   //Display tens place

            i = adc_val%10 ;   //Get the ones place
            ones = i + 30;    // Convert it to ASCII

            lcd_char (i + 0x30);  //Display ones place
            lcd_char ('.'); // Display decimal point
            lcd_char ('0'); // Display 0 digit
            lcd_char (' '); //
            lcd_char ('C'); //
            lcd_char ('e'); //
            lcd_char ('l'); //
            lcd_char ('s'); //
            lcd_char ('.'); //
            k=1;
            msdelay(300);  //Delay between conversions.

        } else if (k == 1) {
        
            init_adc1();    // Init ADC peripheral
            lcd_order (0xC0);   // Goto second line, 0th place of LCD
            lcd_write(info2); // Display Message
            convertgo();   //Trigger conversion

                result = adcresultget();  // Get the humidity
            if (result > 300 && result <800)
            {
            
                // do something

            }
            else
            {
                // do something
            }

            lcd_order (0xC9);   //Goto 9th place on second line of LCD
            i = result/1000 ;  //Get the thousands place
            thousands = i + 0x30;  // Convert it to ASCII
            lcd_char (thousands); // Display thousands place
            
            i = (result%1000)/100; //Get the hundreds place
            hundreds = i + 0x30;  // Convert it to ASCII
            lcd_char (hundreds);  //Display hundreds place
            
            i = (result%100)/10; //Get the tens place
            tens = i + 0x30;   // Convert it to ASCII
            lcd_char (tens);   //Display tens place
            

            i = result%10 ;   //Get the ones place
            ones = i + 30;    // Convert it to ASCII
            lcd_char (i + 0x30);  //Display ones place

            lcd_char ('d'); // Display d
            lcd_char ('e'); // Display e
            lcd_char ('c'); // Display c

            k=0;
            msdelay(300);
        }
        
		PORTDbits.RD0 = 1;

        if ( s2 != 0)
        {   
            gled = 1;
            yled = 0;
            rled = 0;
            speed = 62;
        } else if (s3 != 0)
        {
            gled = 0;
            yled = 1;
            rled = 0;
            speed = 124;
        } else if (s4 != 0)
        {
            gled = 0;
            yled = 0;
            rled = 1;
            speed = 186;
        } 

    CCPR1L = speed;				    // CCPR1L value - 75% = 186, 50% = 124, 25% = 62	
    CCP1CON = 0x1C;			        // PWM Mode, 0b01 for 					// DC1B1:DC1B0
	T2CONbits.TMR2ON=1;		        // Turn ON Timer2
	PIR1bits.TMR2IF=0;			    // Clear Flag
	while (PIR1bits.TMR2IF==0);
    }
}

void init_adc0()
{
ADCON0=0b00000000; //A/D Module is OFF and Channel 0 is selected
ADCON1=0b00001110; // Reference as VDD & VSS, AN0 set as analog pins
ADCON2=0b10001110; // Result is right Justified, 2TAD, FOSC/64
ADCON0bits.ADON=1; //Turn ON ADC module
}

void init_adc1()
{
ADCON0=0b00000100; //A/D Module is OFF and Channel 1 is selected
ADCON1=0b00001101; // Reference as VDD & VSS, AN1 set as analog pins
ADCON2=0b10001110; // Result is right Justified, 2TAD, FOSC/64
ADCON0bits.ADON=1; //Turn ON ADC module
}

void convertgo()
{
 ADCON0bits.GO=1;
}

unsigned int adcresultget()
{
unsigned int ADC_Result=0;
while(ADCON0bits.DONE);
ADC_Result=ADRESL;
ADC_Result|=((unsigned int)ADRESH) << 8;
return ADC_Result;
}

void msdelay (unsigned int time) //Function to generate delay
{
unsigned int i, j;
for (i = 0; i < time; i++)
for (j = 0; j < 710; j++);  //Calibrated for a 1 ms delay in MPLAB
}

void lcd_init(void)         // Function to initialise the LCD
{
    lcd_order(0x38);      // initialization of 16X2 LCD in 8bit mode
    msdelay(1);
    lcd_order(0x01);      // clear LCD
    msdelay(1);
    lcd_order(0x0C);      // cursor off
    msdelay(1);
    lcd_order(0x06);      // curser right shift
    msdelay(1);
}

void lcd_order(unsigned char cmd) //Function to pass command to the LCD
{
    LCD_DATA = cmd;  //Send data on LCD data bus
    rs = 0;    //RS = 0 since command to LCD
    rw = 0;    //RW = 0 since writing to LCD
    en = 1;    //Generate High to low pulse on EN
    msdelay(1);
    en = 0;
}

void lcd_char(unsigned char data)//Function to write data to the LCD
{
    LCD_DATA = data; //Send data on LCD data bus
    rs = 1;    //RS = 1 since data to LCD
    rw = 0;    //RW = 0 since writing to LCD
    en = 1;    //Generate High to low pulse on EN
    msdelay(1);
    en = 0;
}
//Function to write string to LCD
void lcd_write(static char *str)   
{
   int i = 0;
   while (str[i] != 0)
    {
    lcd_char(str[i]);      // sending data on LCD byte by byte
    msdelay(1);
    i++;
    }
}

/* © Ahmad Siraj MY 2020 */
