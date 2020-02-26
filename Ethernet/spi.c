
#include <stdint.h>
#include "spi.h"

 void W5500_select(){SPI_PORT &=~(1<<SPI_SS);}
 void W5500_release(){SPI_PORT |=(1<<SPI_SS);}

 unsigned char W5500_rx(){return W5500_rxtx(0xff);}
 void W5500_tx(unsigned char data){ W5500_rxtx(data);}

uint8_t W5500_rxtx(uint8_t data)
{
	SPDR = data;
		while (!(SPSR & (1<<SPIF)));
		return SPDR;
}



void spiW5500_init(void)
{

	SPI_DDR |= (1<<SPI_SCK) | (0<<SPI_MISO) | (1<<SPI_MOSI) | (1<<SPI_SS);
	SPI_PORT= (0<<SPI_SCK) | (0<<SPI_MISO) | (0<<SPI_MOSI) | (0<<SPI_SS);


	// SPI initialization
	// SPI Type: Master
	// SPI Clock Rate: 250,000 kHz
	// SPI Clock Phase: Cycle Start
	// SPI Clock Polarity: Low
	// SPI Data Order: MSB First
	SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (0<<SPR0);
	SPSR=(0<<SPI2X);
}
