#include <libc.h>

#include "log.h"


#define TSL2561_ADDR	(0x39)

#define BMP180_ADDR		(0x77)

#define AM3212_ADDR		(0x5C)


void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(__u8 _ucByte);
__u8 i2c_ReadByte(void);
__u8 i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
__u8 i2c_CheckDevice(__u8 _Address);


__u8 i2c_checkdevice(__u8 _Address)
{
	__u8 ucAck;

	i2c_CfgGpio();		/* ����GPIO */

	
	i2c_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address);
	ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

	i2c_Stop();			/* ����ֹͣ�ź� */

	return ucAck;
}


__u32 i2c_test()
{
	__u8 data;
	
	if (i2c_checkdevice(0xA0) == 0)
	{
		PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
		//return 1;
	}
	else
	{
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
		i2c_Stop(); 	
		//return 0;
	}
	return 0;

	if (i2c_checkdevice(AM3212_ADDR) == 0)
	{
		PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
		return 1;
	}
	else
	{
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
		i2c_Stop();		
		//return 0;
	}
	
	if (i2c_checkdevice(TSL2561_ADDR) == 0)
	{
		PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
		return 1;
	}
	else
	{
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
		i2c_Stop(); 	
		//return 0;
	}
	
	if (i2c_checkdevice(BMP180_ADDR) == 0)
	{
		PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
		return 1;
	}
	else
	{
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
		i2c_Stop(); 	
		//return 0;
	}

	//return 0;

	if (i2c_checkdevice(0xB8) == 0)	/* AM2312 */
	{
		PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
		i2c_Start();
		i2c_SendByte(0xB8);
		if (i2c_WaitAck() != 0)
		{
			PRINT_EMG("%s-%d\n", __func__, __LINE__);
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}
		i2c_SendByte(0x00);
		i2c_Start();
		i2c_SendByte(0xB9);
		if (i2c_WaitAck() != 0)
		{
			PRINT_EMG("%s-%d\n", __func__, __LINE__);
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}
		data = i2c_ReadByte();
		PRINT_EMG("data: 0x%x \n", data);
		i2c_Ack();
		i2c_Stop();

		//return 1;
	}
	else
	{
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
		i2c_Stop(); 	
		//return 0;
	}

if (i2c_checkdevice(0xEE) == 0) /* BMP180 */
{
	PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
	i2c_Start();
	i2c_SendByte(0xEE);
	if (i2c_WaitAck() != 0)
	{
		PRINT_EMG("%s-%d\n", __func__, __LINE__);
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	i2c_SendByte(0xD0);
	i2c_Start();
	i2c_SendByte(0xEF);
	if (i2c_WaitAck() != 0)
	{
		PRINT_EMG("%s-%d\n", __func__, __LINE__);
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	data = i2c_ReadByte();
	PRINT_EMG("data: 0x%x \n", data);
	i2c_Ack();
	i2c_Stop();
	//return 1;
}
else
{
	/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
	PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
	i2c_Stop(); 	
	//return 0;
}

if (i2c_checkdevice(0x29) == 0)
{
	PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
	//return 1;
}
else
{
	/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
	PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
	i2c_Stop();
	//return 0;
}


if (i2c_checkdevice(0x49) == 0)
{
	PRINT_EMG("%s-%d succ\n", __func__, __LINE__);
	//return 1;
}
else
{
	/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
	PRINT_EMG("%s-%d fail\n", __func__, __LINE__);
	i2c_Stop();
	//return 0;
}

return 0;
cmd_fail:
	PRINT_EMG("cmd fail!\n");
	return 1;
}
