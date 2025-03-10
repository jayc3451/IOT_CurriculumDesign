#include <SPI.h>
#include <RFID.h>
#include <stdio.h>
#include <string.h>

/*
  RFID-MC522接线-Arduino Mega
  RST	            49
  MISO	          50
  MOSI	          51
  SCK	            52
  SDA 	          53
*/

// 定义变量
#define RECORD_MAX_NUM 8
#define MAX_NUM 8
unsigned char str[MAX_LEN];  //MAX_LEN为16，数组最大长度
unsigned char zxp[RECORD_MAX_NUM][MAX_LEN] = {{0x04, 0xD9, 0xB7, 0x71}, {0x93, 0x69, 0xE8, 0x1A}, };  // 有记录的卡号 后面改写成结构体仿照键值对

typedef struct Record
{
  unsigned char record[MAX_LEN];
  unsigned char name[12];
}RECORD[RECORD_MAX_NUM];

void Lora_Init(void)
{
  Serial1.begin(115200);
  Serial1.println("AT+NCONFIG=D2D,1122,10,0,20,2");
  delay(500);
  Serial1.println("AT+SETKEY=ADD,18025573826180255738261802557382,C48A5FAE");
  delay(500);
}

String string2HexString(const unsigned char* name)
{
  String hexStr = "";
  while (*name) 
  {  
    // 将每个字符转换为两位十六进制
    char byteHex[3] = {0};  
    sprintf(byteHex, "%02X", (unsigned char)*name);  
    hexStr += byteHex;  
    name++;  
  }  
  return hexStr;  
}

void Lora_SendName(const char name[]) 
{  
  String hexStr = string2HexString(name);
  // 构建AT命令的基础部分  
  String command = "AT+SEND=";  
    
  command += hexStr.length()/2;
  command += ",";
    
  command += hexStr + ",3344,1,0,0";
    
  // 通过串口发送命令  
  Serial1.println(command);  
}  


// RC522 引脚 D53 - 读卡器CS引脚、D49 - 读卡器RST引脚
RFID rfid(53,49);

void RFID_Init(void)
{
  SPI.begin();
  rfid.init();
}

void RFID_Read(void)
{
  if(rfid.findCard(PICC_REQIDL, str) == MI_OK) 
  {
    //防冲突检测,读取卡序列号
    if(rfid.anticoll(str) == MI_OK) 
    {
      RFID_Check();
    }
    //选卡（锁定卡片，防止多数读取，去掉本行将连续读卡）
    rfid.selectTag(str);
  }
  rfid.halt();  //命令卡片进入休眠状态
}

void RFID_Check(void)
{
  char checkFlag = 0;
  for(int i = 0; i < RECORD_MAX_NUM; i++)
  {
    if(strncmp(str, zxp[i], 4) == 0)
    {
      // 门禁验证成功
      checkFlag = 1;
      Serial.println("Welcome Zxp");
      Lora_SendName("zxp");
      break;
    }
  }
  if(!checkFlag)
  {
    Serial.println("Unknow");
    Lora_SendName("Unknow");
  }
}

void setup()
{
  Serial.begin(115200);
  RFID_Init();
  Lora_Init();
}

void loop()
{
  RFID_Read();

  if (Serial1.available() > 0)
  {
      // 把串口1接收缓冲区开头直到换行符 存储到字符串data里
      String data = Serial1.readStringUntil('\n');
      // 由于发送的温湿度数据格式是以“,”分隔开的
      // 获取“,”的下标方便做处理
      int commaIndex = data.indexOf(',');
      Serial.println(data);
  }
  // Serial1.println("AT+SEND=6,018025573826,3344,1,0,0");
  // delay(2000);
}

