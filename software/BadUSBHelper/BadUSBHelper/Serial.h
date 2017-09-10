#pragma once
#include<string>
#include<Windows.h>

using namespace std;

class Serial
{
public:
	static Serial* GetInstance();

	bool OpenPort(CString sSerialPort);

    bool InitPort(CString sSerialPort);   //���ڳ�ʼ��

    int ReadFromPort(void* buff, int size); //�Ӵ��ڶ�����

    bool WriteToPort(const void* buff, int size);//д���ݵ�����

    bool ClosePort();   //�رմ���
    HANDLE m_hCom;
protected:
    Serial();
    ~Serial();

    static Serial* m_pSerial;
	string m_sSerialPort;
	//int m_iSerialRate;

};