#include <msp430.h>
#include "lcd.h"

// LCD Ports

#define DAT_PORT    P1
#define D4          BIT4
#define D5          BIT5
#define D6          BIT2
#define D7          BIT3
#define RS_PORT     P2
#define RS          BIT2
#define EN_PORT     P4
#define EN          BIT0

// Temp sensor constants

#define CALADC_15V_30C  *((unsigned int *)0x1A1A)                 // Temperature Sensor Calibration-30 C
                                                                  // See device datasheet for TLV table memory mapping
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)                 // Temperature Sensor Calibration-High Temperature (85 for Industrial, 105 for Extended)

volatile float ADC_Result[4];                                    // 12-bit ADC conversion result array
char temperature_string[30];
volatile float IntDegF;
volatile float IntDegC;

volatile unsigned int hundreds, tens, units, tenths, hundredths, thousandths, tenthousandths;
volatile long int remainder;
volatile float temperature;
volatile long int temp;

int relay_delay = 0;
int transfer_delay = 500;

// general strings used for LCD

char* water_string = "Water:";
char* yes = "yes!";
char* no = "no!";

char motion_string_true[] = {0x6D, 0x6F, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x59, 0x65, 0x73, 0x21, '\0'};
char motion_string_false[] = {0x6D, 0x6F, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x4E, 0x6F, '\0'};

int m=0;
char result[100];
int count;

void port_init()
{
    // Configure output to relay and buzzer
    P5DIR |= BIT0;
    P2DIR |= BIT0;

    P1DIR |= BIT0;
    P1OUT |= BIT0;
    P1SEL0 |= BIT6 | BIT7;                    // set 2-UART pin as second function
    P4SEL0 |= BIT2 | BIT3;                    // set 2-UART pin as second function
    P4SEL1 &= ~BIT2;                    // set 2-UART pin as second function
    P4SEL1 &= ~ BIT3;                    // set 2-UART pin as second function

    // Configure ADC Pins, in this case we're using 5.1 and 5.3
    P5SEL0 |=  BIT1 + BIT3;
    P5SEL1 |=  BIT1 + BIT3;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

}

void uart_init(void){
    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 8;                             // 115200
    UCA1MCTLW = 0xD600;
    UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void ConfigClocks(void)
{
    CSCTL3 = SELREF__REFOCLK;               // Set REFO as FLL reference source
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_0;// DCOFTRIM=3, DCO Range = 1MHz
    CSCTL2 = FLLD_0 + 30;                   // DCODIV = 1MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                // Enable FLL
    Software_Trim();                        // Software Trim to get the best DCOFTRIM value
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                               // default DCODIV as MCLK and SMCLK source
}

void strreverse(char* begin, char* end)      // Function to reverse the order of the ASCII char array elements
{
    char aux;
    while(end>begin)
        aux=*end, *end--=*begin, *begin++=aux;
}



void itoa(int value, char* str, int base) {  //Function to convert the signed int to an ASCII char array
    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* wstr=str;
    int sign;

    // Validate that base is between 2 and 35 (inlcusive)
    if (base<2 || base>35){
        *wstr='\0';
        return;
    }

    // Get magnitude and th value
    sign=value;
    if (sign < 0)
        value = -value;

    do // Perform interger-to-string conversion.
        *wstr++ = num[value%base]; //create the next number in converse by taking the modolus

    while(value/=base);  // stop when you get  a 0 for the quotient
    if(sign<0) //attch sign character, if needed
        *wstr++='-';
    *wstr='\0'; //Attach a null character at end of char array. The string is in revers order at this point
    strreverse(str,wstr-1); // Reverse string
}

void Software_Trim()
{
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3;
    unsigned char endLoop = 0;

    do
    {
        CSCTL0 = 0x100;                         // DCO Tap = 256
        do
        {
            CSCTL7 &= ~DCOFFG;                  // Clear DCO fault flag
        }while (CSCTL7 & DCOFFG);               // Test DCO fault flag

        //__delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ);// Wait FLL lock status (FLLUNLOCK) to be stable
                                                           // Suggest to wait 24 cycles of divided FLL reference clock
        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;                   // Read CSCTL0
        csCtl1Read = CSCTL1;                   // Read CSCTL1
        oldDcoTap = newDcoTap;                 // Record DCOTAP value of last time
        newDcoTap = csCtl0Read & 0x01ff;       // Get DCOTAP value of this time
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4;// Get DCOFTRIM value

        if(newDcoTap < 256)                    // DCOTAP < 256
        {
            newDcoDelta = 256 - newDcoTap;     // Delta value between DCPTAP and 256
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256)) // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }
        else                                   // DCOTAP >= 256
        {
            newDcoDelta = newDcoTap - 256;     // Delta value between DCPTAP and 256
            if(oldDcoTap < 256)                // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }

        if(newDcoDelta < bestDcoDelta)         // Record DCOTAP closest to 256
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0);                      // Poll until endLoop == 1
    CSCTL0 = csCtl0Copy;                       // Reload locked DCOTAP
    CSCTL1 = csCtl1Copy;                       // Reload locked DCOFTRIM

    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Poll until FLL is locked
}

void general_ADC_init()
{
    // Configure ADC12
   ADCCTL0 |= ADCSHT_10 | ADCON;                             // ADCON, S&H=16 ADC clks
   ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer
   ADCCTL2 &= ~ADCRES;                                      // clear ADCRES in ADCCTL
   ADCCTL2 |= ADCRES_2;                                     // 12-bit conversion results
}

void sample_motion()
{
    // Configure ADC for motion sensor

    ADCMCTL0 &= ~ADCSREF_1;
    ADCCTL0 &= ~ADCENC;     // Disable
    ADCMCTL0 = ADCINCH_11;  // ADC Channel for sensor 1 (11)
    ADCCTL0 |= ADCENC | ADCSC;      // Re-Enable and start another
    while (ADCCTL1 & ADCBUSY) /*EMPTY*/; // Spin until complete
    ADC_Result[0] = ADCMEM0;    // Capture result
    ADCCTL0 &= ~ADCENC;        // Disable so we can change INCH later.
}

void sample_water()
{
    // Configure ADC water sensor

    ADCCTL0 &= ~ADCENC;     // Disable
    ADCMCTL0 = ADCINCH_9;  // ADC channel for sensor 2 (9)
    ADCCTL0 |= ADCENC | ADCSC;      // Re-Enable and start another
    while (ADCCTL1 & ADCBUSY) /*EMPTY*/; // Spin until complete (not very long)
    ADC_Result[1] = ADCMEM0;    // Capture result
    ADCCTL0 &= ~ADCENC;        // Disable so we can change INCH later.
}

void sample_temp()
{
    ADCMCTL0 &= ~ADCINCH_9;
    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;
    // Configure reference
    PMMCTL0_H = PMMPW_H;                                          // Unlock the PMM registers
    PMMCTL2 |= INTREFEN | TSENSOREN;                              // Enable internal reference and temperature sensor
    __delay_cycles(400);
    ADCCTL0 |= ADCENC | ADCSC;      // Re-Enable and start another
    while (ADCCTL1 & ADCBUSY) /*EMPTY*/; // Spin until complete (not very long)
    temp = ADCMEM0;    // Capture result
    // Temperature in Celsius
    // The temperature (Temp, C)=
    IntDegC = ((temp-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30)-5;

    // Temperature in Fahrenheit
    // Tf = (9/5)*Tc | 32
    IntDegF = 9*IntDegC/5+32;

    ADC_Result[2] = IntDegC;
    ADC_Result[3] = IntDegF;

    ADCCTL0 &= ~ADCENC;        // Disable so we can change INCH later.

}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                   // Stop WDT

    P1DIR = BIT6; // P1.6 outputs
    P1OUT = 0; // LEDs off
    ConfigClocks();
    port_init();
    uart_init();
    port_init();
    general_ADC_init(); // ADC settings common throughout all sensors
    lcd_init(DAT_PORT, D4, D5, D6, D7, RS_PORT, RS, EN_PORT, EN);


    while(1)
    {
        sample_temp();
        sample_motion();
        sample_water();

        if(transfer_delay == 500)
        {
            _delay_cycles(20000);
            int acount =0;
            result[acount]='B';

            while((UCA1IFG & UCTXIFG)==0);
            UCA1TXBUF = result[acount] ;                   //Transmit the received data.

            itoa(ADC_Result[2],result,10);
            acount = 0;

            while(result[acount]!='\0')
            {
              while((UCA1IFG & UCTXIFG)==0);                                    //Wait Unitl the UART transmitter is ready //UCTXIFG
              UCA1TXBUF = result[acount++] ;                   //Transmit the received data.
            }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            _delay_cycles(20000);
            result[acount]='B';

            while((UCA1IFG & UCTXIFG)==0);
            UCA1TXBUF = result[acount] ;                   //Transmit the received data.

            itoa(ADC_Result[0],result,10);
            acount = 0;

            while(result[acount]!='\0')
            {
              while((UCA1IFG & UCTXIFG)==0);                                   //Wait Unitl the UART transmitter is ready //UCTXIFG
              UCA1TXBUF = result[acount++] ;                   //Transmit the received data.
            }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            _delay_cycles(20000);
            result[acount]='B';

            while((UCA1IFG & UCTXIFG)==0);
            UCA1TXBUF = result[acount] ;                   //Transmit the received data.

            itoa(ADC_Result[1],result,10);
            acount = 0;

            while(result[acount]!='\0')
            {
              while((UCA1IFG & UCTXIFG)==0);                                    //Wait Unitl the UART transmitter is ready //UCTXIFG
              UCA1TXBUF = result[acount++] ;                   //Transmit the received data.
            }

            transfer_delay = 0;
        }

        /////////////////////////////////////////////////////////////////////

        // logic for relay and buzzer, printing to LCD

        lcd_setCursor(1,0);

        if(ADC_Result[0] == 4095)
        {
            P2OUT |= BIT0;
            lcd_print(motion_string_true);
        }

        else if(ADC_Result[0] < 4095 && relay_delay > 1000)
        {
            P2OUT &= ~BIT0;
            lcd_print(motion_string_false);
            relay_delay = 0;
        }


        lcd_setCursor(0,8);
        lcd_print(water_string);

        if(ADC_Result[1] < 4095)
        {
            P5OUT |= BIT0;
            lcd_setCursor(1, 13);
            lcd_print(yes);
        }
        else
        {
            P5OUT &= ~BIT0;
            lcd_setCursor(1, 13);
            lcd_print(no);
        }

        lcd_setCursor(0,0);

        // Convert temperature float to char (only ones and tens)
        temperature = ADC_Result[3];
        temperature *= 10000;//250625.
        temp = (long int)temperature;//250625

        tens = temp/100000;//2
        remainder = temp - tens*100000;//50625
        units = remainder/10000;//5
        remainder = remainder - units*10000;//625
        tenths = remainder/1000;//0
        remainder = remainder - tenths*1000;//625
        hundredths = remainder/100;//6

        // ascii conversion add 0x30
        char temperature_string[] = {0x54, 0x65, 0x6D, 0x70, 0x3A, tens+0x30, units+0x30, '\0'};

        lcd_print(temperature_string);

        relay_delay++;
        transfer_delay++;
        continue;
    }

}


