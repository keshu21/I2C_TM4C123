#include "TM4C123.h"                    // Device header
#include <stdio.h>

void I2C_Init(void);
void I2C0_Send(uint8_t slave_addr, char data);
void PORTF_Init(void);
void delay(unsigned long);
char I2C0_Receive(uint8_t slave_addr);

#define RCGCI2C (*((volatile unsigned long*)(0x400FE000+0x620)))
#define RCGCGPIO (*((volatile unsigned long*)(0x400FE000+0x608)))

// PB2->SCL and PB3->SDA
#define PORTB_BASE  0x40005000
#define PORTB_AFSEL_R  (*((volatile unsigned long*) (PORTB_BASE + 0x420)))
#define PORTB_PCTL_R   (*((volatile unsigned long*) (PORTB_BASE + 0x52C)))
#define PORTB_DIR_R    (*((volatile unsigned long*) (PORTB_BASE + 0x400)))
#define PORTB_DEN_R    (*((volatile unsigned long*) (PORTB_BASE + 0x51C)))
#define PORTB_DATA_R   (*((volatile unsigned long*) (PORTB_BASE + 0x3FC)))
#define PORTB_AMSEL_R  (*((volatile unsigned long*) (PORTB_BASE + 0x528)))
#define PORTB_ODR_R    (*((volatile unsigned long*) (PORTB_BASE + 0x50C)))

#define I2C0_BASE      (0x40020000)
#define I2C_MSA_R      (*((volatile unsigned long*) (I2C0_BASE + 0x00)))
#define I2C_MCS_R      (*((volatile unsigned long*) (I2C0_BASE + 0x004)))
#define I2C_MDR_R      (*((volatile unsigned long*) (I2C0_BASE + 0x008)))
#define I2C_MTPR_R     (*((volatile unsigned long*) (I2C0_BASE + 0x00C)))
#define I2C_MCR_R      (*((volatile unsigned long*) (I2C0_BASE + 0x020)))

#define PORTF_BASE    0x40025000
#define PORTF_DIR_R   (*((volatile unsigned long*) (PORTF_BASE + 0x400)))
#define PORTF_DEN_R   (*((volatile unsigned long*) (PORTF_BASE + 0x51C)))
#define PORTF_DATA_R  (*((volatile unsigned long*) (PORTF_BASE + 0x3FC)))
#define PORTF_AFSEL_R (*((volatile unsigned long*) (PORTF_BASE + 0x420)))
#define PORTF_PCTL_R  (*((volatile unsigned long*) (PORTF_BASE + 0x52C)))
#define PORTF_AMSEL_R (*((volatile unsigned long*) (PORTF_BASE + 0x528)))

int main(){
    char arr[3] = {'R', 'B', 'G'};
    I2C_Init();
    PORTF_Init();
    uint8_t ad = 0x2C;
    int i = 0;
    delay(1000);
    while(1) {
        I2C0_Send(ad, arr[i]);
        delay(2000000);
        char data = I2C0_Receive(ad);
        
        if (data == 'R') {
            PORTF_DATA_R = 0x02; // Red LED on PF1
        } else if (data == 'G') {
            PORTF_DATA_R = 0x08; // Green LED on PF3
        } else if (data == 'B') {
            PORTF_DATA_R = 0x04; // Blue LED on PF2
        }
        
        i = (i + 1) % 3;
        delay(2000000);
    }
}

void I2C_Init(void) {
    RCGCI2C |= 0x01;
    RCGCGPIO |= 0x02;

    PORTB_AFSEL_R |= 0x0C;
    PORTB_ODR_R |= 0x08; // Open drain for SDA
    PORTB_DEN_R |= 0x0C;
    PORTB_PCTL_R |= 0x3300;
    PORTB_AMSEL_R &= ~0x0C;

    I2C_MCR_R = 0x10; // Initialize master function
    I2C_MTPR_R = 7; // Set clock speed to 100kbps
}

void PORTF_Init(void) {
    RCGCGPIO |= 0x20; // Enable GPIO Port F clock
    PORTF_DIR_R = 0x0E; // PF1, PF2, and PF3 as output (LEDs)
    PORTF_DEN_R |= 0x0E; // Digital enable PF1, PF2, and PF3
    PORTF_AFSEL_R &= ~0x0E; // Disable alternate function
    PORTF_PCTL_R &= ~0xFFF0; // Configure as GPIO
    PORTF_AMSEL_R &= ~0x0E; // Disable analog function
}

void I2C0_Send(uint8_t slave_addr, char data) {
    while (I2C_MCS_R & 0x01) {}; // Wait until I2C0 is not busy
    I2C_MSA_R = (slave_addr << 1) & 0xFE; // Set slave address and write operation
    I2C_MDR_R = data & 0xFF;
    I2C_MCS_R = 0x07; // Start, Run, and Stop conditions

    while (I2C_MCS_R & 0x01) {}; // Wait for the transmission to complete
}

char I2C0_Receive(uint8_t slave_addr) {
    while (I2C_MCS_R & 0x01) {}; // Wait until I2C0 is not busy
    I2C_MSA_R = (slave_addr << 1) | 0x01; // Set slave address and read operation
    I2C_MCS_R = 0x07; // Start, Run, and Stop conditions

    while (I2C_MCS_R & 0x01) {}; // Wait for the reception to complete
    return (char) I2C_MDR_R & 0xFF;
}

void delay(unsigned long t) {
    unsigned long i = t;
    while (i > 0) {
        i--;
    }
}





