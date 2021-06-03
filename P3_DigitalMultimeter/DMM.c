/*
 *
 * DMM.c
 *
 */

#include "msp.h"
#include "ADC.h"
#include "DMM.h"

#define  ESC    27
#define SPACE   32
#define ENTER   13
#define openBracket 91  // '['
#define closeBracket 93  // ']'
#define  COLON     58
#define SEMICOLON  59
#define ASCII0  48
#define ASCII1  49
#define ASCII2  50
#define ASCII3  51
#define ASCII4  52
#define ASCII5  53
#define ASCII6  54
#define ASCII7  55
#define ASCII8  56
#define ASCII9  57


volatile uint16_t captureValue[2] = {0};    //volatile means go to memory and can change at any time
volatile uint16_t captureFlag = 0;
volatile uint16_t done = 0;
volatile uint16_t capturePeriod;
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
int num; static int vals[5];// int frequency;
int timer_captureflag = 0;

void timer_capture(void){
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
/*
     while (1)
     {
         if (timer_captureflag)
         {
           __disable_irq();  // protect capture times

           capturePeriod = captureValue[1] - captureValue[0];  // time difference

           timer_captureflag = 0;    // clear capture flag

           __enable_irq();   // start capturing again

           // Do any time or freq calculations here
           //int frequency = 1/capturePeriod;
         }
     }*/
}

// TimerA0_N interrupt service routine
void TA0_N_IRQHandler(void){
     volatile static uint32_t captureCount = 0;   //static: variable will retain its call during function calls

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

}


void toTerminal(int key){
    for (num = 1000; num > 0; num--);        // Delay
    /*if((key < 10) & (key > -1)){ //helpful for declaring values 0-9
        key += 48; //values 0-9 will turn to their ASCII values
    }*/
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

void get_frequency(){
    int frequency = 1/capturePeriod;

    int val1 = (frequency%10000) /1000;
    int val2 = (frequency%1000) /100;
    int val3 = (frequency%100) /10;
    int val4 = frequency%10;

    //Save individual digits in array to then call to terminal
    //values will be returned as ASCII values (for 0-9, ASCII = number + 48)
    // ASCII values: ' ' = 32, '.' = 46, 'V' = 86, enter = 13
    vals[0] = val1+48;  vals[1] = val2+48; vals[2] = val3+48;
    vals[3] = val4+48; vals[4] = 13;

    int index = 0;    //Set n to zero to begin calling index of vals[]
    while (index<5){
        toTerminal(vals[index]);
        //increment n to transfer the next character from vals[]
        index++;
    }
    //return frequency;

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

  //Now output the AC Voltages (if available)

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

      //Move cursor right 9 times
      move_cursor('R', ASCII9);

      //output "Hz" to terminal
      toTerminal(H);
      toTerminal(z);

      //toTerminal(ENTER);

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
    get_ADCdata();
    move2measure(ASCII1);
    get_DC();
    DC_to_UART();
    move2measure(ASCII3);
    get_peak();
    AC_to_UART();
    move2measure(ASCII4);
    get_RMS();
    RMS_to_UART();

    move2measure(ASCII6);
    get_frequency();
}

uint32_t check_timerflag(void) {
 return timer_captureflag;
}

int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;   // stop WDT
    beginADC();
    //clear_screen();
    print_labels();
    //timer_capture();
    while(1){
        print_data();

        //if(check_timerflag()){
        //  __disable_irq();  // protect capture times

         // capturePeriod = captureValue[1] - captureValue[0];  // time difference

         // timer_captureflag = 0;    // clear capture flag
         // __enable_irq();   // start capturing again
          // Do any time or freq calculations here

        if(check_ADCflag()){
            get_ADCdata();
        }
    }
}



