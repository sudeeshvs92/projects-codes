#include<pic.h>
#include<htc.h>
#include<stdio.h>
#define _XTAL_FREQ 40000000
#define RS RD2
#define EN RD1
#define RW RD3
#define DATA PORTD
#define LOCK RC2
#define BUZZ RC5
#define START RC1 
#define SB RB1 
#define I2C_SPEED	100	


unsigned  char lat[12];
unsigned  char lan[12];
unsigned int f=0;
unsigned int j=0;
unsigned int pos=0;
unsigned int comma=0;
unsigned int gpse=0;
const char ph[]="+918129910401";
unsigned  int flagr=0;
unsigned  int sensor=0;
unsigned int rpm=0;
unsigned int tcout=0;
int temper;
unsigned int fuel;
unsigned int collition;
unsigned int gpsrec=0;
unsigned int finish=1;
unsigned int rev=0;
unsigned int i=0;
unsigned int rpmp=0;
unsigned int sec=0;
unsigned int gpsiva=0;
char RXd;



	
void lcd_data(char d);
void lcd_cammand(char c);
void lcd_init();
void lcd(char *p);

void tx_rx_init();
void txdata(char Tx);
void txchar(char *t);

void acd_init(void);
unsigned int adc_read(int ch);

void InitI2C(void);
void I2C_Start(void);
void I2C_ReStart(void);
void I2C_Stop(void);
void I2C_Send_ACK(void);
void I2C_Send_NACK(void);
bit  I2C_Write_Byte(unsigned char);
unsigned char I2C_Read_Byte(void);

void Write_Byte_EEPROM(unsigned int, unsigned char);
unsigned char Read_Byte_EEPROM(unsigned int);
unsigned int slen(unsigned char *ch);

void gsm_str(char *gc);
void gsm_init(void);
void gsm_msg(char *msg);



void main()
{
int t;
int gpsav=0;
int acc=0;
int gpsesize;
int Address;
char str[30];
unsigned char EBread='\0';
unsigned int lk=0;
unsigned int on=0;


TRISD=0X00;
TRISC6=0;
TRISC5=0;
TRISC7=1;
TRISB1=1;
TRISB0=1;
TRISB2=0;

RB2=0;
BUZZ=0;

GIE=0;
PEIE=1;
INTE=1;
INTF=0;
TMR1IE=1;
TMR1IF=0;
RCIE= 1;
RCIF=0;

TMR1H =7;           
TMR1L =207;                    
T1CON=0X30;

lcd_init();
InitI2C();
tx_rx_init();
acd_init();
gsm_init();

lcd_cammand(0X80);
lcd("BLACK BOX");
lcd_cammand(0XC0);
lcd("KELTRON");
__delay_ms(10);
lcd_cammand(0x01);
CREN=0;
GIE=1;

while(1)
{

if(LOCK==1&&lk==0)
{
while(LOCK);
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("DOOR LOCK..!");
lcd_cammand(0xC0);
lcd("ENABLED..!");
gsm_msg("DOOR LOCK ENABLED..!");
lk=1;
__delay_ms(10);
lcd_cammand(0x01);
}
else if(LOCK==1&&lk==1)
{
while(LOCK);
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("DOOR LOCK..!");
lcd_cammand(0xC0);
lk=0;

gsm_msg("WARNING..!,DOOR LOCK DISABLED..!");
lcd("DISABLED..!");

__delay_ms(10);
lcd_cammand(0x01);
}

if(START==1&&on==0)
{
BUZZ=1;
while(START);
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("ENGIN");
lcd_cammand(0xC0);
lcd("STARTING..!");
gsm_msg("engin started..");
on=1;
BUZZ=0;
lcd_cammand(0x01);
}
else if(START==1&&on==1)
{
while(START);
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("ENGIN");
lcd_cammand(0xC0);
on=0;
lcd("STOP...!");

lcd_cammand(0x01);
}



if(lk==1)
{
if(SB==0)
{
BUZZ=1;
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("WARNING..!");
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("DOOR");
lcd_cammand(0xC0);
lcd("OPEN..");
gsm_msg("WARNING..!,DOOR OPEN ,LOCK ENABLED..!");
__delay_ms(60);

lcd_cammand(0x01);
}
}
if(on==0)
{
CREN=1;
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("ENGIN OFF..!");
if(rev==1)
{
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("data recovery");
i=0;
Address=0x00;
gpsesize=Read_Byte_EEPROM(Address);
Address=0x02;
while(i<=gpsesize+3)
{
EBread=Read_Byte_EEPROM(Address);
txdata(EBread);

Address++;
i++;
}

rev=0;
}
}

if(on==1)
{
CREN=0;

if(SB==0)
{
BUZZ=1;
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("WARNING..!");
lcd_cammand(0x01);
lcd_cammand(0x80);
lcd("DOOR");
lcd_cammand(0xC0);
lcd("OPEN..");
__delay_ms(60);
lcd_cammand(0x01);
}
else
{
BUZZ=0;
}

if(flagr==1)
{
fuel=adc_read(0);
lcd_cammand(0x80);
lcd("FU:");
if(fuel>0 && fuel<100)
{

lcd("0  %");
gsm_msg(" FUEL EMPTY ..!,REFILL IMMEDIATELY.. ");
BUZZ=1;
}
else if(fuel>100 && fuel <200)
{

lcd("15 %");
gsm_msg("RUNING OUT OF FUEL LESS THAN 15%..");
BUZZ=1;
__delay_ms(10);
BUZZ=0;
}
else if(fuel>200 && fuel <400)
{

lcd("35 %");
}
else if(fuel>400 && fuel <600)
{
lcd("65 %");
}
else if(fuel>600 && fuel <1000)
{
lcd("85 %");
}
else if(fuel>1000)
{
lcd("100%");
}
temper=adc_read(1);
t=temper+5;
lcd_cammand(0X88);
lcd("TEM:");
sprintf(str,"%dc  ",t);
lcd(str);
collition=adc_read(2);
if(collition>500)
{
acc=1;
}
else
{
acc=0;
}
if(rpm>300)
{
BUZZ=1;
lcd_cammand(0x01);
lcd_cammand(0X80);
lcd("WARNING..!");
lcd_cammand(0XC0);
lcd("OVER SPEED..");
__delay_ms(20);
BUZZ=0;
}

lcd_cammand(0Xc2);
lcd("RPM:");
sprintf(str,"%d  ",rpmp);
lcd_cammand(0XC9);
lcd(str);
}

if(sensor==1)
{
sensor=0;
}


}




if(acc==1 && on==1)
{
CREN=1;
gpse=1;
flagr=0;
on==0;
BUZZ=1;
lcd_cammand(0x01);
lcd_cammand(0X82);
lcd("ACCIDENT..!");
gsm_msg("ACCIDENT OCCURE");
while(gpsav!=1)
{
gpse=1;
lcd_cammand(0XC0);
lcd("WAITING GPS.!");
if(gpsrec==1)
{
if(gpsiva==0)
{
gpsav=1;

}
else if(gpsiva==1 )
{
gpsav=0;
gpsiva=0;
}
gpsrec=0;

}
}
lcd_cammand(0x01);
lcd_cammand(0X82);
lcd("LOCATION SEND.");
sprintf(str,"SOS,Latitude:%s,Longitude:%s",lat,lan);
gsm_msg(str);

i=0;
Address=0x02;
gpsesize=0;
temper=adc_read(1);
t=temper*0.49;
sprintf(str,"$temp:%d,",t);
gpsesize=gpsesize+slen(str);
while(i<=slen(str))
{
Write_Byte_EEPROM(Address,str[i]);
Address++;
i++;

}
fuel=adc_read(0);
sprintf(str,"fuel:%d,",fuel);
i=0;
while(i<=slen(str))
{
Write_Byte_EEPROM(Address,str[i]);
Address++;
i++;
}
gpsesize=gpsesize+slen(str);
collition=adc_read(2);
sprintf(str,"collision:%d,",collition);
i=0;
while(i<=slen(str))
{
Write_Byte_EEPROM(Address,str[i]);
Address++;
i++;
}
gpsesize=gpsesize+slen(str);
sprintf(str,"door:%d*",SB);
i=0;
while(i<=slen(str))
{
Write_Byte_EEPROM(Address,str[i]);
Address++;
i++;
}
gpsesize=gpsesize+slen(str);
Address=0x00;
Write_Byte_EEPROM(Address,gpsesize);
sprintf(str,"%d,",gpsesize);
on=0;
acc=0;
}

}
}





void lcd_data(char d)
{

DATA=d;
RW=0;
RS=1;
EN=1;
__delay_ms(2);
EN=0;
__delay_ms(2);
DATA=d<<4;
RW=0;
RS=1;
EN=1;
__delay_ms(2);
EN=0;

}
void lcd_cammand(char c)
{

DATA=c;
RW=0;
RS=0;
EN=1;
__delay_ms(2);
EN=0;
__delay_ms(2);
DATA=c<<4;
RW=0;
RS=0;

EN=1;
__delay_ms(2);
EN=0;
}
void lcd_init()
{
lcd_cammand(0X30);
lcd_cammand(0X30);
lcd_cammand(0X30);
lcd_cammand(0X20);
EN=1;
__delay_ms(2);
EN=0;

lcd_cammand(0X28);
lcd_cammand(0X0C);
lcd_cammand(0X06);
lcd_cammand(0X01);

}
void lcd(char *p)
{
while(*p!='\0')
{
lcd_data(*p);
*p++;
__delay_ms(2);
}
}

void tx_rx_init()
{
TXSTA=0X24;
SPBRG=25;
RCSTA=0X90;
RCIE=1;
CREN=0;
}

void txdata(char Tx)
{
SPEN=1;
TXEN=0;
TXREG=Tx;
__delay_ms(2);
TXEN=1;
while(!TRMT);
}

void txchar(char *t)
{

while(*t!='\0')
{
txdata(*t);
*t++;
}
}

void acd_init(void)
{

	ADCON1  = 0x80;	     
						 
	TRISA   = 0x2f;      
	TRISE   = 0x07;		 
	ADCON0  = 0x81;		 
}
unsigned int adc_read(int ch)
{   
   	ADCON0 &= 0xc7;        
	ADCON0 |= (ch<<3); 
    
    __delay_ms(10);         
							
	GO_nDONE  = 1;		    

	while(GO_nDONE);       
	return ((ADRESH<<8)+ADRESL);   
}

void InitI2C(void)
{	
    TRISC4= 1;	
	TRISC3= 1;		
	SSPADD  = ((_XTAL_FREQ/4000)/I2C_SPEED) - 1;	
	SSPSTAT = 0x80;		
	SSPCON  = 0x28;		
}

void I2C_Start(void)
{
	SEN = 1;			
	while(!SSPIF);		
	SSPIF = 0;			

}

void I2C_ReStart(void)
{
	RSEN = 1;			
	while(!SSPIF);		
	SSPIF = 0;			

}

void I2C_Stop(void)
{
	PEN = 1;		
	while(!SSPIF);		
	SSPIF = 0;		
}

void I2C_Send_ACK(void)
{
	ACKDT = 0;		
	ACKEN = 1;			
	while(!SSPIF);	
	SSPIF = 0;			
}


void I2C_Send_NACK(void)
{
	ACKDT = 1;			
	ACKEN = 1;		
	while(!SSPIF);	
	SSPIF = 0;	
}



bit I2C_Write_Byte(unsigned char Byte)
{
	SSPBUF = Byte;	
	while(!SSPIF);	
	SSPIF = 0;		

	return ACKSTAT;	
}

unsigned char I2C_Read_Byte(void)
{
	RCEN = 1;		
	while(!SSPIF);	
	SSPIF = 0;		
    return SSPBUF;		
}


void Write_Byte_EEPROM(unsigned int Address, unsigned char DataByte)
{
	I2C_Start();										
	while(I2C_Write_Byte(0XA0) == 1)
{	
    I2C_ReStart();	
}		
	I2C_Write_Byte(Address);							
	I2C_Write_Byte(DataByte);						
	I2C_Stop();										
}


unsigned char Read_Byte_EEPROM(unsigned int Address)
{
	unsigned char Byte = 0;							
	I2C_Start();										


	while(I2C_Write_Byte(0xA0 ) == 1)
	{	I2C_ReStart();	}		

	I2C_Write_Byte(Address);						
	I2C_ReStart();									


	I2C_Write_Byte(0XA1);		

	Byte = I2C_Read_Byte();							

	I2C_Send_NACK();									
	I2C_Stop();										

	return Byte;			
}

unsigned int slen(unsigned char *ch)
{
int l=0;

while(*ch!='\0')
{
l++;
*ch++;
}
return l-1;
}
void static interrupt isr()
{

if(INTF)
{
if(flagr==0)
{
sensor=0;
flagr=1;
rpm=0;
TMR1ON=1;
INTF=0;
}
else
{
rpm++;
INTF=0;
}
}
if(TMR1IF==1)
{

TMR1H=7;
TMR1L=207;
TMR1IF=0;
if(tcout==118)
{

rpm=0;
tcout=0;
flagr=0;
TMR1ON=0;

}
if(sec==2)
{
RB2=~RB2;
sec=0;
rpmp=rpm*60;
sensor=1;
rpm=0;
}
tcout++;
sec++;


}
while(RCIF==1)
{
if(FERR||OERR)
{
FERR=0;
OERR=0;
RCIF=0;
}
else
{
RCIF=0;
RXd=RCREG;
if(RXd=='$' )
{
if(gpsrec==0 )
{
f=0;
j=0;
pos=1;
comma=0;
finish=0;
gpsiva=0;
}
}
else if(RXd=='@')
{
rev=1;
}
else if(RXd=='G' )
{
pos=2;
}
else if(RXd=='p' )
{
pos=3;
}
else if(RXd=='R' )
{
pos=4;
}
else if(RXd=='M' )
{
pos=5;
}
else if(RXd=='C' )
{
pos=6;
}
else if(pos==6 && RXd==',')
{
comma++;
}
if(comma==2 && RXd=='V')
{
gpsiva=1;
}
if(comma==3)
{
if(RXd!=',')
{ 
lat[f++]=RXd;
}
} 
if(comma==5 )
{
if(RXd!=',')
{
lan[j++]=RXd;
}
}
if(comma==6)
{
comma=0;
pos=0;
f=0;
j=0;
finish=1;
gpsrec=1;

}

}
}
}
void gsm_str(char *gc)
{
  
__delay_ms(2);
while(*gc!='\0')
{
  txdata(*gc);	
__delay_ms(2);
	*gc++;
}	
}
void gsm_init(void)
{
	gsm_str("AT");
	gsm_str("\n\r");
 __delay_ms(100);
	gsm_str("ATE0");
	gsm_str("\n\r");
 __delay_ms(100);
	gsm_str("AT+CMGF=1");
	gsm_str("\n\r");
 __delay_ms(100);
	
}	
void gsm_msg(char *msg)
{


	gsm_str("AT+CMGS=\"");
    gsm_str(ph);
    txdata('"');
    __delay_ms(100);
    gsm_str(msg);
    txdata(0x1A);
}
						