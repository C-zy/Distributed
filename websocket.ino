#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* name  = "DESKTOP-UIGNCP3 1294";
const char* password  = "123123123";

//服务器地址
String remoteHost = "192.168.1.4";
// 初始化WebSocketsClient对象
WebSocketsClient wsClient;

#define timeSeconds 2

// PIR OUTPUT端口 D6 GPIO12 LED端口 D4 GPIO2
const int LED = 2;
const int PIR_sensor = 12;

// 获取当前时间
unsigned long now = millis();
// 上一次触发中断的时间
unsigned long lastTrigger = 0;
// 是否开启计时器的标志
boolean startTimer = false;
//  定时时间周期 2s
unsigned long detectDuration = 1000 * timeSeconds;

// 检测人体感应的中断触发函数，将LED置为高电平，且给开启计时器的标志startTimer置为true, 表示开始计时，给lastTrigger赋值触发中断的时间
ICACHE_RAM_ATTR void detectHuman() {
  Serial.println("Human Detected!!!");
  digitalWrite(LED, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup(void)
{
  Serial.begin(115200);  // 启动串口通讯，波特率设置为115200

  Serial.println("开始连接");

  WiFi.begin(name , password);
  Serial.print("正在连接到");
  Serial.print(name );

  while (WiFi.status() != WL_CONNECTED) //判定网络状态
  {
    delay(500);
    Serial.print("无线模式成功开启，网络连接成功");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("无线IP地址为: ");
    Serial.println(WiFi.localIP());

    // 将人体感应数据口连接的IO口设置为输入上拉模式，使引脚即使在未连接外部组件的时候也能保持确定的逻辑电平。
    pinMode(PIR_sensor, INPUT_PULLUP);
    // 将人体感应数据口连接的IO口设置为中断模式, 赋值中断函数和将中断模式设置为RISING（当引脚由低电平变为高电平时，触发中断）
    attachInterrupt(digitalPinToInterrupt(PIR_sensor), detectHuman, RISING);

    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    wsClient.begin(remoteHost, 8085);
    // 指定事件处理函数
    wsClient.onEvent([](WStype_t type, uint8_t * payload, size_t length) {

      if (type == WStype_TEXT) {
        // 接收来自服务端的信息（服务端FLASH按键状态），并控制LED的工作
        String data = (char*)payload;
        Serial.print("接收来自服务端的信息 " + data);
      }
    });
  }
}
void loop() {
  wsClient.loop();
  // 获取当前时间
  now = millis();
  // 如果有人灯灭，向服务器发送信息
  if (startTimer && (now - lastTrigger > detectDuration)) {
    Serial.println("Human detected stopped...");
    digitalWrite(LED, LOW);
    startTimer = false;
    wsClient.sendTXT("2");
  }
}
