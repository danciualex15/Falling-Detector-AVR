#include <avr/io.h>
#include <util/delay.h>
#include "buzzer.h"

#define BUZZER_PIN PD3

void buzzer_init(void) {
    DDRD |= (1 << BUZZER_PIN); 
}

void buzzer_on(void) {
    PORTD |= (1 << BUZZER_PIN);
}

void buzzer_off(void) {
    PORTD &= ~(1 << BUZZER_PIN);
}