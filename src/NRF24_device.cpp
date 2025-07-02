#include "NRF24_device.h"
#include <string.h> // 为memcpy添加

NRF24Device::NRF24Device(uint8_t ce, uint8_t csn, uint8_t mosi, uint8_t miso, uint8_t sck)
{
    ce_pin = ce;
    csn_pin = csn;
    mosi_pin = mosi;
    miso_pin = miso;
    sck_pin = sck;

#if defined(ESP32)
    rf24_spi = new SPIClass(HSPI);
#else

    rf24_spi = new SPIClass(mosi_pin, miso_pin, sck_pin);
#endif
    radio = new RF24(ce_pin, csn_pin);
}

NRF24Device::~NRF24Device()
{
    delete radio;
    delete rf24_spi;
}

bool NRF24Device::begin(const uint8_t *write_addr, const uint8_t *read_addr, bool is_sender)
{
#if defined(ESP32)
    rf24_spi->begin(sck_pin, miso_pin, mosi_pin, csn_pin);
#else
    rf24_spi->begin();
#endif

    if (!radio->begin(rf24_spi, ce_pin, csn_pin))
    {
        return false;
    }

    // 保存地址
    memcpy(tx_address, write_addr, 6);
    memcpy(rx_address, read_addr, 6);

    radio->setPALevel(RF24_PA_LOW);
    radio->enableDynamicPayloads();
    radio->enableAckPayload();
    radio->setPayloadSize(32);
    radio->setDataRate(RF24_1MBPS);

    if (is_sender)
    {
        radio->openWritingPipe(tx_address);
        radio->openReadingPipe(1, rx_address);
        radio->stopListening();
        radio->setRetries(1, 1);
    }
    else
    {
        radio->setAutoAck(true);
        radio->openWritingPipe(tx_address);
        radio->openReadingPipe(1, rx_address);
        radio->startListening();
        
    }

    return true;
}

size_t NRF24Device::send(uint8_t *send_buffer, int send_len, uint8_t *recv_buffer)
{
    radio->flush_tx();
    radio->flush_rx();

    bool report = radio->write(send_buffer, send_len);

    if (report)
    {
        uint8_t pipe;
        if (radio->available(&pipe))
        {
            size_t bytes = radio->getDynamicPayloadSize();
            radio->read(recv_buffer, bytes);
            return bytes;
        }
    }
    return 0;
}

size_t NRF24Device::changeAddressAndSend(const uint8_t *address, uint8_t *send_buffer, int send_len, uint8_t *recv_buffer)
{
    radio->openWritingPipe(address);
    size_t recv_len = send(send_buffer, send_len, recv_buffer);
    return recv_len;
}

void NRF24Device::sendOnly(uint8_t *send_buffer, int send_len)
{
    radio->setAutoAck(false);
    radio->writeFast(send_buffer, send_len);
}

size_t NRF24Device::receive(uint8_t *recv_buffer, uint8_t *send_buffer, uint8_t send_len)
{
    uint8_t pipe;
    if (radio->available(&pipe))
    {
        size_t bytes = radio->getDynamicPayloadSize();
        radio->read(recv_buffer, bytes);

        radio->writeAckPayload(1, send_buffer, send_len);

        return bytes;
    }
    return 0;
}

size_t NRF24Device::receiveOnly(uint8_t *recv_buffer)
{
    uint8_t pipe;
    if (radio->available(&pipe))
    {
        uint8_t bytes = radio->getDynamicPayloadSize();
        radio->read(recv_buffer, bytes);
        return bytes;
    }
    return 0;
}

void NRF24Device::setChannel(uint8_t channel, bool is_sender)
{
    radio->stopListening();
    delay(10);
    radio->setChannel(channel * 8);
    delay(10);
    if (is_sender)
    {
        radio->stopListening();
    }
    else
    {
        radio->startListening();
    }
    radio->printPrettyDetails();
}

bool NRF24Device::switchAddress(const uint8_t* write_addr, const uint8_t* read_addr, bool is_sender)
{
    // 停止监听
    radio->stopListening();
    delay(5);
    
    // 清空缓冲区
    radio->flush_rx();
    radio->flush_tx();
    
    // 保存新地址
    memcpy(tx_address, write_addr, 6);
    memcpy(rx_address, read_addr, 6);
    
    // 重新配置地址
    if (is_sender)
    {
        radio->openWritingPipe(tx_address);
        radio->openReadingPipe(1, rx_address);
        radio->stopListening();
        radio->setRetries(1, 1);
    }
    else
    {
        radio->setAutoAck(true);
        radio->openWritingPipe(tx_address);
        radio->openReadingPipe(1, rx_address);
        radio->startListening();
    }
    
    // 打印新的配置
    radio->printPrettyDetails();
    
    return true;
}

void NRF24Device::flushBuffers()
{
    radio->flush_rx();
    radio->flush_tx();
}

void NRF24Device::powerDown()
{
    radio->powerDown();
}

void NRF24Device::powerUp()
{
    radio->powerUp();
    delay(5); // 给一些时间让模块启动
}

bool NRF24Device::reset(bool is_sender)
{
    // 停止监听
    radio->stopListening();
    delay(5);
    
    // 清空缓冲区
    flushBuffers();
    
    // 重新配置无线参数
    radio->setPALevel(RF24_PA_LOW);
    radio->enableDynamicPayloads();
    radio->enableAckPayload();
    radio->setPayloadSize(32);
    radio->setDataRate(RF24_1MBPS);
    
    // 根据模式重新配置
    if (is_sender)
    {
        radio->openWritingPipe(tx_address);
        radio->openReadingPipe(1, rx_address);
        radio->stopListening();
        radio->setRetries(1, 1);
    }
    else
    {
        radio->setAutoAck(true);
        radio->openWritingPipe(tx_address);
        radio->openReadingPipe(1, rx_address);
        radio->startListening();
    }
    
    // 打印新的配置
    radio->printPrettyDetails();
    
    return true;
}