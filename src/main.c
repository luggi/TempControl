 /* Includes ------------------------------------------------------------------*/
#include "board.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
extern config_t cfg;
control_t control;
float input_voltage[ADC_CHANNEL_COUNT];
float previous_setpoint1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void adc_get_measurements(void);

int fputc(int c, FILE * f)
{
    UB_USB_CDC_SendChar(c);
    return c;
}

/*
set manualMode = 1000
set manualMode = 0
set voltageoutput =  0.000
set currentoutput =  0.000
set maxAmps =  3.100
set ampscale =  613.200
set setpoint1 =  0.000
set setpoint2 =  33.000
set pid1Pterm =  0.800
set pid1Iterm =  0.010
set pid1Dterm =  0.500
*/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already cfgred, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
    uint32_t interval1 = 100;
    uint32_t interval2 = 100;
    uint32_t time_now;
    static uint8_t failure_counter[TEMPERATURE_SENSOR_COUNT];
    float previous_temperature[TEMPERATURE_SENSOR_COUNT];
    float temp;
    char buffer[8];

    systemInit();
    init_GPIO();

    UB_DAC_Init(DUAL_DAC);
    
    UB_ADC2_DMA_Init();

    control.setpoint[PID1] = 0;
    control.setpoint[PID2] = 0;
    
    cfg.eeprom = read_config();
    
    while (1) {
        if (UB_USB_CDC_GetStatus() == USB_CDC_CONNECTED)
            cliProcess();

        if (cfg.manualMode) {
            outputCurrentToDac(cfg.output.internalAmpereOutput);
            outputVoltageToDac(cfg.output.voltageOutput);
        }

        time_now = millis();
        if (interval1 < time_now) {
            interval1 = time_now + cfg.cycletime;
            for(int i = 0; i < TEMPERATURE_SENSOR_COUNT; i++) {
                
                previous_temperature[i] = control.temperature[i];
                control.temperature[i] = read_celsius(i);
                if (failure_counter[i] > 2)
                    previous_temperature[i] = control.temperature[i];    
                if (control.temperature[i] < (previous_temperature[i] - MAX_TEMP_VARIATION) || control.temperature[i] > (previous_temperature[i] + MAX_TEMP_VARIATION)) { // filter spikes
                    control.temperature[i] = previous_temperature[i];
                    failure_counter[i]++;
                } else {
                   failure_counter[i] = 0;
               }
            }
            
            if (!cfg.manualMode) {
                pid_calc(&cfg.pid1, control.setpoint[PID1], control.temperature[SENSOR1], ((float)cfg.cycletime / 1000.0f));
                pid_calc(&cfg.pid2, control.setpoint[PID2], control.temperature[SENSOR2], ((float)cfg.cycletime / 1000.0f));
            }
            
            if (cfg.debug == 1) {
                printf("%i;%s\r\n",(millis()/1000),ftoa(control.temperature[SENSOR1], buffer));
            }
            else if (cfg.debug == 2)
            {
                  printf("T1:%s;",ftoa(control.temperature[SENSOR1], buffer));
                  printf("T2:%s;",ftoa(control.temperature[SENSOR2], buffer));
                  printf("V:%s;", ftoa(input_voltage[VOLTAGE_MOSFET], buffer));
                  printf("A:%s;",ftoa(input_voltage[CURRENT_MOSFET], buffer));
                  printf("VIN:%s\r\n", ftoa(input_voltage[VOLTAGE_INPUT], buffer));
            }
                
        }
        if (interval2 < time_now) { // read ADC every 10ms
            interval2 = time_now + 10;
            adc_get_measurements();
        }
    }
}

void adc_get_measurements(void) {
    for(int i=0; i < ADC_CHANNEL_COUNT; i++)
    {
        input_voltage[i] = ((float)UB_ADC2_DMA_Read(i) / 4095.0f) * cfg.input.scale[i];
    }
}
