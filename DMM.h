/*
 *
 * DMM.h
 *
 */

#ifndef DMM_H_
#define DMM_H_

    ////get functions
    /*int get_peak(void);
    int get_RMS(void);
    int get_DC(void);
    void DC_to_UART(void);
    void AC_to_UART(void);
    void RMS_to_UART(void);
    void get_vals(void);
    void timer_capture(void);
    void toTerminal(int key);
    void clear_screen(void);
    void cursor_down(void);
    int get_frequency(void);
    void print_labels(void); //print labels for measurements
    void move2measure(int row); //set Escape code to move cursor to each measurement
    void print_data(void);  //print_data(DC Voltage, AC Voltage, True RMS, Frequency);
    uint32_t check_timerflag(void);*/

     // when functions are complete
     //functions
        void start(void);
        void print_data(void);

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

#endif /* DMM_H_ */
