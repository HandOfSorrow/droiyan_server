#ifndef _COMMON_H_
#define _COMMON_H_
#include <Winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <ctype.h> 
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <shellapi.h>
#include <wininet.h>

#define FILELOG  "c:\\ShootSunDro.Log"
#define GameName "Droiyan Online"
//#define GameName	"CDX Example 2"

#define MainWndTxt	       "�����ս-����"
#define SHOOTSUN_VERSION   "�����ս1.00 (���ڱ���������д)"

#define WM_AUTOBANK		0X400+1343
#define WM_AUTOWEAPON	0X400+1344
#define WM_AUTODRUG		0X400+1345
#define WM_AUTODALL		0X400+1346

#define WM_INITUIDATA		0X400+1369
#define WM_POPUPMAIN		0X400+1370
#define WM_CLOSEMAIN		0X400+1371
#define WM_COMMONDMAIN		0X400+1372
#define WM_COMMONDCONFIG1	0X400+1373
#define WM_COMMONDCONFIG2	0X400+1374
#define WM_COMMONDCONFIG3	0X400+1375
#define WM_COMMONDCONFIG3_1	0X400+1376
#define WM_COMMONDCONFIG3_2	0X400+1377
#define WM_COMMONDCONFIG4	0X400+1378
#define WM_COMMONDCONFIG4_1 0x400+1379
#define WM_COMMONDCONFIG4_2 0x400+1380
#define WM_COMMONDCONFIG4_3 0x400+1381
#define WM_COMMONDCONFIG4_4 0x400+1382
#define WM_COMMONDCONFIG5	0X400+1383
#define WM_COMMONDCONFIG5_1 0x400+1384
#define WM_COMMONDCONFIG5_2 0x400+1385
#define WM_COMMONDCONFIG5_3 0x400+1386
#define WM_COMMONDCONFIG6	0X400+1387
#define WM_COMMONDCONFIG6_1	0X400+1388
#define WM_COMMONDCONFIG6_2	0X400+1389
#define WM_COMMONDCONFIG7	0X400+1390
#define WM_COMMONDCONFIG8	0X400+1391
#define WM_COMMONDCONFIG8_1	0X400+1392
#define WM_COMMONDCONFIG8_2	0X400+1393
#define WM_COMMONDCONFIG8_3	0X400+1394
#define WM_COMMONDCONFIG9	0X400+1395
#define WM_COMMONDCONFIG9_1	0X400+1396
#define WM_COMMONDCONFIG9_2	0X400+1397
#define WM_COMMONDCONFIG9_3	0X400+1398
#define WM_COMMONDCONFIG9_4	0X400+1399
#define WM_NOTIFYCONFIG1	0X400+1400
#define WM_NOTIFYCONFIG2	0X400+1401
#define WM_STOP		    0X400+1402

#define WM_NOTIFYCONFIG5    0x400 + 1500
#define WM_NOTIFYCONFIG6    0x400 + 1501

#define WM_COMMON_SPEED1    0x400 + 1502
#define WM_COMMON_SPEED2    0x400 + 1503

#define WM_XOR              0x400 + 1600

#define WM_CONNECT          0x400 + 1700

#define WM_ACCEPT_0            WM_USER + 1
#define WM_ACCEPT_1            WM_USER + 2
#define WM_ACCEPT_2            WM_USER + 3
#define WM_SERVER_READCLOSE_0  WM_USER + 4
#define WM_SERVER_READCLOSE_1  WM_USER + 5
#define WM_SERVER_READCLOSE_2  WM_USER + 6
#define WM_CLIENT_READCLOSE_0  WM_USER + 7
#define WM_CLIENT_READCLOSE_1  WM_USER + 8
#define WM_CLIENT_READCLOSE_2  WM_USER + 9
#define WM_LOGIN_READCLOSE     WM_USER + 10


#define WINTHREAD unsigned __stdcall

#define	D_WORD_XOR1MUL				((WORD)0x009d)
#define	D_WORD_XOR3BASE				((WORD)0x086d)
#define	D_WORD_XOR3MUL				((WORD)0x087b)
#define	D_XORTABLE_LENGTH			8

#define QueryKeyV(s, name, var) \
	size = s; \
	if (RegQueryValueEx(myKey, name, 0, &type, (LPBYTE) var, &size) != 0) { if (err) { RegCloseKey(myKey); return -1; } }

#define SetKeyV(name, var, s, t) \
	RegSetValueEx(myKey, name, 0, t, (LPBYTE) var, s);

#define SERVERTAB                      9
#define MAXDATA                        9
#define MAXMAP                         29
#define MAXPLAYER                      1000
#define MAXFILTER                      1035
#define TABNUMBER                      9               //TAB����
#define SETTINGNUM                     3               //���ݴ�ŵ�����
#define OBJ_INFO_CHAIN_NUMBER          500         
#define ID_SHARED_FILE                 "HWIJNTJES"     //�����ڴ��ļ���

struct UIDATA
{

	HWND GamehWnd;  //��Ϸ����
	HWND hMain;		//��DIALGO����
	HWND hGame;
	HWND hDebug;
	BOOL ShowMainDialog;
	BOOL ShowDebugDialog;
	BOOL bInstallUI;
	DWORD GameTheadID;

	HWND hMainConfig[TABNUMBER];	    // ��Ϸ����TAB����
	HWND hInfoConfig[3];

	HWND hMainConfig3_1;
	HWND hMainConfig3_2;

	HWND hMainConfig4_1;
	HWND hMainConfig4_2;
	HWND hMainConfig4_3;
	HWND hMainConfig4_4;

	HWND hMainConfig5_1;
	HWND hMainConfig5_2;
	HWND hMainConfig5_3;

	HWND hMainConfig6_1;
	HWND hMainConfig6_2;

	HWND hMainConfig8_1;
	HWND hMainConfig8_2;
	HWND hMainConfig8_3;

	HWND hMainConfig9_1;
	HWND hMainConfig9_2;
	HWND hMainConfig9_3;
	HWND hMainConfig9_4;

	BOOL RUNED;
	BYTE XorTable[8];

	char szExePath[256];

} ;
struct DATAMAP {

	BYTE Code1;
	BYTE Code2;

	int NaiJiu;
	char Data[256];

} ;

struct STATUS 
{
	BOOL Door;
	int Mode;
} ;

struct MAPDATA 
{
	BYTE Now;              // �������ڵĵ���
	BOOL Door;             // �������Ƿ����
	int  Mode;             // 

	int Site;              // 0 ���� 1 ҽԺ 2 ����
	                       // 3      4      5      6
	                       //            7
	BOOL AutoSave;         // �Ƿ��Զ���������
	char Name[128];        // �õ���������
} ;

struct AUTOPLAY
{
	char Name[128];          // ����

	int Time;               // ʱ��

	int Danger;             // Σ��ָ��
} ;

/*
    ===============================================================

    ���н����Һ͹��������Ϣ����

    enemy_chain1      Ϊ����npc ���� ��ң�����Ʒ�Ķ���
    enemy_chain2      Ϊ�ص����

	=================================================================
*/

struct OBJ_BANK_CHAIN
{
	BYTE Code1;
	BYTE Code2;

	BYTE Index;

	int Mode;               // 0 Ϊ������ 1 Ϊ�������� 2 ΪЯ��   

	int Number;             // ά������

	OBJ_BANK_CHAIN *next;

} ;

struct SPEAK_CHAIN
{
	BYTE Mode;                            // ����ģʽ 
	char buf[255];                        //
	BOOL Status;

	SPEAK_CHAIN *next; 
} ;

struct OBJ_INFO_CHAIN
{
	BYTE Code1;
	BYTE Code2;

	// 0 ���� 1 NPC 2 ��� 3 δ֪ �� �ȴ���չ
	int Mode;                        // ����

	int X;
	int Y;

	int Number;                      // ����

	char Name[128];                   // ����

	char Team[128];                   // ��֯


	int BloodMax;                     // ������ֵ
	int BloodNow;
};
// ������Ʒ
struct NOVELITY_SETUP_CHAIN
{
	BYTE Code1;
	BYTE Code2;

	int Mode;

	char Name[128];

	NOVELITY_SETUP_CHAIN *next;
} ;

struct OBJ_CHUM_CHAIN
{
	char Name[128];

	int Mode;

	int Attack;

	OBJ_CHUM_CHAIN *next;
} ;

struct NOVELITY_INFO_CHAIN 
{
	BYTE Code1;
	BYTE Code2;

	int X;
	int Y;

	int Number;

	char Name[128];

	struct NOVELITY_INFO *next;
} ;

struct OBJ_SITE_CHAIN
{
	char Name[128];

	int Time;

	int Danger;

	int X;
	int Y;

	int Mode;

	struct OBJ_SITE_CHAIN *next;
} ;

struct DOOR 
{
	char Name[20];                       // ���Ƶص�
	int Number;                          // λ��
 
	int Mode;                            // ��������
	BYTE Site;                           // �ط�
	BOOL Status;                         // �Ƿ�������
} ;

struct SERVER 
{
	char ID[64];
	char IP[32];
	int Port;
} ;

struct FILTER
{
	BYTE Code1;                           // ��Ʒ����
	BYTE Code2;

	char Name[128];                       // ��Ʒ����

	int Mode1;
	int Mode2;
	int Mode3;	
} ;

struct QUEUE
{
	int Mode;                             // 0 �� 1 ��

	char Name[64];                        // 

	BOOL Status;                          // �Ƿ񼤻�
} ;

struct ATTACK 
{
	QUEUE Queue[30];                      // 

	BYTE Code1;                           // ������Ŀ�����
	BYTE Code2;

	char Name1[64];                       // ���˴���

	char Name2[64];                       // �ӳ�����

	int PlayerMode;                       // ���ڵ�ģʽ

	BOOL Enable;                          // ����

} ;

struct GAMEIP
{
	char Name[64];
	char IP[32];
	unsigned int Port;
} ;

struct CONFIG {

	BOOL AUTO_PLAY;                      // �Զ��һ�

	int GameMode;                        // �Ƿ����� 0 ���� 1 �ѻ�

	BOOL LOADDOOR;                       // �Ƿ���ش�����


	BYTE Server;                         // �������б�
	char NameID[128];                    // ��ʱ�û�������

	char User_Name[32];
	char User_Password[32];

	int Server1;
	int Server2;

	char Char[64];

	BYTE TalkMode;
	char TalkBuf[255];

	BOOL Game;
 
	char IP[32];                         // 
	int Port;
	char IDPassword[128];

	BOOL AUTO_CONNECT;                   // �Զ����� ����ͻ���
 
	BOOL AUTO_CONNECT_MODE1;             // ����ģʽ1
	BOOL AUTO_CONNECT_MODE2;             // ����ģʽ2

	int AUTO_CONNECT_TIME;               // ��������Ӧʱ�Զ����� ʱ��

	BOOL RUNED;

	BOOL PLAYING;                        // ������Ϸ

//	BOOL 
	BOOL AUTO_LOCK_TARGET;               // ����Ŀ��
	BOOL AUTO_GAME_INFO;                 // ս����Ϣ
	BOOL AUTO_CTRL;                      // ��ʾʰȡ
	BOOL AUTO_OTHER_AGGRESSIVE;          // ���湥��
	BOOL AUTO_OVER;                      // ��ʦ��ǽ
	BOOL AUTO_SELL;                      // ����װ��

	BOOL INFO_PLAYER_DIALOG;             // ������ʾ�����Ϣ
	BOOL INFO_NPC_DIALOG;                // ������ʾ������Ϣ

	BOOL AUTO_SAVE_SITE1;                // �Ƿ��Զ�������ӵص�
	BOOL AUTO_SAVE_SITE2;                // �������Զ�����ص�

	BOOL AUTO_NOVELITY;                  // �Զ�����
	BOOL AUTO_SPELL;                     // �Զ���ħ��

	BOOL AUTO_NOVELITY_0;                // ʰȡ��
	BOOL AUTO_NOVELITY_1;                // ʰȡ��
	BOOL AUTO_NOVELITY_2;                // ʰȡ��
	BOOL AUTO_NOVELITY_3;                // ʰȡ��

	BOOL AUTO_SELL_NOVELITY_0;           // ����
	BOOL AUTO_SELL_NOVELITY_1;           // ����
	BOOL AUTO_SELL_NOVELITY_2;           // ����
	BOOL AUTO_SELL_NOVELITY_3;           // ����

	BOOL NO_ATTACK;                      // ��������
	BOOL RECV_INFO;                      // ������Ϣ

	BOOL CHECK_8_1;                      // ��ʦ��ǽ
	BOOL CHECK_8_2;

	BOOL AUTO_MONEY;                     // �Զ���Ǯ
	BOOL AUTO_SPEAK;                     // �Զ�����

	BOOL AUTO_SPEAK_PK;                  // PK���Զ�����
	char SPEAK_PK[256];

	BOOL AUTO_AGGRESSIVE;                // �Զ�����
	BOOL AUTO_COUTERATTACK;              // �Զ�����
	BOOL AUTO_MAX_KILL; //�Զ�ʹ�ñ�ɱ
	int  MaxKill_time;

	BOOL AUTO_ENERGY;                    // ��������
	
	BOOL AUTO_BLOOD;                     // �Զ���Ѫ
	BOOL AUTO_PHYSICAL;                  // �Զ�����

	int SPEAK_TIMER;                     // ���Լ��ʱ��

	BOOL AUTO_DO_1;                      // ������Ʒ
	BOOL AUTO_DO_2;
	BOOL AUTO_DO_3;
	BOOL AUTO_DO_4;

	BOOL CHECK_9_1_1;                    // 
	BOOL CHECK_9_1_2;
	BOOL CHECK_9_1_3;
	BOOL CHECK_9_1_4;
	BOOL CHECK_9_1_5;

	BOOL EDIT_9_1_1;                     // ��������
	BYTE COMBO_9_1_1;                    // ��ʾģʽ


	int BLOOD_MODE;                      // 0 = A 1 = B 2 = C
	int BLOOD_ADD_MODE;

	int SPELL_MODE;                      // 0 = A 1 = B 2 = C
	int PHYSICAL_MODE;                   // 0 = A 1 = B 2 = C

	BOOL BACK_CASTLE;                    // �Զ��سǱ���

	BOOL AUTO_BUY;                       // �����Զ�����
	BOOL AUTO_BUYBUMBER;                 // �����Զ��������������

	int EDIT_OTHER_1;                    // ����ҩˮ������ֵ
	int EDIT_OTHER_2;                    // ħ��ҩˮ������ֵ
	int EDIT_OTHER_3;                    // 
	int EDIT_OTHER_4;                    // 
	int EDIT_OTHER_5;                    // 
	int EDIT_OTHER_6;                    // 

	int BLOOD_USE_MODE;                  // ʹ���ж�ģʽ
	int SPELL_USE_MODE;                  // 

	BOOL AUTO_SELL_SCORE;                // �Զ���װ��
	int SELL_SCORE;                      // ����


	BOOL BANK_0;                         // �����Զ�ȡ����
	BOOL BANK_1;

	BOOL AUTO_OPEN_DOOR;                 // ������ʱ���Զ��򿪴�����

	int EDIT_8_1_1;                      // 
	int EDIT_8_1_2;                      // 
	int EDIT_8_1_3;
	int EDIT_8_1_4;
	int EDIT_8_1_5;
	int EDIT_8_1_6;
	int EDIT_8_1_7;
	int EDIT_8_1_8;
	int EDIT_8_1_9;
	int EDIT_8_1_10;
	int EDIT_8_1_11;
	int EDIT_8_1_12;
	int EDIT_8_1_13;
	int EDIT_8_1_14;
	int EDIT_8_1_15;
	int EDIT_8_1_16;
	int EDIT_8_1_17;
	int EDIT_8_1_18;
	int EDIT_8_1_19;
	int EDIT_8_1_20;
	int EDIT_8_1_21;
	int EDIT_8_1_22;

	int HotKey;

	int NetSpeed;                        // �����ٶ�
	int GameSpeed;                       // ��Ϸ�ٶ�

	int MoveRunSpeed;                    // �ܲ��ƶ��ٶ�
	int MoveSpellSpeed;                  // ˲���ƶ��ٶ�
	int MoveSpeed;                       // �ƶ��ٶ�

	int RUNBUYNUMBER;                    // ��������ҩˮ������ֵ
 

	char Computer_NPC1[3][20];            // ����
	                                      // ҽԺ
	                                      // ����	

	char Computer_NPC2[3][20];            // ����
	                                      // ҽԺ
	                                      // ����	

	BOOL Player_Mode_1;                  // 
	BOOL Player_Mode_2;                  // 
	BOOL Player_Mode_3;                  // 
	BOOL Player_Mode_4;                  // 
	BOOL Player_Mode_5;                  // 
	BOOL Player_Mode_6;                  // 
	BOOL Player_Mode_7;                  // 
	BOOL Player_Mode_8;                  // 
	BOOL Player_Mode_9;                  // 
	BOOL Player_Mode_10;                 // 
	BOOL Player_Mode_11;                 // 
	BOOL Player_Mode_12;                 // 
	BOOL Player_Mode_13;                 // 
	BOOL Player_Mode_14;                 // 
	BOOL Player_Mode_15;                 // 
	BOOL Player_Mode_16;                 // 

	int AggressiveMode1;                 // ��ͨ����ѡ��
	int AggressiveMode2;                 // ���湥��ѡ��

	int Speed1;                          // ��Ϸ�ٶ�
	int Speed2;                          // �����ٶ�

	BOOL WITH_AGGRESSIVE;                 // �Ƿ�������湥��
	BOOL OTHER_ADD_BLOOD;                 // �����Ѫ

	UINT m_nTalk_TimerID;
	UINT m_nMainAggressive_TimerID;
	UINT m_nOtherAggressive_TimerID;
	UINT m_nSpell_TimerID;
	UINT m_nGameSpeed_TimerID;
	UINT m_nStay_TimerID;
	UINT m_nOtherBlood_TimerID;
	UINT m_nGotoSite_TimerID;
	UINT m_nMove_TimerID;
	UINT m_nBank_TimerID;
	UINT m_Authentication_TimerID;

	BYTE XORTable[8];

	int	m_nGameSpeeder;
	BOOL bSpeedChanged;
	BOOL bCtrlKey;

	BOOL BANK;

	char ServerIP[24];

	int ServerPort;
} ;

struct NOVELITYDATA
{
	BYTE Code[23];
} ;

struct NOVELITYSTATUS
{
	BYTE Now; 
	int Number; 
	int Modify; 
	int Color;
	int Status;
} ;

struct NOVELITYSELL
{
	BOOL Sell;                          // �Ƿ�����
	BOOL Recv;                          // �Ƿ���յ���������������Ϣ

	char Name[128];                     // ����

	BYTE Code[24];                      // ����
} ;

struct NOVELITYUSE
{
	int BloodANumber;                   // �� A ���� ����
	int BloodBNumber;
	int BloodCNumber;
	int BloodDNumber;

	BYTE BloodANow;                     // �� A ���� λ��
	BYTE BloodBNow;
	BYTE BloodCNow;
	BYTE BloodDNow;

	int PhysicalANumber;                // ���� A  ���� ����
	int PhysicalBNumber;
	int PhysicalCNumber;

	BYTE PhysicalANow;                  // ���� A ���� λ�� 
	BYTE PhysicalBNow;
	BYTE PhysicalCNow;

    int SpellANumber;                   // �� A ���� ����
	int SpellBNumber;
	int SpellCNumber;
	int SpellDNumber;

	BYTE SpellANow;                     // �� A ���� λ��
	BYTE SpellBNow;
	BYTE SpellCNow;
	BYTE SpellDNow;

	int SpellA_Number;                  // ���� A
	int SpellB_Number;                  // ���� B

	BYTE SpellA_Now;                    // ���� A
	BYTE SpellB_Now;                    // ���� B

	int CityNumber;                     // �س�ҩˮ����������
	BYTE CityNow;                       // �س�ҩˮ�����ڵ�λ��

	int RandomNumber;                   // ����ҩˮ�����ڵ�����
	BYTE RandomNow;                     // ����ҩˮ�����ڵ�λ��

	BYTE DoorNumber;                    // ���������� ��������
	int DoorNow;                        // ������λ��

	int EnergyWeaponNumber;             // ����������ֵ
	BYTE EnergyWeaponNow;               // ��������

	int AggressiveEnergyNow;            // ��ɱֵ
	int AggressiveEnergyMax;            // ����ɱֵ
} ;

struct GAMEDATA 
{
	char user[128];
	char password[128];
	char IP[128];
	char PlayerAcount1[128];
	int  PlayerAcount_len;
	char StrPKWARE[20480];
	int PKLen;
	BOOL ALL;                           // ��ҩ 
	BOOL WEAPON;                        // ����
	BOOL SELL;                          // ��װ��
	BOOL BANK;                          // ����
	BOOL DRUG;                          // ��ҩ

	BOOL RECV_SELL;                     // ������������Ϣ
	BOOL RECV_REPAIR;                   // ���յ�ά����Ϣ
	BOOL RECV_BANK;                     // ����������Ϣ


	int PlayerLevel;                    // �ȼ�
	int PlayerMode;                     // ְҵ����
	char PlayerName[128];               // �������
	char PlayerAcount[128];

	DOOR Door[30];                      // ������
	BYTE DoorNumber;                    // ��������������
	BYTE DoorNow;                       // ����������λ��
	BOOL DoorTmp;                       // ��ʱ�����ŵ�����
	int ServerID;
	char stats[8];
	char NamePlayerInfo[256];
	char PlayerInfoPath[256];

	BYTE TalkMode;
	char TalkBuf[256];

	SOCKET s;                           // 

	BOOL Moved;                         // �Ƿ��Ѿ���ʼ�ƶ�
	BOOL MoveSpell;                     // �Ƿ���ƶ�ħ��
	BOOL Goto_Aggressive;               // �Ƿ��Ѿ���Ŀ��
            // 

	int Goto_X;				            // Ŀ��x����
	int Goto_Y;				            // Ŀ��y����
	int x;                          //����X����
	int y;                          //����Y����
	int z;                          // ���ڵ�λ��

	int Site_X;                         // ��Χ
	int Site_Y;                         // 

	int Danger;                         // Σ�ճ̶�

	BOOL MOTION_STOP;                   // ���������ʱֹͣ���ж���
	BOOL DO_NOVELITY;                   // ��������ʰȡ��Ʒ��
	BOOL DO_BUY;                        // ���������ҩ�����������Ⱥ��ɹһ�
	BOOL DO_MOVE;                       // ְҵ�����ƶ�
	BOOL DO_PAUSE;                      // �һ���ͣ��
	BOOL DO_MODFIYSITE;                 // �һ����ڸ����ص��С���������һ�Σ��Ա��ص���﹥��
	BOOL DO_BEGIN;                      // ��һ�������һ���־
	BOOL DO_TMP_SITE;                   // ��ʱ�л��ص�ʱ��Ҫ

	BOOL CONNECTING;                    // ��������

	BYTE MYSENDTime1;
	BYTE MYSENDTime2;
	BYTE MYSENDTime3;
	BYTE MYSENDTime4;

	BYTE MYRECVTime1;
	BYTE MYRECVTime2;
	BYTE MYRECVTime3;
	BYTE MYRECVTime4;

	int  BankMode;                       // �Ƿ�Ϊ˽�˻��ۺ�����
	BOOL BankInfo;                       // �Ƿ��������������Ϣ

	int Time;                            // ���ڵ�ϵͳʱ��
	int SiteNumber;                      // �ص�����
	char *TMPDOOR;                       // ��ʱ�ص��¼
//	int TMPDOORNUMBER;                   // �ص�����
	 

	int Blood_Now;                       // ���ڵ�������ֵ
	int Blood_Max;                       // ���������ֵ
	int Blood_N[6];                      // ����ҩ��λ�� a b c d
	int Blood_W;//�سǱ���

	int Physical_Now;                    // �������ڵ���ֵ
	int Physical_Max;                    // �������ֵ
	int Physical_N[6];                      // ������λ�� a b c

    int Spell_Now;                       // ħ�����ڵ���ֵ
    int Spell_Max;                       // ħ�����ֵ
	int Spell_N[6];                      // ����ҩ��λ�� a b c d ba bb
	BYTE lockID[2]; //����������Ŀ��
	BYTE Spell_lockID[2];
	BYTE Spell_code;

	int Energy_Now;                      // ��ǰ������ֵ
	int EnergyWeapon_Now1;                // ǹ��ǰ��������λ��
	int EnergyWeapon_Now2;                // ����ǰ��������λ��
	int EnergyWeapon_Number;             // ��ǰ�������ĸ���
	int Return_Now;   //ʱ��ҩˮ

	int ME_X;                            // ������ڵ�x����
	int ME_Y;                            // ������ڵ�y���� 

	int nGotoCounte;					 // ˲�ƴ���

	BOOL SystemInfo;                    // �Ƿ�������ʾϵͳ��Ϣ
	char SystemInfoStr[25600];            // ϵͳ��Ϣ��ʾ
	BYTE SystemInfoMode;                // ��ʾģʽ
	int SystemLen;


	BYTE AggressiveCode1;                // �������ڹ����Ĺ������
	BYTE AggressiveCode2;                // �������ڹ����Ĺ������
	char AggressiveStr[128];             // ���ڹ����Է�������

	int AggressiveLast;                  // �ϴ���ֵ
	int AggressiveNow;                   // ���ι�����ֵ

	BYTE TempCode1;
	BYTE TempCode2;

	BYTE MeCode1;                        // ������ڵĴ���
    BYTE MeCode2;                        // ��ҵ������

	int AggressiveX;                     // ����x����
	int AggressiveY;                     // ����y����

	BOOL AggressiveStatus;               // �������ڵ�״̬

	NOVELITYDATA Novelity39[39];         // ������ϵ� ��Ʒ�� 24 
	BOOL SellNovelity39[39];             // ��������װ����ʱ����
	BYTE Code39[4];                      // 

	NOVELITYDATA Novelity48[48];         // ˽������
	BYTE Code48[5];                      // 

	NOVELITYDATA Novelity150[150];       // �ۺ�����
	BYTE Code150[7]; 

	NOVELITYUSE  Novelity_39;            // ������Ʒ
	NOVELITYUSE  Novelity_48;            // ������Ʒ
	NOVELITYUSE  Novelity_150;           // ������Ʒ

	FILTER SellFilter[24];               // ����װ��
	FILTER StayFilter[24];               // ����װ��

	BOOL Player_Mode_1;                  // 
	BOOL Player_Mode_2;                  // 
	BOOL Player_Mode_3;                  // 
	BOOL Player_Mode_4;                  // 
	BOOL Player_Mode_5;                  // 
	BOOL Player_Mode_6;                  //
	BOOL Player_Mode_7;                  //
	BOOL Player_Mode_8;                  //
	BOOL Player_Mode_9;                  //
	BOOL Player_Mode_10;                 // 
	BOOL Player_Mode_11;                 // 
	BOOL Player_Mode_12;                 // 
	BOOL Player_Mode_13;                 //
	BOOL Player_Mode_14;                 // 
	BOOL Player_Mode_15;                 // 
	BOOL Player_Mode_16;                 // 

	STATUS Status;                       // 
} ;

//BOOL GetAllWnd();

HANDLE CreateMapFile();
void ReadDataEx(LPSTR szData,DWORD dwOffsize,DWORD dwCopySize);
void WriteDataEx(LPSTR szData,DWORD dwOffsize,DWORD dwCopySize);

void LoadDoorItem(HWND hWnd);
BOOL GetDoorStatus(char Name[20]);
int GetMeCityMode(BYTE Code);
void AddNovelityInfo(int index);
void AddInfoList(int i, char tmp1[32], char *tmp2, char tmp3[32], char tmp4[32]);
void DeleteAll4_1();
void AddInfoList4_1(int i, char buf1[12], char buf2[128], char buf3[32], char buf4[32]);
void DeleteAll4_2();
void AddInfoList4_2(int i, char buf1[128], char buf2[32]);
void DeleteAll4_3();
void AddInfoList4_3(int i, char buf1[12], char buf2[128], char buf3[32], char buf4[32]);
void DeleteAll5_1();
void AddInfoList5_1(int i, char *buf1, char *buf2, char *buf3);

void DeleteAll6();
void AddInfoList6_1(int i, char *buf1, char *buf2, char *buf3);
void DeleteAll9_1();
void AddInfoList9_1(int i, char *buf1, char *buf2, char *buf3); 
void ConverNPCString(char* szName,char* szPos,char* szDest,int nDestlen);
char* GetMapName(int Code);

void SaveLog_(char *p);
BOOL InstallPKware(char* cExePath);
void UninstallPKware();
int PKware_Implode(char *buf, int nLen, int resLen);
int PKware_Explode(char *buf, int nLen, int resLen);
void TFPKware_(LPBYTE lpTarget, int nLen);

void InstallTab(int index);
void UninstallUI();

//int PoupDialog();

struct NOVELITY_SETUP_CHAIN *add_novelity_setup_obj(char Name[128], int Mode);
struct NOVELITY_SETUP_CHAIN *search_novelity_setup_obj(struct NOVELITY_SETUP_CHAIN *h, char Name[256], NOVELITY_SETUP_CHAIN **pp);
struct NOVELITY_SETUP_CHAIN *sub_novelity_setup_obj(char Name[128]);

void create_site_obj();
void create_chum_obj();
void create_obj();
void clear_obj();

int search_code_obj(BYTE Code1, BYTE Code2);
int search_space_obj();
void add_obj(BYTE Code1, BYTE Code2, int X, int Y, int BloodNow, int BloodMax, int mode, char *buf1, char *buf2);
void sub_obj(BYTE Code1, BYTE Code2);
void modify_obj(BYTE Code1, BYTE Code2, int X, int Y, int BloodNow, int BloodMax);

struct OBJ_SITE_CHAIN *search_time_obj(struct OBJ_SITE_CHAIN *h, int Time, OBJ_SITE_CHAIN **pp);
struct OBJ_SITE_CHAIN *search_site_obj(struct OBJ_SITE_CHAIN *h, char name[128], OBJ_SITE_CHAIN **pp);
struct OBJ_SITE_CHAIN *add_site_obj(char name[128], int Time, int Danger, int X, int Y, int Mode);
struct OBJ_SITE_CHAIN *sub_site_obj(char name[128]);


struct OBJ_CHUM_CHAIN *search_chum_obj(struct OBJ_CHUM_CHAIN *h, char Name[128], OBJ_CHUM_CHAIN **pp);
struct OBJ_CHUM_CHAIN *add_chum_obj(int mode, char Name[128], int Attack);
struct OBJ_CHUM_CHAIN *sub_chum_obj(char name[128]);

struct SPEAK_CHAIN *search_speak_chain(struct SPEAK_CHAIN *h, char name[128], SPEAK_CHAIN **pp);
struct SPEAK_CHAIN *sub_speak_chain(char name[255]);
struct SPEAK_CHAIN *add_speak_chain(char buf[255], int Mode, BOOL Status);
struct OBJ_BANK_CHAIN *add_bank_chain(BYTE Code1, BYTE Code2, BYTE Index, int Mode, int Number);


#endif