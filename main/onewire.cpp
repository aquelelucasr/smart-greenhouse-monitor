
#include "onewire.h"
#include "digital.h"
#include "delay.h"

ONEWIRE::ONEWIRE(gpio_num_t p)
{
	DEBUG("ONEWIRE:construtor\n");
	PIN_DADOS = p;
}

void ONEWIRE::low(void)
{
	digital.pinMode(PIN_DADOS, OUTPUT);
    digital.digitalWrite(PIN_DADOS,LOW);
}
void ONEWIRE::high(void)
{
     digital.pinMode(PIN_DADOS, INPUT);
}
uint8_t ONEWIRE::reset (void)
{
	uint8_t resposta;
	low();
	delay_us(480);
	high();
	delay_us(70);
	resposta=digital.digitalRead(PIN_DADOS);
  	delay_us(400);
  	return resposta;
}


uint8_t ONEWIRE::readBit(void)
{
    uint8_t c;
     low();
      delay_us(2);
      high();
      delay_us(11);
      c = digital.digitalRead(PIN_DADOS);
      delay_us(48);
      return c;
}
uint8_t ONEWIRE::readByte(void)
{
    uint8_t valor,x,c;
    
    valor  = 0;
   for (x=0;x<8;x++)
   {
      low();
      delay_us(5);
      high();
      delay_us(10);
      c = digital.digitalRead(PIN_DADOS);
      valor = (valor  ) | (c<<x);
      delay_us(70);
   }
   return valor;

}
void ONEWIRE::escreve_bit(uint8_t meu_bit)
{
  if (meu_bit)
  {
      low();
      delay_us(10);
      high();
      delay_us(55);
  }
  else
  {
      low();
      delay_us(56);
      high(); 
  }
  delay_us(2);
}
void ONEWIRE::writeByte(uint8_t v)
{
	DEBUG("ONEWIRE:writeByte\n");

	uint8_t x;
  	for (x=0;x<8;x++)
  	{
    		escreve_bit((v >> x) & 1); 
  	}

	
}
