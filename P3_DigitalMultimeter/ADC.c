/*
*
* ADC.c
*
*/
#include "msp.h"
#include "ADC.h"

static uint32_t DigiRead = 0;
static uint32_t measurement ;
static uint32_t ADCflag = 0;
int timer_captureflag = 0;
volatile uint16_t captureValue[2] = {0};    //volatile means go to memory and can change at any time
volatile uint16_t captureFlag = 0;
volatile uint16_t capturePeriod;

void ADC_init(void) {

    // GPIO Setup
    P1->OUT &= ~BIT0;                 // Clear LED to start
    P1->DIR |= BIT0;                  // Set P1.0/LED to output
    P5->SEL1 |= BIT4;                 // Configure P5.4 for ADC
    P5->SEL0 |= BIT4;

    // Sampling time = 4 clock cycles, ADC14 on    //4, 16, 96, 192...0, 2, 5, 7
    ADC14->CTL0 = ADC14_CTL0_SHT0_5 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;     // Use sampling timer, 14-bit conversion

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1; // A1 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;        // Enable ADC conv complete interrupt
    ADC14->CTL0 |= ADC14_CTL0_ENC;        // Enable conversions

    // Enable ADC interrupt in NVIC module
   // NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    // Enable global interrupt
    //__enable_irq();

    // Start sampling/conversion
    ADC14->CTL0 |= ADC14_CTL0_SC;

}

void UART_init(void){
    //configure UART pins
    P1-> SEL0 |= BIT2 | BIT3;

    //configure UART
    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;
    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST | EUSCI_B_CTLW0_SSEL__SMCLK;

    EUSCI_A0->BRW=1;  //Using baud rate calculator
    EUSCI_A0->MCTLW = (10 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; //initialize eUSCI

    // Enable global interrupt
    __enable_irq();
}

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

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    P1->OUT |= BIT0;                // P1.0 = 1
    //reset measurement before getting new calibrated values
    measurement = 0;

    //read the digital output from the waveform generator
    DigiRead = ADC14->MEM[0];
    //Setting ADCflag to 1 to confirm it went through the interrupt
    ADCflag = 1;
    P1->OUT &= ~BIT0;               // P1.0 = 0

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


uint32_t check_ADCflag(void) {
 return ADCflag;
}

uint32_t check_timerflag(void) {
    return timer_captureflag;
}

void beginADC(void) {
    ADC_init();
    UART_init();
}


int get_ADCdata(void) {
    // __disable_irq();  // disable interrupts

    // Start sampling/conversion
    ADC14->CTL0 |= ADC14_CTL0_SC;

     //read the digital output from the waveform generator
     DigiRead = ADC14->MEM[0];

     ADCflag = 0;    // clear capture flag

    //Calibrate value for measured voltage
    measurement = (202*DigiRead) - 1240;

   // __enable_irq();   // enable interrupt again

    return measurement;
}


int get_frequency(){

        __disable_irq();  // protect capture times

        capturePeriod = captureValue[1] - captureValue[0];  // time difference

        int frequency = 1/capturePeriod;

        timer_captureflag = 0;    // clear capture flag

        __enable_irq();   // start capturing again

        return frequency;
}

