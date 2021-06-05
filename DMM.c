/*
 *
 * DMM.c
 *
 */

#include "msp.h"
#include "ADC.h"
#include "DMM.h"
#include "math.h"


int num; const int sample_num = 500;
static unsigned int index, count;
static int vals[9];
int max_val;        //create global variable to hold the max value seen
int min_val;        //create a global variable to hold the min value seen
int prev_ACval, prev_DCval;
int peak_peak, scaled_v, sample_cap1, sample_cap2;
unsigned long int volt_sum;


void toTerminal(int key){
    for (num = 1000; num > 0; num--);        // Delay
    EUSCI_A0->TXBUF = key;
}

//Escape sequence for clearing screen when needed
void clear_screen(void){
    toTerminal(ESC);
    toTerminal('c');
    for (num = 2000; num > 0; num--);        // Delay
}

/*Escape sequence for moving cursor
 * up     <ESC>[{COUNT}A
 * down   <ESC>[{COUNT}B
 * right  <ESC>[{COUNT}C
 * left   <ESC>[{COUNT}D
 * where COUNT == move_val
 */
void move_cursor(char direction, int move_val){
    toTerminal(ESC);
    toTerminal(openBracket);
    toTerminal(move_val);
    if (direction == 'U')
        toTerminal('A'); //move up
    else if (direction == 'D')
        toTerminal('B'); //move down
    else if (direction == 'R')
        toTerminal('C'); //move right
    else if (direction == 'L')
        toTerminal('D'); //move left
    for (num = 2000; num > 0; num--);        // Delay
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

int get_RMS(void){
    int RMS_val = sqrt(volt_sum/sample_num);
    return RMS_val;
}

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
    vals[0] = val1+48; vals[1] = '.';          vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48; vals[5] = val5+48; vals[6] = val6+48; vals[7] = val7+48;
    vals[8] = ENTER;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<9){
        toTerminal(vals[index]);     //echo data to terminal
        index++;
    }
}


void AC_to_UART(void){
    int p2p;        //creates a variable to hold the AC peak to peak value

    p2p = get_peak();      //takes the peak to peak value from get_peak function and converts it into a Digital value

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
    vals[0] = val1+48; vals[1] = '.';          vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48; vals[5] = val5+48; vals[6] = val6+48; vals[7] = val7+48;
    vals[8] = ENTER;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<8){
        toTerminal(vals[index]);        //echo data to terminal
        index++;
    }
}


void RMS_to_UART(void){
    int dig_rms= get_RMS();            //creates a varibale to hold the digital value of the RMS

    //divide measurement digits by factors of 10 to get digits
    int val1 = (dig_rms%10000) /1000;
    int val2 = (dig_rms%1000) /100;
    int val3 = (dig_rms%100) /10;
    int val4 = dig_rms%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    vals[0] = val1+48; vals[1] = '.';  vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48;
    vals[5] = ENTER;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<6){
      //for (count = 10000; count > 0; count--);        // Delay
        toTerminal(vals[index]);
        //increment n to transfer the next character from vals[]
        index++;
    }
}


void freq_to_UART(void){

    //int frequency = get_frequency();

    int frequency = abs(sample_cap2 - sample_cap1)*2;

    int val1 = (frequency%10000) /1000;
    int val2 = (frequency%1000) /100;
    int val3 = (frequency%100) /10;
    int val4 = frequency%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    vals[0] = val1+48;  vals[1] = val2+48; vals[2] = val3+48;
    vals[3] = val4+48; vals[4] = ENTER;

    int index = 0;    //Set n to zero to begin calling index of vals[]

    while (index<5){
        toTerminal(vals[index]);
        //increment n to transfer the next character from vals[]
        index++;
    }
}

void print_labels(void){

    //output the DC Voltage
    int DC_index;
    int DC_voltage[12] = {'D', 'C', SPACE, 'V', 'o', 'l', 't', 'a', 'g', 'e', COLON, SPACE};
   //Clear current data before updating with new data
    clear_screen();
    for(DC_index = 0; DC_index < 12; DC_index++){
        toTerminal(DC_voltage[DC_index]);  //output "DC Voltage: "
    }

    //Move cursor right 8 times
    move_cursor('R', ASCII9);

    toTerminal('V');

    toTerminal(ENTER);

    //Move cursor down twice
    move_cursor('D', ASCII2);

   //Display the label for peak of peak voltage of AC
      int AC_index;
      int AC_voltage[12] = {'A', 'C', SPACE, 'V', 'o', 'l', 't', 'a', 'g', 'e', COLON, SPACE};
      for(AC_index = 0; AC_index < 12; AC_index++){
          toTerminal(AC_voltage[AC_index]);  //output "AC Voltage: "
      }
      //Move cursor right 8 times
      move_cursor('R', ASCII9);

      //output "Vpp" to terminal
      toTerminal('V');
      toTerminal('p');
      toTerminal('p');

      toTerminal(ENTER);

      //Move cursor down once
      move_cursor('D', ASCII1);

    //Display the RMS AC Voltage label "True RMS AC Voltage: "
      int trueRMS[10] = {'T', 'r', 'u', 'e', SPACE, 'R', 'M', 'S', COLON, SPACE};
      int RMS_index;
      for(RMS_index = 0; RMS_index < 10; RMS_index++){
          toTerminal(trueRMS[RMS_index]);  //output "True RMS: "
      }

      //Move cursor right 10 times  (9+1)
      move_cursor('R', ASCII9);
      move_cursor('R', ASCII2);


      //output "Vrms" to terminal
      toTerminal('V');
      toTerminal('r');
      toTerminal('m');
      toTerminal('s');

      toTerminal(ENTER);

      //Move cursor down twice
      move_cursor('D', ASCII2);

    //Display Frequency label
      int freq_display[11] = {'F','r', 'e', 'q', 'u', 'e', 'n', 'c', 'y', COLON, SPACE};
      int freq_index;
      for(freq_index = 0; freq_index < 11; freq_index++){
          toTerminal(freq_display[freq_index]);  //output "Frequency: "
      }

      //Move cursor right 10 times
      move_cursor('R', ASCII9);
      move_cursor('R', ASCII1);


      //output "Hz" to terminal
      toTerminal('H');
      toTerminal('z');

}


/*set Escape code to move cursor to each measurement
 * DC:           row 1
 * AC peak-peak: row 3
 * True RMS:     row 4
 * Frequency:    row 6
 */
void move2measure(int row){
    toTerminal(ESC);
    toTerminal(openBracket);
    toTerminal(row); //ASCII value for the row number
    toTerminal(SEMICOLON);
    toTerminal(ASCII1); //ASCII 1
    toTerminal(ASCII3); //ASCII 3
    toTerminal('H');
}

//Setting up the ADC and terminal before getting measurements
void start(void){
    beginADC();
    print_labels();
    timer_capture();
}

//Print updated data values to terminal
void print_data(void){
    //__disable_irq();  // disable interrupts
    move2measure(ASCII1);
    DC_to_UART();
    move2measure(ASCII3);
    AC_to_UART();
    move2measure(ASCII4);
    RMS_to_UART();
    move2measure(ASCII6);
    //if(check_timerflag())
       freq_to_UART();
    //__enable_irq();   // enable interrupt again
}




