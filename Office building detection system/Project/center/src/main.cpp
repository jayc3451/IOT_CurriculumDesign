#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <cJSON.h>
#include <HardwareSerial.h>
#include <Ticker.h>
#include <PubSubClient.h>


String ssid = "Zzz";
String password = "1234567890";
WebServer server(80);

HardwareSerial customSerial(1);
String buffer = ""; 
Ticker ticker1;
Ticker ticker2;
WiFiClient esp32Client;
PubSubClient client(esp32Client);

const char* mqtt_server = "mqtts.heclouds.com";  // OneNet mqtt服务器域名
const int mqtt_port = 1883;   // mqtt端口号
#define product_id  "OcFyEAzH58"
#define device_id  "center"
#define token  "version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Fcenter&et=1762233160&method=md5&sign=Epu9EknM3DSEmUGfxXHQ8g%3D%3D"
#define ONENET_TOPIC_PROP_POST "$sys/" product_id "/" device_id "/" "thing/property/post"   // 设备属性上报请求主题
#define ONENET_TOPIC_PROP_SET "$sys/" product_id "/" device_id "/" "thing/property/set"      
#define ONENET_TOPIC_PROP_POST_REPLY "$sys/" product_id "/" device_id "/" "thing/property/post/reply"   // 上报响应
#define ONENET_TOPIC_PROP_SET_REPLY "$sys/" product_id "/" device_id "/" "thing/property/set_reply" 
#define ONENET_TOPIC_PROP_FORMAT "{\"id\":\"%u\",\"version\":\"1.0\",\"params\":%s}"     // 设备属性上报格式模板
bool accessFlag = false;
int postMsgId;
// format bytes
String formatBytes(size_t bytes)
{
    if (bytes < 1024)
    {
        return String(bytes) + "B";
    }
    else if (bytes < (1024 * 1024))
    {
        return String(bytes / 1024.0) + "KB";
    }
    else if (bytes < (1024 * 1024 * 1024))
    {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    }
    else
    {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

void onServeStaticSubDir(File &dir, String topDir)
{
    File file = dir.openNextFile();
    while (file)
    {
        String path = String(file.path());
        String uri = path.substring(path.indexOf(topDir) + topDir.length(), path.indexOf(".gz"));
        server.serveStatic(uri.c_str(), LittleFS, path.c_str());
        Serial.print("uri: ");
        Serial.println(uri);
        Serial.print("path: ");
        Serial.println(path);
        file = dir.openNextFile();
    }
}

void onServeStatic(String dir)
{
    File wwwDir = LittleFS.open(dir);

    if (wwwDir.isDirectory())
    {
        File file = wwwDir.openNextFile();
        while (file)
        {
            if (file.isDirectory())
            {
                file = LittleFS.open(file.path());
                onServeStaticSubDir(file, dir);
            }
            else
            {
                String path = String(file.path());
                String uri = path.substring(path.indexOf(dir) + dir.length(), path.indexOf(".gz"));
                server.serveStatic(uri.c_str(), LittleFS, path.c_str());
                Serial.print("uri: ");
                Serial.println(uri);
                Serial.print("path: ");
                Serial.println(path);
            }
            file = wwwDir.openNextFile();
        }
    }
}

void Lora_Receive(void)
{
    if (customSerial.available() >= 5) 
    {     
        int index = 0;
        char incomingByte = customSerial.read(); // 读取一个字节
        if (incomingByte == 0xFF)    // 检测到起始字节
        {
            customSerial.println("Start byte detected.");
            while (customSerial.available())
            {
                customSerial.println("have data");
                buffer = customSerial.readStringUntil(0x0A);  // 结束字节
            } 
            buffer += "\0";
            accessFlag = true;
            Serial.println(buffer);
        }
    }
}

#define JSON_PARAMS_FORMAT "{\"access\":{\"value\":\"%s\"}}"
// #define BOOL2STRING(state) state ? "true" : "false"

void OneNet_Prop_Post(void)
{
  if(client.connected())
  {
    char params[256];   // 属性参数
    char jsonBuf[256];  // json数据包缓冲区
    sprintf(params, JSON_PARAMS_FORMAT, buffer);

    // Serial.println(params);
    sprintf(jsonBuf, ONENET_TOPIC_PROP_FORMAT, postMsgId++, params);
    Serial.println(jsonBuf);

    if(client.publish(ONENET_TOPIC_PROP_POST, jsonBuf))
    {
      Serial.println("Post property success!");
    }
    else
    {
      Serial.println("Post property failed~");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived[");
  Serial.print(topic);
  Serial.print("]");
  for(int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);   //打印消息内容
  }
  Serial.println();
}

void OneNet_Connect()
{
  client.setServer(mqtt_server, mqtt_port);
  client.connect(device_id, product_id, token);
  if(client.connected())
  {
    Serial.println("Connected to Onenet");
  }
  else
  {
    Serial.println("Failed to connect to OneNet");
  }
  client.subscribe(ONENET_TOPIC_PROP_SET);    //订阅设备属性设置请求  OneNet --> 设备
  client.subscribe(ONENET_TOPIC_PROP_POST_REPLY);     //订阅设备属性上报响应
  client.setCallback(callback);
}


void sendMessage(void)
{
  if(accessFlag)
  {
    OneNet_Prop_Post();
    accessFlag = false;
  }
}

void setup(void)
{
    Serial.begin(115200);
    customSerial.begin(9600, SERIAL_8N1, 4, 5); // Lora
    customSerial.println("Serial_1 ready");

    Serial.setDebugOutput(true);         // 默认情况下，调用Serial.begin时，将禁用WiFi库的诊断输出。 要再次启用调试输出，请调用 Serial.setDebugOutput(true)
    LittleFS.begin();
    {
        File root = LittleFS.open("/");
        File file = root.openNextFile();
        while (file)
        {
            String fileName = file.name();
            size_t fileSize = file.size();
            Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
            file = root.openNextFile();
        }
        Serial.printf("\n");
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    OneNet_Connect();
    Serial.println();

    // 启动Web服务器
    server.begin();
    Serial.println("HTTP server started");

    server.serveStatic("/", LittleFS, "/www/index.html");
    onServeStatic("/www");

    // use it to load content from LittleFS
    server.onNotFound([]()
                      { server.send(404, "text/plain", "FileNotFound"); });

    ticker1.attach(1, Lora_Receive);
    ticker2.attach(1, sendMessage);
}

void loop(void)
{
    if(!client.connected()) OneNet_Connect(); // 重新连接
    server.handleClient();  // web server
    client.loop();     // mqtt
    delay(2); // allow the cpu to switch to other tasks
}

