

 /*
 *
 * ADC.h
 *
 */

#ifndef ADC_H_
#define ADC_H_

    //functions
    void beginADC(void);

    uint32_t check_ADCflag(void);

    ////get functions
    int get_peak(void);
    int get_RMS(void);
    int get_DC(void);
    void DC_to_UART(void);
    void AC_to_UART(void);
    void RMS_to_UART(void);
    void get_vals(void);
    //void UART_init(void);
    void get_ADCdata(void);


#endif /* ADC_H_ */


