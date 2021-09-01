// Use only core 1 for demo purposes
#define LED_BUILTIN 2

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int num_tasks = 5;

typedef struct Message{
  char body[20];
  uint8_t len;
}Message;

static SemaphoreHandle_t sem_params;
static SemaphoreHandle_t mutex;

void myTask(void *parameters){
  // Copy the struct from parameer to a local variable
  Message msg = *(Message *)parameters;
  // Increment semaphore to indicate that the parameter has been read
  xSemaphoreGive(sem_params);
  if(xSemaphoreTake(mutex,portMAX_DELAY)==pdTRUE){
    // Print out message contents
    Serial.print("Received : ");
    Serial.print(msg.body);
    Serial.print(" | len : ");
    Serial.println(msg.len);
    xSemaphoreGive(mutex);
    // Wait for a while and delete self
    vTaskDelay(1000/portTICK_PERIOD_MS);
    //vTaskDelete(NULL);
  }
}

void setup(){
  char task_name[12];
  Message msg;
  char text[20] = "All your base";
  Serial.begin(115200);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Counting Semophore Demo---");

  // Create semophores, and initialise at zero
  sem_params = xSemaphoreCreateCounting(num_tasks,0);
  mutex = xSemaphoreCreateMutex();
  
  strcpy(msg.body, text); // Create a message to use as a argument common to all tasks
  msg.len = strlen(text);

  for(int i=0;i<num_tasks;i++){
    // Generate unique name string for a task
    sprintf(task_name, "Task %i",i);
    
    xTaskCreatePinnedToCore(
      myTask,
      task_name,
      1024,
      (void *)&msg,
      1,
      NULL,
      app_cpu
      );
  }
  
  // Wait or all tasks to read shared memory
  for(int i=0;i < num_tasks;i++){
    xSemaphoreTake(sem_params, portMAX_DELAY);
  }

  Serial.println("All tasks created");
  
}

void loop(){
  // Do nothing but allow yielding to lower-priority tasks
  vTaskDelay(1000/portTICK_PERIOD_MS);
}
