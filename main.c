/*
 *
 * main.c
 *
 */

#include "msp.h"
#include "ADC.h"
#include "UART_P3.h"
#include "DMM.h"
#include "timerA.h"

int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;   // stop WDT
    start(); //setup the ADC, UART,
    while(1){
        //collect measurements and print them to terminal
        print_data();
    }
}
