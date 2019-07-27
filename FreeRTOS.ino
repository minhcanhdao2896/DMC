#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <Servo.h>
#include <Arduino_FreeRTOS.h>
#include <BH1750.h>
#include <SPI.h>
#include <RFID.h>
#include <Wire.h> 
#include <ArduinoJson.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <SoftwareSerial.h>

#define SS_PIN 53
#define RST_PIN 24
RFID rfid(SS_PIN, RST_PIN);
Servo myservo;
Servo myservo1;
BH1750 lightMeter;

SoftwareSerial mySerial = SoftwareSerial(19, 18); //RX,TX
SoftwareSerial sim = SoftwareSerial(17, 16); //RX,TX

typedef struct
{
  int ucTemp;
  int ucHump;
  int ucCD;
  int ucGAS;
  uint16_t ucLux;
  int ucCT;
  int status_Q;
  int status_D;
  int status_CC;
  int status_CS;
  int status_B;
  int status_PS; 
  int ucFire; 
} xData;
QueueHandle_t xQueue  = xQueueCreate(1, sizeof(xData));
QueueHandle_t xQueue1 = xQueueCreate(1, sizeof(xData));
QueueHandle_t xQueue2 = xQueueCreate(1, sizeof(xData));
SemaphoreHandle_t xBinarySemaphore;
LiquidCrystal_I2C lcd(0x27,20,4);
const int CTPIN = 26;
const int CDPIN = 7;
const int DHTPIN = 4;//DHT11
const int DHTTYPE = DHT11;
const int RELAY_Q = 9;
const int RELAY_D = 10;
const int RELAY_B = 11;
const int RELAY_PS = 12;
const int FIRE = 48;
const int LED_HUMP = 39;
const int LED_TEMP = 33; //29
const int LED_CD = 43;
//const int LED_GAS = 43;  //27
const int LED_CHAY = 49;
const int Buzzer = 22;
unsigned char reading_card[5]; // Mảng đọc mã card
unsigned char master[5] = { 154, 179, 44, 131, 134 }; // Mã Card phù hợp để mở cửa
unsigned char slave[5] = { 177, 183, 230, 49, 209 }; // Mã Card phù hợp để đóng cửa
unsigned char master1[5] = { 67, 210, 79, 131, 93 }; // Mã Card phù hợp để test chống trộm
unsigned char i, j, l;
unsigned long timenow;
volatile int h=0;               
volatile boolean zero_cross=0;  
int AC_pin =5;                
int dim = 50;                   
bool flag_high=false;
bool flag_mid=false;
bool flag_low=false;
int freqStep = 75; 
int CT = 0;  // BIẾN TRẠNG THÁI ĐỂ SEND SMS
int TTG = 0;
int TTT = 0;
int TTT1= 0;
int TTH = 0;
int TTC = 0;
int TTCT = 0;
int BCT = 0; // BIẾN BẬT TẮT CHẾ ĐỘ CHỐNG TRỘM SEND LÊN WEB
int temp1 = 35;
int temp2 = 23;
int hump  = 93;
int gas   = 220;
char chuoi[100];  // BIẾN SEND ESP
String phoneNumber = "+84964921207"; //-> change with your number
char textMessage[100];
//int length_t = 0;
char DataReceiver[100];  // ĐỌC DỮ LIỆU NHẬN TỪ ESP VÀ MODULE SIM
char DataReceiver1[100];
char DataReceiver2[100];
int count = 0;
int statusControl = 0; // ĐIỀU KHIỂN CHẾ ĐỘ TỰ ĐỘNG OR BẰNG TAY
int statusSafe = 0; // KÍCH HOẠT CHẾ ĐỘ CHỐNG TRỘM
int statusSMS = 0;  // BIẾN GỬI SMS ĐẾN NGƯỜI QUẢN LÝ

DHT dht(DHTPIN, DHTTYPE);
byte degree[8] = {0B01110,0B01010,0B01110,0B00000,0B00000,0B00000,0B00000,0B00000};
//void baoDongISR();
void xulyduLieu(void *pvParameters );
void vData( void *pvParameters );
//void baoDong(void *pvParameters);
void hienThi( void *pvParameters );
void vSendSerial(void *pvParameters );
void setup() {
  delay(7000);
  Serial.begin(57600);
  Serial1.begin(57600);
  Serial2.begin(57600);
  mySerial.begin(57600);
  sim.begin(57600);
  delay(1000);
  SetupSIM();
  Wire.begin();
  lightMeter.begin();
  SPI.begin();
  rfid.init();
  myservo.attach(2);
  myservo.write(180);
  delay(50);
  myservo1.attach(6);
  myservo1.write(0);
  delay(50);
 
//-----------------------------------------
  
  pinMode(Buzzer,OUTPUT);
  digitalWrite(Buzzer, LOW);
  while (!Serial) {}
  pinMode(A0, INPUT_PULLUP); 
//  attachInterrupt(0, baoDongISR, FALLING);//Ngat 0 noi chan digital so 2, goi ham baoDongISR khi co su kien ngat, kich hoat khi trang thai chan digital chuyen tu muc cao ve thap
  pinMode(CDPIN, INPUT_PULLUP);  // PIR
  pinMode(CTPIN, INPUT_PULLUP);  // HN02
  pinMode(FIRE,INPUT_PULLUP); 
  pinMode(RELAY_Q,OUTPUT); // RELAY DIEU KHIEN QUAT
  pinMode(RELAY_D,OUTPUT); // RELAY DIEU KHIEN DEN
  pinMode(RELAY_B,OUTPUT); // RELAY DIEU KHIEN MAY BOM
  pinMode(RELAY_PS,OUTPUT); // RELAY DIEU KHIEN PHUN SUONG
  pinMode(LED_HUMP,OUTPUT);
  pinMode(LED_TEMP,OUTPUT);
  pinMode(LED_CD,OUTPUT);
//  pinMode(LED_GAS,OUTPUT);
  pinMode(LED_CHAY,OUTPUT);
  digitalWrite(RELAY_D,HIGH);
  digitalWrite(RELAY_Q,HIGH);
  digitalWrite(RELAY_B,LOW);
  digitalWrite(RELAY_PS,LOW);
  pinMode(AC_pin, OUTPUT);                          
  delay(500);
  Timer1.initialize(freqStep);                     
  Timer1.attachInterrupt(dim_check, freqStep);      
  attachInterrupt(1, zero_cross_detect, RISING);
  //attachInterrupt(1, baoDongISR, FALLING);
  vSemaphoreCreateBinary( xBinarySemaphore );   
  if( xBinarySemaphore != NULL )
  {
    xTaskCreate(hienThi,(const portCHAR *)"Display", 1000, NULL, 2, NULL);
    xTaskCreate(vSendSerial,(const portCHAR *)"Serial", 1000, NULL, 3, NULL);
    xTaskCreate(vData,"SEND1", 1000, NULL, 1, NULL );
    xTaskCreate(xulyduLieu,(const portCHAR *)"xuly", 1000, NULL, 2, NULL );
  }  
  vTaskStartScheduler();
} 
void zero_cross_detect() 
{    
  zero_cross = true;              
  h=0;
  digitalWrite(AC_pin, LOW);       
}                 
void dim_check() 
{                   
  if(zero_cross == true) 
  {              
    if(h>=dim) 
    {
      digitalWrite(AC_pin, HIGH);       
      h=0;                          
      zero_cross = false; 
    }
    else 
    {
      h++;                    
    }                                
  }                                  
} 
void playTone(long duration, int freq)
{
  duration *= 1000;
  int period = (1.0 / freq) * 1000000;
  long elapsed_time = 0;
  while (elapsed_time < duration)
  {
    digitalWrite(Buzzer,HIGH);
    delay(500);
//    delayMicroseconds(period / 2);
    digitalWrite(Buzzer, LOW);
    delay(200);
//    delayMicroseconds(period / 2);
    elapsed_time += (period);
  }
}   
void callNumber() {
  Serial2.print (F("ATD"));
  Serial2.print (phoneNumber);
  Serial2.print (F(";\r\n"));
  Serial.print ("ATD");
  Serial.print (phoneNumber);
  Serial.println (";\r\n");
  delay (5000) ;// gọi trong 20 giây.chỗ này các bạn tùy chỉnh theo ý mình cho phù hợp .
//  sim.println("ATH");   // kết thúc cuộc gọi.
}

void sendMessage(char textMessage[100]){
  Serial2.println("AT+CMGF=1");
  delay(1000);
  Serial2.println("AT+CMGS=\"+84964921207\"\r");
  delay(1000);
  Serial2.println(textMessage);
  delay(1000);
  Serial2.println((char)26);
  delay(100);
  Serial.println();
}

void SetupSIM(){
  // Bat nguon Module SIM
  Serial.println("Power on module");
  // Baudrate tu Uno sang Module
  Serial2.println("AT"); // Checking module 
  delay(1000);
  Serial.println("Set baudrate to sim800: 57600");
  Serial2.println("ATE0"); 
  delay(1000);
  Serial2.println("AT+IPR=57600");
  delay(1000);
  // Cho thoi gian de module vao mang GSM
  Serial.println("SIM is ready to send receive sms");
  // Chon che do TEXT mode
  Serial2.println("AT+CMGF=1");
  delay(100);
  // Set module to send SMS data to serial out upon receipt 
  Serial2.println("AT+CNMI=2,2,0,0,0");
  delay(100);
}
void loop()
{
   
}
void vData( void *pvParameters)
{
  portBASE_TYPE xStatus;
  xData xDataSendSS;
  const TickType_t xTicksToWait = 1000/ portTICK_PERIOD_MS; 
  pinMode(13,OUTPUT);
  dht.begin(); 
//  Serial2.begin(57600);  // test thử hoặc sử dụng hàm serialEvent()
  for( ;; )
  { 
    xDataSendSS.ucHump = dht.readHumidity();
    xDataSendSS.ucTemp = dht.readTemperature();
    xDataSendSS.ucCD = digitalRead(CDPIN); 
    xDataSendSS.ucCT = digitalRead(CTPIN);
    xDataSendSS.ucFire = digitalRead(FIRE);
    xDataSendSS.status_Q = digitalRead(RELAY_Q);
    xDataSendSS.status_D = digitalRead(RELAY_D);
    xDataSendSS.status_B = digitalRead(RELAY_B);
    xDataSendSS.status_PS = digitalRead(RELAY_PS);
    xDataSendSS.status_CC = myservo.read();
    xDataSendSS.status_CS = myservo1.read();
    float k = analogRead(A0);
    xDataSendSS.ucGAS = constrain(k,100,1024);  // gán nằm trong khoảng (100,1024)

    xDataSendSS.ucLux = lightMeter.readLightLevel();
    delay(200);

    if(flag_high)
    {
      while(dim>10)
      {
        dim--;
        delay(50);
      }
    }
    else if(flag_mid)
    {
      while(dim>85)
      {
        dim--;
        delay(50);
      }    
      while(dim<85)
      {
        dim++;
        delay(50);
      }    
     }
    else if(flag_low)
    {
      while(dim<128)
      {
        dim++;
        delay(50);
      }
    }
   
  if (rfid.isCard()) 
   {
     if (rfid.readCardSerial()) // Nếu có thẻ
      {

          for (i = 0; i < 5; i++)
          {

              reading_card[i] = rfid.serNum[i]; //Lưu mã thẻ đọc được vào mảng reading_card
          }
          Serial.println();
          //verification
          for (i = 0; i < 5; i++)
          {
              //So sáng từng phần tử của mảng reading_card với mảng master , thẻ open door
              if (reading_card[i] != master[i]) //Nếu có 1 phần tử bất kỳ nào không phù hợp...thỳ thoát vòng lặp, lúc này ta nhận được giá trị của i
              {
                  break;
              }
          }
          // Tương tự với thẻ Slave , thẻ close door
          for (j = 0; j < 5; j++)
          {
              if (reading_card[i] != slave[i])
              {
                  break;
              }
          }
          for (l = 0; l < 5; l++)
          {
              if (reading_card[i] != master1[i])
              {
                  break;
              }
          }
      }
      rfid.halt();
  }
  
  if (isnan(xDataSendSS.ucTemp) || isnan(xDataSendSS.ucHump)) {}
    else
    {
      xStatus = xQueueSendToBack(xQueue, &xDataSendSS, 0);
      xStatus = xQueueSendToBack(xQueue1, &xDataSendSS, 0);
      xStatus = xQueueSendToBack(xQueue2, &xDataSendSS, 0);
      Serial.println("OK SEND!");
    }
  taskYIELD();
     
  } 
} 

void vSendSerial(void *pvParameters)
{
  (void) pvParameters;
  xData xReceiveData;
  portBASE_TYPE xStatus;  
  const TickType_t xTicksToWait = 100/portTICK_PERIOD_MS;
  
  for (;;)
  {
    xStatus = xQueueReceive(xQueue1, &xReceiveData, xTicksToWait);
    if(xStatus == pdTRUE)
    {      
      if(Serial2.available())
      {
        Serial.println("Co tin nhan den");
        while(Serial2.available())
        {
          DataReceiver[count++] = Serial2.read();
//          receiverSMS = String(receiverSMS+DataReceiver[count]);
//          Serial.println(DataReceiver);
        }
//        Serial.println(DataReceiver);
        count = 0;
//        DataReceiver2[100]=DataReceiver[100];
//        DataReceiver2[100]="";
//        Serial.println(DataReceiver2);
//--------------------------------------------
        if(strstr(DataReceiver,"kichhoat"))
        {
          statusControl = 1;
          Serial.println("BAT CHE DO DIEU KHIEN BANG TAY");
          statusSMS = 1;
        }
//--------------------------------------------        
        if(strstr(DataReceiver,"huy"))
        {
          statusControl = 0;
          Serial.println("TAT CHE DO DIEU KHIEN BANG TAY");
          statusSMS = 2;
        }
//------------------------------------------------
        if(strstr(DataReceiver,"batchongtrom"))
        {
          statusSafe = 1;
          Serial.println("BAT CHE DO CHONG TROM");
          statusSMS = 3;
        }
//------------------------------------------------
        if(strstr(DataReceiver,"tatchongtrom"))
        {
          statusSafe = 0;
          Serial.println("TAT CHE DO CHONG TROM");
          statusSMS = 4;
        }
//-----------------------------------------------
        if(strstr(DataReceiver,"batden"))
        {
          statusSMS = 5;
          if(statusControl == 1){
            digitalWrite(RELAY_D,LOW);
            Serial.println("BAT DEN");
          }
        }
        
        if(strstr(DataReceiver,"batquat"))
        {
          statusSMS = 6;
          if(statusControl == 1){
            digitalWrite(RELAY_Q,LOW);
            Serial.println("BAT QUAT");
          }
        }
        
        if(strstr(DataReceiver,"batbom"))
        {
          statusSMS = 7;
          if(statusControl == 1){
            digitalWrite(RELAY_B,HIGH);
            Serial.println("BAT MAY BOM");
          }
        }
        
        if(strstr(DataReceiver,"batphunsuong"))
        {
          statusSMS = 8;
          if(statusControl == 1){
            digitalWrite(RELAY_PS,HIGH);
            Serial.println("BAT MAY PHUN SUONG");
          }
        }
        
        if(strstr(DataReceiver,"battatca"))
        {
          statusSMS = 9;
          if(statusControl == 1){
            digitalWrite(RELAY_D,LOW);
            digitalWrite(RELAY_Q,LOW);
            digitalWrite(RELAY_B,HIGH);
            digitalWrite(RELAY_PS,HIGH);
            Serial.println("BAT TAT CA THIET BI");
          }
        }
        
        if(strstr(DataReceiver,"tatden"))
        {
          statusSMS = 10;
          if(statusControl == 1){
            digitalWrite(RELAY_D,HIGH);
            Serial.println("TAT DEN");
          }
        }
        
        if(strstr(DataReceiver,"tatquat"))
        {
          statusSMS = 11;
          if(statusControl == 1){
            digitalWrite(RELAY_Q,HIGH);
            Serial.println("TAT QUAT");
          }
        }
        
        if(strstr(DataReceiver,"tatbom"))
        {
          statusSMS = 12;
          if(statusControl == 1){
            digitalWrite(RELAY_B,LOW);
            Serial.println("TAT BOM");
          }
        }
        
        if(strstr(DataReceiver,"tatphunsuog"))
        {
          statusSMS = 13;
          if(statusControl == 1){
            digitalWrite(RELAY_PS,LOW);
            Serial.println("TAT PHUN SUONG");
          }
        }

        if(strstr(DataReceiver,"tattatca"))
        {
          statusSMS = 14;
          if(statusControl == 1){
            digitalWrite(RELAY_D,HIGH);
            digitalWrite(RELAY_Q,HIGH);
            digitalWrite(RELAY_B,LOW);
            digitalWrite(RELAY_PS,LOW);
            Serial.println("TAT TAT CA");
          }
        }
      }

      if(statusSMS == 1)
        {
          sendMessage("Da kich hoat che do dieu khien SMS");
          statusSMS = 0;
        }
      if(statusSMS == 2)
        {
          sendMessage("Da huy che do dieu khien SMS"); 
          statusSMS = 0;
        }
      if(statusSMS == 3)
        {
          sendMessage("Da kich hoat che do chong trom");
          statusSMS = 0;
        }
      if(statusSMS == 4)
        {
          sendMessage("Da tat che do chong trom");
          statusSMS = 0;
        }
      if(statusSMS == 5)
        {
          sendMessage("Da bat den");
          statusSMS = 0;
        }
      if(statusSMS == 6)
        {
          sendMessage("Da bat quat");
          statusSMS = 0;
        }
      if(statusSMS == 7)
        {
          sendMessage("Da bat may bom");
          statusSMS = 0;
        }
      if(statusSMS == 8)
        {
          sendMessage("Da bat phun suong");
          statusSMS = 0;
        }
      if(statusSMS == 9)
        {
          sendMessage("Da bat tat ca cac thiet bi");
          statusSMS = 0;
        }
      if(statusSMS == 10)
        {
          sendMessage("Da tat den");
          statusSMS = 0;
        }
      if(statusSMS == 11)
        {
          sendMessage("Da tat quat");
          statusSMS = 0;
        }
      if(statusSMS == 12)
        {
          sendMessage("Da tat may bom");
          statusSMS = 0;
        }
      if(statusSMS == 13)
        {
          sendMessage("Da tat phun suong");
          statusSMS = 0;
        }
      if(statusSMS == 14)
        {
          sendMessage("Da tat tat ca cac thiet bi");
          statusSMS = 0;
        }
//-------------------------------------------------------------      
     if(Serial1.available())
      {
        while(Serial1.available())
        {
          DataReceiver1[count++] = Serial1.read();

        }
        count=0;
        Serial.println(DataReceiver1);
        if(strstr(DataReceiver1,"9"))
        {
          statusControl = 1;
        }
        if(strstr(DataReceiver1,"8"))
        {
          statusControl = 0;
        }
        if(strstr(DataReceiver1,"0"))
        {
          if(statusControl == 1)
          {
            digitalWrite(RELAY_D, HIGH); 
          }            
        }
        if(strstr(DataReceiver1,"1")) 
        {
          if(statusControl == 1)
          {
            digitalWrite(RELAY_D, LOW); 
          }    
        }
        
       if(strstr(DataReceiver1,"2")) 
       {
          if(statusControl == 1)
          {
            digitalWrite(RELAY_Q, HIGH); 
          }  
       }
        
        if(strstr(DataReceiver1,"3"))
        {
          if(statusControl == 1)
          {
            digitalWrite(RELAY_Q,LOW);
          }  
        } 
        
        if(strstr(DataReceiver1,"4"))
        {
          if(statusControl == 1)
          {
            digitalWrite(RELAY_B,LOW); 
          }  
        } 
        
        if(strstr(DataReceiver1,"5")) 
        {         
          if(statusControl == 1)
          {
            digitalWrite(RELAY_B,HIGH);
          }  
        }
        
        if(strstr(DataReceiver1,"6"))
        {         
          if(statusControl == 1)
          {
            digitalWrite(RELAY_PS,LOW);
          }  
        }      
        if(strstr(DataReceiver1,"7")) 
        {        
          if(statusControl == 1)
          {
            digitalWrite(RELAY_PS, HIGH);
          }  
        }
       } 
              
      for(int z=0;z<=100;z++)
      {
        DataReceiver[z]='\0';
      }
      for(int z=0;z<=100;z++)
      {
        DataReceiver1[z]='\0';
      }

      if(statusSafe == 1)
      {
        BCT = 1;
      }else BCT = 0;

      sprintf(chuoi, "T%dH%dC%dG%dL%dF%dB%dP%dW%dD%dA%dS%d",xReceiveData.ucTemp,xReceiveData.ucHump,xReceiveData.ucCT,xReceiveData.ucGAS,xReceiveData.status_D,xReceiveData.status_Q,xReceiveData.status_B,xReceiveData.status_PS,xReceiveData.status_CS,xReceiveData.status_CC,xReceiveData.ucFire,BCT);
//      Serial.println(chuoi);
      Serial1.println(chuoi);
//      vTaskDelay(3000/portTICK_PERIOD_MS);
      delay(3000);
    }
  }
}      

void hienThi(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  lcd.begin();
  xData xReceiveData;
  portBASE_TYPE xStatus;  
  const TickType_t xTicksToWait = 100/ portTICK_PERIOD_MS;
//  pinMode(DHTPIN, INPUT);
  lcd.print("TEMP&HUMD: ");
  lcd.setCursor(0,1);
  lcd.print("FIRE: ");
  lcd.setCursor(0,2);
  lcd.print("TROM: "); 
  lcd.setCursor(0,3);
  lcd.print("DOOR:");  
  lcd.createChar(1, degree);   
  for (;;) // A Task shall never return or exit.
  {
     xStatus = xQueueReceive(xQueue, &xReceiveData, xTicksToWait);
    if(xStatus == pdTRUE)
    {
      
      lcd.setCursor(11,0);
      lcd.print(xReceiveData.ucTemp);
      lcd.write(1);
      lcd.print("C");
      lcd.setCursor(16,0);
      lcd.print(xReceiveData.ucHump);
      lcd.print("%");
    
      if (xReceiveData.ucGAS>=gas ) 
        {
          lcd.setCursor(6,1);
          lcd.print("PHAT HIEN KHOI");
          if(xReceiveData.ucFire == 1)
          {
            lcd.setCursor(6,1);
            lcd.print("CANH BAO CHAY ");
          }
        }
      else 
        {
          lcd.setCursor(6,1);
          lcd.print("AN TOAN       ");
        }
      if(statusSafe == 1)
      {
        lcd.setCursor(6,2);
        lcd.print("KICH HOAT     ");
      }
      else
      {
        lcd.setCursor(6,2);
        lcd.print("KO KICH HOAT  ");
      }
      if(statusSafe == 1 && i != 5 && xReceiveData.status_CC!=0 && xReceiveData.ucCT == 0)
      {      
        lcd.setCursor(6,2);
        lcd.print("PHAT HIEN TROM");
      }     
//---------------THẺ TỰ ĐỘNG ĐÓNG MỞ CỬA----------------      
     if(i == 5)
      {
        myservo.write(90);
        lcd.setCursor(6,3);
        lcd.print("MO CUA       ");
        delay(10000);
        myservo.write(180);
        lcd.setCursor(6,3);
        lcd.print("DONG CUA      ");
        i=4;
      }
      if (j == 5)
        {
          myservo.write(180);
          lcd.setCursor(6,3);
          lcd.print("DONG CUA      ");
        }
//------------- THẺ TEST CHỐNG TRỘM -----------------      
      if (l == 5)
      {
        lcd.setCursor(6,3);
        lcd.print("TEST TROM     ");
        myservo.write(80);
      }
      
//--------------------------- LED CẢNH BÁO --------------------------------
      // Nhiệt độ và khí vượt ngưỡng gas >220 và temp>33 ==> cảnh báo cháy 
      if (xReceiveData.ucGAS >= gas && xReceiveData.ucFire == 1 ) 
        {
          digitalWrite(LED_CHAY,HIGH);
          digitalWrite(Buzzer,HIGH);
        }
      else 
        {
          digitalWrite(LED_CHAY,LOW); 
          digitalWrite(Buzzer,LOW);
        }
         
        
      //Ngưỡng nhiệt độ 23<nhiệt độ<=34 ==> cảnh báo nhiệt độ
      if((xReceiveData.ucTemp > temp1) || (xReceiveData.ucTemp < temp2))
        {   
          digitalWrite(LED_TEMP,HIGH); 
        }   
      else
        {
          digitalWrite(LED_TEMP,LOW);
        }
      // Ngưỡng độ ẩm >93
      if(xReceiveData.ucHump >=hump)
        {
          digitalWrite(LED_HUMP,HIGH);
        }
      else
        {
          digitalWrite(LED_HUMP,LOW);
        }
      //--------------------------------------------------------------           
//------ BIẾN ĐIỀU KHIỂN TỰ ĐỘNG statusControl = 0---------
      if(statusControl == 0) 
      {
        if(xReceiveData.ucHump >= hump || xReceiveData.ucTemp < temp2)
        {
          digitalWrite(RELAY_D,LOW);
        }
        else
        {
          digitalWrite(RELAY_D,HIGH);
        }
        if(xReceiveData.ucTemp > temp1 ) 
        {
          digitalWrite(RELAY_Q,LOW);
          digitalWrite(RELAY_PS,HIGH);
          
        }
        else
        {
          digitalWrite(RELAY_Q,HIGH);
          digitalWrite(RELAY_PS,LOW);
        }
        
        if (xReceiveData.ucGAS >= gas && xReceiveData.ucFire == 1) 
        {
          digitalWrite(RELAY_B,HIGH);
          digitalWrite(RELAY_Q,HIGH);
        }
        else 
        {
          digitalWrite(RELAY_B,LOW);
        }
      }
      if(statusSafe == 1 && i != 5 && xReceiveData.status_CC!=0 && xReceiveData.ucCT == 0)
      {
        digitalWrite(LED_CD,HIGH);
        digitalWrite(Buzzer,HIGH);
      }
      else 
      {
        digitalWrite(LED_CD,LOW);
        digitalWrite(Buzzer,LOW);
      }
      if(xReceiveData.ucTemp > temp1 || (xReceiveData.ucGAS>=gas && xReceiveData.ucFire == 1))
      {
        myservo1.write(90);
      }
      else
      {
        myservo1.write(0);
      }
      if(xReceiveData.ucHump >=hump || xReceiveData.ucTemp > temp1 || xReceiveData.ucTemp <temp2 )
      {
         playTone(50,300);
      }else playTone(0,0);
      
// -------------------- SEND SMS --------------------------------
      // Send sms cảnh báo nhiệt độ ngưỡng 1
      if((xReceiveData.ucTemp >=34 && xReceiveData.ucTemp <=35) && TTT1 == 0)
      {
         sendMessage("Canh bao nhiet do nguong 1");
         TTT1 = 1;
      }
      else if((xReceiveData.ucTemp < 34 || xReceiveData.ucTemp >35)&& TTT1 == 1)
      {
         TTT1 = 0;
      }
      // Send sms cảnh báo ngưỡng 2
      if((xReceiveData.ucTemp >temp1 || xReceiveData.ucTemp <temp2) && TTT == 0)
      {
         sendMessage("Canh bao nhiet do vuot nguong nguy hiem");
         TTT = 1;
      }
      else if((xReceiveData.ucTemp <=temp1 && xReceiveData.ucTemp >=temp2) && TTT == 1)
      {
         TTT = 0;
      }
      // Send sms cảnh báo độ ẩm
      if((xReceiveData.ucHump >=hump ) && TTH == 0)
      {
         sendMessage("Canh bao do am");
         TTH = 1;
      }
      else if((xReceiveData.ucHump <hump) && TTH == 1)
      {
         TTH = 0;
      }
      // Send sms cảnh báo khói
      if (xReceiveData.ucGAS >=gas  && TTG == 0 ) 
      {
        sendMessage("Canh bao khoi");
        TTG = 1;
      }
      else if(xReceiveData.ucGAS <gas  && TTG == 1)
      {
        TTG = 0;
      }
      // Send sms cảnh báo cháy
      if ((xReceiveData.ucFire == 1 && xReceiveData.ucGAS >=gas ) && TTC == 0 ) 
      {
        sendMessage("Canh bao chay");
        TTC = 1;
      }
      else if((xReceiveData.ucFire == 0 && xReceiveData.ucGAS <gas ) && TTC == 1)
      {
        TTC = 0;
      }
      if(statusSafe == 1 && i != 5 && xReceiveData.status_CC!=0 && xReceiveData.ucCT == 0 && TTCT == 0)
      {
        TTCT = 1;
        sendMessage("Canh bao dot nhap");
      }
      else if (statusSafe == 1 && i != 5 && xReceiveData.status_CC!=0 && xReceiveData.ucCT == 1 && TTCT == 1)
      {
        TTCT = 0;
      }     
  }
} 
}
void xulyduLieu(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  xData xReceiveData;
  portBASE_TYPE xStatus;  
  const TickType_t xTicksToWait = 100/ portTICK_PERIOD_MS;
  for (;;) // A Task shall never return or exit.
  {
     xStatus = xQueueReceive(xQueue2, &xReceiveData, xTicksToWait);
    if(xStatus == pdTRUE)
    {
              
      // xử lý chống trộm đc kích hoạt khi chế độ chống trộm đc kích hoạt
      if(statusSafe == 1 && i != 5 && xReceiveData.status_CC!=0 && xReceiveData.ucCT == 0)
      {
        delay(1000);
        callNumber();
        delay(1000);
      }
      if (xReceiveData.ucGAS >=gas && xReceiveData.ucFire == 1) 
      {
        delay(1000);
        callNumber();
        delay(1000);
      } 
      if(xReceiveData.ucLux <= 10 && xReceiveData.ucCD == LOW)
      {
        flag_mid=false;
        flag_high=true;
        flag_low=false;
      }
      if(xReceiveData.ucLux <= 10 && xReceiveData.ucCD == HIGH)
      {
        flag_mid=true;
        flag_high=false;
        flag_low=false;
      }
      if(xReceiveData.ucLux > 10)
      {
        flag_mid=false;
        flag_high=false;
        flag_low=true;
      }
      
    }
  }
}
