#include <DHT.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


const char* ssid = "Zzz";
const char* password = "1234567890";

const char* mqtt_server = "mqtts.heclouds.com";  // OneNet mqtt服务器域名
const int mqtt_port = 1883;   // mqtt端口号

#define product_id  "OcFyEAzH58"
#define device_id  "office_1"
#define token  "version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_1&et=1761720044&method=md5&sign=JFXruNyGm%2BFixVErw7Mmdg%3D%3D"

#define ONENET_TOPIC_PROP_POST "$sys/" product_id "/" device_id "/" "thing/property/post"   // 设备属性上报请求主题
#define ONENET_TOPIC_PROP_SET "$sys/" product_id "/" device_id "/" "thing/property/set"      
#define ONENET_TOPIC_PROP_POST_REPLY "$sys/" product_id "/" device_id "/" "thing/property/post/reply"   // 上报响应
#define ONENET_TOPIC_PROP_SET_REPLY "$sys/" product_id "/" device_id "/" "thing/property/set_reply" 
#define ONENET_TOPIC_PROP_FORMAT "{\"id\":\"%u\",\"version\":\"1.0\",\"params\":%s}"     // 设备属性上报格式模板


int pinDHT = D7;
int pinFire = D6;   // 数字量
int pinExist = D5;
int pinLightSensor = A0;
int pinLight = D8;
int pinBeep = D0;
int pinAirRelay = D1;

float temp = 0.0;
float humi = 0.0;
float co2 = 0.000;
bool fireState = false;
bool lightState = false;
bool existState = false;
bool airConductState = false;
bool fanState = false;
int lightSensorVal = 1000;    // 光敏模块模拟量 0~1000值越高光线越暗
int ticker1count = 0;       // 计数器1
int ticker2count = 0;       // 计数器2
int ticker3count = 0;       // 计数器3
int postMsgId = 0;          // 消息id 需要改变

Ticker ticker1;
Ticker ticker2;
Ticker ticker3;
DHT dht(pinDHT, DHT11);
WiFiClient esp8266Client;
PubSubClient client(esp8266Client);


void ticker1Counter(void);
void ticker2Counter(void);
void ticker3Counter(void);
void Task_Schedule(void);

void DHT11_Init(void);
void getTempHumi(void);
void FireSensor_Init(void);
void getFireSensorState(void);
void LightSensor_Init(void);
void getLightSensor(void);
void HCSR501_Init(void);
void getExistState(void);

void OneNet_Init(void);
void WiFi_Connect(void);
void Keep_Connect(void);
void OneNet_Connect(void);
void OneNet_Prop_Post(void);
void callback(char* topic, byte* payload, unsigned int length);


void setup()
{
  Serial.begin(9600);
  DHT11_Init();
  FireSensor_Init();
  LightSensor_Init();
  HCSR501_Init();
  OneNet_Init();

  ticker1.attach_ms(1, ticker1Counter);   // 1ms加一次
  ticker2.attach_ms(1, ticker2Counter);
  // ticker3.attach_ms(1, ticker3Counter);
  ticker3.attach(10, OneNet_Prop_Post);   // 10s 上报一次
} 

void loop()
{
  Keep_Connect();
  Task_Schedule();
  client.loop();
}

void ticker1Counter(void)
{
  ticker1count ++;
}

void ticker2Counter(void)
{
  ticker2count ++;
}

void ticker3Counter(void)
{
  ticker3count ++;
}

void Task_Schedule(void)
{
  if(ticker1count >= 2000)    // 2s 读取一次dht11
  { 
    getTempHumi();
    getExistState();
    ticker1count = 0;
  }
  if(ticker2count >= 1000)    // 1s 读取一次火焰和亮度
  {
    getFireSensorState();
    getLightSensor();
    ticker2count = 0;
  }
}

void DHT11_Init(void)
{
  dht.begin();
}

void getTempHumi(void)
{
  humi = dht.readHumidity();
  temp = dht.readTemperature();
  bool readSuccess;
  if((isnan(humi) || isnan(temp)))  readSuccess =  false;
  else readSuccess = true;

  if(readSuccess)
  {
    // Serial.print("humi: "); Serial.print(humi);
    // Serial.print("temp: "); Serial.print(temp);
    // Serial.println();
  }
  else
  {
    // Serial.println("无法从DHT传感器读取数据!");
  } 
}

void FireSensor_Init(void)
{
  pinMode(pinFire, INPUT);
}

void getFireSensorState(void)
{
  fireState = digitalRead(pinFire) ? true : false;
  // Serial.print("fireState: ");Serial.println(fireState);
}

void LightSensor_Init(void)
{
  pinMode(pinLightSensor, INPUT);
}

void getLightSensor(void)
{
  lightSensorVal = analogRead(pinLightSensor);
  // Serial.print("lightSensorVal: ");Serial.println(lightSensorVal);
}

void HCSR501_Init(void)
{
  pinMode(pinExist, INPUT);
}

void getExistState(void)
{
  existState = digitalRead(pinExist);
  // Serial.print("existState: ");Serial.println(existState);
}

void device_Init(void)
{
  pinMode(pinBeep, OUTPUT);
  digitalWrite(pinBeep, HIGH);    // 有源蜂鸣器低电平触发
  pinMode(pinLight, OUTPUT);
  digitalWrite(pinLight, LOW);
}

void deviceControl(void)
{
  // 光敏传感器控制led灯
  if(lightSensorVal < 500)
  {
    lightState = true;
    digitalWrite(pinLight, HIGH);  
  }
  else
  {
    lightState = false;
    digitalWrite(pinLight, LOW);
  }
  Serial.print("lightState: ");Serial.println(lightState);
  // 火焰传感器控制蜂鸣器
  if(fireState) digitalWrite(pinBeep, LOW);   // 低电平触发
  else digitalWrite(pinBeep, HIGH);

}

void OneNet_Init(void)
{
  WiFi_Connect();
  OneNet_Connect();
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

void Keep_Connect(void)
{
  if(WiFi.status() != WL_CONNECTED) WiFi_Connect();
  if(!client.connected()) OneNet_Connect(); 
}

#define JSON_PARAMS_FORMAT "{\"temp\":{\"value\":%.1f},\"humi\":{\"value\":%.1f},\"co2\":{\"value\":%.3f},\"airconductor\":{\"value\":%s},\"exist\":{\"value\":%s},\"fan\":{\"value\":%s},\"fire\":{\"value\":%s},\"light\":{\"value\":%s}}"
#define BOOL2STRING(state) state ? "true" : "false"
// 上报设备属性
void OneNet_Prop_Post()
{
  if(client.connected())
  {
    char params[256];   // 属性参数
    char jsonBuf[256];  // json数据包缓冲区
    sprintf(params, JSON_PARAMS_FORMAT, temp, humi, co2, BOOL2STRING(airConductState), BOOL2STRING(existState), BOOL2STRING(fanState), BOOL2STRING(fireState), BOOL2STRING(lightState));

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
    if(params.containsKey("light"))
    {
      lightState = params["light"];
      Serial.print("Set light:");
      Serial.println(lightState);
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
