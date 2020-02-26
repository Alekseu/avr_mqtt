
#include <avr/io.h>

#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_MISO PB4
#define SPI_MOSI PB3
#define SPI_SCK  PB5
#define SPI_SS   PB2


 void W5500_select();
 void W5500_release();

 unsigned char W5500_rx();
 void W5500_tx(unsigned char data);

void spiW5500_init(void);

uint8_t W5500_rxtx(uint8_t data);
