#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif
#define LED_BUILTIN 2

static const int led_pin = LED_BUILTIN;

static const uint16_t timer_divider = 8; // clk @ 10mhz
static const uint64_t timer_max_count = 1000000;
static const TickType_t task_delay = 2000/portTICK_PERIOD_MS;

static volatile int isr_counter;
static hw_timer_t *timer = NULL;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

/*void IRAM_ATTR onTimer(){
  int pin_state = digitalRead(led_pin);
  digitalWrite(led_pin, !pin_state);
}*/

void IRAM_ATTR onTimer(){
  portENTER_CRITICAL_ISR(&spinlock);
  isr_counter++;
  portEXIT_CRITICAL_ISR(&spinlock);
}

void printValues(void *parameters){
  while(1){
    while(isr_counter > 0){
      Serial.println(isr_counter);
      portENTER_CRITICAL(&spinlock);
      isr_counter--;
      portEXIT_CRITICAL(&spinlock);
    }
    vTaskDelay(task_delay);
  }
}

void setup() {
  pinMode(led_pin,OUTPUT);

  Serial.begin(115200);

  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--FreeRTOS ISR Critical section demo--");

  xTaskCreatePinnedToCore(
    printValues,
    "print values",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);
    
  timer = timerBegin(0, timer_divider, true);

  timerAttachInterrupt(timer, &onTimer, true);

  timerAlarmWrite(timer, timer_max_count, true);

  timerAlarmEnable(timer);

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}
