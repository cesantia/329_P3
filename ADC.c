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


int get_ADCdata(void) {
        __disable_irq();  // disable interrupts

        // Start sampling/conversion
        ADC14->CTL0 |= ADC14_CTL0_SC;

         //read the digital output from the waveform generator
         DigiRead = ADC14->MEM[0];

         ADCflag = 0;    // clear capture flag

        //Calibrate value for measured voltage
        measurement = (202*DigiRead) - 1240;

        __enable_irq();   // enable interrupt again

    return measurement;
}



uint32_t check_ADCflag(void) {
 return ADCflag;
}
