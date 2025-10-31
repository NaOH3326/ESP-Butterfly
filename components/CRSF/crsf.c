#include "crsf.h"

//##############################################################################################
//变量声明
//##############################################################################################
const char *TAG = "UART_PARSER";
channel_data_t channel_data;
QueueHandle_t uart_queue;
uint8_t crsf_buff[128];
size_t crsf_buff_length =0;
const uint8_t CRC8_table[256] = {
    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
};
//##############################################################################################
//CRC8快速校验
//##############################################################################################
uint8_t crc8_calc_fast(const uint8_t *data, size_t len) {         
    uint8_t crc = 0;
    for (size_t i = 0; i < len; i++) {
        crc = CRC8_table[crc ^ data[i]];
    }
    return crc;
}
//##############################################################################################
//CRSF数据解包
//##############################################################################################
void process_packet(const uint8_t *data, size_t len) {
if (len > 4) { // 最小包长度：sync+len+type+crc8
        ESP_LOGI(TAG, "Packet received, length: %d", len);
    //包结构化
    const uart_packet_t *packet = (const uart_packet_t *)data;
    // 检查同步字节
    if (packet->sync != 0xC8) {
        ESP_LOGE(TAG, "Invalid sync byte: 0x%02X", packet->sync);
        return;}
    // 检查长度字段
    if (packet->len != len - 2) { // -2 因为 sync 和 len 字段不算在 len 中
        ESP_LOGE(TAG, "Length mismatch: expected %d, got %d", packet->len, len - 2);
        return;}
    // 检查类型
    if (packet->type != 0x16) {
        return; }
    // 计算CRC（从type到payload末尾）
    size_t crc_data_len = len - 3;                                              // -3 因为 sync, len, crc8 字段
    uint8_t calculated_crc = crc8_calc_fast(&packet->type, crc_data_len);
    uint8_t received_crc = data[len - 1];                                       // 最后一个字节是CRC8
    if (calculated_crc != received_crc) {
        ESP_LOGE(TAG, "CRC error: calculated 0x%02X, received 0x%02X", calculated_crc, received_crc);
        return;}
    // 处理payload
    size_t payload_len = packet->len - 2;                                        // -2 因为 type 和 crc8 字段
    if (payload_len * 8 < 11) {                                                  // payload至少11位
        ESP_LOGE(TAG, "Payload too short for 11-bit data");
        return; }
    // 计算信道数量
    int num_channels = (payload_len * 8) / 11;
    if (num_channels > 8) num_channels = 8; // 限制通道数量
    ESP_LOGI(TAG, "Processing type 0x14 packet, payload len: %d, channels: %d", payload_len, num_channels);
    // 解析并赋值每个通道
        channel_data.ch1 = ((data[3]>>0) | (data[4]<<8)) & 0x07FF;
        channel_data.ch2 = ((data[4]>>3) | (data[5]<<5)) & 0x07FF;
        channel_data.ch3 = ((data[5]>>6) | (data[6]<<2) | (data[7]<<10)) & 0x07FF;
        channel_data.ch4 = ((data[7]>>1) | (data[8]<<7)) & 0x07FF;
        channel_data.ch5 = ((data[8]>>4) | (data[9]<<4)) & 0x07FF;
        channel_data.ch6 = ((data[9]>>7) | (data[10]<<1)|(data[11]<<10)) & 0x07FF;
        channel_data.ch7 = ((data[11]>>1) | (data[12]<<7)) & 0x07FF;
        channel_data.ch7 = ((data[12]>>4) | (data[13]<<4)) & 0x07FF;
        channel_data.ch8 = ((data[13]>>7) | (data[14]<<1)|(data[15]<<10)) & 0x07FF;
    }
}
//##############################################################################################
//CRSF初始化
//##############################################################################################
void crsf_init(uart_port_t uart_port,int tx_pin ,int rx_pin,const int uart_buffer_size){
uart_config_t uart_config = {
    .baud_rate = 420000 ,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
};
ESP_ERROR_CHECK(uart_param_config(uart_port, &uart_config));
ESP_ERROR_CHECK(uart_set_pin(uart_port, tx_pin, rx_pin, -1, -1));
ESP_ERROR_CHECK(uart_driver_install(uart_port, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
}
//##############################################################################################
//CRSF获取信道数据
//##############################################################################################
void crsf_get_ch(uart_port_t uart_port){

uart_get_buffered_data_len(uart_port, (size_t*)&   crsf_buff_length);
uart_read_bytes(uart_port,crsf_buff,crsf_buff_length,1);
process_packet(crsf_buff,crsf_buff_length);
uart_flush(uart_port);

}
//##############################################################################################