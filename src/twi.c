#include "twi.h"
#include <util/delay.h>

void twi_init(void) {
    DDRC  &= ~((1 << PC4) | (1 << PC5));
    PORTC |=  ((1 << PC4) | (1 << PC5));
    TWCR = 0;
    TWBR = (uint8_t)TWBR_VAL;
    TWSR = (0 << TWPS1) | (0 << TWPS0);
}

void twi_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

uint8_t twi_start_timeout(void) {
    // send the START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    // count down
    uint32_t timeout = 2000000; 
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) {
            return 1;
        }
    }
    return 0;
}

uint8_t twi_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    uint32_t timeout = 10000;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) return 1;
    }
    return 0;
}

uint8_t twi_read_ack(uint8_t *data) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    
    uint32_t timeout = 10000;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) return 1;
    }
    *data = TWDR;
    return 0;
}

void twi_read_nack(uint8_t *data) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1<<TWINT)));
    *data = TWDR;
}

void twi_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}
