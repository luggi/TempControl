//
// Serial protocol parser
//  
//
/*#include "board.h"
#include "crc16.h"

extern config_t cfg;
extern control_t control;

static uint8_t calculate_CRC16(char *data, int length);
static void compute_command(char * data, int length);
void serial_reply_success(void);
void serial_reply(char *data, int length, uint8_t command);

enum state {
    STARTFRAME,
    COMMAND,
    PAYLOAD_LENGTH,
    PAYLOAD,
    CRC16_FIRST_BYTE,
    CRC16_SECOND_BYTE,
    ENDFRAME
};

enum command_set_table {
    SET_PID = 0,
    SET_OUTPUT_SCALES,
    SET_OUTPUT_LIMITS,
    SET_INPUT_SCALES,
    SET_TEMPERATURE,
    SET_CURRENT,
    SET_VOLTAGE,
    SET_CYCLETIME,
    SAVE_DATA
};

enum command_read_table {
    READ_TEMPERATURES = 128,
    READ_VOLTAGES,
    READ_CURRENTS,
    READ_MODE,
    READ_PID_SETTINGS,
    READ_PID_OUTPUT
};

void parseSerialData(char byte)
{
    static int state = STARTFRAME;
    static int byteCounter = 0;
    static int payload_length = 0;
    static char buffer[200];
    
    switch(state) {
        case STARTFRAME:
            if (byte == '<')
                byteCounter = 0;
            else if (byte == '&' && byteCounter == 1) {
                byteCounter++;
                state = COMMAND;
            }
            else
                byteCounter = 0;
        break;
        case COMMAND:
            ++byteCounter;
            buffer[byteCounter - 2] = byte;
            state = PAYLOAD_LENGTH;
        break;
        case PAYLOAD_LENGTH:
            ++byteCounter;
            buffer[byteCounter - 2] = byte;
            payload_length = byte;
            if (payload_length > 180)
                state = STARTFRAME;
        break;
        case PAYLOAD:
            ++byteCounter;
            --payload_length;
            buffer[byteCounter - 2] = byte;
            if (payload_length == 0)
                state = CRC16_FIRST_BYTE;
        break;
        case CRC16_FIRST_BYTE:
            ++byteCounter;
            buffer[byteCounter] = byte;
            state = CRC16_SECOND_BYTE;
        break;
        case CRC16_SECOND_BYTE:
            ++byteCounter;
            buffer[byteCounter] = byte;
            if(calculate_CRC16(buffer, byteCounter))
                state = ENDFRAME;
            else
                state = STARTFRAME;
        break;
        case ENDFRAME:
            if (byte == '>')
                compute_command(buffer, byteCounter);
            state = STARTFRAME;
        break;
    }
}

#define COMMAND_POSITION 0
#define DATA_START 2

static float raw_to_float(char *buffer, uint8_t *counter)
{
    *counter += 4;
    return (float)(((int32_t)buffer[3] << 24) | ((int32_t)buffer[2] << 16) | ((int32_t)buffer[1] << 8) | (int32_t)buffer[0]);
}

static int32_t raw_to_int32_t(char *buffer, uint8_t *counter)
{
    *counter += 4;
    return (int32_t)(((int32_t)buffer[3] << 24) | ((int32_t)buffer[2] << 16) | ((int32_t)buffer[1] << 8) | (int32_t)buffer[0]);
}

static void compute_command(char *buffer, int byteCounter)
{
    char buf[100];
    uint8_t command;
    uint8_t counter = 0;
    command = buffer[COMMAND_POSITION];
    counter = DATA_START;
    
    switch(command)
    {
        case SET_PID:
            if (buffer[counter] == 1) {
                counter++;
                cfg.pid1.P = raw_to_float(&buffer[counter],&counter);
                cfg.pid1.I = raw_to_float(&buffer[counter],&counter);
                cfg.pid1.D = raw_to_float(&buffer[counter],&counter);
                cfg.pid1.windup = raw_to_float(&buffer[counter],&counter);
                cfg.pid1.Tf = raw_to_float(&buffer[counter],&counter);
            } else if (buffer[counter] == 2) {
                counter++;
                cfg.pid2.P = raw_to_float(&buffer[counter],&counter);
                cfg.pid2.I = raw_to_float(&buffer[counter],&counter);
                cfg.pid2.D = raw_to_float(&buffer[counter],&counter);
                cfg.pid2.windup = raw_to_float(&buffer[counter],&counter);
                cfg.pid2.Tf = raw_to_float(&buffer[counter],&counter);
            }
            serial_reply_success();
        break;
        case SET_OUTPUT_SCALES:
            cfg.output.scaleDAC1ToA = raw_to_float(&buffer[counter],&counter);
            cfg.output.scaleDAC2ToV = raw_to_float(&buffer[counter],&counter);
            cfg.output.scaleVoltageToAmpere = raw_to_float(&buffer[counter],&counter);
            serial_reply_success();
        break;
        case SET_OUTPUT_LIMITS:
            cfg.output.maxAmps = raw_to_float(&buffer[counter],&counter);
            cfg.output.maxVoltage = raw_to_float(&buffer[counter],&counter);
            cfg.output.maxPowerloss = raw_to_float(&buffer[counter],&counter);
            serial_reply_success();
        break;
        case SET_INPUT_SCALES:
            for(int i = 0;i < 4; i++)
                cfg.input.scale[i] = raw_to_float(&buffer[counter],&counter);
            serial_reply_success();
        break;
        case SET_TEMPERATURE:
            control.setpoint[PID1] = raw_to_float(&buffer[counter],&counter);
            control.setpoint[PID2] = raw_to_float(&buffer[counter],&counter);
            serial_reply_success();
        break;
        case SET_VOLTAGE:
            cfg.output.voltageOutput = raw_to_float(&buffer[counter],&counter);
            serial_reply_success();            
        break;
        case SET_CURRENT:
            if (buffer[counter] == 1) {
                counter++;
                cfg.output.internalAmpereOutput = raw_to_float(&buffer[counter], &counter);
            } else if (buffer[counter] == 2) {
                counter++;
                cfg.output.externalAmpereOutput = raw_to_float(&buffer[counter], &counter);
            }
            serial_reply_success();
        break;
        case SET_CYCLETIME:
            cfg.cycletime = raw_to_int32_t(&buffer[counter], &counter);
            serial_reply_success();
        break;
        
        case READ_TEMPERATURES:
        *((float*)(&buf[0])) = control.temperature[SENSOR1];
        *((float*)(&buf[4])) = control.temperature[SENSOR2];
        serial_reply(buf, 8, READ_TEMPERATURES);
            
        break;
        

    }
    
}

static uint8_t calculate_CRC16(char *data, int length){
    uint16_t crc16_1, crc16_2;
    
    crc16_1 = crc16_ccitt(data, length-2);
    crc16_2 = (uint16_t)data[length] << 8 | data[length - 1];
    return (crc16_1 == crc16_2);
}*/
