#include <ESP8266WiFi.h>
#include <SoftwareSerial.h> 
#include <ArduinoJson.h>
/////////////////////////////
SoftwareSerial mySerial(D2,D3);  
const char* ssid = "TP-LINK_CA6A";
const char* password = "TAMSOMUOI";
const char* host = "datnbkdn.000webhostapp.com";
int t,h,c,g,l,q,b,s,w,ws,lu,tro;
char dataReceiver[100];
String temp="";
String humd="";
String cd="";
String gas="";
String light="";
String fan="";
String bom="";
String phunsuong="";
String door="";
String Window="";
String Lua="";
String Trom="";
int length_t = 0;
String line2, tr, tr1, tr2, tr3, tr4, sa, sa1, sa2, sa3, sa4;
int after, before;
void setup() 
{  
  Serial.begin(57600);
  mySerial.begin(57600);
  pinMode(D2,INPUT);
  pinMode(D3,OUTPUT);
  delay(100);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
}
void loop() {
if(Serial.available())
{
  Divide(); 
}
  // Sử dụng wifi client để tạo kết nối TCP
  WiFiClient client;
  const int httpPort = 80; //dich vu web la port 80, dns la port 53, dich vu ftp la port 21 // muon truy cap den server phai co dia chi ip=host va dich vu web laf port 80
  if (!client.connect(host, httpPort)) 
  {
    return;
  }
    String url = "/update.php?temp=" + String(t) + "&hump="+ String(h)+"&chuyenDong="+ String(c)+"&khiGas="+ String(g)+"&Den="+ String(l)+"&Quat="+ String(q)+"&Bom="+ String(b)+"&Phunsuong="+ String(s)+"&Door="+ String(w)+"&Window="+ String(ws)+"&Trom="+ String(tro)+"&Lua="+ String(lu);
  //Gửi request lên sever
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n"+ 
               "Connection: close\r\n\r\n");
if (!client.connect(host, httpPort)) 
  {
    return;
  }
  String url1 = "/status.php";
  client.print(String("GET ") + url1 + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if ((millis() - timeout) > 500) //sau mỗi 5s clinet sẽ dừng 
    {
      client.stop();
      return;
    }
  } 
  String line1;
  while(client.available())
  {
      String line = client.readStringUntil('\r');
      //Serial.println(line);
      if(line.lastIndexOf('|')>=0)
      {
          line1=line;
          tr=line1.substring(10, 11);
          tr1=line1.substring(2, 3);
          tr2=line1.substring(4, 5);         
          tr3=line1.substring(6, 7);          
          tr4=line1.substring(8, 9);
         
          sa=line2.substring(10, 11);
          sa1=line2.substring(2, 3);
          sa2=line2.substring(4, 5);
          sa3=line2.substring(6, 7);
          sa4=line2.substring(8, 9);
          line2=line1;
          if(tr!=sa)
           {
                    Serial.println(tr);
           }
          if(tr1!=sa1)
           {
                   Serial.println(tr1);
           }
          if(tr2!=sa2)
           {
                   Serial.println(tr2);
           }
          if(tr3!=sa3)
           {
                   Serial.println(tr3);
           }
          if(tr4!=sa4)
           {
                   Serial.println(tr4);
           }   
          break;
      }
      else continue;
 
}
}
void Divide()
{
    char dataReceiver[100];
    int len=sizeof(dataReceiver);
    memset(dataReceiver,0,100);
    Serial.readBytesUntil( '\n',dataReceiver,100);
   // Serial.println(dataReceiver);
    for(int i = 0; i < len; i++)
     {    
          if(dataReceiver[i]=='T')
          {
                for(int j = i+1; j < len; j++)
                {
                    if(dataReceiver[j]=='H')
                    {       
                       for(int k = j+1; k < len; k++)
                       {                 
                           humd+=dataReceiver[k];                      
                       }                            
                       break;
                    }                   
                    temp+=dataReceiver[j];                  
                }
                break;        
           }       
    }
    t=temp.toInt();
    h=humd.toInt();
    //--------------------------------------------------//
    for(int i1 = 0; i1 < len; i1++)
     {    
          if(dataReceiver[i1]=='C')
          {
                for(int j1 = i1+1; j1 < len; j1++)
                {
                    if(dataReceiver[j1]=='G')
                    {       
                       for(int k1 = j1+1; k1 < len; k1++)
                       {                 
                           gas+=dataReceiver[k1];                      
                       }                            
                       break;
                    }                   
                    cd+=dataReceiver[j1];                  
                }
                break;        
           }       
    }
    c=cd.toInt();
    g=gas.toInt();
    //--------------------------------------------------//
    for(int i2 = 0; i2 < len; i2++)
     {    
          if(dataReceiver[i2]=='L')
          {
                for(int j2 = i2+1; j2 < len; j2++)
                {
                    if(dataReceiver[j2]=='F')
                    {       
                       for(int k2 = j2+1; k2 < len; k2++)
                       {                 
                           fan+=dataReceiver[k2];                      
                       }                            
                       break;
                    }                   
                    light+=dataReceiver[j2];                  
                }
                break;        
           }       
    }
    l=light.toInt();
    q=fan.toInt();
    //--------------------------------------------------//
    for(int i3 = 0; i3 < len; i3++)
     {    
          if(dataReceiver[i3]=='B')
          {
                for(int j3 = i3+1; j3 < len; j3++)
                {
                    if(dataReceiver[j3]=='P')
                    {       
                       for(int k3 = j3+1; k3 < len; k3++)
                       {                 
                           phunsuong+=dataReceiver[k3];                      
                       }                            
                       break;
                    }                   
                    bom+=dataReceiver[j3];                  
                }
                break;        
           }       
    }
    s=phunsuong.toInt();
    b=bom.toInt();
    //--------------------------------------------------//
    for(int i4 = 0; i4 < len; i4++)
     {    
          if(dataReceiver[i4]=='W')
          {
                for(int j4 = i4+1; j4 < len; j4++)
                {
                    if(dataReceiver[j4]=='D')
                    {       
                       for(int k4 = j4+1; k4 < len; k4++)
                       {                 
                           door+=dataReceiver[k4];                      
                       }                            
                       break;
                    }                   
                    Window+=dataReceiver[j4];                  
                }
                break;        
           }       
    }
    w=door.toInt();
    ws=Window.toInt();
    //--------------------------------------------------//
    for(int i5 = 0; i5 < len; i5++)
     {    
          if(dataReceiver[i5]=='A')
          {
                for(int j5 = i5+1; j5 < len; j5++)
                {
                    if(dataReceiver[j5]=='S')
                    {       
                       for(int k5 = j5+1; k5 < len; k5++)
                       {                 
                           Trom+=dataReceiver[k5];                      
                       }                            
                       break;
                    }                   
                    Lua+=dataReceiver[j5];                  
                }
                break;        
           }       
    }
    tro=Trom.toInt();
    lu=Lua.toInt();
    temp=" ";
    humd=" ";
    cd=" ";
    gas=" ";
    light=" ";
    fan=" ";
    bom=" ";
    phunsuong=" ";
    door=" ";
    Window=" ";
    Trom=" ";
    Lua=" ";    
}
