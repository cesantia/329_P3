/*
 *
 * main.c
 *
 */

#include "msp.h"
#include "ADC.h"
#include "DMM.h"

/*
int main(void) {


    WDT_A->CTL = WDT_A_CTL_PW |       // Stop WDT
                 WDT_A_CTL_HOLD;
    start();
     while(1){
         if(check_flag()){
            //Get data and transfer to CCS terminal
             get_data();
         }
     }
}
*/


int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;   // stop WDT
    beginADC();
    //clear_screen();
    print_labels();
    //timer_capture();
    while(1){

        //if(check_timerflag()){
       //     get_frequency();
       // }

       print_data();
    }
}
