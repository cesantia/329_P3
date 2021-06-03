/*
*
* ADC.c
*
*/
#include "msp.h"
#include "ADC.h"
#include "math.h"

static uint32_t DigiRead = 0;
static uint32_t measurement ;
static unsigned int index, count;
static int vals[11];
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
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    // Enable global interrupt
    __enable_irq();

    for (count = 20000; count > 0; count--);    // Delay
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
}
/*
void ADC_to_UART(void){
    __disable_irq();  // disable interrupts

    ADCflag = 0;    // clear capture flag

    //Calibrate value for measured voltage
    measurement = (202*DigiRead) - 1240;

    //divide measurement digits by factors of 10 to get digits

    int val1 = measurement/1000000;
    int val2 = (measurement%1000000) /100000;
    int val3 = (measurement%100000) /10000;
    int val4 = (measurement%10000) /1000;
    int val5 = (measurement%1000) /100;
    int val6 = (measurement%100) /10;
    int val7 = measurement%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    // ASCII values: ' ' = 32, '.' = 46, 'V' = 86, enter = 13
    vals[0] = val1+48; vals[1] = 46;          vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48; vals[5] = val5+48; vals[6] = val6+48; vals[7] = val7+48;
    vals[8] = 32;          vals[9] = 86;          vals[10] = 13;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<11){
        // EUSCI_A0->RXBUF = vals[index];
        for (count = 20000; count > 0; count--);        // Delay

        //echo data to terminal
        EUSCI_A0->TXBUF = vals[index];
        //increment n to transfer the next character from vals[]
        index++;
    }
    for (count = 1000; count > 0; count--);        // Delay
    ADCflag = 0;    // clear capture flag
    __enable_irq();   // enable interrupt again
}*/

void DC_to_UART(void){
    int DC_val;        //creates a variable to hold the AC peak to peak value
    DC_val = get_DC();

    //divide measurement digits by factors of 10 to get digits
    int val1 = DC_val/1000000;
    int val2 = (DC_val%1000000) /100000;
    int val3 = (DC_val%100000) /10000;
    int val4 = (DC_val%10000) /1000;
    int val5 = (DC_val%1000) /100;
    int val6 = (DC_val%100) /10;
    int val7 = DC_val%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    // ASCII values: ' ' = 32, '.' = 46, 'V' = 86, enter = 13
    vals[0] = val1+48; vals[1] = 46;          vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48; vals[5] = val5+48; vals[6] = val6+48; vals[7] = val7+48;
    vals[8] = 13;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<9){
        // EUSCI_A0->RXBUF = vals[index];
        for (count = 20000; count > 0; count--);        // Delay

        //echo data to terminal
        EUSCI_A0->TXBUF = vals[index];
        //increment n to transfer the next character from vals[]
        index++;
    }
}



// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    //reset measurement before getting new calibrated values
    measurement = 0; index = 0;

    //read the digital output from the waveform generator
    DigiRead = ADC14->MEM[0];
    //Setting ADCflag to 1 to confirm it went through the interrupt
    ADCflag = 1;
}

uint32_t check_ADCflag(void) {
 return ADCflag;
}

void get_ADCdata(void) {
    //while(!check_ADCflag());
    //ADC_to_UART();
     //__disable_irq();  // disable interrupts

    ADCflag = 0;    // clear capture flag

    //Calibrate value for measured voltage
    measurement = (202*DigiRead) - 1240;
    for (count = 1000; count > 0; count--);        // Delay

    //__enable_irq();   // enable interrupt again

}

void beginADC(void) {
    ADC_init();
    UART_init();
}



//for loop way to make sure that we have the highest value

int max_val;        //create global variable to hold the max value seen
int min_val;        //create a global variable to hold the min value seen
int peak_peak;


int get_peak(void){

    int meas_from_ADC = measurement; //will take in the digital value of the voltage to compare with and update the max and min values

    max_val =0;
    min_val =3;
    //max_val = 0
    //min_val = 3
    // by setting this value, we will guarantee that the first iteration of the for loop will set the values to the first voltage read
    //use a for loop to give it a delay and make sure that we are getting the maximum value

    for( count = 0; count < 200; count++){
        if (meas_from_ADC> max_val){
            max_val = meas_from_ADC;
            }
            //as the measurement variable updates, it will continually update the max val to ensure accuracy
        if(meas_from_ADC < min_val){
            min_val = meas_from_ADC;
            }
            //as the measurement variable updates, it will continually update the min val to ensure accuracy
          peak_peak = max_val - min_val;
          //peak_peak will update the peak to peak value which will be returned by this function
       }
         return peak_peak;      //returns digital peak to peak value
}

int get_RMS(void){
    //int RMS_val;
    int pk_pk = get_peak();  //will create a variable to hold the peak to peak value generated by the get_peak function
    int RMS_val = pk_pk/(sqrt(2));      //will divide peak to peak by square root of 2 to get rms value
    return RMS_val;                     //function will return the digital RMS value
}

int get_DC(void){
    int pk_pk = get_peak();
    int DC_val = max_val - (pk_pk/2);
    return DC_val;          //return digital DC value
}

void AC_to_UART(void){
    int p2p;        //creates a variable to hold the AC peak to peak value

    p2p = get_peak();      //takes the peak to peak value from get_peak function and converts it into a Digital value
    //or = peak_peak;


    //divide measurement digits by factors of 10 to get digits

    int val1 = p2p/1000000;
    int val2 = (p2p%1000000) /100000;
    int val3 = (p2p%100000) /10000;
    int val4 = (p2p%10000) /1000;
    int val5 = (p2p%1000) /100;
    int val6 = (p2p%100) /10;
    int val7 = p2p%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    // ASCII values: ' ' = 32, '.' = 46, 'V' = 86, enter = 13
    vals[0] = val1+48; vals[1] = 46;          vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48; vals[5] = val5+48; vals[6] = val6+48; vals[7] = val7+48;
    vals[8] = 13;   //       vals[9] = 86;          vals[10] = 13;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<9){
        // EUSCI_A0->RXBUF = vals[index];
        for (count = 20000; count > 0; count--);        // Delay

        //echo data to terminal
        EUSCI_A0->TXBUF = vals[index];
        //increment n to transfer the next character from vals[]
        index++;
    }
}


void RMS_to_UART(void){
    int dig_rms= get_RMS();            //creates a varibale to hold the digital value of the RMS

    //divide measurement digits by factors of 10 to get digits

    int val1 = dig_rms/1000000;
    int val2 = (dig_rms%1000000) /100000;
    int val3 = (dig_rms%100000) /10000;
    int val4 = (dig_rms%10000) /1000;
    int val5 = (dig_rms%1000) /100;
    int val6 = (dig_rms%100) /10;
    int val7 = dig_rms%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    // ASCII values: ' ' = 32, '.' = 46, 'V' = 86, enter = 13
    vals[0] = val1+48; vals[1] = 46;          vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48; vals[5] = val5+48; vals[6] = val6+48; vals[7] = val7+48;
    vals[8] = 13; //         vals[9] = 86;          vals[10] = 13;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<9){
        // EUSCI_A0->RXBUF = vals[count];
        for (count = 20000; count > 0; count--);        // Delay

        //echo data to terminal
        EUSCI_A0->TXBUF = vals[index];
        //increment n to transfer the next character from vals[]
        index++;
    }
}
/*
void get_vals(void){  //could be an int?
    get_peak();
    get_RMS();
    get_DC();
}*/


