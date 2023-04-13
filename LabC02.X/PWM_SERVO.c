#include "PWM_SERVO.h"
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000
#endif
void PWM_config(char canal, float periodo_ms){
    PR2 = (char) (periodo_ms/(4*(1.0f/_XTAL_FREQ)*16));   //mejor usar la constante_Xtal_freq 
    if (canal == 1){
        TRISCbits.TRISC1 = 1; //RC1 / CCP2 como entrada
        CCP1CONbits.P1M = 0; //PWM mode una salida
        CCP1CONbits.CCP1M = 0b1100; //Modo PWM - no hay diferencia pero asi el codigo parece mas al del ccp2
        CCPR1L = 0x0f; //Inicio del ciclo de trabajo
        CCP1CONbits.DC1B0 = 0;  
        CCP1CONbits.DC1B1 = 0;  
    }
    else if (canal == 2){
        TRISCbits.TRISC2 = 1; //RC2 / CCP1 como entrada
        //CCP2CONbits.CCP2M = 0; //PWM mode una salida no es necesario
        CCP2CONbits.CCP2M = 0b1100;  //Modo PWM  
        CCPR2L = 0x0f; //Inicio del ciclo de trabajo
        CCP2CONbits.DC2B0 = 0;
        CCP2CONbits.DC2B1 = 0;
    }    
    PIR1bits.TMR2IF = 0; //Limpiamos la bandera del TMR2
    T2CONbits.T2CKPS = 0b11; //prescaler 1:16  - solo son 2 bits, no 3
    T2CONbits.TMR2ON = 1; //Encendemos el TMR2
    while(PIR1bits.TMR2IF == 0); //Esperamos a que se complete un ciclo del TMR2 (hasta que la bandera del TMR2 se encienda)
    PIR1bits.TMR2IF = 0; //Bajamos la bandera del TMR2
    if (canal == 1)
        TRISCbits.TRISC1 = 0;
    else 
        TRISCbits.TRISC2 = 0; 
    return;
}
void PWM_duty(char canal, float duty){
    int particion = (int)(4*duty/((1.0f/_XTAL_FREQ)*16));
    if (canal == 1){
        CCPR1L = (char)(particion >> 2); //Lo corremos dos bits  a la derecha para ingresarle los 8 bits m�s significativos
        CCP1CONbits.DC1B0 = (particion&(0b1)); //Hacemos un and 
        CCP1CONbits.DC1B1 = ((particion>>1) &(0b1)); //Hacemos un and
    }
    else if (canal == 2){
        CCPR2L = (char)(particion >> 2); //Lo corremos dos bits  a la derecha para ingresarle los 8 bits m�s significativos
        CCP2CONbits.DC2B0 = (particion&(0b1)); //Hacemos un and }
        CCP2CONbits.DC2B1 = ((particion>>1) &(0b1)); //Hacemos un and
    }
    return;
}

 