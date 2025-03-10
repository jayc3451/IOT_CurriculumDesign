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
#define RECORD_MAX_NUM  8       // 保存卡号最大数量
#define ID_MAX_LEN      4       // 保存卡号位数长度
#define NAME_MAX_LEN    12      // 保存名字最大长度

unsigned char read_id[MAX_LEN];  // MAX_LEN为16，数组最大长度
unsigned char zxp[RECORD_MAX_NUM][MAX_LEN] = {{0x04, 0xD9, 0xB7, 0x71}, {0x93, 0x69, 0xE8, 0x1A}, };  // 有记录的卡号 后面改写成结构体仿照键值对

struct Record
{
  unsigned char id[MAX_LEN];
  unsigned char name[NAME_MAX_LEN];
}record[RECORD_MAX_NUM];

/********RFID*********/
RFID rfid(53,49);
void RFID_Init(void);
void RFID_Read(void);
void RFID_Check(void);
void RFID_Saved(void);
/********Lora*********/
void Lora_Init(void);
String string2HexString(const unsigned char* name);
void Lora_SendName(const char name[]);


void setup()
{
  Serial.begin(115200);
  RFID_Init();
  // RFID_Saved();
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
}

void RFID_Init(void)
{
  SPI.begin();
  rfid.init();
}

void RFID_Read(void)
{
  if(rfid.findCard(PICC_REQIDL, read_id) == MI_OK) 
  {
    //防冲突检测,读取卡序列号
    if(rfid.anticoll(read_id) == MI_OK) 
    {
      RFID_Check();
    }
    //选卡（锁定卡片，防止多数读取，去掉本行将连续读卡）
    rfid.selectTag(read_id);
  }
  rfid.halt();  //命令卡片进入休眠状态
}

void RFID_Check(void)
{
  char checkFlag = 0;
  for(int i = 0; i < RECORD_MAX_NUM; i++)
  {
    if(strncmp(read_id, record[i].id, 4) == 0)
    {
      // 门禁验证成功
      checkFlag = 1;
      // Serial.print("Welcome "); Serial.println(record[i].id);
      Lora_SendName(record[i].id);
      break;
    }
  }
  if(!checkFlag)
  {
    Serial.println("Unknow");
    Lora_SendName("Unknow");
  }
}

void RFID_Saved(void)
{
  unsigned char temp_id1[MAX_LEN] = {0x04, 0xD9, 0xB7, 0x71};  
  unsigned char temp_id2[MAX_LEN] = {0x93, 0x69, 0xE8, 0x1A};
   // 使用 memcpy 为 id 数组赋值  使用 strcpy 为 name 数组赋值
  memcpy(record[0].id, temp_id1, MAX_LEN);  
  strcpy(record[0].name, "Zxp");  
  memcpy(record[1].id, temp_id2, MAX_LEN);  
  strcpy(record[1].name, "Jay");  
}

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
  Serial.println(command);  
  Serial1.println(command);
}  
