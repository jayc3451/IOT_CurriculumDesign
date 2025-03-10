#include <ESP8266WiFi.h>
#include <DHT.h>

// 定义温湿度传感器相关引脚 并将DHT类实例化
int pinDHT = D7;
DHT dht(pinDHT, DHT11);
// 定义WiFi模块作为STA连接的AP的标识名和密码
const char *ssid = "Zzz";
const char *password = "1234567890";
// 将WiFiServer类实例化，WiFi模块作为TCP服务器，端口号为80
WiFiServer server(80);
// 定义串口缓冲区、传感器逻辑判断标志等变量
#define BUFFER_SIZE 64    // 宏定义串口接收缓冲区大小
char buffer[BUFFER_SIZE]; // 串口接收缓冲区
int bufferIndex = 0;      // 缓冲区索引
int relayState = 0;       // 继电器状态
int fireFlag = 0;         // 消防状态
int lightFlag = 0;        // 补光灯状态

// 上电初始化配置函数
void setup()
{
    Serial.begin(115200); // 串口初始化 波特率115200
    pinMode(2, OUTPUT);   // 2号引脚配置为输出模式，板载LED灯
    digitalWrite(2, 0);   // 2号引脚拉低，TX有数据后板载LED会闪烁
    dht.begin();          // DHT11初始化
    // WiFi模块作为STA连接到AP 并通过串口打印相关信息
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    // 500ms查询一次连接状态，不成功打点
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // 连接成功 打印WiFi模块由AP分配到的地址
    Serial.println("");
    Serial.println("WiFi connected");
    server.begin();
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}
// loop循环函数
void loop()
{
    // 实例化WiFiClient类，调用server.available()监听是否有客户端连接
    WiFiClient client = server.available();
    // 没有客户端连接，延时2s后返回，重新执行loop()函数
    if (!client)
    {
        delay(2000);
        return;
    }
    // 定义温湿度浮点数变量，调用dht的方法解算传感器温湿度信息
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    // 检查读取是否成功 不成功则直接返回重新执行loop()函数
    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("无法从DHT传感器读取数据!");
        return;
    }
    // 串口打印温湿度数据
    Serial.print("DHT11 ~ TEM-HUM : ");
    Serial.print((int)temperature);
    Serial.print("℃,");
    Serial.print((int)humidity);
    Serial.println("%");
    delay(1000); // DHT11数据更新周期为1s
    Serial.print(temperature);
    Serial.print(",");
    Serial.println(humidity);
    // 如果串口RX有数据，阻塞接收串口数据
    while (Serial.available() > 0)
    {
        char incomingByte = Serial.read(); // 读取一个字节的数据

        // 如果是换行符或回车符，则处理缓冲区中的数据
        if (incomingByte == '\n' || incomingByte == '\r')
        {
            buffer[bufferIndex] = '\0'; // 在字符串末尾添加空字符以构成字符串
            // 根据接收到的数据执行相应的操作
            if (strcmp(buffer, "Relay ON") == 0)
            {
                relayState = 1; // 继电器状态位置1
            }
            else if (strcmp(buffer, "Relay OFF") == 0)
            {
                relayState = 0; // 继电器状态位置0
            }
            else if (strcmp(buffer, "Warning") == 0)
            {
                fireFlag = 1; // 火焰标志位置1
            }
            else if (strcmp(buffer, "Normal") == 0)
            {
                fireFlag = 0; // 火焰标志位置0
            }
            // 重置缓冲区索引以准备接收新的数据
            bufferIndex = 0;
        }
        else // 如果不是换行符或回车符，则将字符添加到缓冲区中
        {
            if (bufferIndex < BUFFER_SIZE - 1) // 确保不会溢出缓冲区
            {
                buffer[bufferIndex++] = incomingByte;
            }
            else // 如果缓冲区已满 重置缓冲区以丢弃旧数据
            {
                bufferIndex = 0;
            }
        }
    }
    // 刷新客户端缓存区 定义HTML页面转换成的字符串
    client.flush();
    String s = "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n\r\n"
               "<!DOCTYPE html>\n"
               "<html lang=\"en\">\n"
               "<head>\n"
               "    <meta charset=\"UTF-8\">\n"
               "    <meta http-equiv=\"Refresh\" content=\"5\">\n"
               "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, minimum-scale=0.5, maximum-scale=2.0, user-scalable=yes\">\n"
               "    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
               "    <title>工厂环境检测系统</title>\n"
               "    <style>\n"
               "        h2, h1 { line-height: 1%; }\n"
               "        body { margin: 0; padding: 0; width: 340px; background: LightCyan; }\n"
               "        .button { width: 100px; height: 100px; text-align: center; font-weight: 100; color: darkcyan; margin: 0 40px 40px 0; position: relative; border: 6px solid darkcyan; background: LightCyan; font-size: 20px; border-radius: 50%; }\n"
               "        .top1 { width: 360px; height: 45px; color: #FFF; border: 1px solid darkcyan; background: darkcyan; font-size: 25px; border-radius: 0%; }\n"
               "    </style>\n"
               "</head>\n"
               "<body>\n"
               "    <button class=\"button top1\">工厂环境检测系统</button>\n"
               "    <center style=\"left: 20px; position: relative;\">\n"
               "        <br/>\n"
               "        <button type=\"button\" class=\"button\" value=\"temp\">温度<span style=\"color: red; font-size: 25px;\">";
    // 拼接温度和湿度数据
    s += ((int)temperature);
    s += "°C</span></button>\n";
    s += "        <button type=\"button\" class=\"button\" value=\"humi\">湿度<span style=\"color: green; font-size: 25px;\">";
    s += ((int)humidity);
    s += "%</span></button>\n";
    // 拼接继电器状态信息
    s += "        <button type=\"button\" class=\"button\" value=\"humi\">继电器<span style=\"color: green; font-size: 25px;\">";
    if (relayState == 1)
    {
        s += "<b>ON</b>";
    }
    else
    {
        s += "<b>OFF</b>";
    }
    s += "</span></button>\n";
    // 拼接继电器状态信息
    if (fireFlag == 1)
    {
        s += "        <button type=\"button\" class=\"button\" value=\"humi\">消防<br><span style=\"color: red; font-size: 25px;\">";
        s += "<b>异常</b>";
    }
    else
    {
        s += "        <button type=\"button\" class=\"button\" value=\"humi\">消防<br><span style=\"color: green; font-size: 25px;\">";
        s += "<b>正常</b>";
    }
    s += "</span></button>\n";
    // 闭合HTML标签
    s += "    </center>\n"
         "</body>\n"
         "</html>";
    // 将HTML字符串发送给客户端
    client.print(s);
    delay(1);
}
