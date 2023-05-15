#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

#define MOTOR_PWM_FREQ 1000  // Motor PWM frequency in Hz
#define BUTTON_POLL_PERIOD_MS 10  // Button poll period in milliseconds

// Motor control pins
#define MOTOR_ENABLE_GPIO_BASE GPIO_PORTF_BASE
#define MOTOR_ENABLE_PIN GPIO_PIN_3
#define MOTOR_A_GPIO_BASE GPIO_PORTC_BASE
#define MOTOR_A_PIN GPIO_PIN_4
#define MOTOR_B_GPIO_BASE GPIO_PORTC_BASE
#define MOTOR_B_PIN GPIO_PIN_5

// Button pins
#define BUTTON_FORWARD_GPIO_BASE GPIO_PORTD_BASE
#define BUTTON_FORWARD_PIN GPIO_PIN_6
#define BUTTON_BACKWARD_GPIO_BASE GPIO_PORTD_BASE
#define BUTTON_BACKWARD_PIN GPIO_PIN_7

// Task handles
TaskHandle_t motorControlTaskHandle;

// Motor direction
typedef enum {
    MOTOR_STOP,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} MotorDirection;

// Motor control parameters
typedef struct {
    MotorDirection direction;
    uint32_t speed;
} MotorControlParams;

// Button states
typedef enum {
    BUTTON_RELEASED,
    BUTTON_PRESSED
} ButtonState;

// Button states for forward and backward buttons
ButtonState forwardButtonState = BUTTON_RELEASED;
ButtonState backwardButtonState = BUTTON_RELEASED;

// Motor control task
void motorControlTask(void *pvParameters)
{
    MotorControlParams motorParams = {.direction = MOTOR_STOP, .speed = 0};

    for (;;) {
        // Update motor control based on button states
        if (forwardButtonState == BUTTON_PRESSED && backwardButtonState == BUTTON_RELEASED) {
            motorParams.direction = MOTOR_FORWARD;
            motorParams.speed = 50;  // Set motor speed as desired
        } else if (forwardButtonState == BUTTON_RELEASED && backwardButtonState == BUTTON_PRESSED) {
            motorParams.direction = MOTOR_BACKWARD;
            motorParams.speed = 50;  // Set motor speed as desired
        } else {
            motorParams.direction = MOTOR_STOP;
            motorParams.speed = 0;
        }

        // Update motor control
        // Set H-bridge pins based on motor direction
        switch (motorParams.direction) {
            case MOTOR_STOP:
                GPIOPinWrite(MOTOR_A_GPIO_BASE, MOTOR_A_PIN, 0);
                GPIOPinWrite(MOTOR_B_GPIO_BASE, MOTOR_B_PIN, 0);
                break;
            case MOTOR_FORWARD:
                GPIOPinWrite(MOTOR_A_GPIO_BASE, MOTOR_A_PIN, MOTOR_A_PIN);
                GPIOPinWrite(MOTOR_B_GPIO_BASE, MOTOR_B_PIN, 0);
                break;
            case MOTOR_BACKWARD:
                GPIOPinWrite(MOTOR_A_GPIO_BASE, MOTOR_A_PIN, 0);
                GPIOPinWrite(MOTOR_B_GPIO_BASE, MOTOR_B_PIN, MOTOR_B_PIN);
                break;
        }

        // Update motor speed by changing PWM duty cycle

        // The code to update the motor speed goes here
        // You will need to set up PWM output and control its duty cycle based on motorParams.speed

        vTaskDelay(pdMS_TO_TICKS(10));  // Delay for 10ms
    }
}
