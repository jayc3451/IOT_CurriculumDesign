模块：RC522	开发板：Arduino Mega

接线：
RST	49
MISO	50
MOSI	51
SCK	52
SDA	53


结果说明：
S50类型卡，uchar str[MAX_LEN=16]读取，0000008 04D9B771
卡号长度8位，第9位是卡号长度，低八位为卡号

打印卡号:
Serial.print(str[i], HEX)

04D9B771(白卡） 9369E81A(蓝卡)

存储 结构体


打印 名字+卡号
at发送 名字对应byte
回传