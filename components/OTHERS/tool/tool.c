
#include "tool.h"
static const char* TAG ="TEST";

//##############################################################################################
//字节转十六进制字符
//##############################################################################################
void bytes_to_hex(const uint8_t *data, size_t len, char *hex_str, size_t hex_str_len)   {
    if (hex_str_len < len * 3) {                                            // 每个字节需要3个字符（两个十六进制字符和一个空格）
        ESP_LOGE("ERROR", "Hex string buffer too small");
        return;
    }
    for (int i = 0; i < len; i++) {
        sprintf(hex_str + i * 3, "%02X ", data[i]);                          // 格式化为两位十六进制，并添加空格
    }
    hex_str[len * 3 - 1] = '\0';                                             // 去掉最后一个空格，并终止字符串
}
//##############################################################################################
//CRC8速算表生成
//##############################################################################################
void crc8_generate_table(uint8_t poly,uint8_t crc8_table[256]) {                         
    for (int i = 0; i < 256; i++) {
        uint8_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
        crc8_table[i] = crc;
    }
ESP_LOGI(TAG, "CRC8 table generated (poly=0x%02X)", poly);    // 打印前几个表项用于调试
ESP_LOGI(TAG, "Table[0..3]: 0x%02X, 0x%02X, 0x%02X, 0x%02X", crc8_table[0], crc8_table[1], crc8_table[2], crc8_table[3]);}