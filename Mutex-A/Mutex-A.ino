// Use only core 1 for demo purposes

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static int shared_var = 0;
static SemaphoreHandle_t mutex;

void incTask(void *parameters){
  int local_var;
  while(1){
    // Take mutex prior to critical section
    if(xSemaphoreTake(mutex,0)==pdTRUE){
      local_var = shared_var;
      local_var++;
      vTaskDelay(random(100,500)/portTICK_PERIOD_MS);
      shared_var = local_var;
      
      xSemaphoreGive(mutex);
      
      Serial.println(shared_var);
    } else {
      // Do something else
    }
  }
}

void setup(){
  randomSeed(analogRead(0));
  
  Serial.begin(115200);

  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Race condition Demo---");

  mutex = xSemaphoreCreateMutex();
  
  xTaskCreatePinnedToCore(
    incTask,
    "Increment task 1",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);

  xTaskCreatePinnedToCore(
    incTask,
    "Increment task 2",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);

    xTaskCreatePinnedToCore(
    incTask,
    "Increment task 3",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);

  xTaskCreatePinnedToCore(
    incTask,
    "Increment task 4",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);

  vTaskDelete(NULL);
}

void loop(){
  
}
