// Use only core 1 for demo purposes

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Some string to print
char msg[] = "SHREEVALLABHA PRABHAKARA ALAVAKONDA";
// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

int my_delay = 1000;


// Tasks

// Task: Print to serial terminal with lower priority
void startTask1(void *parameter){
  int msg_len = strlen(msg);
  while(1){
    for(int i=0;i<msg_len;i++){
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}


void startTask2(void *parameter){
  while(1){
    // Get input from the user, through serial console
    Serial.print("*");
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(300);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("----FREERTOS TASK DEMO----");
  // Print self priority
  Serial.println("Setup and loop task running on core");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));

  // Task to run forever
  xTaskCreatePinnedToCore(
    startTask1,
    "Task1",
    1024,
    NULL,
    1,
    &task_1,
    app_cpu
    );

  // Task to run once with higher priority
  xTaskCreatePinnedToCore(
    startTask2,
    "Task 2",
    1024,
    NULL,
    2,
    &task_2,
    app_cpu
    );
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<3;i++){
    vTaskSuspend(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }
  
  // Delete the lower priority task
  if(task_1!=NULL){
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}
