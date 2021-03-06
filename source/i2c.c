#include "aduc812.h"
#include "i2c.h"

//25 ��� ��������
static void delay( void )
{
char ch = 0;

    while( ch++ < 2 );
}

//������� �� I2C ����� (8 ���) � �������� ������������� (acknowledge).
static __bit send_byte( unsigned char ch ) //Returns ack (0 = acknowledged)
{
char i;
__bit ack;

    MDE = 1;//Output
    MCO = 0;

    for( i = 0 ; i < 8; i++, ch <<= 1 )
    {
        MDO = ( ch & 0x80 ) ? 1 : 0 ;
        MCO = 1;

        delay();

        MCO = 0;
    }

    MDE = 0;//Input
    MCO = 1;

    delay();

    ack = MDI;
    MCO = 0;
    return ack;
}

//��������� ��������� Start ���������� i2c.
static void start(void)
{
    MDE = 1;//Output
    MDO = 1;
    MCO = 1;

    delay();

    MDO = 0;

    delay();

    MCO = 0;
}

//��������� ��������� Stop ���������� i2c.
static void stop(void)
{
    MDE = 1; //Output
    MCO = 0;
    MDO = 0;
    MCO = 1;

    delay();

    MDO = 1;

    delay();

    MDE = 0; //Input (release line)
}


//������ ������ �� ���������� i2c (Start + ������� i2c-������ slave-����������).
static __bit begin( unsigned char addr )//Returns ack (0 = acknowledged)
{
    start();

    return send_byte( addr );
}

//������� ������������� (acknowledge) ����������.
static void ack( void )//Sends ack
{
    MDE = 1; //Output
    MCO = 0;
    MDO = 0;
    MCO = 1;

    delay();

    MCO = 0;
}

//������� "���������������" (not acknowledged) ����������.
static void nack(void) //Sends NAck
{
    MDE = 1;
    MCO = 0;
    MDO = 1;
    MCO = 1;

    delay();

    MCO = 0;
}

//�������� �� ���������� slave-���������� � ������ (������ + ��������� ������� +
//���������� ������)
__bit get_ack( unsigned char address ) //Returns 1 if there was an ACK
{
    I2CM = 1; //I2C Master mode

    if( begin( address & 0xFE ) ) 
    { 
        stop(); 
        return 0;
    }

    stop();

    return 1;
}

//��������� 8 ��� � ���� ������ i2c ��� ������������� ��� ��������������� ������.
unsigned char recv_byte(void)
{
char i;
unsigned char ch = 0;

    MDE = 0; //Input
    MCO = 0;

    for( i = 0; i < 8; i++ )
    {
        ch  <<= 1;
        MCO   = 1;

        delay();

        ch   |= MDI;

        MCO   = 0;
    }

    return ch;
}

//��������� ����� ������ �� i2c-���������� � 8-��������� ���������� �������� 
//�������������. �������� ���� ���������� � ������� ������ __xdata.
__bit receive_block(unsigned char address, unsigned char addr, unsigned char __xdata * block,unsigned char len)
{ //addr - address in target
unsigned char i, ch;

    I2CM    = 1;                    // I2C Master mode
    address = ( address & 0xFE );   // Write

    if( begin( address ) ) 
    {
        stop();
        return 1;
    }   //Error - No ACK

    if( send_byte( addr ) )
    {
        stop();
        return 1;
    }

    delay();
    delay();

    address |= 1;  //Read

    if( begin( address ) ) 
    {
        stop();
        return 1;
    }

    delay();

    if( len-1 )
    {
        for( i = 0; i< ( len - 1 ) ;i++)
        {
            ch = recv_byte();
            ack();

            *block++ = ch;
        }
    }

    ch = recv_byte();
    nack();

    *block = ch;
    stop();

    return 0;
}

//������ ����� ������ �� ���������� ������ i2c-���������� � 8-��������� �������� �������������.
__bit transmit_block(unsigned char address, unsigned char addr, unsigned char __xdata * block,unsigned char len)
{ //addr - address in target
unsigned char ch, i;

    I2CM    = 1;                //I2C Master mode
    address = address & 0xFE;   //Write

    if( begin( address ) ) 
    {
        stop();
        return 1;
    }   //Error - no Ack

    if( send_byte( addr ) ) 
    {
        stop();
        return 1;
    }

    for( i = 0; i < len; i++,block++)
    {
        ch = *block;
        if( send_byte( ch ) )
        { 
            stop(); 
            return 1;
        }   //Not to the end of the block
    }

    stop();

    return 0;
}