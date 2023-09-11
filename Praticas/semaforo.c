#include <msp430g2553.h>

#define S1_VERMELHO     BIT4
#define S2_VERMELHO     BIT0

#define S1_VERDE    BIT6
#define S2_VERDE    BIT2

#define S1_AMARELO      BIT5
#define S2_AMARELO      BIT1

static int STATE = 0;
void INIT_PORTS(void);
void SET_PORTS(void);
void  SET_TIMER0(void);
void SET_TIMER1(void);
void SET_CLOCK(void);



void main(void) {
    SET_CLOCK();
    INIT_PORTS();
    SET_TIMER0();
    SET_TIMER1();
    do {

        } while(1);


}



void INIT_PORTS(void) { //FUNÇÃO PARA INICIALIZAÇÃO DAS PORTAS

P1DIR |= 0xFF ;
P1OUT =  S1_VERDE ;
P2DIR |= 0xFF;
P2OUT =  S2_VERMELHO + S2_AMARELO ;

//BIT2 COMO ENTRADA COM RESISTOR PULL UP
P1DIR &= ~BIT2;
P1REN |= BIT2;
P1OUT |= BIT2;

P1IFG &= ~BIT2;
P1IES = 1;
P1IE |= BIT2;



}

void SET_CLOCK(){
    WDTCTL = WDTPW | WDTHOLD;

        /*
           OSCILADORES:
           VLO:                 Não utilizado
           LXFT1CLK:            32.768 Hz
           DCOCLK:              ~ 16 MHz (via dados de calibrado do fab.)
           SAÍDAS DE CLOCK:
           ACLK = LFXT1CLK:     32.768 Hz
           MCLK = DCOCLK:       ~ 16 MHz
           SMCLK = DCOCLK/8:    ~ 2 MHz
        */
        DCOCTL = CALDCO_16MHZ;
        BCSCTL1 = CALBC1_16MHZ;
        BCSCTL2 = DIVS0 + DIVS1;
        BCSCTL3 = XCAP0 + XCAP1;

        while(BCSCTL3 & LFXT1OF);

        __enable_interrupt();
}


void SET_TIMER0() {

    TA0CTL = TASSEL_1 + MC_1 + ID_3   + TAIE ;
    TA0CCR1 = 8167; //2 SEGUNDOS
    TA0CCR0 = 40959; //10 SEGUNDIOS
    TA0CCTL0 = CCIE;
    TA0CCTL1 = CCIE;


}
void SET_TIMER1() {

    //AMALERO FASE 4
        TA1CTL =TASSEL_1 + MC_0 + ID_3   + TAIE ;//INICIA PARADO
        TA1CCR1 = 8167;//2 SEGUNDOS
        TA1CCR0 = 32767;//8 SEGUNDOS
        TA1CCTL0 = CCIE;
        TA1CCTL1 = CCIE;
}


#pragma vector=PORT1_VECTOR
__interrupt void RTI_BOTAO(void){
    if(STATE == 0)
    {
        TA0CCTL0 =CCIE + CCIFG ;
    }
    if(STATE == 1)

    {
        TA1CCTL0 =CCIE + CCIFG ;
    }
    P1IFG &= ~BIT2;
    P1IE |= BIT2;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0CCRO_RTI(void){
    STATE = 1;
    P1OUT =  S1_VERMELHO + S1_AMARELO ;
    P2OUT =  S2_VERDE ;
    TA0CTL = TASSEL_1 + MC_0 + ID_3   + TAIE ;
    TA1CTL = TASSEL_1 + MC_1 + ID_3   + TAIE ;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TA0CCR1_R2_RTI(void){
    switch (TA0IV){
            case 2:
                P1OUT =  S1_VERMELHO ;
                P2OUT =  S2_VERDE ;
            case 4:
                break;
            case 10:
                break;
    }
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TA1CCRO_RTI(void){
    STATE = 0;
    P1OUT =  S1_VERDE ;
    P2OUT =  S2_VERMELHO + S2_AMARELO ;

    TA0CTL = TASSEL_1 + MC_1 + ID_3   + TAIE ;
    TA1CTL = TASSEL_1 + MC_0 + ID_3   + TAIE ;

}
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TA1CCR1_R2_RTI(void){
    switch (TA1IV){
            case 2:
                P1OUT =  S1_VERDE ;
                P2OUT =  S2_VERMELHO ;
                break;
            case 4:
                break;
            case 10:
                break;
    }

}
