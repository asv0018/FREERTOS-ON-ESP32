#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buffer_length = 255;

// Globals
static char *msg_ptr = NULL;
static volatile uint8_t msg_flag = 0;

// Tasks

// Task: To read message from Serial buffer
void readSerial(void *parameters){
  char c;
  char buff[buffer_length];
  uint8_t idx = 0;

  // Clear the whole buffer;
  memset(buff,0,buffer_length);

  // Loop forever
  while(1){
    // Read characters from serial
    if(Serial.available()>0){
      c = Serial.read();
      // Store received character to buffer if not over buffer limit
      if(idx < buffer_length - 1){
        buff[idx++] = c;
      }
      // Create a message buffer for print task
      if (c == '\n'){
        // Last character in the string is '\n', so we need to replace
        // it with '\0' to make it null-terminated
        buff[idx - 1] = '\0';
        
        if (msg_flag == 0){
          msg_ptr = (char*)pvPortMalloc(idx * sizeof(char));
          // If maloc returns 0,(out of memory), throw an error and reset
          configASSERT(msg_ptr);

          // Copy message
          memcpy(msg_ptr, buff,idx);

          // Notify other task, that message is ready
          msg_flag = 1;
          
        }
        memset(buff,0,buffer_length);
        idx = 0;
      }
    }
  }
}
// Task : To print message whenever flag is set and free buffer
void printMessage(void *parameters){
  while(1){
    if(msg_flag == 1){
      Serial.println(msg_ptr);
      // Give amount of free heap memory
      Serial.print("Free heap size: ");
      Serial.println(xPortGetFreeHeapSize());
      
      // Free buffer, set pointer to null, and clear flag
      vPortFree(msg_ptr);
      msg_ptr = NULL;
      msg_flag = 0;
    }
  }
}


void setup(){
  Serial.begin(115200);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Heap Demo---");
  Serial.println("Enter a string : ");

  xTaskCreatePinnedToCore(
    readSerial,
    "Read Serial",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);

  xTaskCreatePinnedToCore(
    printMessage,
    "Print Message",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);

  // Delete 'Setup and loop' task
  vTaskDelete(NULL);
}

void loop(){
  
}
