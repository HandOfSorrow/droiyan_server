#pragma once

class Encode
{
public:
	Encode(void);
public:
	~Encode(void);
public:
	SOCKET Socket;
	BOOL con;
	void S_Encode_Decode(char *lpTarget, char * lpSource, int nLen);
	int S_Encode_send(SOCKET s,char *data);
	int S_Encode_recv(SOCKET s,char *data);
	BOOL connect_Server();
	int Send_User(char *name,int len);
	int Time;
};
/*****************************************
�����ʽ  ���� 200�ֽ�
FF 88  ��ͷ
88 FF  ��β
�ӵ�47���ֽڿ��洢���ݵ�87�ֽڽ��� ������
�ӵ�97�ֵ�107�ֽ���   ����KEY�����
�����ֽ��������

��������1���ֽڱ�ʾ �������
�ͻ��ˣ�
0x11: ע���˺� +�˺ų��� + �˺� + ʹ��ʱ�� ��8λ�ڣ�
0x22����֤�û��� +�˺ų��� + �˺�
0x33���������û���ʹ��ʱ��  +�˺ų��� + �˺� + ʹ��ʱ�� ��8λ�ڣ�
�����
0xEE ��֤�ɹ�  + ����ʱ��
0xDD ��֤ʧ��
0xBB �����ɹ�
0xAA ����ʧ��
******************************************/