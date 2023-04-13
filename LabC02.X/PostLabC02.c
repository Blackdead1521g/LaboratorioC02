 /* Archivo: PostLab02.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarcón
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Utilizar los PWM del pic y crear un nuevo PWM para la intensidad de un led
 * Hardware: Potenciómetros en RA0, RA2 y RA5; 2 servo motores en RC1 y RC2, 1 led en RC3
 * 
 * Creado: 10 de abril, 2023
 * Última modificación: 13 de abril, 2023
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
#include "PWM_SERVO.h"
#include "PWM_LED.h"

#define _tmr0_value 98 // valor de tmr0 para que la interrupción sea cada 20ms 
#define _XTAL_FREQ 4000000 //definimos la frecuencia del oscilador
#define canal 2 //Canal 1 del PWM
#define periodo 0.004f //Periodo de 4 ms
#define periodoTotal 20 //Tendremos un periodo de 20 ms para el led
#define ciclo_trabajo 0.0025f //Ciclo de trabajo de 2 ms
#define LED_PIN PORTCbits.RC3 /// usar e0 como salida del led

//---------------------Variables---------------------------------
int i = 0;
int dutyPot = 0;
int periodoPot = 0;
int potenciometro = 0;
float potMapeado = 0;
                           //0     1      2     3     4
/*const char num_display[] = {0xFC, 0x60, 0xDA, 0xFE, 0x66,
                            0xB6, 0xBE, 0xE0, 0xFE, 0xF6};*/
                           // 5     6     7     8     9

//-------------------Prototipos de funciones------------------
void setup(void);
void duty_cicle(int ciclo);

//----------------------Interrupciones---------------------------
void __interrupt() isr(void) {
    if (INTCONbits.T0IF){ //Si se activa la bandera de interrupción del TMR0
        duty_cicle(dutyPot); //Llamaremos a nuestra función de ciclo de trabajo para el led
        INTCONbits.T0IF = 0; //Limpiamos la bandera del TMR0
        TMR0 = _tmr0_value; //Restablecemos el valor del TMR0
    }
    if (PIR1bits.ADIF) { //Si se activa la bandera de interrupcion del ADC
        if (ADCON0bits.CHS == 0b0000){ //Si está en ADC AN0
            PWM_duty(1, ciclo_trabajo*(ADRESH/255.0f)); //Llamamos a nuestra función de ciclo de trabajo
        }
        else if (ADCON0bits.CHS == 0b0100){ //Si está en ADC AN4
            PWM_duty(2, ciclo_trabajo*(ADRESH/255.0f)); //Llamamos a nuestra función de ciclo de trabajo
        }
        else if (ADCON0bits.CHS == 0b0010){//Si está en ADC AN2
            potenciometro = ADRESH; //Asignar a la variable potenciometro el valor del potenciometro del PORTA2
        }
        
        PIR1bits.ADIF = 0; //Limpiar la bandera de la interrupcion del ADC
    }
    return; 
}

void main(void) {
    setup (); 
    ADCON0bits.GO = 1; //Activamos la lectura del ADC
    while(1){ //loop forever
        potMapeado = (0.39215686f*potenciometro)/100; //Esta variable contendrá el valor que tenga el potenciometro en un rango (0-100) en porcentaje
        dutyPot = (int)(periodoTotal * potMapeado); //En el ciclo de trabajo ingresamos el porcentaje del potenciometro multiplicado por el total del periodo 
                                               //para que vaya incrementando/decrementando el ciclo de trabajo conforme se modifique el potenciometro
        PORTB = (unsigned char)dutyPot; //Presentamos el valor del ciclo de trabajo en el PUERTOB para verificar el valor del ciclo conforme se varía el potenciometro
        if (ADCON0bits.GO == 0) { // Si la lectura del ADC se desactiva
            if(ADCON0bits.CHS == 0b0000) //Revisamos si el canal esta en el AN0
            {
                ADCON0bits.CHS = 0b0100; //Si, sí está cambiamos el ADC al canal AN4
            }
            else if(ADCON0bits.CHS == 0b0100) //Revisamos si el canal esta en el AN4
            {
                ADCON0bits.CHS = 0b0010; //Si, sí está cambiamos el ADC al canal AN2
            }
            else if(ADCON0bits.CHS == 0b0010) //Revisamos si el canal esta en el AN2
            {
                ADCON0bits.CHS = 0b0000; //Si, sí está cambiamos el ADC al canal AN0
            }
            __delay_us(1000); //Este es el tiempo que se dará cada vez que se desactiva la lectura
            ADCON0bits.GO = 1; //Activamos la lectura del ADC
        }
    }
    return;
}

void duty_cicle(int ciclo){
    i++; //Incrementamos nuestra variable comparadora
    if (i <= ciclo){ //Si nuestra variable comparadora es menor o igual a nuestro ciclo de trabajo
        LED_PIN = 1; //Que se mantenga encendido nuestro led
    }
    else{
        LED_PIN = 0; //Sino que lo apague
        i = 0; //Reseteamos nuestra variable comparadora
    }
    return;
}

void setup(void){
    //definir digitales
    ANSELbits.ANS0 = 1; //Seleccionamos solo los dos pines que utilizaremos como analógicos
    ANSELbits.ANS2 = 1;
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
    OSCCONbits.IRCF = 0b110 ; ///4Mhz
    OSCCONbits.SCS = 1; //Utilizar oscilados interno
    

    /////////////// tmr0
    OPTION_REGbits.T0CS = 0; //Usar Timer0 con Fosc/4
    OPTION_REGbits.PSA = 0; //Prescaler con el Timer0
    OPTION_REGbits.PS2 = 1; //Prescaler de 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;  
    TMR0 = _tmr0_value; ///VALOR INICIAL DEL TMR0
    
    /////////Banderas e interrupciones
    PIR1bits.ADIF = 0; //Apagamos la bandera del ADC
    INTCONbits.PEIE = 1; //Habilitar interrupciones periféricas
    PIE1bits.ADIE = 1; //Habilitar interrupciones del ADC
    INTCONbits.GIE = 1; //Habilitar interrupciones globales
    ADCON0bits.GO = 1; //Activamos la lectura del ADC
    
        //Configuración ADC
    ADCON0bits.CHS = 0b0000; //Elegimos canal RA0 como inicial
    ADCON1bits.VCFG0 = 0; //Voltaje referenciado de 0V
    ADCON1bits.VCFG1 = 0; //Voltaje referenciado de 5V
    ADCON0bits.ADCS = 0b10; // Fosc/32
     
    ADCON1bits.ADFM = 0; //Justificado a la izquierda
    ADCON0bits.ADON = 1;//Encendemos el módulo del ADC
     __delay_ms(1); 
    
    //////Configuración PWM
    PWM_config(canal, periodo);
    PWM_duty(canal, ciclo_trabajo);
    return;
}