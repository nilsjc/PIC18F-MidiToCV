/*
 * File:   main.c
 * Author: Nils Edvardsson
 *
 * Created on den 6 september 2020, 20:35
 */
// PIC18F4620 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
// variables
unsigned char MIDI_CHANNEL = 0; // selected midi channel
unsigned char waiting = 0;      // this is a rough way to check data bytes in correct order
unsigned char velocity = 0;
unsigned char pitch = 0;
unsigned char oldPitch = 0;     // used when checking note of
unsigned char busyPlaying = 0;  // for filtering other notes if one is playing, should be changed
unsigned char buffer[32];      // kind of midi buffer, could probably be much smaller
unsigned int index = 0;         // index is used to indicate the use of the buffer
unsigned char noteRecently = 0; // for checking if the previous status was note on
unsigned char incomingMidiChannel = 0;  // used when to check which midi channel is playing

void helloThere(void)
{
    unsigned int x;
    PORTBbits.RB1 = 1;
    for(x=0; x<0xFFFF; x++){
        // do nothing
    }
    PORTBbits.RB1 = 0;
}

char uartRead(){
    while(!RCIF){
        // do nothing
    }
    return RCREG;
}

void processMidimessage(unsigned char* midiMessage)
{
    // if this two bytes of no interest is detected: return
    if(*midiMessage==0xFE)
    {
        return;
    }
    else if(*midiMessage==0xF8)
    {
        return;
    }
    
    // this is to check if incoming byte is status or data
    unsigned char dataBit = 0;
    dataBit = *midiMessage & 0b10000000;
    
    if(dataBit > 0)
    {   // it is a status byte
            incomingMidiChannel = *midiMessage & 0b00001111;
            noteRecently = 0;
            if(*midiMessage == 0x90)   // note on
            {
                waiting = 3;   
            }else
            {  
            }
        
        return;
        
    }else{
        // it is data byte
        // check if it is correct midi channel,
        // otherwise return
        if(incomingMidiChannel != MIDI_CHANNEL)
        {
            return;
        }
        // was the last status byte a note on?
        // then the data byte is note information
        if(noteRecently == 1)
        {
            waiting = 3;
        }
        
        if(waiting == 3)
        {
            pitch = *midiMessage;// take key
            waiting = 2;
            return;
        }
        
        if(waiting == 2)
        {
            noteRecently = 1;
            velocity = *midiMessage;// take velocity
            if(velocity > 0)
            {
                if(busyPlaying == 0)
                {
                    oldPitch = pitch;
                    busyPlaying = 1;
                    PORTBbits.RB0 = 1;
                    PORTD = pitch;
                }
                waiting = 0;
                return;
                
            }else{ 
                if (pitch == oldPitch)
                {
                    PORTBbits.RB0 = 0;
                    busyPlaying = 0;
                    waiting = 0;
                    return;
                }}
            
        } 
    }
}
void __interrupt() myISR()
{
    if(RCIF == 1)
    {
        unsigned char incomingMessage = uartRead();
        buffer[index] = incomingMessage;
        index++;
        if(index>31)
        {
            // buffer overflow
            // feel free to experiment ;-)
            helloThere();
            index = 31;
        }
    }
}


void main(void) {
    OSCCON=0x70;         // Select 8 MHz internal clock
    INTCONbits.GIE=1;    // Enable all unmasked global INT
    INTCONbits.PEIE=1;   // Enable Peripheral interrupt
    PIR1bits.RCIF=1;
    PIE1bits.RCIE=1;     // enable usart receive interrupt
    
    SWDTEN = 0;          // disable watchdog timer
    
    TRISC = 0b10000000;  // Set All on PORTC as Output
    TRISB = 0b00000000;
    TRISD = 0b00000000;
    PORTC = 0x00;        // Turn Off all PORTC
    PORTB = 0x00;
    SPBRG = 15;           // set correct baud rate
    BRGH = 1;
    TX9 = 0;
    SYNC = 0;
    SPEN = 1;
    CREN = 1;
    TXEN = 1;
    
    
    helloThere();
    // actual program
    for(;;)
    {
        // endless loop
        if(index>2) // something is in the buffer
        {
            unsigned int scan;
            for(scan=0; scan<index; scan++) // scan thru the buffer
            {
                processMidimessage(&buffer[scan]);
                
            }
            index = 0;  // we have checked the buffer and set index from start again

        }
        
    }
    return;
}
