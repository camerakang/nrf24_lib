#ifndef __NRF24_DEVICE__
#define __NRF24_DEVICE__
#include <SPI.h>
#include "RF24.h"

class NRF24Device {
private:
    RF24* radio;
    SPIClass* rf24_spi;
    uint8_t ce_pin;
    uint8_t csn_pin;
    uint8_t mosi_pin;
    uint8_t miso_pin;
    uint8_t sck_pin;

    
    uint8_t tx_address[6];  // 写入地址
    uint8_t rx_address[6];  // 读取地址

public:
    /**
     * @brief NRF24设备构造函数
     * @param ce CE引脚号
     * @param csn CSN引脚号
     * @param mosi MOSI引脚号
     * @param miso MISO引脚号
     * @param sck SCK引脚号
     */
    NRF24Device(uint8_t ce = 2, uint8_t csn = 3, uint8_t mosi = 5, uint8_t miso = 6, uint8_t sck = 4);
    
    /**
     * @brief 析构函数，释放资源
     */
    ~NRF24Device();
    
    /**
     * @brief 初始化NRF24设备
     * @param write_addr 写入地址
     * @param read_addr 读取地址
     * @param is_sender 是否为发送端
     * @return 初始化是否成功
     */
    bool begin(const uint8_t* write_addr, const uint8_t* read_addr, bool is_sender = true);
    
    /**
     * @brief 发送数据并等待接收应答
     * @param data 要发送的数据
     * @param len 数据长度
     * @param recv_data 接收应答数据的缓冲区
     * @return 接收到的应答数据长度
     */
    size_t send(uint8_t* data, int len, uint8_t* recv_data = nullptr);
    
    /**
     * @brief 仅发送数据，不等待应答
     * @param data 要发送的数据
     * @param len 数据长度
     */
    void sendOnly(uint8_t* data, int len);
    
    /**
     * @brief 接收数据并发送应答
     * @param recv_data 接收数据的缓冲区
     * @param ack_data 要发送的应答数据
     * @param ack_len 应答数据长度
     * @return 接收到的数据长度
     */
    size_t receive(uint8_t* recv_data, uint8_t* ack_data = nullptr, uint8_t ack_len = 0);
    
    /**
     * @brief 仅接收数据，不发送应答
     * @param recv_data 接收数据的缓冲区
     * @return 接收到的数据长度
     */
    size_t receiveOnly(uint8_t* recv_data);
    
    /**
     * @brief 更改目标地址并发送数据
     * @param address 新的目标地址
     * @param data 要发送的数据
     * @param len 数据长度
     * @param recv_data 接收应答数据的缓冲区
     * @return 接收到的应答数据长度
     */
    size_t changeAddressAndSend(const uint8_t* address, uint8_t* data, int len, uint8_t* recv_data);

    /**
     * @brief 设置通道
     * @param channel 通道号
     * @param is_sender 是否为发送端
     */
    void setChannel(uint8_t channel, bool is_sender);
};

#endif
