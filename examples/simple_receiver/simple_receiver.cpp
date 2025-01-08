#include <Arduino.h>
#include "NRF24_device.h"

// 定义引脚
#define CE_PIN   2
#define CSN_PIN  3
#define MOSI_PIN 5
#define MISO_PIN 6
#define SCK_PIN  4

NRF24Device radio(CE_PIN, CSN_PIN, MOSI_PIN, MISO_PIN, SCK_PIN);

void setup() {
    Serial.begin(115200);
    
    uint8_t write_addr[] = {0x06, 0x07, 0x08, 0x09, 0x0A};
    uint8_t read_addr[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    if (!radio.begin(write_addr, read_addr, false)) {
        Serial.println("无线模块初始化失败！");
        while (1) { delay(1000); }
    }
    
    Serial.println("接收端初始化成功！");
}

void loop() {
    uint8_t recv_buffer[32];
    uint8_t ack_data[] = "Received!";
    
    size_t recv_len = radio.receive(recv_buffer, ack_data, sizeof(ack_data));
    
    if (recv_len > 0) {
        Serial.print("收到数据：");
        Serial.write(recv_buffer, recv_len);
        Serial.println();
    }
} 