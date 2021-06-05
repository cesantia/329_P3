

 /*
 *
 * ADC.h
 *
 */

#ifndef ADC_H_
#define ADC_H_

    //functions
    void beginADC(void);
    void timer_capture(void);

    uint32_t check_ADCflag(void);
    uint32_t check_timerflag(void);
    int get_frequency(void);

    //void UART_init(void);
    int get_ADCdata(void);


#endif /* ADC_H_ */


