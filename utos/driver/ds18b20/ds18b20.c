#include "int.h"
#include "ds18b20.h"
#include "log.h"
#include "systick.h"

static void                           DS18B20_GPIO_Config                       ( void );
static void                           DS18B20_Mode_IPU                          ( void );
static void                           DS18B20_Mode_Out_PP                       ( void );
static void                           DS18B20_Rst                               ( void );
static uint8_t                        DS18B20_Presence                          ( void );
static uint8_t                        DS18B20_ReadBit                           ( void );
static uint8_t                        DS18B20_ReadByte                          ( void );
static void                           DS18B20_WriteByte                         ( uint8_t dat );
static void                           DS18B20_SkipRom                           ( void );
static void                           DS18B20_MatchRom                          ( void );


uint8_t ds18b20_id[8];

 /**
  * @brief  DS18B20 ��ʼ������
  * @param  ��
  * @retval ��
  */
uint8_t DS18B20_Init(void)
{
	uint8_t p;
	uint32_t *id;
	
	DS18B20_GPIO_Config ();
	
	DS18B20_DQ_1;
	
	DS18B20_Rst();

	p = DS18B20_Presence ();

	DS18B20_ReadId(ds18b20_id);

	id = (uint32_t *)ds18b20_id;
	
	PRINT_EMG("ds18b20 id: [%x%x]\n", __REV(id[0]), __REV(id[1]));
	
	PRINT_EMG("DS18B20_Init %s \n", p == 0 ? "succ" : "fail");

	return p;
}


/*
 * ��������DS18B20_GPIO_Config
 * ����  ������DS18B20�õ���I/O��
 * ����  ����
 * ���  ����
 */
static void DS18B20_GPIO_Config(void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	

	/*����DS18B20_DQ_GPIO_PORT������ʱ��*/
	DS18B20_DQ_SCK_APBxClock_FUN ( DS18B20_DQ_GPIO_CLK, ENABLE); 

	/*ѡ��Ҫ���Ƶ�DS18B20_DQ_GPIO_PORT����*/															   
  	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��DS18B20_DQ_GPIO_PORT*/
  	GPIO_Init ( DS18B20_DQ_GPIO_PORT , &GPIO_InitStructure );

}

/*
 * ��������DS18B20_Mode_IPU
 * ����  ��ʹDS18B20-DATA���ű�Ϊ����ģʽ
 * ����  ����
 * ���  ����
 */
static void DS18B20_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;

	  	/*ѡ��Ҫ���Ƶ�DS18B20_DQ_GPIO_PORT����*/	
	  GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;

	   /*��������ģʽΪ��������ģʽ*/ 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	

	  /*���ÿ⺯������ʼ��DS18B20_DQ_GPIO_PORT*/
	  GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}


/*
 * ��������DS18B20_Mode_Out_PP
 * ����  ��ʹDS18B20-DATA���ű�Ϊ���ģʽ
 * ����  ����
 * ���  ����
 */
static void DS18B20_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	 	/*ѡ��Ҫ���Ƶ�DS18B20_DQ_GPIO_PORT����*/															   
  	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/*���ÿ⺯������ʼ��DS18B20_DQ_GPIO_PORT*/
  	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}


/*
 *�������ӻ����͸�λ����
 */
static void DS18B20_Rst(void)
{
	/* ��������Ϊ������� */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DQ_0;
	/* �������ٲ���480us�ĵ͵�ƽ��λ�ź� */
	udelay(750);
	
	/* �����ڲ�����λ�źź��轫�������� */
	DS18B20_DQ_1;
	
	/*�ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������*/
	udelay(15);
}


/*
 * ���ӻ����������صĴ�������
 * 0���ɹ�
 * 1��ʧ��
 */
static uint8_t DS18B20_Presence(void)
{
	uint8_t pulse_time = 0;
	
	/* ��������Ϊ�������� */
	DS18B20_Mode_IPU();
	
	/* �ȴ���������ĵ�������������Ϊһ��60~240us�ĵ͵�ƽ�ź� 
	 * �����������û����������ʱ�������ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
	 */
	while( DS18B20_DQ_IN() && pulse_time<100 )
	{
		pulse_time++;
		udelay(1);
	}	
	/* ����100us�󣬴������嶼��û�е���*/
	if( pulse_time >=100 )
		return 1;
	else
		pulse_time = 0;
	
	/* �������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us */
	while( !DS18B20_DQ_IN() && pulse_time<240 )
	{
		pulse_time++;
		udelay(1);
	}	
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}


/*
 * ��DS18B20��ȡһ��bit
 */
static uint8_t DS18B20_ReadBit(void)
{
	uint8_t dat;
	
	/* ��0�Ͷ�1��ʱ������Ҫ����60us */	
	DS18B20_Mode_Out_PP();
	/* ��ʱ�����ʼ���������������� >1us <15us �ĵ͵�ƽ�ź� */
	DS18B20_DQ_0;
	udelay(10);
	
	/* ���ó����룬�ͷ����ߣ����ⲿ�������轫�������� */
	DS18B20_Mode_IPU();
	udelay(2);
	
	if( DS18B20_DQ_IN() == SET )
		dat = 1;
	else
		dat = 0;
	
	/* �����ʱ������ο�ʱ��ͼ */
	udelay(45);
	
	return dat;
}


/*
 * ��DS18B20��һ���ֽڣ���λ����
 */
static uint8_t DS18B20_ReadByte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = DS18B20_ReadBit();		
		dat = (dat) | (j<<i);
	}
	
	return dat;
}


/*
 * дһ���ֽڵ�DS18B20����λ����
 */
static void DS18B20_WriteByte(uint8_t dat)
{
	uint8_t i, testb;
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		/* д0��д1��ʱ������Ҫ����60us */
		if (testb)
		{			
			DS18B20_DQ_0;
			/* 1us < �����ʱ < 15us */
			udelay(8);
			
			DS18B20_DQ_1;
			udelay(58);
		}		
		else
		{			
			DS18B20_DQ_0;
			/* 60us < Tx 0 < 120us */
			udelay(70);
			
			DS18B20_DQ_1;			
			/* 1us < Trec(�ָ�ʱ��) < �����*/
			udelay(2);
		}
	}
}


 /**
  * @brief  ����ƥ�� DS18B20 ROM
  * @param  ��
  * @retval ��
  */
static void DS18B20_SkipRom ( void )
{
	DS18B20_Rst();	   
	
	DS18B20_Presence();	 
	
	DS18B20_WriteByte(0XCC);		/* ���� ROM */
	
}


 /**
  * @brief  ִ��ƥ�� DS18B20 ROM
  * @param  ��
  * @retval ��
  */
static void DS18B20_MatchRom ( void )
{
	DS18B20_Rst();	   
	
	DS18B20_Presence();	 
	
	DS18B20_WriteByte(0X55);		/* ƥ�� ROM */
	
}


/*
 * �洢���¶���16 λ�Ĵ�������չ�Ķ����Ʋ�����ʽ
 * ��������12λ�ֱ���ʱ������5������λ��7������λ��4��С��λ
 *
 *         |---------����----------|-----С�� �ֱ��� 1/(2^4)=0.0625----|
 * ���ֽ�  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----����λ��0->��  1->��-------|-----------����-----------|
 * ���ֽ�  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * �¶� = ����λ + ���� + С��*0.0625
 */
 /**
  * @brief  ������ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ 
  * @param  ��
  * @retval �¶�ֵ
  */
float DS18B20_GetTemp_SkipRom ( void )
{
	uint8_t tpmsb, tplsb;
	short s_tem;
	float f_tem;
	
	
	DS18B20_SkipRom ();
	DS18B20_WriteByte(0X44);				/* ��ʼת�� */
	
	
	DS18B20_SkipRom ();
  DS18B20_WriteByte(0XBE);				/* ���¶�ֵ */
	
	tplsb = DS18B20_ReadByte();		 
	tpmsb = DS18B20_ReadByte(); 
	
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* ���¶� */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	
	
	
}


 /**
  * @brief  ��ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ 
  * @param  ds18b20_id�����ڴ�� DS18B20 ���кŵ�������׵�ַ
  * @retval ��
  */
void DS18B20_ReadId ( uint8_t * ds18b20_id )
{
	uint8_t uc;
	
	
	DS18B20_WriteByte(0x33);       //��ȡ���к�
	
	for ( uc = 0; uc < 8; uc ++ )
	  ds18b20_id [ uc ] = DS18B20_ReadByte();
	
}

uint32_t DS18B20_GetTemp()
{
	 uint8_t tpmsb, tplsb, i;
	 short s_tem;
	 uint32_t f_tem;
	 
	 DS18B20_MatchRom ();			 //ƥ��ROM
	 
   for(i=0;i<8;i++)
		 DS18B20_WriteByte ( ds18b20_id [ i ] ); 
	 
	 DS18B20_WriteByte(0X44);				 /* ��ʼת�� */
 
	 
	 DS18B20_MatchRom ();			 //ƥ��ROM
	 
	 for(i=0;i<8;i++)
		 DS18B20_WriteByte ( ds18b20_id [ i ] ); 
	 
	 DS18B20_WriteByte(0XBE);				 /* ���¶�ֵ */
	 
	 tplsb = DS18B20_ReadByte();	  
	 tpmsb = DS18B20_ReadByte(); 
	 
	 s_tem = tpmsb<<8;
	 s_tem = s_tem | tplsb;

	 if( s_tem < 0 )	 /* ���¶� */
		 f_tem = (~s_tem+1) * 625;
	 else
		 f_tem = s_tem * 625;
	 
	 return f_tem;	 
	 
	 
 }
 

 /**
  * @brief  ��ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ 
  * @param  ds18b20_id����� DS18B20 ���кŵ�������׵�ַ
  * @retval �¶�ֵ
  */
float DS18B20_GetTemp_MatchRom ( uint8_t * ds18b20_id )
{
	uint8_t tpmsb, tplsb, i;
	short s_tem;
	float f_tem;
	
	DS18B20_MatchRom ();            //ƥ��ROM
	
  for(i=0;i<8;i++)
		DS18B20_WriteByte ( ds18b20_id [ i ] );	
	
	DS18B20_WriteByte(0X44);				/* ��ʼת�� */

	
	DS18B20_MatchRom ();            //ƥ��ROM
	
	for(i=0;i<8;i++)
		DS18B20_WriteByte ( ds18b20_id [ i ] );	
	
	DS18B20_WriteByte(0XBE);				/* ���¶�ֵ */
	
	tplsb = DS18B20_ReadByte();		 
	tpmsb = DS18B20_ReadByte(); 
	
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* ���¶� */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	
	
	
}

void ds18b20_main()
{
	static int init = 0;
	
	if (init == 0) {
		DS18B20_Init();
		init = 1;
	}
	
	while (1) {
		//__local_irq_disable(); /* FIXME: udelay depend the irq */
		PRINT_EMG("ds18b20 temp: %d\n",DS18B20_GetTemp());
		//__local_irq_enable();  /* FIXME: udelay depend the irq */
		mdelay(2000);
	}
}