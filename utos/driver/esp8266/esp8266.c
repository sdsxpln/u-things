#include <libc.h>  
#include "esp8266.h"
#include "systick.h"

#define true    (1)
#define false   (0)
#define macNVIC_PriorityGroup_x NVIC_PriorityGroup_2

static void                   ESP8266_GPIO_Config                 ( void );
static void                   ESP8266_USART_Config                ( void );
static void                   ESP8266_USART_NVIC_Configuration    ( void );



struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };


static char * __itoa( int value, char *string, int radix );

void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
    const char *s;
    int d;   
    char buf[16];


    va_list ap;
    va_start(ap, Data);

    while ( * Data != 0 )     // ������������
    {				                          
        if ( * Data == 0x5c )  //'\'
        {									  
            switch ( *++Data )
            {
                case 'r':							          //���
                    USART_SendData(USARTx, 0x0d);
                    Data ++;
                    break;

                case 'n':							          //���
                    USART_SendData(USARTx, 0x0a);	
                    Data ++;
                    break;

                default:
                    Data ++;
                    break;
            }			 
        }

        else if ( * Data == '%')
        {									  //
            switch ( *++Data )
            {				
                case 's':										  //���
                    s = va_arg(ap, const char *);

                    for ( ; *s; s++) 
                    {
                        USART_SendData(USARTx,*s);
                        while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                    }

                    Data++;

                    break;

                case 'd':			
                    //���
                    d = va_arg(ap, int);

                    __itoa(d, buf, 10);

                    for (s = buf; *s; s++) 
                    {
                        USART_SendData(USARTx,*s);
                        while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                    }

                    Data++;

                    break;

                default:
                    Data++;

                    break;

            }		 
        }

        else USART_SendData(USARTx, *Data++);

        while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );

    }
}


/*
 * ����itoa
 * ��  ������������
 * ��  �-radix =10 ��10��������0
 *         -value �������
 *         -buf �������
 *         -radix = 10
 * ��  ��
 * ��  ��
 * ��  ��USART2_printf()��
 */
static char * __itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;

    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


/**
 * @brief  ESP8266��ʼ������
 * @param  ��
 * @retval ��
 */
void ESP8266_Init ( void )
{
    ESP8266_GPIO_Config (); 

    ESP8266_USART_Config (); 


    macESP8266_RST_HIGH_LEVEL();

    macESP8266_CH_DISABLE();


}


/**
 * @brief  ��ʼ��ESP8266�õ���GPIO����
 * @param  ��
 * @retval ��
 */
static void ESP8266_GPIO_Config ( void )
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;


    /* ���� CH_PD ����*/
    macESP8266_CH_PD_APBxClock_FUN ( macESP8266_CH_PD_CLK, ENABLE ); 

    GPIO_InitStructure.GPIO_Pin = macESP8266_CH_PD_PIN;	

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    GPIO_Init ( macESP8266_CH_PD_PORT, & GPIO_InitStructure );	 


    /* ���� RST ����*/
    macESP8266_RST_APBxClock_FUN ( macESP8266_RST_CLK, ENABLE ); 

    GPIO_InitStructure.GPIO_Pin = macESP8266_RST_PIN;	

    GPIO_Init ( macESP8266_RST_PORT, & GPIO_InitStructure );	 


}


/**
 * @brief  ��ʼ��ESP8266�õ��� USART
 * @param  ��
 * @retval ��
 */
static void ESP8266_USART_Config ( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    /* config USART clock */
    macESP8266_USART_APBxClock_FUN ( macESP8266_USART_CLK, ENABLE );
    macESP8266_USART_GPIO_APBxClock_FUN ( macESP8266_USART_GPIO_CLK, ENABLE );

    /* USART GPIO config */
    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =  macESP8266_USART_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macESP8266_USART_TX_PORT, &GPIO_InitStructure);  

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = macESP8266_USART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(macESP8266_USART_RX_PORT, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = macESP8266_USART_BAUD_RATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(macESP8266_USARTx, &USART_InitStructure);


    /* �ж����� */
    USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж� 
    USART_ITConfig ( macESP8266_USARTx, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	

    ESP8266_USART_NVIC_Configuration ();


    USART_Cmd(macESP8266_USARTx, ENABLE);


}


/**
 * @brief  ���� ESP8266 USART �� NVIC �ж�
 * @param  ��
 * @retval ��
 */
static void ESP8266_USART_NVIC_Configuration ( void )
{
    NVIC_InitTypeDef NVIC_InitStructure; 


    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig ( macNVIC_PriorityGroup_x );

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = macESP8266_USART_IRQ;	 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}


/*
 * ��������ESP8266_Rst
 * ����  ������WF-ESP8266ģ��
 * ����  ����
 * ����  : ��
 * ����  ���� ESP8266_AT_Test ����
 */
void ESP8266_Rst ( void )
{
#if 0
    ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	

#else
    macESP8266_RST_LOW_LEVEL();
    mdelay( 500 ); 
    macESP8266_RST_HIGH_LEVEL();

#endif

}


/*
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
    strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�

    macESP8266_Usart ( "%s\r\n", cmd );

    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
        return true;

    mdelay( waittime );                 //��ʱ

    strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

    macPC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );

    if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
        return ( ( uint8_t ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
                ( uint8_t ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 

    else if ( reply1 != 0 )
        return ( ( uint8_t ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );

    else
        return ( ( uint8_t ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );

}


/*
 * ��������ESP8266_AT_Test
 * ����  ����WF-ESP8266ģ�����AT��������
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
//void ESP8266_AT_Test ( void )
//{
//	macESP8266_RST_HIGH_LEVEL();
//	
//	Delay_ms ( 1000 ); 
//	
//	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 200 ) ) ESP8266_Rst ();  	

//}
void ESP8266_AT_Test ( void )
{
    char count=0;

    macESP8266_RST_HIGH_LEVEL();
    mdelay ( 1000 );
    while ( count < 10 )
    {
        if( ESP8266_Cmd ( "AT", "OK", NULL, 500 ) ) return;
        ESP8266_Rst();
        ++ count;
    }
}


/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
    switch ( enumMode )
    {
        case STA:
            return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 

        case AP:
            return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 

        case STA_AP:
            return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 

        default:
            return false;
    }

}


/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
    char cCmd [120];

    snprintf ( cCmd, sizeof(cCmd), "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );

    return ESP8266_Cmd ( cCmd, "OK", NULL, 5000 );

}


/*
 * ��������ESP8266_BuildAP
 * ����  ��WF-ESP8266ģ�鴴��WiFi�ȵ�
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 *       ��enunPsdMode��WiFi���ܷ�ʽ�����ַ���
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_BuildAP ( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode )
{
    char cCmd [120];

    snprintf ( cCmd, sizeof(cCmd), "AT+CWSAP=\"%s\",\"%s\",1,%d", pSSID, pPassWord, enunPsdMode );

    return ESP8266_Cmd ( cCmd, "OK", 0, 1000 );

}


/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
    char cStr [20];

    snprintf ( cStr, sizeof(cStr), "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );

    return ESP8266_Cmd ( cStr, "OK", 0, 500 );

}


/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
    char cStr [100] = { 0 }, cCmd [120];

    switch (  enumE )
    {
        case enumTCP:
            snprintf ( cStr, sizeof(cStr), "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
            break;

        case enumUDP:
            snprintf ( cStr, sizeof(cStr), "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
            break;

        default:
            break;
    }

    if ( id < 5 )
        snprintf ( cCmd, sizeof(cCmd), "AT+CIPSTART=%d,%s", id, cStr);

    else
        snprintf ( cCmd, sizeof(cCmd), "AT+CIPSTART=%s", cStr );

    return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );

}


/*
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
    char cCmd1 [120], cCmd2 [120];

    if ( enumMode )
    {
        snprintf ( cCmd1, sizeof(cCmd1), "AT+CIPSERVER=%d,%s", 1, pPortNum );

        snprintf ( cCmd2, sizeof(cCmd2), "AT+CIPSTO=%s", pTimeOver );

        return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
                ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
    }

    else
    {
        snprintf ( cCmd1, sizeof(cCmd1), "AT+CIPSERVER=%d,%s", 0, pPortNum );

        return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
    }

}


/*
 * ��������ESP8266_Get_LinkStatus
 * ����  ����ȡ WF-ESP8266 ������״̬�����ʺϵ��˿�ʱʹ��
 * ����  ����
 * ����  : 2�����ip
 *         3����������
 *         3��ʧȥ����
 *         0����ȡ״̬ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_Get_LinkStatus ( void )
{
    if ( ESP8266_Cmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
    {
        if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:2\r\n" ) )
            return 2;

        else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:3\r\n" ) )
            return 3;

        else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:4\r\n" ) )
            return 4;		

    }

    return 0;

}


/*
 * ��������ESP8266_Get_IdLinkStatus
 * ����  ����ȡ WF-ESP8266 �Ķ˿ڣ�Id������״̬�����ʺ϶�˿�ʱʹ��
 * ����  ����
 * ����  : �˿ڣ�Id��������״̬����5λΪ��Чλ���ֱ��ӦId5~0��ĳλ����1���Id���������ӣ�������0���Idδ��������
 * ����  �����ⲿ����
 */
uint8_t ESP8266_Get_IdLinkStatus ( void )
{
    uint8_t ucIdLinkStatus = 0x00;


    if ( ESP8266_Cmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
    {
        if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:0," ) )
            ucIdLinkStatus |= 0x01;
        else 
            ucIdLinkStatus &= ~ 0x01;

        if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:1," ) )
            ucIdLinkStatus |= 0x02;
        else 
            ucIdLinkStatus &= ~ 0x02;

        if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:2," ) )
            ucIdLinkStatus |= 0x04;
        else 
            ucIdLinkStatus &= ~ 0x04;

        if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:3," ) )
            ucIdLinkStatus |= 0x08;
        else 
            ucIdLinkStatus &= ~ 0x08;

        if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:4," ) )
            ucIdLinkStatus |= 0x10;
        else 
            ucIdLinkStatus &= ~ 0x10;	

    }

    return ucIdLinkStatus;

}


/*
 * ��������ESP8266_Inquire_ApIp
 * ����  ����ȡ F-ESP8266 �� AP IP
 * ����  ��pApIp����� AP IP ��������׵�ַ
 *         ucArrayLength����� AP IP ������ĳ���
 * ����  : 0����ȡʧ��
 *         1����ȡ�ɹ�
 * ����  �����ⲿ����
 */
uint8_t ESP8266_Inquire_ApIp ( char * pApIp, uint8_t ucArrayLength )
{
    unsigned char uc;

    char * pCh;


    ESP8266_Cmd ( "AT+CIFSR", "OK", 0, 500 );

    pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "APIP,\"" );

    if ( pCh )
        pCh += 6;

    else
        return 0;

    for ( uc = 0; uc < ucArrayLength; uc ++ )
    {
        pApIp [ uc ] = * ( pCh + uc);

        if ( pApIp [ uc ] == '\"' )
        {
            pApIp [ uc ] = '\0';
            break;
        }

    }

    return 1;

}


/*
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_UnvarnishSend ( void )
{
    if ( ! ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) )
        return false;

    return 
        ESP8266_Cmd ( "AT+CIPSEND", "OK", ">", 500 );

}


/*
 * ��������ESP8266_ExitUnvarnishSend
 * ����  ������WF-ESP8266ģ���˳�͸��ģʽ
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_ExitUnvarnishSend ( void )
{
    Delay_ms ( 1000 );

    macESP8266_Usart ( "+++" );

    Delay_ms ( 500 ); 

}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
    char cStr [20];
    uint8_t bRet = false;


    if ( enumEnUnvarnishTx )
    {
        macESP8266_Usart ( "%s", pStr );

        bRet = true;

    }

    else
    {
        if ( ucId < 5 )
            snprintf ( cStr, sizeof(cStr), "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

        else
            snprintf ( cStr, sizeof(cStr), "AT+CIPSEND=%d", ulStrLength + 2 );

        ESP8266_Cmd ( cStr, "> ", 0, 100 );

        bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 500 );
    }

    return bRet;

}


/*
 * ��������ESP8266_ReceiveString
 * ����  ��WF-ESP8266ģ������ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 * ����  : ���յ����ַ����׵�ַ
 * ����  �����ⲿ����
 */
char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
    char * pRecStr = 0;


    strEsp8266_Fram_Record .InfBit .FramLength = 0;
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;

    while ( ! strEsp8266_Fram_Record .InfBit .FramFinishFlag );
    strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';

    if ( enumEnUnvarnishTx )
        pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

    else 
    {
        if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD" ) )
            pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

    }

    return pRecStr;

}


/*
 * ��������ESP8266_CWLIF
 * ����  ����ѯ�ѽ����豸��IP
 * ����  ��pStaIp������ѽ����豸��IP
 * ����  : 1���н����豸
 *         0���޽����豸
 * ����  �����ⲿ����
 */
uint8_t ESP8266_CWLIF ( char * pStaIp )
{
    uint8_t uc, ucLen;

    char * pCh, * pCh1;


    ESP8266_Cmd ( "AT+CWLIF", "OK", 0, 100 );

    pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "," );

    if ( pCh )
    {
        pCh1 = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "AT+CWLIF\r\r\n" ) + 11;
        ucLen = pCh - pCh1;
    }

    else
        return 0;

    for ( uc = 0; uc < ucLen; uc ++ )
        pStaIp [ uc ] = * ( pCh1 + uc);

    pStaIp [ ucLen ] = '\0';

    return 1;

}


/*
 * ��������ESP8266_CIPAP
 * ����  ������ģ��� AP IP
 * ����  ��pApIp��ģ��� AP IP
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
uint8_t ESP8266_CIPAP ( char * pApIp )
{
    char cCmd [ 30 ];


    snprintf ( cCmd, sizeof(cCmd), "AT+CIPAP=\"%s\"", pApIp );

    if ( ESP8266_Cmd ( cCmd, "OK", 0, 5000 ) )
        return 1;

    else 
        return 0;

}

/**************************************************/

/********************************** �û���Ҫ���õĲ���**********************************/
#define   macUser_ESP8266_BulitApSsid         "utos-wifi"      //Ҫ�������ȵ������
#define   macUser_ESP8266_BulitApEcn           WPA2_PSK               //Ҫ�������ȵ�ļ��ܷ�ʽ
#define   macUser_ESP8266_BulitApPwd           "wildfire"         //Ҫ�������ȵ����Կ

#define   macUser_ESP8266_TcpServer_IP         "192.168.121.169"      //������������IP��ַ
#define   macUser_ESP8266_TcpServer_Port       "8080"             //�����������Ķ˿�   

#define   macUser_ESP8266_TcpServer_OverTime   "1800"             //��������ʱʱ�䣨��λ���룩


/**
 * @brief  ESP8266 ��Sta Tcp Client��͸��
 * @param  ��
 * @retval ��
 */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
    uint8_t ucId, ucLen;
    uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;

    char cStr [ 100 ] = { 0 }, cCh;

    char * pCh, * pCh1;

    PRINT_EMG( "\r\n�������� ESP8266 ......\r\n" );

    macESP8266_CH_ENABLE();

    ESP8266_AT_Test ();

    ESP8266_Net_Mode_Choose ( AP );

    while ( ! ESP8266_CIPAP ( macUser_ESP8266_TcpServer_IP ) );

    while ( ! ESP8266_BuildAP ( macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd, macUser_ESP8266_BulitApEcn ) );	

    ESP8266_Enable_MultipleId ( ENABLE );
    while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_TcpServer_Port, macUser_ESP8266_TcpServer_OverTime ) );

    ESP8266_Inquire_ApIp ( cStr, 20 );
    PRINT_EMG ( "\r\n��ģ��WIFIΪ%s�����뿪�Ż���Ϊ%s\r\nAP IP Ϊ��%s�������Ķ˿�Ϊ��%s\r\n�ֻ������������Ӹ� IP �Ͷ˿ڣ���������5���ͻ���\r\n",
            macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd, cStr, macUser_ESP8266_TcpServer_Port );


    strEsp8266_Fram_Record.InfBit.FramLength = 0;
    strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;	

    while ( 1 )
    {		
        if ( strEsp8266_Fram_Record.InfBit.FramFinishFlag )
        {
            USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //���ô��ڽ����ж�
            strEsp8266_Fram_Record.Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

            PRINT_EMG( "\r\nrecv [%s]\r\n", strEsp8266_Fram_Record.Data_RX_BUF );//

            if ( ( pCh = strstr ( strEsp8266_Fram_Record.Data_RX_BUF, "+IPD," ) ) != 0 ) 
            {
                ucId = * ( pCh + strlen ( "+IPD," ) ) - '0';
                ESP8266_SendString ( DISABLE, cStr, strlen ( cStr ), ( ENUM_ID_NO_TypeDef ) ucId );
            }

            strEsp8266_Fram_Record.InfBit.FramLength = 0;
            strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;	

            USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж�

        }

    }


}

