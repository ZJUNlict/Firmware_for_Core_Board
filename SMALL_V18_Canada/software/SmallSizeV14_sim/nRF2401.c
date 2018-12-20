#include "nRF2401.h"

static unsigned int nRF2401_reg_io = 0;
static unsigned int nRF2401_reg_out = 0;
extern robot_t g_robot;
nRF2401 nRF2401_dev;
unsigned char g_2401_address_flag=0;
char receive_flag;


/*---------function headers----------*/
void Init2401Tx( RFConfig  );
void Init2401Rx( RFConfig  );
void Tra_Rf_Write( unsigned char b );
unsigned char Tra_Rf_Read( void );

int Tra_ReceivePacket( nRF2401 *dev );
unsigned char Tra_TransmitPacket( unsigned char *p_read,unsigned int data_len,RFConfig tconf);


void SetCLK1Dir(unsigned char val);  // 0 for input , 1 for output
void SetCLK1(unsigned char val);

void SetDATADir(unsigned char val);  // 0 for input , 1 for output
void Set_DATA(unsigned char val);
unsigned char GetDATA(void);
unsigned char GetDR1(void);

void set_config_mode(void);
void end_config_mode(void);
void set_a_bit_to(char txrx); 
void set_a_bit_receive(void);
void set_a_bit_trans(void);



/*-----------------------------------------------------------------
 *  �������ƣ� init_nRF2401_dev_rx
 *  ���������� 2401ģ��Ľ���ģʽ��ʼ��
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 *  ��ע:      �����ֵ��趨����<<PTR4000>>�ĵ�
 -----------------------------------------------------------------*/ 
int init_nRF2401_dev_rx()
{
	//TODO :: Add initial nRF2401 device code here
	unsigned char temp;
	
	nRF2401_dev.RFconf.addr_count = NRF2401_ADDR_COUNT;
	nRF2401_dev.RFconf.buf[0] = PACKET_LEN * 8;
	nRF2401_dev.RFconf.buf[1] = PACKET_LEN * 8;

	nRF2401_dev.RFconf.buf[2] = NRF2401_CAR_4;
	nRF2401_dev.RFconf.buf[3] = NRF2401_CAR_3;
	nRF2401_dev.RFconf.buf[4] = NRF2401_CAR_2;
	nRF2401_dev.RFconf.buf[5] = NRF2401_CAR_1;
	nRF2401_dev.RFconf.buf[6] = NRF2401_CAR_0;

	nRF2401_dev.RFconf.buf[7] = NRF2401_ADDR1_4;
	nRF2401_dev.RFconf.buf[8] = NRF2401_ADDR1_3;
	nRF2401_dev.RFconf.buf[9] = NRF2401_ADDR1_2;
	nRF2401_dev.RFconf.buf[10] = NRF2401_ADDR1_1;
	nRF2401_dev.RFconf.buf[11] = NRF2401_ADDR1_0;

	temp = 0;
	
	temp = temp | (0x03);   //16bit CRC enable

	temp = temp + ((NRF2401_ADDR_COUNT * 8) << 2);
	nRF2401_dev.RFconf.buf[12] = temp;

	temp = g_robot.frq;
    
	temp = (temp << 1) + 1;//
    
	
	nRF2401_dev.RFconf.buf[14] = temp; // set nRF2401's RF frequency, and tx mode

	nRF2401_reg_io = 0;
	nRF2401_reg_out = 0;
	IOWR(PIO_NF2401_INOUT_BASE,1,1);

	Init2401Rx(nRF2401_dev.RFconf);

	return 0;
}


/*-----------------------------------------------------------------
 *  �������ƣ� init_nRF2401_dev_tx
 *  ���������� 2401ģ��ķ���ģʽ��ʼ��
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 *  ��ע:      �����ֵ��趨����<<PTR4000>>�ĵ�
 -----------------------------------------------------------------*/ 
int init_nRF2401_dev_tx()
{
	//TODO :: Add initial nRF2401 device code here
	unsigned char temp;
	
	nRF2401_dev.RFconf.addr_count = NRF2401_ADDR_COUNT;
	nRF2401_dev.RFconf.buf[0] = (PACKET_LEN_UP) * 8;
	nRF2401_dev.RFconf.buf[1] = (PACKET_LEN_UP) * 8;

	nRF2401_dev.RFconf.buf[2] = NRF2401_CAR_4;
	nRF2401_dev.RFconf.buf[3] = NRF2401_CAR_3;
	nRF2401_dev.RFconf.buf[4] = NRF2401_CAR_2;
	nRF2401_dev.RFconf.buf[5] = NRF2401_CAR_1;
	nRF2401_dev.RFconf.buf[6] = NRF2401_CAR_0;

	nRF2401_dev.RFconf.buf[7] = NRF2401_ADDR1_4;
	nRF2401_dev.RFconf.buf[8] = NRF2401_ADDR1_3;
	nRF2401_dev.RFconf.buf[9] = NRF2401_ADDR1_2;
	nRF2401_dev.RFconf.buf[10] = NRF2401_ADDR1_1;
	nRF2401_dev.RFconf.buf[11] = NRF2401_ADDR1_0;

	temp = 0;

	temp = temp | (0x03);   //16bit CRC enable

	temp = temp + ((NRF2401_ADDR_COUNT * 8) << 2);
	nRF2401_dev.RFconf.buf[12] = temp;

	temp = g_robot.frq; //+ 2; 
    
	temp = (temp << 1) + 0;       


	nRF2401_dev.RFconf.buf[14] = temp; // set nRF2401's RF frequency, and tx mode

	nRF2401_reg_io = 0;
  	nRF2401_reg_out = 0;

  	Init2401Tx(nRF2401_dev.RFconf);

	return 0;
}


/*----------------------------------------------------------------------------
 *  �������ƣ� get_nRF2401_packet
 *  ���������� ��2401�л�ȡһ�����ݣ� ��Tra_ReceivePacket������ȫ��ͬ
 *             ֱ�ӵ�����Tra_ReceivePacket������������Ϊ����send_nRF2401_packet
 *             ��ʽ��ͳһ
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 ----------------------------------------------------------------------------*/ 
int get_nRF2401_packet(nRF2401 *dev)
{
	//TODO :: Add get a packet from nRF2401 device code here

	return (Tra_ReceivePacket(dev));
}



/*-----------------------------------------------------------------
 *  �������ƣ� send_nRF2401_packet
 *  ���������� ��2401ģ�鷢��һ�����ݣ�ʹ�䷢�͡��������Ƚ�2401����
 *             ��Ϊ����ģʽ������2401��������ʹ�䷢������
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int send_nRF2401_packet(nRF2401 *dev)
{
	//TODO :: Add send a packet to nRF2401 device code here
	unsigned char r;
	set_a_bit_trans();

	r = Tra_TransmitPacket(dev->buf.buf,dev->buf.pos,dev->RFconf);

	receive_flag=0;
	return r;
}


/*-----------------------------------------------------------------
 *  �������ƣ�Init2401Tx( RFConfig tconf )
 *  ����������NRF2401���Ͷ˳�ʼ��
 *
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
-----------------------------------------------------------------*/ 
void Init2401Tx( RFConfig tconf )
{
	unsigned char i;

	SetPowerUp(1);
	SetCE( 0 );
	SetCS( 1 );
	delay(1);
	for( i = 0; i < tconf.n; i++ )
	{
		Tra_Rf_Write( tconf.buf[i] );
	}
	SetCS( 0 );
 	SetCE( 1 );
	delay(1);
}


/*-----------------------------------------------------------------
 *  �������ƣ�Init2401Rx( RFConfig rconf )
 *  ����������NRF2401���ܶ˳�ʼ��
 *
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
-----------------------------------------------------------------*/ 
void Init2401Rx( RFConfig rconf )
{
	unsigned char i;

	SetPowerUp(1);
	SetCE( 0 );
	SetCS( 1 );
	delay(1);


	for( i = 0; i < rconf.n; i++ )
	{
		Tra_Rf_Write( rconf.buf[i] );
	}

	SetCS( 0 );
	SetCE( 1 );
	delay(1);

}


/*-----------------------------------------------------------------
 *  �������ƣ�void Tra_Rf_Write( unsigned char b )
 *  ��������������Ƶ��Ԫд����//дһ���ֽ�
 *
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
-----------------------------------------------------------------*/ 
void Tra_Rf_Write( unsigned char b )
{
	unsigned char i;
	alt_irq_context context;

	SetCLK1Dir(1);
	SetDATADir(1);
	for( i = 0; i < 8; i++ )
	{
		context =  alt_irq_disable_all (); // We must make the transfer that don't be interrupted
    	SetCLK1( 0 );
		if( ( b & 0x80 ) == 0x00 )
			Set_DATA( 0 );
		else
			Set_DATA( 1 );
		b <<= 1;
		SetCLK1( 1 );
   		alt_irq_enable_all(context);
	}
	SetCLK1( 0 );
}


/*-------------------------------------------------------------------
 *	��������unsigned char Tra_Rf_Read(void)
 *	���룺��
 *	�������������
 *	��������������Ƶ�˿ڶ����ݣ�//��һ���ֽ�
---------------------------------------------------------------------*/
unsigned char Tra_Rf_Read( void )
{
	unsigned char b;
	unsigned char i;
	unsigned temp;
  	alt_irq_context context;

	SetCLK1Dir(1);
	SetDATADir(0);
	b = 0;
	context =  alt_irq_disable_all ();	
	for( i = 0; i < 8; i++ )
	{	
    	b <<= 1;
		SetCLK1( 0 );
		SetCLK1( 1 );
		temp = GetDATA();
		if( temp == 1 )
		b += 0x01;
	}
	alt_irq_enable_all(context);
	SetCLK1( 0 );
	return b;
}



/*-----------------------------------------------------------------
 *  �������ƣ� Tra_ReceivePacket
 *  ���������� ��2401ģ���ж�ȡһ����
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int Tra_ReceivePacket( nRF2401 *dev)//��һ����
{
	//unsigned char i;
	//SetCE(0);
	 
	dev->buf.pos=0;
	while ( GetDR1() )
	{ 
	// led_on( 0 );     //������������� ��һ����2401��Ҫ����ʱ�䡣
		dev->buf.buf[dev->buf.pos] = Tra_Rf_Read();
		dev->buf.pos++;
		if (dev->buf.pos == dev->buf.len)  //Though buffer is full, we MUST read the rest data 
		{
//			dev->packet_error++;
			while ( GetDR1())
			{
				Tra_Rf_Read();
			}
			return -1;
		}
	}
	SetCE( 1 );
  //led_off( 0 );
  	if(dev->buf.pos>0) return 1;
	else  return 0;
}



/*-----------------------------------------------------------------
 *  �������ƣ� Tra_TransmitPacket
 *  ���������� ��2401ģ�鷢��һ������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
unsigned char Tra_TransmitPacket( unsigned char *p_read,unsigned int data_len,RFConfig tconf)
{
	unsigned char k;

	SetCE( 1 );
	for( k = 0; k < tconf.addr_count; k++ )
	{
		Tra_Rf_Write(tconf.buf[12-tconf.addr_count+ k]+0x11);
	}

	for( k = 0; k < data_len ; k++ )
	{
		Tra_Rf_Write( p_read[k] );
	}
	SetCE(0);

	return k;
}


/*-----------------------------------------------------------------
 *  �������ƣ� SetCE
 *  ���������� ���������ֵ��CE��������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void SetCE(unsigned char val)
{
  	if (val == 0)
		nRF2401_reg_out = nRF2401_reg_out & (~0x2);
	else 
		nRF2401_reg_out = nRF2401_reg_out | 0x2;
	
 	 IOWR(PIO_NF2401_OUT_BASE,0,nRF2401_reg_out);
	delay(0);
}




/*-----------------------------------------------------------------
 *  �������ƣ� SetCS
 *  ���������� ���������ֵ��CS��������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void SetCS(unsigned char val)
{
  	if (val == 0)
		nRF2401_reg_out = nRF2401_reg_out & (~0x1);
  	else 
		nRF2401_reg_out = nRF2401_reg_out | 0x1;

	IOWR(PIO_NF2401_OUT_BASE,0,nRF2401_reg_out);
	delay(0);

}


/*-----------------------------------------------------------------
 *  �������ƣ� SetPowerUp
 *  ���������� ���������ֵ��PWR��������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/
void SetPowerUp(unsigned char val)
{
  	if (val == 0) 
		nRF2401_reg_out = nRF2401_reg_out & (~0x4);
  	else 
		nRF2401_reg_out = nRF2401_reg_out | 0x4;

	IOWR(PIO_NF2401_OUT_BASE,0,nRF2401_reg_out);

}



/*-----------------------------------------------------------------
 *  �������ƣ� SetCLK1Dir
 *  ���������� ���������ֵ��CLK1�ķ����������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/
void SetCLK1Dir(unsigned char val)  // 0 for input , 1 for output
{
	
	unsigned int temp;
  
  	temp = IORD(PIO_NF2401_INOUT_BASE,1);
  	if (val == 0) 
		temp = temp & (~0x2);
	else 
		temp = temp | 0x2;
  	IOWR(PIO_NF2401_INOUT_BASE,1,temp);
 }


/*-----------------------------------------------------------------
 *  �������ƣ� SetCLK1
 *  ���������� ���������ֵ��CLK1��������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void SetCLK1(unsigned char val)
{
	
  	if (val == 0) 
		nRF2401_reg_io = nRF2401_reg_io & (~0x2);
  	else 
		nRF2401_reg_io = nRF2401_reg_io | 0x2;
  	IOWR(PIO_NF2401_INOUT_BASE,0,nRF2401_reg_io);
	delay(0);

}


/*-----------------------------------------------------------------
 *  �������ƣ� SetDATADir
 *  ���������� ���������ֵ��DATA�ķ����������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void SetDATADir(unsigned char val)  // 0 for input , 1 for output
{
	
  	unsigned int temp;
  
  	temp = IORD(PIO_NF2401_INOUT_BASE,1);
  	if (val == 0) temp = temp & (~0x1);
  	else temp = temp | 0x1;
  	IOWR(PIO_NF2401_INOUT_BASE,1,temp);

}



/*-----------------------------------------------------------------
 *  �������ƣ� Set_DATA
 *  ���������� ���������ֵ��DATA��ֵ��������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void Set_DATA(unsigned char val)
{

  	if (val == 0) 
		nRF2401_reg_io = nRF2401_reg_io & (~0x1);
  	else 
		nRF2401_reg_io = nRF2401_reg_io | 0x1;
  	IOWR(PIO_NF2401_INOUT_BASE,0,nRF2401_reg_io);
	delay(0);
}



/*-----------------------------------------------------------------
 *  �������ƣ� GetDATA
 *  ���������� ��ȡDATA����ֵ
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
unsigned char GetDATA(void)
{
  	unsigned int temp;
  
  	temp = IORD(PIO_NF2401_INOUT_BASE,0);
  	temp = temp & 0x1;
  	if ( temp == 0 ) return 0;
  	else return 1; 
}



/*-----------------------------------------------------------------
 *  �������ƣ� GetDR1
 *  ���������� ��ȡDR1����ֵ��һ�������ж�2401�Ƿ��յ�����
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
unsigned char GetDR1(void)
{
  	unsigned int temp;
  
  	temp = IORD(PIO_NF2401_IN_BASE,0);
  	temp = temp & 0x1;
  
  	if ( temp == 0 ) return 0;
  	else return 1;
}



/*-----------------------------------------------------------------
 *  �������ƣ� set_config_mode
 *  ���������� ��2401���ó�Ϊ����ģʽ
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void set_config_mode()
{
	SetCE( 0);
	SetCS( 1);
	usleep(10);
}


/*-----------------------------------------------------------------
 *  �������ƣ� end_config_mode
 *  ���������� ֹͣ����ģʽ����2401���ó�Ϊ�շ�ģʽ
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void end_config_mode()
{
	SetCS(0);
	SetCE(1);
	usleep(300);			//�����������ʱ���һ��һ�£�������
}


/*---------------------------------------------------------------------------------------------
 *  �������ƣ� set_a_bit_to
 *  ���������� 2401�ķ��ͽ����л����������������ֵȷ���л�Ϊ���ͻ��ǽ���
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 *  ��ע:      ��2401������ʼ���������Ժ�ͨ���ı�һ���ֽھͿ��Խ�2401�ڽ��պͷ���ģʽ֮��ת��
 *             ����ת�������Լ�Э�飬����<<PTR4000>>�������
 ---------------------------------------------------------------------------------------------*/ 
void set_a_bit_to(char txrx)  //��ʱ
{
	SetDATADir(1);
	set_config_mode();
	SetCLK1(0);
	Set_DATA( txrx);
	SetCLK1(1);
	SetCLK1(0);
	end_config_mode();
}



/*---------------------------------------------------------------------------------------------
 *  �������ƣ� set_a_bit_receive
 *  ���������� ��2401�л���Ϊ����ģʽ
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 *  ��ע:      ��2401������ʼ���������Ժ�ͨ���ı�һ���ֽھͿ��Խ�2401�ڽ��պͷ���ģʽ֮��ת��
 *             ����ת�������Լ�Э�飬����<<PTR4000>>�������
 ---------------------------------------------------------------------------------------------*/ 

void set_a_bit_receive()
{
	set_a_bit_to(NRF2401_RX);
	set_receive_flag();
}


/*----------------------------------------------------------------------------------------------
 *  �������ƣ� set_a_bit_trans
 *  ���������� ��2401�л���Ϊ����ģʽ
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 *  ��ע:      ��2401������ʼ���������Ժ�ͨ���ı�һ���ֽھͿ��Խ�2401�ڽ��պͷ���ģʽ֮��ת��
 *             ����ת�������Լ�Э�飬����<<PTR4000>>�������
 ----------------------------------------------------------------------------------------------*/ 

void set_a_bit_trans()
{
	set_a_bit_to(NRF2401_TX);
}



/*-----------------------------------------------------------
 *  �������ƣ� set_receive_flag
 *  ���������� ���ɽ��ձ�־λ��λ
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -------------------------------------------------------------*/ 
inline void set_receive_flag()
{
	receive_flag=1;
}


/*-----------------------------------------------------------
 *  �������ƣ� rst_nRF2401
 *  ���������� ����2401
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -------------------------------------------------------------*/ 
void rst_nRF2401()
{
	nRF2401_reg_io = 0;
  	nRF2401_reg_out = 0;
   	IOWR(PIO_NF2401_INOUT_BASE,1,1);

  	Init2401Tx(nRF2401_dev.RFconf);
}



/*-----------------------------------------------------------------
 *  �������ƣ� change_nRF2401_frq
 *  ���������� 2401ģ��Ƶ�����
 *  
 *
 *
 *  ���ߣ�     �
 *  �޸����ڣ� 2013-4-7
 *  ��ע:      �����ֵ��趨����<<PTR4000>>�ĵ�
 -----------------------------------------------------------------*/ 
void change_nRF2401_frq(unsigned char frq)
{
    
	frq = (frq<< 1) + 1;       


	nRF2401_dev.RFconf.buf[14] = frq; // set nRF2401's RF frequency, and tx mode

	nRF2401_reg_io = 0;
  	nRF2401_reg_out = 0;
    IOWR(PIO_NF2401_INOUT_BASE,1,1);

  	Init2401Tx(nRF2401_dev.RFconf);
}

