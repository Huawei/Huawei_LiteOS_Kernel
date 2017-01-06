#include "delay_driver.h"

void delay_1ms(unsigned int ms)
{
    unsigned int y;
  	
    for(y=1500*ms;y>0;y--);
}

void delay_4us(unsigned int us)
{
    unsigned int x; 

    for(x=5*us;x>0;x--);
}
void delay_ms(unsigned int ms)  
{
    unsigned int y;
  	
    for(y=2000*ms;y>0;y--);
        for(int i=6000;i>0;i--);
}