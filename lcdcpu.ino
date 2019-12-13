#include<LiquidCrystal.h>//包含1602液晶头文件
LiquidCrystal lcd(12,11,5,4,3,2);//初始化液晶引脚

//在1602常规字符没有℃，需要自定义
byte temp[8]={
  0b10000,
  0b00110,
  0b01001,
  0b01000,
  0b01000,
  0b01001,
  0b00110,
  0b00000,
};

void setup() {
  // 打开串口
  Serial.begin(9600);
  while(!Serial)
    continue;
  Serial.println("Serial Port Ready");
  //测试串口
  while(Serial.available()==0)
  {
    lcdConnectionTest();
  }
}

char frame[20] = {}; //用于存储AIDA64从串口发来的信息
byte inByte;
int num1,num2;//存储CPU和SSD温度数据


void refreshlcd()
{
  //num1=num1+48;//由于是用LCD1602显示，直接使用采集到数据即可，不用再加回48；
  lcd.setCursor(0,0);
  lcd.print("CPU");
  lcd.setCursor(5,0);
  lcd.print(num1);
  lcd.setCursor(9,0);
  lcd.write(byte(0));//输出℃
  
  //num2=num2+48;
  lcd.setCursor(0,1);
  lcd.print("SSD");
  lcd.setCursor(5,1);
  lcd.print(num2);
  lcd.setCursor(9,1);
  lcd.write(byte(0));//输出℃
}


void lcdConnectionTest()
{
  lcd.begin(16,2);//设置行列值
  lcd.createChar(0,temp);
  lcd.print("System Initial!");
  delay(1000);
  lcd.clear();
}

void loop() {
  //注意：本段代码要求AIDA64设置必须和视频中完全相同才能解析 1.第一个标签以C开头，其后不能再出现字母C  2.标签只能为3个字母  3.取消勾选所有的单位显示
  //没办法，AIDA64发数据的格式我们也改不了...你们有没有更好的办法取代我这个沙雕方法...
  if(Serial.available()>0)
  {
      inByte = Serial.read();
      if(inByte==0x43)  //判断是不是字母C，找到数据帧起点
      { 
        frame[0] = inByte;
        int count = 1;
        for(int i = 1; i < 20; i++)
        {
          while(Serial.available()==0);
          frame[i] = Serial.read();
          count++;  
        }
        Serial.println(" ");
        //根据帧格式提取出四个数值
        num1 = 0;
        for(int i =4; i<8; i++)
        {
          if(frame[i]==0x20) break;
          num1 = num1*10+(frame[i]-48);
        }
        num2 = 0;
        for(int i =14; i<18; i++)
        {
          if(frame[i]==0x20) break;
          num2 = num2*10+(frame[i]-48);
        }

        Serial.println(num1);
        Serial.println(num2);

        refreshlcd();
      }
  }
  
}
