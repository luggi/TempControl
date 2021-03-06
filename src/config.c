#include "config.h"

#define FLASH_WRITE_ADDR  ((uint32_t)0x0800C000)     // Sektor_2


config_t cfg;

int8_t save_config(void)
{
    FLASH_Status flash_status;
    flash_status = FLASH_COMPLETE;
    
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    flash_status = FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3);

    if (flash_status != FLASH_COMPLETE) {
        FLASH_Lock();
        return 0;
    }
    
    for (int i = 0; i < sizeof(config_t); i += 4) {    
        flash_status = FLASH_ProgramWord(FLASH_WRITE_ADDR + i, *(uint32_t *)((char *)&cfg + i));
        
        if(flash_status != FLASH_COMPLETE) {
            FLASH_Lock();
            return 0;
        }
    }
    FLASH_Lock();
    return 1;
}

int8_t read_config(void)
{
    if(VALID_EEPROM()) {
        memcpy(&cfg, (char *)FLASH_WRITE_ADDR, sizeof(config_t));
        return 1;
    }
    else
        resetConf();
    
    return 0; 
}

// Default settings
void resetConf(void)
{
    memset(&cfg, 0, sizeof(config_t)); // clear all
    
    cfg.magic_be = 0xBE;
    cfg.magic_ef = 0xEF;
    
    cfg.output.ampereOffset = -0.073f;  // todo make init set defaults function with eeprom recall
    cfg.output.scaleDAC1ToA = 460.0f;
    cfg.output.maxAmps = 5.0f;
    cfg.output.scaleDAC2ToV = 394.0f;
    cfg.output.maxVoltage = 10.0f;
    cfg.output.voltageOffset = 0.054;
    cfg.output.scaleVoltageToAmpere = 1.0f / 12.0f;
    cfg.output.maxPowerloss = 12.0f;
    
    cfg.input.scale[VOLTAGE_MOSFET] = 36.3f;
    cfg.input.scale[CURRENT_MOSFET] = 9.708f;
    cfg.input.scale[VOLTAGE_INPUT] = 18.81f;
    cfg.input.scale[VOLTAGE_VMON] = 10.56f * 5.0f; // correctly scaled for EA-PSI 8080-120
    cfg.input.scale[CURRENT_CMON] = 10.56f * 12.0f;
    
    pid_init(&cfg.pid1, 0.40f, 0.0002f, 15.0f, 0.0f, 10.0f, outputCurrentfromPID1);
    pid_init(&cfg.pid2, 0.12f, 0.00002f, 15.0f, 0.0f, 10.0f, outputCurrentfromPID2);
    
    cfg.manualMode = 1;
    cfg.output.internalAmpereOutput = 0.0f;
    cfg.output.voltageOutput = 0.0f;
    cfg.debug = 0;
    cfg.cycletime = 1000;
    cfg.pid1outputChannel = 1;
    cfg.pid2outputChannel = 2;
}

static uint8_t VALID_EEPROM(void)
{
    const config_t *temp = (const config_t *)FLASH_WRITE_ADDR;
    // check size and magic numbers
    if (temp->magic_be != 0xBE || temp->magic_ef != 0xEF)
        return 0;
    else
        return 1;
}
