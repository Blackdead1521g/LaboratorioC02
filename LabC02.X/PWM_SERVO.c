#include "PWM_SERVO.h"

void PWM_config(char canal, float periodo_ms){
    TRISCbits.TRISC1 = 1; //RC1 / CCP2 como entrada
    TRISCbits.TRISC2 = 1; //RC2 / CCP1 como entrada
    PR2 = periodo_ms/(4*(1/8000000)*16);
    if (canal == 1){
            CCP1CONbits.P1M = 0; //PWM mode una salida
            CCP1CONbits.CCP1M = 0b1101; //Modo PWM
            CCPR1L = 0x0f; //Inicio del ciclo de trabajo
            CCP1CONbits.DC1B0 = 0;  
            CCP1CONbits.DC1B1 = 0;  
    }
    else if (canal == 2){
            CCP2CONbits.CCP2M = 0; //PWM mode una salida
            CCP2CONbits.CCP2M = 0b1100;  //Modo PWM  
            CCPR2L = 0x0f; //Inicio del ciclo de trabajo
            CCP2CONbits.DC2B0 = 0;
            CCP2CONbits.DC2B1 = 0;
    }    
    PIR1bits.TMR2IF = 0; //Limpiamos la bandera del TMR2
    T2CONbits.T2CKPS = 0b111; //prescaler 1:16
    T2CONbits.TMR2ON = 1; //Encendemos el TMR2
    while(PIR1bits.TMR2IF == 0); //Esperamos a que se complete un ciclo del TMR2 (hasta que la bandera del TMR2 se encienda)
    PIR1bits.TMR2IF = 0; //Bajamos la bandera del TMR2
    TRISCbits.TRISC1 = 0; //RC1 / CCP2 como salida
    TRISCbits.TRISC2 = 0; //RC2 / CCP1 como salida
    return;
}
void PWM_duty(char canal, float duty){
    char particion = (duty/((1/8000000)*16));
    if (canal == 1){
        CCPR1L = (particion >> 2); //Lo corremos dos bits  a la derecha para ingresarle los 8 bits más significativos
        CCP1CONbits.DC1B0 = (particion&(0b1)); //Hacemos un and 
        CCP1CONbits.DC1B1 = ((particion>>1) &(0b1)); //Hacemos un and
    }
    else if (canal == 2){
        CCPR2L = (particion >> 2); //Lo corremos dos bits  a la derecha para ingresarle los 8 bits más significativos
        CCP2CONbits.DC2B0 = (particion&(0b1)); //Hacemos un and }
        CCP2CONbits.DC2B1 = ((particion>>1) &(0b1)); //Hacemos un and
    }
    return;
}

 