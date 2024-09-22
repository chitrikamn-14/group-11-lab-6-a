#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
// Function Declarations
void GPIO_PORTF_setup(void);   // Sets up GPIO Port F pins and interrupts
void PWMConfig(void);          // Configures the PWM generator
void GPIOFHandler(void);       // Interrupt handler for button presses
void dual_switch(void);        // Logic to change duty cycle based on button press
void SysTickHandler(void) {}   // Placeholder for SysTick, not used in this code

// Constants
#define frequency 100000      // 100 kHz frequency
#define time_period (16000000/frequency)  // Time period based on 16 MHz system clock
#define del_duty 15           // Duty cycle change for each button press


// Global Variables
volatile int duty;  // Variable to store current PWM duty cycle

// GPIO Port F setup to configure buttons and LEDs, with interrupts enabled
void GPIO_PORTF_setup(void)
{
    SYSCTL_RCGCGPIO_R |= (1 << 5);        // Enable clock for Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;       // Unlock Port F
    GPIO_PORTF_CR_R = 0x1F;               // Allow changes to PF4-PF0
    GPIO_PORTF_PUR_R = 0x11;              // Enable pull-up resistors for PF4 (SW1) and PF0 (SW2)
    GPIO_PORTF_DEN_R = 0x1F;              // Enable digital functionality for PF4-PF0
    GPIO_PORTF_DIR_R = 0x0E;              // Set PF3, PF2, PF1 as output (LEDs), PF4, PF0 as input (buttons)

    // Configure interrupts for buttons on PF4 (SW1) and PF0 (SW2)
    GPIO_PORTF_IM_R &= ~0x11;             // Mask interrupts for PF4 and PF0 during setup
    GPIO_PORTF_IS_R &= ~0x11;             // Edge-sensitive interrupts
    GPIO_PORTF_IBE_R &= ~0x11;            // Trigger interrupt on one edge (falling edge)
    GPIO_PORTF_IEV_R &= ~0x11;            // Set falling edge as the trigger

    GPIO_PORTF_ICR_R |= 0x11;             // Clear any prior interrupt on PF4 and PF0
    GPIO_PORTF_IM_R |= 0x11;              // Unmask interrupts for PF4 and PF0

    NVIC_PRI7_R &= 0xFF3FFFFF;            // Set priority for Port F interrupt
    NVIC_EN0_R |= (1 << 30);              // Enable interrupt 30 (Port F)
}

// Configure the PWM generator for 100 kHz frequency on PF2 with variable duty cycle
void PWMConfig(void)
{
    SYSCTL_RCGCPWM_R |= (1 << 1);         // Enable clock for PWM1
    GPIO_PORTF_AFSEL_R |= (1 << 2);       // Enable alternate function for PF2 (PWM output)
    GPIO_PORTF_PCTL_R |= 0x500;           // Configure PF2 as PWM output

    PWM1_3_CTL_R = 0x00;                  // Disable PWM generator 3 while configuring
    PWM1_3_GENA_R = 0x8C;                 // Set PWM3A for a down-counting PWM signal
    PWM1_3_LOAD_R = 160;                  // Set period for 100 kHz PWM (time_period = 160)
    PWM1_3_CMPA_R = (duty / 100) * time_period - 1; // Set initial duty cycle
    PWM1_3_CTL_R |= 0x01;                 // Enable PWM generator 3
    PWM1_ENABLE_R |= 0x040;               // Enable PWM1 on PF2 (PWM channel 6)
}

