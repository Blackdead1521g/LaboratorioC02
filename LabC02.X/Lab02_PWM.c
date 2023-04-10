 /* Archivo: PostLab01.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarcón
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Convertir una señal analógica a digital
 * Hardware: Potenciómetros en RA0 Y RA5; 3 displays en RD0, RD1 Y RD2
 * 
 * Creado: 27 de marzo, 2023
 * Última modificación: 30 de marzo, 2023
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//---------------------Librerías----------------------------------
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pic16f887.h>


#define _tmr0_value 255 // valor de tmr0 para la interupcion 
#define _XTAL_FREQ 8000000 //definimos la frecuencia del oscilador

//---------------------Variables---------------------------------
char turno;
                           //0     1      2     3     4
const char num_display[] = {0xFC, 0x60, 0xDA, 0xFE, 0x66,
                            0xB6, 0xBE, 0xE0, 0xFE, 0xF6};
                           // 5     6     7     8     9

//-------------------Prototipos de funciones------------------
void setup(void);

//----------------------Interrupciones---------------------------
void __interrupt() isr(void) {
    if (PIR1bits.ADIF) { //Si se activa la bandera de interrupcion del ADC
        if (turno == 1){ //Si está en ADC AN0
            CCPR1L = ((ADRESH >> 1) + 124); //Asignar el PORTB el valor del potenciomnetro del PORTA0
        }
        else if (turno == 0){ //Si está en ADC AN4
            CCPR2L = ((ADRESH >> 1) + 124); //Asignar a la variable potenciometro el valor del potenciometro del PORTA5
        }
        PIR1bits.ADIF = 0; //Limpiar la bandera de la interrupcion del ADC
    }
    return; 
}

void main(void) {
    setup (); 
    ADCON0bits.GO = 1; //Activamos la lectura del ADC
    while(1){ //loop forever
        if (ADCON0bits.GO == 0) { // Si la lectura del ADC se desactiva
            if(ADCON0bits.CHS == 0b0000) //Revisamos si el canal esta en el AN0
            {
                turno = 0;
                ADCON0bits.CHS = 0b0100; //Si, sí está cambiamos el ADC al canal AN4
            }
            else if(ADCON0bits.CHS == 0b0100) //Revisamos si el canal esta en el AN4
            {
                turno = 1;
                ADCON0bits.CHS = 0b0000; //Si, sí está cambiamos el ADC al canal AN0
            }
            __delay_us(1000); //Este es el tiempo que se dará cada vez que se desactiva la lectura
            ADCON0bits.GO = 1; //Activamos la lectura del ADC
        }
    }
    return;
}


void setup(void){
    //definir digitales
    ANSELbits.ANS0 = 1; //Seleccionamos solo los dos pines que utilizaremos como analógicos
    ANSELbits.ANS4 = 1;
    ANSELH = 0; 
    
    //Definimos puertos que serán salidas
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    
    //Limpiamos los puertos
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    //////////////oscilador
    OSCCONbits.IRCF = 0b111 ; ///8Mhz
    OSCCONbits.SCS = 1; //Utilizar oscilados interno
    

    /////////////// tmr0
    OPTION_REGbits.T0CS = 0; //Usar Timer0 con Fosc/4
    OPTION_REGbits.PSA = 0; //Prescaler con el Timer0
    OPTION_REGbits.PS2 = 1; //Prescaler de 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;  
    TMR0 = _tmr0_value; ///VALOR INICIAL DEL TMR0
    
    /////////Banderas e interrupciones
    INTCONbits.GIE = 1; //Habilitar interrupciones globales
    INTCONbits.PEIE = 1; //Habilitar interrupciones periféricas
    PIE1bits.ADIE = 1; //Habilitar interrupciones del ADC
    PIR1bits.ADIF = 0; //Apagamos la bandera del ADC
    ADCON0bits.GO = 1; //Activamos la lectura del ADC
    
        //Configuración ADC
    ADCON0bits.CHS = 0b0000; //Elegimos canal RA0 como inicial
    ADCON1bits.VCFG0 = 0; //Voltaje referenciado de 0V
    ADCON1bits.VCFG1 = 0; //Voltaje referenciado de 5V
    ADCON0bits.ADCS = 0b10; // Fosc/32
     
    ADCON1bits.ADFM = 0; //Justificado a la izquierda
    __delay_ms(1); 
    ADCON0bits.ADON = 1;//Encendemos el módulo del ADC
    
    //////Configuración PWM
    TRISCbits.TRISC1 = 1; //RC1 / CCP2 como entrada
    TRISCbits.TRISC2 = 1; //RC2 / CCP1 como entrada
    
    PR2 = 255; //Valor del PWM periodo
    
    CCP1CONbits.P1M = 0; //PWM mode una salida
    CCP1CONbits.CCP1M = 0b1100; //Modo PWM    
    
    CCP2CONbits.CCP2M = 0; //PWM mode una salida
    CCP2CONbits.CCP2M = 0b1100;  //Modo PWM  
    
    CCPR1L = 0x0f; //Inicio del ciclo de trabajo
    CCPR2L = 0x0f; //Inicio del ciclo de trabajo
    
    CCP1CONbits.DC1B = 0; 
    CCP2CONbits.DC2B0 = 0;
    CCP2CONbits.DC2B1 = 0;
    
    PIR1bits.TMR2IF = 0; //Limpiamos la bandera del TMR2
    T2CONbits.T2CKPS = 0b111; //prescaler 1:16
    T2CONbits.TMR2ON = 1; //Encendemos el TMR2
    while(PIR1bits.TMR2IF == 0); //Esperamos a que se complete un ciclo del TMR2 (hasta que la bandera del TMR2 se encienda)
    PIR1bits.TMR2IF = 0; //Bajamos la bandera del TMR2
    TRISCbits.TRISC1 = 0; //RC1 / CCP2 como salida
    TRISCbits.TRISC2 = 0; //RC2 / CCP1 como salida
    
    turno = 1;
    return;
}
