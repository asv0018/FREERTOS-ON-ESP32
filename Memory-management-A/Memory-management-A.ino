#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void testTask(void *parameter){
  while(1){
    int a = 1;
    int b[100];
    for (int i=0;i<100;i++){
      b[i] = a+1;
    }
    Serial.println(b[0]);

    // Print out remaining stack memory (words)
    Serial.print("High water mark: ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

    // Print out number of free heap memory bytes before malloc
    Serial.print("Heap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    int *ptr = (int*)pvPortMalloc(1024*sizeof(int));

    if (ptr==NULL){
      Serial.println("Not enough heap broh!");
    }else{
      for(int i=0;i<1024;i++){
        ptr[i] = 3;
      }
    }

    // Free up our allocated memory
    

    // Print out number of free heap memory bytes after malloc
    Serial.print("Heap after malloc :");
    Serial.println(xPortGetFreeHeapSize());
    // Wait for a while
    vPortFree(ptr);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Wait a moment to start (so we don't miss serial output)
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Memory Demo---");
  xTaskCreatePinnedToCore(
    testTask,
    "Test tank",
    1500,
    NULL,
    1,
    NULL,
    app_cpu);
    vTaskDelete(NULL);
}

void loop() {
  
}
