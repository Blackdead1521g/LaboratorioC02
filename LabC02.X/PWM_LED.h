/* 
 * File:   PWM_LED.h
 * Author: Black
 *
 * Created on 12 de abril de 2023, 10:39 PM
 */

#ifndef PWM_LED_H
#define	PWM_LED_H

float Mapeo_Pot(int valor_potenciometro);
int Duty_Pot(int periodo, float PotMapeado);
void Duty_Cicle(int i, int ciclo, char LED_PIN);

#endif	/* PWM_LED_H */

