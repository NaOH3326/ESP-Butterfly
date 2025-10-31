#ifndef _TOOL_H_
#define _TOOL_H_

#include <stdio.h>
#include "esp_log.h"





//将字节转化为十六进制字符，hex_str的长度必须大于等于data的三倍
void bytes_to_hex(const uint8_t *data, size_t len, char *hex_str, size_t hex_str_len) ;
//校验表生成函数，请在主程序开始时生成
void crc8_generate_table(uint8_t poly,uint8_t crc8_table[256]) ;


#endif
