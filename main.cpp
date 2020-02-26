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



///TODO вынести все настройки ip,mqtt,порты и т.д. в eeprom также подписатс€ на событие получени€ настроек и сохранени€ настроек

#define MQTT_SOCKET_NUMBER 1

extern "C"
{


#include "Ethernet/wizchip_conf.h"
#include "Ethernet/DHCP/dhcp.h"
#include "Ethernet/spi.h"
#include "Ethernet/socket.h"

#define DHCP 1

#define SOCK_DHCP       6
#define DATA_BUF_SIZE   200

wiz_NetInfo gWIZNETINFO;

uint8_t gDATABUF[DATA_BUF_SIZE];

unsigned char ip_ok = 0;
unsigned char my_dhcp_retry = 0;
unsigned char mac[6]={0,1,2,3,4,6};
unsigned char ip[4]={192,168,0,5};
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

#include "Ethernet/umqtt/umqtt.h"

#define MQTT_BUFFERLENGTH 128

unsigned char mqttaddr[4]={192,168,2,208};

umqtt_connection con;

uint8_t rxbuff[MQTT_BUFFERLENGTH];
uint8_t txbuff[MQTT_BUFFERLENGTH];
uint8_t buff[MQTT_BUFFERLENGTH];

void connected_callback(umqtt_connection* _con)
{
	umqtt_subscribe(_con, (char*)"/serv/#");
}

void message_callback(umqtt_connection* _con , char *topic, uint8_t *data, int len)
{
	//umqtt_publish(_con,(char*)"avr",data,len);
}

void new_pack(umqtt_connection* _con)
{

	memset(buff,0,MQTT_BUFFERLENGTH);

	int len = umqtt_circ_pop(&_con->txbuff, buff, MQTT_BUFFERLENGTH);

	int count = 0, offset=0, send_ = len;
	int try_ = 10000;
	do
	{
		--try_;
		count=	send(MQTT_SOCKET_NUMBER,buff+offset,len);
		if(count>0)
		{
			_delay_ms(100);
			offset = count;
			len = len - count;
		}
	}
	while (offset!=send_ && try_>0);

}

}

#include "sensorStorage.h"

void sendbyte(char b)
{
	 while(!(UCSR0A & (1<<UDRE0)));
	 	UDR0 = b;
}

 void SendString(char *StringPtr)
 {

 	while(*StringPtr != 0x00)
 	{
 		sendbyte(*StringPtr);
 		StringPtr++;
 	}

 }

int main()
{


	// Port C initialization
	// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=Out
	DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (1<<DDC0);
	// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=0
	PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART0 Mode: Asynchronous
	// USART Baud Rate: 9600
	UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
	UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
	UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
	UBRR0H=0x00;
	UBRR0L=0x67;

	// Port D initialization
	// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=In Bit1=In Bit0=In
	DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
	// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=T Bit1=T Bit0=T
	PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

	// External Interrupt(s) initialization
	// INT0: On
	// INT0 Mode: Rising Edge
	// INT1: Off
	// Interrupt on any change on pins PCINT0-7: Off
	// Interrupt on any change on pins PCINT8-14: Off
	// Interrupt on any change on pins PCINT16-23: Off
	EICRA=(0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (1<<ISC00);
	EIMSK=(0<<INT1) | (1<<INT0);
	EIFR=(0<<INTF1) | (1<<INTF0);
	PCICR=(0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

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

		con.connected_callback = connected_callback;
		con.message_callback = message_callback;
		con.new_packet_callback = new_pack;

		int counter=0;
		int sendDataCounter=0;
		int currentsensor=0;

	while(true)
	{

		sensorProcessing();

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
				con.clientid = (char*)"mqtt_AVR";
				con.kalive = 60;
				con.rxbuff.start = rxbuff;
				con.rxbuff.length = MQTT_BUFFERLENGTH;

				con.txbuff.start = txbuff;
				con.txbuff.length = MQTT_BUFFERLENGTH;

				umqtt_connect(&con);

			}
			break;
		case SOCK_LISTEN:

			break;
		case SOCK_ESTABLISHED:
			len= getSn_RX_RSR(MQTT_SOCKET_NUMBER);
			if(len >0)
			{
				memset(gDATABUF,0,DATA_BUF_SIZE);
				len = recv(MQTT_SOCKET_NUMBER,gDATABUF,DATA_BUF_SIZE);
				if(len>0)
				{
					SendString((char*)"receive from mqtt\r\n ");
					umqtt_circ_push(&con.rxbuff, (uint8_t *) gDATABUF, (int) len);
					umqtt_process(&con);
				}
			}

			break;
		case SOCK_CLOSE_WAIT:
			close(MQTT_SOCKET_NUMBER);
			break;
		}

		if(counter++>=150)
		{
			counter=0;
			umqtt_ping(&con);
		}

		if(sendDataCounter++>=100)
		{
			char _path[14];
			memset(_path,0,14);
			_path[0]='/';
			_path[1]='R';
			_path[2]='F';
			_path[3]='4';
			_path[4]='3';
			_path[5]='3';
			_path[6]='/';
			_path[7]='Z';
			_path[8]= _sensors[currentsensor].Zone+0x30;
			_path[9]='/';
			_path[10]=(((_sensors[currentsensor].Code)/10%10)+0x30);
			_path[11]=(((_sensors[currentsensor].Code)%10)+0x30);
			_path[12]='/';
			_path[13]=0;

			char _data[2];
			char _state = _sensors[currentsensor].State ==0? 0:1;
			memset(_data,0,3);

			_data[0]=((_state%10)+0x30);
			_data[1]=0;

			umqtt_publish(&con,_path,(uint8_t*)_data,2);

			if(currentsensor++>sensorCount)
			{
				currentsensor=0;
			}
		}


		PORTC|=(1<<0);
		_delay_ms(100);
		PORTC&=~(1<<0);
		_delay_ms(100);
	}
	return 0;
}

