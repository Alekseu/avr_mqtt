/*
 * sensorStorage.h
 *
 *  Created on: 19 нояб. 2018 г.
 *      Author: Алексей
 */

#ifndef SENSORSTORAGE_H_
#define SENSORSTORAGE_H_

#define SENSOR_ACTIVE_TIME 30

struct Sensors
{
	unsigned char Zone;
	unsigned char Code;
	bool State;
	unsigned int uptime;
};

Sensors _sensors[100];


volatile char zone=0;
volatile char data=0;
volatile char data_was_read =0;

unsigned char sensorCount=0;
bool sensorWasFound=false;


ISR(INT0_vect)
{
	data=0;
	if((PINC & (1<<1))>0){data|=(1<<0);}
	if((PINC & (1<<2))>0){data|=(1<<1);}
	if((PINC & (1<<3))>0){data|=(1<<2);}
	if((PINC & (1<<4))>0){data|=(1<<3);}
	data_was_read=1;
}

void sensorProcessing()
{
	if(data_was_read==0)
			{
				switch(zone)
				{
				case 0:
					PORTD|=(1<<3);
					PORTD&=~((1<<4)|(1<<5));
					zone=1;

					break;
				case 1:
					PORTD|=(1<<4);
					PORTD&=~((1<<3)|(1<<5));
					zone=2;

					break;
				case 2:
					PORTD|=(1<<5);
					PORTD&=~((1<<3)|(1<<4));
					zone=0;
					break;

				}
			}

			if(data!=0 && data_was_read==1)
			{
				sensorWasFound = false;
				//пробегаемся по всем текущим датчикам в памяти
				for(int i=0;i<sensorCount;i++)
				{
					if(_sensors[i].Zone == zone && _sensors[i].Code == data)
					{
						_sensors[i].State = true;
						_sensors[i].uptime = SENSOR_ACTIVE_TIME;
						sensorWasFound = true;
					}
				}

				if(!sensorWasFound)
				{
					_sensors[sensorCount].Zone=zone;
					_sensors[sensorCount].Code = data;
					_sensors[sensorCount].State = true;
					_sensors[sensorCount].uptime=SENSOR_ACTIVE_TIME;
					sensorCount++;
				}

					data=0;
					data_was_read=0;
			}
			else
			{
				for(int i=0;i<sensorCount;i++)
				{
					if(_sensors[i].State)
					{
						if(_sensors[i].uptime>0)
						{
							_sensors[i].uptime--;
						}
						else
						{
							_sensors[i].State = false;
						}
					}
				}
			}


}



#endif /* SENSORSTORAGE_H_ */
