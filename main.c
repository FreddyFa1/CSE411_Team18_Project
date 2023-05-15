#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "/semphr.h"
#include "projectInit.h"
#include "TM4C123.h"

	#define PB1 (1<<0);
	#define PB2 (1<<1);
	#define PB3 (1<<2);
	#define PB4 (1<<3);
	#define PB5 (1<<4);
	#define Switch1 (1<<5);
	#define LimitS1 (1<<6);
	#define LimitS2 (1<<7);
	
int task1_status;
int task2_status;
int task3_status;
int task4_status;
int task5_status;

xSemaphoreHandle xBinarySemaphore1;
xSemaphoreHandle xBinarySemaphore2;
xSemaphoreHandle xBinarySemaphore3;
xSemaphoreHandle xBinarySemaphore4;
xSemaphoreHandle xBinarySemaphore5;
xSemaphoreHandle xMutex;

int upper_limit_switch;
int lower_limit_switch;
int enable_switch;
int window_up;
int window_down;
int passengerPB1;
int passengerPB2;
int driverPB1;
int driverPB2;
int last_task_running;

void passenger_close(void *pvParameters);
void passenger_open(void *pvParameters);
void driver_close(void *pvParameters);
void driver_open(void *pvParameters);
void disable_passenger(void *pvParameters);
void emergency_close(void *pvParameters);
void update_task_status (int);
int was_task_preempted (int);
int create_semaphores ();
void Motor_Rotation(void);
void System_Init(void);

void System_Init()
{
	//Motor Init
	SYSCTL->RCGCGPIO |= 0x20;   /* enable clock to PORTF */
	GPIOA->DIR |= (1<<3)|(1<<2);            /* pin digital */
  GPIOA->DEN |= (1<<3)|(1<<2);            /* pin input */
	
	//Pushbutton and Switch init
	SYSCTL->RCGCGPIO |= 0x02;  // enable clock to PORTB
	GPIOB->DEN = 0x3EU;
	GPIOA->DIR = 0x0U;

	
	//LimitSwitch Init
	GPIOA->DEN |= (1<<7)|(1<<6); 
}
	
int main() {
  
  if (create_semaphores() == 1){
    xTaskCreate( passenger_close , "passenger_close", 240, NULL, 1, NULL );
    xTaskCreate( passenger_open , "passenger_open", 240, NULL, 1, NULL );
    xTaskCreate( driver_close , "driver_close", 240, NULL, 2, NULL );
    xTaskCreate( driver_open , "driver_open", 240, NULL, 2, NULL );
    xTaskCreate( emergency_close , "emergency_close", 240, NULL, 3, NULL );
  
    vTaskStartScheduler();
  }

  for(;;);
}

void passenger_close(void *pvParameters){
    
    for(;;){
        
      xSemaphoreTake( xBinarySemaphore1, portMAX_DELAY );
      xSemaphoreTake( xMutex, portMAX_DELAY );

      if(upper_limit_switch == 0){
        xSemaphoreGive( xMutex );
        continue;
      }
      update_task_status(1);
      vTaskDelay(50);
      window_down = 0;
      window_up = 1;
      vTaskDelay(50);
      xSemaphoreGive( xMutex );
      if(passengerPB1 == 1){
        for(;;){
          xSemaphoreTake( xMutex, portMAX_DELAY );
          if(was_task_preempted(1) == 1){
              break;
          }
          if((passengerPB1 == 0 && passengerPB2 == 0 ) || upper_limit_switch == 1
              || enable_switch == 0){
              update_task_status(1);
              window_up = 0;
              break;
          }
          if(passengerPB1 == 0 && passengerPB2 == 1){
              update_task_status(1);
              window_up = 0;
              break;
          }

          xSemaphoreGive( xMutex );
        }
        xSemaphoreGive( xMutex );
      }
      else {
        for(;;){
          xSemaphoreTake( xMutex, portMAX_DELAY );
          if(was_task_preempted(1) == 1){
            break;
          }
          if(upper_limit_switch == 1){
            update_task_status(1);
            window_up = 0;
            break;
          }
          if(passengerPB1 == 0 && passengerPB2 == 1){
            update_task_status(1);
            window_up = 0;
            break;
          }
          xSemaphoreGive( xMutex );
        }
        xSemaphoreGive( xMutex );
      }
    }
}

void passenger_open(void *pvParameters){
    
    for(;;){
        
      
      xSemaphoreTake( xBinarySemaphore2, portMAX_DELAY );
      xSemaphoreTake( xMutex, portMAX_DELAY );
      
      if(lower_limit_switch == 0){
        xSemaphoreGive( xMutex );
        continue;
      }
      
      update_task_status(2);
      vTaskDelay(50);
      window_up = 0;
      window_down = 1;
      vTaskDelay(50);
      xSemaphoreGive( xMutex );
      
      if(passengerPB2 == 1){
        for(;;){
          xSemaphoreTake( xMutex, portMAX_DELAY );
          
          if(was_task_preempted(2)){
              window_down = 0;
              break;
          }
          if((passengerPB1 == 0 && passengerPB2 == 0 ) || lower_limit_switch == 1 
             || enable_switch == 0){
              update_task_status(2);
              window_down = 0;
              break;
          }
          if(passengerPB1 == 1 && passengerPB2 == 0){
              update_task_status(2);
              window_down = 0;
              break;
          }

          xSemaphoreGive( xMutex );
        }
        xSemaphoreGive( xMutex );
      }
      else {
        for(;;){
          xSemaphoreTake( xMutex, portMAX_DELAY );
          if(was_task_preempted(2) == 1){
            window_down = 0;
            break;
          }
          if(lower_limit_switch == 1){
            update_task_status(2);
            window_down = 0;
            break;
          }
          if(passengerPB1 == 1 && passengerPB2 == 0){
            update_task_status(2);
            window_down = 0;
            break;
          }
          xSemaphoreGive( xMutex );
        }
        xSemaphoreGive( xMutex );
      }
    }
}
void driver_close(void *pvParameters){

  for(;;){
    xSemaphoreTake( xBinarySemaphore3, portMAX_DELAY ); 
    xSemaphoreTake( xMutex, portMAX_DELAY );
    update_task_status(3);
    if( upper_limit_switch == 1 || (window_down == 1 && passengerPB1 == 0) ){
      window_down = 0;
      update_task_status(3);
      xSemaphoreGive( xMutex );
      continue;
      }
    window_down = 0;
    vTaskDelay(50);
    window_up = 1;
    vTaskDelay(50);
    xSemaphoreGive( xMutex );
    if(driverPB1 == 1){
      for(;;){
        xSemaphoreTake( xMutex, portMAX_DELAY );
        if(was_task_preempted(3) == 1){
          update_task_status(3);
          window_up = 0;
          break;
        }
        if((driverPB1 == 0 && driverPB2 == 0 ) || upper_limit_switch == 1){
          update_task_status(3);
          window_up = 0;
          break;
        }
        if(driverPB1 == 0 && driverPB2 == 1){
          update_task_status(3);
          window_up = 0;
          break;
        }

        xSemaphoreGive( xMutex );
      }
      xSemaphoreGive( xMutex );
    }
    else {
      for(;;){
        xSemaphoreTake( xMutex, portMAX_DELAY );
        if(was_task_preempted(3) == 1){
          update_task_status(3);
          window_up = 0;
          break;
        }
        if(upper_limit_switch == 1){
          update_task_status(3);
          window_up = 0;
          break;
        }
        if(driverPB1 == 0 && driverPB2 == 1){
          update_task_status(3);
          window_up = 0;
          break;
        }
        xSemaphoreGive( xMutex );
      }
      xSemaphoreGive( xMutex );
    }
  }
}

void driver_open(void *pvParameters){

  for(;;){
    xSemaphoreTake( xBinarySemaphore4, portMAX_DELAY ); 
    xSemaphoreTake( xMutex, portMAX_DELAY );
    update_task_status(4);
    if( lower_limit_switch == 0 || (window_up == 1 && passengerPB2 == 0) ){
      window_up = 0;
      update_task_status(4);
      xSemaphoreGive( xMutex );
      continue;
      }
    window_up = 0;
    vTaskDelay(50);
    window_down = 1;
    vTaskDelay(50);
    xSemaphoreGive( xMutex );
    if(driverPB2 == 1){
      for(;;){
        xSemaphoreTake( xMutex, portMAX_DELAY );
        if(was_task_preempted(4) == 1){
          update_task_status(4);
          window_up = 0;
          break;
        }
        if((driverPB1 == 0 && driverPB2 == 0 ) || lower_limit_switch == 1){
          update_task_status(4);
          window_up = 0;
          break;
        }
        if(driverPB1 == 1 && driverPB2 == 0){
          update_task_status(4);
          window_up = 0;
          break;
        }

        xSemaphoreGive( xMutex );
      }
      xSemaphoreGive( xMutex );
    }
    else {
      for(;;){
        xSemaphoreTake( xMutex, portMAX_DELAY );
        if(was_task_preempted(4) == 1){
          update_task_status(4);
          window_up = 0;
          break;
        }
        if(lower_limit_switch == 1){
          update_task_status(4);
          window_up = 0;
          break;
        }
        if(driverPB1 == 1 && driverPB2 == 0){
          update_task_status(4);
          window_up = 0;
          break;
        }
        xSemaphoreGive( xMutex );
      }
      xSemaphoreGive( xMutex );
    }
  }
}

void emergency_close(void *pvParameters){
    
  for(;;){
    update_task_status(5);
    xSemaphoreTake( xBinarySemaphore5, portMAX_DELAY ); 
    if( lower_limit_switch == 1 ){
      window_up = 0;
      update_task_status(5);
      continue;
    }
    window_up = 0;
    window_down = 1;
    xSemaphoreTake( xMutex, portMAX_DELAY );
    for( int i = 0; i < 5; i++ ){
      vTaskDelay( 100 / portTICK_RATE_MS );
      if( lower_limit_switch == 1 ){
        break;
      }
    }
    xSemaphoreGive( xMutex );
    update_task_status(5);
  }
}

void update_task_status(int task_number){
    
  if (task_number == 1){
      if (task1_status == 1){
          task1_status = 0;
      }
      else{
          task1_status = 1;
      }
      last_task_running = 1;
  }
  if (task_number == 2){
      if (task2_status == 1){
          task2_status = 0;
      }
      else{
          task2_status = 1;
      }
      last_task_running = 2;
  }
  if (task_number == 3){
      if (task3_status == 1){
          task3_status = 0;
      }
      else{
          task3_status = 1;
      }
      last_task_running = 3;
  }
  if (task_number == 4){
      if (task4_status == 1){
          task4_status = 0;
      }
      else{
          task4_status = 1;
      }
      last_task_running = 4;
  }
  if (task_number == 5){
      if (task5_status == 1){
          task5_status = 0;
      }
      else{
          task5_status = 1;
      }
      last_task_running = 5;
  }
}

int was_task_preempted(int task_number){
    
    if(task_number == last_task_running){
      return 0;
    }
    else{
      return 1;
    }
}

int is_task_running(int task_number){
    
    if (task_number == 1){
      if (task1_status == 1){
        return 1; 
      }
      else{
        return 0;
      }
    }
    if (task_number == 2){
      if (task2_status == 1){
        return 1;
      }
      else{
        return 0;
      }
    }
    if (task_number == 3){
      if (task3_status == 1){
        return 1;
      }
      else{
        return 0;
      }
    }
    if (task_number == 4){
      if (task4_status == 1){
        return 1 ;
      }
      else{
        return 0;
      }
    }
    if (task_number == 5){
      if (task5_status == 1){
        return 1;  
      }
      else{
        return 0;
      }
    }
}
int create_semaphores(void){
    
  vSemaphoreCreateBinary( xBinarySemaphore1 );
  vSemaphoreCreateBinary( xBinarySemaphore2 );
  vSemaphoreCreateBinary( xBinarySemaphore3 );
  vSemaphoreCreateBinary( xBinarySemaphore4 );
  vSemaphoreCreateBinary( xBinarySemaphore5 );
  xMutex = xSemaphoreCreateMutex();
  if( xBinarySemaphore1 != NULL && xBinarySemaphore2 != NULL && 
      xBinarySemaphore3 != NULL && xBinarySemaphore4 != NULL && 
      xBinarySemaphore5 != NULL && xMutex != NULL ){
        
        return 1;
      }
  else{
        
        return 0;
  }
}

void change_motor_direction(void){

  if( window_up == 1 && window_down == 0){

    GPIOA->Data |= (1<<2)
    GPIOA->Data &=~(1<<3)
  }
  if( window_up == 0 && window_down == 1){

    GPIOA->Data |= (1<<3)
    GPIOA->Data &=~(1<<2)
  }
  if( window_up == 0 && window_down == 0){

    GPIOA->Data &=~(1<<2)
    GPIOA->Data &=~(1<<3)
  }
}

//void handler1(void){

  
//  if ( all other tasks aren't running and enable switch is on ){
//     give task1 semaphore
//    }  
    
//   force context switch
//}

//void handler2(void){

  
//  if ( all other tasks aren't running and enable switch is on ){
//     give task2 semaphore
//    }  
    
//   force context switch
//}

//void handler3(void){

  
//  if ( task4 and task5 aren't running ){
//     give task3 semaphore
//    }  
    
//   force context switch
//}

//void handler4(void){

  
//  if ( task3 and task5 aren't running ){
//     give task4 semaphore
//    }  
    
//   force context switch
//}

//void handler5(void){


//   give task5 semaphore
  
    
//   force context switch
//}