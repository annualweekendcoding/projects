TaskHandle_t taskSlowHandle;

void myDelay(unsigned ms)
{
  if (ms>=portTICK_PERIOD_MS) { 
    vTaskDelay(ms/portTICK_PERIOD_MS);
    ms -= ms/portTICK_PERIOD_MS;
  }
  if (ms>0) delay(ms);
}

void setup() {
  ledSetup();
  Serial.begin(115200);
  wifiSetup();
  otaSetup();
  timeSetup();  
  displaySetup();
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    taskSlow,   /* Task function. */
                    "taskSlow",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &taskSlowHandle,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
}

void loop() {
  ledLoop();
  myDelay(1);
}

void taskSlow(void * pvParameters) {
  for (;;) {
    wifiLoop();
    otaLoop();
    timeLoop();
    sonneLoop();
    displayLoop();
    myDelay(50);
  }  
}
