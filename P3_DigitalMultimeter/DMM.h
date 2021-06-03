/*
 *
 * DMM.h
 *
 */

#ifndef DMM_H_
#define DMM_H_
    void time_capture(void);
    void toTerminal(int key);
    void clear_screen(void);
    void cursor_down(void);
    void get_frequency(void);
    void print_labels(void); //print labels for measurements
    void move2measure(int row); //set Escape code to move cursor to each measurement
    void print_data(void);  //print_data(DC Voltage, AC Voltage, True RMS, Frequency);

    /*
     * when functions are complete
     * //functions
        void start(void);
        void get_data(void);
        uint32_t check_flag(void);
     */
    #define  ESC    27
    #define SPACE   32
    #define ENTER   13
    #define openBracket 91  // '['
    #define closeBracket 93  // ']'
    #define  COLON     58
    #define SEMICOLON  59

#endif /* DMM_H_ */
