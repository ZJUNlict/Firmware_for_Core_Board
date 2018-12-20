#include "Pca9557.h"

/*-----------------------------------------------------------------
 *
 *  IO :   0  |  1   |  2   |  3   |  4   |  5   |  6   |  7
 *  0��: num1 | num0 | mod3 | mod2 | mod1 | mod0 | num2 | num3
 *  7��:      |      | frq3 | frq2 | frq1 | frq0 |      |  
 *  ���ӵ�������Ϊ�����
 *
 -----------------------------------------------------------------*/
 
extern robot_t g_robot;


/*---------function headers----------*/
void reset_9557();
void set_data_9557();
void set_clk_9557();
void clr_data_9557();
void clr_clk_9557();
char get_data_9557();
void start_bit9557();
void send_byte(unsigned char data);
int get_acknowledge();
unsigned char get_8bit9557();
void stop_bit9557();
int read_byte_9557(unsigned char addr, char command);
int write_byte_9557(unsigned char addr, unsigned char command, unsigned char data);

/*-----------------------------------------------------------------
 *  function name: init_pca9557
 *  description: ��ʼ��9557��һ����5�����Ի��ᣬÿ������������¶�ȡ
 *				 �Լ����Ƿ����óɹ��������ĸ�������������ø�λ��־
 *				 �������á�����9557�ĵ�ַ�ֱ�Ϊ0��7����Ӳ�����Ӿ���
 *  author:δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
int init_pca9557()
{
	int blood = 6;           //���Ա�־����������ԵĻ���
	int is_to_reset = 1;     //��λ��־
    int temp=0;
	reset_9557();
	while(is_to_reset)
	{
		blood--;		
		if(blood)
		{
			reset_9557();
			if(write_byte_9557(0, CMD_WRITE_CONFIG9557, 0xff) >= 0)	
			{
				if(read_byte_9557(0, CMD_WRITE_CONFIG9557) == 0xff)
				{
					is_to_reset = 0;
				}
				else
				{
					is_to_reset = 1;
					continue;
				}
			}
			else
			{
				is_to_reset = 1;
				continue;
			}
			
			if(write_byte_9557(7, CMD_WRITE_CONFIG9557, 0x3c) >= 0)
			{
				if(read_byte_9557(7, CMD_WRITE_CONFIG9557) == 0x3c)
				{
					is_to_reset = 0;
				}
				else
				{
					is_to_reset = 1;
					continue;
				}
			}
			else
			{
				is_to_reset = 1;
				continue;
			}
			
			if(write_byte_9557(0, CMD_WRITE_POLARITY9557, 0x0) >= 0)
			{
				if(read_byte_9557(0, CMD_WRITE_POLARITY9557) == 0x0)
				{
					is_to_reset = 0;
				}
				else
				{
					is_to_reset = 1;
					continue;
				}
			}
			else
			{
				is_to_reset = 1;
				continue;
			}	
			
			if(write_byte_9557(7, CMD_WRITE_POLARITY9557, 0x0) >= 0)
			{
				if(read_byte_9557(7, CMD_WRITE_POLARITY9557) == 0x0)
				{
					is_to_reset = 0;
				}
				else
				{
					is_to_reset = 1;
					continue;
				}
			}
			else
			{
				is_to_reset = 1;
				continue;
			}

			g_robot.num = (read_byte_9557(0, 0));	
			if(g_robot.num > 0)
			{
				g_robot.mode = ~g_robot.num;
				g_robot.num = ~g_robot.num;
				g_robot.num = ((g_robot.num & 0xc0) >> 6) + ((g_robot.num & 0x1) << 3) + ((g_robot.num & 0x2) << 1);
				g_robot.mode = (g_robot.mode & 0x3c) >>2;
				is_to_reset = 0;
			}
			else
			{
				is_to_reset = 1;
				continue;
			}
			
			g_robot.frq = (read_byte_9557(7, 0));
			if(g_robot.frq >= 0)
			{
				g_robot.frq = ~g_robot.frq;
				
				temp=(g_robot.frq & 0x3c) >> 2;
				if(temp <= 7)					
                {
                	#ifdef FREE_FRQ_SET
                    g_robot.frq = temp << 2;
					#else 
					g_robot.frq = MAN_SET_STARTPOINT + temp * MAN_SET_STEPLENGTH;
					#endif
                }
				else
				{
					#ifdef FREE_FRQ_SET
                    g_robot.frq = (temp << 2) + 58;
 					#else
					g_robot.frq = MAN_SET_STARTPOINT + temp * MAN_SET_STEPLENGTH + 58;
					#endif
                }
				is_to_reset = 0;
			}
			else
			{
				is_to_reset = 1;
				continue;
			}
		}
		else
		{
			g_robot.num = 0;
			g_robot.frq = 1;
			g_robot.mode = SET9557_ERROR_MODE;
			led_off(0);
			led_off(1);
			return -1;
		}
	}
	return 0;
} 


/*-----------------------------------------------------------------
 *  function name: reset_9557
 *  description:  9557�͵�ƽ����һ��ʱ������reset�����reset����
 *                ��������
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void reset_9557()
{
	IOWR(PIO_RESET_9557_BASE, 0, 0);
	delay(130);
	IOWR(PIO_RESET_9557_BASE, 0, 1);
	delay(100);
}


/*-----------------------------------------------------------------
 *  function name: set_data_9557
 *  description:  ����IIC����Ĭ���������������ߣ��������ʱ����
 *                ��IO����Ϊ���룬��������������ߡ�����ߵͱ仯
 *                ����μ�IICͨѶЭ��
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void set_data_9557()
{
	IOWR(PIO_SDA_9557_BASE, 1, 0);
	delay(100);
}


/*-----------------------------------------------------------------
 *  function name: set_clk_9557
 *  description:  ��ʱ���ź�����
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void set_clk_9557()
{
	IOWR(PIO_SCL_9557_BASE, 0, 1);
	delay(100);
}


/*-----------------------------------------------------------------
 *  function name: clr_data_9557
 *  description:  �������ź�����
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void clr_data_9557()
{
	IOWR(PIO_SDA_9557_BASE, 1, 1);
	IOWR(PIO_SDA_9557_BASE, 0, 0);
	delay(100);
}


/*-----------------------------------------------------------------
 *  function name: clr_clk_9557
 *  description:  ��ʱ���ź�����
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void clr_clk_9557()
{
	IOWR(PIO_SCL_9557_BASE, 0, 0);
	delay(100);
}


/*-----------------------------------------------------------------
 *  function name: get_data_9557
 *  description:  �����ݿڶ�ȡһ��λ�����ݲ�����0��1
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
 char get_data_9557()
{
	IOWR(PIO_SDA_9557_BASE, 1, 0);
	delay(100);
	if(IORD(PIO_SDA_9557_BASE, 0) & 0x1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/*-----------------------------------------------------------------
 *  function name: start_bit9557
 *  description:  IICͨѶ��ʼ�źţ�����������IICͨѶ���򣬿�ʼ�ź�
 *				  ��ɺ������ź�����ΪĬ�ϸ���̬
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void start_bit9557()
{
	set_data_9557();
	set_clk_9557();
	clr_data_9557();
	clr_clk_9557();
	set_data_9557();
}


/*-----------------------------------------------------------------
 *  function name: send_byte
 *  description:  ����data�е�һ���ֽڵ����ݣ���λ���ͣ������ƽ�仯
 *				  �������IICͨѶЭ�顣ͨѶ��ɺ�����Ϊ����̬��Ĭ��
 *				  Ϊ����
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void send_byte(unsigned char data)
{
	char i = 0;
	
	 
	for(i=0;i<8;i++)
	{
		if( (data >> (7-i)) & 0x1)
		{
			set_data_9557();
		}
		else
		{
			clr_data_9557();
		}
		set_clk_9557();
		clr_clk_9557();
	}	
	set_data_9557();
}


/*-----------------------------------------------------------------
 *  function name: get_acknowledge
 *  description:  ���������ź��Ƿ�Ϊ��(Ĭ��״̬Ϊ��)�ϴ�оƬ������
 *				  �Ƿ�ɹ�Ӧ�𣬳ɹ�Ӧ���򷵻�1������Ϊ0
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
int get_acknowledge()
{
	set_clk_9557();
   	 
	if(get_data_9557())
	{
		clr_clk_9557();	
 		return 0;
	}
	else
	{
		clr_clk_9557();
 		return 1;
	}
	
}


/*-----------------------------------------------------------------
 *  function name: get_8bit9557
 *  description:  �������źŽ���һ���Լ������ݲ����ء���λ���ա�
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
unsigned char get_8bit9557()
{
	char i = 0;
	unsigned char data = 0;
	for(i=7;i>=0;i--)
	{
		set_clk_9557();
		data += (get_data_9557()) << i;
		clr_clk_9557();
	}
	return data;
}


/*-----------------------------------------------------------------
 *  function name: stop_bit9557
 *  description:  IICͨѶ�����źţ�����������IICͨѶ����
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
void stop_bit9557()
{
	clr_data_9557();
	set_clk_9557();
	set_data_9557();
	clr_clk_9557();
}


/*-----------------------------------------------------------------
 *  function name: read_byte_9557
 *  description:  IICͨѶ��ȡ����ȫ���̣�����������IICͨѶЭ�飬
 *				  �����addrΪ��оƬ��ַ��commandΪ��ȡ������
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
int read_byte_9557(unsigned char addr, char command)
{
	unsigned char data_from_slave = 0;
	start_bit9557();
	send_byte(0x30+(addr<<1));
	if(get_acknowledge())
	{
		send_byte(command);
		if(get_acknowledge())
		{
			start_bit9557();
			send_byte(0x31+(addr<<1));
			if(get_acknowledge())
			{
				data_from_slave = get_8bit9557();
				stop_bit9557();
				return data_from_slave;
			}
			else
			{
//				record_err(NO_ACKNOWLEGE_9557_ERR);
				return -1;
			}
		}
		else
		{
//			record_err(NO_ACKNOWLEGE_9557_ERR);
			return -1;
		}
	}
	else	
	{
//		record_err(NO_ACKNOWLEGE_9557_ERR);
		return -1;
	}
}


/*-----------------------------------------------------------------
 *  function name: read_byte_9557
 *  description:  IICͨѶд����ȫ���̣�����������IICͨѶЭ�飬
 *				  �����addrΪ��оƬ��ַ��commandΪд���ݵ�ģʽ
 *				  dataΪ��д���ݡ�
 *  author: δ֪
 *  last update: 2011-11-26
 -----------------------------------------------------------------*/ 
int write_byte_9557(unsigned char addr, unsigned char command, unsigned char data)
{
	start_bit9557();
	send_byte(0x30+(addr<<1));
	if(get_acknowledge())
	{
		send_byte(command);
		if(get_acknowledge())
		{
			send_byte(data);
			if(get_acknowledge())
			{
				stop_bit9557();
			}
			else
			{
//				record_err(NO_ACKNOWLEGE_9557_ERR);
				return -1;
			}
		}
		else
		{
//			record_err(NO_ACKNOWLEGE_9557_ERR);
			return -1;
		}
	}
	else
	{
//		record_err(NO_ACKNOWLEGE_9557_ERR);
		return -1;
	}
	return 0;
}
