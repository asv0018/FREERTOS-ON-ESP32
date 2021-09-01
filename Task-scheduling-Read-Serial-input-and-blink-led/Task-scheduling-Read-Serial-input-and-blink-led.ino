#include <stdlib.h> // Needed for atoi()

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buff_len = 20;

// Pins
static const int led_pin = 2;

// Globals
static int led_delay = 500;

// Blink LED at rate set by global variable
void toggleLED(void *parameter){
  while(1){
    digitalWrite(led_pin,HIGH);
    vTaskDelay(led_delay/portTICK_PERIOD_MS);
    digitalWrite(led_pin,LOW);
    vTaskDelay(led_delay/portTICK_PERIOD_MS);
  }
}

void readSerial(void *parameters){
  char c;
  char buf[buff_len];
  uint8_t idx = 0;

  // Clear whole buffer
  memset(buf,0,buff_len);
  while(1){
  // Loop forever
  if(Serial.available()>0){
    c = Serial.read();
    if(c=='\n'){
      led_delay = atoi(buf);
      Serial.print("Updated LED delay to: ");
      Serial.println(led_delay);
      memset(buf, 0, buff_len);
      idx = 0;
    } else {
      if(idx < buff_len - 1){
        buf[idx] = c;
        idx++;
      }
    }
  }
}
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led_pin,OUTPUT);
  // Task to run forever
  xTaskCreatePinnedToCore(
    toggleLED,
    "Toggle LED",
    1024,
    NULL,
    1,
    NULL,
    app_cpu
    );

  // Task to run once with higher priority
  xTaskCreatePinnedToCore(
    readSerial,
    "Task 2",
    1024,
    NULL,
    1,
    NULL,
    app_cpu
    );

    vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
