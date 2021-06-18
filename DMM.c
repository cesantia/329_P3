/*
 *
 * DMM.c
 *
 */

#include "msp.h"
#include "UART_P3.h"
#include "ADC.h"
#include "math.h"
#include "timerA.h"


int max_val;        //create global variable to hold the max value seen
int min_val;        //create a global variable to hold the min value seen
int prev_ACval, prev_DCval;
int peak_peak, scaled_v, sample_cap1, sample_cap2;
unsigned long int volt_sum;
const int sample_num = 500;


//Setting up the ADC and terminal before getting measurements
void start(void){
    UART_init();
    ADC_init();
    print_labels();
    timer_capture();
}

int get_DC(void){
    int DC_val = get_ADCdata();

    if(prev_DCval == 0){
        prev_DCval = DC_val;
    }

    if((abs(DC_val - prev_DCval) > 100000)){    // | (peak_peak > 300000)){
        DC_val = 0;
    }
    prev_DCval = DC_val;
    return DC_val;          //return digital DC value
}

int get_peak(void){

    int meas_from_ADC = get_ADCdata(); //will take in the digital value of the voltage to compare with and update the max and min values
    int DC_meas = get_DC();
    static unsigned int count;

    max_val = 0;
    min_val = 3000000;
    // by setting this value, we will guarantee that the first iteration of the for loop will set the values to the first voltage read
    //use a for loop to give it a delay and make sure that we are getting the maximum value
    if (prev_ACval == 0){
        prev_ACval = meas_from_ADC/1000;
    }
    volt_sum=0;
    for(count = 0; count < sample_num; count++){

        meas_from_ADC = get_ADCdata(); //will take in the digital value of the voltage to compare with and update the max and min values
        if((meas_from_ADC < min_val)){// & (meas_from_ADC < prev_ACval)){
            min_val = meas_from_ADC;
            sample_cap1 = count;
            }
        if ((meas_from_ADC > max_val)){ //& (meas_from_ADC > prev_ACval)){
            max_val = meas_from_ADC;
            sample_cap2 = count;
            }
            //as the measurement variable updates, it will continually update the min val to ensure accuracy
          peak_peak = abs(max_val - min_val);
          prev_ACval = meas_from_ADC;
          scaled_v = meas_from_ADC/1000;
          volt_sum += (scaled_v*scaled_v);
    }
     return peak_peak;      //returns digital peak to peak value
}

//detects the sample difference between the max and min voltage values to return a frequency
int frequency_peaks(void){
    return abs(sample_cap2 - sample_cap1);
}

int get_RMS(void){
    int RMS_val = sqrt(volt_sum/sample_num);
    return RMS_val;
}
