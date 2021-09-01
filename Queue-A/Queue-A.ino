#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t msg_queue_len = 5;

// Globals
static QueueHandle_t msg_queue;

void printMessages(void *parameters){
  int item;
  while(1){
    // See if there is a message in the queue (donot block)
    if(xQueueReceive(msg_queue, (void *)&item, 0)==pdTRUE){
      //Serial.println(item);
    }
    Serial.println(item);
    // Wait before trying again
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void setup(){
  // Configure Serial
  Serial.begin(115200);

  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Queue Demo---");

  // Create queue
  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));

  xTaskCreatePinnedToCore(
    printMessages,
    "Print messages",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);
}

void loop(){
  static int num = 0;
  if (xQueueSend(msg_queue, (void *)&num, 10)!=pdTRUE){
    Serial.println("Queue Full");
  }
  num++;
  vTaskDelay(1000/portTICK_PERIOD_MS);
}
