#define SYSCTL_RCGCGPIO_R   (*((volatile int *) 0x400FE608))   
#define GPIO_PORTB_DEN_R    (*((volatile int *) 0x4000551C))
#define GPIO_PORTB_DIR_R    (*((volatile int *) 0x40005400))
#define GPIO_PORTB_DATA_R   (*((volatile int *) 0x40005030))
#define GPIO_PORTB_AMSEL_R  (*((volatile int *) 0x40005528))
#define GPIO_PORTB_AFSEL_R  (*((volatile int *) 0x40005420))
#define GPIO_PORTB_PCTL_R   (*((volatile int *) 0x4000552C))
#define GPIO_PORTB_PUR_R    (*((volatile int *) 0x40005510))
#define GPIO_PORTB_LOCK_R   (*((volatile int *) 0x40005520))
#define GPIO_PORTB_CR_R     (*((volatile int *) 0x40005524))	
#define GPIO_PORTB_CLK_EN  0x02      //enable clock for PORTB
#define GPIO_PORTB_PIN2_EN 0x04			 //enable pin 2 of PORTB
#define GPIO_PORTB_PIN3_EN 0x08			 //enable pin 3 of PORTB
#define GPIO_PORTF_DEN_R (*(( volatile unsigned long *)0x4002551C))
#define LED_ON2            0x04			 //turn on  LED on Pin 2 PORTB
#define LED_OFF2           0x00			 //turn off  LED on Pin 2 PORTB
#define GPIO_PORTF_DIR_R *(( volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DATA_R (*(( volatile unsigned long *)0x40025038)) 