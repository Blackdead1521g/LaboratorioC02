# 1 "PWM_LED.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files/Microchip/MPLABX/v6.05/packs/Microchip/PIC16Fxxx_DFP/1.3.42/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "PWM_LED.c" 2
# 1 "./PWM_LED.h" 1
# 11 "./PWM_LED.h"
float Mapeo_Pot(int valor_potenciometro);
int Duty_Pot(int periodo, float PotMapeado);
void Duty_Cicle(int i, int ciclo, char LED_PIN);
# 1 "PWM_LED.c" 2


float Mapeo_Pot(int valor_potenciometro){
    return ((0.3921568627*valor_potenciometro)/100);
}

int Duty_Pot(int periodo, float PotMapeado){
    return (periodo * PotMapeado);
}

void Duty_Cicle(int i, int ciclo, char LED_PIN){
    i++;
    if (i <= ciclo){
        LED_PIN = 1;
    }
    else{
        LED_PIN = 0;
        i = 0;
    }
    return;
}
