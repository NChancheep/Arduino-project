#include <driver/i2s.h>
#define I2S_WS 12
#define I2S_SD 13
#define I2S_SCK 14

#define I2S_PORT I2S_NUM_0

esp_err_t err;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup I2S ...");

  delay(1000);
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);
}

void loop() {
  int32_t sample = 0;
  int bytes = i2s_pop_sample(I2S_PORT, (char*)&sample, portMAX_DELAY);
  
  Serial.print(20000000); //Upper bound in Serial Plotter
  Serial.print(" ");
  Serial.print(-20000000); //Lower bound in Serial Plotter
  Serial.print(" ");
  
  if(bytes > 0){
    Serial.println(sample);
  }
}

void i2s_install(){
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000, //16 kHz
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //Use left channel
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // interrupt level 1
    .dma_buf_count = 8, // number of buffers
    .dma_buf_len = 64,   // samples per buffer (minimum is 8)
  };

  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }

  Serial.println("I2S driver installed.");
}

void i2s_setpin(){
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
}
