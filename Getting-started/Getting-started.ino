// User only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins
static const int led_pin = 2;

// Our task: Blink an LED
void toggleLED(void *parameter){
  while(1){
    digitalWrite(led_pin, HIGH);
    vTaskDelay(500/portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(500/portTICK_PERIOD_MS);
    Serial.println("first_timer");
  }
}

// Our task: Blink an LED
void toggleLEDD(void *parameter){
  while(1){
    digitalWrite(led_pin, HIGH);
    vTaskDelay(323/portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(323/portTICK_PERIOD_MS);
    Serial.println("second_timer");
  }
}

void setup() {
  pinMode(led_pin, OUTPUT);
  // Function to be called, name, stack alloc, param to pass
  // Task priority, Task handle
  xTaskCreatePinnedToCore(
    toggleLED,
    "Toggle LED",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);
    // Function to be called, name, stack alloc, param to pass
  // Task priority, Task handle
  xTaskCreatePinnedToCore(
    toggleLEDD,
    "Toggle 2 LED",
    1024,
    NULL,
    1,
    NULL,
    app_cpu);
    Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello world");
  delay(10000);
}
