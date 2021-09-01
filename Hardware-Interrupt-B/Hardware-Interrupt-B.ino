// Hardware interrupt using binary semaphore
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif
#define LED_BUILTIN 2

static const int led_pin = LED_BUILTIN;
static const int adc_pin = A0;

static const uint16_t timer_divider = 80; // clk @ 10mhz
static const uint64_t timer_max_count = 1000000;
static const TickType_t task_delay = 2000/portTICK_PERIOD_MS;


static volatile uint16_t val;
static hw_timer_t *timer = NULL;
static SemaphoreHandle_t bin_sem = NULL;

/*void IRAM_ATTR onTimer(){
  int pin_state = digitalRead(led_pin);
  digitalWrite(led_pin, !pin_state);
}*/

void IRAM_ATTR onTimer(){
  BaseType_t task_woken = pdFALSE;

  // Perform action (read from ADC)
  val = analogRead(adc_pin);

  // Give semaphore to tell task that new value is ready
  xSemaphoreGiveFromISR(bin_sem, &task_woken);
  
  if(task_woken){
    portYIELD_FROM_ISR();
  }
}

void printValues(void *parameters){
  while(1){
    xSemaphoreTake(bin_sem, portMAX_DELAY);
    Serial.println(val);
  }
}

void setup() {
  pinMode(led_pin,OUTPUT);

  Serial.begin(115200);

  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--FreeRTOS ISR Buffer Demo--");

  bin_sem = xSemaphoreCreateBinary();

  if(bin_sem == NULL){
    Serial.println("Could not create semaphore");
    ESP.restart();
  }

  // Start task to print out results (higher priority!)
  xTaskCreatePinnedToCore(
    printValues,
    "Print values",
    1024,
    NULL,
    2,
    NULL,
    app_cpu);
  
  timer = timerBegin(0, timer_divider, true);

  timerAttachInterrupt(timer, &onTimer, true);

  timerAlarmWrite(timer, timer_max_count, true);

  timerAlarmEnable(timer);  
}

void loop() {
  // put your main code here, to run repeatedly:

}
