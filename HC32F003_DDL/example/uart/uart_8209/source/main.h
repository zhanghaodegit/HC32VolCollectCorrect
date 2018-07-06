#include <stdint.h>
#define Kv	0.0005815
//#define Ki	81846.617
#define Ki	1
#define Kp 	0.00024769

typedef union
{
    struct
    {
        char byte[12];  //
    };
    struct
    {
        uint16_t head;
        uint16_t rms[3];  
    };
    
}RESULT;  //ÎŞ·ûºÅÕûÊı

typedef struct {
    uint8_t HEAD;
    uint8_t CMD;
    uint8_t DATA[6];
    uint8_t SUM;
}uart_send_t;

uint8_t ChkHostSum(uint8_t *in, int num) {
    uint8_t sum = 0;
    
    while (num--) {
        sum += *in++;
    }
    
    return sum;
}

uint8_t ChkRnSum(uint8_t reg, uint8_t *in, int num) {
    uint8_t sum = 0;
    
	sum += reg;
    while (num--) {
        sum += *in++;
    }    
    return sum;
}
