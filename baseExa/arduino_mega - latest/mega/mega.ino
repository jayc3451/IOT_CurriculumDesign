// OLED显示屏相关库
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 定义相关引脚
int tempLedPin = 13;        // 温度LED
int humLedPin = 12;         // 湿度LED
int relayPin = 22;          // 继电器LED
int beepPin = 23;           // 有源蜂鸣器
int firePin = A1;           // 火焰传感器(模拟量)
int lightSensorPin = A0;    // 光敏模块(模拟量)
int lightCompentatePin = 2; // 补光LED(PWM)
// 定义相关变量
int temperature = 0;       // 温度
int humidity = 0;          // 湿度
int flameVal = 500;        // 火焰传感器模拟量 值小有火
int lightDuty = 0;         // 补光LED占空比 0~255值越大灯越亮
int lightSensorVal = 1000; // 光敏模块模拟量 0~1000值越高光线越暗
int tempThreshold = 24;    // 温度阈值
int humThreshold = 30;     // 湿度阈值
int posX = 0;              // OLED显示屏初始X坐标
int posY = 32;             // OLED显示屏初始Y坐标

// 实例化Adafruit_SSD1306
// OLED显示屏是128x64，通信采用硬件I2C(Wire)
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

// 上电初始化配置函数
void setup()
{
    // 串口初始化 波特率115200
    Serial.begin(115200);  // 默认串口 直接打印在串口监视器上
    Serial1.begin(115200); // 串口1 与ESP8266全双工通信
    // 温湿度LED、继电器、补光灯、蜂鸣器引脚配置为输出模式
    pinMode(tempLedPin, OUTPUT);
    pinMode(humLedPin, OUTPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(lightCompentatePin, OUTPUT);
    pinMode(beepPin, OUTPUT);
    // 火焰传感器、光敏模块引脚配置为输入模式
    pinMode(firePin, INPUT);
    pinMode(lightSensorPin, INPUT);
    // 初始化引脚电平
    digitalWrite(tempLedPin, LOW);              // 高电平点亮 熄灭
    digitalWrite(humLedPin, LOW);               // 高电平点亮 熄灭
    digitalWrite(relayPin, LOW);                // 继电器高电平导通 关闭
    digitalWrite(beepPin, HIGH);                // 有源蜂鸣器低电平触发 关闭
    analogWrite(lightCompentatePin, lightDuty); // 占空比初值为0 补光灯熄灭

    // 初始化OLED显示屏
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay(); // 清屏

    // 设置文本样式
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    // 菜单 温湿度继电器
    display.setCursor(0, 0);
    display.println("TEMP:");
    display.setCursor(0, 16);
    display.println("HUMI:");
    display.setCursor(0, 32);
    display.println("RELAY:");
    // 显示温湿度继电器初始化信息
    display.setCursor(62, 0);
    display.println(temperature);
    display.setCursor(62, 16);
    display.println(humidity);
    display.setCursor(92, 32);
    display.println("OFF");
    // 温湿度指示灯显示ON/OFF
    display.setCursor(92, 0);
    display.println("OFF");
    display.setCursor(92, 16);
    display.println("OFF");
    // 更新显示内容
    display.display();
    Serial.println("Arduino Mega Ready"); // 默认串口打印
}

void loop()
{
    // 读取光敏模块模拟量 值越大光线越亮
    lightSensorVal = analogRead(lightSensorPin);
    // 数值归一化为补光灯占空比
    if (lightSensorVal >= 600)
    {
        // 光线良好不需要照明
        lightDuty = 0;
        // 串口1发送开灯信息给ESP8266处理
        Serial1.println("Light OFF");
    }
    else if (lightSensorVal <= 300)
    {
        // 光线特别昏暗 补光灯占空比为1
        lightDuty = 255;
        // 串口1发送开灯信息给ESP8266处理
        Serial1.println("Light ON");
    }
    else
    {
        // 光线比较昏暗 补光灯占空比随光线动态调整
        lightDuty = map(lightSensorVal, 800, 300, 0, 255);
        // 串口1发送开灯信息给ESP8266处理
        Serial1.println("Light ON");
    }
    // 根据占空比 PWM调节补光灯亮度
    analogWrite(lightCompentatePin, lightDuty);

    // 读取火焰传感器模拟量 模拟量越小距离火源越近
    flameVal = analogRead(firePin);
    if (flameVal <= 150) // 有火
    {
        // 串口1发送消防报警信息给ESP8266处理
        Serial1.println("Warning");
        // 有源蜂鸣器报警(低电平触发)
        digitalWrite(beepPin, LOW);
        // 显示屏显示报警信息
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 48);
        display.println("WARNING!");
        display.display();
    }
    else
    {
        // 串口1发送消防正常信息给ESP8266处理
        Serial1.println("Normal");
        // 关闭有源蜂鸣器
        digitalWrite(beepPin, HIGH);
        // 显示器隐去报警信息
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(0, 48);
        display.println("WARNING!");
        display.display();
    }

    // 串口1与ESP8266通信
    // 如果串口1接收缓冲区有信息
    if (Serial1.available() > 0)
    {
        // 把串口1接收缓冲区开头直到换行符 存储到字符串data里
        String data = Serial1.readStringUntil('\n');
        // 由于发送的温湿度数据格式是以“,”分隔开的
        // 获取“,”的下标方便做处理
        int commaIndex = data.indexOf(',');
        // 定义温湿度局部变量
        int temperature1 = 0;
        int humidity1 = 0;
        // “,”的下标大于0 说明传输正确
        if (commaIndex > 0)
        {
            // 将逗号前后的字符串转换成整型数据分别存储到温湿度局部变量里
            temperature1 = data.substring(0, commaIndex).toInt();
            humidity1 = data.substring(commaIndex + 1).toInt();
        }
        // 温湿度局部变量不等于0 说明有读取到数据
        if (temperature1 != 0 && humidity1 != 0)
        {
            // 如果温度局部变量和全局变量不一致，说明温度有更新
            // 有更新则刷新OLED上的相关数据 没有则保持不变 以免出现频闪
            if (temperature != temperature1)
            {
                // OLED隐藏显示温度信息
                display.setTextColor(SSD1306_BLACK);
                display.setCursor(62, 0);
                display.println(temperature);
                display.display();
                // 将温度全局变量更新
                temperature = temperature1;
            }
            // 如果湿度局部变量和全局变量不一致，说明湿度有更新
            if (humidity != humidity1)
            {
                // OLED隐藏显示湿度信息
                display.setTextColor(SSD1306_BLACK);
                display.setCursor(62, 16);
                display.println(humidity);
                display.display();
                // 将湿度全局变量更新
                humidity = humidity1;
            }
        }
        // 刷新显示OLED上给温湿度信息
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(62, 0);
        display.println(temperature);
        display.setCursor(62, 16);
        display.println(humidity);
        display.display();
        // 默认串口打印温湿度数据到串口监视器
        Serial.print("Temperature: ");
        Serial.println(temperature);
        Serial.print("Humidity: ");
        Serial.println(humidity);
        // 温度大于等于阈值
        if (temperature >= tempThreshold)
        {
            // 点亮温度LED指示灯 默认串口打印信息
            digitalWrite(tempLedPin, HIGH);
            Serial.println("Temperature LED ON");
            // 打开继电器 串口1发送继电器状态给ESP8266处理
            digitalWrite(relayPin, HIGH);
            Serial1.println("Relay ON");
            // OLED显示 继电器信息为 ON
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(92, 32);
            display.println("OFF");
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(104, 32);
            display.println("ON");
            display.display();
            // OLED显示 温度LED指示灯信息为 ON
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(92, 0);
            display.println("OFF");
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(104, 0);
            display.println("ON");
            display.display();
        }
        // 温度小于阈值
        else
        {
            // 熄灭温度LED指示灯 默认串口打印信息
            digitalWrite(tempLedPin, LOW);
            Serial.println("Temperature LED OFF");
            // 关闭继电器 串口1发送继电器状态给ESP8266处理
            digitalWrite(relayPin, LOW);
            Serial1.println("Relay OFF");
            // OLED显示 继电器信息为 OFF
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(104, 32);
            display.println("ON");
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(92, 32);
            display.println("OFF");
            display.display();

            // OLED显示 温度LED指示灯信息为 OFF
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(104, 0);
            display.println("ON");
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(92, 0);
            display.println("OFF");
            display.display();
        }
        // 湿度大于等于阈值
        if (humidity >= humThreshold)
        {
            // 点亮湿度LED指示灯 默认串口打印信息
            digitalWrite(humLedPin, HIGH);
            Serial.println("Humidity LED ON");
            // OLED显示 湿度LED指示灯信息为 ON
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(92, 16);
            display.println("OFF");
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(104, 16);
            display.println("ON");
            display.display();
        }
        // 湿度小于阈值
        else
        {
            // 熄灭湿度LED指示灯 默认串口打印信息
            digitalWrite(humLedPin, LOW);
            Serial.println("Humidity LED OFF");
            // OLED显示 湿度LED指示灯信息为 OFF
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(98, 16);
            display.println("ON");
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(104, 16);
            display.println("OFF");
            display.display();
        }
    }
}