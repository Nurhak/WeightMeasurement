
#include "msp430.h"
#include "lcd_msp.h"

unsigned int sicaklik=0;
volatile char tick;

void agirlik_init(void);
void deger_goster(unsigned int,unsigned int);
void agirlik_oku(void);
void integer_yaz(unsigned int);

void main( void )
{
  WDTCTL = WDTPW + WDTHOLD;
  
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
  __delay_cycles(100000);
  
  TA0CTL = TASSEL_2 + TACLR;
  TA0CCR0 = 50000-1;
  TA0CCTL0 = CCIE;
  
  lcd_init();
  agirlik_init();
  
  lcd_goto(1,2);
  lcd_puts("Suat Fatih KOCYIGIT");
  lcd_goto(2,2);
  lcd_puts("LOAD CELL");
  delay_ms(2000);
  lcd_temizle();
  
  TA0CTL |= MC_1; //Timer Başlatılıyor...
  
  __bis_SR_register(LPM0_bits + GIE);
}

// TimerA0 Kesme Vektörü
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
  tick++;
  if(tick==10)
  {
    agirlik_oku();
    tick = 0;
  }
}

// ADC10 Kesme Vektörü
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

void deger_goster(unsigned int ham,unsigned int ham_s)
{
  lcd_goto(1,1);
  lcd_puts("10 Bit = ");
  integer_yaz(ham);
  lcd_goto(2,1);
  lcd_puts("Sonuc = ");
  integer_yaz(ham_s);
} 

void agirlik_init()
{
  ADC10CTL0 &= ~ENC;
  ADC10AE0  |= 0x01;        
  ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + REF2_5V + ADC10ON + MSC + ADC10IE;  
  ADC10CTL1 = INCH_0 + ADC10SSEL_0 + ADC10DIV_0 + CONSEQ_2;
  ADC10DTC1 = 32;
  __delay_cycles(256);
}

void agirlik_oku()
{
  char i;
  float yeni=0;
  float agirlik_ham,agirlik;
  unsigned int toplam=0;
  unsigned int dma_buf[32];
  
  ADC10SA = (unsigned int)dma_buf;
  ADC10CTL0 |= ENC + ADC10SC;        
  __bis_SR_register(CPUOFF + GIE);    
  
  for(i=0;i<32;i++)
    toplam+=dma_buf[i];
  
  toplam >>= 5;
  
  agirlik_ham =(unsigned int) toplam;
  yeni=(agirlik_ham-168)*5.8411;
  agirlik=(unsigned int)yeni;
  
  deger_goster(agirlik_ham,agirlik);
}

void integer_yaz(unsigned int deger)
{
  lcd_putch( deger/1000+48);
  lcd_putch((deger%1000)/100+48);
  lcd_putch((deger%100)/10+48);
  lcd_putch((deger)%10+48);
}
