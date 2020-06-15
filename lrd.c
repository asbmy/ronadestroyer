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

void init_LCD(void);     //Function to initialise the LCD 
void LCD_command(unsigned char cmd); //Function to pass command to the LCD
void LCD_data(unsigned char data);  //Function to write character to the LCD
void LCD_write_string(static char *str);//Function to write string to the LCD
void msdelay (unsigned int time);  //Function to generate delay

unsigned int Get_ADC_Result(void);  //Function to Get ADC result after conversion
void Start_Conversion(void);   //Function to Start of Conversion

void ADC_Init(void);     //Function to initialize the ADC
void ADC_Init1(void);     //Function to initialize the ADC
void ADC_Init2(void);     //Function to initialize the ADC

void main()
{
    char msg2[] = "Temp:";
    char msg3[] = "made by asmy.dev";
    unsigned char i, Thousands,Hundreds,Tens,Ones;
    unsigned int adc_val, k=0,adc_val11;
    unsigned long Voltage, adc_val3;
    float Voltage1, adc_val1,adc_val2;
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

    ADC_Init();    // Init ADC peripheral
    init_LCD();    // Init LCD Module
    LCD_command (0x8F);   // Goto First line, 16th place of LCD
    msdelay(15);
    LCD_command (0x07);   // Display shift left

    while (1)
    {

        if(k==0) {

            ADC_Init();               // Init ADC peripheral
            LCD_command(0x01);      // clear LCD
            msdelay(15);
            LCD_command(0x06);      // Shift curser right
            msdelay(15);
            LCD_command (0x80);   // Goto first line, 0th place of LCD
            LCD_write_string(msg2); // Display Message
            Start_Conversion();   //Trigger conversion
            adc_val= Get_ADC_Result();//Get the ADC output by polling GO bit
            
            Voltage = (long) adc_val*500.0; //Convert Binary result into temperature
            adc_val = Voltage /1024.0;  
            LCD_command (0x85);   //Goto 5th place on first line of LCD
            i = adc_val/100 ;///Get the Hundreds place
            Hundreds = i + 0x30;  // Convert it to ASCII
            LCD_data (Hundreds);  //Display Hundreds place
            

            i = ((adc_val)%100)/10; //Get the Tens place
            Tens = i + 0x30;   // Convert it to ASCII
            LCD_data (Tens);   //Display Tens place

            i = adc_val%10 ;   //Get the Ones place
            Ones = i + 30;    // Convert it to ASCII

            LCD_data (i + 0x30);  //Display Ones place
            LCD_data ('.'); // Display decimal point
            LCD_data ('0'); // Display 0 digit
            LCD_data (' '); //
            LCD_data ('C'); //
            LCD_data ('e'); //
            LCD_data ('l'); //
            LCD_data ('s'); //
            LCD_data ('.'); //
            k=1;
            msdelay(300);  //Delay between conversions.

        } else if (k == 1) {
        
            ADC_Init1();    // Init ADC peripheral
            LCD_command (0xC0);   // Goto second line, 0th place of LCD
            LCD_write_string(msg3); // Display Message
            Start_Conversion();   //Trigger conversion

                result = Get_ADC_Result();  // Get the humidity
            if (result > 300 && result <800)
            {
            
                // do something

            }
            else
            {
                // do something
            }

            LCD_command (0xC9);   //Goto 9th place on second line of LCD
            i = result/1000 ;  //Get the thousands place
            Thousands = i + 0x30;  // Convert it to ASCII
            LCD_data (Thousands); // Display thousands place
            
            i = (result%1000)/100; //Get the Hundreds place
            Hundreds = i + 0x30;  // Convert it to ASCII
            LCD_data (Hundreds);  //Display Hundreds place
            
            i = (result%100)/10; //Get the Tens place
            Tens = i + 0x30;   // Convert it to ASCII
            LCD_data (Tens);   //Display Tens place
            

            i = result%10 ;   //Get the Ones place
            Ones = i + 30;    // Convert it to ASCII
            LCD_data (i + 0x30);  //Display Ones place

            LCD_data ('d'); // Display d
            LCD_data ('e'); // Display e
            LCD_data ('c'); // Display c

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

void ADC_Init()
{
ADCON0=0b00000000; //A/D Module is OFF and Channel 0 is selected
ADCON1=0b00001110; // Reference as VDD & VSS, AN0 set as analog pins
ADCON2=0b10001110; // Result is right Justified, 2TAD, FOSC/64
ADCON0bits.ADON=1; //Turn ON ADC module
}

void ADC_Init1()
{
ADCON0=0b00000100; //A/D Module is OFF and Channel 1 is selected
ADCON1=0b00001101; // Reference as VDD & VSS, AN1 set as analog pins
ADCON2=0b10001110; // Result is right Justified, 2TAD, FOSC/64
ADCON0bits.ADON=1; //Turn ON ADC module
}

void Start_Conversion()
{
 ADCON0bits.GO=1;
}

unsigned int Get_ADC_Result()
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

void init_LCD(void)         // Function to initialise the LCD
{
    LCD_command(0x38);      // initialization of 16X2 LCD in 8bit mode
    msdelay(15);
    LCD_command(0x01);      // clear LCD
    msdelay(15);
    LCD_command(0x0C);      // cursor off
    msdelay(15);
    LCD_command(0x06);      // curser right shift
    msdelay(15);
}

void LCD_command(unsigned char cmd) //Function to pass command to the LCD
{
    LCD_DATA = cmd;  //Send data on LCD data bus
    rs = 0;    //RS = 0 since command to LCD
    rw = 0;    //RW = 0 since writing to LCD
    en = 1;    //Generate High to low pulse on EN
    msdelay(1);
    en = 0;
}

void LCD_data(unsigned char data)//Function to write data to the LCD
{
    LCD_DATA = data; //Send data on LCD data bus
    rs = 1;    //RS = 1 since data to LCD
    rw = 0;    //RW = 0 since writing to LCD
    en = 1;    //Generate High to low pulse on EN
    msdelay(1);
    en = 0;
}
//Function to write string to LCD
void LCD_write_string(static char *str)   
{
   int i = 0;
   while (str[i] != 0)
    {
    LCD_data(str[i]);      // sending data on LCD byte by byte
    msdelay(1);
    i++;
    }
}

// © Ahmad Siraj MY 2020
