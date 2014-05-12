#include "board.h"
#include "cli.h"

// we unset this on 'exit'
extern uint8_t cliMode;
static void cliDefaults(char *cmdline);
static void cliDump(char *cmdLine);
static void cliExit(char *cmdline);
static void cliHelp(char *cmdline);
static void cliSave(char *cmdline);
static void cliSet(char *cmdline);
static void cliStatus(char *cmdline);
static void cliVersion(char *cmdline);

// include global vars here
extern config_t cfg;
extern control_t control;
extern float input_voltage[ADC_CHANNEL_COUNT];

// signal that we're in cli mode
uint8_t cliMode = 0;

// buffer
static char cliBuffer[48];
static uint32_t bufferIndex = 0;

static float _atof(const char *p);
// static char *ftoa(float x, char *floatString);

typedef struct {
    const char *name;
    const char *param;
    void (*func) (char *cmdline);
} clicmd_t;

// should be sorted a..z for bsearch()
const clicmd_t cmdTable[] = {
    {"defaults", "reset to defaults and reboot", cliDefaults},
    {"dump", "print configurable settings in a pastable form", cliDump},
    {"exit", "", cliExit},
    {"help", "", cliHelp},
    {"save", "save and reboot", cliSave},
    {"set", "name=value or blank or * for list", cliSet},
    {"status", "show system status", cliStatus},
    {"version", "", cliVersion},
};

#define CMD_COUNT (sizeof(cmdTable) / sizeof(clicmd_t))

typedef enum {
    VAR_UINT8,
    VAR_INT8,
    VAR_UINT16,
    VAR_INT16,
    VAR_UINT32,
    VAR_FLOAT
} vartype_e;

typedef struct {
    const char *name;
    const uint8_t type;         // vartype_e
    void *ptr;
    const int32_t min;
    const int32_t max;
} clivalue_t;

const clivalue_t valueTable[] = {
    {"cycleTime_ms", VAR_UINT32, &cfg.cycletime, 10, 5000},
    {"manualMode", VAR_UINT8, &cfg.manualMode, 0, 1},
    {"voltage", VAR_FLOAT, &cfg.output.voltageOutput, 0, 10},
    {"current", VAR_FLOAT, &cfg.output.internalAmpereOutput, 0, 10},
    {"maxvoltage", VAR_FLOAT, &cfg.output.maxVoltage, 0, 20},
    {"maxAmps", VAR_FLOAT, &cfg.output.maxAmps, 0, 10},
    {"maxPowerloss", VAR_FLOAT, &cfg.output.maxPowerloss, 0, 15},
    {"dacampereoffset", VAR_FLOAT, &cfg.output.ampereOffset, -1, 1},
    {"dac1ampscale", VAR_FLOAT, &cfg.output.scaleDAC1ToA, 0, 1000},
    {"dac2scalevolts", VAR_FLOAT, &cfg.output.scaleDAC2ToV, 0, 1000},
    {"scalevoltstoamps", VAR_FLOAT, &cfg.output.scaleVoltageToAmpere, 0, 100},
    {"dacvoltoffset", VAR_FLOAT, &cfg.output.voltageOffset, 0, 1},
    {"setpoint1", VAR_FLOAT, &control.setpoint[PID1], 0, 1000},
    {"setpoint2", VAR_FLOAT, &control.setpoint[PID2], 0, 1000},
    {"pid1Pterm", VAR_FLOAT, &cfg.pid1.P, 0, 100},
    {"pid1Iterm", VAR_FLOAT, &cfg.pid1.I, 0, 10},
    {"pid1Dterm", VAR_FLOAT, &cfg.pid1.D, 0, 10},
    {"pid1Windup", VAR_FLOAT, &cfg.pid1.windup, 0, 1000},
    {"pid2Pterm", VAR_FLOAT, &cfg.pid2.P, 0, 100},
    {"pid2Iterm", VAR_FLOAT, &cfg.pid2.I, 0, 10},
    {"pid2Dterm", VAR_FLOAT, &cfg.pid2.D, 0, 10},
    {"pid2Windup", VAR_FLOAT, &cfg.pid2.windup, 0, 1000},
    {"debug", VAR_UINT8, &cfg.debug, 0, 1},
};

#define VALUE_COUNT (sizeof(valueTable) / sizeof(clivalue_t))


typedef union {
    int32_t int_value;
    float float_value;
} int_float_value_t;

static void cliSetVar(const clivalue_t * var, const int_float_value_t value);
static void cliPrintVar(const clivalue_t * var, uint32_t full);
static void cliPrint(const char *str);
static void cliWrite(uint8_t ch);

#ifndef HAVE_ITOA_FUNCTION

/*
** The following two functions together make up an itoa()
** implementation. Function i2a() is a 'private' function
** called by the public itoa() function.
**
** itoa() takes three arguments:
**        1) the integer to be converted,
**        2) a pointer to a character conversion buffer,
**        3) the radix for the conversion
**           which can range between 2 and 36 inclusive
**           range errors on the radix default it to base10
** Code from http://groups.google.com/group/comp.lang.c/msg/66552ef8b04fe1ab?pli=1
*/

static char *i2a(unsigned i, char *a, unsigned r)
{
    if (i / r > 0)
        a = i2a(i / r, a, r);
    *a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i % r];
    return a + 1;
}

char *itoa(int i, char *a, int r)
{
    if ((r < 2) || (r > 36))
        r = 10;
    if (i < 0) {
        *a = '-';
        *i2a(-(unsigned) i, a + 1, r) = 0;
    } else
        *i2a(i, a, r) = 0;
    return a;
}

#endif

////////////////////////////////////////////////////////////////////////////////
// String to Float Conversion
///////////////////////////////////////////////////////////////////////////////
// Simple and fast atof (ascii to float) function.
//
// - Executes about 5x faster than standard MSCRT library atof().
// - An attractive alternative if the number of calls is in the millions.
// - Assumes input is a proper integer, fraction, or scientific format.
// - Matches library atof() to 15 digits (except at extreme exponents).
// - Follows atof() precedent of essentially no error checking.
//
// 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
//
#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')
static float _atof(const char *p)
{
    int frac = 0;
    float sign, value, scale;

    // Skip leading white space, if any.
    while (white_space(*p)) {
        p += 1;
    }

    // Get sign, if any.
    sign = 1.0f;
    if (*p == '-') {
        sign = -1.0f;
        p += 1;

    } else if (*p == '+') {
        p += 1;
    }
    // Get digits before decimal point or exponent, if any.
    value = 0.0f;
    while (valid_digit(*p)) {
        value = value * 10.0f + (*p - '0');
        p += 1;
    }

    // Get digits after decimal point, if any.
    if (*p == '.') {
        float pow10 = 10.0f;
        p += 1;

        while (valid_digit(*p)) {
            value += (*p - '0') / pow10;
            pow10 *= 10.0f;
            p += 1;
        }
    }
    // Handle exponent, if any.
    scale = 1.0f;
    if ((*p == 'e') || (*p == 'E')) {
        unsigned int expon;
        p += 1;

        // Get sign of exponent, if any.
        frac = 0;
        if (*p == '-') {
            frac = 1;
            p += 1;

        } else if (*p == '+') {
            p += 1;
        }
        // Get digits of exponent, if any.
        expon = 0;
        while (valid_digit(*p)) {
            expon = expon * 10 + (*p - '0');
            p += 1;
        }
        if (expon > 308)
            expon = 308;

        // Calculate scaling factor.
        // while (expon >= 50) { scale *= 1E50f; expon -= 50; }
        while (expon >= 8) {
            scale *= 1E8f;
            expon -= 8;
        }
        while (expon > 0) {
            scale *= 10.0f;
            expon -= 1;
        }
    }
    // Return signed and scaled floating point result.
    return sign * (frac ? (value / scale) : (value * scale));
}

///////////////////////////////////////////////////////////////////////////////
// FTOA
///////////////////////////////////////////////////////////////////////////////
char *ftoa(float x, char *floatString)
{
    int32_t value;
    char intString1[12];
    char intString2[12] = { 0, };
    char *decimalPoint = ".";
    uint8_t dpLocation;

    if (x > 0)                  // Rounding for x.xxx display format
        x += 0.0005f;
    else
        x -= 0.0005f;

    value = (int32_t) (x * 1000.0f);    // Convert float * 1000 to an integer

    itoa(abs(value), intString1, 10);   // Create string from abs of integer value

    if (value >= 0)
        intString2[0] = ' ';    // Positive number, add a pad space
    else
        intString2[0] = '-';    // Negative number, add a negative sign

    if (strlen(intString1) == 1) {
        intString2[1] = '0';
        intString2[2] = '0';
        intString2[3] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 2) {
        intString2[1] = '0';
        intString2[2] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 3) {
        intString2[1] = '0';
        strcat(intString2, intString1);
    } else {
        strcat(intString2, intString1);
    }

    dpLocation = strlen(intString2) - 3;

    strncpy(floatString, intString2, dpLocation);
    floatString[dpLocation] = '\0';
    strcat(floatString, decimalPoint);
    strcat(floatString, intString2 + dpLocation);

    return floatString;
}

static void cliPrompt(void)
{
    cliPrint("\r\n# ");
}

static int cliCompare(const void *a, const void *b)
{
    const clicmd_t *ca = a, *cb = b;
    return strncasecmp(ca->name, cb->name, strlen(cb->name));
}

static void cliDefaults(char *cmdline)
{
    resetConf();
    cliPrint("Resetting to defaults...\r\n");
    delay(10);
}

static void cliDump(char *cmdline)
{
    int i;
    const clivalue_t *setval;

    // print settings
    for (i = 0; i < VALUE_COUNT; i++) {
        setval = &valueTable[i];
        printf("set %s = ", valueTable[i].name);
        cliPrintVar(setval, 0);
        cliPrint("\r\n");
    }
}

static void cliExit(char *cmdline)
{
    cliPrint("\r\nLeaving CLI mode...\r\n");
}

static void cliHelp(char *cmdline)
{
    uint32_t i = 0;

    cliPrint("Available commands:\r\n");
    for (i = 0; i < CMD_COUNT; i++)
        printf("%s\t%s\r\n", cmdTable[i].name, cmdTable[i].param);
}

static void cliSave(char *cmdline)
{
    if(save_config())
        cliPrint("data saved");
    else 
        cliPrint("failed");
}

static void cliPrint(const char *str)
{
    UB_USB_CDC_SendString((char *) str);
}

static void cliWrite(uint8_t ch)
{
    UB_USB_CDC_SendChar((char) ch);
}

static void cliPrintVar(const clivalue_t * var, uint32_t full)
{
    int32_t value = 0;
    char buf[20];

    switch (var->type) {
    case VAR_UINT8:
        value = *(uint8_t *) var->ptr;
        break;

    case VAR_INT8:
        value = *(int8_t *) var->ptr;
        break;

    case VAR_UINT16:
        value = *(uint16_t *) var->ptr;
        break;

    case VAR_INT16:
        value = *(int16_t *) var->ptr;
        break;

    case VAR_UINT32:
        value = *(uint32_t *) var->ptr;
        break;

    case VAR_FLOAT:
        printf("%s", ftoa(*(float *) var->ptr, buf));
        if (full) {
            printf(" %s", ftoa((float) var->min, buf));
            printf(" %s", ftoa((float) var->max, buf));
        }
        return;                 // return from case for float only
    }
    printf("%d", value);
    if (full)
        printf(" %d %d", var->min, var->max);
}

static void cliSetVar(const clivalue_t * var, const int_float_value_t value)
{
    switch (var->type) {
    case VAR_UINT8:
    case VAR_INT8:
        *(char *) var->ptr = (char) value.int_value;
        break;

    case VAR_UINT16:
    case VAR_INT16:
        *(short *) var->ptr = (short) value.int_value;
        break;

    case VAR_UINT32:
        *(int *) var->ptr = (int) value.int_value;
        break;

    case VAR_FLOAT:
        *(float *) var->ptr = (float) value.float_value;
        break;
    }
}

static void cliSet(char *cmdline)
{

    int i;
    int len;
    const clivalue_t *val;
    char *eqptr = NULL;
    int value = 0;
    float valuef = 0;

    len = strlen(cmdline);


    if (len == 0 || (len == 1 && cmdline[0] == '*')) {
        cliPrint("Current settings: \r\n");
        for (i = 0; i < VALUE_COUNT; i++) {
            val = &valueTable[i];
            printf("%s = ", valueTable[i].name);
            cliPrintVar(val, len);      // when len is 1 (when * is passed as argument), it will print min/max values as well, for gui
            cliPrint("\r\n");
        }
    } else if ((eqptr = strstr(cmdline, "=")) != NULL) {
        // has equal, set var
        eqptr++;
        len--;
        value = atoi(eqptr);
        valuef = _atof(eqptr);
        for (i = 0; i < VALUE_COUNT; i++) {
            val = &valueTable[i];
            if (strncasecmp(cmdline, valueTable[i].name, strlen(valueTable[i].name)) == 0) {
                if (valuef >= valueTable[i].min && valuef <= valueTable[i].max) {       // here we compare the float value since... it should work, RIGHT?
                    int_float_value_t tmp;
                    if (valueTable[i].type == VAR_FLOAT)
                        tmp.float_value = valuef;
                    else
                        tmp.int_value = value;
                    cliSetVar(val, tmp);
                    printf("%s set to ", valueTable[i].name);
                    cliPrintVar(val, 0);
                } else {
                    cliPrint("ERR: Value assignment out of range\r\n");
                }
                return;
            }
        }
        cliPrint("ERR: Unknown variable name\r\n");
    } else {
        // no equals, check for matching variables.
        for (i = 0; i < VALUE_COUNT; i++) {
            if (strstr(valueTable[i].name, cmdline)) {
                val = &valueTable[i];
                printf("%s = ", valueTable[i].name);
                cliPrintVar(val, 0);
                printf("\r\n");
            }
        }
    }
}

static void cliStatus(char *cmdline)
{
    char buf[10];

    printf("System Uptime: %d seconds\r\n", millis() / 1000);
    printf("Temperature1: %s degC \r\n", ftoa(control.temperature[SENSOR1], buf));
    printf("Temperature2: %s degC \r\n", ftoa(control.temperature[SENSOR2], buf));
    printf("ADC_MOSFET_V %s \r\n",ftoa(input_voltage[0], buf));
    printf("ADC_MOSFET_A %s \r\n", ftoa(input_voltage[1], buf));
    printf("ADC_VIN %s \r\n", ftoa(input_voltage[2], buf));
    printf("ADC_SENS %s \r\n", ftoa(input_voltage[3], buf));
    printf("eeprom: %d\n\r", cfg.eeprom);
}

static void cliVersion(char *cmdline)
{
    cliPrint("Afro32 CLI version 2.2 " __DATE__ " / " __TIME__);
}

void cliProcess(void)
{
    uint8_t c;
    int i = 0;

    if (!cliMode) {
        cliMode = 1;
        cliPrint("\r\nEntering CLI Mode, type 'exit' to return, or 'help'\r\n");
        cliPrompt();
    }

    while (UB_USB_CDC_DataIsReady()) {
            c = UB_USB_CDC_GetChar();
            if (c == '\t' || c == '?') {
                // do tab completion
                const clicmd_t *cmd, *pstart = NULL, *pend = NULL;
                int i = bufferIndex;
                for (cmd = cmdTable; cmd < cmdTable + CMD_COUNT; cmd++) {
                    if (bufferIndex && (strncasecmp(cliBuffer, cmd->name, bufferIndex) != 0))
                        continue;
                    if (!pstart)
                        pstart = cmd;
                    pend = cmd;
                }
                if (pstart) {   /* Buffer matches one or more commands */
                    for (;; bufferIndex++) {
                        if (pstart->name[bufferIndex] != pend->name[bufferIndex])
                            break;
                        if (!pstart->name[bufferIndex] && bufferIndex < sizeof(cliBuffer) - 2) {
                            /* Unambiguous -- append a space */
                            cliBuffer[bufferIndex++] = ' ';
                            cliBuffer[bufferIndex] = '\0';
                            break;
                        }
                        cliBuffer[bufferIndex] = pstart->name[bufferIndex];
                    }
                }
                if (!bufferIndex || pstart != pend) {
                    /* Print list of ambiguous matches */
                    cliPrint("\r\033[K");
                    for (cmd = pstart; cmd <= pend; cmd++) {
                        cliPrint(cmd->name);
                        cliWrite('\t');
                    }
                    cliPrompt();
                    i = 0;      /* Redraw prompt */
                }
                for (; i < bufferIndex; i++)
                    cliWrite(cliBuffer[i]);
            } else if (!bufferIndex && c == 4) {
                cliExit(cliBuffer);
                return;
            } else if (c == 12) {
                // clear screen
                cliPrint("\033[2J\033[1;1H");
                cliPrompt();
            } else if (bufferIndex && (c == '\n' || c == '\r')) {
                // enter pressed
                clicmd_t *cmd = NULL;
                clicmd_t target;
                cliPrint("\r\n");
                cliBuffer[bufferIndex] = 0;     // null terminate

                target.name = cliBuffer;
                target.param = NULL;

                cmd = bsearch(&target, cmdTable, CMD_COUNT, sizeof cmdTable[0], cliCompare);
                if (cmd)
                    cmd->func(cliBuffer + strlen(cmd->name) + 1);
                else
                    cliPrint("ERR: Unknown command, try 'help'");

                memset(cliBuffer, 0, sizeof(cliBuffer));
                bufferIndex = 0;

                // 'exit' will reset this flag, so we don't need to print prompt again
                if (!cliMode)
                    return;
                cliPrompt();
            } else if (c == 127) {
                // backspace
                if (bufferIndex) {
                    cliBuffer[--bufferIndex] = 0;
                    cliPrint("\010 \010");
                }
            } else if (bufferIndex < sizeof(cliBuffer) && c >= 32 && c <= 126) {
                if (!bufferIndex && c == 32)
                    continue;
                cliBuffer[bufferIndex++] = c;
                cliWrite(c);
            }
            i++;
    }
}
