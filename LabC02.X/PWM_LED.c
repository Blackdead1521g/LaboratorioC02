#include "PWM_LED.h"

float Mapeo_Pot(int valor_potenciometro){
    return ((0.39215686f*valor_potenciometro)/100);
}

int Duty_Pot(int periodo, float PotMapeado){
    return (int)(periodo * PotMapeado);
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



