/* Includes ------------------------------------------------------------------*/
#include "board.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
extern dac_t dac;
config_t config;
float temperature[2];
float setpoint[2];
pid_t pid1;
pid_t pid2;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
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
    /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
    uint32_t interval = 100;
    uint32_t time_now;

    systemInit();
    init_GPIO();

    UB_DAC_Init(DUAL_DAC);

    dac.ampereOffset = -0.06f;  // todo make init set defaults function with eeprom recall
    dac.scale1ToA = 613.2f;
    dac.maxAmps = 3.1f;

    config.manualMode = 0;
    config.cycletime = 1000;

    pid_init(&pid1, 0.8f, 0.01f, 0.5f, 9.0f, outputDacCurrentfromPID);

    while (1) {
        if (UB_USB_CDC_GetStatus() == USB_CDC_CONNECTED)
            cliProcess();

        if (config.manualMode)
            outputCurrentToDac(dac.ampereOutput);

        time_now = millis();
        if (interval < time_now && !config.manualMode) {
            interval = time_now + config.cycletime;

            //temperature[SENSOR1] = read_celsius(SENSOR1);
            temperature[SENSOR2] = read_celsius(SENSOR2);

            pid_calc(&pid1, setpoint[1], temperature[SENSOR2], (1000.0f / (float) config.cycletime));

        }
    }
}
