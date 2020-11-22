#include "stm32f1xx.h"


int clock_init()
{
    // start hse
    RCC->CR |= RCC_CR_HSEON;
    int max_wait_cnt = 0x1000;
    while(!(RCC->CR & RCC_CR_HSERDY))
    {
        if(max_wait_cnt == 0)
        {
            //back up to HSI and return
            RCC->CR &= ~(RCC_CR_HSEON);
            return 1;
        }
        max_wait_cnt--;
    }
    
    //configure pll
    RCC->CFGR |= RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC;
    RCC->CFGR &= ~(RCC_CFGR_PLLXTPRE);

    //start pll
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY))
    {
        if(max_wait_cnt == 0)
        {
            //back up to HSI, plloff and return
            RCC->CR &= ~(RCC_CR_HSEON);
            RCC->CR &= ~(RCC_CR_PLLON);
            return 2;
        }
        max_wait_cnt--;
    }
    //configure FLASH dummy cycles
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    //configure bus dividers
    RCC->CFGR &= ~(RCC_CFGR_HPRE_DIV512);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR &= ~(RCC_CFGR_PPRE2_DIV16);
    //set pll as clk source
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_PLL);

    //turn off HSI
    RCC->CR &= ~(RCC_CR_HSION);
    return 0;
}


void enable_gpio_c_port()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
}

void configure_led_pin()
{
    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOC->CRH |= (0x2 << GPIO_CRH_MODE13_Pos) | (0x0 << GPIO_CRH_CNF13_Pos);
}


#define PIN_HIGH 1
#define PIN_LOW 0

void set_led_pin_val(int val)
{
    if(val == PIN_LOW)
    {
        //GPIOC->BRR = 1<<13;
        GPIOC->ODR &= ~(1<<13);
    }
    else    //high
    {    
        //GPIOC->BSRR = 1<<13;
        GPIOC->ODR |= (1<<13);
    }
    return;
}



int main()
{
    volatile int result = clock_init();
    if(result !=0) while(1);


    enable_gpio_c_port();
    configure_led_pin();

    while(1)
    {
        int i = 0;
        set_led_pin_val(PIN_LOW);
        for(i=0; i<0x40000; i++);
        set_led_pin_val(PIN_HIGH);
        for(i=0; i<0x40000; i++);
    }
    
    return 0;
}