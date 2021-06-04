/*
 *
 * DMM.c
 *
 */

#include "msp.h"
#include "ADC.h"
#include "DMM.h"
#include "math.h"


const int a = 97;   const int A = 65;
const int b = 98;   const int B = 66;
const int c = 99;   const int C = 67;
const int d = 100;  const int D = 68;
const int e = 101;  const int E = 69;
const int f = 102;  const int F = 70;
const int g = 103;  const int G = 71;
const int h = 104;  const int H = 72;
const int i = 105;  const int I = 73;
const int j = 106;  const int J = 74;
const int k = 107;  const int K = 75;
const int l = 108;  const int L = 76;
const int m = 109;  const int M = 77;
const int n = 110;  const int N = 78;
const int o = 111;  const int O = 79;
const int p = 112;  const int P = 80;
const int q = 113;  const int Q = 81;
const int r = 114;  const int R = 82;
const int s = 115;  const int S = 83;
const int t = 116;  const int T = 84;
const int u = 117;  const int U = 85;
const int v = 118;  const int V = 86;
const int w = 119;  const int W = 87;
const int x = 120;  const int X = 88;
const int y = 121;  const int Y = 89;
const int z = 122;  const int Z = 90;
int num; const int sample_num = 50000;
static unsigned int index, count;
static int vals[9];
int max_val;        //create global variable to hold the max value seen
int min_val;        //create a global variable to hold the min value seen
int prev_ACval, prev_DCval;
int peak_peak, voltage;

// cal ascii letter with "a"

void toTerminal(int key){
    for (num = 1000; num > 0; num--);        // Delay
    EUSCI_A0->TXBUF = key;
}

//Escape sequence for clearing screen when needed
void clear_screen(void){
    toTerminal(ESC);
    toTerminal(c);
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
        toTerminal(A); //move up
    else if (direction == 'D')
        toTerminal(B); //move down
    else if (direction == 'R')
        toTerminal(C); //move right
    else if (direction == 'L')
        toTerminal(D); //move left
    for (num = 2000; num > 0; num--);        // Delay
}



int get_peak(void){

    int meas_from_ADC = get_ADCdata(); //will take in the digital value of the voltage to compare with and update the max and min values
    int DC_meas = get_DC();
  //  if (DC_meas != 0)
     //   return 0;

    max_val = 0; // = 0;
    min_val = 3000000;   //1500000
    // by setting this value, we will guarantee that the first iteration of the for loop will set the values to the first voltage read
    //use a for loop to give it a delay and make sure that we are getting the maximum value
    if (prev_ACval == 0){
        prev_ACval = meas_from_ADC;
    }
    vrms=0;
    for(count = 0; count < sample_num; count++){

        meas_from_ADC = get_ADCdata(); //will take in the digital value of the voltage to compare with and update the max and min values
        if((meas_from_ADC < min_val)){// & (meas_from_ADC < prev_ACval)){
            min_val = meas_from_ADC;
            }
        if ((meas_from_ADC > max_val)){ //& (meas_from_ADC > prev_ACval)){
            max_val = meas_from_ADC;
            }
            //as the measurement variable updates, it will continually update the min val to ensure accuracy
          peak_peak = abs(max_val - min_val);
          prev_ACval = meas_from_ADC;
          vrms += (prev_ACval*prev_ACval);
    }
     //prev_ACval = meas_from_ADC;
     return peak_peak;      //returns digital peak to peak value
}

int get_RMS(void){
    //int RMS_val;
    /*
    int pk_pk = get_peak();  //will create a variable to hold the peak to peak value generated by the get_peak function
    int RMS_val = min_val + pk_pk/(sqrt(2));      //will divide peak to peak by square root of 2 to get rms value
    if(RMS_val > max_val)
        return pk_pk/(sqrt(2));
    else
        return RMS_val;                     //function will return the digital RMS value
        */
        //int RMS_val;

  //  int vrms=0;

   // for( count = 0; count < sample_num; count++){
        //vrms= (prev_ACval*prev_ACval) + vrms;
  //  }

    int RMS_val = sqrt(vrms/sample_num);
    return RMS_val;
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
        toTerminal(vals[index]);     //echo data to terminal
        index++;
    }
}


void AC_to_UART(void){
    int p2p;        //creates a variable to hold the AC peak to peak value

    p2p = get_peak();      //takes the peak to peak value from get_peak function and converts it into a Digital value
    //or = pk_pk;


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
     //   for (count = 20000; count > 0; count--);        // Delay
        toTerminal(vals[index]);        //echo data to terminal
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
    vals[0] = val1+48; vals[1] = 46;          vals[2] = val2+48; vals[3] = val3+48;
    vals[4] = val4+48; vals[5] = val5+48; vals[6] = val6+48; vals[7] = val7+48;
    vals[8] = 13; //         vals[9] = 86;          vals[10] = 13;

    index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<9){
      for (count = 10000; count > 0; count--);        // Delay
        toTerminal(vals[index]);
        //increment n to transfer the next character from vals[]
        index++;
    }
}


void freq_to_UART(void){

    int frequency = get_frequency();
    int val1 = (frequency%10000) /1000;
    int val2 = (frequency%1000) /100;
    int val3 = (frequency%100) /10;
    int val4 = frequency%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    // ASCII values: ' ' = 32, '.' = 46, enter = 13
    vals[0] = val1+48;  vals[1] = val2+48; vals[2] = val3+48;
    vals[3] = val4+48; vals[4] = 13;

    int index = 0;    //Set n to zero to begin calling index of vals[]

    while (index<5){
        toTerminal(vals[index]);
        //increment n to transfer the next character from vals[]
        index++;
    }
}

void print_labels(void){
   // int data_input = EUSCI_A0->TXBUF;
    //output the DC Voltage
    int DC_index;
    int DC_voltage[12] = {D, C, SPACE, V, o, l, t, a, g, e, COLON, SPACE};
   //Clear current data before updating with new data
    clear_screen();
    for(DC_index = 0; DC_index < 12; DC_index++){
        toTerminal(DC_voltage[DC_index]);  //output "DC Voltage: "
    }

    //Move cursor right 8 times
    move_cursor('R', ASCII9);

    toTerminal(V);

    toTerminal(ENTER);

    //Move cursor down twice
    move_cursor('D', ASCII2);

   //Display the label for peak of peak voltage of AC
      int AC_index;
      int AC_voltage[12] = {A, C, SPACE, V, o, l, t, a, g, e, COLON, SPACE};
      for(AC_index = 0; AC_index < 12; AC_index++){
          toTerminal(AC_voltage[AC_index]);  //output "AC Voltage: "
      }
      //Move cursor right 8 times
      move_cursor('R', ASCII9);

      //output "Vpp" to terminal
      toTerminal(V);
      toTerminal(p);
      toTerminal(p);

      toTerminal(ENTER);

      //Move cursor down once
      move_cursor('D', ASCII1);

    //Display the RMS AC Voltage label "True RMS AC Voltage: "
      int trueRMS[10] = {T, r, u, e, SPACE, R, M, S, COLON, SPACE};
      int RMS_index;
      for(RMS_index = 0; RMS_index < 10; RMS_index++){
          toTerminal(trueRMS[RMS_index]);  //output "True RMS: "
      }

      //Move cursor right 10 times  (9+1)
      move_cursor('R', ASCII9);
      move_cursor('R', ASCII2);


      //output "Vrms" to terminal
      toTerminal(V);
      toTerminal(r);
      toTerminal(m);
      toTerminal(s);

      toTerminal(ENTER);

      //Move cursor down twice
      move_cursor('D', ASCII2);

    //Display Frequency label
      int freq_display[11] = {F, r, e, q, u, e, n, c, y, COLON, SPACE};
      int freq_index;
      for(freq_index = 0; freq_index < 11; freq_index++){
          toTerminal(freq_display[freq_index]);  //output "Frequency: "
      }

      //Move cursor right 10 times
      move_cursor('R', ASCII9);
      move_cursor('R', ASCII1);


      //output "Hz" to terminal
      toTerminal(H);
      toTerminal(z);

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
    toTerminal(H);
}

void print_data(void){
    //get_ADCdata();
    move2measure(ASCII1);
   // get_DC();
    DC_to_UART();
    move2measure(ASCII3);
   // get_peak();
    AC_to_UART();
    move2measure(ASCII4);
    //get_RMS();
    RMS_to_UART();
    move2measure(ASCII6);
    //get_frequency();
    freq_to_UART();
}




