/*
 * main.cpp
 *
 *  Created on: 14 но€б. 2018 г.
 *      Author: hudienko_a
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "Utils.h"
#include<stdlib.h>


///TODO вынести все настройки ip,mqtt,порты и т.д. в eeprom также подписатс€ на событие получени€ настроек и сохранени€ настроек

unsigned char MQTT_SOCKET_NUMBER =1;


extern "C"
{


#include "Ethernet/wizchip_conf.h"
#include "Ethernet/DHCP/dhcp.h"
#include "Ethernet/spi.h"
#include "Ethernet/socket.h"

//#define DHCP 1

#define SOCK_DHCP       6

wiz_NetInfo gWIZNETINFO;

unsigned char ip_ok = 0;
unsigned char my_dhcp_retry = 0;
unsigned char mac[6]={0,1,2,3,4,6};
unsigned char ip[4]={192,168,1,85};
unsigned char mask[4]={255,255,255,0};
unsigned char gate[4]={192,168,0,1};
unsigned char dns[4]={192,168,0,1};

void my_ip_assign(void)
{
	getIPfromDHCP(gWIZNETINFO.ip);
	getGWfromDHCP(gWIZNETINFO.gw);
	getSNfromDHCP(gWIZNETINFO.sn);
	getDNSfromDHCP(gWIZNETINFO.dns);
	gWIZNETINFO.dhcp = NETINFO_DHCP;
	/* Network initialization */
	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);    // apply from DHCP
	ip_ok=1;
}

void my_ip_conflict(void)
{

}

#include "Ethernet/mqtt2/umqtt.h"
unsigned char mqttaddr[4]={192,168,1,173};
uint32_t ticks=0;
//char _userId[32];
char _topic[32];
//char _data[64];
//int _data_len =0;
bool message = false;


static void *app_malloc(size_t size) {
	return malloc(size);
}

static void app_free(void *ptr) {
	free(ptr);
}


static int netReadPacket(void* pNet, uint8_t **ppBuf) {

	unsigned char soc_status = getSn_SR(*(unsigned char*)pNet);

		if(soc_status== SOCK_ESTABLISHED){
			uint16_t len= getSn_RX_RSR(*(unsigned char*)pNet);

	    if (len)
	    {
	        // allocate some memory to hold the packet data
	        uint8_t *buf = (uint8_t*)app_malloc(len);
	        if (buf)
	        {
	            // call net_client read function to get the packet data
	             len = recv(*(unsigned char*)pNet,buf,len);
	            *ppBuf = buf;
	        }
	        else // allocation failure
	        {
	           // UARTprintf("netRead malloc fail for %u\n", len);
	        }
	    }
	    // return the number of bytes that were copied into the packet buffer
	    // umqtt will free the packet buffer when it is finished
	    return len;

	}
		return 0;
}

static int netWritePacket(void *pNet, const uint8_t *pBuf, uint32_t len, bool isMore){
//	int count = 0, offset=0, send_ = len;
//		int try_ = 1000;
//		do
//		{
//			--try_;

			    unsigned char soc_status = getSn_SR(*(unsigned char*)pNet);
				if(soc_status== SOCK_ESTABLISHED){

					//count =	send(*(unsigned char*)pNet,(unsigned char*)pBuf+offset,len);
					len =	send(*(unsigned char*)pNet,(unsigned char*)pBuf,len);
//					if(count>0)
//					{
//						_delay_ms(10);
//						offset = count;
//						len = len - count;
//					}
				}
//		}
//		while (offset!=send_ && try_>0);
		return len;
}

static umqtt_TransportConfig_t transportConfig =
{
		&MQTT_SOCKET_NUMBER, app_malloc, app_free, netReadPacket, netWritePacket
    // hNet is populated at run time after network is opened
};

static void
ConnackCb(umqtt_Handle_t h, void *pUser, bool sessionPresent, uint8_t retCode){
}

static void
PublishCb(umqtt_Handle_t h, void *pUser, bool dup, bool retain, uint8_t qos,
          const char *pTopic, uint16_t topicLen, const uint8_t *pMsg, uint16_t msgLen){
}

static void
PubackCb(umqtt_Handle_t h, void *pUser, uint16_t pktId){
}

static void
SubackCb(umqtt_Handle_t h, void *pUser, const uint8_t *retCodes,
         uint16_t retCount, uint16_t pktId){
}

static void
PingrespCb(umqtt_Handle_t h, void *pUser){
}

static umqtt_Callbacks_t callbacks =
{
    ConnackCb, PublishCb, PubackCb, SubackCb, NULL, PingrespCb
};


}


void sendbyte(char b)
{
//	 while(!(UCSR0A & (1<<UDRE0)));
//	 	UDR0 = b;
}

void SendString(char *StringPtr)
 {

// 	while(*StringPtr != 0x00)
// 	{
// 		sendbyte(*StringPtr);
// 		StringPtr++;
// 	}

 }


int main()
{


	// Port C initialization
	// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=Out
	DDRC=(1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
	// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=0
	PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

//	// USART initialization
//	// Communication Parameters: 8 Data, 1 Stop, No Parity
//	// USART Receiver: On
//	// USART Transmitter: On
//	// USART0 Mode: Asynchronous
//	// USART Baud Rate: 9600
//	UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
//	UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
//	UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
//	UBRR0H=0x00;
//	UBRR0L=0x67;

	// Port D initialization
	// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=In Bit1=In Bit0=In
	DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
	// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=T Bit1=T Bit0=T
	PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

	PORTC=0x00;
	PORTD=0x00;

	sei();

	_delay_ms(3500);

		SendString((char*)"init spi \r\n ");
		spiW5500_init();

		reg_wizchip_cs_cbfunc(W5500_select, W5500_release);
		reg_wizchip_spi_cbfunc(W5500_rx, W5500_tx);

		uint8_t tmpstr[6] = {0,};

		ctlwizchip(CW_GET_ID,(void*)tmpstr) ;

		static const uint8_t myPHYCFGR = 0xD8;
		setPHYCFGR(myPHYCFGR);

		int count=100;
		uint8_t phy_l;
		do
		{
			ctlwizchip(CW_GET_PHYLINK, (void*)&phy_l);
			SendString((char*)"phi fail\r\n ");
			_delay_ms(100);

		}while(phy_l != PHY_LINK_ON && --count);

		if(count<=0)
		{
			return 0;
		}

		memcpy(gWIZNETINFO.mac,mac,6);
		ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

	#ifdef DHCP
		DHCP_init(SOCK_DHCP, gDATABUF);
		reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);
		while(1)
		{
			uint8_t cl = DHCP_run();
			if(ip_ok)
			{
				SendString((char*)"ip was geted\r\n ");
				break;
			}
			switch(cl)
			{
			case DHCP_IP_ASSIGN:
			case DHCP_IP_CHANGED:
				break;
			case DHCP_IP_LEASED:
				break;
			case DHCP_FAILED:
				my_dhcp_retry++;
				if(my_dhcp_retry > 2)
				{
					gWIZNETINFO.dhcp = NETINFO_STATIC;
					DHCP_stop();      // if restart, recall DHCP_init()
					my_dhcp_retry = 0;
				}
				break;
			default:
				break;
			}
		}
	#else
		memcpy(gWIZNETINFO.ip,ip,4);
		memcpy(gWIZNETINFO.sn,mask,4);
		memcpy(gWIZNETINFO.gw,gate,4);
		memcpy(gWIZNETINFO.dns,dns,4);
		gWIZNETINFO.dhcp = NETINFO_STATIC;

		ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
	#endif



		int _mqttSocket = socket(MQTT_SOCKET_NUMBER,Sn_MR_TCP,1883,0);
		int len=0;
		unsigned char soc_status =0;

		umqtt_Handle_t hu = umqtt_New(&transportConfig, &callbacks, NULL);
//				if (!hu)
//				{
//					for (;;) {}
//				}

		int counter=0;
		int blink=0;
		bool blick_trigger=false;
		int sendCount=0;
		int processCount=0;





	while(true)
	{

		soc_status = getSn_SR(MQTT_SOCKET_NUMBER);
		switch(soc_status)
		{
		case SOCK_CLOSED:

			_mqttSocket = socket(MQTT_SOCKET_NUMBER,Sn_MR_TCP,1883,0);

			break;
		case SOCK_INIT:
			if(connect(_mqttSocket,mqttaddr,1883)==SOCK_OK)
			{
				SendString((char*)"connect to mqtt\r\n ");
				 uint8_t willPayload[] = { '0' };
				 umqtt_Error_t err;
				 err = umqtt_Connect(hu, true, true, 0, 60, "AVR_MQTT",  "/control/1", willPayload, 1, NULL, NULL);
//				 uint8_t qoss[1] = { 0 };
//				 char *topics[1] = { (char*)"/control/1" };
//
//				 umqtt_Subscribe(hu, 1, topics, qoss, NULL);
			}
			break;
		case SOCK_LISTEN:

			break;
		case SOCK_ESTABLISHED:

			break;
		case SOCK_CLOSE_WAIT:
			close(MQTT_SOCKET_NUMBER);
			break;
		}

		if(counter++>=1000)
		{
			counter=0;
			umqtt_PingReq(hu);

		}

		if(sendCount++>=500){

			sendCount=0;
			 umqtt_Publish(hu, "/avr", (const uint8_t*)"test", 4, 0, false, NULL);
			//umqtt_publish(&con,(char*)"/avr",(uint8_t*)"atatatat",8);
			//umqtt_process(&con);
		}

		if(processCount++>=150){
			umqtt_Error_t err = umqtt_Run(hu,ticks);
		}




		if(message){
			message = false;

			if(_strcmp(_topic,(char*)"/control/1")){

				//umqtt_publish(&con,(char*)"avr",(uint8_t*)_data,_data_len);

			}

			if(_strcmp(_topic,(char*)"/control/255")){
				//umqtt_publish(&con,(char*)"avr255",(uint8_t*)_data,len);
			}

		}

		if(blink++>=5)
		{
			blink=0;
			if(blick_trigger){
				PORTD|=(1<<2);
			}
			else
			{
				PORTD&=~(1<<2);
			}
			ticks++;
		}

		_delay_ms(10);


	}
	return 0;
}

