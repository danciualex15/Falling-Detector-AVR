#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/wdt.h>

#include "usart.h"
#include "twi.h"
#include "buzzer.h"
#include "mpu6050.h"

#define BAUD 9600

// Freefall: < 0.4g (4096 * 0.4 = 1638). Squared = 2,683,044
#define FREEFALL_SQ_THRESHOLD 2683044UL 

// Impact: > 3.0g (4096 * 3.0 = 12288). Squared = 150,994,944
#define IMPACT_SQ_THRESHOLD   150994944UL

int main(void) {
    int16_t ax, ay, az;
    uint32_t magnitude_sq;
    
    uint8_t fall_state = 0; 
    uint16_t impact_timer = 0;

    MCUSR = 0;
    wdt_disable();

    buzzer_init();
    MPU6050_Init_FallDetect();
    USART0_init(CALC_USART_UBRR(BAUD));
	USART0_use_stdio();

    while (1) {
        if (mpu6050_get_accel(&ax, &ay, &az)) {
            printf("!!! I2C TIMEOUT - Resetting !!!\n");
            twi_stop();
            _delay_ms(100);
            twi_init();
            _delay_ms(100);
            MPU6050_Init_FallDetect();
            ax = ay = az = 0;
        }
        printf("X: %5d | Y: %5d | Z: %5d\n", ax, ay, az);

        magnitude_sq = ((int32_t)ax * ax) + ((int32_t)ay * ay) + ((int32_t)az * az);

        if (fall_state == 0) {
            // STATE 0: Waiting for free-fall
            if (magnitude_sq < FREEFALL_SQ_THRESHOLD) {
                fall_state = 1;
                impact_timer = 0;
            }
        } 
        else if (fall_state == 1) {
            // STATE 1: Waiting for Impact
            impact_timer++;
            
            if (magnitude_sq > IMPACT_SQ_THRESHOLD) {
                // Impact detected 
                fall_state = 2; 
            } 
            else if (impact_timer > 50) { 
                // Timeout: it was in free-fall but never hit anything hard
                fall_state = 0; 
            }
        } 
        else if (fall_state == 2) {
            // STATE 2: ALARM!
            buzzer_on();
            _delay_ms(2000);
            buzzer_off();
            fall_state = 0;
        }

        _delay_ms(10);
    }
}