#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
// Function Declarations
void GPIO_PORTF_setup(void);   // Sets up GPIO Port F pins and interrupts
void PWMConfig(void);          // Configures the PWM generator
void GPIOFHandler(void);       // Interrupt handler for button presses
void dual_switch(void);        // Logic to change duty cycle based on button press
void SysTickHandler(void) {}   // Placeholder for SysTick, not used in this code


