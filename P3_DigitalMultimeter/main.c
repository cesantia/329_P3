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
        UART_init();
        print_labels();
        move2measure(1);
        //move_right(8);
        //print_data();
        //move2measure(3);
        //move_right(8);
        //print_data();
        while(1);
}
