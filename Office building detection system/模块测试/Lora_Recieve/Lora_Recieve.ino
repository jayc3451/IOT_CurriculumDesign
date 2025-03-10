#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial customSerial(1);

String buffer; // 用于存储字节数组的String

void setup() {
  Serial.begin(115200); // 开启用于调试的串口
  customSerial.begin(9600, SERIAL_8N1, 4, 5);

  Serial.println("ready");
  customSerial.println("ready1");
}

void loop() {
  if (customSerial.available()) {
    int incomingByte = customSerial.read(); // 读取一个字节

    // 检查是否是起始标志0xFF
    if (incomingByte == 0xFF) {
      Serial.println("Start byte detected.");

      // 清空缓冲区
      buffer = "";

      // 继续读取直到遇到结束标志0x0A
      while (customSerial.available() && incomingByte != 0x0A) {
        buffer += (char)incomingByte; // 将读取的字节添加到缓冲区
        incomingByte = customSerial.read();
      }

      // 如果读取到结束标志0x0A，也将其添加到缓冲区
      if (incomingByte == 0x0A) {
        buffer += (char)incomingByte; // 添加结束标志
        customSerial.println("End byte detected.");

        // 处理缓冲区中的数据
        processBuffer(buffer);
      }
    }
  }
}

void processBuffer(const String &buffer) {
  // 这里处理缓冲区中的数据
  Serial.print("Received data: ");
  for (int i = 0; i < buffer.length(); i++) {
    Serial.print(buffer[i], HEX); // 以十六进制形式打印每个字节
    Serial.print(" ");
  }
  Serial.println();
}