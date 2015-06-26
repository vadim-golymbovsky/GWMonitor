

/*-----( Import needed libraries )-----*/
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>//!!!
#include <ICMPPing.h>


/*-----( Declare Constants )-----*/

//----------------------------------------
// Настройки сети
//----------------------------------------
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; //МАК адрес
IPAddress ip(192,168,0,243);                       //IP  адрес
IPAddress myDns(8,8,8,8);                          //Предпочитаемый DNS
IPAddress gateway(192,168,0,254);  
IPAddress pingAddr(192,168,0,72);

const int pingInInternetCount = 4;
IPAddress pingInInternet[pingInInternetCount] = { IPAddress(8,8,8,8), IPAddress(8,8,4,4), IPAddress(77,88,8,8), IPAddress(77,88,8,1) };
int pingInInternetIndex=0;

const int pingInLocalCount = 6;
IPAddress pingInLocal[pingInLocalCount] = { 
   IPAddress(192,168,0,71)
 , IPAddress(192,168,0,72)
 , IPAddress(192,168,0,128) 
 , IPAddress(192,168,0,139)
 , IPAddress(192,168,0,140) 
 , IPAddress(192,168,0,141) 
 };
int pingInLocalIndex=0;

const int gatewaysCount = 3;
IPAddress gateways[gatewaysCount] = { IPAddress(192,168,0,1), IPAddress(192,168,0,3), IPAddress(192,168,0,254) };
int gatewaysIndex=0;


//----------------------------------------
//Настройки датчиков и платы
//----------------------------------------

#define LED_PIN 13     //Вывод для подключения светодиода индикации


/*-----( Declare objects )-----*/
// initialize the library instance:
EthernetClient client;
SOCKET pingSocket = 0;
char buffer [256];
ICMPPing ping(pingSocket, (uint16_t)random(0, 255));

/*----( SETUP: RUNS ONCE )----*/
void setup() 
{
  /*-(start serial port to see results )-*/
  Serial.begin(9600);
  delay(1000);
  Serial.println();
  Serial.println("*************************");
  Serial.println("*   GW Monitor v1.0     *");
  Serial.println("* Vadim_Soft  (c) 2015  *");
  Serial.println("*************************");
  Serial.println();
  Serial.println("Start programm....");
  Serial.println();
  pinMode(LED_PIN, OUTPUT);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns,gateway);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  delay(1000);

}/*--(end setup )---*/

void loop() /*----( LOOP: RUNS CONSTANTLY )----*/
{
digitalWrite(LED_PIN, HIGH);

Serial.println("");
Serial.println("");
Serial.println("");
Serial.println("***************************************************");
int gwWorked=checkGateways();
Serial.println("--------------------------------------");
Serial.print("Gateway work ");
Serial.print(gwWorked);
Serial.print(" from ");
Serial.println(gatewaysCount);
gwWorked=checkLocalServers();
Serial.println("--------------------------------------");
Serial.print("Local servers work ");
Serial.print(gwWorked);
Serial.print(" from ");
Serial.println(pingInLocalCount);

digitalWrite(LED_PIN, LOW);  
delay(5000);
}

int checkGateways()
{
int gwWorked=0;
  
   Serial.println("====================================");
   Serial.println("Begin checking gateways  ... ");
   Serial.println("====================================");
   for(int g=0;g<gatewaysCount;g++)
   {
      IPAddress curg=gateways[g];
      // start the Ethernet connection using a fixed IP address and DNS server:
      Ethernet.begin(mac, ip, myDns,gateway);
      Serial.print("Getaway: ");
      Serial.println(curg);
      Serial.println("4 ping  ... ");
      int res = 0;
      res+=SendPing(getNextPingInInternet());
      delay(200);
      res+=SendPing(getNextPingInInternet());
      delay(200);
      res+=SendPing(getNextPingInInternet());
      delay(200);
      res+=SendPing(getNextPingInInternet());
      delay(200);
      if(res<2)
        Serial.println("--------------BAD GATEWAY-------------");
      else gwWorked++;
   }  //for(int g=0;g<gatewaysCount;g++)
   Serial.println("====================================");
   Serial.println("End checking gateways. ");
   Serial.println("====================================");

   return gwWorked;
}

int checkLocalServers()
{
int gwWorked=0;
  
   Serial.println("====================================");
   Serial.println("Begin checking Local Servers  ... ");
   Serial.println("====================================");
   for(int g=0;g<pingInLocalCount;g++)
   {
      IPAddress curg=pingInLocal[g];

      Serial.print("Local server: ");
      Serial.println(curg);
      Serial.println("3 ping  ... ");
      int res = 0;
      res+=SendPing(curg);
      delay(200);
      res+=SendPing(curg);
      delay(200);
      res+=SendPing(curg);
      delay(200);      
      if(res<2)
        Serial.println("--------------BAD SERVER-------------");
      else gwWorked++;
   }  //for(int g=0;g<gatewaysCount;g++)
   Serial.println("====================================");
   Serial.println("End checking Local Servers.");
   Serial.println("====================================");

   return gwWorked;
}






IPAddress getNextPingInInternet()
{
  int res =  pingInInternetIndex;
  pingInInternetIndex++;  
  if(pingInInternetIndex>=pingInInternetCount) pingInInternetIndex=0;
  return pingInInternet[res];
}

IPAddress getNextPingInLocal()
{
  int res =  pingInLocalIndex;
  pingInLocalIndex++;  
  if(pingInLocalIndex>=pingInInternetCount) pingInLocalIndex=0;
  return pingInLocal[res];
}


int SendPing(IPAddress pingIp)
{
  int res=0;
  ICMPEchoReply echoReply = ping(pingIp, 4);
if (echoReply.status == SUCCESS)
  {
    sprintf(buffer,
            "Reply[%d] from: %d.%d.%d.%d: bytes=%d time=%ldms TTL=%d",
            echoReply.data.seq,
            echoReply.addr[0],
            echoReply.addr[1],
            echoReply.addr[2],
            echoReply.addr[3],
            REQ_DATASIZE,
            millis() - echoReply.data.time,
            echoReply.ttl);
            res=1;
  }
   else
  {
    Serial.print("IP address for ping: ");
    Serial.println(pingIp);
    sprintf(buffer, "Echo request failed; %d", echoReply.status);
  }
  Serial.println(buffer);
  return res;
}
