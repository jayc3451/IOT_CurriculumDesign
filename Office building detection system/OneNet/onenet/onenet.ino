#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <ArduinoJson.h>

const char* ssid = "Zzz";
const char* password = "1234567890";

const char* mqtt_server = "mqtts.heclouds.com";  // mqtt server address
const int mqtt_port = 1883;

#define product_id  "OcFyEAzH58"
#define device_id  "office_1"
#define token  "version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_1&et=1761720044&method=md5&sign=JFXruNyGm%2BFixVErw7Mmdg%3D%3D"

#define ONENET_TOPIC_PROP_POST "$sys/" product_id "/" device_id "/" "thing/property/post"   // 设备属性上报请求主题
#define ONENET_TOPIC_PROP_SET "$sys/" product_id "/" device_id "/" "thing/property/set"      
#define ONENET_TOPIC_PROP_POST_REPLY "$sys/" product_id "/" device_id "/" "thing/property/post/reply"   // 上报响应
#define ONENET_TOPIC_PROP_SET_REPLY "$sys/" product_id "/" device_id "/" "thing/property/set_reply" 
#define ONENET_TOPIC_PROP_FORMAT "{\"id\":\"%u\",\"version\":\"1.0\",\"params\":%s}"     // 设备属性格式模板


int postMsgId = 0;  // 消息id需要改变
float temp = 26.0;
float humi = 50;
bool LED_Status = 0;

WiFiClient esp8266Client;
PubSubClient client(esp8266Client);
Ticker ticker;

void WiFi_Connect(void);
void OneNet_Connect(void);
void OneNet_Prop_Post(void);
void callback(char* topic, byte* payload, unsigned int length);

void setup()
{
  Serial.begin(115200);
  WiFi_Connect();
  OneNet_Connect();
  ticker.attach(10, OneNet_Prop_Post);
}

void loop()
{
  while(WiFi.status() != WL_CONNECTED)
  {
    WiFi_Connect();
  }
  if(!client.connected())
  {
    OneNet_Connect(); 
  }
  client.loop();
}

void WiFi_Connect()
{
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi Network");
  Serial.println(WiFi.localIP());
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
// 上报设备属性
void OneNet_Prop_Post()
{
  if(client.connected())
  {
    char params[256];   // 属性参数
    char jsonBuf[256];  // json数据包缓冲区
    sprintf(params, "{\"temp\":{\"value\":%.1f},\"humi\":{\"value\":%.1f},\"led\":{\"value\":%s}}", temp, humi, LED_Status ? "true" : "false");
    Serial.println(params);
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
// 当订阅的主题有消息时，会调用此函数
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
  if(strcmp(topic, ONENET_TOPIC_PROP_SET)==0)
  {
    DynamicJsonDocument doc(100); //创建json文档用于解析消息内容
    
    DeserializationError error = deserializeJson(doc, payload);   // 解析消息内容
    if(error)
    {
      Serial.print(F("deserializeJson() failed"));
      Serial.println(error.c_str());
      return;
    }
    JsonObject setAlinkMsgObj = doc.as<JsonObject>();   // 创建json文档的根对象
    JsonObject params = setAlinkMsgObj["params"];   // 获取params对象
    if(params.containsKey("led"))
    {
      LED_Status = params["led"];
      Serial.print("Set LED:");
      Serial.println(LED_Status);
    }
    if(params.containsKey("temp"))
    {
      temp = params["temp"];
      Serial.print("Set temp:");
      Serial.println(temp);
    }
    if(params.containsKey("humi"))
    {
      humi = params["humi"];
      Serial.print("Set humi:");
      Serial.println(humi);
    }
    serializeJsonPretty(setAlinkMsgObj, Serial);    // 打印json文档内容
    String str = setAlinkMsgObj["id"];    // 获取消息id
    char SendBuf[100];    // 响应消息缓冲区
    sprintf(SendBuf, "{\"id\":\"%s\",\"code\":200,\"msg\":\"success\"}", str.c_str());    // 构造响应消息
    Serial.println(SendBuf);
    delay(100);
    if(client.publish(ONENET_TOPIC_PROP_SET_REPLY, SendBuf))    // 发布响应消息
    {
      Serial.println("Send set reply success!");
    }
    else
    {
      Serial.println("Send set reply failed~");
    }
  }
}

