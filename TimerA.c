/*
 *
 * TimerA.c
 *
 */

#include "msp.h"
#include "TimerA.h"

int timer_captureflag = 0;
volatile uint16_t captureValue[2] = {0};    //volatile means go to memory and can change at any time
volatile uint16_t captureFlag = 0;
volatile uint16_t capturePeriod;
int prev_freq;

void timer_capture(void){
    P2->OUT &= ~BIT0;                 // Clear LED to start
    P2->DIR |= BIT0;                  // Set P1.0/LED to output
    P2->SEL0 |= BIT5;     // TA0.CCI2A input capture pin, second function
    P2->DIR &= ~BIT5;

     // TimerA0_A2 Setup     //TRM p.799
     TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CM_1 | // Capture rising edge,
             TIMER_A_CCTLN_CCIS_0 |          // Use CCI2A (P2.5),
             TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
             TIMER_A_CCTLN_CAP |             // Enable capture mode,
             TIMER_A_CCTLN_SCS;              // Synchronous capture

     TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | // Use SMCLK as clock source,
             TIMER_A_CTL_MC__CONTINUOUS |    // Start timer in continuous mode
             TIMER_A_CTL_CLR;                // clear TA0R

     NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);

     // Enable global interrupt
     __enable_irq();
}


// TimerA0_N interrupt service routine
void TA0_N_IRQHandler(void){
     volatile static uint32_t captureCount = 0;   //static: variable will retain its call during function calls
     P1->OUT |= BIT0;                // P1.0 = 1

     if (TIMER_A0->CCTL[2] & TIMER_A_CCTLN_CCIFG)
     {

       captureValue[captureCount] = TIMER_A0->CCR[2];
       captureCount++;

       if (captureCount == 2)
       {
         captureCount = 0;
         timer_captureflag = 1;
       }

       // Clear the interrupt flag
       TIMER_A0->CCTL[2] &= ~(TIMER_A_CCTLN_CCIFG);
     }
     P1->OUT &= ~BIT0;               // P1.0 = 0
}


int get_frequency(void){
    int frequency, current_freq;
        __disable_irq();  // protect capture times

        capturePeriod = captureValue[1] - captureValue[0];  // time difference

        current_freq = capturePeriod % 30000 / 25;

        frequency = (current_freq + prev_freq)/2;

        timer_captureflag = 0;    // clear capture flag

        prev_freq = current_freq;  //saves the previous frequency to average it over time

        __enable_irq();   // start capturing again

        return frequency;
}


uint32_t check_timerflag(void) {
    return timer_captureflag;
}
