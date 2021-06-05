/*
 *
 * main.c
 *
 */

#include "msp.h"
#include "ADC.h"
#include "DMM.h"


int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;   // stop WDT
    start(); //setup the ADC, UART,
    while(1){
      if(check_timerflag()){
           get_frequency();
       }
      //collect measurements and print them to terminal
       if(check_ADCflag())
           get_ADCdata();
      print_data();
    }
}
