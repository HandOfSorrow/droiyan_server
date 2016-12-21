// USER.cpp: implementation of the USER class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Server.h"
#include "USER.h"
#include "COM.h"
#include "CircularBuffer.h"
#include "BufferEx.h"
#include "Item.h"
#include "Hyper.h"
#include "Mcommon.h"
#include "Search.h"
#include "ServerDlg.h"
#include "Mcommon.h"
#include "UserManager.h"
#include "UNI_CHAR.h"
#include "RoyalRumble.h"

#include "EventZone.h"

#include "SharedQueue.h"
#include "ShopSystem.h"
#define _CHINA
#define D_WORD_XOR1MUL                          ((WORD)0x009d)
#define D_WORD_XOR3BASE                         ((WORD)0x086d)
#define D_WORD_XOR3MUL                          ((WORD)0x087b)
#define D_XORTABLE_LENGTH                       8
char G_BYTE_DOII_XorTable[D_XORTABLE_LENGTH] = { 0x31, 0x54, 0x76, 0x58, 0x42, 0x22, 0x12, 0x53 };

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "Extern.h"		//	�ܺκ��� ����

extern CUserManager *g_pUQM;
extern CSearch *g_pUserList;
extern CServerDlg *g_pMainDlg;

// zinee 02/11/26 for Mail Agent
extern CSharedQueue		g_MailAgentQueue;

//extern CRITICAL_SECTION m_CS_ReceiveData[];
extern CRITICAL_SECTION m_CS_FileWrite;
extern CRITICAL_SECTION m_CS_EventItemLogFileWrite;
extern CPtrList RecvPtrList[];
extern long nRecvDataLength[];

// IKING 2002.1.
extern CRITICAL_SECTION m_CS_LoginData;
extern CPtrList RecvLoginData;
extern long nLoginDataCount;
extern CRITICAL_SECTION m_CS_LogoutData;
extern CPtrList RecvLogoutData;
extern long nLogoutDataCount;				

extern ShopSystem g_Shop;
extern CPtrList RecvSqlData;
extern CRITICAL_SECTION m_CS_SqlData;
extern long nSqlDataCount;	
extern int g_ChatEnable[];

extern CHATDATAPACKET *g_WaitRecvDataChat[DATA_BUF_SIZE_FOR_THREAD+1][AUTOMATA_THREAD+1];
extern int g_nChatDataHead[];
extern int g_nChatDataTail[];

extern BYTE g_ServerId;

extern TCHAR g_arServerIPAddr[16];
extern CString g_strARKRRWinner;
extern DWORD g_ThreadErr[];

extern CEventZone	g_QuestEventZone;

extern long	g_PotionViewOnOff;
//
/// ��ȣȭ ����
// ���� ���� �ٸ���...
//T_KEY private_key = 0x1234567890123456;
//T_KEY private_key = 0x4584263589548694;
T_KEY private_key = 0xd9b4f1a524657845;
////////////////////////////////////////////////////////////////////// 
//	��������
//
int g_8x[]	= {-2, -1, 0, 1, 2, 1, 0, -1};
int g_8y[]	= {0, -1, -2, -1, 0, 1, 2, 1};
int g_16x[]	= {-4, -3, -2, -1, 0, 1, 2, 3, 4, 3, 2, 1, 0, -1, -2, -3};
int g_16y[]	= {0, -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3, 4, 3, 2, 1};

POINT g_rPt[8][6] = 
{
	{{-1, -1}, {0, -1}, {-1, 0}, {0, 0}, {100, 100}, {100, 100}},
	{{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}},
	{{0, -1}, {1, -1}, {0, 0}, {1, 0}, {100, 100}, {100, 100}},
	{{-1, -1}, {0, -1}, {-1, 0}, {0, 0}, {-1, 1}, {0, 1}},
	{{0, -1}, {1, -1}, {0, 0}, {1, 0}, {0, 1}, {1, 1}},
	{{-1, 0}, {0, 0}, {-1, 1}, {0, 1}, {100, 100}, {100, 100}},
	{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}},
	{{0, 0}, {1, 0,}, {0, 1}, {1, 1}, {100, 100}, {100, 100}},
};

short		g_sHPConst[CLASS_NUM] = {6, 3, 4, 4}; // ����, ������, ����, �ѱ�
short		g_sPPConst[CLASS_NUM] = {1, 5, 3, 5};
short		g_sSPConst[CLASS_NUM] = {2, 2, 2, 2};

short		g_sHPLV[CLASS_NUM]	= {5, 3, 4, 3};
short		g_sPPLV[CLASS_NUM]	= {1, 4, 2, 3};
short		g_sSPLV[CLASS_NUM]	= {1, 1, 1, 1};

short		g_sHPAdd[CLASS_NUM]	= {10, 30, 40, 35};
short		g_sPPAdd[CLASS_NUM]	= {20, 10, 20, 20};
short		g_sSPAdd[CLASS_NUM]	= {20, 25, 35, 25};

int			g_iAccessoriSlot[MAX_ACCESSORI] = {2, 3, 6, 7};

extern short		g_sHPConst[CLASS_NUM];
extern short		g_sPPConst[CLASS_NUM];
extern short		g_sSPConst[CLASS_NUM];

extern short		g_sHPLV[CLASS_NUM];
extern short		g_sPPLV[CLASS_NUM];
extern short		g_sSPLV[CLASS_NUM];

extern short		g_sHPAdd[CLASS_NUM];
extern short		g_sPPAdd[CLASS_NUM];
extern short		g_sSPAdd[CLASS_NUM];

extern CRoyalRumble	g_RR;
//////////////////////////////////////////////////////////////////////
// �ش� �迭���� ����Ҽ��ִ� ����迭 ����(������ ���̺� ����)
//
int g_BrawlClass[] = {8,9,10,11,12,13,14,15};
int g_StaffClass[] = {4,5,6,7,12,13,14,15};
int g_EdgedClass[] = {2,3,6,7,10,11,14,15};
int g_FireArmsClass[] = {1,3,5,7,9,11,13,15};

///////////////////////////////////////////////////////////////////////
//	�����ϰ� ��ų�� ���� ��Ī�� �ȵǾ �����Ӽ��� ��ų�������� ����Ʈ�Ѵ�.
//								����		������				����			�ѱ�			  ���迭
BYTE g_DynamicSkillInfo[MAX_SKILL_MAGIC_NUM] = { 8,7,6,9,  0,  13,12,10,11,  0,  16,15,14,17,  0,  21,19,18,20,  0,  0,0,0 };

BOOL g_bDebug = FALSE;

int	g_CityRank[5][3] =		// ����϶��� ���Ҽ�ġ
{
	{{3200}, {2950}, {3800}},
	{{1800}, {2300}, {3800}},
	{{1100}, {1870}, {2700}},
	{{1333}, {1870}, {2600}},
	{{1333}, {1870}, {2600}},
};
							// �� ��޴� ���� �ʿ��ġ
int	g_CityRankValue[] = {-39413, -26242, -11899, -2112, -332, 0, 500, 4000, 26300, 43900, 64000};
					//�ָ�  ������ ����  �ѱ�
int	g_ClassWeight[] = {150,  100,   150,  130};		// �� �迭�� �⺻ ����

//װ������ ����
//����
int g_ItemAttUpgrade[] = {10000, 10000, 10000, 10000, 10000, 7000, 7000, 6000, 5000, 4000,2000,1000,800,300,10};
//װ��
int g_ItemDefUpgrade[] = {10000, 10000, 10000, 10000, 10000, 9000, 8000, 7000, 6000, 5000,4000,3000,600,20,10};
//�ټ�
int g_ItemDefUpgrade_[] = {10000, 10000, 10000, 10000, 80000, 7000, 6000, 3000, 2000,1000,700,500,200,100,10};
//����
int g_ItemAccessoriUpgrade[] = {9500, 9500, 9500};

int g_ItemBlessingUpgrade[] = {5000, 8500, 10000};
int g_ItemNormalDownUpgrade[] = {6000, 10000};
/***************************************
����:ת�����ݰ�
����ֵ: �޷�ֵ
����: 2006��12��19��
�汾:1.00
****************************************/
void SaveDump(char* lpTarget, int len)
{
	
	int i;
	
	FILE *fp = fopen("c:\\send_rev_dump.log", "a+");
	if(fp != NULL) 
	{
		for(i=0;i<len;i++){
			fprintf(fp, _T("%2.2x "), (unsigned char)lpTarget[i]);       //   ��ʮ�����Ƶ���ʽ�洢.   
		}
		fprintf(fp, "\n"); 
		fclose(fp);
	}
	
}
/***************************************
����:��ʼ��KEY
����ֵ: �޷�ֵ
����: 2006��12��19��
�汾:1.00
*****************************************/
void USER::init_encode(__int64 key2)
{
	int i;
	char *p;
	p=(char*)&key2;
	for(i=0; i<D_XORTABLE_LENGTH; i++){
    	key[i] = p[i] ^ G_BYTE_DOII_XorTable[i];
   }
}

/***************************************
����:�������AND����
����ֵ: �޷�ֵ
����: 2006��12��19��
�汾:1.00
*****************************************/
void USER::Encode_Decode(char* lpTarget, char* lpSource, int nLen,int f)
{
	memcpy(lpTarget,lpSource,nLen);
	return ;
	BYTE	l_BYTE_Xor1 = (WORD)nLen * D_WORD_XOR1MUL, l_BYTE_Xor3;

	WORD	l_WORD_Xor3 = D_WORD_XOR3BASE, nCir;

	for (nCir=0; nCir<(WORD)nLen; nCir++) 
	{
		l_BYTE_Xor3    = HIBYTE(l_WORD_Xor3);
		lpTarget[nCir] = lpSource[nCir] ^ l_BYTE_Xor1 ^ key[nCir % D_XORTABLE_LENGTH] ^ l_BYTE_Xor3;
		l_WORD_Xor3   *= D_WORD_XOR3MUL;
	}
// 	if(f==1)
// 		SaveDump(lpSource,nLen);
// 	else
// 		SaveDump(lpTarget,nLen);
	
}

  //////////////////////////////////////////////////////////////////////////////////////////////////////
//	�ش� Ÿ�Կ� ���� �����̻� �ð� ������ Ŭ���� �Ѵ�.
//
void USER::ClearAbnormalTime(DWORD dwType)
{
	DWORD dwCurrTime = GetTickCount();

	switch(dwType)
	{
	case ABNORMAL_TIME:
		m_tAbnormalKind = ABNORMAL_BYTE_NONE;
		m_dwAbnormalTime = 0;
		m_dwLastAbnormalTime = dwCurrTime;

		ClearAbnormalGroup(ABNORMAL_CLEAR_STATUS);
		break;

	case PSI_ONE_TIME:
		m_dwLastHasteTime	= dwCurrTime;			// ���������� ���̽�Ʈ�� �ɸ��ð�
		m_dwLastShieldTime	= dwCurrTime;			// ���������� ���带 �� �ð�
		m_dwLastDexUpTime	= dwCurrTime;			// ���������� DEX UP �� �� �ð�
		m_dwLastMaxHPUpTime	= dwCurrTime;			// ���������� MAX HP UP �� �� �ð�

		m_dwLastFastRunTime			= dwCurrTime;	// ���������� �н�Ʈ���� �� �ð�
		m_dwLastMindShockTime		= dwCurrTime;	// ���������� ���ε� ��ũ�� �� �ð�
		m_dwLastPsiShieldTime		= dwCurrTime;	// ���������� ���̿��н��带 ���ð�
		m_dwLastPiercingShieldTime = dwCurrTime;	// ���������� �Ǿ�̽��带 �� �ð�
		m_dwLastBigShieldTime       = dwCurrTime;

		m_dwHasteTime		= 0;					// Haste
		m_dwShieldTime		= 0;					// Shield
		m_dwDexUpTime		= 0;					// Dex Up
		m_dwMaxHPUpTime		= 0;					// Max HP Up

		m_dwFastRunTime			= 0;				// Fast Run
		m_dwMindShockTime		= 0;				// Mind Shock
		m_dwPsiShieldTime		= 0;				// Psionic Shield
		m_dwPiercingShieldTime	= 0;				// Piercing Shield
		m_dwBigShieldTime   = 0;


		
		ClearAbnormalGroup(ABNORMAL_CLEAR_PSI_ONE);
		if(m_bNecklaceOfShield) AddAbnormalInfo(ABNORMAL_SHIELD);	// ��ȣ�� �����

		break;

	case PSI_TWO_TIME:
		m_dwLastAdamantineTime	= dwCurrTime;	// ���������� Adamantine�� �� �ð�
		m_dwLastMightyWeaponTime= dwCurrTime;	// ���������� MightyWeapon�� �� �ð�
		m_dwLastBerserkerTime	= dwCurrTime;	// ���������� Berserker�� �� �ð�
		
		m_dwAdamantineTime		= 0;			// Adamantine
		m_dwMightyWeaponTime	= 0;			// Mighty Weapon
		m_dwBerserkerTime		= 0;			// Berserker

		ClearAbnormalGroup(ABNORMAL_CLEAR_PSI_TWO);
		break;

	case PSI_THREE_TIME:
		m_dwLastMindGuardTime	= dwCurrTime;	// ���������� Mind Guard �� ���ð�
		m_dwMindGuardTime		= 0;			// Mind Guard

		ClearAbnormalGroup(ABNORMAL_CLEAR_PSI_THREE);
		break;

	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction ...
//
USER::USER()
{	
	m_pCB		= new CCircularBuffer(20000);		// Socket���� Data�� �޾Ƽ� �׾Ƶδ� ����
	m_pCBCore	= new CCircularBuffer(20000);		// Socket���� ���� Data�߿� ��ȿ�� ��Ŷ�� ��󳻾� �����ϴ� ����

	m_pSharedMemory = NULL;
	m_pMD			= NULL;

	// IKING 2002.1.
	m_server		= 0;
	m_dwLastDownExp	= 0;
	m_bPseudoString = FALSE;//yskang 0.1 ȣĪ[��Ī]�� ��¥���� ��¥���� ����^^
	m_nGuildUserInfoType = 0x00;//yskang 0.2
	ZeroMemory(m_strLoveName,sizeof(m_strLoveName));

	InitializeCriticalSection(&m_CS_ExchangeItem);
	InitializeCriticalSection(&m_CS_ShopItem);
}

USER::~USER()
{
	int i = 0;
	EnterCriticalSection( &m_CS_ExchangeItem );
	for(i = 0; i < m_arExchangeItem.GetSize(); i++)
	{
		if( m_arExchangeItem[i] )
		{
			delete m_arExchangeItem[i];
			m_arExchangeItem[i] = NULL;
		}
	}
	m_arExchangeItem.RemoveAll();
	LeaveCriticalSection( &m_CS_ExchangeItem );

	EnterCriticalSection( &m_CS_ShopItem );
	for(i = 0; i < m_arShopItem.GetSize(); i++)
	{
		if( m_arShopItem[i] )
		{
			delete m_arShopItem[i];
			m_arShopItem[i] = NULL;
		}
	}
	m_arShopItem.RemoveAll();
	LeaveCriticalSection( &m_CS_ShopItem );

	if(m_pCB) delete m_pCB;
	if(m_pCBCore) delete m_pCBCore;

	DeleteCriticalSection(&m_CS_ExchangeItem);
	DeleteCriticalSection(&m_CS_ShopItem);
}

int USER::Init( int bufCreateFlag )
{
	CBSocket::Init(bufCreateFlag);

	m_ConnectionSuccessTick = 0;

	if ( m_Sid == -1 )
	{
		m_Sid = 0;
		m_iModSid = AUTOMATA_THREAD;
		m_uid = m_Sid;
	}
	else
	{
		m_iModSid = m_Sid % AUTOMATA_THREAD;
		m_uid = m_Sid;
	}
	
	m_isCryptionFlag = 0;
	m_Recv_value = 0;
	m_Send_value = 0;
	m_public_key = 0;

	SetPublicKey();
	m_cryptfn.SetPublicKey(m_public_key );
	init_encode(m_public_key);
	m_cryptfn.SetPrivateKey(private_key);

	// ��ġ ���� �ʱ�ȭ �ϱ�...
	if ( m_uid >= 0 )
		g_pUserList->SetUserUid( m_uid,this );

	m_Recv_value = 0;
	m_UserStatus = USER_JUST_CONNECTED;
	m_pCom = g_pMainDlg->GetCOM();
	m_UserFlag = FALSE;

	InitUser();

	m_ConnectionSuccessTick = GetTickCount();

	// �� change IP, Port...
	m_strZoneIP = "";
	m_nZonePort	= -1;

	return 1;
}

BOOL USER::InitUser()
{
	// Compress Init
	memset( m_CompBuf, NULL, 10240 );		// ������ �����͸� ������ ����
	m_iCompIndex = 0;						// ������ �������� ����
	m_CompCount = 0;						// ������ �������� ����

	// Memory DB Data Init
	m_pSharedMemory = NULL;
	m_pMD = NULL;

	// Speed Hack Check ���� �ʱ�ȭ
	m_iTickCount = 0;
	m_dwServerTickBasic = 0;
	m_dwServerTick = 0;
	m_dwClientTick = 0;


	m_ZoneIndex = 0;
	m_bWhisper	= TRUE;						// �Ӹ� ���
	m_bLive		= USER_DEAD;				// Live Or Dead

	m_NowZoneChanging = FALSE;				// �� ü�����Ҷ� TRUE;

	m_bBuddyMode = FALSE;					// Buddy Off
//	m_bTradeMode	= FALSE;				// Trade Off
	m_bTradeWaiting = FALSE;				// Trade ��û�� ����� ��ٸ��� ��
	m_bNowTrading	= FALSE;				// ���� �ŷ���
	m_bExchangeOk	= FALSE;				// Exchange �³�
	m_bTradeItem	= FALSE;				// Item�� �÷�����(X)
	m_iTradeUid		= -1;

	m_bPShopOpen	= FALSE;				// Personal shop open or closed
	m_bPShopView	= FALSE;				// Personal shop being viewed
	m_iPShopViewuid = -1;					// Personal shop view uid
	m_bViewingAShop	= FALSE;				// User viewing a personal shop

	m_bNoItemMove	= FALSE;

	m_bRefuse		= FALSE;				// �⺻ �ɼ��� ������ �ޱ� 
	m_iMaxWeight	= 0;					// ����ִ� ����
	m_iCurWeight	= 0;

	m_dwGuild = -1;							// ��� �ε���	
	m_sGuildVersion = -1;					// ��� ���� ����
	m_bGuildMaster = FALSE;					// ��� ������
	m_bRepresentationGuild = FALSE;			// ���� ���� ����	 
	::ZeroMemory(m_strGuildName, sizeof(m_strGuildName)); // ��� �̸�

	m_bFieldWarApply = FALSE;
	m_FieldWarGMUid = 0;					// �ʵ����϶� ���� �渶 uid
	m_dwFieldWar = 0;						// �ʵ��� �����̸� ����� ��� ��ȣ�� ���´�.

	::ZeroMemory(m_strChar, sizeof(m_strChar));
	::ZeroMemory(m_strTradeUser, sizeof(m_strTradeUser));
	::ZeroMemory(m_PersonalShopName, sizeof(m_PersonalShopName));

	m_sTempSTR = 0;					
	m_sTempCON = 0;					
	m_sTempDEX = 0;					
	m_sTempINT = 0;					
	m_sTempVOL = 0;					
	m_sTempWIS = 0;	
	
	m_tTempClass = 255;

	m_nHavePsiNum = 0;						// ���� ���̿��� ��
	m_tDir = 0;								// ������ ���� �ִ� ����
	m_tIsOP = 0;							// ������� �Ǵ�
	m_bLogOut = FALSE;							// Logout ���ΰ� �Ǵ�
	m_bZoneLogOut = FALSE;

	int i = 0;										// �ݰݽ�ų ó���� ���� �ʱ�ȭ		
	for(i = 0; i < COUNTERATTACK_MAX_NUM; i++)
	{
		m_CounterAttackList[i].iDamage = 0;
		m_CounterAttackList[i].iNpcUid = -1;
		m_CounterAttackList[i].lSid = 0;
		m_CounterAttackList[i].dwLastAttackTime = GetTickCount();
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++) // �ŷ��� �κ��� �ٲ�� ������ ���� üũ����
	{ 
		m_TradeItemNum[i].sSid = -1; 
		m_TradeItemNum[i].sNum = 0; 
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++) // ������ ���Ը� �ʱ�ȭ
	{ 
		m_UserItem[i].sLevel = 0;			 // �� �������� ����Ҽ� �ִ� ����
		m_UserItem[i].sSid = -1; 
		m_UserItem[i].sCount = 0;
		m_UserItem[i].sBullNum = 0;
		m_UserItem[i].sDuration = -1;
		m_UserItem[i].tIQ = 0;
		for(int j = 0; j < MAGIC_NUM; j++) m_UserItem[i].tMagic[j] = 0;
		m_UserItem[i].iItemSerial = 0;
	}

	m_bMakeBuddy = FALSE;			// ¯���� üũ 
	m_bNowBuddy = FALSE;			// ���� ����������
	for(i = 0; i < MAX_BUDDY_USER_NUM; i++)
	{
		m_MyBuddy[i].uid = -1;
		::ZeroMemory(m_MyBuddy[i].m_strUserID, sizeof(m_MyBuddy[i].m_strUserID));		
	}

	for(i = 0; i < MAX_GUILD_USER; i++)
	{
		m_MyGuildUser[i].sLevel = 0;	// �������� ����
		::ZeroMemory(m_MyGuildUser[i].GuildUser, sizeof(m_MyGuildUser[i].GuildUser));		
	}

	for(i = 0; i < MAGIC_COUNT; i++)		// ���� ������ �ʱ�ȭ
	{
		m_DynamicUserData[i] = 0;
	}

	for(i = 0; i < EBODY_MAGIC_COUNT; i++)	// EBody ���� ���� �ʱ�ȭ
	{	
		m_DynamicEBodyData[i] = 0;
	}

	InitTelList(m_strTeleport);
	InitEventSlot();

//	m_bLive = USER_LIVE;
	m_nCharNum = 0;

	DWORD dwCurrTime = GetTickCount();

	m_dwLastSpeedTime = dwCurrTime;
	m_dwLastMoveAndRun = dwCurrTime;
	m_dwLastAttackTime = dwCurrTime;
	m_dwLastPsiAttack = dwCurrTime;
	m_dwLastMaxAttack = dwCurrTime;
	m_dwCastDelay = 0;
	m_dwLastTimeCount = dwCurrTime;
	m_dwLastAbnormalTime = dwCurrTime;
	m_dwLastHPTime = dwCurrTime;
	m_dwLastPsiDamagedTime = 0;

	m_dwLastHasteTime = dwCurrTime;
	m_dwLastShieldTime = dwCurrTime;
	m_dwLastDexUpTime = dwCurrTime;
	m_dwLastMaxHPUpTime = dwCurrTime;
	m_dwLastBigShieldTime = dwCurrTime;

	m_dwLastFastRunTime			= dwCurrTime;
	m_dwLastMindShockTime		= dwCurrTime;
	m_dwLastMindGuardTime		= dwCurrTime;
	m_dwLastPsiShieldTime		= dwCurrTime;
	m_dwLastPiercingShieldTime	= dwCurrTime;

	m_dwLastUpdateTime = dwCurrTime;	//
	m_dwLastSaveTime   = dwCurrTime;

	m_dwBSTime=0;			//���ܱ��ʱ��
	m_ShouhuTime=dwCurrTime;
	m_dwBSLaseTime=dwCurrTime;// ���ܱ�Ŀ�ʼʱ��
	m_dwLastItem44Time = dwCurrTime;
	//���ӹ���Ŀ��Ѫ��
	m_ShowHP=0;
	m_lRequireNum = 0;
	m_ionlineTime = dwCurrTime; //����ʱ�䡣�һ�AKR
	m_dwShopTime = dwCurrTime;
	m_tAbnormalKind		= 0;
	m_dwAbnormalTime	= 0;

	m_dwHasteTime		= 0;
	m_dwShieldTime		= 0;
	m_dwDexUpTime		= 0;
	m_dwMaxHPUpTime		= 0;
	m_dwAdamantineTime	= 0;
	m_dwMightyWeaponTime= 0;
	m_dwBerserkerTime	= 0;

	m_dwFastRunTime			= 0;
	m_dwMindShockTime		= 0;
	m_dwMindGuardTime		= 0;
	m_dwPsiShieldTime		= 0;
	m_dwPiercingShieldTime	= 0;
	m_dwBigShieldTime   = 0;

	m_dwLastHiExpTime		= dwCurrTime;
	m_dwLastMagicFindTime	= dwCurrTime;
	m_dwLastNoChatTime		= dwCurrTime;

	m_dwHiExpTime		= 0;	// ����ġ 2��
	m_dwMagicFindTime	= 0;	// �������� 5��
	m_dwNoChatTime		= 0;	// ä��

	m_dwNoDamageTime		= 0;
	m_dwLastNoDamageTime	= dwCurrTime;

	m_dwAbnormalInfo	= ABNORMAL_NONE;
	m_dwAbnormalInfo_	= ABNORMAL_NONE;

	m_state = STATE_DISCONNECTED;

	m_ItemFieldInfoCount = 0;
	memset( m_ItemFieldInfoBuf, NULL, 8192 );
	m_ItemFieldInfoIndex = 0;

	InitUserItem();
	InitGuildItem();			// ������ ������ ��Ź�Ѵ�.
	InitAccountBankItem();		// ������ ������ ��Ź�Ѵ�.

	m_bPkStatus = FALSE;		// ��õ��� ī������������ ����
	m_dwPkStatusTime = 0;		// ī�������϶� ���� �ð��� ����
	m_iCityValue = 0;			// ���� ���� ���Ⱚ
	m_sCityRank = 0;			// �ù� ���
	m_sKillCount = 0;			// PK���� Ƚ��

	m_sMagicSTR = 0;				// �����Ӽ��� E_Boady�������� ��ȭ�Ȱ�
	m_sMagicCON = 0;
	m_sMagicDEX = 0;
	m_sMagicVOL = 0;
	m_sMagicWIS = 0;

	m_sMagicMaxHP = 0;
	m_sMagicMaxPP = 0;
	m_sMagicMaxSP = 0;
	m_Hit=0;
	m_Avoid=0;
	m_SKill=0;
//	m_tPsiAbnormal = 0;

	EnterCriticalSection( &m_CS_ExchangeItem );
	for(i = 0; i < m_arExchangeItem.GetSize(); i++)
	{
		if(m_arExchangeItem[i] != NULL)
		{
			delete m_arExchangeItem[i];
			m_arExchangeItem[i] = NULL;
		}
	}
	m_arExchangeItem.RemoveAll();
	LeaveCriticalSection( &m_CS_ExchangeItem );
	
	EnterCriticalSection( &m_CS_ShopItem );
	for(i = 0; i < m_arShopItem.GetSize(); i++)
	{
		if ( m_arShopItem[i] != NULL )
		{
			delete m_arShopItem[i];
			m_arShopItem[i] = NULL;
		}
	}
	m_arShopItem.RemoveAll();

	LeaveCriticalSection( &m_CS_ShopItem );

	for( i = 0; i < m_arItemLog.GetSize(); i++ )
	{
		if( m_arItemLog[i] )
		{
			delete m_arItemLog[i];
		}
	}
	m_arItemLog.RemoveAll();

	m_dwSaintTime = 0;
	m_dwSaintTimeCount = GetTickCount();

	m_lDeadUsed = 0;

	//�Ҹ� �׼������� ���� �ʱ�ȭ
	m_bRingOfLife		= 0;
	m_bNecklaceOfShield	= 0;
	m_bEarringOfProtect	= 0;

	return TRUE;
}

void USER::InitEventSlot()
{
	for( int i = 0; i < m_arEventNum.GetSize(); i++)
	{
		if( m_arEventNum[i] )
		{
			delete m_arEventNum[i];
			m_arEventNum[i] = NULL;
		}
	}
	m_arEventNum.RemoveAll();
}

void USER::InitUserItem()
{
	int i, j;

	for(i = 0; i < TOTAL_ITEM_NUM; i++) // ������ ���Ը� �ʱ�ȭ
	{ 
		m_UserItem[i].sLevel = 0;	
		m_UserItem[i].sSid = -1; 
		m_UserItem[i].sCount = 0;
		m_UserItem[i].sBullNum = 0;
		m_UserItem[i].sDuration = 0;
		m_UserItem[i].tIQ = 0;
		for(j = 0; j < MAGIC_NUM; j++) m_UserItem[i].tMagic[j] = 0;
		m_UserItem[i].iItemSerial = 0;
	}

	for(i = 0; i < MAGIC_COUNT; i++)
	{
		m_DynamicUserData[i] = 0;
	}

	for(i = 0; i < EBODY_MAGIC_COUNT; i++)	// EBody ���� ���� �ʱ�ȭ
	{	
		m_DynamicEBodyData[i] = 0;
	}
}

void USER::InitUserBankItem()
{
	int i, j;
	
	m_dwBankDN = 0;

	for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++) // ���� ������ ���Ը� �ʱ�ȭ
	{ 
		m_UserBankItem[i].sLevel = 0; 
		m_UserBankItem[i].sSid = -1; 
		m_UserBankItem[i].sCount = 0;
		m_UserBankItem[i].sBullNum = 0;
		m_UserBankItem[i].sDuration = 0;
		m_UserBankItem[i].tIQ = 0;
		for(j = 0; j < MAGIC_NUM; j++) m_UserBankItem[i].tMagic[j] = 0;

		m_UserBankItem[i].iItemSerial = 0;
	}
}

void USER::InitAccountBankItem()
{
	int i, j;
	
	m_dwAccountBankDN = 0;

	for(i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++) // ���� ���� ������ ���Ը� �ʱ�ȭ
	{ 
		m_AccountBankItem[i].sLevel = 0; 
		m_AccountBankItem[i].sSid = -1; 
		m_AccountBankItem[i].sCount = 0;
		m_AccountBankItem[i].sBullNum = 0;
		m_AccountBankItem[i].sDuration = 0;
		m_AccountBankItem[i].tIQ = 0;
		for(j = 0; j < MAGIC_NUM; j++) m_AccountBankItem[i].tMagic[j] = 0;

		m_AccountBankItem[i].iItemSerial = 0;
	}
}

void USER::InitGuildItem()
{
	int i, j;
	
	m_dwGuildDN = 0;

	for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++) // ���� ������ ���Ը� �ʱ�ȭ
	{ 
		m_GuildItem[i].sLevel = 0; 
		m_GuildItem[i].sSid = -1; 
		m_GuildItem[i].sCount = 0;
		m_GuildItem[i].sBullNum = 0;
		m_GuildItem[i].sDuration = 0;
		m_GuildItem[i].tIQ = 0;
		for(j = 0; j < MAGIC_NUM; j++) m_GuildItem[i].tMagic[j] = 0;

		m_GuildItem[i].iItemSerial = 0;
	}
}

//////////////////////////////////////////////////////////////////////
//	Inline Fn...
//

USER* USER::GetUser(int uid)
{
	if( uid < 0 || uid + USER_BAND >= NPC_BAND ) return NULL;

	//if(uid >= MAX_USER)
	//{
	//	TRACE("######### USER::GetUser() : Invalid UID = %d\n" , uid);
	//	return NULL;
	//}

	return m_pCom->GetUserUid(uid);
}

USER* USER::GetUser(TCHAR* id)
{
	if(!strlen(id)) return NULL;
	
	CString szSource = id;
	USER *pUser = NULL;

	for( int i = 0; i < MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if ( pUser->m_state == STATE_DISCONNECTED || pUser->m_state == STATE_LOGOUT ) continue;

		if ( !szSource.CompareNoCase(pUser->m_strUserID) ) return pUser;
	}

	return NULL;
}
/*
inline int USER::GetUid(int x, int y )
{
	MAP* pMap = g_zone[m_ZoneIndex];
	return pMap->m_pMap[x][y].m_lUser;
}

inline BOOL USER::SetUid(int x, int y, int id)
{
	MAP* pMap = g_zone[m_ZoneIndex];

	if(!pMap) return FALSE;
	if(pMap->m_pMap[x][y].m_bMove != 0) return FALSE;
	if(pMap->m_pMap[x][y].m_lUser != 0 && pMap->m_pMap[x][y].m_lUser != id ) return FALSE;
/*
	long lUid = m_uid + USER_BAND;
	long temp = pMap->m_pMap[x][y].m_lUser;

	if(InterlockedCompareExchange(&pMap->m_pMap[x][y].m_lUser, (LONG)0, lUid) == lUid)
	{
		long tt = pMap->m_pMap[x][y].m_lUser;
		return TRUE;
	}
*/
/*	pMap->m_pMap[x][y].m_lUser = id;

	return TRUE;
}
*/
//-------------------------------------------------------------------------------//

CNpc* USER::GetNpc(int nid)
{
	if( nid < 0 || nid >= g_arNpc.GetSize() ) return NULL;

	return g_arNpc[nid];
}

CNpc* USER::GetNpc(TCHAR* pNpcName)
{
	CNpc* pNpc = NULL;

	int nSize = g_arNpc.GetSize();

	for( int i = 0; i < g_arNpc.GetSize(); i++)
	{
		pNpc = g_arNpc[i];
		if( !pNpc ) continue;

		if( _tcscmp(pNpc->m_strName, pNpcName) == 0)
		{
			return pNpc;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
//	Communication Part
int USER::NoEncodeSend(int length, char *pBuf)
{
	if ( m_SockFlag != 1 ) return 0;

	int rv = CBSocket::B_Send( length, pBuf );

	if ( m_SockFlag == 0 || rv == -2 )
	{
		//SockCloseProcess();
		return rv;
	}

	return length;
}

void USER::Send(TCHAR *pBuf, int nLength)
{
	char pTBuf[RECEIVE_BUF_SIZE+1];

	if ( m_SockFlag != 1 ) return;

	if ( nLength > PACKET_DATA_SIZE ) return;

	if(nLength >= SEND_BUF_SIZE - 6)
	{
		TRACE("#### Too Big Send Data!!! (%s, %d, size = %d)\n", __FILE__, __LINE__, nLength);
		return;
	}

//	::ZeroMemory(pTBuf, SEND_BUF_SIZE);
	TCHAR tibuf[SEND_BUF_SIZE+1];
	::ZeroMemory(pTBuf, SEND_BUF_SIZE);
	::ZeroMemory(tibuf, SEND_BUF_SIZE);

	int index = 0;
	MYSHORT slen;

	pTBuf[index++] = (char) PACKET_START1;		// ��Ŷ ������ �˸��� 2 Byte
	pTBuf[index++] = (char) PACKET_START2;

//	slen.i = nLength;
	if ( m_isCryptionFlag == 1 )
	{
		m_Send_value++;							// ��Ŷ�� �Ϸ� ��ȣ�� �ο��Ѵ�.
		DWORD nSendOrder = m_Send_value;
		m_Send_value &= 0x00ffffff;
		nSendOrder &= 0x00ffffff;

		memcpy( tibuf, &nSendOrder, sizeof(DWORD) );
		memcpy( &tibuf[4], pBuf, nLength );
		nLength += sizeof(DWORD);

		slen.i = nLength + 1;						//��ȣȭ���θ� ��Ÿ���� Ŀ�ǵ� ����
	}
	else slen.i = nLength;

	pTBuf[index++] = (char) (slen.b[0]);		// ��Ŷ ���̸� Short(2Byte)�� �Ǿ� ������
	pTBuf[index++] = (char) (slen.b[1]);		// 2Byte��� �Ͽ� 1Byte * 256 + 2Byte�� �ƴ�

//	memcpy(pTBuf + index, pBuf, nLength);
	// ��ȣȭ...
	if ( m_isCryptionFlag == 1 )
	{
		pTBuf[index++] = ENCRYPTION_PKT;
		memcpy(pTBuf + index, tibuf, nLength);
#ifdef _CHINA
		Encode_Decode((char *)pTBuf + index, (char *)tibuf, nLength,1);
#else
		m_cryptfn.JvEncryptionFast( nLength, (BYTE*)tibuf, (BYTE*)pTBuf + index );
#endif
	}
	else
	{
		memcpy(pTBuf + index, pBuf, nLength);
	}
	
	index += nLength;

	pTBuf[index++] = (char) PACKET_END1;		// ��Ŷ ���� �˸��� 2 Byte
	pTBuf[index++] = (char) PACKET_END2;


	NoEncodeSend(index, pTBuf);

}

/*
void USER::ProcessData()
{
	if ( m_UserStatus == 0 && m_UserFlag == FALSE ) return;

	if(!m_BufCount) return;

	int len = 0;
	BOOL bEncryption = FALSE;

	m_pCB->PutData(m_pBuf, m_BufCount);		// ���� Data�� ���ۿ� �ִ´�

	while (PullOutCore(len, bEncryption))
	{
		if( len <= 0 ) break;

		Parse( len, m_RData, bEncryption );

		bEncryption = FALSE;
	}
}
*/

void USER::ProcessData(char *pIBuf, int length)
{
	if ( m_UserStatus == 0 && m_UserFlag == FALSE ) return;

	if(!length) return;

	int len = 0;
	BOOL bEncryption = FALSE;

	m_pCB->PutData(pIBuf, length);		// ���� Data�� ���ۿ� �ִ´�

	while (PullOutCore(len, bEncryption))
	{
		if( len <= 0 ) break;

		Parse( len, m_RData, bEncryption );

		bEncryption = FALSE;
	}
}

BOOL USER::PullOutCore(int &length, BOOL &bEncryption)
{
	BYTE		*pTmp;
	int			len = 0, sPos = 0, ePos = 0;
	BOOL		foundCore = FALSE;
	MYSHORT		slen;

	len = m_pCB->GetValidCount();

	if(len <= 0 || len >= RECEIVE_BUF_SIZE) return FALSE;

	pTmp = new BYTE[len];
	m_pCB->GetData((char*)pTmp, len);

	for (int i = 0; i < len && !foundCore; i++)
	{
		if (i + 2 >= len) break;

		if (pTmp[i] == PACKET_START1 && pTmp[i + 1] == PACKET_START2)
		{
			sPos = i + 2;

			slen.b[0] = pTmp[sPos];
			slen.b[1] = pTmp[sPos + 1];

			length = (int)slen.i;

			if (length <= 0) goto cancelRoutine;
			if (length > len || length > CORE_BUFF_SIZE) goto cancelRoutine;

			if((BYTE)pTmp[sPos + 2] == ENCRYPTION_PKT)
			{
				bEncryption = TRUE;
			}

			ePos = sPos + length + 2;
			if((ePos + 2) > len ) goto cancelRoutine;

			if (pTmp[ePos] == PACKET_END1 && pTmp[ePos+1] == PACKET_END2)
			{
				::ZeroMemory(m_RData, CORE_BUFF_SIZE+1);

				if(bEncryption) 
#ifdef _CHINA
					Encode_Decode((char *)m_RData, (char *)(pTmp+sPos+3), length -1,0);
#else
					m_cryptfn.JvDecryptionFast( length -1, (BYTE *)(pTmp+sPos+3), (BYTE *)m_RData );
#endif
				else				::CopyMemory((void *)m_RData, (const void *)(pTmp+sPos+2), length);
//				if(bEncryption) 	::CopyMemory((void *)m_RData, (const void *)(pTmp+sPos+3), length - 1);
//				else				::CopyMemory((void *)m_RData, (const void *)(pTmp+sPos+2), length);

				m_RData[length] = 0;

				foundCore = TRUE;
				break;
			}
			else
			{
				m_pCB->HeadIncrease(3);
				break;
			}
		}
	}
	if (foundCore) m_pCB->HeadIncrease(6 + length); //6: header 2+ end 2+ length 2

	delete[] pTmp;

	return foundCore;

cancelRoutine:
	if(pTmp) delete[] pTmp;
	return foundCore;
}
/*
BOOL USER::PullOutCore(char *&data, int &length)
{
	BYTE		*pTmp;
	int			len = 0, sPos = 0, ePos = 0;
	BOOL		foundCore = FALSE;
	MYSHORT		slen;

	len = m_pCB->GetValidCount();

	if(len == 0 || len < 0)
	{
		data = NULL;
		return FALSE;
	}

	pTmp = new BYTE[len];
	m_pCB->GetData((char*)pTmp, len);

	for (int i = 0; i < len && !foundCore; i++)
	{
		if (i + 2 >= len) break; 

		if (pTmp[i] == PACKET_START1 && pTmp[i + 1] == PACKET_START2)
		{
			sPos = i + 2;

			slen.b[0] = pTmp[sPos];
			slen.b[1] = pTmp[sPos + 1];

			length = (int)slen.i;

			if (length <= 0) goto cancelRoutine;
			if (length > len) goto cancelRoutine;
			ePos = sPos + length + 2;

			if((ePos + 2) > len ) goto cancelRoutine;

			if (pTmp[ePos] == PACKET_END1 && pTmp[ePos+1] == PACKET_END2)
			{
				data = new char[length+1];
				::CopyMemory((void *)data, (const void *)(pTmp+sPos+2), length);
				data[length] = 0;
				foundCore = TRUE;
				break;
			}
			else 
			{
				data = NULL;
				m_pCB->HeadIncrease(3);
				break;
			}
		}
	}
	if (foundCore) m_pCB->HeadIncrease(6 + length); //6: header 2+ end 2+ length 2

	delete[] pTmp;

	return foundCore;

cancelRoutine:
	data = NULL;
	delete[] pTmp;
	return foundCore;
}
*/
//void USER::Parse(int len, TCHAR *pBuf)
void USER::Parse(int len, TCHAR *pBuf, BOOL &bEncryption)
{
/*	int index = 0;
	BYTE command = GetByte(pBuf+ index, index);
\ 
	UserTimer();
*/
	int index = 0;
	DWORD tempVer = 0;
	DWORD dwValue = 0;

	BYTE command = 0;

	if(bEncryption)
	{
		dwValue = GetDWORD(m_RData + index, index);

		tempVer = dwValue - m_Recv_value;

		if(tempVer > 100) return;		// ��Ŷ ��ȣȭ �ӽ� �׽�Ʈ����

		m_Recv_value = dwValue;
	}

	command = GetByte(m_RData, index);
	if(command == 0)
		bEncryption = TRUE;

	UserTimer();

	// Before Game Start...
	if(command == CHECK_ALIVE_REQ) CheckServerTest();
	else if(command == ENCRYPTION_START_REQ) EncryptionStartReq(pBuf + index);
	else if(command == ANNOUNCE_NOTICE) AnnounceNotice(pBuf + index);

	if( !bEncryption ) return;		// ��ȣȭ�� �ȵǾ� ������ ����..	

	g_ThreadErr[m_iModSid] = command;

	if(command != CLIENT_TICK) 
	{
		m_dwNoDamageTime = 0;
		m_dwLastNoDamageTime = GetTickCount();
	}

	// Before Game Start...
	switch( command )
	{
//	case SESSION_LOGIN_REQ:
//		SessionLoginReq(pBuf + index);
//		break;
	//case 0:
//		GameStart_(pBuf + index+14);
	//	break;
	
//	case NEW_ACCOUNT_REQ:
//		NewAccountReq(pBuf + index);
//		break;
	
	case ACCOUNT_LOGIN_REQ:
		AccountLoginReq(pBuf + index);
//		AccountLoginReqWithThread(pBuf+index);
//		AccountLoginReqWithDbServer(pBuf+index);
		break;

	case NEW_CHAR_REQ:
		NewCharReq(pBuf + index);
//		NewCharReqWithThread(pBuf + index);
		break;

	case CHECK_ID_REQ:
		CheckIDReq(pBuf + index);
		break;

	case REROLL_REQ:
		RerollReq(pBuf + index);
		break;

	case DELETE_CHAR_REQ:
		DeleteCharReq(pBuf + index);
//		DeleteCharReqWithThread(pBuf + index);
		break;

	case GAME_START_REQ:
		GameStart(pBuf + index);
//		GameStartWithThread(pBuf + index);
//		GameStartWithDbServer(pBuf + index);
		break;

	case ZONE_LOGIN_REQ:
		ZoneLoginReq(pBuf + index);
		break;

	case LOGOUT_REQ:
		SoftClose();
//		LogOut();
		break;

	case CLIENT_TICK:
		if ( g_ChatEnable[m_iModSid] == TRUE ) CheckSpeedHack();
		break;

//	case CHECK_ALIVE_REQ:			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ �׽�Ʈ �ڵ���
//		CheckServerTest();
//		break;
//	case ANNOUNCE_NOTICE:
//		AnnounceNotice(pBuf + index);
//		break;
	default:
		break;
	}

	if( g_bShutDown )
	{
		SoftClose();
		return;
	}

	// Game Start...
	switch( command )
	{
	case CHAT_REQ:
//		if ( g_ChatEnable[m_iModSid] == TRUE )	ChatReq(pBuf + index);
		if ( g_ChatEnable[m_iModSid] == TRUE ) ChatReqWithThread(len-1, pBuf + index);
		break;

	case LIVE_REQ:
		LiveReq(pBuf + index);
		break;

	case REVIVAL_REQ://��֮��..����
		RevivalReq( pBuf + index );
		break;

	case USERINFO_REQ:
		UserInfoReq(pBuf + index);
		break;

	case CLIENT_TIMER:					// �ϴ� SP�� ȸ���Ѵ�.
//		UserTimer();
		break;

	case ITEM_TRADE_REQ:
		ItemTradeReq(pBuf + index);
		break;

	case EXCHANGE_REQ:
		ExchangeReq(pBuf + index);
		break;

	case RESTART_REQ:
		RestartReq(pBuf + index);
//		RestartReqWithThread(pBuf + index);
		break;

	case HELP_REQ:
		HelpStatusOpenReq();
		break;

	case SCREEN_CAPTURE_REQ:
		ScreenCaptureReq( );
		break;

	case EXPRESSION_REQ:
		ExpressionReq( pBuf + index );
		break;

	case EXPRESSION_OPEN:
		ExpressionOpen( );
		break;

	case PERSONAL_SHOP_REQ:
		PersonalShopReq( pBuf + index );
		break;


//	case SERVER_TICK_REQ:
//		SendServerTick();
//		break;

	default:
		break;
	}

	if(m_bLive == USER_DEAD) return;		// �Ʒ��� ��Ŷ�� ������ ������� ��츸 ó��
	if(m_bNowTrading == TRUE) return;		// Trade �߿��� ������ ���Ѵ�.
	if(m_bNoItemMove == TRUE) return;		// �������� ������ �� ���� �����̸� ����
	if(m_bPShopOpen == TRUE) return;		// User has Personal shop open
	if(m_bViewingAShop == TRUE) return;		// A User is viewing another users shop
	if(m_state != STATE_GAMESTARTED) return;
	if( m_bZoneLogOut ) return;

	switch( command )
	{
	case CLIENT_EVENT:
		ClientEvent(pBuf+index);
		break;

	case BANK_ITEM_MOVE_REQ:
		BankItemMoveReq(pBuf + index);
		break;

	case BUY_ITEM:
		BuyItem(pBuf + index);
		break;

	case BUY_PSY_REQ:
		BuyPsi(pBuf + index);
		break;

	case SELL_ITEM:
		SellItem(pBuf + index);
		break;

	case REPAIR_ITEM:
		RepairItem(pBuf + index);
		break;

	case MOVE_CHAT_REQ:
		MoveChatReq(pBuf + index);
		break;

	case CHANGE_DIR_REQ:
		ChangeDir(pBuf + index);
		break;

	case MOVE_FIRST_REQ:
		MoveFirstReq(pBuf + index);
		break;
	case MOVE_REQ:
		MoveReq(pBuf + index);
		break;
	case MOVE_END_REQ:
		MoveEndReq(pBuf + index);
		break;

	case RUN_FIRST_REQ:
		RunFirstReq(pBuf + index);
		break;
	case RUN_REQ:
		RunReq(pBuf + index);
		break;
	case RUN_END_REQ:
		RunEndReq(pBuf + index);
		break;
	
	case FIND_USER_REQ:
		FindUserReq(pBuf + index);
		break;

	case BBS_NEXT:
		BBSNext(pBuf + index);
//		BBSNextWithThread(pBuf + index);
		break;

	case BBS_READ:
		BBSRead(pBuf + index);
//		BBSReadWithThread(pBuf + index);
		break;

	case BBS_WRITE:
		BBSWrite(pBuf + index);
//		BBSWriteWithThread(pBuf + index);
		break;

	case BBS_EDIT:
		BBSEdit(pBuf + index);
//		BBSEditWithThread(pBuf + index);
		break;

	case BBS_DELETE:
		BBSDelete(pBuf + index);
//		BBSDeleteWithThread(pBuf + index);
		break;

	case PSI_ATTACK_REQ:
		PsiAttack(pBuf + index);
		break;

	case ATTACK_REQ:
		Attack(pBuf + index);
		break;

	case TRADE_MODE_REQ:
		SetTradeMode(pBuf + index);
		break;

	case ITEM_MOVE_REQ: //�ƶ�װ��������Ʒ ����װ����.
		ItemMoveReq(pBuf + index);
		break;

	case ITEM_GIVE_REQ:
		ItemGiveReq(pBuf + index);
		break;

	case ITEM_LOAD_REQ:
		ItemLoadReq(pBuf + index);
		break;

	case ITEM_USE_REQ:
		ItemUseReq(pBuf + index);
		break;

	case CHANGE_WEAPON_REQ:
		ChangeWeaponReq(pBuf + index);
		break;

	case SKILL_ONOFF_REQ:
		SkillOnOffReq(pBuf + index);
		break;

	case PSI_CHANGE_REQ:
		PsiChangeReq(pBuf + index);
		break;

	case USER_STATUS_SKILL_UP_REQ:
		UserStatusSkillUp(pBuf + index);
		break;

	case BUDDY_MODE_REQ: 
		SetBuddyMode(pBuf + index);
		break;

	case UPGRADE_ITEM_REQ:
//		SendSystemMsg( IDS_USER_UNDER_CONSTRUCTION, SYSTEM_NORMAL, TO_ME);
//		UpgradeItemReq(pBuf + index);
		UpgradeItemReqBegin(pBuf + index);
		break;

	case GUILD_OPEN_REQ:
		NewGuild(pBuf + index);
//		NewGuildWithThread(pBuf + index);
		break;

	case GUILD_WAREHOUSE_CLOSE:
		GuildWharehouseCloseReq();
		break;

	case GUILD_ADD_REQ:
		GuildAddReq(pBuf + index);
		break;

	case GUILD_MARK_ADD_REQ:
		GuildMarkAddReq(pBuf + index);
//		GuildMarkAddReqWithThread(pBuf + index);
		break;

	case GUILD_ITEM_MOVE_REQ:
		GuildItemMoveReq(pBuf + index);
//		GuildItemMoveReqWithThread(pBuf + index);
		break;

	case GUILD_USER_INFO_REQ:
		GuildUserInfoReq(pBuf + index);
		break;

	case GUILD_TAX_REQ:
		GetStoreTax(pBuf + index);
		break;

	case GUILD_WAR_REQ:
		GetGuildWarScheme(pBuf + index);
		break;

	case GUILD_WAR_APPLY_REQ:
		GuildWarApplyReq(pBuf + index);
		break;

	case GUILD_FIELD_WAR_RESULT:
		SendGuildWarFieldApplyResult(pBuf + index);
		break;

	case GUILD_HOUSE_APPLY_REQ:
		MassZoneMove(pBuf + index);
		break;

	case TELEPORT_REQ:
		TeleportReq();
		break;

	case TELEPORT_EDIT_REQ: 
		TelportEdit(pBuf + index);
		break;

	case TELEPORT_COPY_REQ:
		TeleportCopyReq(pBuf + index);
		break;

	case TELEPORT_COPY_RESULT:
		TeleportCopyResult(pBuf + index);
		break;

	case EVENT_UPGRADE_ITEM_REQ:
		EventUpgradeItemReq(pBuf + index);
		break;

	case ADDRESS_WINDOW_OPEN:
	case ADDRESS_WINDOW_OPEN_NEW:
		RecvAddress( pBuf + index );
		break;

	case PRE_REG_USER_EVENT_REQ:
		RecvEventItemSerial( pBuf + index );
		break;

	case PRE_REG_GAMEROOM_EVENT_REQ:
		RecvEventItemSerialForGameRoom( pBuf + index );
		break;

	case ACCOUNT_BANK_ITEM_MOVE_REQ:
		AccountBankItemMoveReq( pBuf + index );
		break;

	case DRESSING:
		m_bNoItemMove = TRUE;
		DressingReq( pBuf + index );
		m_bNoItemMove = FALSE;
		break;

	case SELECT_OVER100_SKILL:
		SelectOver100Skill( pBuf+index );
		break;

	case GUILD_INVITE_REQ:
		SendGuildInviteReq( pBuf+index );
		break;

	case GUILD_INVITE_RESULT:
		GuildInviteResult( pBuf+index );
		break;

	case QUESTWINDOW_OPEN_REQ:
		QuestWindowOpenReq( pBuf + index);
		break;

	case LOVE_NAME_REQ://--yskang 0.1
		LoveName(pBuf+index);
		break;

	// Mail system by zinee 02-11-15
	case MAIL_INFO_REQ:
		MailInfoReq( pBuf+index );
		break;

	case MAIL_SEND_REQ:
		MailSendReq( pBuf+index );
		break;

	case MAIL_LIST_REQ:
		MailListReq( pBuf+index );
		break;

	case MAIL_CONTENTS_REQ:
		MailContentReq( pBuf+index );
		break;

	case MAIL_DELETE_REQ:
		MailDeleteReq( pBuf+index );
		break;

	case EBODY_IDENTIFY_REQ:
		EBodyIdentifyReq( pBuf + index );
		break;

	case PK_BUTTON_REQ:
		TogglePKButton( pBuf + index );
		break;

	case TRANSFORM_REQ:
		TransformReq( pBuf + index  );
		break;
	case 68: //��ױ����
		Face_painting(pBuf + index,len);
		break;
	case 30: //����
		DianGuangBan(pBuf + index,len);
		break;
	default:
		break;
	}
}


//////////////////////////////////////////////////////////////////////
//��ױ���� m_iSkin m_iHair m_sGender m_strFace
//////////////////////////////////////////////////////////////////////
void USER::Face_painting(TCHAR *pBuf,int len)
{
	int index=0,i;
	byte sGender;

	int slot=GetShort(pBuf, index); //ʹ����Ʒλ��
	if(m_UserItem[slot].sSid!=1000 && m_UserItem[slot].sSid!=1004 && m_UserItem[slot].sSid!=1018)
		return ;
	for(i=0;i<10;i++){
		m_strFace[i]=GetByte(pBuf, index);
	}
	sGender=GetByte(pBuf, index);
	if(sGender==1){
		m_sGender=m_sGender+1;
		if(m_sGender>1)
			m_sGender=0;
	}
	
	SendMyInfo(TO_INSIGHT, INFO_MODIFY);

	CBufferEx	TempBuf;
	TempBuf.Add((byte)68);
	Send(TempBuf, TempBuf.GetLength());

	CBufferEx	TempBuf1;
	ReSetItemSlot(&m_UserItem[slot]);
	TempBuf1.Add(ITEM_GIVE_RESULT);
	TempBuf1.Add((BYTE)0x1);
	TempBuf1.Add((BYTE)slot);
	TempBuf1.Add(m_UserItem[slot].sLevel);
	TempBuf1.Add(m_UserItem[slot].sSid);
	TempBuf1.Add(m_UserItem[slot].sDuration);
	TempBuf1.Add(m_UserItem[slot].sBullNum);
	TempBuf1.Add(m_UserItem[slot].sCount);
	for(int j = 0; j < MAGIC_NUM; j++) TempBuf1.Add(m_UserItem[slot].tMagic[j]);
	TempBuf1.Add(m_UserItem[slot].tIQ);
	Send(TempBuf1, TempBuf1.GetLength());

}
//////////////////////////////////////////////////////////////////////
//����
//////////////////////////////////////////////////////////////////////
void USER::DianGuangBan(TCHAR *pBuf,int len)
{
//	SaveDump(pBuf,len);
	CBufferEx	TempBuf,TempBuf1;
	int index=1;
	

	int slot=GetShort(pBuf, index); //ʹ����Ʒλ��
	if(slot<10||slot>34)
		return ;
	if(m_UserItem[slot].sSid!=1021 ||m_UserItem[slot].sDuration<=0)
		return ;
	SendDuration(slot, 1);
	if(m_UserItem[slot].sDuration<=0){
		ReSetItemSlot(&m_UserItem[slot]);
		TempBuf1.Add(ITEM_GIVE_RESULT);
		TempBuf1.Add((BYTE)0x1);
		TempBuf1.Add((BYTE)slot);
		TempBuf1.Add(m_UserItem[slot].sLevel);
		TempBuf1.Add(m_UserItem[slot].sSid);
		TempBuf1.Add(m_UserItem[slot].sDuration);
		TempBuf1.Add(m_UserItem[slot].sBullNum);
		TempBuf1.Add(m_UserItem[slot].sCount);
		for(int j = 0; j < MAGIC_NUM; j++) TempBuf1.Add(m_UserItem[slot].tMagic[j]);
		TempBuf1.Add(m_UserItem[slot].tIQ);
		Send(TempBuf1, TempBuf1.GetLength());
	//	SaveDump(TempBuf1,TempBuf1.GetLength());

	}
	TempBuf.Add((byte)0x1f);
	TempBuf.Add((byte)1);
	TempBuf.Add((byte)0x25);
	TempBuf.Add((DWORD)0);
	TempBuf.AddString(m_strUserID);
	TempBuf.AddString(pBuf+4);
	
	SendAll(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////////////
//	Session DB�� Account �� �ִ��� Ȯ���ϴ� Stored Proc
//
BOOL USER::CheckSessionLoginReq(TCHAR *strAccount, TCHAR *strPassword, int& nCount)
{
	nCount = 0;
	
	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	BOOL bQuerySuccess = TRUE;
	
	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LoginReq (\'%s\', \'%s\',?)}"), strAccount, strPassword);
	
	SQLSMALLINT	sCount = 0;
	SQLINTEGER sCountInd;

	hstmt = NULL;

	int db_index = -1;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;
	
	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
//		g_DBSession[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLBindParameter(hstmt,1 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sCount, 0, &sCountInd);
	if (retcode!=SQL_SUCCESS) 
	{
//		g_DBSession[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLExecDirect (hstmt, (unsigned char *)szSQL, SQL_NTS);
	if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (TRUE)
		{
			retcode = SQLFetch(hstmt);
			if (retcode !=SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO)
			{
				break;
			}
		}
	}
	else if (retcode==SQL_ERROR)
	{
		bQuerySuccess = FALSE;
		DisplayErrorMsg(hstmt);
	}
	else if (retcode==SQL_NO_DATA)
	{
		bQuerySuccess = FALSE;
	}
	
	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

	if( !bQuerySuccess ) 
	{
//		BREAKPOINT();
		g_DBSession[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	nCount = sCount;

	g_DBSession[m_iModSid].ReleaseDB(db_index);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//	�̹� ���� Account �� ������ ������ �ִ��� �˻��Ѵ�.
//
BOOL USER::IsDoubleAccount(char *account)
{
	if( !strlen( account ) ) return TRUE;

	CString strSource = account;

	// IKING 2001.1.
	USER *pUser = NULL;
	for( int i = 0; i < MAX_USER; i++)
	{
		if(i == m_uid) continue;

		pUser = m_pCom->GetUserUid(i);
		if( pUser == NULL ) continue;

		if( !strSource.CompareNoCase(pUser->m_strAccount))
		{
			if( pUser->m_state != STATE_DISCONNECTED && pUser->m_state != STATE_LOGOUT)
			{
			//	pUser->SendSystemMsg( IDS_USER_DOUBLE_ACCOUNT, SYSTEM_SPECIAL, TO_ME);				
			//	pUser->SoftClose();
				return TRUE;
			}
		}
	}
	//

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////
//	Session �� Login ���� ���� ó��
//
void USER::SessionLoginReq(TCHAR *pBuf)
{
	int			index = 0, nCount;
	TCHAR		strAccount[ACCOUNT_LENGTH+1], strPassword[PASSWORD_LENGTH+1];
	BYTE		result = FAIL, error_code = 0;
	CBufferEx	TempBuf;

	::ZeroMemory(strAccount, sizeof(strAccount));
	::ZeroMemory(strPassword, sizeof(strPassword));

	if(!GetVarString(sizeof(strAccount), strAccount, pBuf, index)) return;
	if(!GetVarString(sizeof(strPassword), strPassword, pBuf, index)) return;
	
	if( IsDoubleAccount( strAccount ) )
	{
		TRACE( "DOUBLE ACCOUNT - %s\n", strAccount );
		error_code = ERR_3;
		goto result_send;
	}

	if(CheckSessionLoginReq(strAccount, strPassword, nCount) == FALSE)
	{
		error_code = UNKNOWN_ERR;
		goto result_send;
	}
	
	if(nCount == 0) result = SUCCESS;
	else error_code = (BYTE)nCount;

result_send:

	TempBuf.Add(SESSION_LOGIN_RESULT);
	TempBuf.Add(result);

	if(result != SUCCESS)
	{
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		SoftClose();
	}
	else Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////////
//	Session �� New Account �� �����.
//
void USER::NewAccountReq(TCHAR *pBuf)
{
	int		index = 0, nRet = -1;
	BYTE	result = FAIL;

	TCHAR	szUserID	[12 + 1];
	TCHAR	szPassword	[12 + 1];
	TCHAR	szRealName	[40 + 1];
	TCHAR	szSocNo		[15 + 1];
	TCHAR	szAddress	[80 + 1];
	TCHAR	szTel		[20 + 1];
	TCHAR	szEmail		[30 + 1];
	TCHAR	szQuiz01	[50 + 1];
	TCHAR	szAnswer01	[50 + 1];

	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];
	BOOL			bQuerySuccess = TRUE;
	CDatabase*		pDB = NULL;
	int				db_index = 0;
	SQLSMALLINT		sRet = 0;
	SQLINTEGER		sRetInd;

	if(!GetVarString(12, szUserID,			pBuf, index)) goto result_send;
	if(!GetVarString(12, szPassword,		pBuf, index)) goto result_send;
	if(!GetVarString(40, szRealName,		pBuf, index)) goto result_send;
	if(!GetVarString(15, szSocNo,			pBuf, index)) goto result_send;
	if(!GetVarString(80, szAddress,			pBuf, index)) goto result_send;
	if(!GetVarString(20, szTel,				pBuf, index)) goto result_send;
	if(!GetVarString(30, szEmail,			pBuf, index)) goto result_send;
	if(!GetVarString(50, szQuiz01,			pBuf, index)) goto result_send;
	if(!GetVarString(50, szAnswer01,		pBuf, index)) goto result_send;	
	
	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{? = call NewAccount (\'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\')}"), 
		szUserID, szPassword, szRealName, szSocNo, szAddress, szTel, szEmail, szQuiz01, szAnswer01);
	
	hstmt = NULL;
	
	pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
		//g_DBSession[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}

	retcode = SQLBindParameter(hstmt, 1 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &sRetInd);
	if (retcode!=SQL_SUCCESS) 
	{
		g_DBSession[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}

	retcode = SQLExecDirect (hstmt, (unsigned char *)szSQL, SQL_NTS);
	if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (TRUE)
		{
			retcode = SQLFetch(hstmt);
			if (retcode !=SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO)
			{
				break;
			}
		}
	}
	else if (retcode==SQL_ERROR)
	{
		bQuerySuccess = FALSE;
		DisplayErrorMsg(hstmt);
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DBSession[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}
	else if (retcode==SQL_NO_DATA)
	{
		bQuerySuccess = FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

	if(bQuerySuccess) nRet = sRet;

	g_DBSession[m_iModSid].ReleaseDB(db_index);

result_send:
		
	if(bQuerySuccess == TRUE && nRet == 0) result = SUCCESS;

	index = 0;
	SetByte(m_TempBuf, NEW_ACCOUNT_RESULT, index);
	SetByte(m_TempBuf, result, index);
	Send(m_TempBuf, index);
}

///////////////////////////////////////////////////////////////////////////////////
//	Session ���� �����۾��� ��ġ�� ĳ���͸� �����ϱ� ���� Login
//
void USER::AccountLoginReq(TCHAR *pBuf)
{
/*	if( g_bShutDown ) return;

	int		index = 0;
	BYTE	result = FAIL, error_code = 0;
	int		old_index = 0;
	TCHAR	szTemp[8096];

	TRACE("AccountLoginReq Request...Check");

#ifdef _GETVARSTRING_DEBUG
	char strFn[128];
	sprintf( strFn, "AccountLoginReq1" );
	int	nIDLength = GetVarString(m_strAccount, pBuf, sizeof(BYTE), index, strFn);
#endif
#ifndef _GETVARSTRING_DEBUG
	int	nIDLength = GetVarString(m_strAccount, pBuf, sizeof(BYTE), index);
#endif

	if(nIDLength == 0 || nIDLength > ACCOUNT_LENGTH)
	{
		error_code = 1;
		goto result_send;
	}

	if(IsDoubleAccount(m_strAccount))
	{
		error_code = ERR_2;
		goto result_send;
	}

	if(!LoadCharData(m_strAccount))
	{
		// Load Character Data Fail...
		error_code = UNKNOWN_ERR;
		goto result_send;
	}
	else
	{
		m_state = STATE_CONNECTED;
		result = SUCCESS;
	}

result_send:

	index = 0;
	SetByte(m_TempBuf, ACCOUNT_LOGIN_RESULT, index );
	SetByte(m_TempBuf, result, index );

	old_index = index;
	::CopyMemory(szTemp, m_TempBuf, old_index);

	if(result == SUCCESS)
	{
		SetByte(m_TempBuf, (BYTE)m_nCharNum, index);
	}
	else
	{
		SetByte(m_TempBuf, error_code, index);
		Send( m_TempBuf, index );
		SoftClose();
		return;
	}

	if(m_nCharNum != 0 && result == SUCCESS)
	{
		for(int i = 0; i < 3; i++)
		{
			if(m_strChar[i][0])
			{
				SetByte(m_TempBuf, (BYTE)i, index);
				if(!SendCharInfo(m_strChar[i], m_TempBuf, index))
				{
					result = FAIL;
					error_code = UNKNOWN_ERR;
					index = 0;
					SetByte(m_TempBuf, ACCOUNT_LOGIN_RESULT, index );
					SetByte(m_TempBuf, result, index );
					SetByte(m_TempBuf, error_code, old_index);
					Send(m_TempBuf, old_index);
					SoftClose();
					return;
				}
			}
		}
	}

//	UpdateCurrentUserTable();				// ���ǿ� ���� IP�� �����Ѵ�.

	Send(m_TempBuf, index);
*/
	if( g_bShutDown ) return;

	int		index = 0;
	BYTE	result = FAIL, error_code = 0;
	int		old_index = 0;
	TCHAR	szTemp[8096];

//	TRACE("AccountLoginReq Request...Check");

	int	nIDLength = GetVarString(ACCOUNT_LENGTH + 1, m_strAccount, pBuf, index);

	if(nIDLength == 0 || nIDLength > ACCOUNT_LENGTH)	
	{
		error_code = 1;
		goto result_send;
	}

	if(IsDoubleAccount(m_strAccount))
	{
		error_code = ERR_2;
		goto result_send;
	}

	if(!LoadCharData(m_strAccount))
	{
		// Load Character Data Fail...
		error_code = UNKNOWN_ERR;
		goto result_send;
	}
	else 
	{
		m_state = STATE_CONNECTED;
		result = SUCCESS;
	}

result_send:

	index = 0;
	SetByte(m_TempBuf, ACCOUNT_LOGIN_RESULT, index );
	SetByte(m_TempBuf, result, index );

	old_index = index;
	::CopyMemory(szTemp, m_TempBuf, old_index);

	if(result == SUCCESS)
	{
		SetByte(m_TempBuf, (BYTE)m_nCharNum, index);
	}
	else
	{
		SetByte(m_TempBuf, error_code, index);
		Send( m_TempBuf, index );
		SoftClose();
		return;
	}

	if(m_nCharNum != 0 && result == SUCCESS)
	{
		for(int i = 0; i < 3; i++)
		{
			if(m_strChar[i][0])	
			{
				SetByte(m_TempBuf, (BYTE)i, index);
				if(!SendCharInfo(m_strChar[i], m_TempBuf, index))
				{
					result = FAIL;
					error_code = UNKNOWN_ERR;
					index = 0;
					SetByte(m_TempBuf, ACCOUNT_LOGIN_RESULT, index );
					SetByte(m_TempBuf, result, index );
					SetByte(m_TempBuf, error_code, old_index);
					Send(m_TempBuf, old_index);
					SoftClose();
					return;
				}
			}
		}
	}

//	UpdateCurrentUserTable();				// ���ǿ� ���� IP�� �����Ѵ�.

	Send(m_TempBuf, index);
}

void USER::ZoneLoginReq(TCHAR *pBuf)
{
	int			index = 0;
	TCHAR		szPw[PASSWORD_LENGTH + 1];
	TCHAR		szID[CHAR_NAME_LENGTH+1];
	BYTE		result = FAIL, error_code = 0;
	CPoint		pt(-1, -1);
	CBufferEx	TempBuf;
	int			iMemory = 0;
	int			iMemoryAccountBank = 0;

	TRACE("ZoneLoginReq Request...Check\n");

	int nIDLength, nPwLength, nCharLength;
	int iMyServer = -1;
	USER *pDoubleUser = NULL;

	nIDLength = GetVarString( sizeof( m_strAccount ), m_strAccount, pBuf, index);
	if(nIDLength == 0 || nIDLength > CHAR_NAME_LENGTH)	
	{
		error_code = ERR_1;
		goto result_send;
	}
	nPwLength = GetVarString( sizeof( szPw ), szPw, pBuf, index);
	if(nPwLength == 0 || nPwLength > CHAR_NAME_LENGTH)	
	{
		error_code = ERR_1;
		goto result_send;
	}
	nCharLength = GetVarString( sizeof( szID ), szID, pBuf, index);
	if(nCharLength == 0 || nCharLength > CHAR_NAME_LENGTH)	
	{
		error_code = ERR_1;
		goto result_send;
	}
	
	// IKING 2002.1.
	iMyServer = GetShort(pBuf, index);

	m_iMyServer = iMyServer;

	if( !IsMyDBServer( m_iMyServer ) )
	{
		m_iMyServer = -1;
		error_code = 255;
		goto result_send;
	}
	//

	if(nIDLength == 0 || nIDLength > CHAR_NAME_LENGTH)	
	{
		error_code = ERR_1;
		goto result_send;
	}

	//���� ���̵� ������ ���� �Ѵ�...
	pDoubleUser = GetUser( szID );
	if( pDoubleUser != NULL ) 
	{  
		//double fors test
		error_code = ERR_1;
		goto result_send;
/*		if( pDoubleUser->m_state != STATE_DISCONNECTED && pDoubleUser->m_state != STATE_LOGOUT )
		{
			error_code = ERR_5;
			TempBuf.Add(GAME_START_RESULT);
			TempBuf.Add(FAIL);
			TempBuf.Add(error_code);
			Send(TempBuf, TempBuf.GetLength());

			pDoubleUser->SendSystemMsg( IDS_USER_DOUBLE_CHAR, SYSTEM_SPECIAL, TO_ME);
			pDoubleUser->SoftClose();
			return;
		} */
	}

	g_pMainDlg->BridgeServerUserZoneLogIn( m_uid, m_strAccount, szID );
	return;


	InitUser();
//	InitMemoryDB(m_uid);
	iMemory = CheckMemoryDB( szID );
//	iMemory = 0;		// �Ƶ� ���� �͵� ������, ���� ���� �͵� ������.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ����� ������.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ��͵� �־���.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 2;		// �Ƶڶ� ������ ���� ���� ������, ������ ���� ���� �־���.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�. (���߿� ��â�� MemoryDB���� �������� ��ƾ�� �������̴�.)

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserData( szID ) )
		{
			error_code = ERR_2;
			goto result_send;
		}
	}

	/*
	if( !IsZoneInThisServer(m_curz) )
	{
		ChangeServer(m_curz);
		
		ReInitMemoryDB();

		SoftClose();
		return;
	}
	*/

	m_state = STATE_CONNECTED;

	// zone, zoneindex�� ��ǥ ��ȯ...
//	ZoneChangeInfoSet(m_curz, m_curx, m_cury);

	pt = FindNearAvailablePoint_S(m_curx, m_cury);	// DB�� ����� ��ǥ�� ������ �� �ִ� ��ǥ���� �Ǵ�
	if(pt.x == -1 || pt.y == -1) 
	{
		error_code = ERR_4;
		goto result_send;
	}

//	CheckUserLevel();					// ����ü�跹���� 25������ �Ѿ��� ��� 

	m_curx = pt.x; m_cury = pt.y;
	SetUid(m_curx, m_cury, m_uid + USER_BAND );		// ���� ��ġ���� ����
	m_presx = -1;
	m_presy = -1;

	pt = ConvertToClient(m_curx, m_cury);

	GetMagicItemSetting();							// ���� �������� ���� �Ӽ�, ���������� ���� ������ �ݿ��Ѵ�.

	m_UserFlag = TRUE;

	result = SUCCESS;

	m_ConnectionSuccessTick = 0;


	InitMemoryDB(m_uid);

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
		if( !LoadAccountBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
	}

	// �� ü���� Ȯ�ι� ���� �ʱ�ȭ...
	m_strZoneIP = "";
	m_nZonePort	= -1;

	SetPsiAbnormalStatus();

result_send:
	TempBuf.Add(GAME_START_RESULT);
	TempBuf.Add(result);

	if(result != SUCCESS)
	{
		ReInitMemoryDB();

		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}
	
	SendCharData();						// ������ �� ������ ������.

	TempBuf.Add((short)m_curz);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);

	TempBuf.Add(m_tDir);

	TempBuf.Add((BYTE)g_GameTime);
	TempBuf.Add((BYTE)g_GameMinute);

	Send(TempBuf, TempBuf.GetLength());

	SetGameStartInfo();	

	SendSystemMsg( IDS_USER_OPERATOR_MAIL1, SYSTEM_NORMAL, TO_ME);
//	SendSystemMsg( IDS_USER_OPERATOR_MAIL2, SYSTEM_NORMAL, TO_ME);
}

void USER::AccountLoginReqWithThread(TCHAR *pBuf)
{
	char id[MAX_ID+1];
	int index = 0;
	BYTE	result = FAIL, error_code = 0;

	int	nIDLength = GetVarString(sizeof(id), id, pBuf, index);

	if(nIDLength == 0 || nIDLength > ACCOUNT_LENGTH)	
	{
		error_code = 1;
		index = 0;
		SetByte(m_TempBuf, ACCOUNT_LOGIN_RESULT, index );
		SetByte(m_TempBuf, result, index );
		SetByte(m_TempBuf, error_code, index);
		Send( m_TempBuf, index );
		SoftClose();
		return;
	}

	LOGINOUTTHREADDATA *pLIOTD;
	pLIOTD = new LOGINOUTTHREADDATA;
	pLIOTD->CODE = ACCOUNT_LOGIN_REQ;
	pLIOTD->UID = m_uid;
	memset(pLIOTD->ID, NULL, CHAR_NAME_LENGTH+sizeof(int)+1);
	memcpy(pLIOTD->ID, pBuf, index );

	EnterCriticalSection( &m_CS_LoginData );
	RecvLoginData.AddTail(pLIOTD);
	nLoginDataCount = RecvLoginData.GetCount();
	LeaveCriticalSection( &m_CS_LoginData );
}

///////////////////////////////////////////////////////////////////////////////////
//	Character ������ �����´�.
//
BOOL USER::LoadCharData(TCHAR *id)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	BOOL bFind = FALSE;
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_CHAR_DATA (\'%s\')}")/*, m_iMyServer*/, id );
	
	SQLCHAR		strChar[3][CHAR_NAME_LENGTH+1];
	SQLINTEGER	strCharInd[3];

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	for( int i = 0; i < 3; i++)
	{
		::ZeroMemory(strChar[i], CHAR_NAME_LENGTH+1);
		strCharInd[i] = SQL_NTS;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if( retcode != SQL_SUCCESS )
	{
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS );
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		retcode = SQLFetch( hstmt );

		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
		{
			SQLGetData( hstmt, 1, SQL_C_CHAR, strChar[0], CHAR_NAME_LENGTH, &strCharInd[0] );
			SQLGetData( hstmt, 2, SQL_C_CHAR, strChar[1], CHAR_NAME_LENGTH, &strCharInd[1] );
			SQLGetData( hstmt, 3, SQL_C_CHAR, strChar[2], CHAR_NAME_LENGTH, &strCharInd[2] );
		}
		else 
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		//BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	m_nCharNum = 0;
	int nSize = sizeof(m_strChar);
	::ZeroMemory(m_strChar, sizeof(m_strChar));

	_tcscpy( m_strChar[0], (LPCTSTR)strChar[0]);		if( strlen( m_strChar[0] ) ) m_nCharNum++;
	_tcscpy( m_strChar[1], (LPCTSTR)strChar[1]);		if( strlen( m_strChar[1] ) ) m_nCharNum++;
	_tcscpy( m_strChar[2], (LPCTSTR)strChar[2]);		if( strlen( m_strChar[2] ) ) m_nCharNum++;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//	Client�� Character �⺻ ������ ������.
//
BOOL USER::SendCharInfo(TCHAR *strChar, TCHAR* pBuf, int& index)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	BOOL			bFind = FALSE;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call SEND_CHAR_DATA (\'%s\')}"), strChar );
	
	SQLCHAR		strCharID[CHAR_NAME_LENGTH + 1], strFace[10];
	SQLINTEGER	strCharInd = SQL_NTS;

	SQLSMALLINT	sSTR, sCON, sDEX, sVOL, sWIS, sGender;
	SQLINTEGER	sInd;

	SDWORD		dSkin, dHair;

	SQLSMALLINT	sLevel;
	SQLCHAR		byClass;

	SQLSMALLINT	sHP, sPP, sSP;

	SQLCHAR		strSkill[_SKILL_DB], strItem[_ITEM_DB];

	::ZeroMemory(strCharID, sizeof(strCharID));
	::ZeroMemory(strFace, sizeof(strFace));
	::ZeroMemory(strSkill, sizeof(strSkill));
	::ZeroMemory(strItem, sizeof(strItem));

	sSTR = sCON = sDEX = sVOL = sWIS = sGender = 0;
	dSkin = dHair = 0;

	byClass = 0;
	sLevel = 1;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS );
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		retcode = SQLFetch( hstmt );

		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
		{
			int i = 1;

			SQLGetData( hstmt, i++,	SQL_C_CHAR,		strCharID,	sizeof(strCharID),	&strCharInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sSTR,		sizeof(sSTR),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sCON,		sizeof(sCON),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sDEX,		sizeof(sDEX),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sVOL,		sizeof(sVOL),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sWIS,		sizeof(sWIS),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_ULONG,	&dSkin,		sizeof(dSkin),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_ULONG,	&dHair,		sizeof(dHair),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sGender,	sizeof(sGender),	&sInd);

			SQLGetData( hstmt, i++, SQL_C_BINARY,	strFace,	sizeof(strFace),	&sInd);

			SQLGetData( hstmt, i++,	SQL_C_TINYINT,	&byClass,	sizeof(byClass),	&sInd);

			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sLevel,	sizeof(sLevel),		&sInd);

			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sHP,		sizeof(sHP),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sPP,		sizeof(sPP),		&sInd);
			SQLGetData( hstmt, i++,	SQL_C_SSHORT,	&sSP,		sizeof(sSP),		&sInd);

			SQLGetData( hstmt, i++,	SQL_C_BINARY,	strSkill,	sizeof(strSkill),	&sInd);
			SQLGetData( hstmt, i++,	SQL_C_BINARY,	strItem,	sizeof(strItem),	&sInd);
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);			
			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	TCHAR strID[CHAR_NAME_LENGTH+1];
	_tcscpy(strID, (TCHAR*)strCharID);
	short nSTR = sSTR; 
	short nCON = sCON;
	short nDEX = sDEX;
	short nVOL = sVOL;
	short nWIS = sWIS;

	short	sMaxHP = 0;
	short	sMaxPP = 0;
	short	sMaxSP = 0;

	DWORD dwSkin = dSkin;
	DWORD dwHair = dHair;

	BYTE  cGender = (BYTE)sGender;

	CBufferEx TempBuf;
	TempBuf.AddString(strID);
	TempBuf.Add(nSTR);
	TempBuf.Add(nCON);
	TempBuf.Add(nDEX);
	TempBuf.Add(nVOL);
	TempBuf.Add(nWIS);
	TempBuf.Add(dwSkin);
	TempBuf.Add(dwHair);
	TempBuf.Add(cGender);

	TempBuf.AddData((LPTSTR)strFace, 10);

	TempBuf.Add(byClass);
	TempBuf.Add(sLevel);
	TempBuf.Add((short)sHP);
	TempBuf.Add((short)sPP);
	TempBuf.Add((short)sSP);

	sMaxHP	= g_sHPConst[byClass] * nCON + g_sHPLV[byClass] * (sLevel - 1) + g_sHPAdd[byClass];
	sMaxPP	= g_sPPConst[byClass] * nWIS + g_sPPLV[byClass] * (sLevel - 1) + g_sPPAdd[byClass];
	sMaxSP	= g_sSPConst[byClass] * nCON + g_sSPLV[byClass] * (sLevel - 1) + g_sSPAdd[byClass];

	TempBuf.Add((short)sMaxHP);
	TempBuf.Add((short)sMaxPP);
	TempBuf.Add((short)sMaxSP);

	// Parse Skill
	CHAR szSkillBuf[3 * TOTAL_SKILL_NUM];
	::ZeroMemory(szSkillBuf, sizeof(szSkillBuf));
	if(!GetSkillNameLevel(byClass, (LPTSTR)strSkill, szSkillBuf)) return FALSE;

	// Parse Item
	TCHAR szItemBuf[2 * EQUIP_ITEM_NUM];		// 2 = Item Pid, 10 = Totoal Equip Slot Count
	::ZeroMemory(szItemBuf, sizeof(szItemBuf));

	if(!GetEquipItemPid((LPTSTR)strItem, szItemBuf))
	{
		return FALSE;
	}

	TempBuf.AddData(szSkillBuf, 3 * TOTAL_SKILL_NUM);
	TempBuf.AddData(szItemBuf, 2* EQUIP_ITEM_NUM);

	::CopyMemory(pBuf + index, TempBuf, TempBuf.GetLength());
	index += TempBuf.GetLength();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//	���ο� ĳ���͸� �����.
//
void USER::NewCharReq(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[4096];
	
	SDWORD			sFaceLen = 10;
	SDWORD			sSkillLen	= _SKILL_DB;
	SDWORD			sPsiLen		= _PSI_DB;
	SDWORD			sItemLen	= _ITEM_DB;

	int db_index = 0;
	CDatabase* pDB;
	int				i;

	const	int	nInitMax = 25; 

	int		SumPoint = 0;
	int		index = 0;
	TCHAR	szAccount[ACCOUNT_LENGTH + 1], szName[CHAR_NAME_LENGTH + 1], szFace[10];
	BYTE	result = FAIL, error_code = 0;
	BYTE	nCharNum	= 0;
	int		nTempLength = 0;

	CString szTemp		= _T("");
	short	nSTR		= 0;
	short	nCON		= 0;
	short	nDEX		= 0;
	short	nVOL		= 0;
	short	nWIS		= 0;

	short	sHP = 0;
	short	sPP = 0;
	short	sSP = 0;

	DWORD	dwSkin		= 0;
	DWORD	dwHair		= 0;

	BYTE	cGender		= 0;
	BYTE	byClass		= 0;

	if(!GetVarString(sizeof(szAccount), szAccount, pBuf, index)) {error_code = 1; goto result_send; }
	nCharNum = GetByte(pBuf, index);
	if(!GetVarString(sizeof(szName), szName, pBuf, index)) {error_code = 2; goto result_send; }

	nTempLength = _tcslen(szAccount);
	if(!szAccount[0] ||  nTempLength > ACCOUNT_LENGTH)
	{
		error_code = 1;
		goto result_send;
	}

	if(m_nCharNum >= 3 || nCharNum > 2 || nCharNum < 0)
	{
		error_code = 2;
		goto result_send;
	}

	nTempLength = _tcslen(szName);
	if(nTempLength == 0 || nTempLength > CHAR_NAME_LENGTH) 
	{
		error_code = 8;
		goto result_send;
	}

	szTemp = szName;

	if(!UNI_CHAR::CheckString(szTemp))
	{
		result = FAIL;
		error_code = 8;
		index = 0;
		SetByte(m_TempBuf, NEW_CHAR_RESULT, index);
		SetByte(m_TempBuf, result, index);
		SetByte(m_TempBuf, error_code, index);
		Send(m_TempBuf, index);
		return;
	}
/*	if(szTemp.Find(' ') != -1 || szTemp.Find('\'') != -1 || szTemp.Find('"') != -1 || szTemp.Find('.') != -1 || szTemp.Find(',') != -1)
	{
		result = FAIL;
		error_code = 8;
		index = 0;
		SetByte(m_TempBuf, NEW_CHAR_RESULT, index);
		SetByte(m_TempBuf, result, index);
		SetByte(m_TempBuf, error_code, index);
		Send(m_TempBuf, index);
		return;
	}
*/
	if(IsReservedID(szName))
	{
		result = FAIL;
		error_code = 7;
		index = 0;
		SetByte(m_TempBuf, NEW_CHAR_RESULT, index);
		SetByte(m_TempBuf, result, index);
		SetByte(m_TempBuf, error_code, index);
		Send(m_TempBuf, index);
		return;
	}

	switch(m_iMyServer)
	{
	case	0:	// �׽�Ʈ
		break;
	case	1:	// ��ī�罺
		strcat( szName, "[I]" );
		break;
	case	2:	// ���ĵ���
		strcat( szName, "[E]" );
		break;
	case	3:	// ��������
		strcat( szName, "[K]" );
		break;
	case	4:	// �ʸ���
		strcat( szName, "[F]" );
		break;
	case	5:	// �ƽ�Ʈ
		strcat( szName, "[A]" );
		break;
	case	6:	// �׶���
		strcat( szName, "[G]" );
		break;
	case	7:	// �似�콺
		strcat( szName, "[P]" );
		break;
	case	8:	// �ж��
		strcat( szName, "[M]" );
		break;
	case	9:	// ���̳�Ʈ
		strcat( szName, "[R]" );
		break;
	case	10:	// Ÿ����
		strcat( szName, "[T]" );
		break;
	default:
		result = FAIL;
		error_code = 255;
		index = 0;
		SetByte(m_TempBuf, NEW_CHAR_RESULT, index);
		SetByte(m_TempBuf, result, index);
		SetByte(m_TempBuf, error_code, index);
		Send(m_TempBuf, index);
		return;
	}

	// �����ϴ� ID���� �˻�
	if(IsExistCharId(szName))
	{
		error_code = 7;
		goto result_send;
	}

	nSTR		= (short)m_sTempSTR;
	nCON		= (short)m_sTempCON;
	nDEX		= (short)m_sTempDEX;
	nVOL		= (short)m_sTempVOL;
	nWIS		= (short)m_sTempWIS;

	sHP = 0;
	sPP = 0;
	sSP = 0;

	dwSkin		= GetDWORD(pBuf, index);
	dwHair		= GetDWORD(pBuf, index);

	cGender		= GetByte (pBuf, index);

	::ZeroMemory(szFace, sizeof(szFace));
	GetString(szFace, pBuf, 10, index);
	
	byClass		= GetByte(pBuf, index);
	if(byClass != m_tTempClass) 
	{
		error_code = 10;
		goto result_send;
	}

	if(byClass < 0 || byClass > 3)
	{
		error_code = 10;
		goto result_send;
	}

	//-------------[ �ʱ� �ɷ�ġ �˻� ]-----------------------------//
	if(SumPoint > 65)		// �ʱ� �ɷ�ġ�� ���� 65
	{
		error_code = 9;
		goto result_send;
	}

	if(nSTR < 1 || nSTR > nInitMax ||
	   nCON < 1 || nCON > nInitMax ||
	   nDEX < 1 || nDEX > nInitMax ||
	   nVOL < 1 || nVOL > nInitMax ||
	   nWIS < 1 || nWIS > nInitMax)
	{
		error_code = 3;
		goto result_send;
	}

	//-----------[ �ɸ��� Reroll ��ġ �ʱ�ȭ ] -------------------------------//
	m_sTempSTR = 0;					
	m_sTempCON = 0;					
	m_sTempDEX = 0;					
	m_sTempVOL = 0;					
	m_sTempWIS = 0;					

	m_tTempClass = 255;

	SumPoint = nSTR + nCON + nDEX + nVOL + nWIS;

	// �Ǻλ�, �Ӹ���, ��Ÿ�Ͽ� ���� �˻�� ���Ŀ�... 

	pDB = g_DB[m_iModSid].GetDB( db_index );		// ������ ��� �������� üũ�ϰ� ���� �Ҵ�
	if( !pDB ) return ;
	
	sHP = g_sHPConst[byClass] * nCON + g_sHPAdd[byClass];
	sPP = g_sPPConst[byClass] * nWIS + g_sPPAdd[byClass];
	sSP = g_sSPConst[byClass] * nCON + g_sSPAdd[byClass];
	
	// Skill, Item, Psionic
	TCHAR strSkill[_SKILL_DB], strItem[_ITEM_DB], strPsi[_PSI_DB];
	::ZeroMemory(strSkill, sizeof(strSkill));
	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strPsi, sizeof(strPsi));

	sFaceLen = sizeof(szFace);
	sSkillLen = sizeof(strSkill);
	sItemLen = sizeof(strItem);
	sPsiLen = sizeof(strPsi);

	MakeEmptySkill(strSkill, byClass);
	MakeInintItem(strItem, byClass);
	MakeEmptyPsi(strPsi);

	// Stored Procedure Call
	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call NEW_CHAR (\'%s\', %d, \'%s\', %d, %d, %d, %d, %d, %d, %d, %d, %d, ?, ?, ?, ?, %d, %d, %d)}")/*, m_iMyServer*/, 
						szAccount, nCharNum, szName,
						nSTR, nCON, nDEX, nVOL, nWIS,
						dwSkin, dwHair,
						cGender,
						byClass, 
						sHP, sPP, sSP);

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if(retcode != SQL_SUCCESS )
	{
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}
	
	i = 1;
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(szFace),		0, (TCHAR*)szFace,		0, &sFaceLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strSkill),	0, (TCHAR*)strSkill,	0, &sSkillLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strPsi),		0, (TCHAR*)strPsi,		0, &sPsiLen );
	
	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS);
	if( retcode == SQL_SUCCESS || retcode == SQL_NEED_DATA || retcode == SQL_SUCCESS_WITH_INFO )
	{
	}
	else if( retcode == SQL_ERROR )
	{
		DisplayErrorMsg( hstmt );
		error_code = UNKNOWN_ERR;

		g_DB[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt );
	g_DB[m_iModSid].ReleaseDB(db_index);

	if(!LoadCharData(m_strAccount))
	{
		error_code = UNKNOWN_ERR;

		g_DB[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}
	else result = SUCCESS;

result_send:
	TCHAR FailBuf[8192];
	int nOldIndex = 0;
	index = 0;
	
	SetByte(m_TempBuf, NEW_CHAR_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result != SUCCESS)
	{
		SetByte(m_TempBuf, error_code, index);
		::CopyMemory(FailBuf, m_TempBuf, index);
		nOldIndex = index;
		Send(m_TempBuf, index);
		return;
	}

	SetByte(m_TempBuf, nCharNum, index);

	if(!SendCharInfo(szName, m_TempBuf, index))
	{
		Send(FailBuf, nOldIndex);
		return;
	}

	Send(m_TempBuf, index);
}

void USER::NewCharReqWithThread(TCHAR *pBuf)
{
	int index = 0;
	TCHAR	szAccount[ACCOUNT_LENGTH + 1], szName[CHAR_NAME_LENGTH + 1], szFace[11];

	if(!GetVarString(sizeof(szAccount), szAccount, pBuf, index)) return;
	BYTE	nCharNum		= GetByte(pBuf, index);
	if(!GetVarString(sizeof(szName), szName, pBuf, index))return;

	DWORD	dwSkin		= GetDWORD(pBuf, index);
	DWORD	dwHair		= GetDWORD(pBuf, index);
	BYTE	cGender		= GetByte (pBuf, index);
	
	GetString(szFace, pBuf, 10, index);

	BYTE	byClass		= GetByte(pBuf, index);

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = NEW_CHAR_REQ_WITH_THREAD;
	Datalength = index;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, pBuf, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );
}

////////////////////////////////////////////////////////////////////////////////
//	�⺻��ġ���� �����Ѵ�.
//
void USER::RerollReq(TCHAR *pBuf)
{
	int index = 0;
	BYTE tClass = GetByte(pBuf, index);
	BYTE tBasic[] = {13, 12, 11, 10, 9};
	
	int i = 0;
	int nSum = 0;
	int nRemain = 0;
	int nRand = 0;

	for(i = 0; i < 5; i++) nSum += tBasic[i];

	nRemain = 65 - nSum;
	while(nRemain > 0)
	{
		for(i = 0; i < 5; i++)
		{
			nRand = myrand(0, 100);
			if(nRand > 50) nRand = 1;
			else nRand = 0;

			if(nRand == 1 && tBasic[i] < 25) 
			{
				tBasic[i]++;
				nRemain--;
			}
			if(nRemain <= 0) break;
		}
	}

	i = 0;
	switch(tClass)
	{
	case 0:			// ����
		m_sTempCON = tBasic[i++];
		m_sTempDEX = tBasic[i++];
		m_sTempSTR = tBasic[i++];
		m_sTempVOL = tBasic[i++];
		m_sTempWIS = tBasic[i++];
		break;
	case 1:			// ������
		m_sTempWIS = tBasic[i++];
		m_sTempVOL = tBasic[i++];
		m_sTempCON = tBasic[i++];
		m_sTempDEX = tBasic[i++];
		m_sTempSTR = tBasic[i++];
		break;
	case 2:			// ����
		m_sTempSTR = tBasic[i++];
		m_sTempCON = tBasic[i++];
		m_sTempDEX = tBasic[i++];
		m_sTempWIS = tBasic[i++];
		m_sTempVOL = tBasic[i++];
		break;
	case 3:			// �ѱ�
		m_sTempDEX = tBasic[i++];
		m_sTempCON = tBasic[i++];
		m_sTempWIS = tBasic[i++];
		m_sTempVOL = tBasic[i++];
		m_sTempSTR = tBasic[i++];
		break;
	}

	index = 0;
	SetByte(m_TempBuf, REROLL_RESULT, index);
	SetByte(m_TempBuf, m_sTempSTR, index);
	SetByte(m_TempBuf, m_sTempCON, index);
	SetByte(m_TempBuf, m_sTempDEX, index);
	SetByte(m_TempBuf, m_sTempVOL, index);
	SetByte(m_TempBuf, m_sTempWIS, index);

	Send(m_TempBuf, index);
}

/////////////////////////////////////////////////////////////////////////////////
//	�̹� �����ϴ� ĳ�������� �˻��Ѵ�.
//
BOOL USER::IsExistCharId(TCHAR *id)
{
	/*
	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	BOOL bFind = FALSE;
	
	memset( szSQL, 0x00, 1024 );
	_sntprintf(szSQL, sizeof(szSQL), TEXT("SELECT strUserId FROM GAMEUSER WHERE strUserId = \'%s\'"), id );
	
	hstmt = NULL;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS );
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		retcode = SQLFetch( hstmt );

		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
		{
			bFind = TRUE;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return TRUE;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if(!bFind ) return FALSE;

	return TRUE;
	*/

	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	SQLSMALLINT	sRet;
	SQLINTEGER sRetInd = SQL_NTS;

	sRet = 0;

	memset( szSQL, 0x00, 1024 );
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call IS_EXIST_CHAR (\'%s\', ?)}"), id);
	
	hstmt = NULL;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		return FALSE;
	}

	retcode = SQLBindParameter(hstmt, 1 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &sRetInd);
	if( retcode != SQL_SUCCESS )
	{
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS );
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
//		retcode = SQLFetch( hstmt );
//
//		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
//		{
//		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return TRUE;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if( sRet == 0 ) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
//	ĳ���͸� �����Ѵ�.
//
void USER::DeleteCharReq(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	SQLHSTMT	hstmt = NULL;

	int		index = 0;
	TCHAR	szAccount[ACCOUNT_LENGTH+1], szChar[CHAR_NAME_LENGTH+1];
	TCHAR	szJumin[20], szIP[128];
	BYTE	result = FAIL, error_code = 0;

	::ZeroMemory(szJumin, sizeof(szJumin));
	::ZeroMemory(szIP, sizeof(szIP));

	int db_index = 0;
	CDatabase* pDB = NULL;

	int nAccountLength = 0, nCharLength = 0, nJuminCount = 0;
	BYTE CharIndex = 0;

	nAccountLength = GetVarString(sizeof(szAccount), szAccount, pBuf, index);
	if(nAccountLength == 0 || nAccountLength > ACCOUNT_LENGTH) 
	{
		error_code = 1;
		goto result_send;
	}

	nCharLength	= GetVarString(sizeof(szChar), szChar, pBuf, index);
	if(nCharLength == 0 || nCharLength > CHAR_NAME_LENGTH) 
	{
		error_code = 2;
		goto result_send;
	}

	CharIndex = GetByte(pBuf, index);

	nJuminCount	= GetVarString(sizeof(szJumin), szJumin, pBuf, index);
	if(nJuminCount == 0 || nJuminCount > 15) 
	{
		error_code = 5;
		goto result_send;
	}
	
//	GetVarString(szIP, pBuf, sizeof(BYTE), index);	���� IP�� �ϴ� �����Ѵ�.


	if(CharIndex < 0 || CharIndex > 2)
	{
		error_code = 3;
		goto result_send;
	}
	
	if(m_strChar[CharIndex][0] == 0) 
	{
		error_code = 3;
		goto result_send;
	}

	if(_stricmp(szChar, m_strChar[CharIndex]) != 0)
	{
		error_code = 2;
		goto result_send;
	}

	if(CheckJuminNumber(szAccount, szJumin) == FALSE)
	{
		error_code = 6;		// !!Check
		goto result_send;
	}

	if(!IsExistCharId(szChar)) 
	{
		error_code = 3;
		goto result_send;
	}
	
	pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	SQLRETURN		retcode;
	TCHAR			szSQL[1024];
	
	memset(szSQL, 0x00, 1024);
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{? = call DEL_CHAR (\'%s\', \'%s\', %d)}")/*, m_iMyServer*/, szAccount, szChar, CharIndex);
	
	SQLSMALLINT	sRet;
	SQLINTEGER sRetInd;	

	sRet = 0;
	
	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode != SQL_SUCCESS)
	{
		error_code = 4;

		//g_DB[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}
	
	retcode = SQLBindParameter(hstmt, 1 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &sRetInd);
	if (retcode!=SQL_SUCCESS) 
	{
		error_code = 4;
		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}
	
	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS );
	
	if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (TRUE)
		{
			retcode = SQLFetch(hstmt);
			if (retcode !=SQL_SUCCESS && retcode !=SQL_SUCCESS_WITH_INFO)
			{
				break;
			}
		}
	}
	else 
	{
		error_code = 4;
		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		goto result_send;
	}
	
	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if(sRet == 0) result = SUCCESS;
	else if(sRet == 100)
	{
		error_code = 7;
		goto result_send;
	}

	if( !LoadCharData(szAccount) )
	{
		// ��ε� ���� - �����Ϳ� �̻�
		error_code = UNKNOWN_ERR;
		goto result_send;
	}

result_send:
	index = 0;
	SetByte(m_TempBuf, DELETE_CHAR_RESULT, index);
	SetByte(m_TempBuf, result, index);
	if(result != SUCCESS) SetByte(m_TempBuf ,error_code, index);
	else SetByte(m_TempBuf, CharIndex, index);

	Send(m_TempBuf, index);
}

void USER::DeleteCharReqWithThread(TCHAR *pBuf)
{
	int		index = 0;
	BYTE	result = FAIL, error_code = 0;
	TCHAR	szAccount[20], szChar[30];
	TCHAR	szJumin[20], szIP[128];
	int		nAccountLength =0, nCharLength = 0, nJuminCount = 0;
	BYTE	CharIndex = 0;

	::ZeroMemory(szJumin, sizeof(szJumin));
	::ZeroMemory(szIP, sizeof(szIP));

	nAccountLength	= GetVarString(sizeof(szAccount), szAccount, pBuf, index);
	if(nAccountLength == 0 || nAccountLength > ACCOUNT_LENGTH) 
	{
		error_code = 1;
		goto result_send;
	}

	nCharLength	= GetVarString(sizeof(szChar), szChar, pBuf, index);
	if(nCharLength == 0 || nCharLength > CHAR_NAME_LENGTH) 
	{
		error_code = 2;
		goto result_send;
	}

	CharIndex = GetByte(pBuf, index);

	nJuminCount	= GetVarString(sizeof(szJumin), szJumin, pBuf, index);
	if(nJuminCount == 0 || nJuminCount > 15) 
	{
		error_code = 5;
		goto result_send;
	}

	if(!GetVarString(sizeof(szIP), szIP, pBuf, index)) return;

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = DELETE_CHAR_REQ;
	Datalength = index;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, pBuf, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );
	return;

result_send:
	index = 0;
	SetByte(m_TempBuf, DELETE_CHAR_RESULT, index);
	SetByte(m_TempBuf, result, index);
	if(result != SUCCESS) SetByte(m_TempBuf ,error_code, index);
	else SetByte(m_TempBuf, CharIndex, index);

	Send(m_TempBuf, index);
}

///////////////////////////////////////////////////////////////////////////////
//	���� ����
//
void USER::GameStart(TCHAR *pBuf)
{
	if( g_bShutDown ) return;

	int			index = 0;
	BYTE		result = FAIL, error_code = 0;
	TCHAR		szID[CHAR_NAME_LENGTH+1];
	CPoint		pt(-1, -1);
	CBufferEx	TempBuf;
	int			iMemory = 0;
	int			iMemoryAccountBank = 0;

	//TCHAR		bankstr[_BANK_DB];
	//TCHAR		accountbankstr[_ACCOUNT_BANK_DB];
	
	USER *pDoubleUser = NULL;

	int	nIDLength = GetVarString(sizeof(szID), szID, pBuf, index);

	if(nIDLength == 0 || nIDLength > CHAR_NAME_LENGTH)	
	{
		error_code = ERR_1;
		goto result_send;
	}

	int iMyServer, nAccountID;
	TCHAR		szAccountID[ACCOUNT_LENGTH+1];
#ifdef _CHINA
	iMyServer=1;
	index=index+2;
#else
	iMyServer = GetShort(pBuf, index);
#endif

	m_iMyServer = -1;

	if( !IsMyDBServer( iMyServer ) )
	{
		error_code = 255;
		goto result_send;
	}

	m_iMyServer = iMyServer;

	nAccountID = GetVarString(sizeof(szAccountID), szAccountID, pBuf, index);
	
	if(nAccountID == 0 || nAccountID > ACCOUNT_LENGTH)
	{
		error_code = 1;
		goto result_send;
	}
	
	strcpy(m_strAccount,szAccountID);

	if(IsDoubleAccount(m_strAccount))
	{
		error_code = ERR_1;
		goto result_send;
	}

	//���� ���̵� ������ ���� �Ѵ�...
	pDoubleUser = GetUser( szID );
	if( pDoubleUser != NULL ) 
	{
// fors test double char
		error_code = ERR_1;
		goto result_send;
/*		if( pDoubleUser->m_state != STATE_DISCONNECTED && pDoubleUser->m_state != STATE_LOGOUT )
		{
			error_code = ERR_5;
			TempBuf.Add(GAME_START_RESULT);
			TempBuf.Add(FAIL);
			TempBuf.Add(error_code);
			Send(TempBuf, TempBuf.GetLength());

			pDoubleUser->SendSystemMsg( IDS_USER_DOUBLE_CHAR, SYSTEM_SPECIAL, TO_ME);
			pDoubleUser->SoftClose();
			return;
		} */
	}

	g_pMainDlg->BridgeServerUserLogIn( m_uid, szAccountID, szID );

	// alisia - ���⼭ ����� �Ѵ�. ����� �޾Ƽ� ������ �����Ѵ�.
	return;



	InitUser();
//	InitMemoryDB(m_uid);
	iMemory = CheckMemoryDB( szID );
//	iMemory = 0;		// �Ƶ� ���� �͵� ������, ���� ���� �͵� ������.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ����� ������.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ��͵� �־���.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 2;		// �Ƶڶ� ������ ���� ���� ������, ������ ���� ���� �־���.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�. (���߿� ��â�� MemoryDB���� �������� ��ƾ�� �������̴�.)

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserData( szID ) )
		{
			error_code = ERR_2;
			goto result_send;
		}
	}
	

	if( !IsZoneInThisServer(m_curz) )
	{
		ChangeServer(m_curz);
		
		ReInitMemoryDB();

		SoftClose();
		return;
	}
//	SetZoneIndex(m_curz);

	m_state = STATE_CONNECTED;

	pt = FindNearAvailablePoint_S(m_curx, m_cury);	// DB�� ����� ��ǥ�� ������ �� �ִ� ��ǥ���� �Ǵ�
	if(pt.x == -1 || pt.y == -1) 
	{
		error_code = ERR_4;
		goto result_send;
	}

	CheckUserLevel();								// ����ü�跹���� 25������ �Ѿ��� ��� 

	m_curx = pt.x; m_cury = pt.y;
	SetUid(m_curx, m_cury, m_uid + USER_BAND );		// ���� ��ġ���� ����
	m_presx = -1;
	m_presy = -1;

	pt = ConvertToClient(m_curx, m_cury);

//	SetUserToMagicUser();							// ���� �ɷ�ġ�� ���� ���������� ���Ѵ�.
	GetMagicItemSetting();							// ���� �������� ���� �Ӽ�, ���������� ���� ������ �ݿ��Ѵ�.

	m_UserFlag = TRUE;

	result = SUCCESS;

	m_ConnectionSuccessTick = 0;


	InitMemoryDB(m_uid);

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
		if( !LoadAccountBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
	}

/*
	if( iMemory == 0 )
	{
		if( !LoadUserBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}

//		iMemoryAccountBank = CheckMemoryAccountBankDB( m_strAccount );

//		if( iMemoryAccountBank == 0 )
//		{
//			if( !LoadAccountBank() )
//			{
//				error_code = ERR_2;
//				result = FAIL;
//				goto result_send;
//			}
//		}
//		else
//		{
//			UserAccountBankItemToStr( accountbankstr );
//			SetMemAccountBank( accountbankstr );
//		}
	}
*/
result_send:
	TempBuf.Add(GAME_START_RESULT);
	TempBuf.Add(result);

	if(result != SUCCESS)
	{
		ReInitMemoryDB();

		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
//		SockCloseProcess();
		return;
	}
	
	SendCharData();						// ������ �� ������ ������.

	//TRACE( "Initial Point : %d %d\n", m_curx, m_cury);
	
	TempBuf.Add((short)m_curz);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);

	TempBuf.Add(m_tDir);

	TempBuf.Add((BYTE)g_GameTime);
	TempBuf.Add((BYTE)g_GameMinute);

	Send(TempBuf, TempBuf.GetLength());

	SetGameStartInfo();	

	SendSystemMsg( IDS_USER_OPERATOR_MAIL1, SYSTEM_NORMAL, TO_ME);
//	SendSystemMsg( IDS_USER_OPERATOR_MAIL2, SYSTEM_NORMAL, TO_ME);
}

void USER::GameStartWithThread(TCHAR *pBuf)
{
	int			index = 0;
	BYTE		result = FAIL;
	TCHAR		szID[CHAR_NAME_LENGTH+1];
	CBufferEx	TempBuf;

	int	nIDLength = GetVarString(sizeof(szID), szID, pBuf, index);
	if ( nIDLength == 0 || nIDLength > CHAR_NAME_LENGTH )
	{
		ReInitMemoryDB();

		TempBuf.Add(GAME_START_RESULT);
		TempBuf.Add(result);
		TempBuf.Add(ERR_1);
		Send(TempBuf, TempBuf.GetLength());
		SoftClose();
		return;
	}

	LOGINOUTTHREADDATA *pLIOTD;
	pLIOTD = new LOGINOUTTHREADDATA;
	pLIOTD->CODE = GAME_START_REQ;
	pLIOTD->UID = m_uid;
	memset(pLIOTD->ID, NULL, CHAR_NAME_LENGTH+sizeof(int)+1);
	memcpy(pLIOTD->ID, pBuf, index );

	EnterCriticalSection( &m_CS_LoginData );
	RecvLoginData.AddTail(pLIOTD);
	nLoginDataCount = RecvLoginData.GetCount();
	LeaveCriticalSection( &m_CS_LoginData );
}

///////////////////////////////////////////////////////////////////////////
//	Game Start �ÿ� �ʿ��� ���� ����
//
void USER::SetGameStartInfo()
{
	// Game�� Start �ɶ� �ʿ��� ���� ����...
//	SendQuickChange();					// Ȥ�� ���� ���ӿ��� ������ ����� ��ҵȰ��� �ִ��� �˾ƺ���.
	SendUserStatusSkill();

	SendWeatherInMoveZone();			// ������ ������ȭ�� �˾ƺ���. 
	SetWeather(1, 0);					// ������ �ð�

	m_state = STATE_GAMESTARTED;

//	SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	SightRecalc();						// �� �þ߾��� ������ ������ ������.

//	LoadUserBank();
//	LoadAccountBank();
	int rank = m_sCityRank + CITY_RANK_INTERVAL;
	if( rank != SAINT_RANK )		// ���� ���ߵ� ����Ʈ�� �ƴ϶��
	{
		m_dwSaintTime = 0;
	}

	UpdateMemUserAll(TRUE);

	m_dwServerTick = GetTickCount();

	m_dwLastNoDamageTime = GetTickCount();
	m_dwNoDamageTime = NO_DAMAGE_TIME;
	//TRACE("######### Game Start ##################\n");
}

///////////////////////////////////////////////////////////////////////////
//	������ �������� ������ ������ ���� �Ӽ��� �ݿ��Ѵ�.(�������� ����)
//	
void USER::GetMagicItemSetting()
{
	int i, j;

	int nCount = 4;										// ����, �����϶� ���� Ƚ��
	int magic_slot = 0;
	int dynamic_slot = 0;
	BOOL bOldShield = CheckAbnormalInfo(ABNORMAL_SHIELD);
	CBufferEx TempBuf;

	for(i = 0; i < MAGIC_COUNT; i++) m_DynamicUserData[i] = 0;
	for(i = 0; i < EBODY_MAGIC_COUNT; i++) m_DynamicEBodyData[i] = 0;
	for(i = 0; i < 10; i++) m_DynamicMagicItem[i]= 0;
	

	for(i = 0; i < EQUIP_ITEM_NUM; i++)					// ��ü ������ ���Ը� �� �˻��Ѵ�.
	{
		if(m_UserItem[i].sSid >= 0)			
		{												// ���⿡�� ������ �ݿ��� ������ �������� �����Ѵ�.				
			if((m_UserItem[i].tIQ > NORMAL_ITEM && m_UserItem[i].tIQ <= REMODEL_MAGIC_ITEM)||m_UserItem[i].tIQ==12)	// �� ���� �Ӽ��� �پ� ������...
			{
				if(m_UserItem[i].tIQ == UNIQUE_ITEM) nCount = MAGIC_NUM;
				if(m_UserItem[i].tIQ == 12) {
					nCount = 5;
					GetMagicItem_100(m_UserItem[i].tMagic[6]);
					GetMagicItem_100(m_UserItem[i].tMagic[7]);
				}
														// ����, ����� 4�� ���Ը� �������� �����ϴµ� ��
				if(i != LEFT_HAND)
				{
					for(j = 0; j < nCount; j++)			// ���� ����ŭ ������. 
					{				
						magic_slot = m_UserItem[i].tMagic[j];
						if(magic_slot < 0 || magic_slot >= g_arMagicItemTable.GetSize()) continue;

						dynamic_slot = g_arMagicItemTable[magic_slot]->m_sSubType;
						if(dynamic_slot < 0 || dynamic_slot >= MAGIC_COUNT) continue;

						m_DynamicUserData[dynamic_slot] += g_arMagicItemTable[magic_slot]->GetMagicItemValue();
					}
				}
			}

			if(i == BODY || i == PANTS)	// 80 �� �̻��� ���ʿ��� Ư�� �ɷ�ġ�� �ִ�.
			{
				if(m_byClass == 3)
				{
					if(m_UserItem[i].sSid < g_arItemTable.GetSize())
					{
						if(g_arItemTable[m_UserItem[i].sSid]->m_byRLevel == 80 && g_arItemTable[m_UserItem[i].sSid]->m_byClass == 1)
						{
							m_DynamicUserData[MAGIC_MAX_HP_UP] += g_arItemTable[m_UserItem[i].sSid]->m_byMPP;
						}
					}
				}
			}
		}
	}

	nCount = 5;
	for(i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++)	// E-Body Magic �Ӽ� �߰�
	{
		if(m_UserItem[i].sSid >= 0)			
		{												// ���⿡�� ������ �ݿ��� ������ �������� �����Ѵ�.				
			if(m_UserItem[i].tIQ == MAGIC_ITEM )		// �� ���� �Ӽ��� �پ� ������...
			{
				for(j = 0; j < nCount; j++)			// ���� ����ŭ ������. 
				{				
					magic_slot = m_UserItem[i].tMagic[j];
					if(magic_slot < 0 || magic_slot >= g_arEBodyTable.GetSize()) continue;
					
					dynamic_slot = g_arEBodyTable[magic_slot]->m_sSubType;
					if(dynamic_slot < 0 || dynamic_slot >= EBODY_MAGIC_COUNT) continue;
					
					m_DynamicEBodyData[dynamic_slot] += g_arEBodyTable[magic_slot]->m_sChangeValue;
				}
			}
			if(m_UserItem[i].sSid==987){
				//����������
				m_DynamicMagicItem[7]=m_DynamicMagicItem[7] + m_UserItem[i].tMagic[5]*6;
			}else{ //�����˼���
				m_DynamicMagicItem[5]=m_DynamicMagicItem[5] + m_UserItem[i].tMagic[5]*2;
			}
		}
	}
	//ͳ���ػ�����
    GetMagicItemSetting_38();
	GetSkillSetting_130();
	//�ټ��ϵķ�ħ�� Ѫ��ʯ��
m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+m_DynamicMagicItem[0];
m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+m_DynamicMagicItem[1];
m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+m_DynamicMagicItem[2];



	// �Ҹ� �Ǽ��縮 üũ
	m_bRingOfLife		= FALSE;
	m_bNecklaceOfShield	= FALSE;
	m_bEarringOfProtect = FALSE;
	for(i = 0; i < MAX_ACCESSORI; i++)
	{
		if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_RING_OF_LIFE)		m_bRingOfLife = TRUE;
		if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_NECKLACE_OF_SHIELD)	m_bNecklaceOfShield = TRUE;
		if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_EARRING_OF_PROTECT)	m_bEarringOfProtect = TRUE;
	}

	if(m_bNecklaceOfShield == TRUE) 
	{
		AddAbnormalInfo(ABNORMAL_SHIELD);
		if(bOldShield == FALSE)
		{
			TempBuf.Add(SET_USER_STATE);
			TempBuf.Add(m_uid + USER_BAND);
			TempBuf.Add(m_dwAbnormalInfo);
			TempBuf.Add(m_dwAbnormalInfo_);

			SendInsight(TempBuf, TempBuf.GetLength());
		}
	}
	else
	{
		if(bOldShield == TRUE && m_dwShieldTime == 0)
		{
			DeleteAbnormalInfo(ABNORMAL_SHIELD);
			TempBuf.Add(SET_USER_STATE);
			TempBuf.Add(m_uid + USER_BAND);
			TempBuf.Add(m_dwAbnormalInfo);
			TempBuf.Add(m_dwAbnormalInfo_);

			SendInsight(TempBuf, TempBuf.GetLength());
		}
	}

	if(m_dwMaxHPUpTime)	SetUserToMagicUser(1);	// ������ �⺻�ɷ�ġ �������� �����Ѵ�.
	else SetUserToMagicUser();	
	//CheckMagicItemMove();
}
//ͳ���ػ�����.
void USER::GetMagicItemSetting_38()
{
	if(m_UserItem[TOTAL_ITEM_NUM-2].sSid==-1)
		return ;
	if(m_UserItem[TOTAL_ITEM_NUM-2].sDuration<=0)
		return ;
	if(m_UserItem[TOTAL_ITEM_NUM-2].tIQ!=9) //�����ǲ����ػ����� ���Ƿ���
		return ;
	switch (m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]){//�����ػ���������
			case 1://1 ��������2
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+2;
				break;
			case 2:// 2 ��������3
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+3;
				break;
			case 3:// 3 ��������5
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+5;
				break;
			case 4://4  �����ȼ�����1
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+16;
				break;
			case 5://5  �����ȼ�����2
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+32;
				break;
			case 6:// 6 �����ȼ�����3
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+48;
				break;
			case 7:// 7 ���Է�����2ת�����Լ�����
				break;
			case 8://8���Է�����5ת�����Լ�����
				break;
			case 9://9 ���Է�����10ת�����Լ�����
				break;
			case 10:// 10����Χ���2%�˺�
				break;
			case 11://11 ����Χ���5%�˺�
				break;
			case 12://12 ����Χ���10%�˺�
				break;
			case 13://13 ��������3
					m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+3;
				break;
			case 14://14 ��������6
					m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+6;
				break;
			case 15://15 ��������10
					m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+10;
				break;
			case 16://16ħ����������3
				m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+3;
				break;
			case 17:// 17ħ����������6
				m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+6;
				break;
			case 18:// 18ħ����������10
				m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+10;
				break;
			case 19://19�����˶��Լ������˺�2%���䵽��������
				break;
			case 20://20�����˶��Լ������˺�3%���䵽��������
				break;
			case 21://21�����˶��Լ������˺�5%���䵽��������
				break;
			case 22://22 ��־���ֵ���2%
				break;
			case 23:// 23��־���ֵ���3%
				break;
			case 24://24 ��־���ֵ���5%
				break;
			case 25:// 25 ������10%
				break;
			case 26:// 26 ������20%
				break;
			case 27:// 27 ������50%
				break;
			case 28:// 28 ���м�������1
				m_DynamicUserData[MAGIC_ALL_SKILL_UP]=m_DynamicUserData[MAGIC_ALL_SKILL_UP]+1;
				break;
			case 29://29���м�������2
				m_DynamicUserData[MAGIC_ALL_SKILL_UP]=m_DynamicUserData[MAGIC_ALL_SKILL_UP]+2;
				break;
			case 30://30���м�������3
				m_DynamicUserData[MAGIC_ALL_SKILL_UP]=m_DynamicUserData[MAGIC_ALL_SKILL_UP]+3;
				break;
			case 31://31������Ʒ���2%
				break;
			case 32:// 32������Ʒ���5%
				break;
			case 33://33������Ʒ���10%
				break;
			case 34://34����������10
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+10;
				break;
			case 35:// 35����������20
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+20;
				break;
			case 36://36����������30
				m_DynamicUserData[MAGIC_DAMAGE_UP]=m_DynamicUserData[MAGIC_DAMAGE_UP]+30;
				break;
			case 37://37��ȡ�Է�����10
				break;
			case 38:// 38��ȡ�Է�����20
				break;
			case 39://39��ȡ�Է�����30
				break;
			case 40://40��������10
				m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+10;
				break;
			case 41://41��������20
				m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+20;
				break;
			case 42://  42��������50
				m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+50;
				break;
				
	}
	for(int i=0;i<3;i++){
		switch (m_UserItem[TOTAL_ITEM_NUM-2].tMagic[1+i]){//�����ػ�666����
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6://ÿ�μӵ�3��
				m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+m_UserItem[TOTAL_ITEM_NUM-2].tMagic[1+i]*3;
				break;
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12://ÿ����15Ѫ
				m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[1+i]-6)*15;
				break;
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18://ÿ������5��AGIC_DEFENSE_UP
				m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[1+i]-12)*5;
				break;
		 default:
		 	break;
		}

	}
	
	
}
//����130����
void USER::GetSkillSetting_130()
{
	m_Hit=0;
	m_Avoid=0;
	int iCount,sLevel;

	for (int i=3*m_byClass;i<(3*m_byClass+3);i++){
		switch(i){
			case 0: ////����֮�� //��������
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					m_Hit=(sLevel-1)*10+30;
				}
				break;
			case 1:////����֮�� ��������
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*20+150;
					m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+iCount;

				}
				break;
			case 2: 	// �������   ��Ϊ ��һ�������� 30�� 30�� �Ժ�ÿ�� ���� 3�㿹 3���
					sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*6+30;
					m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+iCount;
					m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+iCount;
				}
				break;
			case 3://	//���� ħ��  ��Ч

				break;
			case 4:	//����֮�� ��������
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*12+90;
					m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+iCount;
				}
				break;
			case 5://����֮��
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					m_Avoid=(sLevel-1)*4+24;
				}
				break;
			case 6:	//����֮�� ��������
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*16+120;
					m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+iCount;
				}
				break;
			case 7:	//�������  ��һ�� ���ӷ� 40  ÿ������1��
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*1+40;
					m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+iCount;
				}
				break;
			case 8:		// ħ������  ��һ�� ���ӿ� 42  ÿ������3��
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*3+42;
					m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+iCount;
				}
				break;
			case 9:	//����֮��   ��һ�� ����+105 ��ÿ������14������
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*14+105;
					m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+iCount;
				}
				break;
			case 10:	// ħ������  ��һ�� ���ӿ� 45  ÿ������5��
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					iCount=(sLevel-1)*8+65;
					m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+iCount;
				}
				break;
			case 11://����֮��    ��һ�� ���� 50   ÿ������7��
				sLevel=m_UserSkill_[i].tLevel;
				if(sLevel>0&&sLevel<=20){
					m_Avoid=(sLevel-1)*7+50;
				}
				break;
			default:
				break;

		}

	}

	if(m_iSkin==1) {m_Hit=15;m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+100;}
	if(m_iSkin==2) {m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+300;}


}
int USER::GetMagicItem_100(byte tMagic)
{
	if (tMagic<3||tMagic>52)
		return 0;
	int iDynamic=(tMagic-3)/10;
	int isLevel= (tMagic-3)%10+1;
	int iValue;
	switch(iDynamic){
		case 0://��+5
			iValue=5*isLevel; break;
		case 1://ħ����+5
			iValue=5*isLevel; break;
		case 2://23 ��������10
			iValue=10*isLevel; break;
		case 3://��+4 ������
			iValue=0; break;
		case 4://ħ���˼��� 10
			iValue=10*isLevel; break;
		case 5://�����˼��� 5
			iValue=5*isLevel; break;
		case 6://�����˼��� 10
			iValue=10*isLevel; break;
							 
	}
	m_DynamicMagicItem[iDynamic]=m_DynamicMagicItem[iDynamic]+iValue;
	return iValue;

}

///////////////////////////////////////////////////////////////////////////
//	Log Out
//
void USER::LogOut()
{
	if(m_bLogOut == TRUE) return;
	if(m_state != STATE_GAMESTARTED) return;	// �̿� STATE_CONNECTED�� �϶��� �ش� �ʱ�ȭ�� �޸𸮿������Ƿ� �����ؾ��Ѵ�.

	m_bLogOut = TRUE;
//	USER *pUser = NULL;
												// �Ϲ� �ʵ��� ���̶��..		
	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0)		
	{
		if(m_bGuildMaster) 
		{
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_GUILD_DEFEAT, m_strGuildName);
			SendGuildWarFieldEnd((LPTSTR)(LPCTSTR)strMsg);// �׺�
		}
	}

	if(m_tGuildHouseWar == GUILD_WARRING) CheckGuildUserListInGuildHouseWar(); // �ٸ� �������� ���ϳ� üũ..

	if(m_tQuestWar == GUILD_WARRING) g_QuestEventZone.CheckUserStateInEventZone(m_curz);//CheckQuestEventZoneWarEnd();

	if(m_bNowBuddy == TRUE)				// �������̸� �뺸�Ѵ�.
	{
		for(int i = 0; i < MAX_BUDDY_USER_NUM; i++)
		{
			if(m_MyBuddy[i].uid == m_uid + USER_BAND) SendBuddyUserLeave(i);
		}
	}

	// �ŷ����̸� �ŷ���� ó��
	if(m_bNowTrading == TRUE) 
	{
		BYTE result = 0x00;
		USER *pTradeUser = NULL;
		if(m_iTradeUid != -1)	pTradeUser = GetUser(m_iTradeUid - USER_BAND);

		if(pTradeUser != NULL)	pTradeUser->SendExchangeFail(result, (BYTE)0x05);
	}

	// �ξⷳ�� ó��
	ExitRoyalRumble();

	// ��ȣ�ڵ� �߰�
	int nRet1 = 0, nRet2 = 0, nRet3 = 0;
	nRet1 = UpdateMemBankDataOnly();
	nRet3 = UpdateMemAccountBankDataOnly();
	nRet2 = UpdateUserData(TRUE);

	if(nRet1 == 1 && nRet2 == 1 && nRet3 == 1) ReInitMemoryDB();
	else
	{
		if(nRet1 == 0)
		{
			UpdateMemBankDataOnly();
		}
		if(nRet2 == 0)
		{
			UpdateUserData(TRUE);
		}
		if(nRet3 == 0)
		{
			UpdateMemAccountBankDataOnly();
		}
		ReInitMemoryDB();
	}

//	SessionLogOut();			// CURRENTUSER TABLE���� �α׸� �����.
	// ��ȣ�ڵ� ( x, y, z���� ������ �Ѵ°����� ���Ҽ��ִ� )
	MAP* pMap = NULL;

	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) goto go_result;

	pMap = g_zone[m_ZoneIndex];		if( !pMap ) goto go_result;

	if( m_curx < 0 || m_curx >= pMap->m_sizeMap.cx ) goto go_result;
	if( m_cury < 0 || m_cury >= pMap->m_sizeMap.cy ) goto go_result;

	if( g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser == USER_BAND + m_uid ) 
		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);

go_result:
	SendMyInfo(TO_INSIGHT, INFO_DELETE);

	m_state = STATE_LOGOUT;

	g_pMainDlg->BridgeServerUserLogOut( m_uid, m_strUserID );
}

void USER::ZoneLogOut(int save_z, int save_x, int save_y)
{
	if(m_bLogOut == TRUE) return;
	if(m_state != STATE_GAMESTARTED) return;	// �̿� STATE_CONNECTED�� �϶��� �ش� �ʱ�ȭ�� �޸𸮿������Ƿ� �����ؾ��Ѵ�.

	m_bLogOut = TRUE;
//	USER *pUser = NULL;
												// �Ϲ� �ʵ��� ���̶��..		
	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0)		
	{
		if(m_bGuildMaster) 
		{
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_GUILD_DEFEAT, m_strGuildName);
			SendGuildWarFieldEnd((LPTSTR)(LPCTSTR)strMsg);// �׺�
		}
	}

	if(m_tGuildHouseWar == GUILD_WARRING) CheckGuildUserListInGuildHouseWar(); // �ٸ� �������� ���ϳ� üũ..

	if(m_bNowBuddy == TRUE)				// �������̸� �뺸�Ѵ�.
	{
		for(int i = 0; i < MAX_BUDDY_USER_NUM; i++)
		{
			if(m_MyBuddy[i].uid == m_uid + USER_BAND) SendBuddyUserLeave(i);
		}
	}

	// �ŷ����̸� �ŷ���� ó��
	if(m_bNowTrading == TRUE) 
	{
		BYTE result = 0x00;
		USER *pTradeUser = NULL;
		if(m_iTradeUid != -1)	pTradeUser = GetUser(m_iTradeUid - USER_BAND);

		if(pTradeUser != NULL)	pTradeUser->SendExchangeFail(result, (BYTE)0x05);
	}

	// �ξⷳ�� ó��
	ExitRoyalRumble();

	int backup_x, backup_y, backup_z;

	backup_z = m_curz;
	backup_x = m_curx;
	backup_y = m_cury;

	m_curz = save_z;
	m_curx = save_x;
	m_cury = save_y;

	// ��ȣ�ڵ� �߰�
	int nRet1 = 0, nRet2 = 0, nRet3 = 0;
	nRet1 = UpdateMemBankDataOnly();
	nRet3 = UpdateMemAccountBankDataOnly();
	nRet2 = UpdateUserData(TRUE);

	if(nRet1 == 1 && nRet2 == 1 && nRet3 == 1) ReInitMemoryDB();
	else
	{
		if(nRet1 == 0)
		{
			UpdateMemBankDataOnly();
		}
		if(nRet2 == 0)
		{
			UpdateUserData(TRUE);
		}
		if(nRet3 == 0)
		{
			UpdateMemAccountBankDataOnly();
		}
		ReInitMemoryDB();
	}

	m_curz = backup_z;
	m_curx = backup_x;
	m_cury = backup_y;

//	SessionLogOut();			// CURRENTUSER TABLE���� �α׸� �����.
	// ��ȣ�ڵ� ( x, y, z���� ������ �Ѵ°����� ���Ҽ��ִ� )
	MAP* pMap = NULL;

	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) goto go_result;

	pMap = g_zone[m_ZoneIndex];		if( !pMap ) goto go_result;

	if( m_curx < 0 || m_curx >= pMap->m_sizeMap.cx ) goto go_result;
	if( m_cury < 0 || m_cury >= pMap->m_sizeMap.cy ) goto go_result;

	if( g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser == USER_BAND + m_uid ) 
		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);

go_result:
	SendMyInfo(TO_INSIGHT, INFO_DELETE);

	m_state = STATE_LOGOUT;
}

///////////////////////////////////////////////////////////////////////////
//	Session�� Log�� ����� ���ؼ�  by zi_gi 2002-03-27
//
void USER::SessionLogOut()
{
	return;
/*	
	if(m_bSessionLogOut) return;
		
	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UM_LOGOUT ( \'%s\', %d )}"), m_strAccount, 1 );
//	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GAMESERVER_LOGOUT_DROIYAN ( \'%s\', %d )}"), m_strAccount, 1 );
	
	hstmt = NULL;

	int db_index = -1;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) return;
	
	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
		return;
	}

	retcode = SQLExecDirect (hstmt, (unsigned char *)szSQL, SQL_NTS);
	if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
	}

	else if (retcode==SQL_ERROR)
	{
//		DisplayErrorMsg(hstmt);
	}
	else if (retcode==SQL_NO_DATA)
	{
	}
	
	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

	g_DBSession[m_iModSid].ReleaseDB(db_index);

	m_bSessionLogOut = TRUE;

	TRACE("CURRENUSER TABLE Success. \n");
*/
	return;
}

///////////////////////////////////////////////////////////////////////////
//	Ŭ������ ���������� DB�� ������Ʈ �Ѵ�.
//
BOOL USER::UpdateUserData(BOOL bLogOut)
{
	if(!bLogOut) if(UpdateMemUserAll()) return TRUE;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strFace[10], strSkill[_SKILL_DB], strItem[_ITEM_DB], strPsi[_PSI_DB], strTel[_TEL_DB];
	TCHAR			strQuickItem[_QUICKITEM_DB];
	TCHAR			strHaveEvent[_EVENT_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));

	::ZeroMemory(strFace, sizeof(strFace));
	::ZeroMemory(strSkill, sizeof(strSkill));
	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strPsi, sizeof(strPsi));	
	::ZeroMemory(strTel, sizeof(strTel));
	::ZeroMemory(strHaveEvent, sizeof(strHaveEvent));
	
	::ZeroMemory(strQuickItem, sizeof(strQuickItem));

	::CopyMemory(strFace, m_strFace, sizeof(m_strFace));

	UserSkillToStr(strSkill);
	UserItemToStr(strItem);
	UserPsiToStr(strPsi);
	UserTelToStr(strTel);
	UserHaveEventDataToStr(strHaveEvent);

	DWORD dwCurTime = ConvertCurTimeToSaveTime();			// ���� �ð� ������ ����

	SDWORD sFaceLen		= sizeof(strFace);
	SDWORD sSkillLen	= sizeof(strSkill);
	SDWORD sItemLen		= sizeof(strItem);
	SDWORD sPsiLen		= sizeof(strPsi);
	SDWORD sQuickLen	= sizeof(strQuickItem);
	SDWORD sEventLen	= sizeof(strHaveEvent);
	SDWORD sTelLen		= sizeof(strTel);	

	m_tPsiOneKind = m_tPsiTwoKind = m_tPsiThreeKind = 0;
	m_dwPsiOneTime = m_dwPsiTwoTime = m_dwPsiThreeTime = 0;

	// Psionic One
	if(m_dwHasteTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_HASTE;
		m_dwPsiOneTime = m_dwHasteTime;
	}
	if(m_dwShieldTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_SHIELD;
		m_dwPsiOneTime = m_dwShieldTime;
	}
	if(m_dwDexUpTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_DEXUP;
		m_dwPsiOneTime = m_dwDexUpTime;
	}
	if(m_dwMaxHPUpTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_HPUP;
		m_dwPsiOneTime = m_dwMaxHPUpTime;
	}
	if(m_dwFastRunTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_FAST_RUN;
		m_dwPsiOneTime = m_dwFastRunTime;
	}
	if(m_dwMindShockTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_MIND_SHOCK;
		m_dwPsiOneTime = m_dwMindShockTime;
	}
	if(m_dwPsiShieldTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_PSI_SHIELD;
		m_dwPsiOneTime = m_dwPsiShieldTime;
	}
	if(m_dwBigShieldTime != 0)
	{
		m_tPsiOneKind = 30;
		m_dwPsiOneTime = m_dwBigShieldTime;
	}
	if(m_dwPiercingShieldTime != 0) 
	{
		m_tPsiOneKind = PSIONIC_PIERCING_SHIELD;
		m_dwPsiOneTime = m_dwPiercingShieldTime;
	}

	// Psionic Two
	if(m_dwAdamantineTime != 0) 
	{
		m_tPsiTwoKind = PSIONIC_ADAMANTINE;
		m_dwPsiTwoTime = m_dwAdamantineTime;
	}
	if(m_dwMightyWeaponTime != 0) 
	{
		m_tPsiTwoKind = PSIONIC_MIGHTYWEAPON;
		m_dwPsiTwoTime = m_dwMightyWeaponTime;
	}
	if(m_dwBerserkerTime != 0) 
	{
		m_tPsiTwoKind = PSIONIC_BERSERKER;
		m_dwPsiTwoTime = m_dwBerserkerTime;
	}

	// Psionic Three
	if(m_dwMindGuardTime != 0) 
	{
		m_tPsiThreeKind = PSIONIC_MIND_GUARD;
		m_dwPsiThreeTime = m_dwMindGuardTime;
	}

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_USER_DATA (\'%s\',%d,%d,%d,%d,%d,%d,%d,%d,\
		?, %d,%d,%d, %d, %d,%d,  %d,%d,  %d, \
		%d,%d,%d,%d,%d,%d, %d,%d,%d,%d, \
		?,?,?,?, %d,%d,\
		%d, %d, ?, %d, %d,\
		?, %d,\
		%d, %d, %d,\
		\'%s\', \
		%d,%d,		%d,%d,		%d,%d)}"), 
		m_strUserID,	m_sSTR,	m_sCON,	m_sDEX,	m_sVOL,	m_sWIS,	m_iSkin, m_iHair, m_sGender,	
		m_curz,	m_curx,	m_cury,		m_dwBuddy,		m_dwGuild, m_dwExp,		m_sPA, m_sSkillPoint,	m_dwXP,
		m_sMaxHP, m_sHP, m_sMaxPP, m_sPP, m_sMaxSP,	m_sSP,		m_dwDN,	m_sCityRank, m_sLevel,	m_byClass,

		m_tAbnormalKind, m_dwAbnormalTime,

		m_bLive, m_iCityValue, m_sKillCount, dwCurTime, 
		m_dwSaintTime, 
		m_dwHiExpTime, m_dwMagicFindTime, m_dwNoChatTime,
		m_strLoveName, //--yskang 0.1 
		m_tPsiOneKind, m_dwPsiOneTime,		m_tPsiTwoKind, m_dwPsiTwoTime,		m_tPsiThreeKind, m_dwPsiThreeTime); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if(retcode != SQL_SUCCESS)
		return FALSE;

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strFace),	0, (TCHAR*)strFace,		0, &sFaceLen );

		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strSkill),	0, (TCHAR*)strSkill,	0, &sSkillLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strPsi),		0, (TCHAR*)strPsi,		0, &sPsiLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strQuickItem),	0, (TCHAR*)strQuickItem,	0, &sQuickLen );

		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strHaveEvent),	0, (TCHAR*)strHaveEvent,	0, &sEventLen );

		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strTel),			0, (TCHAR*)strTel,			0, &sTelLen );

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			bQuerySuccess = FALSE;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//	���������� �������̵��� ���� ���������� DB�� ������Ʈ �Ѵ�.
//
BOOL USER::UpdateUserExchangeData(USER *pUser)
{ 
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return FALSE;
	if(UpdateMemUserAll() && pUser->UpdateMemUserAll()) return TRUE;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strItem[_ITEM_DB], strUserItem[_ITEM_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strUserItem, sizeof(strUserItem));

	UserItemToStr(strItem);
	pUser->UserItemToStr(strUserItem);

	SDWORD sItemLen			= sizeof(strItem);
	SDWORD sUserItemLen		= sizeof(strUserItem);

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_USER_EXCHANGE_DATA (\'%s\',%d,?,\'%s\',%d,?)}"), 
		m_strUserID, m_dwDN, pUser->m_strUserID, pUser->m_dwDN); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if(retcode != SQL_SUCCESS)
		return FALSE;

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strUserItem),	0, (TCHAR*)strUserItem,		0, &sUserItemLen );

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			bQuerySuccess = FALSE;
		}
	}
	
	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//	���õ� ĳ������ ������ �����´�.
//
BOOL USER::LoadUserData(TCHAR *szID)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];
	DWORD			dwMaxDN = 0;
	BYTE			tPoint = 0;
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_USER_DATA (\'%s\')}"), szID);
	
	SQLSMALLINT sSTR, sCON, sDEX, sVOL, sWIS;
	SQLSMALLINT sBasicSTR, sBasicCON, sBasicVOL, sBasicDEX, sBasicWIS;
	SQLUINTEGER	iSkin, iHair, iExp, iXP, iDN;
	SQLINTEGER iCityValue, iGuild;
	SQLSMALLINT	sGender;
	SQLCHAR		strFace[10];
	SQLSMALLINT	sLZ, sLX, sLY, sPA;
	SQLUINTEGER iBuddy;
	SQLSMALLINT	sMaxHP, sHP, sMaxPP, sPP, sMaxSP, sSP, sCityRank, sKillCount;
	SQLSMALLINT sLevel;
	SQLCHAR		byClass, bLive;
	
	SQLSMALLINT	sSkillPoint;

	SQLCHAR		strSkill[_SKILL_DB], strItem[_ITEM_DB], strPsi[_PSI_DB], strHaveEvent[_EVENT_DB], strTel[_TEL_DB];
	
	SQLCHAR		strQuickItem[_QUICKITEM_DB];	// �� �������� ���̻� �������� �ʴ´�.

	SQLCHAR		tAbnormalKind;
	SQLUINTEGER iAbnormalTime;

	//--yskang 0.1
	SQLCHAR		strDbLoveName[LOVE_NAME_LENGTH];

	SQLSMALLINT	sChangeClass[CLASS_NUM];

	SQLCHAR		tIsOP;

	SQLUINTEGER iSaintTime;		// ����Ʈ ������ ī��Ʈ ���̱� ����
	SQLINTEGER	sInd;

	SQLUINTEGER iHiExpTime, iMagicFindTime, iNoChatTime;			// ����ġ2��, ����Ȯ��5��, ä�ݽð�

	SQLCHAR		tPsiOneKind, tPsiTwoKind, tPsiThreeKind;			// Haste, Shield, Dex Up, Max HP Up, Fast Run, Mind Shock, Psi Shield, Piercing Shield
	SQLUINTEGER	iPsiOneTime, iPsiTwoTime, iPsiThreeTime;			// Adamantine, MightyWeapon, Berserker
																	// Mind Guard

	sSTR = sCON = sDEX = sVOL = sWIS = 0;
	sBasicSTR = sBasicCON = sBasicVOL = sBasicWIS = sBasicDEX = 0;
	sLZ = sLX = sLY = 1;
	sPA = 0;
	iBuddy = iGuild = 0;
	iExp = iXP = iDN = iCityValue = sKillCount = 0;
	sMaxHP = sHP = sMaxPP = sPP = sMaxSP = sSP = 0;
	sCityRank = 0;
	sLevel = 1;
	byClass = tIsOP = bLive = 0;

	tAbnormalKind = 0;
	iAbnormalTime = 0;

	iSaintTime = 0;

	iHiExpTime = iMagicFindTime = iNoChatTime = 0;

	tPsiOneKind = tPsiTwoKind = tPsiThreeKind = 0;
	iPsiOneTime = iPsiTwoTime = iPsiThreeTime = 0;

	sSkillPoint = 0;

	::ZeroMemory(strFace, sizeof(strFace));
	::ZeroMemory(strSkill, sizeof(strSkill));
//	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strPsi, sizeof(strPsi));
	::ZeroMemory(strTel, sizeof(strTel));
	::ZeroMemory(strHaveEvent, sizeof(strHaveEvent));
	
	::ZeroMemory(strDbLoveName,sizeof(strDbLoveName));

	memset(strItem, -1, sizeof(strItem));
	memset(strQuickItem, -1, sizeof(strQuickItem));		// ���� ���������� ������� �ʴ´�. ���߿� �ٸ��뵵�� ����Ϸ��� ��񿡼� ������ ����

	for(i = 0; i < TOTAL_PSI_NUM; i++)					// �ʱ�ȭ�� �� �ؾ� �Ѵ�.
	{
		m_UserPsi[i].sSid = -1;
		m_UserPsi[i].tOnOff = FALSE;
	}

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Load User Data !!\n");
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS);
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		retcode = SQLFetch( hstmt );

		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
		{
			int i = 1;
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sSTR, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sCON, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sDEX, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sVOL, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sWIS, sizeof(SQLSMALLINT), &sInd );

			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iSkin,		sizeof(iSkin),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iHair,		sizeof(iHair),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sGender,	sizeof(sGender),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_BINARY,	strFace,	sizeof(strFace),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sLZ,		sizeof(sLZ),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sLX,		sizeof(sLX),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sLY,		sizeof(sLY),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iBuddy,	sizeof(iBuddy),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SLONG,	&iGuild,	sizeof(iGuild),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iExp,		sizeof(iExp),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sPA,		sizeof(sPA),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sSkillPoint,	sizeof(sSkillPoint),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iXP,		sizeof(iXP),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sMaxHP,	sizeof(sMaxHP),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sHP,		sizeof(sHP),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sMaxPP,	sizeof(sMaxPP),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sPP,		sizeof(sPP),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sMaxSP,	sizeof(sMaxSP),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sSP,		sizeof(sSP),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iDN,		sizeof(iDN),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sCityRank,	sizeof(sCityRank),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sLevel,	sizeof(sLevel),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_TINYINT,	&byClass,	sizeof(byClass),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_BINARY,	strSkill,	sizeof(strSkill),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_BINARY,	strPsi,		sizeof(strPsi),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_BINARY,	strItem,	sizeof(strItem),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sChangeClass[0],	sizeof(sChangeClass[0]),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sChangeClass[1],	sizeof(sChangeClass[1]),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sChangeClass[2],	sizeof(sChangeClass[2]),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT,	&sChangeClass[3],	sizeof(sChangeClass[3]),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_TINYINT,	&tAbnormalKind,	sizeof(tAbnormalKind),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iAbnormalTime,	sizeof(iAbnormalTime),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_BINARY,	strQuickItem,	sizeof(strQuickItem),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_TINYINT,	&tIsOP,			sizeof(tIsOP),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_TINYINT,	&bLive,		sizeof(bLive),		&sInd );

			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sBasicSTR, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sBasicCON, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sBasicDEX, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sBasicVOL, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sBasicWIS, sizeof(SQLSMALLINT), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SLONG,  &iCityValue,sizeof(iCityValue),&sInd );

			SQLGetData( hstmt, i++, SQL_C_BINARY, strHaveEvent,	sizeof(strHaveEvent), &sInd );
			SQLGetData( hstmt, i++, SQL_C_SSHORT, &sKillCount,	sizeof(sKillCount),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_BINARY, strTel,		sizeof(strTel), &sInd );
			
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iSaintTime,sizeof(iSaintTime),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iHiExpTime,		sizeof(iHiExpTime),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iMagicFindTime,	sizeof(iMagicFindTime),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iNoChatTime,		sizeof(iNoChatTime),	&sInd );

			SQLGetData( hstmt, i++, SQL_C_CHAR,	strDbLoveName,sizeof(strDbLoveName),	&sInd );	//-- yskang 0.1

			SQLGetData( hstmt, i++, SQL_C_TINYINT,	&tPsiOneKind,	sizeof(tPsiOneKind),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iPsiOneTime,	sizeof(iPsiOneTime),	&sInd );			

			SQLGetData( hstmt, i++, SQL_C_TINYINT,	&tPsiTwoKind,	sizeof(tPsiTwoKind),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iPsiTwoTime,	sizeof(iPsiTwoTime),	&sInd );			

			SQLGetData( hstmt, i++, SQL_C_TINYINT,	&tPsiThreeKind,	sizeof(tPsiThreeKind),	&sInd );
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iPsiThreeTime,	sizeof(iPsiThreeTime),	&sInd );	
	
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		//	BREAKPOINT();

			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	// Copy User Data
	strcpy(m_strUserID, szID);	// User ID

	ZeroMemory(m_strLoveName,sizeof(m_strLoveName));
	
	if(strlen((char *)strDbLoveName)<1)//--yskang 0.1
	{
		m_bPseudoString=TRUE;
		strcpy(m_strLoveName,"");
	}
	else
	{
		//-----------------------------------------------------------
		//--yskang 0.6 ���� ����ڿ��Ը� ȣĪ �ο�
		if(m_iDisplayType !=5 && m_iDisplayType !=6)
			strcpy(m_strLoveName,(char *)strDbLoveName);//--yskang 0.1
		else
			strcpy(m_strLoveName,"");
		//------------------------------------------------------------
	}

	m_sSTR = sSTR;				// ��
	m_sCON = sCON;				// �ǰ�
	m_sDEX = sDEX;				// ��ø
	m_sVOL = sVOL;				// ����
	m_sWIS = sWIS;				// ����

	m_sTempSTR = (BYTE)sBasicSTR;		// �ʱ� �Ѱ��� �޾ƿ´�.(�����ٿ��϶� ��)
	m_sTempCON = (BYTE)sBasicCON;
	m_sTempDEX = (BYTE)sBasicDEX;
	m_sTempVOL = (BYTE)sBasicVOL;
	m_sTempWIS = (BYTE)sBasicWIS;

	m_iSkin = iSkin;			// �Ǻλ�
	m_iHair = iHair;			// �Ӹ���

	m_curz = sLZ;				// Zone
	m_curx = sLX;				// ��ġ x
	m_cury = sLY;				// ��ġ y

	m_sGender = sGender;		// ����

	::CopyMemory(m_strFace, strFace, 10);	// �Ӹ����

	m_dwBuddy = iBuddy;			// Buddy
	m_dwGuild = iGuild;			// Guild
	m_tFortressWar = GUILD_WAR_AFFTER;
	m_tGuildWar = GUILD_WAR_AFFTER;
	m_tGuildHouseWar = GUILD_WAR_AFFTER;
	m_tQuestWar = GUILD_WAR_AFFTER;

	if( m_dwGuild >= 1 )
	{
		if( !GetAbsentGuildInfo( (int)m_dwGuild ) )
		{
			m_dwGuild = -1;
		}
	}

	if(m_dwGuild >= 1)			// ��忡 ���� �����͸� �ʱ�ȭ�Ѵ�.
	{
		SetGuildVariable();
	}

	CheckGuildWar();			// ���� ������ ���̶�� ��� �������� �˸���.(������ ��������)

	m_bFieldWarApply = FALSE;
	m_FieldWarGMUid = 0;		// �ʵ����϶� ���� �渶 uid
	m_dwFieldWar = 0;			// �ʵ��� �����̸� ����� ��� ��ȣ�� ���´�.

	m_dwExp	  = iExp;			// Experience

	m_sPA = sPA;				// PA Point

	m_sSkillPoint = sSkillPoint;// Skill Point

	m_dwXP = iXP;				// X Point
	
	m_sLevel	= sLevel;		// ����

	if(iExp > GetNextLevel(sLevel)) iExp = GetNextLevel(sLevel);	// ���� �ڵ�.
	m_byClass	= byClass;		// Ŭ����

	m_sMaxHP	= g_sHPConst[m_byClass] * m_sCON + g_sHPLV[m_byClass] * (m_sLevel - 1) + g_sHPAdd[m_byClass];
	m_sMaxPP	= g_sPPConst[m_byClass] * m_sWIS + g_sPPLV[m_byClass] * (m_sLevel - 1) + g_sPPAdd[m_byClass];
	m_sMaxSP	= g_sSPConst[m_byClass] * m_sCON + g_sSPLV[m_byClass] * (m_sLevel - 1) + g_sSPAdd[m_byClass];

	m_bLive		= bLive;		// �׾�����, ��Ҵ���?

	m_sHP		= sHP;			// ���� HP
	m_sPP		= sPP;			// ���� PP
	m_sSP		= sSP;			// ���� SP

	if(m_sHP <= 0 || m_bLive == USER_DEAD) 
	{
		m_bLive = USER_DEAD;
		SetZoneIndex(m_curz);
		IsDeadUser();		// ���� ������ ������...

		m_bLive = USER_LIVE;
		m_sHP = m_sMaxHP;
	}
	else SetZoneIndex(m_curz);

	if(iDN < 0) iDN = 0;

	m_dwDN		= iDN;			// ������

	m_iCityValue = iCityValue;	// �ù� ������
	m_sKillCount = sKillCount;	// PK���� Ƚ��

	for(i = 0; i < 11; i++)
	{
		if(m_iCityValue < g_CityRankValue[i])// ����ġ���� ���� ���������� ������ �Ǵ��Ѵ�.
		{
			if(i < 6) { m_sCityRank = i - CITY_RANK_INTERVAL; }
			else { m_sCityRank = i -1 - CITY_RANK_INTERVAL; }
			break;
		}
	}

	if(m_iCityValue >= g_CityRankValue[10]) m_sCityRank = CITY_RANK_INTERVAL;

	::CopyMemory(m_strSkill, strSkill, sizeof(m_strSkill));		// ��ų
	::CopyMemory(m_strItem, strItem, sizeof(m_strItem));		// ������
	::CopyMemory(m_strPsi, strPsi, sizeof(m_strPsi));			// ���̿���
//SaveDump((char*)strItem,sizeof(strItem));
	StrToUserSkill((LPTSTR)strSkill);
	StrToUserItem((LPTSTR)strItem);
	StrToUserPsi((LPTSTR)strPsi);
	StrToHaveEventData((LPTSTR)strHaveEvent);
	StrToUserTel((LPSTR)strTel);

	m_dwExpNext	= GetNextLevel(m_sLevel);						// ���������� �Ǳ����� ����ġ

	for(i = 0; i < TOTAL_SKILL_NUM; i++)						// ��ų ������ 
	{
		GetSkillInfo(i, m_UserSkillInfo[i]);
	}
//	m_UserChangeSkillInfo;										// ������ ������� ?(5)��ų�� ����

	for(i = 0; i < CLASS_NUM; i++)
	{
		m_sChangeClass[i] = sChangeClass[i];
	}

	m_tAbnormalKind = tAbnormalKind;							// �����̻� ����
	m_dwAbnormalTime = iAbnormalTime;
	m_dwLastAbnormalTime = GetTickCount();

	// Quick Item ������ ���� ���� �ʴ´�.
//	int index = 0;		
//	for(i = TOTAL_ITEM_NUM; i < TOTAL_ITEM_SLOT_NUM; i++)
//	{
//		m_UserItem[i].sSid = GetShort((char*)strQuickItem, index);
//	}

//	dwMaxDN = m_sLevel * MAX_LEVEL_DN;									// �������� �̻����� üũ
//	if( m_dwDN > dwMaxDN ) m_dwDN = dwMaxDN; 
	m_tDir = myrand(0, 7);												// ���� �����ִ� ������ �������� �ʱ�ȭ�Ѵ�.
		
	m_tIsOP = tIsOP;
	if(m_tIsOP == 255)//yskang debug ��� ���� �� �������� 255�� ���� ���� ������ ���� �ڵ� ����
		m_tIsOP = 0;
	if(tIsOP == 1) AddAbnormalInfo(OPERATION_MODE);						// �⺻�� ��������...

	CheckInvalidGuildZone();

	DWORD dwCurrTime = GetTickCount();
	m_dwLastSpeedTime = dwCurrTime;
	m_dwLastMoveAndRun = dwCurrTime;
	m_dwLastAttackTime = dwCurrTime;
	m_dwLastPsiAttack = dwCurrTime;
	m_dwLastPsiDamagedTime = 0;
	m_dwCastDelay = 0;
	m_dwLastTimeCount = dwCurrTime;
	m_dwLastAbnormalTime = dwCurrTime;
	m_dwLastHPTime = dwCurrTime;

	m_dwLastHasteTime = dwCurrTime;
	m_dwLastShieldTime = dwCurrTime;
	m_dwLastDexUpTime = dwCurrTime;
	m_dwLastMaxHPUpTime = dwCurrTime;

	m_dwNoDamageTime = 0;
	m_dwLastNoDamageTime = dwCurrTime;

	m_dwSaintTime = iSaintTime;		// ����Ʈ ������ ī��Ʈ �ٿ��ֱ�

	m_dwLastHiExpTime		= dwCurrTime;	
	m_dwLastMagicFindTime	= dwCurrTime;
	m_dwLastNoChatTime		= dwCurrTime;

	m_dwHiExpTime		= iHiExpTime;		// ����ġ 2��
	m_dwMagicFindTime	= iMagicFindTime;
	m_dwNoChatTime		= iNoChatTime;
	
	m_dwLastAdamantineTime		= dwCurrTime;	
	m_dwLastMightyWeaponTime	= dwCurrTime;
	m_dwLastBerserkerTime		= dwCurrTime;

	m_dwLastFastRunTime			= dwCurrTime;
	m_dwLastMindShockTime		= dwCurrTime;
	m_dwLastPsiShieldTime		= dwCurrTime;
	m_dwLastPiercingShieldTime	= dwCurrTime;

	m_dwLastMindGuardTime		= dwCurrTime;

	SetPsiOne(tPsiOneKind, iPsiOneTime);
	SetPsiTwo(tPsiTwoKind, iPsiTwoTime);
	SetPsiThree(tPsiThreeKind, iPsiThreeTime);
	
	SetPsiAbnormalStatus();

	GetRecoverySpeed();													// ���� Ŭ������ ȸ���ӵ��� �����Ѵ�.

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
//	�ش� ������ ������ �̺�Ʈ�� �����Ѵ�.
//
void USER::StrToHaveEventData(TCHAR *pBuf)
{
	int index = 0;
	int eventnum;

	InitEventSlot();

	if( !pBuf[0] ) return;

	short HaveEventNum = GetShort( pBuf, index );

	if(HaveEventNum > MAX_EVENT_NUM) HaveEventNum = MAX_EVENT_NUM;

	int* pEventNum;

	for(int i = 0; i < HaveEventNum; i++)
	{
		eventnum = GetShort( pBuf, index );

		pEventNum = new int;

		*(pEventNum) = eventnum;

		m_arEventNum.Add( pEventNum );
	}
}

///////////////////////////////////////////////////////////////////////
//	�ش� ������ ������ �̺�Ʈ�� DB�� �����Ѵ�.
//
void USER::UserHaveEventDataToStr(TCHAR *pBuf)
{
	int index = 0;
	int num = m_arEventNum.GetSize();

	if(num >= MAX_EVENT_NUM) num = MAX_EVENT_NUM;

	SetShort( pBuf, num, index );

	int temp_int;

	for( int i = 0; i < num; i++)
	{
		temp_int = *(m_arEventNum[i]);

		SetShort( pBuf, temp_int, index );
	}
}

///////////////////////////////////////////////////////////////////////
//	�ش� ������ ��尡 �ִٸ� ��忡 �ʿ��� ������ �ʱ�ȭ�Ѵ�.
//
void USER::SetGuildVariable()
{	
	int nLen = 0;
	int iUserIndex = -1;
	CGuild *pGuild = NULL;
//	CGuildUser *pGuildUser = NULL;

	pGuild = GetGuild(m_dwGuild);

	m_tGuildWar = GUILD_WAR_AFFTER;
	m_tFortressWar = GUILD_WAR_AFFTER;

	if(!pGuild)	m_dwGuild = -1;									// �����̶��
	else
	{
		iUserIndex = pGuild->GetUser(m_strUserID);

		if(iUserIndex < 0) m_dwGuild = -1;
		else													// ���� ��忡 ���ϸ�
		{
			m_bRepresentationGuild = FALSE;						// ���� NO! 

			if(strcmp(pGuild->m_strMasterName, m_strUserID) == 0) m_bGuildMaster = TRUE; //�渶���� üũ
			else if(pGuild->m_lSubMaster == 1)
			{
				if(strcmp(pGuild->m_strSubMasterName, m_strUserID) == 0) m_bRepresentationGuild = TRUE; //�渶���� üũ
			}
			
			if(strcmp(g_arGuildData[m_dwGuild]->m_strUsedUser, m_strUserID) == 0)		// �ش� ���â�� �̿��ϴ��� üũ
			{
				g_arGuildData[m_dwGuild]->m_iUsedUser = -1;
				::ZeroMemory(g_arGuildData[m_dwGuild]->m_strUsedUser, sizeof(g_arGuildData[m_dwGuild]->m_strUsedUser));
				InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0); 
			}
			
			m_sGuildVersion = pGuild->m_sVersion;				// ������ 
			strcpy(m_strGuildName, pGuild->m_strGuildName);		// ����̸�
		}
	}

	ReleaseGuild();			// ����...	
}

///////////////////////////////////////////////////////////////////////
//	ó�� ���ӿ� �����Ҷ� �ش� ��尡 ������ �ϸ� �ڵ����� �����ϵ����Ѵ�.
//
//  ������ �������� �ϵ���... 
void USER::CheckGuildWar()
{
	int i, j;
	short nCount = 1;

	BOOL bWar = FALSE;

	CGuildFortress *pFort = NULL;

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		pFort = g_arGuildFortress[i];

		if(!pFort) continue;

		bWar = FALSE;
		nCount = 1;
		CBufferEx TempBuf, TempBufData;

		if(pFort->m_lUsed == 1)									// �ش� ������ ���� ���̸�...
		{
			if(m_dwGuild >= 1)
			{
				if(pFort->m_iGuildSid == m_dwGuild) 
				{
					bWar = TRUE;
					BeginFortressWar();	
				}
				else
				{
					for(j =0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)
					{
						if(pFort->m_arAttackGuild[j].lGuild <= 0) continue;

						if(pFort->m_arAttackGuild[j].lGuild == m_dwGuild)
						{
							bWar = TRUE;
							BeginFortressWar();			// ��� ���� �����Ѵ�.
							break;
						}
					}
				}

				TempBufData.Add(pFort->m_iGuildSid);			// ��� ���			

				for(j =0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)		// ���� ���...
				{
					if(pFort->m_arAttackGuild[j].lGuild > 0)
					{
						TempBufData.Add(pFort->m_arAttackGuild[j].lGuild);
						nCount++;
					}
				}
			}

			TempBuf.Add(GUILD_WAR);
			TempBuf.Add((BYTE)0x01);						// ����� ����...
			TempBuf.Add((BYTE)GUILD_FOTRESS_WARRING);		// ������...
			TempBuf.Add((short)pFort->m_sFortressID);

			if(bWar)
			{
				TempBuf.Add((BYTE)0x01);
				TempBuf.Add(nCount);							// �� ����
				TempBuf.AddData(TempBufData, TempBufData.GetLength());
			}
			else TempBuf.Add((BYTE)0x00);

			Send(TempBuf, TempBuf.GetLength());
		}
	}
}

///////////////////////////////////////////////////////////////////////
//	���� Ŭ������ �˸´� ȸ�� �ӵ��� ���Ѵ�.
//
void USER::GetRecoverySpeed()
{
	int weight = 0;

	m_iHPIntervalTime = 0;		// HP ȸ�� �ð� ����
	m_iSPIntervalTime = 0;		// SP ȸ�� �ð� ����
	m_iPPIntervalTime = 0;		// PP ȸ�� �ð� ����

	m_iHPRate = 1;
	m_iPPRate = 1;
	m_iSPRate = 1;

	m_iHPIntervalTime = 15000;	// Init default	
	m_iSPIntervalTime = 15000;
	m_iPPIntervalTime = 15000;

	if(CheckRecoverTableByClass())
	{
		m_iHPRate = g_arRecoverTable[m_byClass]->m_byHPDivide;
		m_iPPRate = g_arRecoverTable[m_byClass]->m_byPPDivide;
		m_iSPRate = g_arRecoverTable[m_byClass]->m_bySPDivide;

		m_iHPIntervalTime = (int)(g_arRecoverRateTable[m_byClass]->m_sHpSpeedupTime - (double)(g_arRecoverRateTable[m_byClass]->m_byHpSpeedupLevel/30));
		m_iSPIntervalTime = (int)(g_arRecoverRateTable[m_byClass]->m_sSpSpeedupTime - (double)(g_arRecoverRateTable[m_byClass]->m_bySpSpeedupLevel/30));
		m_iPPIntervalTime = (int)(g_arRecoverRateTable[m_byClass]->m_sPpSpeedupTime - (double)(g_arRecoverRateTable[m_byClass]->m_byPpSpeedupLevel/30));
	}
	
	weight = (int)((double)m_iMaxWeight * 0.7 + 0.5);		// ���� �������� ���� ȸ�� �ӵ��� ��...
	if(m_iCurWeight >= weight) 
	{
		m_iHPIntervalTime = m_iHPIntervalTime * 2;
		m_iSPIntervalTime = m_iSPIntervalTime * 2;
		m_iPPIntervalTime = m_iPPIntervalTime * 2;
	}
}

///////////////////////////////////////////////////////////////////////
//	���� ������ Zone �� ������ ����ϰ� �ִ� ������ �˻��Ѵ�.
//
BOOL USER::IsZoneInThisServer(int zone)
{
	int temp_zone = -1;

	for(int i = 0; i < g_zone.GetSize(); i++)
	{
		if( g_zone[i]->m_Zone == zone )	return TRUE;
		//TRACE("��ͼ���� %d \n",g_zone[i]->m_Zone);
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//	�Ű������� Zone �� ��� �������� �����ϴ����� ã�Ƽ� 
//	������ ZONEINFO Table �� ������Ʈ �Ѵ�.
//
//	��ã�� ���� ���߿� �ڵ�
void USER::ChangeServer(int zone)
{
	int server_index = -1;
	int i, j;
	BOOL found = FALSE;
	BOOL bFoundDBIndex = FALSE;

	for( i = 0; i < g_server.GetSize(); i++)
	{
		if(found) break;

		if(g_server[i])
		{
			if( g_server[i]->m_sDBIndex != -1 )
			{
				bFoundDBIndex = FALSE;

				for( j = 0; j < g_arMyServer.GetSize(); j++ )
				{
					if( *(g_arMyServer[j]) == g_server[i]->m_sDBIndex )
					{
						bFoundDBIndex = TRUE;
						break;
					}
				}

				if( !bFoundDBIndex ) continue;
			}

			for( j = 0; j < g_server[i]->m_zone.GetSize(); j++)
			{
				if(found) break;

				if(g_server[i]->m_zone[j])
				{
					if( *(g_server[i]->m_zone[j]) == zone)
					{
						server_index = i;
						found = TRUE;
					}
				}
			}
		}
	}

	if(server_index == -1) 
	{
		TRACE("Can't find valid point...\n");
		return;// FALSE;
	}
/*
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call CHANGE_SERVER (\'%s\',%d)}"), m_strUserID, server_index);

	int db_index = 0;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
//		bQuerySuccess = FALSE;
	}

	retcode = SQLExecDirect (hstmt, (unsigned char *)szSQL, SQL_NTS);
	if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
	}
	else if (retcode==SQL_ERROR)
	{
		DisplayErrorMsg(hstmt);
		if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBSession[m_iModSid].ReleaseDB(db_index);
*/
}

///////////////////////////////////////////////////////////////////////////
//	Zone Index �� �����Ѵ�.
//
void USER::SetZoneIndex(int zone)
{
	for(int i = 0; i < g_zone.GetSize(); i++)
	{
		if( g_zone[i]->m_Zone == zone )
		{
			m_ZoneIndex = i;
			break;
		}
	}
}

//######################################################################################
//	Send Functions

////////////////////////////////////////////////////////////////////////////////////////
//	�þ߰� ����ƴ��� �Ǵ��ϰ� ����ƴٸ� ���泻���� Ŭ���̾�Ʈ�� �����Ѵ�.
//
void USER::SightRecalc()
{
	int sx, sy;
	sx = m_curx / SIGHT_SIZE_X;
	sy = m_cury / SIGHT_SIZE_Y;

	int dir_x = 0;
	int dir_y = 0;

	if( sx == m_presx && sy == m_presy ) return;
	
	if( m_presx == -1 || m_presy == -1 )
	{
		dir_x = 0;
		dir_y = 0;
	}
	else if( m_presx == -2 || m_presy == -2 )
	{
		dir_x = DIR_OUTSIDE;
		dir_y = DIR_OUTSIDE;
	}
	else
	{
		if( sx > m_presx && abs(sx-m_presx) == 1 )		dir_x = DIR_H;
		if( sx < m_presx && abs(sx-m_presx) == 1 )		dir_x = DIR_L;
		if( sy > m_presy && abs(sy-m_presy) == 1 )		dir_y = DIR_H;
		if( sy < m_presy && abs(sy-m_presy) == 1 )		dir_y = DIR_L;
		if( abs(sx-m_presx) > 1 )						dir_x = DIR_OUTSIDE;
		if( abs(sy-m_presy) > 1 )						dir_y = DIR_OUTSIDE;
	}

//	TRACE("Sight changed....[%d] [%d] -> [%d] [%d] (%d %d)\n", m_presx, m_presy, sx, sy, dir_x, dir_y);

	int prex = m_presx;
	int prey = m_presy;
	m_presx = sx;
	m_presy = sy;

	SendUserInfoBySightChange(dir_x, dir_y, prex, prey);
}

/////////////////////////////////////////////////////////////////////////////////////
//	�þߺ������� ���� �������� ����
//
void USER::SendUserInfoBySightChange(int dir_x, int dir_y, int prex, int prey)
{
	int min_x = 0, min_y = 0;
	int max_x = 0, max_y = 0;

	int sx = m_presx;
	int sy = m_presy;

	int modify_index = 0;
	TCHAR modify_send[10000];	::ZeroMemory(modify_send, sizeof(modify_send));
	FillUserInfo(modify_send, modify_index, INFO_MODIFY);

	int delete_index = 0;
	TCHAR delete_send[4096];	::ZeroMemory(delete_send, sizeof(delete_send));
	FillUserInfo(delete_send, delete_index, INFO_DELETE);

	if( prex == -1 || prey == -1 )
	{
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
//		SendItemFieldInfoToMe();
		SendCompressedRangeInfoToMe();
		return;
	}
	if( dir_x == DIR_OUTSIDE || dir_y == DIR_OUTSIDE )
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);	// ��� �����ʹ� ��õ����.
//		SendItemFieldInfoToMe();
		SendCompressedRangeInfoToMe();
		return;
	}
	if( dir_x > 0 )
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx+1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
//		SendItemFieldInfoToMe();
	}
	if( dir_y > 0 )
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy+1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
//		SendItemFieldInfoToMe();
	}
	if( dir_x < 0 )
	{
		min_x = (prex+1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
//		SendItemFieldInfoToMe();
	}
	if( dir_y < 0 )
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey+1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
//		SendItemFieldInfoToMe();
	}

	SendCompressedRangeInfoToMe();
}

////////////////////////////////////////////////////////////////////////////////////
//	���� �þ� ������ �����Ͽ� ������
//
void USER::SendCompressedRangeInfoToMe()
{
	if( !m_CompCount || m_CompCount < 0 || !m_iCompIndex || m_iCompIndex < 0 )
	{
		m_CompCount = 0;
		m_iCompIndex = 0;
		return;
	}

	m_CompMng.FlushAddData();

	m_CompMng.AddData( m_CompBuf, m_iCompIndex );

	m_CompMng.PreCompressWork();
	m_CompMng.Compress();

	int comp_data_len = m_CompMng.GetCompressedDataCount();
	int org_data_len = m_CompMng.GetUnCompressDataLength();
	DWORD crc_value = m_CompMng.GetCrcValue();

	CBufferEx	TempBuf;

	TempBuf.Add( SIGHT_INFO_COMPRESSED );
	TempBuf.Add( (short)comp_data_len );
	TempBuf.Add( (short)org_data_len );
	TempBuf.Add( crc_value );
	TempBuf.Add( (short)m_CompCount );

	char* packet = m_CompMng.GetExtractedBufferPtr();

	TempBuf.AddData( packet, comp_data_len );

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if(pNewData == NULL)
	{
		m_CompCount = 0;
		m_iCompIndex = 0;
		m_CompMng.FlushAddData();
		return;
	}

	pNewData->flag = SEND_USER;
	pNewData->len = TempBuf.GetLength();

	::CopyMemory(pNewData->pBuf, TempBuf, TempBuf.GetLength());

	pNewData->uid = m_uid;

	// IKING 2001.1.
	//EnterCriticalSection( &(m_pCom->m_critSendData) );
	//m_pCom->m_arSendData.Add( pNewData );
	//LeaveCriticalSection( &(m_pCom->m_critSendData) );
	//PostQueuedCompletionStatus( m_pCom->m_hSendIOCP, 0, 0, NULL );
	m_pCom->Send(pNewData);

	if(pNewData) delete pNewData;
	//

	m_CompCount = 0;
	m_iCompIndex = 0;
	m_CompMng.FlushAddData();
}

////////////////////////////////////////////////////////////////////////////////////
//	Ư�� ���� �Ǵ� NPC�� ������ Ŭ���̾�Ʈ���� ��û�� ��� ó��
//
void USER::UserInfoReq(TCHAR *pBuf)
{
	int index = 0;
	int nUid = GetInt(pBuf, index);
	
	if(nUid < 0 || nUid >= INVALID_BAND) return;
	
	if(nUid >= USER_BAND && nUid < NPC_BAND)
	{
		USER* pSendUser = GetUser(nUid - USER_BAND);	
//		if(pSendUser == NULL || pSendUser->m_curz != m_curz || pSendUser->m_state != STATE_GAMESTARTED) return;
		if( pSendUser == NULL || pSendUser->m_state != STATE_GAMESTARTED ) return;

		if( !IsInSight( pSendUser->m_curx, pSendUser->m_cury, pSendUser->m_curz ) ) return;
		
		SendUserInfo(pSendUser, INFO_MODIFY);
	}
	if(nUid >= NPC_BAND && nUid < INVALID_BAND)
	{
		CNpc* pNpc = GetNpc(nUid - NPC_BAND);
		if(pNpc == NULL) return;

		if( !IsInSight( pNpc->m_sCurX, pNpc->m_sCurY, pNpc->m_sCurZ ) ) return;

		SendNpcInfo(pNpc, INFO_MODIFY);
	}
}

BOOL USER::IsInSight(int x, int y, int z)
{
	if( m_curz != z ) return FALSE;

	int min_x, max_x;
	int min_y, max_y;

	int sx = m_curx / SIGHT_SIZE_X;
	int sy = m_cury / SIGHT_SIZE_Y;
	
	min_x = (sx-1)*SIGHT_SIZE_X; if( min_x < 0 ) min_x = 0;
	max_x = (sx+2)*SIGHT_SIZE_X;
	min_y = (sy-1)*SIGHT_SIZE_Y; if( min_y < 0 ) min_y = 0;
	max_y = (sy+2)*SIGHT_SIZE_Y;
	
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;

	MAP* pMap = g_zone[m_ZoneIndex];
	if( !pMap ) return FALSE;
	
	if( max_x >= pMap->m_sizeMap.cx ) max_x = pMap->m_sizeMap.cx - 1;
	if( max_y >= pMap->m_sizeMap.cy ) max_y = pMap->m_sizeMap.cy - 1;

	if( min_x > x || max_x < x ) return FALSE;
	if( min_y > y || max_y < y ) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//	NPC ������ ������.
//
void USER::SendNpcInfo(CNpc *pNpc, BYTE tMode /* INFO_MODIFY */)
{
	if( !pNpc ) return;

	CBufferEx TempBuf;
	TempBuf.Add((BYTE)NPC_INFO);
	TempBuf.Add((BYTE)tMode);
	TempBuf.Add((short)(pNpc->m_sNid + NPC_BAND));
	
	if(tMode != INFO_MODIFY)
	{
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	TempBuf.Add((short)pNpc->m_sPid);
	TempBuf.AddString(pNpc->m_strName);

	CPoint pt = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
	if(pt.x == -1 || pt.y == -1) { pt.x = 1; pt.y = 1; }
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);

//	if(pNpc->m_NpcState == NPC_DEAD) TempBuf.Add((BYTE)0x00);
	if(pNpc->m_sHP == 0) TempBuf.Add((BYTE)0x00);
	else TempBuf.Add((BYTE)0x01);

	TempBuf.Add(pNpc->m_tNpcType);	// NPC Type

	TempBuf.Add((short)pNpc->m_sMaxHP);	// NPC ���� ���¸� ǥ���ϱ� ����(Client �� ����)	
	TempBuf.Add((short)pNpc->m_sHP);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add(pNpc->m_sClientSpeed);
	TempBuf.Add(pNpc->m_byColor);

	if(pNpc->m_tNpcType == NPCTYPE_GUILD_DOOR)
	{
		TempBuf.Add(pNpc->m_sDimension);
	}
	TempBuf.Add(pNpc->m_sQuestSay);
	
	Send(TempBuf, TempBuf.GetLength());
}

int USER::MakeRangeInfoToMe(CNpc* pNpc, BYTE tMode, TCHAR *pData)
{
	if( !pNpc ) return 0;
	CPoint pt = ConvertToClient( pNpc->m_sCurX, pNpc->m_sCurY );
	if( pt.x == -1 || pt.y == -1 ) return 0;

	int index = 0;

	SetByte( pData, (BYTE)NPC_INFO, index );
	SetByte( pData, tMode, index );
	SetShort( pData, (short)(pNpc->m_sNid + NPC_BAND), index );

	if( tMode != INFO_MODIFY )
	{
		return index;
	}

	SetShort( pData, (short)pNpc->m_sPid, index );
	SetByte( pData, (BYTE)strlen( pNpc->m_strName ), index);
	SetString( pData, pNpc->m_strName, strlen( pNpc->m_strName ), index );

	SetShort( pData, (short)pt.x, index );
	SetShort( pData, (short)pt.y, index );

	if( pNpc->m_sHP == 0 ) SetByte( pData, 0X00, index );
	else SetByte( pData, 0X01, index );

	SetByte( pData, pNpc->m_tNpcType, index );

	SetShort( pData, (short)pNpc->m_sMaxHP, index );
	SetShort( pData, (short)pNpc->m_sHP, index );
	SetByte( pData , 0x00, index);
	SetByte( pData , 0x00, index);
	SetByte( pData , 0x00, index);
	SetByte( pData , 0x00, index);
	SetByte( pData , 0x00, index);
	SetByte( pData , 0x00, index);
	SetByte( pData , 0x00, index);
	SetByte( pData , 0x00, index);
	SetShort( pData, pNpc->m_sClientSpeed, index );
	SetByte( pData, pNpc->m_byColor, index );

	if(pNpc->m_tNpcType == NPCTYPE_GUILD_DOOR) 
	{
		SetShort( pData, pNpc->m_sDimension, index);
	}
	SetShort(pData, pNpc->m_sQuestSay, index);

	return index;
}

void USER::AddRangeInfoToMe(CNpc *pNpc, BYTE tMode)
{
	int index = 0;
	TCHAR pData[1024];

	index = MakeRangeInfoToMe( pNpc, tMode, pData );

	if( index )
	{
		m_CompCount++;

		SetShort( m_CompBuf, index, m_iCompIndex );				// ������� ���� ������ ����
		::CopyMemory( m_CompBuf+m_iCompIndex, pData, index );	// ������� ������ ������ ���ۿ� ����
		m_iCompIndex += index;

		if( m_iCompIndex >= 8000 )
		{
			SendCompressedRangeInfoToMe();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
//	Ư�� ������ ���� ������ ������.
//
void USER::SendUserInfo(USER *pUser, BYTE tMode	/*INFO_MODIFY*/)
{
	if( !pUser ) return;

	CBufferEx	TempBuf;
	int i;

	CPoint pos = ConvertToClient( pUser->m_curx, pUser->m_cury );
	if( pos.x == -1 || pos.y == -1 ) { pos.x = 1; pos.y = 1; }

	TempBuf.Add(USER_INFO);
	TempBuf.Add(tMode);
	TempBuf.Add(pUser->m_uid + USER_BAND);

	if(tMode != INFO_MODIFY)
	{
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	TempBuf.AddString(pUser->m_strUserID);

	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);

	TempBuf.Add(pUser->m_iSkin);
	TempBuf.Add(pUser->m_iHair);
	TempBuf.Add((BYTE)pUser->m_sGender);
	TempBuf.AddData(pUser->m_strFace, 10);

	for( i = 0; i < EQUIP_ITEM_NUM; i++) TempBuf.Add(pUser->m_UserItem[i].sSid);

	TempBuf.Add(pUser->m_sHP);
	TempBuf.Add(pUser->m_sMagicMaxHP);

	TempBuf.Add(pUser->m_tDir);
	TempBuf.Add(pUser->m_dwAbnormalInfo);				// �����̻� ������ ������ m_dwAbnormalInfo �ϳ��� �����Ѵ�. JJS07 2002.10.14
	TempBuf.Add(pUser->m_dwAbnormalInfo_);	
	TempBuf.Add((DWORD)0);
	TempBuf.Add((DWORD)0);
	TempBuf.Add(pUser->m_sCityRank);

	TempBuf.Add((int)pUser->m_dwGuild);					//&&&&&&&&&&&& Test Code
	TempBuf.AddString(pUser->m_strGuildName);			// ��� �̸��� �߰�
	TempBuf.Add(pUser->m_sGuildVersion);

	TempBuf.Add(pUser->m_byClass);
	TempBuf.Add((BYTE)pUser->m_bPkStatus);
	TempBuf.AddString(pUser->m_strLoveName); //yskang 0.1

	for(i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) TempBuf.Add(pUser->m_UserItem[i].sSid);	// EBody
	if(m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
		TempBuf.Add((BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]));
		TempBuf.Add((BYTE)0x00);
	}else{
		TempBuf.Add(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]);
		TempBuf.Add((BYTE)0xff);
	}
	TempBuf.AddString(pUser->m_PersonalShopName);
	Send(TempBuf, TempBuf.GetLength());
}

int USER::MakeRangeInfoToMe(USER* pUser, BYTE tMode, TCHAR *pData)
{
	int nLen = 0;
	int i = 0;

	if( !pUser ) return 0;

	CPoint pos = ConvertToClient( pUser->m_curx, pUser->m_cury );
	if( pos.x == -1 || pos.y == -1 ) return 0;

	int index = 0;

	SetByte( pData, USER_INFO, index );
	SetByte( pData, tMode, index );
	SetInt(  pData, pUser->m_uid + USER_BAND, index );

	if( tMode != INFO_MODIFY )
	{
		return index;
	}

	SetByte( pData, (BYTE)strlen( pUser->m_strUserID ), index );
	SetString( pData, pUser->m_strUserID, strlen( pUser->m_strUserID ), index );

	SetShort( pData, pos.x, index );
	SetShort( pData, pos.y, index );

	SetDWORD( pData, pUser->m_iSkin, index );
	SetDWORD( pData, pUser->m_iHair, index );
	SetByte( pData, (BYTE)pUser->m_sGender, index );

	::CopyMemory( pData+index, pUser->m_strFace, 10 );
	index += 10;

	for( i = 0; i < EQUIP_ITEM_NUM; i++ ) SetShort( pData, pUser->m_UserItem[i].sSid, index );

	SetShort( pData, pUser->m_sHP, index );
	SetShort( pData, pUser->m_sMagicMaxHP, index );

	SetByte( pData, pUser->m_tDir, index );

	SetDWORD( pData, pUser->m_dwAbnormalInfo, index);			// �����̻� ����
	SetDWORD( pData, pUser->m_dwAbnormalInfo_, index);
	SetDWORD( pData, 0, index);
	SetDWORD( pData, 0, index);

	/*SetByte( pData, 0x00, index );
	SetByte( pData, 0x00, index );
	SetByte( pData, 0x00, index );
	SetByte( pData, 0x00, index );
	SetByte( pData, 0x00, index );
	SetByte( pData, 0x00, index );
	SetByte( pData, 0x00, index );
	SetByte( pData, 0x00, index );
	*/

	SetShort( pData, pUser->m_sCityRank, index );
	SetInt( pData, pUser->m_dwGuild, index );

	nLen = strlen(pUser->m_strGuildName);
	if(nLen <= 0) nLen = 1;

	SetByte( pData, (BYTE)nLen, index );
	SetString( pData, pUser->m_strGuildName, nLen, index );		// Add Guild Name
	SetShort( pData, pUser->m_sGuildVersion, index );

	SetByte( pData, pUser->m_byClass, index );
	SetByte( pData, pUser->m_bPkStatus, index );
	//-- yskang 0.1 �߰��Ǵ� ��Ŷ.... ȣĪ[��Ī]
	nLen = strlen(pUser->m_strLoveName);
	if(nLen < 1) nLen =1;
	SetByte( pData, (BYTE)nLen, index );
	SetString(pData,pUser->m_strLoveName,nLen,index);

	for( i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) 
		SetShort( pData, pUser->m_UserItem[i].sSid, index );
	//SetByte( pData, 0x00, index );
	//SetByte( pData, 0x00, index );
	if(pUser->m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&pUser->m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
			SetByte(pData,(BYTE)(pUser->m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]),index);
			SetByte( pData, 0x00, index );
		}else{
			SetByte(pData,(BYTE)(pUser->m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]),index);
			SetByte( pData, 0xff, index );
		}
		
	SetByte(pData, strlen(pUser->m_PersonalShopName), index);
	SetString(pData, pUser->m_PersonalShopName, strlen(pUser->m_PersonalShopName), index);
//SaveDump(pData,index);
	return index;
}

void USER::AddRangeInfoToMe(USER *pUser, BYTE tMode)
{
	int index = 0;
	TCHAR pData[1024];

	index = MakeRangeInfoToMe( pUser, tMode, pData );

	if( index )
	{
		m_CompCount++;

		SetShort( m_CompBuf, index, m_iCompIndex );				// ������� ���� ������ ����
		::CopyMemory( m_CompBuf+m_iCompIndex, pData, index );	// ������� ������ ������ ���ۿ� ����
		m_iCompIndex += index;

		if( m_iCompIndex >= 8000 )
		{
			SendCompressedRangeInfoToMe();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//����:������������
//����ֵ:��
// �汾 1.00
//���� 2006.12.21
void USER::SendCharData()
{
	int i = 0;
	CBufferEx TempBuf;
	
	TempBuf.Add(CHAR_DATA);
	TempBuf.AddString(m_strUserID);
	TempBuf.Add(m_sMagicSTR);
	TempBuf.Add(m_sMagicCON);
	TempBuf.Add(m_sMagicDEX);
	TempBuf.Add(m_sMagicVOL);
	TempBuf.Add(m_sMagicWIS);

	TempBuf.Add((short)1);  // �ӽ� �ڵ�

	TempBuf.Add(m_iSkin);
	TempBuf.Add(m_iHair);
	TempBuf.Add((BYTE)m_sGender);
	TempBuf.AddData(m_strFace, 10);

	TempBuf.Add(m_dwExp);
	TempBuf.Add(m_dwXP);

	TempBuf.Add(m_sSkillPoint);
	TempBuf.Add(m_sPA);
//ţţadd
	TempBuf.Add(m_sPA);

	TempBuf.Add(m_sMagicMaxHP);
	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sMagicMaxPP);
	TempBuf.Add(m_sPP);
	TempBuf.Add(m_sMagicMaxSP);
	TempBuf.Add(m_sSP);

	TempBuf.Add(m_dwDN);
	TempBuf.Add(m_sCityRank);

	TempBuf.Add(m_sLevel);
	TempBuf.Add(m_byClass);

	TCHAR strSkill[_SKILL_DB], strItem[_ITEM_DB], strPsi[_PSI_DB],strSkill_[12*3];
	::ZeroMemory(strSkill, sizeof(strSkill));
	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strPsi, sizeof(strPsi));

	UserSkillToStr((LPTSTR)strSkill);

	UserItemToStrForSend( (LPTSTR)strItem );
	UserPsiToStr((LPTSTR)strPsi);
	int index=0;
	for(i = 0; i < 12; i++){
		SetShort(strSkill_, m_UserSkill_[i].sSid,		index);
		SetByte (strSkill_, m_UserSkill_[i].tLevel,	index);
	}
	TempBuf.AddData(strSkill, USER_SKILL_LEN);		// Skill
	TempBuf.AddData(strSkill_,12*3);
	TempBuf.Add((BYTE)m_nHavePsiNum);				// Psionic
	if(m_nHavePsiNum > 0) TempBuf.AddData(strPsi, m_nHavePsiNum * _PSI_SIZE);

	TempBuf.AddData(strItem, USER_ITEM_LEN);		// Item

	TempBuf.Add(m_dwExpNext);// Next Exp
	
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);

	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00)
		;
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);

	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);

	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);

	TempBuf.Add((int)m_dwGuild);					//&&&&&&&&&&&&&& Test Code
	TempBuf.AddString(m_strGuildName);			// ��� �̸��� �߰�
	TempBuf.Add(m_sGuildVersion);

	if(m_dwGuild >= 1)
	{
		//TempBuf.Add((BYTE)0xF4);
		//TempBuf.Add((BYTE)0x02);
		//TempBuf.Add((BYTE)0x41);
		TempBuf.Add((BYTE)0x00);
		TempBuf.Add((BYTE)0x00);
		TempBuf.Add((BYTE)0x00);
	}
	else
	{
		TempBuf.Add((BYTE)0x00);
		TempBuf.Add((BYTE)0x00);
		TempBuf.Add((BYTE)0x00);
	}
	
//	TempBuf.Add((short)m_iMaxWeight);
//	TempBuf.Add((short)m_iCurWeight);
	//
	TempBuf.Add((short)0);
	TempBuf.Add((short)0);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x84); // Finito Added
	TempBuf.Add((BYTE)0x99); // Finito Added
	TempBuf.Add((BYTE)0x2c); // Finito Added
	TempBuf.Add((BYTE)0x01); // Finito Added
//SaveDump(TempBuf,TempBuf.GetLength());
	CBufferEx TempBuf1;

	TempBuf1.Add( (short)(TempBuf.GetLength()) );
	TempBuf1.AddData( TempBuf, TempBuf.GetLength() );

	// ���� �غ� 
	m_CompMng.FlushAddData();

	m_CompMng.AddData( TempBuf1, TempBuf1.GetLength() );

	m_CompMng.PreCompressWork();
	m_CompMng.Compress();

	int comp_data_len = m_CompMng.GetCompressedDataCount();
	int org_data_len = m_CompMng.GetUnCompressDataLength();
	DWORD crc_value = m_CompMng.GetCrcValue();

	CBufferEx SendBuf;

	SendBuf.Add( SIGHT_INFO_COMPRESSED );
	SendBuf.Add( (short)comp_data_len );
	SendBuf.Add( (short)org_data_len );
	SendBuf.Add( crc_value );
	SendBuf.Add( (short)1 );

	char* packet = m_CompMng.GetExtractedBufferPtr();

	SendBuf.AddData( packet, comp_data_len );
	
	TRACE( "%d -> %d\n", org_data_len, comp_data_len );

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if(pNewData == NULL)
	{
		m_CompMng.FlushAddData();
		return;
	}

	pNewData->flag = SEND_USER;
	pNewData->len = SendBuf.GetLength();

	::CopyMemory(pNewData->pBuf, SendBuf, SendBuf.GetLength());

	pNewData->uid = m_uid;

	m_pCom->Send(pNewData);
	if(pNewData) delete pNewData;
	//

	m_CompMng.FlushAddData();

}

//////////////////////////////////////////////////////////////////////////////////
//	����: �����Լ���������
//  ����ֵ: ��
//  �汾: 1.00 (ţţ����)
///////////////////////////////////////////////////////////////////////////////////
void USER::SendMyInfo(BYTE towho, BYTE type)
{
	CBufferEx	TempBuf;
	int i;

	TempBuf.Add(USER_INFO);
	TempBuf.Add(type);
	TempBuf.Add(m_uid + USER_BAND);

	CPoint pos = ConvertToClient( m_curx, m_cury );
	if( pos.x == -1 || pos.y == -1 ) { pos.x = 1; pos.y = 1; }

/*	if(type == INFO_MODIFY)
	{
	*/
		TempBuf.AddString(m_strUserID);
		
		TempBuf.Add((short)pos.x);
		TempBuf.Add((short)pos.y);
		
		TempBuf.Add(m_iSkin);
		TempBuf.Add(m_iHair);
		TempBuf.Add((BYTE)m_sGender);
		TempBuf.AddData(m_strFace, 10);
		
		for(i = 0; i < EQUIP_ITEM_NUM; i++) TempBuf.Add(m_UserItem[i].sSid);

		TempBuf.Add(m_sHP);
		TempBuf.Add(m_sMagicMaxHP);

		TempBuf.Add(m_tDir);

		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);
		TempBuf.Add((DWORD)0);
		TempBuf.Add((DWORD)0);
		TempBuf.Add(m_sCityRank);

		TempBuf.Add((int)m_dwGuild);
		TempBuf.AddString(m_strGuildName);
		TempBuf.Add(m_sGuildVersion);
		TempBuf.Add(m_byClass);
		TempBuf.Add((BYTE)m_bPkStatus);
		TempBuf.AddString(m_strLoveName);

		for( i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) 
			TempBuf.Add(m_UserItem[i].sSid);	// EBody
		if(m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
			//BYTE code=;
			//static BYTE code=0x00;
			TempBuf.Add((BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]));
			//code++;
			TempBuf.Add((BYTE)0x00);
		}else{
			//TempBuf.Add(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]);
			TempBuf.Add(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]);
			TempBuf.Add((BYTE)0xff);
		}
		TempBuf.AddString(m_PersonalShopName);
//SaveDump(TempBuf,TempBuf.GetLength());
	switch(towho)
	{
	case TO_ALL:
		SendAll(TempBuf, TempBuf.GetLength());
		break;

	case TO_ME:
		Send(TempBuf, TempBuf.GetLength());
		break;

	case TO_ZONE:
		SendZone(TempBuf, TempBuf.GetLength());
		break;

	case TO_INSIGHT:
	default:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	��� �������� ������ ����
///////////////////////////////////////////////////////////////////////////////
void USER::SendAll(TCHAR *pBuf, int nLength)
{
	if( nLength <= 0 || nLength >= SEND_BUF_SIZE ) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if( !pNewData ) return;

	pNewData->flag = SEND_ALL;
	pNewData->len = nLength;

	::CopyMemory( pNewData->pBuf, pBuf, nLength );

	// IKING 2001.1.
	//EnterCriticalSection( &(m_pCom->m_critSendData) );
	//m_pCom->m_arSendData.Add( pNewData );
	//LeaveCriticalSection( &(m_pCom->m_critSendData) );
	//PostQueuedCompletionStatus( m_pCom->m_hSendIOCP, 0, 0, NULL );
	m_pCom->Send(pNewData);
	if(pNewData) delete pNewData;
	
}

///////////////////////////////////////////////////////////////////////////////
//Zone------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
void USER::SendZone(TCHAR *pBuf, int nLength)
{
	if( nLength <= 0 || nLength >= SEND_BUF_SIZE ) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if( !pNewData ) return;

	pNewData->flag = SEND_ZONE;
	pNewData->len = nLength;

	::CopyMemory( pNewData->pBuf, pBuf, nLength );

	pNewData->z = m_curz;

	// IKING 2001.1.
	//EnterCriticalSection( &(m_pCom->m_critSendData) );
	//m_pCom->m_arSendData.Add( pNewData );
	//LeaveCriticalSection( &(m_pCom->m_critSendData) );
	//PostQueuedCompletionStatus( m_pCom->m_hSendIOCP, 0, 0, NULL );
	m_pCom->Send(pNewData);
	if(pNewData) delete pNewData;
	//

	/*
	USER* pUser = NULL;

	for(int i = 0; i < MAX_USER; i++)
	{
		pUser = m_pCom->user_array[i];

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED || pUser->m_curz != m_curz) continue;

		pUser->Send(pBuf, nLength);
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////
//	Ư���������� �ִ� �������� ������ ����
///////////////////////////////////////////////////////////////////////////////
void USER::SendRange(TCHAR *pBuf, int nLength, CRect rect)
{
	if( nLength <= 0 || nLength >= SEND_BUF_SIZE) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if( !pNewData ) return;

	pNewData->flag = SEND_RANGE;
	pNewData->len = nLength;

	::CopyMemory( pNewData->pBuf, pBuf, nLength );

	pNewData->uid = 0;
	pNewData->z = m_curz;
	pNewData->rect = rect;
	pNewData->zone_index = m_ZoneIndex;

	// IKING 2001.1.
	//EnterCriticalSection( &(m_pCom->m_critSendData) );
	//m_pCom->m_arSendData.Add( pNewData );
	//LeaveCriticalSection( &(m_pCom->m_critSendData) );
	//PostQueuedCompletionStatus( m_pCom->m_hSendIOCP, 0, 0, NULL );
	m_pCom->Send(pNewData);
	if(pNewData) delete pNewData;
	//
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
void USER::SendInsight(TCHAR *pBuf, int nLength)
{
	if(nLength <= 0 || nLength >= SEND_BUF_SIZE) return;
	
	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if(pNewData == NULL) return;

	pNewData->flag = SEND_INSIGHT;
	pNewData->len = nLength;

	::CopyMemory(pNewData->pBuf, pBuf, nLength);

	pNewData->uid = 0;
	pNewData->x = m_curx;
	pNewData->y = m_cury;
	pNewData->z = m_curz;
	pNewData->zone_index = m_ZoneIndex;

	// IKING 2001.1.
	//EnterCriticalSection( &(m_pCom->m_critSendData) );
	//m_pCom->m_arSendData.Add( pNewData );
	//LeaveCriticalSection( &(m_pCom->m_critSendData) );
	//PostQueuedCompletionStatus( m_pCom->m_hSendIOCP, 0, 0, NULL );
	m_pCom->Send(pNewData);
	if(pNewData) delete pNewData;
}

void USER::SendExactScreen(TCHAR *pBuf, int nLength)
{
	if(nLength <= 0 || nLength >= SEND_BUF_SIZE) return;
	
	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if(pNewData == NULL) return;

	pNewData->flag = SEND_SCREEN;
	pNewData->len = nLength;

	::CopyMemory(pNewData->pBuf, pBuf, nLength);

	pNewData->uid = 0;
	pNewData->x = m_curx;
	pNewData->y = m_cury;
	pNewData->z = m_curz;
	pNewData->zone_index = m_ZoneIndex;

	// IKING 2001.1.
	//EnterCriticalSection( &(m_pCom->m_critSendData) );
	//m_pCom->m_arSendData.Add( pNewData );
	//LeaveCriticalSection( &(m_pCom->m_critSendData) );
	//PostQueuedCompletionStatus( m_pCom->m_hSendIOCP, 0, 0, NULL );
	m_pCom->Send(pNewData);
	if(pNewData) delete pNewData;
	//
}

//////////////////////////////////////////////////////////////////////////////
//	�þߺ����� ���� �ִ� �þ��� �������� ������ ����
//
void USER::SendRemainSight(TCHAR *pBuf, int nLength, CPoint pt)
{
/*	USER* pSendUser = NULL;
	CCellUidArray arUid;
	int nDirection = g_cell[m_nCellZone]->GetDirection(pt, m_ptCell);

	int nD = nDirection - 1;
	int xx = 0, yy = 0, i, j;
	int nUid = 0, nSize = 0;
	int cx, cy;

	for(i = 0; i < 6; i++)
	{
		if(g_rPt[nD][i].x == 100) break;
		
		// Add ...
		xx = g_rPt[nD][i].x;
		yy = g_rPt[nD][i].y;
		
		cx = pt.x + xx;
		cy = pt.y + yy;
		if(cx < 0 || cx >= g_cell[m_nCellZone]->m_sizeCell.cx) continue;
		if(cy < 0 || cy >= g_cell[m_nCellZone]->m_sizeCell.cy) continue;
		
		g_cell[m_nCellZone]->m_pCell[cx][cy].Get(arUid);
		nSize = arUid.GetSize();
		
		for(j = 0; j < nSize; j++)
		{
			nUid = arUid.GetAt(j);
			if(nUid >= USER_BAND && nUid < NPC_BAND)
			{
				pSendUser = m_pCom->GetUserUid(nUid - USER_BAND);		
				if(pSendUser == NULL || pSendUser->m_state != STATE_GAMESTARTED || pSendUser->m_curz != m_curz) continue;
				
				pSendUser->Send(pBuf, nLength);
			}
		}
	}

	arUid.RemoveAll();
*/
}

//////////////////////////////////////////////////////////////////////////////
//	�� ȭ�鳻�� �������� ������ ����
//
void USER::SendScreen(TCHAR *pBuf, int nLength)
{
	USER* pUser = NULL;
	int dx, dy;

	for(int i = 0; i< MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED || pUser->m_curz != m_curz) continue;

		dx = abs(m_curx - pUser->m_curx);
		dy = abs(m_cury - pUser->m_cury);

		if((dx + dy) <= (SCREEN_Y * 2))
		{
			pUser->Send(pBuf, nLength);
		}
	}
}

//##############################################################################
//
//	PC ������ ���� ��ƾ
//
//##############################################################################

//////////////////////////////////////////////////////////////////////////////
//	������ȯ �䱸�� ó���Ѵ�.
//
void USER::ChangeDir(TCHAR *pBuf)
{
	int		index = 0;
	BYTE	dir;

	dir = GetByte(pBuf, index);
	if(dir < 0 || dir >= 8) return;

	m_tDir = dir;

	CBufferEx TempBuf;
	TempBuf.Add(CHANGE_DIR_RESULT);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add(m_tDir);

//	SendInsight(TempBuf, TempBuf.GetLength());
	SendExactScreen(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	���� �����ִ� ���⸦ ǥ���Ѵ�. 
//
int USER::GetDirection(CPoint ptStart, CPoint ptEnd, int nDirCount)
{
	int nDir;					//	3 4 5
								//	2 8 6
								//	1 0 7
	int deltax = ptEnd.x - ptStart.x;
	int deltay = ptEnd.y - ptStart.y;

	if (deltax==0) {
		if (ptEnd.y>ptStart.y) nDir = DIR_DOWNRIGHT;		// ->		
		else nDir = DIR_UPLEFT;							// ->
	}
	else {
		double lean = (double)abs(deltay)/abs(deltax);

		if (ptEnd.y>ptStart.y) {
			if (ptEnd.x>ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_RIGHT;
					else if (lean>2.) nDir = DIR_DOWN;
					else nDir = DIR_RIGHT;		// -> 
				}
				else {
					if (lean<.2) nDir = DIR_RIGHT;
					else if (lean<.4) nDir = DIR_RIGHT+8;
					else if (lean<1) nDir = DIR_DOWNRIGHT;
					else if (lean<5) nDir = DIR_DOWNRIGHT+8;
					else			 nDir = DIR_DOWN;
				}

			}
			else if (ptEnd.x < ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_LEFT;
					else if (lean>2.) nDir = DIR_DOWN;
					else nDir = DIR_DOWN;		// -> 
				}
				else {
					if (lean<.2) nDir = DIR_LEFT;
					else if (lean<.4) nDir = DIR_LEFT+7;
					else if (lean<1) nDir = DIR_DOWNLEFT;
					else if (lean<5) nDir = DIR_DOWNLEFT+7;
					else			 nDir = DIR_DOWN;
				}
			}
		}
		else {
			if (ptEnd.x>ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_UPRIGHT;	// ->
					else if (lean>2.) nDir = DIR_UP;
					else nDir = DIR_UP;		// ->
				}
				else {
					if (lean<.2) nDir = DIR_RIGHT;
					else if (lean<.4) nDir = DIR_RIGHT+7;
					else if (lean<1) nDir = DIR_UPRIGHT;
					else if (lean<5) nDir = DIR_UPRIGHT+7;
					else			 nDir = DIR_UP;
				}
			}
			else if (ptEnd.x < ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_DOWNLEFT;		// -> 
					else if (lean>2.) nDir = DIR_UP;
					else nDir = DIR_LEFT;				// -> 2()
				} else {
					if (lean<.2) nDir = DIR_LEFT;
					else if (lean<.4) nDir = DIR_LEFT+8;
					else if (lean<1) nDir = DIR_UPLEFT;
					else if (lean<5) nDir = DIR_UPLEFT+8;
					else			 nDir = DIR_UP;
				}
			}
		}
	}

	return nDir;
/*if (deltax==0){
		if (ptEnd.y>ptStart.y) nDir = DIR_DOWN;		// ->DIR_DOWNRIGHT		
		else nDir = DIR_UP;							// ->DIR_UPLEFT
	}
	else {
		double lean = (double)abs(deltay)/abs(deltax);

		if (ptEnd.y>ptStart.y) {
			if (ptEnd.x>ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_RIGHT;
					else if (lean>2.) nDir = DIR_DOWN;
					else nDir = DIR_DOWNRIGHT;		// -> DIR_RIGHT
				}
				else {
					if (lean<.2) nDir = DIR_RIGHT;
					else if (lean<.4) nDir = DIR_RIGHT+8;
					else if (lean<1) nDir = DIR_DOWNRIGHT;
					else if (lean<5) nDir = DIR_DOWNRIGHT+8;
					else			 nDir = DIR_DOWN;
				}

			}
			else if (ptEnd.x < ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_LEFT;
					else if (lean>2.) nDir = DIR_DOWN;
					else nDir = DIR_DOWNLEFT;		// -> DIR_DOWN
				}
				else {
					if (lean<.2) nDir = DIR_LEFT;
					else if (lean<.4) nDir = DIR_LEFT+7;
					else if (lean<1) nDir = DIR_DOWNLEFT;
					else if (lean<5) nDir = DIR_DOWNLEFT+7;
					else			 nDir = DIR_DOWN;
				}
			}
		} else {
			if (ptEnd.x>ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_RIGHT;	// ->DIR_UPRIGHT
					else if (lean>2.) nDir = DIR_UP;
					else nDir = DIR_UPRIGHT;		// ->DIR_UP
				} else {
					if (lean<.2) nDir = DIR_RIGHT;
					else if (lean<.4) nDir = DIR_RIGHT+7;
					else if (lean<1) nDir = DIR_UPRIGHT;
					else if (lean<5) nDir = DIR_UPRIGHT+7;
					else			 nDir = DIR_UP;
				}
			}
			else if (ptEnd.x < ptStart.x) {
				if (nDirCount == 8) {
					if (lean<.3) nDir = DIR_LEFT;		// -> DIR_DOWNLEFT
					else if (lean>2.) nDir = DIR_UP;
					else nDir = DIR_UPLEFT;				// -> 2(DIR_LEFT)
				}
				else {
					if (lean<.2) nDir = DIR_LEFT;
					else if (lean<.4) nDir = DIR_LEFT+8;
					else if (lean<1) nDir = DIR_UPLEFT;
					else if (lean<5) nDir = DIR_UPLEFT+8;
					else			 nDir = DIR_UP;
				}
			}
		}
	}
*/
//m_tDir =  nDir;
}
////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
void USER::MoveFirstReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int will_x, will_y, x, y;
	int index = 0;
    DWORD checkTick;

	x = will_x = GetShort( pBuf, index );
	y = will_y = GetShort( pBuf, index );
	m_tDir = GetByte( pBuf, index );
    checkTick = GetDWORD(pBuf, index);
	if(CheckMoveSpeedHack(checkTick)) return;
	if(Move_C(will_x, will_y)) result = SUCCESS;
	
	index = 0;
	SetByte(m_TempBuf, MOVE_FIRST_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		SetInt(m_TempBuf, m_uid + USER_BAND, index);
		CPoint t = ConvertToClient(m_curx, m_cury);
		SetShort(m_TempBuf, t.x, index);
		SetShort(m_TempBuf, t.y, index);

		SendInsight(m_TempBuf, index);
		
		//TRACE("Move First Fail : uid = %d, UserName = %s, curx = %d, cury = %d\n", m_uid, m_strUserID, t.x, t.y);
		return;
	}
	

	SetInt(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, will_x, index);
	SetShort(m_TempBuf, will_y, index);

	SendInsight(m_TempBuf, index);

	SightRecalc();

//	UserTimer();		// SP�� ���� TICK���� ä���ش�. 
}

//////////////////////////////////////////////////////////////////////////////////
//	PC �� �����̴� ���� ó��
//
void USER::MoveReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int will_x, will_y, x, y;
	int index = 0;
    DWORD checkTick;

	x = will_x = GetShort( pBuf, index );	// �����̷��� ��
	y = will_y = GetShort( pBuf, index );
	m_tDir = GetByte( pBuf, index );
    checkTick = GetDWORD(pBuf, index);
	if(CheckMoveSpeedHack(checkTick)) return;

	if(Move_C(x, y)) result = SUCCESS;
	
	index = 0;
	SetByte(m_TempBuf, MOVE_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		SetInt(m_TempBuf, m_uid + USER_BAND, index);
		CPoint t = ConvertToClient(m_curx, m_cury);
		SetShort(m_TempBuf, t.x, index);
		SetShort(m_TempBuf, t.y, index);

		SendInsight(m_TempBuf, index);
		
		//TRACE("Move Fail : uid = %d, UserName = %s, curx = %d, cury = %d\n", m_uid, m_strUserID, t.x, t.y);
		return;
	}

	SetInt(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, will_x, index);
	SetShort(m_TempBuf, will_y, index);
	
	SendInsight(m_TempBuf, index);

	SightRecalc();

//	UserTimer();		// SP�� ���� TICK���� ä���ش�. 
}
/////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////
void USER::MoveEndReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int end_x, end_y, x, y;
	int index = 0;
    DWORD checkTick;

	x = end_x = GetShort( pBuf, index );	// �����̷��� ��������
	y = end_y = GetShort( pBuf, index );
	m_tDir = GetByte( pBuf, index );
    checkTick = GetDWORD(pBuf, index);
	if(CheckMoveSpeedHack(checkTick)) return;

	if(Move_C(x, y)) result = SUCCESS;
	
	index = 0;
	SetByte(m_TempBuf, MOVE_END_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		SetInt(m_TempBuf, m_uid + USER_BAND, index);
		CPoint t = ConvertToClient(m_curx, m_cury);
		SetShort(m_TempBuf, t.x, index);
		SetShort(m_TempBuf, t.y, index);

		SendInsight(m_TempBuf, index);
		
		//TRACE("Move End Fail : uid = %d, UserName = %s, curx = %d, cury = %d\n", m_uid, m_strUserID, t.x, t.y);
		return;
	}

	SetInt(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, end_x, index);
	SetShort(m_TempBuf, end_y, index);
	
	SendInsight(m_TempBuf, index);

	SightRecalc();

}

////////////////////////////////////////////////////////////////////////////////
//	Ư�� ��ǥ�� �̵� (��ڸ��)
//
void USER::MoveChatReq(TCHAR *pBuf, BOOL bPsi)
{
	if(bPsi == FALSE && m_tIsOP != 1) return;

	BYTE result = FAIL;
	int will_x, will_y;
	int index = 0;

	will_x = GetShort( pBuf, index );	// �����̷��� ��
	will_y = GetShort( pBuf, index );

	CPoint pt = FindNearAvailablePoint_C(will_x, will_y);
//	if(pt.x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || pt.y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) return;

	CPoint pc;
	if(pt.x != -1 && pt.y != -1) 
	{
		pc = ConvertToServer(pt.x, pt.y);
		if(pc.x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || pc.y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) return;

		if(pc.x < 0 || pc.y < 0) 
		{
			result = FAIL;
		}
		else
		{
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[pc.x][pc.y].m_lUser, USER_BAND + m_uid);
			m_curx = pc.x;
			m_cury = pc.y;

			result = SUCCESS;
		}
	}

	index = 0;
	SetByte(m_TempBuf, MOVE_CHAT_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		// Move Chat ���� �־���� �ϳ�?
//CPoint t = ConvertToClient(m_curx, m_cur_y);
//SetShort(m_TempBuf, t.x, index);
//SetShort(m_TempBuf, t.y, index);

		Send(m_TempBuf, index);
		return;
	}

	SetInt(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, pt.x, index);
	SetShort(m_TempBuf, pt.y, index);
	Send(m_TempBuf, index);

	SendInsight(m_TempBuf, index);

	SightRecalc();
	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"move_chat:x=%d,y=%d", pt.x,pt.y);
	WriteOpratorLog(strOP,CHAT_LOG);
}
//////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
void USER::MopCatchReq(TCHAR *pBuf)
{
	int index = 0;
	TCHAR szNpcName[80];

	::ZeroMemory(szNpcName, sizeof(szNpcName));

	if(!GetVarString(sizeof(szNpcName), szNpcName, pBuf, index)) return;

	CNpc* pNpc = GetNpc(szNpcName);
	if(pNpc == NULL) return;

	BYTE result = FAIL;
	int will_x, will_y;

	will_x = pNpc->m_sCurX;	// �����̷��� ��
	will_y = pNpc->m_sCurY;

	CPoint pt = FindNearAvailablePoint_S(will_x, will_y);
	CPoint ptNew;
	if(pt.x != -1 && pt.y != -1) 
	{
		ptNew = ConvertToClient(pt.x, pt.y);
		if(ptNew.x != -1 && ptNew.y != -1)
		{
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + m_uid);
			m_curx = pt.x;
			m_cury = pt.y;

			result = SUCCESS;
		}
	}

	index = 0;
	SetByte(m_TempBuf, MOVE_CHAT_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		// Move Chat ���� �־���� �ϳ�?
//^^	CPoint t = ConvertToClient(m_curx, m_cur_y);
//		SetShort(m_TempBuf, t.x, index);
//		SetShort(m_TempBuf, t.y, index);
//		TRACE("Move Chat Req Fail - %d : %d %d\n", m_uid, will_x, will_y);
		Send(m_TempBuf, index);
		return;
	}

	SetInt(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, ptNew.x, index);
	SetShort(m_TempBuf, ptNew.y, index);
	Send(m_TempBuf, index);

	SendInsight(m_TempBuf, index);

	SightRecalc();

	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"trace_npc:%s,x=%d,y=%d",szNpcName,ptNew.x,ptNew.y);
	WriteOpratorLog(strOP,CHAT_LOG);
}

///////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////
void USER::UserCallReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int index = 0;
	char szUserName[255];
	USER* pUser = NULL;
	CPoint pt, ptNew;
    CString moonname =_T("") ;

	moonname=m_strUserID;	
	if( m_tIsOP != 1 ) return;									// ��ڰ� �ƴ�

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength == 0 || nLength > CHAR_NAME_LENGTH) return;		// �߸��� �������̵� 
	
	pUser = GetUser(szUserName);
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	
	if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;	// �ڱ��ڽ��� �ȵ�
	
//	pt = FindNearAvailablePoint_S(m_curx, m_cury);
//	if(pt.x <= -1 && pt.y <= -1) return;						// ��� ������ �� ��ǥ�� ����

	if(pUser->m_curz != m_curz)									// ������ ��ڰ� �������� ���� ���� ���
	{
		if( !IsZoneInThisServer(pUser->m_curz) ) return;		// �� �����ȿ� �ִ����� �ƴ� ����

		ptNew = ConvertToClient(m_curx, m_cury);
		BOOL bSuccess = pUser->ZoneChangeProcess(m_curz, ptNew.x, ptNew.y);	//^^ Check ���
		
		if(bSuccess)
		{
//			pUser->SendZoneChange(bSuccess);
			pUser->SendWeatherInMoveZone();				// �̵� ���� ������ȭ�� �˸���.
		}
		return;
	}
	else
	{													// ���� ���̸� �̵�����...		
/*
		ptNew = ConvertToClient(pt.x, pt.y);
		if(ptNew.x == -1 || ptNew.y == -1) return;

		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pUser->m_curx][pUser->m_cury].m_lUser, 0);
		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + pUser->m_uid);
		pUser->m_curx = pt.x;
		pUser->m_cury = pt.y;
		
		result = SUCCESS;
*/
		pt = pUser->FindNearAvailablePoint_S( m_curx, m_cury );
		ptNew = ConvertToClient( pt.x, pt.y );

		if( ptNew.x == -1 || ptNew.y == -1 ) return;

		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pUser->m_curx][pUser->m_cury].m_lUser, 0);
		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + pUser->m_uid);
		pUser->m_curx = pt.x;
		pUser->m_cury = pt.y;
		
		result = SUCCESS;
	}
	
	index = 0;
	SetByte(m_TempBuf, MOVE_CHAT_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		Send(m_TempBuf, index);
		return;
	}

	SetInt(m_TempBuf, pUser->m_uid + USER_BAND, index);
	SetShort(m_TempBuf, ptNew.x, index);
	SetShort(m_TempBuf, ptNew.y, index);
	pUser->Send(m_TempBuf, index);	// �������Դ� ���ο� ��ǥ����...
	
	pUser->SendInsight(m_TempBuf, index);
	
	pUser->SightRecalc();

	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"call_user:%s,z=%d,x=%d,y=%d",pUser->m_strUserID, pUser->m_curz,pUser->m_curx,pUser->m_cury);
	WriteOpratorLog(strOP,CALL_USER_LOG);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void USER::UserRemoteCallReq(TCHAR *pBuf)
{
	CBufferEx TempBuf;
	int will_x = 0, will_y = 0;
	BYTE result = FAIL, error_code = 0;
	int index = 0;
	char szUserName[255];
	USER* pUser = NULL;
	CPoint pt, ptNew;

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) return;
	
	pUser = GetUser(szUserName);
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	
	if( pUser->m_ZoneIndex < 0 || pUser->m_ZoneIndex >= g_zone.GetSize() ) 
	{
		m_curz = 1;
		m_curx = 192;
		m_cury = 803;
		SetZoneIndex(m_curz);
		return;
	}
	if(pUser->m_curx >= g_zone[m_ZoneIndex]->m_sizeMap.cx || pUser->m_curx < 0)
	{
		m_curz = 1;
		m_curx = 192;
		m_cury = 803;
		SetZoneIndex(m_curz);
		return;
	}
	if(pUser->m_cury >= g_zone[m_ZoneIndex]->m_sizeMap.cy || pUser->m_cury < 0) 
	{
		m_curz = 1;
		m_curx = 192;
		m_cury = 803;
		SetZoneIndex(m_curz);
		return;
	}

	if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;		// �ڱ� �ڽ��� �ȵ�

	will_x = GetShort( pBuf, index );
	will_y = GetShort( pBuf, index );

	pt = FindNearAvailablePoint_C(will_x, will_y);	// ���ϵǴ� ��ǥ�� Ŭ���̾�Ʈ ��ǥ��
	if(pt.x == -1 || pt.y == -1) 
	{ 
		// ��ڰ� ��� ���� �ϱ� ���ؼ��� ���⼭ ��ڿ��� ���и޼����� �����°� ������ ����.
		return;
	}
	
//	ptNew = ConvertToClient(pt.x, pt.y);
//	if(ptNew.x == -1 || ptNew.y == -1) return;
	ptNew = pt;
	pt = ConvertToServer( pt.x, pt.y );
	if(pt.x == -1 || pt.y == -1) return;
	
	::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pUser->m_curx][pUser->m_cury].m_lUser, 0);
	::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + pUser->m_uid);
	pUser->m_curx = pt.x;
	pUser->m_cury = pt.y;
	
	result = SUCCESS;

	index = 0;
	SetByte(m_TempBuf, MOVE_CHAT_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		Send(m_TempBuf, index);
		return;
	}

	SetInt(m_TempBuf, pUser->m_uid + USER_BAND, index);
	SetShort(m_TempBuf, ptNew.x, index);
	SetShort(m_TempBuf, ptNew.y, index);
	pUser->Send(m_TempBuf, index);	// �������Դ� ���ο� ��ǥ����...
	
	SendInsight(m_TempBuf, index);
	
	SightRecalc();
	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"Remote_User:%s,x=%d,y=%d", pUser->m_strUserID,ptNew.x,ptNew.y);
	WriteOpratorLog(strOP,CALL_USER_LOG);
}

/*
//////////////////////////////////////////////////////////////////////////////////
//	���� ��ġ�� �߽����� 25 ���� ������ �� �ִ� ��ǥ�� �ִ��� �Ǵ�
//
CPoint USER::FindNearAvailablePoint(int xpos, int ypos)
{
	int i;

	if(IsMovable(xpos, ypos)) return CPoint(xpos, ypos);	// ���� ��ġ

	for(i = 0; i < sizeof(g_8x) / sizeof(g_8x[0]); i++)		// �ֺ� 8����
	{
		if(IsMovable(xpos + g_8x[i], ypos + g_8y[i])) return CPoint(xpos + g_8x[i], ypos + g_8y[i]);
	}

	for(i = 0; i < sizeof(g_16x) / sizeof(g_16x[0]); i++)	// �ֺ� 16����
	{
		if(IsMovable(xpos + g_16x[i], ypos + g_16y[i])) return CPoint(xpos + g_16x[i], ypos + g_16y[i]);
	}

	return CPoint(-1, -1);
}
*/

//////////////////////////////////////////////////////////////////////////////////
//	���� ��ġ�� �߽����� 25 ���� �����۸� ���������ִ� ��ǥ�� �������� ����
//
CPoint USER::FindNearRandomPoint(int x, int y)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return CPoint(-1, -1);

	CPoint t;
	int i;
	int iX, iY;
	int rand_x = 1, rand_y = 1;

	MAP *pMap = g_zone[m_ZoneIndex];
	if( !pMap ) return CPoint(-1, -1);
	if( !pMap->m_pMap ) return CPoint(-1, -1);
	if(x >= pMap->m_sizeMap.cx || x < 0 || y >= pMap->m_sizeMap.cy || y < 0) return CPoint(-1, -1);

	int dir[25][2];

	if(g_zone[m_ZoneIndex]->m_pMap[x][y].m_lUser == 0)
	{
		if( IsMovable_S( x, y ) )
		{
			if(g_zone[m_ZoneIndex]->m_pMap[x][y].iIndex == -1) return CPoint( x, y );
		}
	}

//	X					Y
	dir[0][0]  =  0;		dir[0][1] =  0;		// 
	dir[1][0]  = -1;		dir[1][1] =  0;		// 
	dir[2][0]  = -1;		dir[2][1] =  1;		// 
	dir[3][0]  =  0;		dir[3][1] =  1;		// 
	dir[4][0]  =  1;		dir[4][1] =  1;		// 

	dir[5][0]  =  1;		dir[5][1] =  0;		// 
	dir[6][0]  =  1;		dir[6][1] = -1;		// 
	dir[7][0]  =  0;		dir[7][1] = -1;		// 
	dir[8][0]  = -1;		dir[8][1] = -1;		// 
	dir[9][0]  = -2;		dir[9][1] = -1;		// 

	dir[10][0] = -2;		dir[10][1] =  0;	// 
	dir[11][0] = -2;		dir[11][1] =  1;	// 
	dir[12][0] = -2;		dir[12][1] =  2;	// 
	dir[13][0] = -1;		dir[13][1] =  2;	// 
	dir[14][0] =  0;		dir[14][1] =  2;	// 

	dir[15][0] =  1;		dir[15][1] =  2;	// 
	dir[16][0] =  2;		dir[16][1] =  2;	// 
	dir[17][0] =  2;		dir[17][1] =  1;	// 
	dir[18][0] =  2;		dir[18][1] =  0;	// 
	dir[19][0] =  2;		dir[19][1] = -1;	// 

	dir[20][0] =  2;		dir[20][1] = -2;	// 
	dir[21][0] =  1;		dir[21][1] = -2;	// 
	dir[22][0] =  0;		dir[22][1] = -2;	// 
	dir[23][0] = -1;		dir[23][1] = -2;	// 
	dir[24][0] = -2;		dir[24][1] = -2;	// 

	rand_x = myrand(1, 24, TRUE);
	rand_y = myrand(0, 1, TRUE);

	iX = dir[rand_x][rand_y] + x;
	iY = dir[rand_x][rand_y] + y;

	rand_x = iX; rand_y = iY;

	if( rand_x >= pMap->m_sizeMap.cx || rand_x < 0 || rand_y >= pMap->m_sizeMap.cy || rand_y < 0) return CPoint(-1, -1);

	if(g_zone[m_ZoneIndex]->m_pMap[rand_x][rand_y].m_lUser == 0)
	{
		if( IsMovable_S( rand_x, rand_y ) )
		{
			if(g_zone[m_ZoneIndex]->m_pMap[rand_x][rand_y].iIndex == -1) return CPoint( rand_x, rand_y );
		}
	}

	rand_x = x, rand_y = y;

	for( i = 1; i < 25; i++)
	{
		iX = rand_x + dir[i][0];
		iY = rand_y + dir[i][1];

		if( iX >= pMap->m_sizeMap.cx || iX < 0 || iY >= pMap->m_sizeMap.cy || iY < 0) continue;
		if(g_zone[m_ZoneIndex]->m_pMap[iX][iY].m_lUser != 0) continue;	// �� ������ Ȯ���Ѵ�.

		if( IsMovable_S( iX, iY ) )
		{
			if(g_zone[m_ZoneIndex]->m_pMap[iX][iY].iIndex == -1) return CPoint( iX, iY );
		}
	}

	return CPoint(-1, -1);
}


//////////////////////////////////////////////////////////////////////////////////
//	x, y �� �̵� (x, y) �� Ŭ���̾�Ʈ ��ǥ��
//
BOOL USER::Move_C(int x, int y, int nDist /* = 1 */)
{
	CPoint t = ConvertToServer(x, y);
	if( t.x == -1 || t.y == -1 ) return FALSE;
	
//	if(!GetDistance(t.x, t.y, nDist)) return FALSE;
	// alisia
	if( abs( m_curx - t.x ) > nDist || abs( m_cury - t.y ) > nDist ) return FALSE;

	if(!IsMovable_S(t.x, t.y)) return FALSE;
	if(t.x == m_curx && t.y == m_cury) return FALSE;

		// jjs
	if(g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser == 0 || g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser == m_uid + USER_BAND)
	{
		//::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
		g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser = 0;
	}
	//::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[t.x][t.y].m_lUser, m_uid + USER_BAND);
	g_zone[m_ZoneIndex]->m_pMap[t.x][t.y].m_lUser = m_uid + USER_BAND;

	m_curx = t.x;
	m_cury = t.y;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//	������ǥ�� �������� x, y �� �̵��� �� �ִ� ������ �Ǵ�
//
BOOL USER::IsMovable_S(int x, int y)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;

	if( !g_zone[m_ZoneIndex] ) return FALSE;
	if( !g_zone[m_ZoneIndex]->m_pMap ) return FALSE;
	if( x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || x < 0 || y >= g_zone[m_ZoneIndex]->m_sizeMap.cy || y < 0) return FALSE;

//	CPoint t = ConvertToClient( x, y );		// �����̷��� ������ǥ�� Ŭ���̾�Ʈ���� �������̴� ��ǥ�� ����
//	if( t.x == -1 || t.y == -1 ) return FALSE;

	if(g_zone[m_ZoneIndex]->m_pMap[x][y].m_bMove) return FALSE;

	int temp_uid = g_zone[m_ZoneIndex]->m_pMap[x][y].m_lUser;

	if( temp_uid != 0 && temp_uid != m_uid + USER_BAND)
	{
		if( temp_uid >= NPC_BAND && temp_uid < INVALID_BAND)
		{
			CNpc *pNpc = NULL;
			pNpc = GetNpc( temp_uid - NPC_BAND );

			if( pNpc == NULL )
			{
				g_zone[m_ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}
			if( pNpc->m_sCurX != x || pNpc->m_sCurY != y )
			{
				if(pNpc->m_sDimension > 0) return FALSE;

				g_zone[m_ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}
		}
		else if(temp_uid >= USER_BAND && temp_uid < NPC_BAND)
		{
			if(temp_uid - USER_BAND < 0 || temp_uid - USER_BAND >= MAX_USER)
			{
				g_zone[m_ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}

			USER *pUser = NULL;
			pUser = GetUser( temp_uid - USER_BAND );

			if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED)
			{
				g_zone[m_ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}

			if( pUser->m_curx != x || pUser->m_cury != y || pUser->m_state != STATE_GAMESTARTED )
			{
				g_zone[m_ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}
		}

		return FALSE;
	}

	return TRUE;
}
BOOL USER::IsMovable_S(int zone, int x, int y)
{
	int ZoneIndex = -1;
	for(int j = 0; j < g_zone.GetSize(); j++)
	{
		if( g_zone[j]->m_Zone == zone )
		{
			ZoneIndex = j;
			break;
		}
	}
	if( ZoneIndex < 0 || ZoneIndex >= g_zone.GetSize() ) return FALSE;

	if( !g_zone[ZoneIndex] ) return FALSE;
	if( !g_zone[ZoneIndex]->m_pMap ) return FALSE;
	if( x >= g_zone[ZoneIndex]->m_sizeMap.cx || x < 0 || y >= g_zone[ZoneIndex]->m_sizeMap.cy || y < 0) return FALSE;

//	CPoint t = ConvertToClient( x, y );		// �����̷��� ������ǥ�� Ŭ���̾�Ʈ���� �������̴� ��ǥ�� ����
//	if( t.x == -1 || t.y == -1 ) return FALSE;

	if(g_zone[ZoneIndex]->m_pMap[x][y].m_bMove) return FALSE;

	int temp_uid = g_zone[ZoneIndex]->m_pMap[x][y].m_lUser;

	if( temp_uid != 0 && temp_uid != m_uid + USER_BAND)
	{
		if( temp_uid >= NPC_BAND && temp_uid < INVALID_BAND)
		{
			CNpc *pNpc = NULL;
			pNpc = GetNpc( temp_uid - NPC_BAND );

			if( pNpc == NULL )
			{
				g_zone[ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}
			if( pNpc->m_sCurX != x || pNpc->m_sCurY != y )
			{
				if(pNpc->m_sDimension > 0) return FALSE;

				g_zone[ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}
		}
		else if(temp_uid >= USER_BAND && temp_uid < NPC_BAND)
		{
			if(temp_uid - USER_BAND < 0 || temp_uid - USER_BAND >= MAX_USER)
			{
				g_zone[ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}

			USER *pUser = NULL;
			pUser = GetUser( temp_uid - USER_BAND );

			if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED)
			{
				g_zone[ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}

			if( pUser->m_curx != x || pUser->m_cury != y || pUser->m_state != STATE_GAMESTARTED )
			{
				g_zone[ZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return TRUE;
			}
		}

		return FALSE;
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////
//	���� ������ ��ġ�� (xpos, ypos) ������ �Ÿ��� ���
//
BOOL USER::GetDistance(int xpos, int ypos, int dist, int* ret)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;
	if(xpos >= g_zone[m_ZoneIndex]->m_sizeMap.cx || xpos < 0 || ypos >= g_zone[m_ZoneIndex]->m_sizeMap.cy || ypos < 0) return FALSE;

	int dx = abs(xpos - m_curx);
	int dy = abs(ypos - m_cury);

	if(ret != NULL) *ret = (dx + dy) / 2;

	if((dx + dy) > (dist * 2)) return FALSE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
//	ó�� �ٷ��� �� ���� ó��
//
void USER::RunFirstReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int index = 0;
	int wx_1, wy_1, x1, x2;
	int wx_2, wy_2, y1, y2;
//	int ex, ey;
	DWORD checkTick;

	CPoint ptOld(-1, -1);

	if(GetSP() <= 0) 
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_STAMINA, SYSTEM_NORMAL, TO_ME);
		goto fail_result;
	}

	x1 = wx_1 = GetShort( pBuf, index );
	y1 = wy_1 = GetShort( pBuf, index );
	x2 = wx_2 = GetShort( pBuf, index );
	y2 = wy_2 = GetShort( pBuf, index );
	m_tDir = GetByte( pBuf, index );

//	ex = GetShort( pBuf, index );
//	ey = GetShort( pBuf, index );

	checkTick = GetDWORD(pBuf, index);
	if(CheckRunSpeedHack(checkTick, x1 == x2)) return;

	if(IsMovable_C(x1, y1) == FALSE) goto fail_result;
	if(Move_C(x2, y2, 2) == FALSE)
	{
		if(Move_C(x1, y1, 2) == FALSE) goto fail_result;
		wx_2 = wy_2 = -1;	
	}
	
	DecSP(1);
	index = 0;
	result = SUCCESS;
	SetByte(m_TempBuf, RUN_FIRST_RESULT, index );
	SetByte(m_TempBuf, result, index);
	SetInt	(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, wx_1, index);
	SetShort(m_TempBuf, wy_1, index);
	SetShort(m_TempBuf, wx_2, index);
	SetShort(m_TempBuf, wy_2, index);
//	SetShort(m_TempBuf, ex, index);
//	SetShort(m_TempBuf, ey, index);

	SendInsight(m_TempBuf, index);
	SightRecalc();

	m_dwLastMoveAndRun = GetTickCount();

	SendSP();	//^^ ���׹̳� ������ ���� �����
	return;

fail_result:
	index = 0;

	ptOld = ConvertToClient(m_curx, m_cury);
	if(ptOld.x < 0 || ptOld.y < 0) return;

	SetByte(m_TempBuf, RUN_FIRST_RESULT, index );
	SetByte(m_TempBuf, FAIL, index);
	SetInt(m_TempBuf, m_uid + USER_BAND, index );
	SetShort(m_TempBuf, ptOld.x, index );
	SetShort(m_TempBuf, ptOld.y, index );

	SendInsight(m_TempBuf, index);
}

/////////////////////////////////////////////////////////////////////////////////////
//	�ٰ� �ִ� ���� ó��
//
void USER::RunReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int index = 0;
	int wx_1, wy_1, x1, y1;
	int wx_2, wy_2, x2, y2;
	DWORD checkTick;

	CPoint ptOld;

	if(GetSP() <= 0) 
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_STAMINA, SYSTEM_NORMAL, TO_ME);
		goto fail_result;
	}

	x1 = wx_1 = GetShort( pBuf, index );
	y1 = wy_1 = GetShort( pBuf, index );
	x2 = wx_2 = GetShort( pBuf, index );
	y2 = wy_2 = GetShort( pBuf, index );
	m_tDir = GetByte( pBuf, index );

	checkTick = GetDWORD(pBuf, index);
	if(CheckRunSpeedHack(checkTick, x1 == x2)) return;

	if(IsMovable_C(x1, y1) == FALSE) goto fail_result;
	if(Move_C(x2, y2, 2) == FALSE)
	{
		if(Move_C(x1, y1, 2) == FALSE) goto fail_result;
		wx_2 = wy_2 = -1;
	}

	DecSP(1);

	index = 0;
	result = SUCCESS;
	SetByte(m_TempBuf, RUN_RESULT, index );
	SetByte(m_TempBuf, result, index);

	SetInt	(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, wx_1, index);
	SetShort(m_TempBuf, wy_1, index);
	SetShort(m_TempBuf, wx_2, index);
	SetShort(m_TempBuf, wy_2, index);
	
	SendInsight(m_TempBuf, index);
	SightRecalc();

	m_dwLastMoveAndRun = GetTickCount();
	SendSP();	//^^ ���׹̳� ������ ���� �����

	return;

fail_result:
	index = 0;
	ptOld = ConvertToClient(m_curx, m_cury);
	if(ptOld.x < 0 || ptOld.y < 0) return;

	SetByte(m_TempBuf, RUN_RESULT, index );
	SetByte(m_TempBuf, FAIL, index);
	SetInt(m_TempBuf, m_uid + USER_BAND, index );
	SetShort(m_TempBuf, ptOld.x, index );
	SetShort(m_TempBuf, ptOld.y, index );

	SendInsight(m_TempBuf, index);
}

//////////////////////////////////////////////////////////////////////////////////////
//	�ٴٰ� �������� �������� ���� ó��
//
void USER::RunEndReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int index = 0;
	int wx_1, wy_1, x1, y1;
	int wx_2, wy_2, x2, y2;
	CPoint ptOld;
	DWORD checkTick;

	if(GetSP() <= 0) goto fail_result;

	x1 = wx_1 = GetShort( pBuf, index );
	y1 = wy_1 = GetShort( pBuf, index );
	x2 = wx_2 = GetShort( pBuf, index );
	y2 = wy_2 = GetShort( pBuf, index );
	m_tDir = GetByte( pBuf, index );

	checkTick = GetDWORD(pBuf, index);
	if(CheckRunSpeedHack(checkTick, x1 == x2)) return;

	if(x1 == x2 && y1 == y2)
	{
		if(Move_C(x1, y1, 2) == FALSE) goto fail_result;
		wx_2 = wy_2 = -1;
	}
	else
	{
		if(IsMovable_C(x1, y1) == FALSE) goto fail_result;
		if(Move_C(x2, y2, 2) == FALSE)
		{
			if(Move_C(x1, y1, 2) == FALSE) goto fail_result;
			wx_2 = wy_2 = -1;
		}
	}

	DecSP(1);

	index = 0;
	result = SUCCESS;
	SetByte(m_TempBuf, RUN_RESULT, index );
	SetByte(m_TempBuf, result, index);

	SetInt	(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, wx_1, index);
	SetShort(m_TempBuf, wy_1, index);
	SetShort(m_TempBuf, wx_2, index);
	SetShort(m_TempBuf, wy_2, index);
	
	SendInsight(m_TempBuf, index);
	SightRecalc();

	m_dwLastMoveAndRun = GetTickCount();
	SendSP();	//^^ ���׹̳� ������ ���� �����

	return;

fail_result:
	index = 0;
	ptOld = ConvertToClient(m_curx, m_cury);
	if(ptOld.x < 0 || ptOld.y < 0) return;

	SetByte(m_TempBuf, RUN_END_RESULT, index );
	SetByte(m_TempBuf, FAIL, index);
	SetInt(m_TempBuf, m_uid + USER_BAND, index );
	SetShort(m_TempBuf, ptOld.x, index );
	SetShort(m_TempBuf, ptOld.y, index );

	SendInsight(m_TempBuf, index);
}

///////////////////////////////////////////////////////////////////////////////////
//	���� SP �� ��´�.
//
short USER::GetSP()
{
	BYTE	tWeaponClass = 255;

	int iSkillSid = 0;
	int iLevel = 0, iRandom = 0, iSP = 0;

	double dPlusSP = 1.0;
	int iAddSP = 0;

	DWORD dwDiffTime = GetTickCount() - m_dwLastMoveAndRun;

	if(dwDiffTime >= m_iSPIntervalTime)
	{
		if(IsCanUseWeaponSkill(tWeaponClass))	// ���� �ڽ��� �����տ� �� ������ ��ų�� ������
		{
			int tClass = tWeaponClass * SKILL_NUM; 

			for(int i = tClass; i < tClass + SKILL_NUM; i++)			// ��ų�� SPȸ�� ��ų�� ������ �ݿ��Ѵ�.
			{
				iSkillSid = m_UserSkill[i].sSid;

				if(iSkillSid == SKILL_SP_RECOVER_UP)					// 13 index
				{
					iLevel = m_UserSkill[i].tLevel;							

					// �����ۿ� ���� ��ų ���� ����
					if(g_DynamicSkillInfo[iSkillSid] >= MAGIC_COUNT) continue;
					if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
					//if(iLevel > SKILL_LEVEL) iLevel = SKILL_LEVEL;
					if(iSkillSid >= g_arSkillTable.GetSize()) continue;
					if(iLevel < 0 || iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;

					iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
					if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel)) iSP = 1;

					dPlusSP = 1.0 + (double)iSP * g_arSkillTable[iSkillSid]->m_arRepair.GetAt(iLevel) / 100.0;
				}
			}
		}
							// ������
		if(m_iSPRate <= 0) 
		{
			m_iSPRate = 1;
			TRACE("ȸ������ ����\n");
		}

		if(IsCity() && CheckRecoverTableByClass()) iAddSP = g_arRecoverTable[m_byClass]->m_byTown;
		m_sSP += (int)(dPlusSP * (double)m_sMagicCON/m_iSPRate ) + iAddSP;
		if(m_sSP > m_sMagicMaxSP) m_sSP = m_sMagicMaxSP;

		m_dwLastMoveAndRun = GetTickCount();
	}

	return m_sSP;
}

BOOL USER::CheckRecoverTableByClass()
{
	if(m_byClass < 0 || m_byClass >= g_arRecoverTable.GetSize()) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//	SP �� ���ҽ�Ų��.
//
short USER::DecSP(short sDec)
{
	m_sSP -= sDec;
	if(m_sSP < 0) m_sSP = 0;

	return m_sSP;
}

////////////////////////////////////////////////////////////////////////////////////
//	SP �� Ŭ���̾�Ʈ�� ������.
//
void USER::SendSP(BOOL bLevelDown)
{
	CBufferEx TempBuf;

	if(m_bLive == USER_DEAD && !bLevelDown) return;

	if(m_sSP > m_sMagicMaxSP) m_sSP = m_sMagicMaxSP;

	TempBuf.Add(SET_STAEMINA);
	TempBuf.Add(m_sSP);

	Send(TempBuf, TempBuf.GetLength());
}

//######################################################################
//
//	Character Chatting
//
//######################################################################


////////////////////////////////////////////////////////////////////////
//	ä�� ó��(���� ���̸� ����)
//
void USER::ChatReqWithLength(TCHAR *pBuf, int iLen)
{
	int index = 0;
	BYTE type = NORMAL_CHAT;
    CString moonname =_T("") ;

	type = GetByte(pBuf, index);

	if( pBuf[index+1] == '/' && type != WHISPER_CHAT )
	{
		ExecuteChatCommand( pBuf+index+2, iLen );
		return;
	}

	switch( type )
	{
	case NORMAL_CHAT:		// �Ϲ�ä��
		NormalChat(pBuf + index);
		break;

	case WHISPER_CHAT:		// �ӼӸ�
		WhisperChat(pBuf + index);
		break;

	case SHOUT_CHAT:		// 8 ȭ�� ��ħ
		ShoutChat(pBuf + index);
		break;

	case BUDDY_INVITE: case BUDDY_OFF:	// Buddy ��û, Ż��
		BuddyUserChange(pBuf + index, type);
		break;
	case BUDDY_CHAT:		// Buddy ä��
		BuddyChat(pBuf + index);
		break;

	case ALL_CHAT:						// ��ü ä��
		if(m_tIsOP != 1) break;			// ������ ��ڸ� �Ѵ�.
		SendAllChat(pBuf + index);
		break;

	case WHISPER_OK:
	case WHISPER_DENY:
		WhisperOnOff(pBuf);
		break;

	case PK_COUNT_CHAT:
		SendPKCount();
		break;

	case MOP_CHAT:
		if(m_tIsOP != 1) break;
		MopCatchReq(pBuf + index);
		break;
	case MOP_CALL_CHAT:		// 17 ���� ��ȯ	��� ��ó��... 
		if(m_tIsOP != 1) break;
		MopCallReq(pBuf + index);
		break;

	case CALL_CHAT:			// 15 ������ ��ȯ
		moonname=m_strUserID;	
		if( m_tIsOP != 1 ) break;
		UserCallReq(pBuf + index);
		break;

	case REMOTE_CHAT:		// 16 Ư����ġ�� Ư�� ������ �̵�
		if(m_tIsOP != 1) break;
		UserRemoteCallReq(pBuf + index);
		break;

	case GUILD_CHAT:
		GuildChat(pBuf + index);
		break;
//	case GUILD_INVITE:		// 18 ��� ������ �Ƿ��Ѵ�.	
//		GuildInvite(pBuf + index);
//		GuildInviteWithThread(pBuf + index);
//		break;
	case GUILD_OFF:			// 20 ��� Ż��
		GuildOff(pBuf + index);
//		GuildOffWithThread(pBuf + index);
		break;
	case GUILD_KICK_OUT:	// 21 ���� ���� �߹�
    	GuildKickOut(pBuf + index);
	//	GuildKickOutWithThread(pBuf + index);
		break;
	case GUILD_DISPERSE:
		GuildDisperse(pBuf + index);
//		GuildDisperseWithThread(pBuf + index);
		break;

	case ITEM_MODE_CHAT:	// 22 �����ִ� �������� �³�, �źθ� ����.
		SetItemMode(pBuf + index);
		break;

	case GUILD_FIELD_WAR:
		SendGuildWarFieldApply(pBuf + index);
		break;

	case GUILD_SURRENDER:	// 26 �ʵ����� �׺��Ѵ�.
		{
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_GUILD_DEFEAT, m_strGuildName);
			SendGuildWarFieldEnd((LPTSTR)(LPCTSTR)strMsg);// �׺�
		}
		break;

	case COPY_TELEPORT:
		break;

	case GUILD_USER_CHECK:
		GuildUserAllInforChat();
		break;

	case GUILD_WAR_APPLY_LIST:
		SendFortressAttackGuildList(pBuf + index);
		break;

	case GUILD_FORTRESS_DOOR:
		OpenFortressDoor();
		break;

	case GUILD_DEPUTE_POWER:
		DeputeGuildMasterOfPower(pBuf + index);
		break;

	case GUILD_STOP_POWER:
		RemoveGuildMasterOfPower(pBuf + index);
		break;

	case GUILD_CALL_CHAT: //yskang 0.2 (35) ��Ʈ���� �渶�� ������ȯ
		GuildUserCallReq(pBuf + index);
		break;
	case GUILD_USER_INFO_CALL://yskang 0.2
		m_nGuildUserInfoType =0x03;
		GuildUserInfoReq(pBuf + index);
		break;
	}	
}

////////////////////////////////////////////////////////////////////////
//	ä�� ó��
//
void USER::ChatReq(TCHAR *pBuf)
{
	int index = 0;
    CString moonname =_T("") ;
	BYTE type = NORMAL_CHAT;

	type = GetByte(pBuf, index);

	if( pBuf[index+1] == '/' && type != WHISPER_CHAT )
	{
		ExecuteChatCommand( pBuf+index+2 );
		return;
	}

	switch( type )
	{
	case NORMAL_CHAT:		// �Ϲ�ä��
		NormalChat(pBuf + index);
		break;

	case WHISPER_CHAT:		// �ӼӸ�
		WhisperChat(pBuf + index);
		break;

	case SHOUT_CHAT:		// 8 ȭ�� ��ħ
		ShoutChat(pBuf + index);
		break;

	case BUDDY_INVITE: case BUDDY_OFF:	// Buddy ��û, Ż��
		BuddyUserChange(pBuf + index, type);
		break;
	case BUDDY_CHAT:		// Buddy ä��
		BuddyChat(pBuf + index);
		break;

	case ALL_CHAT:						// ��ü ä��
		if(m_tIsOP != 1) break;			// ������ ��ڸ� �Ѵ�.
		SendAllChat(pBuf + index);
		break;

	case WHISPER_OK:
	case WHISPER_DENY:
		WhisperOnOff(pBuf);
		break;

	case PK_COUNT_CHAT:
		SendPKCount();
		break;

	case MOP_CHAT:
		if(m_tIsOP != 1) break;
		MopCatchReq(pBuf + index);
		break;
	case MOP_CALL_CHAT:		// 17 ���� ��ȯ	��� ��ó��... 
		if(m_tIsOP != 1) break;
		MopCallReq(pBuf + index);
		break;

	case CALL_CHAT:			// 15 ������ ��ȯ
		moonname=m_strUserID;	
		if( m_tIsOP != 1 ) break;
		UserCallReq(pBuf + index);
		break;

	case REMOTE_CHAT:		// 16 Ư����ġ�� Ư�� ������ �̵�
		if(m_tIsOP != 1) break;
		UserRemoteCallReq(pBuf + index);
		break;

	case GUILD_CHAT:
		GuildChat(pBuf + index);
		break;
//	case GUILD_INVITE:		// 18 ��� ������ �Ƿ��Ѵ�.	
//		GuildInvite(pBuf + index);
//		GuildInviteWithThread(pBuf + index);
		break;
	case GUILD_OFF:			// 20 ��� Ż��
		GuildOff(pBuf + index);
//		GuildOffWithThread(pBuf + index);
		break;
	case GUILD_KICK_OUT:	// 21 ���� ���� �߹�
		GuildKickOut(pBuf + index);
//		GuildKickOutWithThread(pBuf + index);
		break;
	case GUILD_DISPERSE:
		GuildDisperse(pBuf + index);
//		GuildDisperseWithThread(pBuf + index);
		break;

	case ITEM_MODE_CHAT:	// 22 �����ִ� �������� �³�, �źθ� ����.
		SetItemMode(pBuf + index);
		break;

	case GUILD_FIELD_WAR:
		SendGuildWarFieldApply(pBuf + index);
		break;

	case GUILD_SURRENDER:	// 26 �ʵ����� �׺��Ѵ�.
		{
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_GUILD_DEFEAT, m_strGuildName);
			SendGuildWarFieldEnd((LPTSTR)(LPCTSTR)strMsg);// �׺�
		}
		break;

	case COPY_TELEPORT:
		break;

	case GUILD_USER_CHECK:
		GuildUserAllInforChat();
		break;

	case GUILD_WAR_APPLY_LIST:
		SendFortressAttackGuildList(pBuf + index);
		break;

	case GUILD_FORTRESS_DOOR:
		OpenFortressDoor();
		break;

	case GUILD_CALL_CHAT: //yskang 0.2 (35) ��Ʈ���� �渶�� ������ȯ
		GuildUserCallReq(pBuf + index);
		break;
	case GUILD_USER_INFO_CALL://yskang 0.2
		m_nGuildUserInfoType =0x03;
		GuildUserInfoReq(pBuf + index);
		break;
	}	
}

void USER::ChatReqWithThread(int length, TCHAR *pBuf)
{
	int head;
	BYTE *pData;
	CHATDATAPACKET *pCDP;

	pCDP = new CHATDATAPACKET;
	if ( pCDP == NULL )	return;

	pData = new BYTE[length+1];
	if ( pData == NULL )
	{
		delete pCDP;
		return;
	}

	memcpy(pData, pBuf, length);
	pCDP->pData = pData;
	pCDP->pData[length] = '\0';
	pCDP->dcount = length;
	pCDP->code = CHAT_REQ;
	pCDP->UID = m_uid;

	if ( m_iModSid < 0 || m_iModSid > AUTOMATA_THREAD )
	{
		delete pCDP;
		return;
	}

	head = g_nChatDataHead[m_iModSid];

	g_WaitRecvDataChat[head][m_iModSid] = pCDP;
	
	head++;
	head %= DATA_BUF_SIZE_FOR_THREAD;
	g_nChatDataHead[m_iModSid] = head;
}

/////////////////////////////////////////////////////////////////////////
//	��û�� PK Count�� ������. 
//
void USER::SendPKCount()
{
	CString strMsg = _T("");
	strMsg.Format( IDS_USER_PK_COUNT, m_sKillCount);
	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);	
}

/////////////////////////////////////////////////////////////////////////
//	��ü �ý��� �޼����� ������.
//
void USER::SendAllChat(TCHAR *pBuf)
{
	CBufferEx TempBuf;

	TempBuf.Add(SYSTEM_MSG);			// ��ڰ� �ý��� �޼����� �˸���.
//	TempBuf.Add(SYSTEM_NORMAL);
	TempBuf.Add(SYSTEM_ANNOUNCE);
	TempBuf.AddString(pBuf + 1);

	USER* pUser = NULL;

	for(int i = 0; i < MAX_USER; i++)
	{
		pUser = GetUser(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		pUser->Send(TempBuf, TempBuf.GetLength());
	}
	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"All_Chat:%s", pBuf);
	WriteOpratorLog(strOP,CHAT_LOG);
}

/////////////////////////////////////////////////////////////////////////
//	���� Ư����ġ�� �̵���Ų��.
//
void USER::MopCallReq(TCHAR *pBuf)
{
	int index = 0;
	char szNpcName[255];

	if(!GetVarString(sizeof(szNpcName), szNpcName, pBuf, index)) return;

	CNpc* pNpc = GetNpc(szNpcName);
	if(pNpc == NULL)
	{
		SendSystemMsg( IDS_USER_INVALID_MONSTER_NAME, SYSTEM_NORMAL, TO_ME);
		return;
	}

	short x = GetShort(pBuf, index);
	short y = GetShort(pBuf, index);

	CPoint ptCell = ConvertToServer( x, y );
	ptCell = FindNearAvailablePoint_S(ptCell.x, ptCell.y);

	if(ptCell.x != -1 && ptCell.y != -1) 
	{
		GetNpcData(pNpc, ptCell.x, ptCell.y);
		//yskang 0.4
		TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
		sprintf(strOP,"call_npc:%s,x=%d,y=%d", szNpcName,ptCell.x, ptCell.y);
		WriteOpratorLog(strOP,CHAT_LOG);
	}
	else SendSystemMsg( IDS_USER_INVALID_POINT_CALL_MONSTER, SYSTEM_NORMAL, TO_ME);
}

/////////////////////////////////////////////////////////////////////////
//	�Ϲ� ä�� ó��
//
void USER::NormalChat(TCHAR *pBuf)
{
	CBufferEx TempBuf;
	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(NORMAL_CHAT);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_strUserID);
	TempBuf.AddString(pBuf + 1);

	SendScreen(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////
//	�ӼӸ� ó��
//
void USER::WhisperChat(TCHAR *pBuf)
{
	CBufferEx TempMyBuf;
	CBufferEx TempYouBuf;
	BYTE result = FAIL, error_code = 0;
	int index = 0;
	char szUserName[255];
	USER* pUser = NULL;

	if(m_sKillCount >= 100) 
	{
		SendSystemMsg( IDS_USER_TOO_LOW_CITY_VALUE, SYSTEM_ERROR, TO_ME);
		return;
	}

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) // �߸��� �������̵�
	{
		error_code = ERR_6;
		goto result_send;
	}

	pUser = GetUser(szUserName);
	if(!pUser) // �Ӹ� ��� ����
	{
//		error_code = ERR_3;
//		goto result_send;
		g_pMainDlg->BridgeServerWhisperChatReq( m_uid, m_strUserID, szUserName, pBuf + 1 + nLength + 1 );
		return;
	}

	if(pUser->m_state != STATE_GAMESTARTED)  // ���� ���� �������� �ƴ�...
	{
		error_code = ERR_3;
		goto result_send;
	}

	if(pUser->m_bWhisper == FALSE) // ������� �Ӹ� �ź���
	{
		error_code = ERR_2;
		goto result_send;
	}

	if(pUser->m_tIsOP == 1)					// ��ڿ��� �Ӹ��� �ȵ�
	{
		error_code = ERR_3;
		goto result_send;
	}

	if(m_strUserID == pUser->m_strUserID)	// �ڱ��ڽſ��� �Ӹ� -> ����
	{
		return;
	}

/*	if(pUser->m_sKillCount >= 100) 
	{
		SendSystemMsg( IDS_USER_TOO_LOW_OTHER_CITY_VALUE, SYSTEM_ERROR, TO_ME);
		return;
	}
*/
	result = SUCCESS;

result_send:
	TempMyBuf.Add(CHAT_RESULT);
	TempMyBuf.Add(result);

	if(result != SUCCESS)
	{
		TempMyBuf.Add(error_code);
		Send(TempMyBuf, TempMyBuf.GetLength());
		return;
	}

	TempMyBuf.Add(WHISPER_CHAT);
	TempMyBuf.Add((int)0x00);								// ���� ���̵� ������ ������.(�ڵ� ��������)
	TempMyBuf.AddString(pUser->m_strUserID);
	TempMyBuf.AddString(pBuf + 1 + nLength + 1);			// pBuf == IDLen(1) + ID + MsgLen(1) + Msg
	Send(TempMyBuf, TempMyBuf.GetLength());
	
	TempYouBuf.Add(CHAT_RESULT);
	TempYouBuf.Add(result);
	TempYouBuf.Add(WHISPER_CHAT);
	TempYouBuf.Add((int)0x01);								// Client���� ���� �����ϱ�����(���� �Ӹ� ���̵�)
	TempYouBuf.AddString(m_strUserID);
	TempYouBuf.AddString(pBuf + 1 + nLength + 1);			// pBuf == IDLen(1) + ID + MsgLen(1) + Msg
	pUser->Send(TempYouBuf, TempYouBuf.GetLength());
	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"Whisper_Chat:%s,%s", szUserName, pBuf + 1 + nLength + 1);
	WriteOpratorLog(strOP,CHAT_LOG);
}

//////////////////////////////////////////////////////////////////////////
//	8ȭ�� ��ħ ó��
//
void USER::ShoutChat(TCHAR *pBuf)
{
	USER* pUser = NULL;
	if(m_sLevel < 100)
	{
		SendSystemMsg( IDS_USER_TOO_LOW_LEVEL_FOR_SHOUT, SYSTEM_NORMAL, TO_ME);
		return;
	}

	short sp = GetSP();

	if(sp < 10)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_STAMINA_FOR_SHOUT, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(m_sKillCount >= 100) 
	{
		SendSystemMsg( IDS_USER_TOO_LOW_CITY_VALUE, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(m_dwDN < 10000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}
	if( m_dwDN <= 10000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 10000;
	UpdateUserItemDN();						
	SendMoneyChanged();
	DecSP(10);			// ���׹̳� ����
	CBufferEx TempBuf;
	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(SHOUT_CHAT);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_strUserID);
	TempBuf.AddString(pBuf + 1);	

	int min_x = m_curx - ( SIGHT_SIZE_X * 2 );
	int max_x = m_curx + ( SIGHT_SIZE_X * 2 );
	int min_y = m_cury - ( SIGHT_SIZE_Y * 2 );
	int max_y = m_cury + ( SIGHT_SIZE_Y * 2 );

	SendSP();
	//SendToRange( TempBuf, TempBuf.GetLength(), min_x, min_y, max_x, max_y );
	for(int i = 0; i < MAX_USER; i++)
	{
		pUser = GetUser(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		pUser->Send(TempBuf, TempBuf.GetLength());
	}
	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"Shout_Chat:%s", pBuf);
	WriteOpratorLog(strOP,CHAT_LOG);
}

//////////////////////////////////////////////////////////////////////////
//	�Ӹ� ���/�ź�
//
void USER::WhisperOnOff(TCHAR *pBuf)
{
	int index = 0;
	BYTE type = GetByte(pBuf, index);

	if(type == WHISPER_OK) m_bWhisper = TRUE;
	else m_bWhisper = FALSE;

	CBufferEx TempBuf;

	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(type);
	
	Send(TempBuf, TempBuf.GetLength());

	g_pMainDlg->BridgeServerUserWhisperOnOff( m_uid, (BYTE)m_bWhisper );
}

///////////////////////////////////////////////////////////////////////////
//	Ư�� ������ ���ӿ� �����ϰ� �ִ��� ã�ƺ���.
//
void USER::FindUserReq(TCHAR *pBuf)
{
	CBufferEx TempBuf;
	BYTE result = 1;
	int index = 0;
	char szUserName[255];
	USER* pUser = NULL;

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) // �߸��� �������̵�
	{
		result = ERR_1;
		goto result_send;
	}

	pUser = GetUser(szUserName);
	if(!pUser) // ��� ����
	{
		result = ERR_2;
		goto result_send;
	}

	result = 0;

result_send:
	TempBuf.Add(FIND_USER_RESULT);
	TempBuf.Add(result);
	TempBuf.AddString(szUserName);

	Send(TempBuf, TempBuf.GetLength());
}


////////////////////////////////////////////////////////////////////////////
//	���� Buddy ���������� ä���� ������.
//
void USER::BuddyChat(TCHAR *pBuf)
{
	int i;
	if(!m_bNowBuddy) 
	{
		SendSystemMsg( IDS_USER_MAKE_BUDDY_FIRST, SYSTEM_NORMAL, TO_ME);
		return;
	}

	USER *pUser = NULL;

	CBufferEx TempBuf;
	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(BUDDY_CHAT);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_strUserID);
	TempBuf.AddString(pBuf + 1);

	for(i = 0; i < MAX_BUDDY_USER_NUM; i++)
	{
		if(m_MyBuddy[i].uid	>= USER_BAND && m_MyBuddy[i].uid <= NPC_BAND) 
		{
			pUser = GetUser(m_MyBuddy[i].uid - USER_BAND);
			if(!pUser) continue;
			if(pUser->m_state != STATE_GAMESTARTED) continue;
			if(strcmp(pUser->m_strUserID, m_MyBuddy[i].m_strUserID) != 0) continue;

			pUser->Send(TempBuf, TempBuf.GetLength());
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//	Buddy Mode �� On/Off �Ѵ�
//
void USER::SetBuddyMode(TCHAR *pBuf)
{
/*	int index = 0;
	BOOL bMode = (BOOL)GetByte(pBuf, index);

	m_bBuddyMode = bMode;

	if(m_bBuddyMode == FALSE)	// �ʱ�ȭ �Ѵ�.
	{
		int iCount = m_MyBuddy.GetSize();
		for(int i = 0; i < m_MyBuddy.GetSize(); i++)
		{
			if(m_MyBuddy[i].Compare(m_strUserID) == 0) { SendBuddyUserLeave(i);	break; }
		}
	}

	CBufferEx TempBuf;

	TempBuf.Add(BUDDY_MODE_RESULT);
	TempBuf.Add((BYTE)m_bBuddyMode);

	Send(TempBuf, TempBuf.GetLength());
*/
}

////////////////////////////////////////////////////////////////////////////////
//	���� �ִ� �����۸� �³�, ������ �����Ѵ�. 
//
void USER::SetItemMode(TCHAR *pBuf)
{
	int index = 0;	

	BYTE tMode = GetByte(pBuf, index);

	if(tMode == 1)			// ������ �ޱ� �ź�
	{
		m_bRefuse = TRUE;
		SendSystemMsg( IDS_USER_ITEM_RECEIVE_DENIED, SYSTEM_NORMAL, TO_ME);
		return;
	}
	else if(tMode == 0)		// ������ �ޱ� �³�
	{
		m_bRefuse = FALSE;
		SendSystemMsg( IDS_USER_ITEM_RECEIVE_PERMITTED, SYSTEM_NORMAL, TO_ME);
		return;
	}
	
}

///////////////////////////////////////////////////////////////////////////
//	����, ���� �������ֱ�, �ŷ� ������Ұ� �Ǵ��� �Ǵ�(���� : �ֺ� 1���̳�, ���� ���ֺ�)
//
BOOL USER::IsThereUser(USER *pUser)
{
	if( !pUser ) return FALSE;

	int nLen = 0;
	nLen = strlen(pUser->m_strUserID);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return FALSE;

	if( abs( m_tDir - pUser->m_tDir ) != 4 ) return FALSE;

	if( abs( m_curx - pUser->m_curx ) > 2 || abs( m_cury - pUser->m_cury ) > 2 ) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//	���� ������Ұ� �߻��Ұ�� ������.
//
void USER::BuddyUserChange(TCHAR *pBuf, BYTE tType)
{
	int index = 0;
	int iCount = 0;
	int i, diffLevel = 0;
	char szUserName[255];

	USER* pUser = NULL;

//	if(m_bBuddyMode == FALSE) return;

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) // �߸��� �������̵�
	{
		return;
	}

	pUser = GetUser(szUserName);
	if(!pUser) // ��� ����
	{
		CString strTemp = _T("");
		strTemp.Format( IDS_USER_NOT_CONNECTED_NOW, szUserName);
		SendSystemMsg((LPTSTR)(LPCTSTR)strTemp, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(tType == BUDDY_OFF)
	{
//		if(!pUser->m_bBuddyMode || !pUser->m_bNowBuddy) return;
		if(!pUser->m_bNowBuddy) return;
		if(!m_bMakeBuddy && strcmp(m_strUserID, pUser->m_strUserID) != 0)
		{
			SendSystemMsg( IDS_USER_HAVE_NOT_PERMISSION, SYSTEM_NORMAL, TO_ME);
			return;
		}

		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)
		{
			if(m_MyBuddy[i].uid == pUser->m_uid + USER_BAND) { SendBuddyUserLeave(i);	break; }
		}
		return;
	}
	
	if(tType == BUDDY_INVITE)
	{
		if(IsThereUser(pUser) == FALSE || strcmp(m_strUserID, pUser->m_strUserID) == 0)
		{
			SendSystemMsg( IDS_USER_INVALID_BUDDY_CONDITION, SYSTEM_NORMAL, TO_ME);
			return;
		}

/*		if(pUser->m_bBuddyMode == FALSE) // ������� ���� �ź���
		{
			CString strTemp = _T("");
			strTemp.Format( IDS_USER_REQUEST_DENIED, pUser->m_strUserID);
			SendSystemMsg(strTemp.GetBuffer(strTemp.GetLength()), SYSTEM_NORMAL, TO_ME);
			return;
		}
*/
		
		diffLevel = abs(m_sLevel - pUser->m_sLevel);
		if( diffLevel > 30)					// �������̰� 20�� �Ѿ�� �ȵ�
		{
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_BUDDY_LEVEL, pUser->m_strUserID);
			SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_NORMAL, TO_ME);
			return;
		}
		
/*		if(m_strUserID == pUser->m_strUserID && m_bNowBuddy == TRUE)	// �ڱ��ڽſ��� ���� -> �������̸� Ż��
		{
			iCount = m_MyBuddy.GetSize();
			for(i = 0; i < m_MyBuddy.GetSize(); i++)
			{
				if(m_MyBuddy[i].Compare(m_strUserID) == 0) { SendBuddyUserLeave(i);	break; }
			}
			return;
		}
*/
		if(pUser->m_bNowBuddy == TRUE) // ������� ������
		{
			CString strTemp = _T("");
			strTemp.Format( IDS_USER_BUDDY_ALREADY, pUser->m_strUserID);
			SendSystemMsg((LPTSTR)(LPCTSTR)strTemp, SYSTEM_NORMAL, TO_ME);
			return;
		}
										// ó������ �߱����� �ȴ�.
		if(!m_bMakeBuddy && !m_bNowBuddy && m_strUserID != pUser->m_strUserID) // �ڱ�ȥ�ڸ� ����°��� �����ϱ�����
		{
			CBufferEx TempBuf;

			for(i = 0; i < MAX_BUDDY_USER_NUM; i++)		// ó�� ���鶧�� �ʱ�ȭ�Ѵ�.
			{
				m_MyBuddy[i].uid = -1;
				::ZeroMemory(m_MyBuddy[i].m_strUserID, sizeof(m_MyBuddy[i].m_strUserID));		
			}

			m_MyBuddy[0].uid = m_uid + USER_BAND; // �ڱ� ���̵� ���� ���� �;� �Ѵ�.
			strcpy(m_MyBuddy[0].m_strUserID, m_strUserID);
			m_bMakeBuddy = TRUE;
			m_bNowBuddy = TRUE;

			TempBuf.Add(BUDDY_USER_CHANGE);
			TempBuf.Add((BYTE)0x01);
			TempBuf.Add((int)(m_uid + USER_BAND));
			TempBuf.AddString(m_strUserID);
			TempBuf.Add(m_iSkin);
			TempBuf.Add(m_iHair);
			TempBuf.Add((BYTE)m_sGender);
			TempBuf.AddData(m_strFace, 10);
			TempBuf.Add(m_sMagicMaxHP);
			TempBuf.Add(m_sHP);

			Send(TempBuf, TempBuf.GetLength());
		}

		if(m_bMakeBuddy == FALSE)	return;				// ¯�� �ƴϸ� ������~~

		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)
		{
			if(m_MyBuddy[i].uid == -1)
			{
				m_MyBuddy[i].uid = pUser->m_uid + USER_BAND;
				strcpy(m_MyBuddy[i].m_strUserID, szUserName);
				SendBuddyUserChange(BUDDY_JOIN, i);
				return;
			}
		}

		SendSystemMsg( IDS_USER_BUDDY_MAX_USER, SYSTEM_NORMAL, TO_ME);
	}
}

///////////////////////////////////////////////////////////////////////////
//	���� ���������� ��ȭ�� ��������� ������.
//
void USER::SendBuddyUserChange(BYTE type, int iCount)
{	
	CBufferEx TempBuf;

	USER *pInviteUser = NULL;
	USER *pUser = NULL;
	int i, j;
	
	if(type == BUDDY_JOIN)									// ¯�� ��õ�Ͽ� ����	
	{
		pInviteUser = GetUser(m_MyBuddy[iCount].uid - USER_BAND);//.GetBuffer(20));

		if(pInviteUser == NULL) return;
		if(pInviteUser->m_state != STATE_GAMESTARTED) return;

		TempBuf.Add(BUDDY_USER_CHANGE);
		TempBuf.Add((BYTE)0x01);							// ����
		TempBuf.Add((int)(pInviteUser->m_uid + USER_BAND));
		TempBuf.AddString(pInviteUser->m_strUserID);
		TempBuf.Add(pInviteUser->m_iSkin);					// ��
		TempBuf.Add(pInviteUser->m_iHair);
		TempBuf.Add((BYTE)pInviteUser->m_sGender);
		TempBuf.AddData(pInviteUser->m_strFace, 10);
		TempBuf.Add(pInviteUser->m_sMagicMaxHP);					// ���� 
		TempBuf.Add(pInviteUser->m_sHP);

		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)				// ��û�� ������ ������ ������ ��������
		{
			if(m_MyBuddy[i].uid == -1) continue;

			pUser = GetUser(m_MyBuddy[i].uid - USER_BAND);	// ����Ʈ���� ������ ã�´�.	
			if(pUser == NULL) continue;
			if(pUser->m_state != STATE_GAMESTARTED) continue;
															// ¯�� �� �߰��� �ʿ䰡 ����.
			if(i != 0 && i != iCount)					
			{
				for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
				{
					if(pUser->m_MyBuddy[j].uid == -1)
					{
						pUser->m_MyBuddy[j].uid = pInviteUser->m_uid + USER_BAND;
						strcpy(pUser->m_MyBuddy[j].m_strUserID, pInviteUser->m_strUserID);
						break;
					}
				}
			}
			if(i != iCount) pUser->Send(TempBuf, TempBuf.GetLength());
		}

		pInviteUser->m_bNowBuddy = TRUE;					// ���� �ߴٴ� ǥ�ø� ����

		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)				// ������ ���� ���� ���� ���� ��θ�
		{
			CBufferEx TempBuf2;
			pUser = GetUser(m_MyBuddy[i].uid - USER_BAND);
			if(pUser == NULL) continue;
			if(pUser->m_state != STATE_GAMESTARTED) continue;

			pInviteUser->m_MyBuddy[i].uid = m_MyBuddy[i].uid;
			strcpy(pInviteUser->m_MyBuddy[i].m_strUserID, m_MyBuddy[i].m_strUserID);

			TempBuf2.Add(BUDDY_USER_CHANGE);
			TempBuf2.Add((BYTE)0x01);
			TempBuf2.Add((int)(pUser->m_uid + USER_BAND));
			TempBuf2.AddString(pUser->m_strUserID);
			TempBuf2.Add(pUser->m_iSkin);
			TempBuf2.Add(pUser->m_iHair);
			TempBuf2.Add((BYTE)pUser->m_sGender);
			TempBuf2.AddData(pUser->m_strFace, 10);
			TempBuf2.Add((short)pUser->m_sMagicMaxHP);
			TempBuf2.Add((short)pUser->m_sHP);
			
			pInviteUser->Send(TempBuf2, TempBuf2.GetLength());
		}
	}
	else if(type == BUDDY_CHANGE)							// ���� ��ġ�� ������ ������
	{
		TempBuf.Add(BUDDY_USER_CHANGE);
		TempBuf.Add((BYTE)0x03);
		TempBuf.Add((int)(m_uid + USER_BAND));
		TempBuf.Add((short)m_sMagicMaxHP);
		TempBuf.Add((short)m_sHP);

		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)
		{
			pUser = GetUser(m_MyBuddy[i].uid - USER_BAND);
			if(pUser == NULL) continue;
			if(pUser->m_state != STATE_GAMESTARTED) continue;
			pUser->Send(TempBuf, TempBuf.GetLength());
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//	���� ������ Ż�� �����϶� �� ���������� ������.
//
void USER::SendBuddyUserLeave(int iCount)//(/@@@ ���� �κ��� ���߿� ����ȭ ����)
{
	int i, j;
	int iNum = 0;
	USER *pUser = NULL;
	USER *pLeaveUser = NULL;

	CBufferEx TempBuf;										// �������߿� ������

	pLeaveUser = GetUser(m_MyBuddy[iCount].uid - USER_BAND);

	if(pLeaveUser == NULL) goto go_result;
															// ¯�� ������...	
	if(m_bMakeBuddy && strcmp(m_strUserID, pLeaveUser->m_strUserID) == 0)// ¯�� ������
	{
		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)				// ����Ʈ�� ��ϵ� ������ ������. 
		{
			CBufferEx TempBuf;
			if(m_MyBuddy[i].uid == -1) continue;
			pUser = GetUser(m_MyBuddy[i].uid - USER_BAND);

			if(pUser == NULL) continue;
			if(pUser->m_state != STATE_GAMESTARTED) continue;

			TempBuf.Add(BUDDY_USER_CHANGE);
			TempBuf.Add((BYTE)0x02);
			TempBuf.Add((int)(pUser->m_uid + USER_BAND));
			pUser->Send(TempBuf, TempBuf.GetLength());

			if(m_uid != pUser->m_uid)						// �ڱ� �ڽſ��Դ� ���� �ʿ䰡 ����.
			{
				pUser->m_bNowBuddy = FALSE;
				for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
				{
					pUser->m_MyBuddy[j].uid = -1;			// ģ���� ������ �ʱ�ȭ �Ѵ�.
					::ZeroMemory(pUser->m_MyBuddy[j].m_strUserID, sizeof(pUser->m_MyBuddy[j].m_strUserID));
				}
			}
		}
	}
	else if(m_bMakeBuddy)									// ������ ���
	{		
		TempBuf.Add(BUDDY_USER_CHANGE);
		TempBuf.Add((BYTE)0x02);
		TempBuf.Add((int)(pLeaveUser->m_uid + USER_BAND));

		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)				// �����Ѵ�.		
		{
			if(m_MyBuddy[i].uid == -1) continue;
			pUser = GetUser(m_MyBuddy[i].uid - USER_BAND);
			if(pUser == NULL) continue;
			if(pUser->m_state != STATE_GAMESTARTED) continue;
			pUser->Send(TempBuf, TempBuf.GetLength());

			for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
			{
				if(pUser->m_MyBuddy[j].uid == pLeaveUser->m_uid + USER_BAND) 
				{
					pUser->m_MyBuddy[j].uid = -1;	
					::ZeroMemory(pUser->m_MyBuddy[j].m_strUserID, sizeof(pUser->m_MyBuddy[j].m_strUserID));
				}
			}
		}

		iNum = 0;
		pLeaveUser->m_bNowBuddy = FALSE;
		pLeaveUser->m_bMakeBuddy = FALSE;
		for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
		{
			if(m_MyBuddy[j].uid != -1) iNum++;
			pLeaveUser->m_MyBuddy[j].uid = -1;		// ¯�� ���߿� ����...	
			::ZeroMemory(pLeaveUser->m_MyBuddy[j].m_strUserID, sizeof(pLeaveUser->m_MyBuddy[j].m_strUserID));
		}

		if(iNum <= 1)						// ¯ ȥ���϶��� �ػ� 
		{
			CBufferEx TempBuf2;
			TempBuf2.Add(BUDDY_USER_CHANGE);
			TempBuf2.Add((BYTE)0x02);
			TempBuf2.Add((int)(m_uid + USER_BAND));
			Send(TempBuf2, TempBuf2.GetLength());
			m_bNowBuddy = FALSE;
			m_bMakeBuddy = FALSE;
			for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
			{
				m_MyBuddy[j].uid = -1;		// ¯�� ���߿� ����...	
				::ZeroMemory(m_MyBuddy[j].m_strUserID, sizeof(m_MyBuddy[j].m_strUserID));
			}
		}
		return;	
	}
	else
	{
		TempBuf.Add(BUDDY_USER_CHANGE);
		TempBuf.Add((BYTE)0x02);
		TempBuf.Add((int)(pLeaveUser->m_uid + USER_BAND));

		for(i = 0; i < MAX_BUDDY_USER_NUM; i++)				
		{
			if(m_MyBuddy[i].uid == -1) continue;
			pUser = GetUser(m_MyBuddy[i].uid - USER_BAND);
			if(pUser == NULL) continue;
			if(pUser->m_state != STATE_GAMESTARTED) continue;

			pUser->Send(TempBuf, TempBuf.GetLength());

			if(pUser->m_uid != pLeaveUser->m_uid)			// �����µ� �ڱ⿡�� ���� �ʿ� ����.
			{				
				for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
				{
					if(pUser->m_MyBuddy[j].uid == pLeaveUser->m_uid + USER_BAND) 
					{
						pUser->m_MyBuddy[j].uid = -1;	
						::ZeroMemory(pUser->m_MyBuddy[j].m_strUserID, sizeof(pUser->m_MyBuddy[j].m_strUserID));
						break;
					}
				}
			}
		}

		pUser = GetUser(m_MyBuddy[0].uid - USER_BAND);
		if(pUser != NULL)
		{
			iNum = 0;
			for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
			{
				if(pUser->m_MyBuddy[j].uid != -1) iNum++;
			}

			if(iNum <= 1)						// ¯ ȥ���϶��� �ػ� 
			{
				CBufferEx TempBuf2;
				TempBuf2.Add(BUDDY_USER_CHANGE);
				TempBuf2.Add((BYTE)0x02);
				TempBuf2.Add((int)(pUser->m_uid + USER_BAND));
				pUser->Send(TempBuf2, TempBuf2.GetLength());
				pUser->m_bNowBuddy = FALSE;
				pUser->m_bMakeBuddy = FALSE;
				for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
				{
					m_MyBuddy[j].uid = -1;		// ¯�� ���߿� ����...	
					::ZeroMemory(m_MyBuddy[j].m_strUserID, sizeof(m_MyBuddy[j].m_strUserID));
				}
			}
		}
	}

go_result:
	m_bMakeBuddy = FALSE;	
	m_bNowBuddy = FALSE;	
	for(j = 0; j < MAX_BUDDY_USER_NUM; j++)
	{
		m_MyBuddy[j].uid = -1;		// ¯�� ���߿� ����...	
		::ZeroMemory(m_MyBuddy[j].m_strUserID, sizeof(m_MyBuddy[j].m_strUserID));
	}
}

///////////////////////////////////////////////////////////////////////////
//	BBS ó����ƾ���� ������ �� ��� ��������
//
void USER::SendBBSError(BYTE tError)
{
	CBufferEx TempBuf;

	TempBuf.Add(BBS_ERROR);
	TempBuf.Add(tError);

	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////
//	�ش� BBS�� Open �Ѵ�.
//
void USER::BBSOpen(int bbsnum)
{
	CBufferEx TempBuf;
	int index = 0;
	int rowcount = 0;
	int i = 0;

	BOOL bQuerySuccess = FALSE;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_READ_INDEX ( %d, -1 )}" ), bbsnum);

	SQLCHAR		strWriter[CHAR_NAME_LENGTH+1];		::ZeroMemory(strWriter, sizeof(strWriter));
	SQLCHAR		strTitle[50];		::ZeroMemory(strTitle, sizeof(strTitle));
	SQLCHAR		strTime[30];		::ZeroMemory(strTime, sizeof(strTime));

	SQLINTEGER	iIndex, iView;
	SQLINTEGER	iInd;

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To BBS Open(BBS:%d) !!\n", bbsnum);
//		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while(1)
		{
			retcode = SQLFetch( hstmt );

			if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				bQuerySuccess = TRUE;

				i = 1;
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &iInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strWriter,	sizeof(strWriter),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strTime,	sizeof(strTime),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iView,		sizeof(SQLINTEGER),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strTitle,	sizeof(strTitle),	&iInd );

				TempBuf.Add((int)(iIndex));
				TempBuf.AddString((LPTSTR)strWriter);
				TempBuf.AddString((LPTSTR)strTime);
				TempBuf.Add((int)iView);
				TempBuf.AddString((LPTSTR)strTitle);
				
				::ZeroMemory(strWriter, sizeof(strWriter));
				::ZeroMemory(strTitle, sizeof(strTitle));
				::ZeroMemory(strTime, sizeof(strTime));

				rowcount++;
			}
			else if(retcode == SQL_NO_DATA)
			{
				bQuerySuccess = TRUE;
				break;
			}
			else break;
		}
	}
	else if (retcode == SQL_ERROR)
	{
		bQuerySuccess = FALSE;
		DisplayErrorMsg(hstmt);
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}
	
	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess) return;

	index = 0;
	SetByte(m_TempBuf, BBS_OPEN, index);
	SetShort(m_TempBuf, bbsnum, index );
	SetShort(m_TempBuf, rowcount, index );
	SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), index);
	Send(m_TempBuf, index);
}

//////////////////////////////////////////////////////////////////////////////
//	�ش� BBS���� ���� �Խù� ������ ��´�.
//
void USER::BBSNext(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	CBufferEx TempBuf;
	int index = 0;
	int i = 1;

	int	nBBS = GetShort(pBuf, index);
	int	nStart = GetInt(pBuf, index);
	
	int rowcount = 0;
	BOOL bQuerySuccess = FALSE;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_READ_INDEX ( %d, %d )}" ), nBBS, nStart);

	SQLCHAR		strWriter[CHAR_NAME_LENGTH+1];		::ZeroMemory(strWriter, sizeof(strWriter));
	SQLCHAR		strTitle[50];		::ZeroMemory(strTitle, sizeof(strTitle));
	SQLCHAR		strTime[30];		::ZeroMemory(strTime, sizeof(strTime));

	SQLINTEGER	iIndex, iView;
	SQLINTEGER	iInd;

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To BBS Next(BBS = %d, Start = %d) !!\n", nBBS, nStart);

//		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while(1)
		{
			retcode = SQLFetch( hstmt );

			if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
			{
				bQuerySuccess = TRUE;

				i = 1;
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &iInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strWriter,	sizeof(strWriter),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strTime,	sizeof(strTime),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iView,		sizeof(SQLINTEGER),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strTitle,	sizeof(strTitle),	&iInd );

				TempBuf.Add((int)(iIndex));
				TempBuf.AddString((LPTSTR)strWriter);
				TempBuf.AddString((LPTSTR)strTime);
				TempBuf.Add((int)iView);
				TempBuf.AddString((LPTSTR)strTitle);
				
				::ZeroMemory(strWriter, sizeof(strWriter));
				::ZeroMemory(strTitle, sizeof(strTitle));
				::ZeroMemory(strTime, sizeof(strTime));

				rowcount++;
			}
			else if(retcode == SQL_NO_DATA)
			{
				bQuerySuccess = TRUE;
				break;
			}
			else
				break;
		}
	}
	else if (retcode == SQL_ERROR)
	{
		bQuerySuccess = FALSE;
		DisplayErrorMsg(hstmt);
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);	

	if( !bQuerySuccess) return;

	index = 0;
	SetByte(m_TempBuf, BBS_NEXT_RESULT, index);
	SetShort(m_TempBuf, nBBS, index );
	SetShort(m_TempBuf, rowcount, index );
	SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), index);
	Send(m_TempBuf, index);
}

void USER::BBSNextWithThread(TCHAR *pBuf)
{
	int index = 0;

	int	nBBS = GetShort(pBuf, index);
	int	nStart = GetInt(pBuf, index);

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = BBS_NEXT;
	Datalength = index;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, pBuf, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );	
}


//////////////////////////////////////////////////////////////////////////////
//	�Խù��� ������ �д´�.
//
void USER::BBSRead(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	CBufferEx TempBuf;
	int index = 0;
	int i = 1;

	int bbsnum = GetShort( pBuf, index );
	int readindex = GetInt( pBuf, index );

	BOOL bQuerySuccess = FALSE;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[1024];		memset(szSQL, 0x00, sizeof(szSQL));
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_READ_CONTENT ( %d, %d )}" ), bbsnum, readindex );

	SQLCHAR		strWriter[CHAR_NAME_LENGTH+1];		memset( strWriter,	NULL, sizeof(strWriter));
	SQLCHAR		strTitle[51];		memset( strTitle,	NULL, sizeof(strTitle));
	SQLCHAR		strTime[31];		memset( strTime,	NULL, sizeof(strTime));
	SQLCHAR		strContent[5001];	memset( strContent, NULL, sizeof(strContent));

	SQLINTEGER	iIndex;
	SQLINTEGER	iInd;

//	SQLSMALLINT	sRet = -1;
//	SQLINTEGER	iRetInd;

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Read BBS (BBS:%d,Index:%d) !!\n", bbsnum, readindex);
		return;
	}

	/*
	retcode = SQLBindParameter(hstmt, 1 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &iRetInd);
	if (retcode!=SQL_SUCCESS) 
	{
		TRACE("USER::BBSRead(), Line = %d, File = %s\n", __LINE__, __FILE__);
		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}
	*/

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while(1)
		{
			retcode = SQLFetch( hstmt );

			if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
			{
				bQuerySuccess = TRUE;

				i = 1;
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &iInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strWriter,	sizeof(strWriter),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strTitle,	sizeof(strTitle),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strTime,	sizeof(strTime),	&iInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strContent,	sizeof(strContent),	&iInd );

				TempBuf.Add((int)(iIndex));
				TempBuf.AddString((LPTSTR)strWriter);
				TempBuf.AddString((LPTSTR)strTitle);
				TempBuf.AddString((LPTSTR)strTime);
				TempBuf.AddLongString((LPTSTR)strContent, _tcslen((LPCTSTR)strContent));

				memset( strWriter,	NULL, sizeof(strWriter));
				memset( strTitle,	NULL, sizeof(strTitle));
				memset( strTime,	NULL, sizeof(strTime));
				memset( strContent, NULL, sizeof(strContent));

				break;
			}
			else if(retcode == SQL_NO_DATA)
			{
				bQuerySuccess = TRUE;
				break;
			}
			else break;
		}
	}
	else
	{				
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess )
	{
		if(g_bDebug) SendSystemMsg( IDS_USER_BBS_DELETED_ALREADY, SYSTEM_NORMAL, TO_ME);
		SendBBSError(BBS_CONTENTS_DELETE);
		return;
	}
	
	if( !bQuerySuccess ) return;

	index = 0;
	SetByte(m_TempBuf, BBS_READ_RESULT, index);
	SetShort(m_TempBuf, bbsnum, index );
	SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), index);
	Send(m_TempBuf, index);
}

void USER::BBSReadWithThread(TCHAR *pBuf)
{
	int index = 0;

	int bbsnum = GetShort( pBuf, index );
	int readindex = GetInt( pBuf, index );

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = BBS_READ;
	Datalength = index;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, pBuf, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );

}
//////////////////////////////////////////////////////////////////////////////
//	���ο� �Խù��� �ۼ��Ѵ�.
//
void USER::BBSWrite(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int index = 0;
	int i = 1;
//	CString strText = _T("");
	char strTitle[51];		::ZeroMemory(strTitle,		sizeof(strTitle));
	char strContent[5001];	::ZeroMemory(strContent,	sizeof(strContent));

	int bbsnum = GetShort(pBuf, index);

	if(m_tIsOP != 1 && ( bbsnum == 1 || bbsnum == 3 ) )		// �����Խ��� or �ο����� �Խ���
	{
		SendSystemMsg(IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(!GetVarString(sizeof(strTitle), strTitle, pBuf, index)) return;			// ������ 50����Ʈ �̳���...	
	if(!GetVarLongString(sizeof(strContent), strContent, pBuf, index)) return;	// ������ 5000����Ʈ �̳���...
	
	SDWORD sTitleLen	= _tcslen(strTitle);
	SDWORD sContentLen	= _tcslen(strContent);
	SDWORD sIDLen		= _tcslen(m_strUserID);

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

//	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_WRITE ( %d, %s, ?, ? )}" ), bbsnum, m_strUserID);
	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_WRITE ( %d, ?, ?, ? )}" ), bbsnum );

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To Write BBS (BBS:%d,Writer:%s,Title:%d) !!\n", bbsnum, m_strUserID, strTitle);

//		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	i = 1;
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20,		0, (TCHAR*)m_strUserID,	0, &sIDLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 50,		0, (TCHAR*)strTitle,	0, &sTitleLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 5000,	0, (TCHAR*)strContent,	0, &sContentLen );

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
	}
	else if (retcode == SQL_ERROR)
	{
		DisplayErrorMsg(hstmt);
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}
	
	SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	BBSOpen( bbsnum );
}

void USER::BBSWriteWithThread(TCHAR *pBuf)
{
	int index = 0;
	char strTitle[51];		::ZeroMemory(strTitle,		sizeof(strTitle));
	char strContent[5001];	::ZeroMemory(strContent,	sizeof(strContent));

	int bbsnum = GetShort(pBuf, index);

	if(m_tIsOP != 1 && bbsnum == 1)		// �����Խ���
	{
		SendSystemMsg(IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(!GetVarString(sizeof(strTitle), strTitle, pBuf, index)) return;			// ������ 50����Ʈ �̳���...	
	if(!GetVarLongString(sizeof(strContent), strContent, pBuf, index)) return;	// ������ 5000����Ʈ �̳���...

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = BBS_WRITE;
	Datalength = index;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, pBuf, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );
}

//////////////////////////////////////////////////////////////////////////////
//	�Խù��� �����Ѵ�.
//
void USER::BBSEdit(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int index = 0;
	int i = 1;

	char strTitle[51];		::ZeroMemory(strTitle,		sizeof(strTitle));
	char strContent[5001];	::ZeroMemory(strContent,	sizeof(strContent));

	int bbsnum = GetShort(pBuf, index);
	int editindex = GetInt( pBuf, index );

	if(m_tIsOP != 1 && ( bbsnum == 1 || bbsnum == 3 ) )		// �����Խ��� or �ο����� �Խ���
	{
		SendSystemMsg(IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(!GetVarString(sizeof(strTitle), strTitle, pBuf, index)) return;
	if(!GetVarLongString(sizeof(strContent), strContent, pBuf, index)) return;

	SDWORD sTitleLen	= _tcslen(strTitle);
	SDWORD sContentLen	= _tcslen(strContent);
	SDWORD sIDLen		= _tcslen(m_strUserID);

	SQLSMALLINT	sRet = -1;
	SQLINTEGER	iRetInd = SQL_NTS;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_EDIT ( %d, %d, ?, ?, ?, ?)}" ), bbsnum, editindex );

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Edit BBS (BBS:%d,Writer:%s,Index:%d) !!\n", bbsnum, m_strUserID, editindex);

		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	i = 1;
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20,		0, (TCHAR*)m_strUserID,	0, &sIDLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 50,		0, (TCHAR*)strTitle,	0, &sTitleLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 5000,	0, (TCHAR*)strContent,	0, &sContentLen );
	SQLBindParameter( hstmt, i++ ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT,	0,		0, &sRet,				0, &iRetInd);

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	if(sRet == 2)
	{
		if(g_bDebug) SendSystemMsg( IDS_USER_BBS_NO_PERMISSION, SYSTEM_NORMAL, TO_ME);
		SendBBSError(BBS_CONTENTS_DELETE);
		return;
	}
	if(sRet == 3)
	{
		if(g_bDebug) SendSystemMsg( IDS_USER_BBS_DELETED_ALREADY, SYSTEM_NORMAL, TO_ME);
		SendBBSError(BBS_CONTENTS_DELETE);
		return;
	}

	BBSOpen( bbsnum );
}

void USER::BBSEditWithThread(TCHAR *pBuf)
{
	int index = 0;

	char strTitle[51];		::ZeroMemory(strTitle,		sizeof(strTitle));
	char strContent[5001];	::ZeroMemory(strContent,	sizeof(strContent));

	int bbsnum = GetShort(pBuf, index);
	int editindex = GetInt( pBuf, index );

	if(m_tIsOP != 1 && ( bbsnum == 1 || bbsnum == 3 ) )		// �����Խ��� or �ο����� �Խ���
	{
		SendSystemMsg( IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(!GetVarString(sizeof(strTitle), strTitle, pBuf, index)) return;
	if(!GetVarLongString(sizeof(strContent), strContent, pBuf, index)) return;

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = BBS_EDIT;
	Datalength = index;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, pBuf, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );
}

/////////////////////////////////////////////////////////
// MAIL request process fucntion
// zinee 02-11-18

void USER::MailInfoReq( TCHAR* pBuf )
{
	if ( pBuf == NULL ) return;

	TCHAR	TempBuf[10000];
	int		nTempBufPos;
	int		nBufferPos;
	BYTE	error;

	nTempBufPos = 0;
	error = MailGetInfo( TempBuf, nTempBufPos );
//	if( error == 0 ) error = MailGetList( TempBuf, -1, 20, nTempBufPos );

	nBufferPos = 0;
	SetByte(m_TempBuf, MAIL_INFO_RESULT, nBufferPos);
//	SetByte(m_TempBuf, error, nBufferPos );
	SetString(m_TempBuf, TempBuf, nTempBufPos, nBufferPos );
	Send(m_TempBuf, nBufferPos );
}


void USER::MailSendReq( TCHAR* pBuf )
{
	if ( pBuf == NULL ) return;

//	CDatabase*	pDB;
	SQLHSTMT	hstmt = NULL;
	TCHAR		szSQL[1024];	
//	SQLRETURN	retcode;

	USER*	pRecvUser;

	short tSlot;

	int nBufferPos;
//	int	nDBIndex;

	BYTE error;

	SYSTEMTIME	st;
	ItemList	StampItem;

	QUEUE		Queue;
	CBufferEx	TempBuf, QTempBuf;

	SQLCHAR		strUserID[CHAR_NAME_LENGTH+1];		
	SQLCHAR		strSendID[CHAR_NAME_LENGTH+1];
	SQLCHAR		strTime[15];
	BOOL		bRead;
	char		strMessage[1001];

	SQLINTEGER		nUserID = CHAR_NAME_LENGTH+1;		
	SQLINTEGER		nSendID = CHAR_NAME_LENGTH+1;
	SQLINTEGER		nTime = 15;
	SQLINTEGER		nRead = sizeof(bRead);
	SQLINTEGER		nMessage = 1001;

//	SQLSMALLINT		SQLType;
//	SQLSMALLINT		SQL_C_Type;
//	SQLUINTEGER		SQLColLength;
//	SQLINTEGER		SQLDataLength;

	::ZeroMemory(strUserID, sizeof(strUserID));
	::ZeroMemory(strSendID, sizeof(strSendID));
	::ZeroMemory(strTime, sizeof(strTime));
	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strMessage, sizeof(strMessage));
	::ZeroMemory(&Queue, sizeof(QUEUE) );

	nBufferPos	= 1;
	// Get (strUserID, strMessage) from buffer.
	if(!GetVarString(sizeof(strUserID), (char*)strUserID, pBuf, nBufferPos)) return;
	if(!GetVarLongString(sizeof(strMessage), strMessage, pBuf, nBufferPos)) return;

	// �޴� ��� ���̵� �������� �������
	if( IsExistCharId( (char*)strUserID ) == FALSE )
	{
		error = 2;
		nBufferPos = 0;
		SetByte(m_TempBuf, MAIL_SEND_RESULT, nBufferPos);
		SetByte(m_TempBuf, error, nBufferPos );
		Send(m_TempBuf, nBufferPos);
		return;
	}

	::ZeroMemory( &StampItem, sizeof(ItemList) );
	StampItem.sSid = NORMAL_ITEM_STAMP;
	tSlot = (short)GetSameItem(StampItem, INVENTORY_SLOT);

	//���� �κ��� �������� ���ų� �������� 0�� ���
	if( tSlot == -1 )

	{
		error = 1;
		nBufferPos = 0;
		SetByte(m_TempBuf, MAIL_SEND_RESULT, nBufferPos);
		SetByte(m_TempBuf, error, nBufferPos );
		Send(m_TempBuf, nBufferPos);
		return;
	}
	else if( m_UserItem[tSlot].sCount <= 0 )
	{
		error = 1;
		nBufferPos = 0;
		SetByte(m_TempBuf, MAIL_SEND_RESULT, nBufferPos);
		SetByte(m_TempBuf, error, nBufferPos );
		Send(m_TempBuf, nBufferPos);
		return;
	}

	///////////////////////////////////////////////////////////////////////////////

	// strSendID, strTime, bRead
	strcpy( (char*)strSendID, m_strUserID );
	GetLocalTime(&st);
	_sntprintf((char*)strTime, sizeof(strTime), TEXT( "%02d/%02d/%02d %02d:%02d" ), 
				st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute );
	bRead = FALSE;

	QTempBuf.Add( (int)0 );
	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call MAIL_INSERT ( \'%s\',\'%s\',\'%s\',%d,\'%s\' )}" ),strUserID, strSendID, strTime, bRead, strMessage);
	QTempBuf.AddData( szSQL, strlen( szSQL )+1 );
/*	SQLType = SQL_C_CHAR;
	QTempBuf.Add( SQLType );
	SQL_C_Type = SQL_VARCHAR;
	QTempBuf.Add( SQL_C_Type );
	SQLColLength = 31;
	QTempBuf.Add( SQLColLength );
	SQLDataLength = strlen((TCHAR*)strUserID)+1;
	QTempBuf.Add( SQLDataLength );
	QTempBuf.AddData( (TCHAR*)strUserID, SQLDataLength );

	SQLType = SQL_C_CHAR;
	QTempBuf.Add( SQLType );
	SQL_C_Type = SQL_VARCHAR;
	QTempBuf.Add( SQL_C_Type );
	SQLColLength = 31;
	QTempBuf.Add( SQLColLength );
	SQLDataLength = strlen((TCHAR*)strSendID)+1;
	QTempBuf.Add( SQLDataLength );
	QTempBuf.AddData( (TCHAR*)strSendID, SQLDataLength );

	SQLType = SQL_C_CHAR;
	QTempBuf.Add( SQLType );
	SQL_C_Type = SQL_CHAR;
	QTempBuf.Add( SQL_C_Type );
	SQLColLength = 15;
	QTempBuf.Add( SQLColLength );
	SQLDataLength = strlen((TCHAR*)strTime)+1;
	QTempBuf.Add( SQLDataLength );
	QTempBuf.AddData( (TCHAR*)strTime, SQLDataLength );

	SQLType = SQL_C_BINARY;
	QTempBuf.Add( SQLType );
	SQL_C_Type = SQL_VARBINARY;
	QTempBuf.Add( SQL_C_Type );
	SQLColLength = 1001;
	QTempBuf.Add( SQLColLength );
	SQLDataLength = strlen((TCHAR*)strMessage)+1;
	QTempBuf.Add( SQLDataLength );
	QTempBuf.AddData( (TCHAR*)strMessage, SQLDataLength );*/

	if( g_MailAgentQueue.PutQueue( (QUEUE*)((LPTSTR)QTempBuf) ) == TRUE )
		error = 0;
	else error = 3;  // ���Ͻý��� �������� ���
	//////////////////////////////////////////////////////////////////////////////


	/*
	nDBIndex	= 0;
	pDB = g_DB[m_iModSid].GetDB( nDBIndex );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return;
	}


	// strSendID, strTime, bRead
	strcpy( (char*)strSendID, m_strUserID );
	GetLocalTime(&st);
	_sntprintf((char*)strTime, sizeof(strTime), TEXT( "%02d/%02d/%02d %02d:%02d" ), 
				st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute );
	bRead = FALSE;

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call MAIL_INSERT ( ?,?,?,%d,? )}" ), bRead);

	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,	CHAR_NAME_LENGTH+1,	0, (TCHAR*)strUserID,	0, &nUserID );
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,	CHAR_NAME_LENGTH+1,	0, (TCHAR*)strSendID,	0, &nSendID );
	retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR,	15,					0, (TCHAR*)strTime,		0, &nTime );
	retcode = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 1001,			0, (TCHAR*)strMessage,	0, &nMessage );

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO ) error = 0;
	else error = 2; //�������� �ʴ� ���̵�

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(nDBIndex);	
	*/

	nBufferPos = 0;
	SetByte(m_TempBuf, MAIL_SEND_RESULT, nBufferPos);
	SetByte(m_TempBuf, error, nBufferPos );
	if( error == 0 )
	{
		// ��ǥ�� ���Ը� �ϳ� ���ҽ�Ų��.
		m_UserItem[tSlot].sCount -= 1;
		if(m_UserItem[tSlot].sCount <= 0) m_UserItem[tSlot].sSid = -1;

		// ���� ���Կ��� ��ǥ �Ѱ� ���Ը� ���ҽ�Ų��.
		m_iCurWeight -= g_arItemTable[NORMAL_ITEM_STAMP]->m_byWeight;
		if( m_iCurWeight < 0 ) m_iCurWeight = 0;

		SetShort( m_TempBuf, tSlot, nBufferPos );
		SetShort( m_TempBuf, m_UserItem[tSlot].sCount, nBufferPos );;
	}

	Send(m_TempBuf, nBufferPos);

	// ���� ������ �������̸� �������� �����ߴٰ� �˷���
	if( error == 0 )
	{
		pRecvUser = GetUser((TCHAR*)strUserID);
		if( pRecvUser != NULL )
		{
			if(pRecvUser->m_state == STATE_GAMESTARTED 
				&& pRecvUser->m_tIsOP == 0)
			{
				nBufferPos = 0;
				SetByte(m_TempBuf, MAIL_INFO_RESULT, nBufferPos);
				pRecvUser->MailGetInfo( m_TempBuf, nBufferPos );
				pRecvUser->Send(m_TempBuf, nBufferPos );
			}
		}
	}
}

void USER::MailListReq( TCHAR* pBuf )
{
	if ( pBuf == NULL ) return;
/*
	CDatabase*	pDB;
	SQLHSTMT	hstmt = NULL;
	TCHAR		szSQL[1024];	
	SQLRETURN	retcode;

	int i;*/
	int nReadPos;
	int nStartIndex;//, nListCount;
/*	int	nDBIndex;

	SQLINTEGER	nSQLIndex;

	BOOL	bQuerySuccess;

	CBufferEx	TempBuf;

	SQLINTEGER	iIndex;
	SQLCHAR		strUserID[CHAR_NAME_LENGTH+1];		
	SQLCHAR		strSendID[CHAR_NAME_LENGTH+1];
	SQLCHAR		strTime[15];
	SQLCHAR		bRead;


	::ZeroMemory(strUserID, sizeof(strUserID));
	::ZeroMemory(strSendID, sizeof(strSendID));
	::ZeroMemory(strTime, sizeof(strTime));
	::ZeroMemory(szSQL, sizeof(szSQL));

	nDBIndex	= 0;
	pDB = g_DB[m_iModSid].GetDB( nDBIndex );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return;
	}
*/
	nReadPos	= 0;
	nStartIndex = GetInt(pBuf, nReadPos);

/*	if( nStartIndex > 1 ) nStartIndex--;

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call MAIL_READ_LIST ( %d )}" ), nStartIndex);
	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));

	nListCount	= 0;

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while(1)
		{
			retcode = SQLFetch( hstmt );

			if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
			{
				bQuerySuccess = TRUE;

				i = 1;
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strUserID,	sizeof(strUserID),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strSendID,	sizeof(strSendID),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strTime,	sizeof(strTime),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		&bRead,		sizeof(SQL_C_CHAR),	&nSQLIndex );

				TempBuf.Add((SQLINTEGER)(iIndex));
//				TempBuf.AddString((LPTSTR)strUserID);
				TempBuf.AddString((LPTSTR)strSendID);
				TempBuf.AddString((LPTSTR)(strTime));
				TempBuf.Add(bRead);
				
				::ZeroMemory(strUserID, sizeof(strUserID));
				::ZeroMemory(strSendID, sizeof(strSendID));
				::ZeroMemory(strTime, sizeof(strTime));

				nListCount++;
			}
			else if(retcode == SQL_NO_DATA)
			{
				bQuerySuccess = TRUE;
				break;
			}
			else 
			{
				bQuerySuccess = FALSE;
				break;
			}
		}
	}
	else if (retcode == SQL_ERROR)
	{
		bQuerySuccess = FALSE;
		DisplayErrorMsg(hstmt);
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(nDBIndex);	

	if( !bQuerySuccess) return;
*/
	nReadPos = 0;
	SetByte(m_TempBuf, MAIL_LIST_RESULT, nReadPos);

	MailGetList( m_TempBuf, nStartIndex, 8, nReadPos );
//	SetShort(m_TempBuf, nListCount, nReadPos );
//	SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), nReadPos);
	Send(m_TempBuf, nReadPos);
}

int USER::MailGetList( char* pBuf, int nStartIndex, int nCount, int& index )
{
	if ( pBuf == NULL ) return -1;

	CDatabase*	pDB;
	SQLHSTMT	hstmt = NULL;
	TCHAR		szSQL[1024];	
	SQLRETURN	retcode;

	int i;
	int nListCount;
	int	nDBIndex;

	SQLINTEGER	nSQLIndex;

	SQLINTEGER  nUserIDSize;

	BOOL	bQuerySuccess;

	CBufferEx	TempBuf;

	SQLINTEGER	iIndex;
	SQLCHAR		strUserID[CHAR_NAME_LENGTH+1];		
	SQLCHAR		strSendID[CHAR_NAME_LENGTH+1];
	SQLCHAR		strTime[15];
	BOOL		bRead;


	::ZeroMemory(strUserID, sizeof(strUserID));
	::ZeroMemory(strSendID, sizeof(strSendID));
	::ZeroMemory(strTime, sizeof(strTime));
	::ZeroMemory(szSQL, sizeof(szSQL));

	nDBIndex	= 0;
	pDB = g_DB[m_iModSid].GetDB( nDBIndex );
	if( !pDB ) return -1;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return -1;
	}

	if( nStartIndex > 1 ) nStartIndex--;

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call MAIL_READ_LIST ( %d, ? )}" ), nStartIndex );

	nUserIDSize = _tcslen(m_strUserID);

	i=1;
	retcode = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 30,	0, (TCHAR*)m_strUserID,	0, &nUserIDSize );

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));

	nListCount	= 0;

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while(nListCount < nCount)
		{
			retcode = SQLFetch( hstmt );

			if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
			{
				bQuerySuccess = TRUE;

				i = 1;
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strUserID,	sizeof(strUserID),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strSendID,	sizeof(strSendID),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strTime,	sizeof(strTime),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_SHORT,	&bRead,		sizeof(SQL_C_TINYINT),	&nSQLIndex );

				TempBuf.Add((SQLINTEGER)(iIndex));
//				TempBuf.AddString((LPTSTR)strUserID);
				TempBuf.AddString((LPTSTR)strSendID);
				TempBuf.AddString((LPTSTR)(strTime));
				TempBuf.Add(bRead);
				
				::ZeroMemory(strUserID, sizeof(strUserID));
				::ZeroMemory(strSendID, sizeof(strSendID));
				::ZeroMemory(strTime, sizeof(strTime));

				nListCount++;
			}
			else if(retcode == SQL_NO_DATA)
			{
				bQuerySuccess = TRUE;
				break;
			}
			else 
			{
				bQuerySuccess = FALSE;
				break;
			}
		}
	}
	else if (retcode == SQL_ERROR)
	{
		bQuerySuccess = FALSE;
		DisplayErrorMsg(hstmt);
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return -1;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(nDBIndex);	

	if( !bQuerySuccess) return -1;

	SetShort( pBuf, (short)nListCount, index );
	if( nListCount > 0 ) SetString(pBuf, TempBuf, TempBuf.GetLength(), index);

	return nListCount;
}

BYTE USER::MailGetInfo( char* pBuf, int& index )
{
	if ( pBuf == NULL ) return -1;

	CDatabase*	pDB;
	SQLHSTMT	hstmt = NULL;
	TCHAR		szSQL[1024];	
	SQLRETURN	retcode;

	BYTE	error;

	int		i;
	int		nDBIndex;	

	SQLINTEGER	nNewMail = -1;
	SQLINTEGER	nCurMail = -1;

	SQLINTEGER	nUserIDSize;
	SQLINTEGER	nNewMailSize;
	SQLINTEGER	nCurMailSize;

	::ZeroMemory(szSQL, sizeof(szSQL));

	nDBIndex	= 0;
	pDB = g_DB[m_iModSid].GetDB( nDBIndex );
	if( !pDB ) 
	{
		error = 1;
		goto exit;
	}

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		error = 1;
		goto exit;
	}

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call MAIL_READ_INFO ( ?, ?, ? )}" ) );

	nUserIDSize = _tcslen(m_strUserID);
	nNewMailSize = SQL_NTS;
	nCurMailSize = SQL_NTS;

	i=1;
	retcode = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 30,	0, (TCHAR*)m_strUserID,	0, &nUserIDSize );
	retcode = SQLBindParameter(hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0,	0, &nNewMail,	0, &nNewMailSize );
	retcode = SQLBindParameter(hstmt, i++, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0,	0, &nCurMail,	0, &nCurMailSize );

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		error = 0;
	}
	else if (retcode == SQL_ERROR)
	{
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		error = 1;
		goto exit;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(nDBIndex);	

exit:

	SetInt(pBuf, nNewMail, index);
	SetInt(pBuf, nCurMail, index);

	return error;
}


void USER::MailContentReq( TCHAR* pBuf )
{
	if ( pBuf == NULL ) return;

	CDatabase*	pDB;
	SQLHSTMT	hstmt = NULL;
	TCHAR		szSQL[1024];	
	SQLRETURN	retcode;
	SQLINTEGER	nSQLIndex;

	int i;
	int nReadPos;
	int	nDBIndex;

	BYTE error;

	int nMailReqIndex;

	CBufferEx TempBuf;

	BOOL bQuerySuccess;

	SQLINTEGER	iIndex;
	SQLCHAR		strUserID[CHAR_NAME_LENGTH+1];		
	SQLCHAR		strSendID[CHAR_NAME_LENGTH+1];
	SQLCHAR		strTime[15];
	SQLCHAR		bRead[20];
	SQLCHAR		strMessage[1001];

	::ZeroMemory(strUserID, sizeof(strUserID));
	::ZeroMemory(strSendID, sizeof(strSendID));
	::ZeroMemory(strTime, sizeof(strTime));
	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strMessage, sizeof(strMessage));

	nReadPos = 0;
	nMailReqIndex = GetInt( pBuf, nReadPos );

	nDBIndex = 0;
	pDB = g_DB[m_iModSid].GetDB( nDBIndex );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return;
	}

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call MAIL_READ_MESSAGE ( %d )}" ), nMailReqIndex );
	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while(1)
		{
			retcode = SQLFetch( hstmt );

			if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
			{
				bQuerySuccess = TRUE;

				i = 1;
				SQLGetData( hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strUserID,	sizeof(strUserID),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strSendID,	sizeof(strSendID),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		strTime,	sizeof(strTime),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_CHAR,		&bRead,		sizeof(SQL_C_CHAR),	&nSQLIndex );
				SQLGetData( hstmt, i++, SQL_C_BINARY,	strMessage,	sizeof(strMessage),	&nSQLIndex );

				TempBuf.Add((int)(iIndex));
				TempBuf.AddLongString((LPTSTR)strMessage, _tcslen((LPCTSTR)strMessage));

				::ZeroMemory(strUserID, sizeof(strUserID));
				::ZeroMemory(strSendID, sizeof(strSendID));
				::ZeroMemory(strTime, sizeof(strTime));
				::ZeroMemory(strMessage, sizeof(strMessage));

				break;
			}
			else if(retcode == SQL_NO_DATA)
			{
				bQuerySuccess = TRUE;
				break;
			}
			else 
			{
				bQuerySuccess = FALSE;
				break;
			}
		}
	}
	else
	{				
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(nDBIndex);
	
	if( bQuerySuccess ) error = 0;
	else error = 1;
	
	nReadPos = 0;
	SetByte(m_TempBuf, MAIL_CONTENTS_RESULT, nReadPos);
	SetByte(m_TempBuf, error, nReadPos);
	if( error == 0 ) SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), nReadPos);

	Send(m_TempBuf, nReadPos);
}

void USER::MailDeleteReq( TCHAR* pBuf )
{
	if ( pBuf == NULL ) return;

	CDatabase*	pDB;
	SQLHSTMT	hstmt = NULL;
	TCHAR		szSQL[1024];	
	SQLRETURN	retcode;

	SQLINTEGER	nUserIDSize;
	SQLINTEGER		sReturn;
	SQLINTEGER	iReturnIndex;

	int nReadPos;
	int	nDBIndex;

	char pSuccessBuf[10000];
	int	nBufferCount;
	int nMailReqIndex;
	int	nMailNextIndex;
	int nMailListCount;

	int	i;
	BYTE error;

	CBufferEx TempBuf;

	::ZeroMemory( szSQL, sizeof(szSQL) );

	nReadPos = 0;
	nMailReqIndex = GetInt( pBuf, nReadPos );
	nMailNextIndex = GetInt( pBuf, nReadPos );

	nDBIndex = 0;
	pDB = g_DB[m_iModSid].GetDB( nDBIndex );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS ) return;

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call MAIL_DELETE ( %d, ?, ? )}" ), nMailReqIndex );

	nUserIDSize = _tcslen(m_strUserID);
	sReturn			= -1;
	iReturnIndex	= SQL_NTS;

	i=1;
	retcode = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 30,	0, (TCHAR*)m_strUserID,	0, &nUserIDSize );
	retcode = SQLBindParameter(hstmt, i++, SQL_PARAM_OUTPUT,SQL_C_LONG, SQL_INTEGER, 0,		0, &sReturn,			0, &iReturnIndex);
	if (retcode!=SQL_SUCCESS) return;

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(nDBIndex);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(nDBIndex);

	error = (BYTE)sReturn;

	nReadPos = 0;
	SetByte(m_TempBuf, MAIL_DELETE_RESULT, nReadPos);
	SetByte(m_TempBuf, error, nReadPos);
	SetInt(m_TempBuf, nMailReqIndex, nReadPos);

	if( error == 0 )
	{
		nBufferCount = 0;
		MailGetInfo( pSuccessBuf, nBufferCount );
		SetString(m_TempBuf, pSuccessBuf, nBufferCount, nReadPos );

		nBufferCount = 0;
		nMailListCount = MailGetList( pSuccessBuf, nMailNextIndex, 1, nBufferCount );
		//SetShort(m_TempBuf, nMailListCount, nReadPos );
		SetString(m_TempBuf, pSuccessBuf, nBufferCount, nReadPos );
	}

	Send(m_TempBuf, nReadPos);
}



//------------------------------------------------------------
// ������ ���̽��� ������Ʈ �Ѵ�.
//--yskang 0.1
//���Լ��� �����忡���� ȣ���մϴ�.
//------------------------------------------------------------
void USER::InsertLoveName(TCHAR *pBuf)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	BOOL			bSuccess = FALSE;
	TCHAR			szSQL[160];	
	BYTE error_code=0;
	USER *pUser = NULL;
	CDatabase* pDB = NULL;
	CBufferEx TempBuf;
	CBufferEx ResultTempBuf;
	int index = 0;
	int db_index = 0;
	int uid=0;
	uid = GetInt(pBuf, index);
	if(uid < 0 || uid >= INVALID_BAND) { error_code = ERR_2; goto go_result; }
	pUser = GetUser(uid - USER_BAND);								
	if(!pUser) { error_code = ERR_2; goto go_result; }// ������ ����.


	::ZeroMemory(szSQL, sizeof(szSQL));
	index = 0;
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call INSERT_LOVE_NAME (\'%s\', \'%s\',%d)}"), pUser->m_strUserID, pUser->m_strLoveName, pUser->m_dwGuild); 

	
	//pDB = g_DBNew[m_iModSid].GetDB( db_index );
	pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) { error_code = ERR_5; goto go_result; }

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode != SQL_SUCCESS)
		{ error_code = ERR_5; goto go_result; }

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			bQuerySuccess = FALSE;
		}
	}
	
	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	//g_DBNew[m_iModSid].ReleaseDB(db_index);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) { error_code = ERR_5; goto go_result; }

	//ȣĪ�� �޸� ��� �����Ѵ�(?)
	lstrcpy(pUser->m_pMD->m_strLoveName , pUser->m_strLoveName);
	//ȣĪ�� �ٸ� ����鿡�� �˸���(���� ��ũ���� �ִ»���鿡 ���ؼ�?)
	TempBuf.Add(LOVE_NAME);
	TempBuf.Add(uid);
	TempBuf.AddString(pUser->m_strLoveName);
	pUser->SendExactScreen(TempBuf, TempBuf.GetLength());
	bSuccess = TRUE;
	m_bPseudoString=FALSE;//�Է� �����Ͽ����Ƿ� ���� ȣĪ�� �ƴϴ�.
go_result:
	ResultTempBuf.Add(LOVE_NAME_RESULT);
	if(bSuccess == FALSE)
	{
		ResultTempBuf.Add(FAIL);				// ����
		ResultTempBuf.Add(error_code);
		Send(ResultTempBuf,ResultTempBuf.GetLength());
		return;
	}
	ResultTempBuf.Add(SUCCESS);					// ����	
	Send(ResultTempBuf, ResultTempBuf.GetLength());
}



//////////////////////////////////////////////////////////////////////////////
//	�Խù��� �����Ѵ�.
//
void USER::BBSDelete(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int index = 0;
	int bbsnum = GetShort( pBuf, index );
	int delindex = GetInt( pBuf, index );

	if(m_tIsOP != 1 && ( bbsnum == 1 || bbsnum == 3 ) )		//�����Խ��� or �ο����� �Խ���
	{
		SendSystemMsg( IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	SQLSMALLINT	sRet = -1;
	SQLINTEGER	iRetInd = SQL_NTS;

	SDWORD sIDLen		= _tcslen(m_strUserID);

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_DELETE (%d, %d, ?, ?)}" ), bbsnum, delindex );

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Delete BBS (BBS:%d, Index:%d) !!\n", bbsnum, delindex);
		return;
	}

	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20,		0, (TCHAR*)m_strUserID,	0, &sIDLen );
	retcode = SQLBindParameter(hstmt, 2 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &iRetInd);
	if (retcode!=SQL_SUCCESS) 
	{
		TRACE("USER::BBSDelete(), Line = %d, File = %s\n", __LINE__, __FILE__);
		return;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, sizeof(szSQL));
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	if( sRet != 1 )
	{
		if(g_bDebug) SendSystemMsg( IDS_USER_BBS_CANT_DELETE, SYSTEM_NORMAL, TO_ME);
		SendBBSError(BBS_CONTENTS_DELETE);
		return;
	}
	BBSOpen( bbsnum );
}

void USER::BBSDeleteWithThread(TCHAR *pBuf)
{
	int index = 0;
	int bbsnum = GetShort( pBuf, index );
	int delindex = GetInt( pBuf, index );

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = BBS_DELETE;
	Datalength = index;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, pBuf, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );
}

//##################################################################################
//
//	Battle
//
//##################################################################################

//////////////////////////////////////////////////////////////////////////////////
//	User �� ���ݷ��� ���´�.
//
int USER::GetAttack()
{
	// ���߿��� �������Ĵ�� ó���ϵ��� �Ѵ�.
	// ������ �׳� �������� ���´�.

	int nAttack = XdY(2, 80);
	return nAttack;
}

//////////////////////////////////////////////////////////////////////////////////
//	������ ������ ���´�.
//
int USER::GetDefense(short* sItemDefense)
{
	int nDefense = 0;

	short sHeadSid = m_UserItem[HEAD].sSid;
	short sBodySid = m_UserItem[BODY].sSid;
	short sPantsSid = m_UserItem[PANTS].sSid;
	short sShoesSid = m_UserItem[SHOES].sSid;

	short sHeadDefense = GetItemDefense(sHeadSid, HEAD);
	short sBodyDefense = GetItemDefense(sBodySid, BODY);
	short sPantsDefense = GetItemDefense(sPantsSid, PANTS);
	short sShoesDefense = GetItemDefense(sShoesSid, SHOES);

	if(m_UserItem[HEAD].sDuration <= 0) sHeadDefense = 0;
	if(m_UserItem[BODY].sDuration <= 0) sBodyDefense = 0;
	if(m_UserItem[PANTS].sDuration <= 0) sPantsDefense = 0;
	if(m_UserItem[SHOES].sDuration <= 0) sShoesDefense = 0;


	nDefense = sHeadDefense + sBodyDefense + sPantsDefense + sShoesDefense + m_DynamicUserData[MAGIC_DEFENSE_UP] \
			                  + (int)((double)m_sMagicCON * (double)m_DynamicEBodyData[EBODY_CON_TO_DEFENSE] / 100) ; 

	short sMaxLevel = g_arUserLevel.GetSize() + ADD_USER_LEVEL;//yskang temp m_sLevel�� g_arUserLevel�� �迭ũ�⸦ �ѱ�� ����.
	if(m_sLevel >= ADD_USER_LEVEL && m_sLevel<sMaxLevel ) nDefense += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sDefense;

	nDefense += (int)((double) nDefense * (double)m_DynamicEBodyData[EBODY_DEFENSE_UP] / 100); 
	if(nDefense <= 0) nDefense = 0;
	if (m_byClass==1)//����Ƿ�ʦ������30%
	{
		nDefense=nDefense-(int)(nDefense*0.00);
	}
	if (m_byClass==0)//�����ȭ����25%
	{
		nDefense=nDefense + (int)(nDefense*0.30);
	}
	return nDefense;
}

///////////////////////////////////////////////////////////////////////////////////
//	���̾� ��ų
//
double USER::GetIronSkill()
{
	BYTE tWeaponClass = 0;
	
	int IronSkin = 0;
	int iLevel = 0;
	int nDefense = 0;
	int iSkillSid = 0;

	double nISkill = 0;

	if(IsCanUseWeaponSkill(tWeaponClass) == FALSE)		// ���� �ڽ��� �����տ� �� ������ ��ų�� ����� �� ������ �� �Ǽ��̸�
	{
		nDefense = (int)((double)m_sMagicCON/3 + 0.5);
		return nDefense; //���̾� ��Ų�� ������ 
	}

	int tClass = tWeaponClass * SKILL_NUM;
	// �ڱ� ���� ��ų��... 
	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid = m_UserSkill[i].sSid;

		if(iSkillSid == SKILL_IRON) // ���̾� ��Ų(9)					// 0 index
		{			
			iLevel = m_UserSkill[i].tLevel;		// ���� ��ų ���� 
			if(iLevel <= 0) continue;
												// �����ۿ� ���� ��ų ���� ����
			iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];

			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			if(iLevel >= g_arSkillTable[iSkillSid]->m_arInc.GetSize()) return 0;
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;

			IronSkin += g_arSkillTable[iSkillSid]->m_arInc.GetAt(iLevel);
		}
	}

	nISkill = ((double)m_sMagicCON/3)*(1 + (double)IronSkin/100.0) + 0.5; // ������

	if(nISkill <= 0) nISkill = 0;
	return nISkill;
}

///////////////////////////////////////////////////////////////////////////////////
//	Damage ���, ���� m_sHP �� 0 �����̸� ���ó��
//
void USER::SetDamage(int nDamage)
{
	if(nDamage <= 0) return;
	if(m_bLive == USER_DEAD) return;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Test Code
	if( m_sHP <= (short)nDamage )
	{
		if(InterlockedCompareExchange((LONG*)&m_lDeadUsed, (long)1, (long)0) == (long)0)
		{
			m_sHP = 0;
			Dead();
		}
		else return;
	}
	else
	{
		m_sHP -= (short)nDamage;
	}

	SendHP();	// Attack_Result���� �����϶� HP������ ������.

	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	
/*	m_sHP -= (short)nDamage;

	if( m_sHP <= 0 )
	{
		if(InterlockedCompareExchange(&m_lDeadUsed, (LONG)1, 0) == 0)
		{
			m_sHP = 0;
			Dead();
		}
		else m_sHP = 1;
	}

	SendHP();
	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
*/
//	TRACE("USER(%d) HP = %d\n", m_uid, m_sHP);
}

////////////////////////////////////////////////////////////////////////////////////
//	���ó��
//
void USER::Dead()
{
	DWORD dwCurrTime = GetTickCount();

	if(m_bLive == USER_DEAD) return;

	if(UpdateUserData() == FALSE)	// DB�� ������ �ȵǸ�
	{
		m_bLive = USER_LIVE;
		m_sHP = 1;
		SendHP();
		TRACE("DEAD HP =1, UpdateUserData() Fail!!!!\n");
//		SendSystemMsg("UpdateUserData() Fail", SYSTEM_NORMAL, TO_ME);
		return;
	}

//	SetUid(m_curx, m_cury, 0);
	m_bLive = USER_DEAD;

	// �ο����� ó��
	ExitRoyalRumble();

	CPoint pos = ConvertToClient( m_curx, m_cury );
	if( pos.x == -1 || pos.y == -1 ) { pos.x = 162; pos.y = 1452; }	// �̺κ��� ���߿� �ٲ���

	CBufferEx TempBuf;
	TempBuf.Add(DEAD);
	TempBuf.Add((short)(m_uid + USER_BAND));
	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);

	SendInsight(TempBuf, TempBuf.GetLength());
	
	ClearAbnormalTime(ABNORMAL_TIME);
	ClearAbnormalTime(PSI_ONE_TIME);
	ClearAbnormalTime(PSI_TWO_TIME);
	ClearAbnormalTime(PSI_THREE_TIME);
	
	m_dwAbnormalInfo = ABNORMAL_NONE;
	m_dwAbnormalInfo_ = ABNORMAL_NONE;
	m_dwAbnormalInfo_ |= 1;
	m_dwAbnormalInfo_ |=ABNORMAL_COLD;
	
	if(m_tIsOP == 1) AddAbnormalInfo(OPERATION_MODE);
	
	m_dwNoDamageTime	= 0;
	m_dwLastNoDamageTime= GetTickCount();

	SetPsiAbnormalStatus();

	SetUserToMagicUser(0);

	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0)
	{
		if(m_bGuildMaster)			// �渶�� ������.
		{
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_GUILD_DEFEAT, m_strGuildName);
			SendGuildWarFieldEnd((LPTSTR)(LPCTSTR)strMsg);// �׺�
		}
		else EndGuildWar(GUILD_FIELD_WARRING);			// �ٽ� ������ �ȵȴ�.
	}

	// �ŷ����̸� �ŷ���� ó��
	if(m_bNowTrading == TRUE) 
	{
		BYTE result = 0x00;
		SendExchangeFail(result, (BYTE)0x04);

		USER *pTradeUser = NULL;
		if(m_iTradeUid != -1)
		{
			pTradeUser = GetUser(m_iTradeUid - USER_BAND);
			if(pTradeUser != NULL)	
			{
				if(pTradeUser->m_state == STATE_GAMESTARTED) pTradeUser->SendExchangeFail(result, (BYTE)0x05);
			}
		}
	}

//	m_sHP = m_sMaxHP;

//	SendMyInfo(TO_ME, INFO_MODIFY);
//	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
}

///////////////////////////////////////////////////////////////////////////////////
//����������.�͵�����...
// yskang 0.8 add dead log strPKName �߰� 
void USER::GetLevelDownExp(BOOL bPK, int enemyrank, BOOL bEventSummon,TCHAR *strPKName)
{
	//StartLevelDown = TRUE;
	BYTE tSlot = 0;
	CBufferEx TempBuf;

	int minusratio = GetMinusExpRatioByCityRank( enemyrank );
    if (!bPK && m_sLevel>=105) minusratio=minusratio / 2;
	double minusresult = 0.01 * (double)minusratio;

	int skill[] = {0,0,0,0,0};
	int status[] = {0,0,0,0,0};
	int type = 0;
	DWORD minusExp = 0;
	DWORD preExp = 0;
	long lDiffExp = 0;
	int LevelDownType = 10;
	m_dwLastDownExp = 0;

	if(m_bPkStatus) 
	{
		m_dwPkStatusTime = 0;
		m_bPkStatus = FALSE;
	}

	if(m_tGuildHouseWar == GUILD_WARRING) return;// Virtual Room������ ������ ����.
	if(m_sLevel <= 5) return;
	if(CheckInvalidMapType() == 12) return;		// �����

	if(m_tFortressWar == GUILD_WARRING || m_tGuildWar == GUILD_WARRING)
	{
		//minusExp = (DWORD)m_dwExpNext/0;		// ����ս�����������ȼ�
		minusExp = 0;
		LevelDownType = 0;//5%
	}
	if(bEventSummon)							// 
	{
		minusExp = (DWORD)m_dwExpNext/100;		// �̺�Ʈ ħ�� ���� �϶� 1% ����
		LevelDownType = 1;
	}
	else
	{
//		minusExp = (DWORD)m_dwExpNext/10;		// ���� ���� ����ġ�� 10%�� �������� ���ҽ�Ų��.
		minusExp = (DWORD)((double) m_dwExpNext * minusresult );

		//--------------------------------------------------------------------------------------
		//yskang fortress buger ���� ���� �����̶�� ����ġ 5%
		for(int i = 0; i < g_arGuildFortress.GetSize(); i++)
		{
			//break;
			if(!g_arGuildFortress[i]) continue;

			if(m_curz == g_arGuildFortress[i]->m_iZone)
			{
				minusExp = (DWORD)m_dwExpNext/20;		// �������϶�...���� ������϶�...
				LevelDownType = 5;
				break;
			}
			int type = 0;
			type = CheckInvalidMapType();
			if(type == 8 || type == 10 || type == 15)//���� �����̴�. 
			{
				minusExp = (DWORD)m_dwExpNext/20;		// �������϶�...���� ������϶�...
				LevelDownType = 5;
				break;
			}
			if(g_arGuildFortress[i]->m_lUsed == 1)
			{
				if(g_arGuildFortress[i]->m_sFortressID == 1000 && type==9)
				{
					minusExp = (DWORD)m_dwExpNext/20;		// �������϶�...���� ������϶�...
					LevelDownType =5;
					break;
				}
				else if(g_arGuildFortress[i]->m_sFortressID == 1001 && type==11)
				{
					minusExp = (DWORD)m_dwExpNext/20;		// �������϶�...���� ������϶�...
					LevelDownType = 5;
					break;
				}
				else if(g_arGuildFortress[i]->m_sFortressID == 1002 && type==16)
				{
					minusExp = (DWORD)m_dwExpNext/20;		// �������϶�...���� ������϶�...
					LevelDownType=5;
					break;
				}
			}
		}
		//-------------------------------------------------------------------------------------------------

	}
//	if(m_bRingOfLife == TRUE && minusExp > 0)	
	
		// �������� ������ٴ� ������ �˷������
		m_bRingOfLife=false;
		m_bNecklaceOfShield=false;
		m_bEarringOfProtect=false;
		for(int i = 0; i < MAX_ACCESSORI; i++)
		{
			if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_RING_OF_LIFE)		m_bRingOfLife = TRUE;
			if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_NECKLACE_OF_SHIELD)	m_bNecklaceOfShield = TRUE;
			if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_EARRING_OF_PROTECT)	m_bEarringOfProtect = TRUE;
		}
	
		EnterCriticalSection( &m_CS_ExchangeItem );
	if(m_bRingOfLife == TRUE)	
	{
		SendAccessoriDuration(SID_RING_OF_LIFE);
		LiveReq(NULL, 1);
		SendSystemMsg(IDS_RING_OF_LIFE, SYSTEM_ANNOUNCE, TO_ME);
		m_bRingOfLife=false;
		LeaveCriticalSection( &m_CS_ExchangeItem );
		return;
	}
	LeaveCriticalSection( &m_CS_ExchangeItem );
	//--------------------------------------------------------------------------------------------------
	//--yskang 0.6 ���� �����  �����̾� - ����ġ ���Ҹ� �� �ٿ� �ش�. ���������� 1/2�������� 
	int tempExp = minusExp;
	if(m_iDisplayType != 5 && m_iDisplayType !=6 && LevelDownType !=1 )
	{
		if(LevelDownType == 5)
			m_dwExp += (DWORD)m_dwExpNext/40; //����ġ�� 2.5% �ø� ���¿��� ó�����ش�.
		else if(LevelDownType == 10)
			m_dwExp += (DWORD)((double)m_dwExpNext*minusresult/2.0);
		else
			m_dwExp += (DWORD)m_dwExpNext/20;

		tempExp = minusExp/2;
	}
	//--------------------------------------------------------------------------------------------------

	lDiffExp = m_dwExp - minusExp;
	preExp = m_dwExp;

	m_dwLastDownExp	= tempExp;				// ��Ȱ�� ��� �̰���ġ�� �������� ����ġ�� �����Ѵ�.

    if(m_sLevel > 74)	DropItemFromDeadUser(bPK, enemyrank);			//ţţ

	if(lDiffExp >= 0)							// ����ġ�� �����Ѵ�.
	{
		type = 0;
		m_dwExp = lDiffExp;
		SendExp();
		UpdateUserDeadLog(minusExp, preExp,strPKName);//yskang 0.8 add dead log		// ���� ����ġ �α׸� SQL�����忡�� ó�� //�α� ���涧�� ���������� ���̰� �ϱ� ���Ͽ�..tempMinus������ �ʿ�.
		return;	
	}
	else										// ���� �ٿ��� �Ͼ��.
	{	
		type = 2;
		int sCount = 20;						// �ִ� ���� ������ ���� Ƚ��

		lDiffExp = (long)(m_dwExp * (double)100/minusExp);		// 10%������ �󸶸�ŭ�� %�� ���ҵǾ� �ִ���...

		m_sLevel = m_sLevel - 1;				// ���������ϰ� ���ҵ� �������� ����%�� ������.. 

//		long minus = g_arLevelUpTable[m_sLevel]->m_dwExp/10;
		
		long minus=0;
		if(LevelDownType ==5 )
			minus = (long)(g_arLevelUpTable[m_sLevel]->m_dwExp /20);//5;
		else if(LevelDownType == 1)
			minus = (long)(g_arLevelUpTable[m_sLevel]->m_dwExp /100);//1;
		else
			minus = (long)(g_arLevelUpTable[m_sLevel]->m_dwExp * minusresult);///10;

		minus = (long)(minus * (double)( 1.0 - (double)lDiffExp/100));	// �ش� ������ 10%�� ������ ������	
		m_dwExp = (DWORD)(g_arLevelUpTable[m_sLevel]->m_dwExp - minus);
												// Skill Point�� ���Ѵ�.
		if( m_sLevel < 99 )
		{
			if(g_arLevelUpTable[m_sLevel]->m_tBasicUp)
			{
				if(m_sSkillPoint > 0) m_sSkillPoint -= 1; 
				else 
				{ 
					int iSid = 0;
					int tClass = m_byClass * SKILL_NUM;
					sCount = 0;

					if(m_UserSkill[tClass].tLevel > 0)     { skill[sCount] = tClass; sCount++; }
					if(m_UserSkill[tClass + 1].tLevel > 0) { skill[sCount] = tClass + 1; sCount++; }
					if(m_UserSkill[tClass + 2].tLevel > 0) { skill[sCount] = tClass + 2; sCount++; }
					if(m_UserSkill[tClass + 3].tLevel > 0) { skill[sCount] = tClass + 3; sCount++; }
					if(m_UserSkill[tClass + 4].tLevel > 0) { skill[sCount] = tClass + 4; sCount++; }

	//				while(sCount > 0)					//	0~4, 5~9, 10~14, 15~19���� ��ų��
					if(sCount > 0)
					{
						sCount = sCount - 1;
						type = myrand(0, sCount, TRUE);

						m_UserSkill[skill[type]].tLevel -= 1;
					}			
				}										
			}
		}
		else
		{
			if(m_sLevel <119){//100 - 119 ��֮�� ��1��ټ�����
				int emptyskillslot;
				switch( m_byClass )
				{
					case	BRAWL:
						emptyskillslot = 4;
						break;
					case	STAFF:
						emptyskillslot = 9;
						break;
					case	EDGED:
						emptyskillslot = 14;
						break;
					case	FIREARMS:
						emptyskillslot = 19;
						break;
					default:
						return;
				}

				if( emptyskillslot == -1 ) return;
				int skillsid = m_UserSkill[emptyskillslot].sSid;

				if( skillsid < 0 || skillsid >= g_arSkillTable.GetSize() ) { m_UserSkill[emptyskillslot].sSid =0; return;}
				if( g_arSkillTable[skillsid]->m_sSid == -1 ) return;
				m_UserSkill[emptyskillslot].tLevel = (BYTE)((m_sLevel % 100) + 1);
				if(m_UserSkill[emptyskillslot].tLevel>20)
					m_UserSkill[emptyskillslot].tLevel=20;
			}
			else if(m_sLevel <129){// 120 -129 ��1��120����ת����ļ���
				int iIndex=m_byClass *SKILL_NUM;
				int i;
				for(i = iIndex; i < iIndex + SKILL_NUM-1; i++)
				{
					if(m_UserSkill[i].sSid>=20){
						m_UserSkill[i].tLevel--;
						if(m_UserSkill[i].tLevel>=20)
							m_UserSkill[i].tLevel=20;
					}
				}
			}
			else //130 ���ϼ�1�� 130����
			{
				int count= m_UserSkill_[m_byClass*3].tLevel + m_UserSkill_[m_byClass*3+1].tLevel + m_UserSkill_[m_byClass*3+2].tLevel + m_sSkillPoint_;
				if(count== (m_sLevel-130+1))
					return;
			if(count>(m_sLevel-130+1)){
				if(m_sSkillPoint_ > 0){
					m_sSkillPoint_ -= 1;
				}else{
					int iSid = 0;
					int tClass = m_byClass * 3;
					sCount = 0;
					if(m_UserSkill_[tClass].tLevel > 0)     { skill[sCount] = tClass; sCount++; }
					if(m_UserSkill_[tClass + 1].tLevel > 0) { skill[sCount] = tClass + 1; sCount++; }
					if(m_UserSkill_[tClass + 2].tLevel > 0) { skill[sCount] = tClass + 2; sCount++; }
					if(sCount > 0)
					{
						sCount = sCount - 1;
						type = myrand(0, sCount, TRUE);
						m_UserSkill_[skill[type]].tLevel -= 1;
					}
				}
			}
			//ResetOver100LevelSkill( m_sLevel );
			}
		}
		
		if(m_sPA > 0)	m_sPA -= 1;
		else									// �� �̻� ���ٸ� �׿��ִ� �����͸� ���Ѵ�.
		{
			sCount = 0;
			if(m_sSTR > m_sTempSTR) { status[sCount] = 0; sCount++; } 
			if(m_sCON > m_sTempCON) { status[sCount] = 1; sCount++; }
			if(m_sDEX > m_sTempDEX) { status[sCount] = 2; sCount++; }
			if(m_sVOL > m_sTempVOL) { status[sCount] = 3; sCount++; }
			if(m_sWIS > m_sTempWIS) { status[sCount] = 4; sCount++; }

			if(sCount > 0)					// 0~4�� �������� ����.
			{
				sCount = sCount - 1;
				type = myrand(0, sCount, TRUE);

				type = status[type];

				switch(type)
				{
				case USER_STR:
					if(m_sSTR > m_sTempSTR) m_sSTR -= 1;					
					break;
				case USER_CON:
					if(m_sCON > m_sTempCON) m_sCON -= 1;
					break;
				case USER_DEX:
					if(m_sDEX > m_sTempDEX) m_sDEX -= 1;
					break;
				case USER_VOL:
					if(m_sVOL > m_sTempVOL) m_sVOL -= 1; 
					break;
				case USER_WIS:
					if(m_sWIS > m_sTempWIS) m_sWIS -= 1; 
					break;
				default:
					break;
				}
			}

			SetUserToMagicUser();		// �ɷ�ġ ���ҷ� ������ �Ͼ��
		}

		if(m_sHP > m_sMagicMaxHP) m_sHP = m_sMagicMaxHP;
		if(m_sPP > m_sMagicMaxPP) m_sPP = m_sMagicMaxPP;
		if(m_sSP > m_sMagicMaxSP) m_sSP = m_sMagicMaxSP;
	}

	UpdateUserDeadLog(minusExp, preExp,strPKName);	//yskang 0.8 add dead log	// ���� ����ġ �α׸� SQL�����忡�� ó��

	UpdateUserData();

	m_dwExpNext = GetNextLevel(m_sLevel);	// ���� �������� ���� �ִ�ġ

	SendExp((BYTE)0x02);					// ������ ��Ŷ���� Level, Exp, PA, MA, SkillPoint�� ����
	SendUserStatusSkill(); 
	SendSystemMsg( IDS_USER_LEVEL_DOWN, SYSTEM_NORMAL, TO_ME);
}

///////////////////////////////////////////////////////////////////////////////////////
//	�� �ùε�޺��� �������� ����߸� Ȯ���� �����ؼ� ����߸���.
//
void USER::DropItemFromDeadUser(BOOL bPK, int enemyrank)
{
	if(m_state != STATE_GAMESTARTED) return;	// �ٸ� �������� ȣ���Ҽ� �����Ƿ�...

	int i, sid = -1, invensid = -1, equipsid = -1;
	int rank = m_sCityRank + CITY_RANK_INTERVAL;
	int iRandom = myrand(1, 100);
	int iWhere = myrand(1, 100);

	BOOL bDrop = FALSE;
	BOOL bInven = FALSE;

	CWordArray	arEquip, arInven;

	CBufferEx	TempBuf;

	rank = GetDropItemRankByAttackerCityRank( enemyrank );

	if(CheckGuildWarArea())
	{
		rank -= 1;
		if( rank < 0 ) rank = 0;
	}

	switch(rank)							// �� ��޺��� Ȯ���� �ٸ���.
	{
	case CHAONISE_RANK:
		if(iRandom < 90) { bDrop = TRUE; if(iWhere < 20) bInven = TRUE; }
		break;
	case DEATH_RANK:
		if(iRandom < 70) { bDrop = TRUE; if(iWhere < 30) bInven = TRUE; }
		break;
	case EVIL_RANK:
		if(iRandom < 30) { bDrop = TRUE; if(iWhere < 40) bInven = TRUE; }
		break;
	case VILLAIN_RANK:
		if(iRandom < 0) { bDrop = TRUE; if(iWhere < 50) bInven = TRUE; }
		break;		
	case CAIN_RANK:
		if(iRandom < 0) { bDrop = TRUE; if(iWhere < 50) bInven = TRUE; }
		break;

	case COMMOM_RANK:
		if(iRandom < 0) { bDrop = TRUE; if(iWhere < 60) bInven = TRUE; }
		break;

	case CITIZEN_RANK:
		if(iRandom < 0) { bDrop = TRUE; if(iWhere < 70) bInven = TRUE; }
		break;
	case VIUAL_RANK:
		if(iRandom < 0) { bDrop = TRUE; if(iWhere < 70) bInven = TRUE; }
		break;
	case WARRIOR_RANK:
		if(iRandom < 0) { bDrop = TRUE; if(iWhere < 80) bInven = TRUE; }
		break;
	case HERO_RANK:		
		if(iRandom < 0) { bDrop = TRUE; if(iWhere < 80) bInven = TRUE; }
		break;
	case SAINT_RANK:		
		//if(iRandom < 5)  { bDrop = TRUE; if(iWhere < 90) bInven = TRUE; }
		if(iRandom < 0)  { bDrop = TRUE; if(iWhere < 90) bInven = TRUE; }	// ����Ʈ�� �������� ������ �ʴ´�. ���ǰ���
		break;

	case (SAINT_RANK+1):
		bDrop = FALSE;
		break;

	default:
		if(iRandom < 90) { bDrop = TRUE; if(iWhere < 10) bInven = TRUE; }
		break;
	}

	if(bDrop)							// �ٽ� ����ȭ ����...//@@@@@@@@@@@@@@@@@@@@@@@2
	{
		arEquip.RemoveAll();
		arInven.RemoveAll();

		for(i = 0; i < EQUIP_ITEM_NUM; i++)
		{
			if(m_UserItem[i].sSid >= 0 && m_UserItem[i].sSid < g_arItemTable.GetSize())
			{
				if(g_arItemTable[m_UserItem[i].sSid]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND) continue;

				arEquip.Add(i);
			}
		}

		for(i = EQUIP_ITEM_NUM; i < TOTAL_INVEN_MAX; i++)
		{
			if(m_UserItem[i].sSid >= 0 && m_UserItem[i].sSid < g_arItemTable.GetSize())
			{
				if(g_arItemTable[m_UserItem[i].sSid]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND) continue;

				arInven.Add(i);
			}
		}

		iWhere = arInven.GetSize();
		if(iWhere > 0)
		{		 
			iRandom = myrand(1, iWhere);
			invensid = arInven[iRandom - 1];
		}

		iWhere = arEquip.GetSize();
		if(iWhere > 0)
		{
			iRandom = myrand(1, iWhere);
			equipsid = arEquip[iRandom - 1];
		}

		if(bInven) sid = invensid;
		else
		{
//			if(rank == SAINT_RANK) return;
			sid = equipsid;
		}
			
		if(rank <= CHAONISE_RANK)
		{
			if(equipsid >= 0) sid = equipsid;
			else			  sid = invensid;
		}

		if(sid != -1 && m_UserItem[sid].sSid >= 0)
		{
			if(m_UserItem[sid].sSid < g_arItemTable.GetSize())
			{
				/*if(bPK)	// ������ ���������� �������� ������.
				{					
					MakeItemLog(&m_UserItem[sid], ITEMLOG_DEAD_DELETE);
					ReSetItemSlot(&m_UserItem[sid]);
					
					TempBuf.Add(ITEM_GIVE_RESULT);
					TempBuf.Add((BYTE)0x01); 
					
					TempBuf.Add((BYTE)sid);
					TempBuf.Add(m_UserItem[sid].sLevel);
					TempBuf.Add(m_UserItem[sid].sSid);
					TempBuf.Add(m_UserItem[sid].sDuration);
					TempBuf.Add(m_UserItem[sid].sBullNum);
					TempBuf.Add(m_UserItem[sid].sCount);
					
					for(int j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sid].tMagic[j]);
					
					TempBuf.Add(m_UserItem[sid].tIQ); 
					Send(TempBuf, TempBuf.GetLength());
				}
				else
				{
				*/
					int index = 0;
					
					TCHAR tBuf[10];
					::ZeroMemory(tBuf, sizeof(tBuf));
					
					CPoint pos = ConvertToClient( m_curx, m_cury );
					if( pos.x == -1 || pos.y == -1 ) { pos.x = 1; pos.y = 1; }	// ���߿� �� ã�Ƽ� �ٲ���.
					
					SetShort(tBuf, pos.x, index);
					SetShort(tBuf, pos.y, index);
					
					SetByte(tBuf, (BYTE)sid, index);
					SetShort(tBuf, m_UserItem[sid].sCount, index);
					
					GiveItemToMap(tBuf, TRUE);
				//}

				if(!bInven) CheckMagicItemMove();		// ������ ���������������Ƿ� üũ...

			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
//	PP �� Ŭ���̾�Ʈ�� ������.
//
void USER::SendPP(BOOL bLevelDown)
{
	CBufferEx TempBuf;

	if(m_bLive == USER_DEAD && !bLevelDown) return;

	TempBuf.Add(SET_PP);
	TempBuf.Add(m_sPP);

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////////
//	���� PP�� ��´�.
//
short USER::GetPP()
{
	BYTE	tWeaponClass = 255;

	int iSkillSid = 0;
	int iLevel = 0, iRandom = 0, iPP = 0;

	double dPlusPP = 1.0; 
	int iAddSP = 0;

	DWORD dwDiffTime = GetTickCount() - m_dwLastPsiTime;

	if(dwDiffTime >= m_iPPIntervalTime)
	{
		if(IsCanUseWeaponSkill(tWeaponClass))	// ���� �ڽ��� �����տ� �� ������ ��ų�� ������
		{
			int tClass = tWeaponClass * SKILL_NUM; 

			for(int i = tClass; i < tClass + SKILL_NUM; i++)			// ��ų�� HPȸ�� ��ų�� ������ �ݿ��Ѵ�.
			{
				iSkillSid = m_UserSkill[i].sSid;

				if(iSkillSid == SKILL_PP_RECOVER_UP)					// 5 index
				{
					iLevel = m_UserSkill[i].tLevel;		
					if(iLevel < 0) iLevel = 0;

					// �����ۿ� ���� ��ų ���� ����
					if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
					if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
					if(iSkillSid >= g_arSkillTable.GetSize()) continue;

					iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
					if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel)) iPP = 1;

					dPlusPP = 1.0 + iPP * (double)g_arSkillTable[iSkillSid]->m_arRepair.GetAt(iLevel) / 100.0;
				}
			}
		}
								// ������
		if(m_iPPRate <= 0) 
		{
			m_iPPRate = 1;
			TRACE("ȸ������ ����\n");
		}

		if(IsCity() && CheckRecoverTableByClass()) iAddSP = g_arRecoverTable[m_byClass]->m_byTown;
		m_sPP += (int)(dPlusPP * (double)m_sMagicWIS/m_iPPRate ) + iAddSP;
		if(m_sPP > m_sMagicMaxPP) m_sPP = m_sMagicMaxPP;

		m_dwLastPsiTime = GetTickCount();
	}

	return m_sPP;
}

////////////////////////////////////////////////////////////////////////////////////
//	���̿��� ����ó��
//
void USER::PsiAttack(TCHAR *pBuf)
{

#ifdef _ACTIVE_USER
		//if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
		if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	DWORD	dwExp = 0;
	int		nDamage = 0;
	int		nTempHP = 0;

	USER*	pUser = NULL;
	CNpc*	pNpc = NULL;

	BYTE	tWeaponClass = 0;
	BOOL	bCanUseSkill = FALSE;
	int		bSuccessSkill[SKILL_NUM] = {FALSE, FALSE, FALSE, FALSE, FALSE};

	int		nPsiRange = 0;
	int		nTPosX	= 0;
	int		nTPosY	= 0;
	int		nDist	= 100;
	short	sNeedPP	= 25000;
	BYTE	tPsiRegi = 0;
	DWORD	dwPsiCast = 0;

	DWORD	dwCurrTick = GetTickCount();

	BOOL	bPsiSuccess = FALSE;

	int index = 0;
	int nTargetID = GetInt(pBuf, index );	// Target ID �� ��´�.
	BYTE byPsi = GetByte(pBuf, index);		// Psionic sid �� ��´�.
	int nPsiX = -1;							// Teleport�� ��ġ
	int nPsiY = -1;
	CPoint ptPsi(-1, -1);

	if(byPsi < 0 || byPsi >= TOTAL_PSI_NUM) return;

	if(byPsi == PSI_TELEPORT)
	{
		// Teleport �� ��ġ�� ��´�.
		nPsiX = GetInt(pBuf, index);
		nPsiY = GetInt(pBuf, index);

		// �ڷ���Ʈ�� �̵��� �� ���� �ڸ��̸� Ŭ���̾�Ʈ���� ������ ��ǥ �״�� ����
		CPoint pt = FindNearAvailablePoint_C(nPsiX, nPsiY); 
		if(pt.x < 0 || pt.y < 0) 
		{
			pt.x = nPsiX;
			pt.y = nPsiY;
		}

		ptPsi = ConvertToServer(pt.x, pt.y);
		nDist = abs(m_curx - ptPsi.x) + abs(m_cury - ptPsi.y);
		if(nDist > SCREEN_X + SCREEN_Y) return;
	}
	else	// ��� ���� üũ (������ ��)
	{
		if(nTargetID < USER_BAND || nTargetID >= INVALID_BAND) return;	// �߸��� Target �̸� return
	}

	if(IsHavePsionic(byPsi) == FALSE) return;						// �������� ���� ���̿����̸� ����	
	
	// �����Ÿ� ��� ------------------------------------------------------------------------//
	nPsiRange = GetPsiRange(byPsi);
	if(nPsiRange > 0)
	{
		if(nTargetID >= USER_BAND && nTargetID < NPC_BAND)	// USER
		{
#ifdef _ACTIVE_USER
	//if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if( m_iDisplayType == 6) return; //yskang 0.5
#endif
			pUser = GetUser(nTargetID - USER_BAND);	// User Pointer �� ��´�.
			if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;// �߸��� USER �̸� ����
			if(pUser->m_lDeadUsed == 1) return;
			if(pUser->m_bPShopOpen == TRUE) return;							// User has personal shop
//			if(pUser->m_bLive == USER_DEAD)	return;		// Target User �� �̹� �׾������� ����
//			if(pUser->m_sHP == 0) return;
			if(byPsi==23 || byPsi==25){//ȭ�ͽ��ı�ɱ����ֻ����2
				if( m_curz != pUser->m_curz || !GetDistance( pUser->m_curx, pUser->m_cury, (2 + 2) ) ) return;
			}
			// ���� �� ȭ�鿡 �ִ��� �Ǵ�. �� ���������� ��� ��ȭ����� ����
			if( !CheckBuddyMember(pUser->m_strUserID) )
			{
				if(!IsInSight(pUser->m_curx, pUser->m_cury, pUser->m_curz)) return;			
			}
			else
			{
				if( m_curz != pUser->m_curz || !GetDistance( pUser->m_curx, pUser->m_cury, (SCREEN_X + SCREEN_Y) ) ) return;
				// Ÿ���� ���������� ��� �����Ÿ��� ������̺� �����ϰ� 2ȭ����� ����
				nPsiRange = (SCREEN_X + SCREEN_Y);
			}

			if(pUser->m_dwNoDamageTime != 0) // �����ð��߿��� ���� ���̿����̰� ���� ���̿����̰� ������ �� ����.
			{
				if((dwCurrTick - pUser->m_dwLastNoDamageTime) > pUser->m_dwNoDamageTime)		// �����ð��� Ǯ�� �ð��̸�
				{
					pUser->m_dwNoDamageTime = 0;
					pUser->m_dwLastNoDamageTime = GetTickCount();
				}
				else
				{
					pUser->m_dwNoDamageTime = pUser->m_dwNoDamageTime - (dwCurrTick - pUser->m_dwLastNoDamageTime);
					pUser->m_dwLastNoDamageTime = dwCurrTick;
					return;	
				}
			}

			if(CheckBuddyMember(pUser->m_strUserID) || !IsPKZone(pUser))
			{
				if(g_arPsiTable[byPsi]->m_tTarget != 1 && g_arPsiTable[byPsi]->m_tTarget != 2) return;

#ifndef _EVENT_RR 
				// �ο� ���� �ʿ��� IsPKZone�� FALSE�� �����߰�
				// �ٸ� �Ӽ��� ���� ���ִ� ���̶�� ��� ���̿����� ���д�.
				if(pUser->m_curz == g_RR.m_iZoneNum || m_curz == g_RR.m_iZoneNum ) 
				{
					if( CheckInvalidMapType() != pUser->CheckInvalidMapType() ) return;
				}
#else		// �ӽ÷� �ְ� �̺�Ʈ������
				if( m_curz == g_RR.m_iZoneNum || m_curz == 61 || m_curz == 62 || m_curz ==  63)
				{
					if( CheckInvalidMapType() != pUser->CheckInvalidMapType() ) return;				
				}
				if( pUser->m_curz == g_RR.m_iZoneNum || pUser->m_curz == 61 || pUser->m_curz == 62 || pUser->m_curz ==  63)
				{
					if( CheckInvalidMapType() != pUser->CheckInvalidMapType() ) return;				
				}
#endif
			}
			nTPosX = pUser->m_curx;
			nTPosY = pUser->m_cury;
		}
		else if(nTargetID >= NPC_BAND)				// NPC
		{
			pNpc = GetNpc(nTargetID - NPC_BAND);	// NPC Point �� ��´�.
			if(pNpc == NULL) return;				// �߸��� NPC �̸� ����
			if(pNpc->m_NpcState == NPC_DEAD) return;
			if(pNpc->m_sHP == 0) return;// NPC �� �̹� �׾� ������ ����
			if(pNpc->m_byAX == 0 && pNpc->m_byAZ == 0 && pNpc->m_tNpcType == 0) return;			// ���ݴɷ��� ���� ����(���� ����)�� �Ϲ� ������ ���� �ʴ´�.
//			if(pNpc->m_tGuildWar == GUILD_WAR_AFFTER && ((pNpc->m_tNpcType == NPCTYPE_GUILD_NPC) || (pNpc->m_tNpcType == NPCTYPE_GUILD_DOOR))
//			{
//				return;
//			}
			if(byPsi==23 || byPsi==25){//ȭ�ͽ��ı�ɱ����ֻ����2
				if( m_curz != pNpc->m_sCurZ || !GetDistance( pNpc->m_sCurX, pNpc->m_sCurY, (2 + 2) ) ) return;
			}

			if(!IsInSight(pNpc->m_sCurX, pNpc->m_sCurY, pNpc->m_sCurZ)) return;

			nTPosX = pNpc->m_sCurX;
			nTPosY = pNpc->m_sCurY;
		}
		else return;

		if(GetDistance(nTPosX, nTPosY, nPsiRange, &nDist) == FALSE)		// ���̿��� ���� �Ÿ� ���̸� ����
		{
			//if(g_bDebug)SendSystemMsg(_T("### Too Long Distance!!!"), SYSTEM_NORMAL, TO_ME);
			return;
		}
	}

	// ������ Ŭ������ �ش� Ŭ������ ��ų�� �� �� �ִ��� �˻� ---------------//
	bCanUseSkill = IsCanUseWeaponSkill(tWeaponClass);	

	// ���� PP üũ ---------------------------------------------------------//
	sNeedPP = GetNeedPP(bSuccessSkill, tWeaponClass, byPsi);
	if(sNeedPP > m_sPP) 
	{
		SendPsiAttackResult(FAIL, nTargetID, byPsi);
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PP, SYSTEM_NORMAL, TO_ME);
		return;
	}

	// Casting Delay ó��
	dwPsiCast = g_arPsiTable[byPsi]->m_sDelayTime;		// ĳ���� ������

	int iLevel = 0;
	int iDecCast = 0;
	int iSkillSid = 0;
	double dEBodyDec = 0;

	//dwPsiCast -= (int)((double)m_DynamicEBodyData[EBODY_WIS_TO_CAST_DOWN] / 100 * m_sMagicWIS);	// WIS �� % �� ĳ���� ���ҷ�
	//if(dwPsiCast < 0) dwPsiCast = 0;

	dEBodyDec = (double)m_DynamicEBodyData[EBODY_CASTING_DELAY_DOWN] / 100 ;				// EBODY �� ���� ĳ���� �ð� ����
	if(dEBodyDec >= 1) dEBodyDec = 0.0;

	if(tWeaponClass == STAFF)	
	{													// ĳ���� Ÿ�� ����ó��		
		if(m_UserItem[RIGHT_HAND].sSid >= 0 && m_UserItem[RIGHT_HAND].sSid < g_arItemTable.GetSize())
		{
			if(g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_sCTime > 0)
			{
				dwPsiCast -= g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_sCTime;
				if(dwPsiCast < 0) dwPsiCast = 0;
			}
		}

		for(int i = tWeaponClass * SKILL_NUM; i < tWeaponClass * SKILL_NUM + SKILL_NUM; i++)
		{
			iSkillSid = m_UserSkill[i].sSid;

			if(iSkillSid == SKILL_CASTING_DOWN) // ĳ����Ÿ�Ӱ���			8 index
			{
				// ���� ��ų ���� 				
				iLevel = m_UserSkill[i].tLevel;		
				if(iLevel < 0) iLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iSkillSid >= g_arSkillTable.GetSize()) break;
				if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				if(iLevel >= g_arSkillTable[iSkillSid]->m_arInc.GetSize()) return;
				if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
								
				iDecCast = g_arSkillTable[iSkillSid]->m_arInc.GetAt(iLevel);
			}
		}
	}
	
	if(iDecCast > 0 || dEBodyDec > 0)
	{
		dwPsiCast = (DWORD)( dwPsiCast * (1 - ((double)iDecCast / 100)- dEBodyDec ));
	}
	//�ͷ��ٶ�
	if(dwPsiCast < 90) dwPsiCast =90;
	if(!(byPsi==23||byPsi==24||byPsi==25||byPsi==26)){
		m_dwCastDelay = dwPsiCast;
		if(CheckPsiCastingDelay() == FALSE) return;
	}else{
		m_dwCastDelay = g_arPsiTable[byPsi]->m_sDelayTime;
		if(dwCurrTick-m_dwLastMaxAttack<m_dwCastDelay) return;
		m_dwLastMaxAttack=dwCurrTick;
		if(m_dwXP<=0){
			SendSystemMsg( "ս�����鲻���޷�ʹ�ñ�ɱ.", SYSTEM_ERROR, TO_ME);
			return;
		}
		m_dwXP--;
		SendXP();
	}

	
	//���̿��� ����
	if(IsException(nTargetID, byPsi, ptPsi) == FALSE) return;
	
	SendPsiAttackResult(SUCCESS, nTargetID, byPsi);
	BOOL bPsiOK = PsionicProcess(nTargetID, byPsi, ptPsi, sNeedPP);	// �ش� ���̿��� ó��
	
	// PP ���� --------------------------------------------------------//
	m_sPP -= sNeedPP;
//����ʨ���..
	SendPP();
	m_dwLastPsiAttack = GetTickCount();
}

///////////////////////////////////////////////////////////////////////////////////
//	���̿��� ���� ����� ���� ���� ��Ȳ�� �����Ѵ�.(�� : Ÿ�� ��Ż�� ���� �������ȴ�.)
//	���̿��� ó�������� ����� ���������� ����, ���и� �Ǵ��Ѵ�.
//
BOOL USER::IsException(int iTargetID, BYTE tPsi, CPoint ptPsi)
{	
	USER *pUser = NULL;
	CNpc* pNpc = NULL;

	BOOL bUser = FALSE;
	BOOL bMon = FALSE;
	int type = 0, index = 0;

	if(iTargetID >= USER_BAND && iTargetID < NPC_BAND)	// USER
	{
		pUser = GetUser(iTargetID - USER_BAND);	// User Pointer �� ��´�.
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return FALSE;// �߸��� USER �̸� ����
		if(pUser->m_bLive == USER_DEAD)	return FALSE;		// Target User �� �̹� �׾������� ����
		bUser = TRUE;
	}
	else if(iTargetID >= NPC_BAND)						// NPC
	{
		pNpc = GetNpc(iTargetID - NPC_BAND);		// NPC Point �� ��´�.
		if(pNpc == NULL) return FALSE;				// �߸��� NPC �̸� ����
		if(pNpc->m_NpcState == NPC_DEAD) return FALSE;// NPC �� �̹� �׾� ������ ����
		bMon = TRUE;
	}
	else return FALSE;

	switch((int)tPsi)
	{
	// �����迭 -------------------------------------------------------------------------------//
	case 0:	//Recovery	: SP ȸ��
		if(bUser) 
		{
			if(pUser->m_dwMindShockTime != 0) return FALSE;	// Mind Shock ���¿����� �ƹ� ���̿��е� �Ȱɸ���.
		}
		if(bMon) return FALSE;
		break;

	case 1:	// HASTE	: ���ݼӵ� ��� (���� ������ ����)
		if(bUser) 
		{
			if( pUser->m_dwDexUpTime != 0 || pUser->m_dwShieldTime != 0 || pUser->m_dwMaxHPUpTime != 0 || 
				pUser->m_dwPiercingShieldTime != 0 || pUser->m_dwBigShieldTime !=0 || pUser->m_dwPsiShieldTime != 0 || pUser->m_dwMindShockTime != 0) return FALSE;	// �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;
		break;

	case PSIONIC_FAST_RUN:	// �̵��ӵ� ���
		if(bUser) 
		{
			if( pUser->m_dwDexUpTime != 0 || pUser->m_dwShieldTime != 0 || pUser->m_dwMaxHPUpTime != 0 || 
				pUser->m_dwPiercingShieldTime != 0 || pUser->m_dwBigShieldTime !=0 || pUser->m_dwPsiShieldTime != 0 || pUser->m_dwMindShockTime != 0) return FALSE;	// �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;

	// ������ �迭 ----------------------------------------------------------------------------//
	case 3:	// ����
		if(bUser) 
		{
			if(pUser->m_dwMindShockTime != 0) return FALSE;	// Mind Shock ���¿����� �ƹ� ���̿��е� �Ȱɸ���.
		}
		if(bMon) return FALSE;
		
		break;

	case 6:	// ����
		if(bUser) 
		{
			if( pUser->m_dwDexUpTime != 0 || pUser->m_dwBigShieldTime !=0 || pUser->m_dwHasteTime != 0 || pUser->m_dwMaxHPUpTime != 0 || pUser->m_dwMindShockTime != 0 ||
				pUser->m_dwPiercingShieldTime != 0 || pUser->m_dwFastRunTime != 0 || pUser->m_dwPsiShieldTime != 0) return FALSE;	// �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;
		break;

	case 7: // �ڷ���Ʈ 
/*
		type = CheckInvalidMapType();
//		type = ((g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_dwType & 0xFF00 ) >> 8);		
//		if(type > 1 && type < g_arMapTable.GetSize())		// ���ø� �����...
		if(type > 1 && type < 8)
		{
			index = g_arMapTable[type]->m_sStoreIndex;
			if(index < 0 || index >= g_arStore.GetSize()) return TRUE;

			if(g_arStore[index]->m_lUsed == 1)	return FALSE;  // �ش� �Ӽ� ���� ����� ����	
		}
*/
		if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;
		if(ptPsi.x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || ptPsi.x < 0) return FALSE;
		if(ptPsi.y >= g_zone[m_ZoneIndex]->m_sizeMap.cy || ptPsi.y < 0) return FALSE;

		type = ((g_zone[m_ZoneIndex]->m_pMap[ptPsi.x][ptPsi.y].m_dwType & 0xFF00 ) >> 8);

		if(!CheckInvalidZoneState(type)) return FALSE;

		if(bUser) 
		{
			if(pUser->m_dwMindShockTime != 0) return FALSE;	// Mind Shock ���¿����� �ƹ� ���̿��е� �Ȱɸ���.
		}
		if(bMon) return FALSE;
		break;

	case PSIONIC_MIND_SHOCK:
		if(bUser) 
		{
			if(pUser->m_dwMindGuardTime != 0) return FALSE;	// Mind Guard ���¿����� Mind Shock �� �ɼ� ����.
		}
		else return FALSE;

		break;

	// ���˰迭 ------------------------------------------------------------------------------//
	case 9:	// Ecasion	: ����ȸ��
/*		type = CheckInvalidMapType();
//		type = ((g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_dwType & 0xFF00 ) >> 8);
//		if(type > 1 && type < g_arMapTable.GetSize())		// ���ø� �����...
		if(type > 1 && type < 8)
		{
			index = g_arMapTable[type]->m_sStoreIndex;
			if(index < 0 || index >= g_arStore.GetSize()) return TRUE;

			if(g_arStore[index]->m_lUsed == 1)	return FALSE;  // �ش� �Ӽ� ���� ����� ����	
		}
*/
		type = CheckInvalidMapType();
		if(!CheckInvalidZoneState(type)) return FALSE;

		if(bUser) 
		{
			if(pUser->m_dwMindShockTime != 0) return FALSE;	// Mind Shock ���¿����� �ƹ� ���̿��е� �Ȱɸ���.
		}
		if(bMon) return FALSE;
		break;

	case PSIONIC_PSI_SHIELD: // ���̿��� ����
		if(bUser) 
		{
			if( pUser->m_dwShieldTime != 0 || pUser->m_dwHasteTime != 0	|| pUser->m_dwMindShockTime != 0 || pUser->m_dwDexUpTime != 0 || 
				pUser->m_dwMaxHPUpTime != 0 || pUser->m_dwBigShieldTime !=0 || pUser->m_dwPiercingShieldTime != 0 || pUser->m_dwFastRunTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		else return FALSE;

		break;

	case 30: // ���̿��� ����
		if(bUser) 
		{
			if( pUser->m_dwShieldTime != 0 || pUser->m_dwHasteTime != 0	|| pUser->m_dwMindShockTime != 0 || pUser->m_dwDexUpTime != 0 || 
				pUser->m_dwMaxHPUpTime != 0 || pUser->m_dwPsiShieldTime != 0|| pUser->m_dwPiercingShieldTime != 0 || pUser->m_dwFastRunTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;

		break;

	// �ѱ� -------------------------------------------------------------------------------------------//
	case 11: // Dex Up
		if(bUser) 
		{
			if( pUser->m_dwShieldTime != 0 || pUser->m_dwHasteTime != 0	|| pUser->m_dwMindShockTime != 0 || 
				pUser->m_dwFastRunTime != 0 || pUser->m_dwBigShieldTime !=0 || pUser->m_dwPsiShieldTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;
		break;
	case 12: // Max HP Up
		if(bUser) 
		{
			if( pUser->m_dwShieldTime != 0 || pUser->m_dwHasteTime != 0 || pUser->m_dwMindShockTime != 0 ||
				pUser->m_dwFastRunTime != 0 || pUser->m_dwBigShieldTime !=0 || pUser->m_dwPsiShieldTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;
		break;

	case PSIONIC_PIERCING_SHIELD:	// �Ǿ�̽ǵ�
		if(bUser) 
		{
			if( pUser->m_dwShieldTime != 0 || pUser->m_dwHasteTime != 0	|| pUser->m_dwMindShockTime != 0 || 
				pUser->m_dwFastRunTime != 0 || pUser->m_dwBigShieldTime !=0 || pUser->m_dwPsiShieldTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		else return FALSE;

		break;

	// ���迭 -----------------------------------------------------------------------------------------//
	case 13: // Adamantine
		if(bUser) 
		{
			if( pUser->m_dwMightyWeaponTime != 0 || pUser->m_dwBerserkerTime != 0 || pUser->m_dwMindShockTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;
		break;
	case 14: // Mighty Weapon
		if(bUser) 
		{
			if( pUser->m_dwAdamantineTime != 0 || pUser->m_dwBerserkerTime != 0 || pUser->m_dwMindShockTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;
		break;
	case 15: // Berserker
		if(bUser) 
		{
			if( pUser->m_dwAdamantineTime != 0 || pUser->m_dwMightyWeaponTime != 0 || pUser->m_dwMindShockTime != 0) return FALSE; // �̹� �ٸ� ȿ���� ������ ����ȵ�
		}
		if(bMon) return FALSE;
		break;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//	���̿��� ���� ����� �����Ѵ�.
//
void USER::SendPsiAttackResult(BYTE result, int tuid, BYTE psi)
{
	USER*		pUser;
	CBufferEx TempBuf;

	if(m_tIsOP == 1) return;					// ��� �϶��� ���̿��л��°� ���ϸ� �ȵȴ�. (������ Ǯ����...)

	pUser = NULL;

	// E-Body Skills
	if(psi == 23 || psi == 25 || psi == 26)
	{
		TempBuf.Add(ATTACK_RESULT);
		if(result != FAIL)
			TempBuf.Add((BYTE)0x06); // Ebody Attack
		else
			TempBuf.Add(result); // Ebody Attack
	}
	else
	{
		TempBuf.Add(PSI_ATTACK_RESULT);
		TempBuf.Add(result);
	}
	if(result == FAIL) 
	{
		Send(TempBuf, TempBuf.GetLength());
		return;
	}
	if(psi == 23  || psi == 25 || psi == 26)
	{
		;
	}
	else
	{
		TempBuf.Add(psi);				// Psionic sid
	}

	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add(tuid);

	if(psi == 23 || psi == 24 || psi == 25 || psi == 26)
	{
		if(tuid >= NPC_BAND)
		{
			CNpc *pNpc;
			pNpc = GetNpc(tuid - NPC_BAND);
			TempBuf.Add((short)pNpc->m_sHP);
			TempBuf.Add((short)pNpc->m_sMaxHP);
		}
		else if(tuid >= USER_BAND && tuid < NPC_BAND)
		{
			USER *pUser = NULL;
			pUser = GetUser(tuid - USER_BAND);
			TempBuf.Add((short)pUser->m_sHP);
			TempBuf.Add((short)pUser->m_sMagicMaxHP);
		}

	}

	// �ְ� �̺�Ʈ������ ����� ��û 02-11-1 
	if(m_curz == 61 || m_curz == 62 || m_curz == 63)
	{
		SendInsight(TempBuf, TempBuf.GetLength());
	}
	else
	{
		if(tuid >= USER_BAND && tuid < NPC_BAND)	// USER
			pUser = GetUser(tuid - USER_BAND);

		if( pUser != NULL )
		{
			if( ((pUser->m_curx < m_curx-16 && pUser->m_curx >= m_curx+16
				&& pUser->m_cury < m_cury-16 && pUser->m_cury >= m_cury+16)
				|| (( abs( m_curx - pUser->m_curx ) + abs( m_cury - pUser->m_cury ) ) > 16))
				&& pUser->m_curz == m_curz )
				pUser->Send(TempBuf, TempBuf.GetLength());
		}

		SendExactScreen(TempBuf, TempBuf.GetLength());	// 02-10-28 by Youn Gyu
	}
//	SendInsight(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////////
//	����ó��
//
void USER::Attack(TCHAR* pBuf)
{
#ifdef _ACTIVE_USER
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	if( m_UserItem[RIGHT_HAND].sSid == 669 || m_UserItem[RIGHT_HAND].sSid == 670 )		// �����տ� ä���� �������� �����ߴٸ�
	{
		Collect( pBuf );
		return;
	}

	USER*	pUser	= NULL;
	CNpc*	pNpc	= NULL;
	int		nTPosX	= 0;
	int		nTPosY	= 0;
	int		nAttackRange = 0;
	int		nDist	= 100;

	int		nDamage = 0;
	int		nDefense = 0;
//	double	nIronSkin = 0;
	int		nFinalDamage = 0;

	BYTE	tWeaponClass = 0;
	BYTE	tTargetClass = 0;

	BOOL	bCanUseSkill = FALSE;
	int		iRandom = 100;

	int		nHit	= 0;
	int		nAvoid	= 0;
//	int		nWeaponHit	= 0;

	BOOL	bIsHit	= FALSE;
	double	nVCHit	= 0;
	double	nCGuard	= 0;
	BOOL	bIsCritical = FALSE;

	int		nInitDamage = 0;
	
	short	sItemDefense[4] = {0, 0, 0, 0};

	BOOL	bSuccessSkill[SKILL_NUM] = {FALSE, FALSE, FALSE, FALSE, FALSE};
	BOOL	bTargetSkill[SKILL_NUM] = {FALSE, FALSE, FALSE, FALSE, FALSE};
	
	BYTE	tActionIndex = HIT_NONE;
	BYTE	tNpcClass = 0;

	short	sOldDuration = 0, sNewDuration = 0;
	short	sOldBullNum = 0;

	DWORD	dwCurrTick = GetTickCount();

	int		iFireErrDec = 0;
//	double	determine = 0;
//	int iAttackDex = 0, iDefenseDex = 0;
	int	determine = 0;
	int iDexHitRate = 0, iLevelHitRate = 0;
	int iMyDex = 0, iYourDex = 0;

	CByteArray	arSkillAction1, arSkillAction2;							// Skill Action Array

	DWORD dwExp = 0;
	int index = 0;
	int nTargetID = GetShort(pBuf, index);								// ȡ��ServerID

	if(nTargetID < USER_BAND || nTargetID >= INVALID_BAND) return;		// ���ServerID�Ǵ��� ����

	//�ж����ʹ��57����//
	bCanUseSkill = IsCanUseWeaponSkill(tWeaponClass);	

	if(bCanUseSkill)//�����57������Ч
	{
		IsSkillSuccess(bSuccessSkill, tWeaponClass);					//�鿴�����Ƿ�ɹ�
	}

	if(CheckAttackDelay(bSuccessSkill, tWeaponClass) == FALSE)			// Attack Delay Check
	{
//		SendAttackFail(ERR_SHORT_ATTACK_DELAY);
		return;
	}

	// 57�;� -------------------------------------------------------------//
	if(tWeaponClass != 255)	// �Ǽ��� �ƴϸ�
	{
		if(m_UserItem[RIGHT_HAND].sDuration <= 0)
		{
//			SendAttackFail(ERR_BROKEN);
			SendSystemMsg( IDS_USER_ITEM_DAMAGED, SYSTEM_NORMAL, TO_ME);
			return;
		}
	}

	// Target ���� �Ÿ���� ----------------------------------------------------//
	if(nTargetID >= USER_BAND && nTargetID < NPC_BAND)	//������������
	{
		pUser = GetUser(nTargetID - USER_BAND);				// User Pointer �� ��´�.
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;// �߸��� USER �̸� ����
		if(pUser->m_bLive == USER_DEAD || pUser->m_sHP == 0) return;	// Target User �� �̹� �׾������� ����
		if(pUser->m_bPShopOpen == TRUE) return;							// User has personal shop
		if(CheckBuddyMember(pUser->m_strUserID)) return;	// ���� ������̸� ����
		if(!IsPKZone(pUser)) { SendAttackMiss(nTargetID); return; }	// PK���� �ƴϹǷ� �ο������.
		
		if(pUser->m_dwNoDamageTime != 0)		
		{
			if((dwCurrTick - pUser->m_dwLastNoDamageTime) > pUser->m_dwNoDamageTime)		// �����ð��� Ǯ�� �ð��̸�
			{
				pUser->m_dwNoDamageTime = 0;
				pUser->m_dwLastNoDamageTime = GetTickCount();
			}
			else
			{
				pUser->m_dwNoDamageTime = pUser->m_dwNoDamageTime - (dwCurrTick - pUser->m_dwLastNoDamageTime);
				pUser->m_dwLastNoDamageTime = dwCurrTick;
				return;	
			}
		}

		nTPosX = pUser->m_curx;
		nTPosY = pUser->m_cury;
	}
	else if(nTargetID >= NPC_BAND)				//��������ǹ���
	{
		pNpc = GetNpc(nTargetID - NPC_BAND);	// NPC Point �� ��´�.
		if(pNpc == NULL) return;				// �߸��� NPC �̸� ����
		if(pNpc->m_NpcState == NPC_DEAD || pNpc->m_sHP <= 0)
		{
//			pNpc->Dead(); 
			//pNpc->m_Delay = pNpc->SendDead(m_pCom, 0);
			return;// NPC �� �̹� �׾� ������ ����
		}
		if(pNpc->m_byAX == 0 && pNpc->m_byAZ == 0 && pNpc->m_tNpcType == 0) return;			// ���ݴɷ��� ���� ����(���� ����)�� �Ϲ� ������ ���� �ʴ´�.

		nTPosX = pNpc->m_sCurX;
		nTPosY = pNpc->m_sCurY;
	}

	nAttackRange = GetAttackRange(m_UserItem[RIGHT_HAND].sSid);//�õ����
	nAttackRange += 1;	// �������� Ÿ�������Ҷ��� +1�� ���ش�.(�̵� �ӵ������� ���� ���� ���ؼ� ��������...)
	if(tWeaponClass == FIREARMS)
	{
		nAttackRange += m_DynamicUserData[MAGIC_RANGE_UP];//������� 
	}

	if(GetDistance(nTPosX, nTPosY, nAttackRange, &nDist) == FALSE)		// ���ݰ��ɰŸ� ���̸� ����
	{
		if(g_bDebug) SendSystemMsg(_T("����̫Զ��"), SYSTEM_NORMAL, TO_ME);
		return;
	}

	//�Ƿ�����������-------------------------------------//
	if(tWeaponClass == EDGED || tWeaponClass == FIREARMS)
	{
		if(m_UserItem[RIGHT_HAND].sBullNum <= 0)
		{
			if(tWeaponClass == EDGED) SendSystemMsg( IDS_USER_SWORD_PACK_EMPTY, SYSTEM_NORMAL, TO_ME);
			if(tWeaponClass == FIREARMS) SendSystemMsg( IDS_USER_BULLET_EMPTY, SYSTEM_NORMAL, TO_ME);
			return;
		}
		else SetDecBullNum(RIGHT_HAND, 1);//�����ɹ� ����������1
	}

	// ȡ���������-------------------------------------------------------//
	nHit = GetHitRate(m_UserItem[RIGHT_HAND].sSid, tWeaponClass);
	if(nHit < 0) nHit = 0;


	// ȸ�ǰ�/��������/������ ���/������ ���/���� ���ݽð� ����----------//
	if(nTargetID >= USER_BAND && nTargetID < NPC_BAND)	// USER
	{
		pUser = GetUser(nTargetID - USER_BAND);	// User Pointer �� ��´�.
		
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;// �߸��� �����̸� ����
		if(pUser->m_bLive == USER_DEAD || pUser->m_sHP == 0) return;	// Target User �� �̹� �׾������� ����

		// λ���ǲ�����һ��? �ܲ��ܴ�?
		if(!IsInSight(pUser->m_curx, pUser->m_cury, pUser->m_curz)) return;
		
		//ȡ�ñ�����һر�
		nAvoid = pUser->GetAvoid();
		
		// ���߿��� �Ǵ�
		iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5); //�پ缺�� �����ϱ� ����(1, 100)
		
//		iDefenseDex = pUser->m_sMagicDEX;
//		iAttackDex = m_sMagicDEX;	
//		determine = 200 * ((double)iAttackDex / (iAttackDex + iDefenseDex)) * ((double)m_sLevel / (m_sLevel + pUser->m_sLevel));

		if(m_dwDexUpTime) iMyDex = m_sMagicDEX + 10;
		else iMyDex = m_sMagicDEX;

		if(pUser->m_dwDexUpTime) iYourDex = pUser->m_sMagicDEX + 10;
		else iYourDex = pUser->m_sMagicDEX;

		iDexHitRate = (int)( 30.0 * ( (double)iMyDex/(iMyDex + iYourDex) ) + 15.0 );
		iLevelHitRate = (int)( 70.0 * ( (double)m_sLevel/(pUser->m_sLevel + m_sLevel) ) + 15.0 );

		determine = iDexHitRate + iLevelHitRate + nHit + m_Hit - (nAvoid+pUser->m_Avoid);

//		index = (int)(determine + nHit - nAvoid);

		if(determine < ATTACK_MIN) determine = ATTACK_MIN;			// �ּ�
		else if(determine > ATTACK_MAX) determine = ATTACK_MAX;		// �ִ�

		if(iRandom < determine)	bIsHit = TRUE;		// ����
		else bIsHit = FALSE;

		//û������...���͹�����Ч...
		if(bIsHit == FALSE)					
		{
			SendAttackMiss(nTargetID);
			return;
		}

		//����� ----------------------------------------------------------------//
		nDamage = GetNormalInitDamage(tWeaponClass, pUser, bIsCritical);//ȡ����ҵķ�.
		if(nDamage < 0) nDamage = 0;
		if(pUser->m_tAbnormalKind == ABNORMAL_BYTE_COLD) nDamage += 10;		// ������� �ñ� �̻��̸� ������ �߰�

		// ����������
		nFinalDamage = GetFinalDamage(pUser, nDamage, tWeaponClass,bIsCritical);
		nFinalDamage = nFinalDamage +m_DynamicMagicItem[7];
		nFinalDamage=nFinalDamage-(pUser->m_DynamicMagicItem[5]+pUser->m_DynamicMagicItem[6]);
		if(nFinalDamage < 0) nFinalDamage = 15;		// ��� �ڵ�

		sNewDuration = pUser->SendDamagedItem(nFinalDamage); // ���� ������ ���Ҹ� ������. 

		// ������ ������/��Ƽ���� ����
		if(tWeaponClass != 255)	// �Ǽ��� �ƴѰ��
		{
			// ������
			sOldDuration = m_UserItem[RIGHT_HAND].sDuration;
			sNewDuration = (int)myrand(1,10);
			if(tWeaponClass != FIREARMS) 
				SendDuration(RIGHT_HAND, sNewDuration);
			//if(tWeaponClass != FIREARMS) m_UserItem[RIGHT_HAND].sDuration -= sNewDuration;
			else
			{
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if(m_UserItem[RIGHT_HAND].sSid >= 0 && m_UserItem[RIGHT_HAND].sSid < g_arItemTable.GetSize())
				{
					if(iRandom < g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_byErrorRate)
					{
						//iFireErrDec = 10 - GetBreakDec();
						iFireErrDec=4;
						if(iFireErrDec < 0) iFireErrDec = 0;

						//m_UserItem[RIGHT_HAND].sDuration -= iFireErrDec;
						SendDuration(RIGHT_HAND, iFireErrDec);
					}
				}
			}

//			if(m_UserItem[RIGHT_HAND].sDuration < 0) m_UserItem[RIGHT_HAND].sDuration = 0;
//			if(sOldDuration != m_UserItem[RIGHT_HAND].sDuration) SendDuration(RIGHT_HAND);
		}

		BOOL bIsLegal = IsLegalDefence_isdead(pUser);		// �ϴ� �����ϸ� ����������� �Ǵ�//yskang 0.7
		pUser->SetDamage(nFinalDamage);
		if(m_ShowHP==1){
			CString strMsg;
			int iMaxHP=pUser->m_sMaxHP;
			if(m_sMagicMaxHP > m_sMaxHP) iMaxHP = pUser->m_sMagicMaxHP;
			strMsg.Format("%s(%d) HP= %d \\ %d",pUser->m_strUserID,pUser->m_uid + USER_BAND,pUser->m_sHP,iMaxHP);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			
		}
//		if(pUser->m_bLive == FALSE)
		if(pUser->m_lDeadUsed == 1)
		{
			CString strMsg = _T("");

			strMsg.Format( IDS_USER_ATTACK_FROM, m_strUserID);

			int tempRank = m_sCityRank + CITY_RANK_INTERVAL;
			if(bIsLegal == FALSE)//yskang 0.7
				IsChangeCityRank(m_sCityRank, pUser);
			pUser->GetLevelDownExp(USER_PK, tempRank, FALSE, m_strUserID);					// ����ġ�� �׿� ��ȭ���� �ݿ��Ѵ�.
			pUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}

		// ���� ��� ���� ------------------------------------------------------------//
		//yskang 0.3 SendAttackSuccess(nTargetID, arSkillAction1, arSkillAction2, pUser->m_sHP, pUser->m_sMagicMaxHP);
		SendAttackSuccess(nTargetID,bIsCritical, pUser->m_sHP, pUser->m_sMagicMaxHP);//yskang 0.3
		m_dwLastAttackTime = GetTickCount();

/*		//yskang 0.4 ��� �α� ����ϱ�
		TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
		sprintf(strOP,"PKINFO:ACCOUNT%s : CharID%s",pUser->m_strAccount, pUser->m_strUserID);
		WriteOpratorLog(strOP,PKINFO_LOG);
*/
		return;
	}

	if(nTargetID >= NPC_BAND)				// NPC
	{
		pNpc = GetNpc(nTargetID - NPC_BAND);		// NPC Point �� ��´�.
		
		if(pNpc == NULL) return;					// �߸��� NPC �̸� ����
		if(pNpc->m_NpcState == NPC_DEAD) return;
		if(pNpc->m_sHP == 0) return;

		if(pNpc->m_tNpcType == NPCTYPE_GUARD)					// ����̸� Ÿ���� �ش� ������
		{
			pNpc->m_Target.id = m_uid + USER_BAND;
			pNpc->m_Target.x = m_curx;
			pNpc->m_Target.y = m_cury;
			pNpc->m_Target.failCount = 0;
			pNpc->Attack(m_pCom);

			return;
		}

		// ���� �� ȭ�鿡 �ִ��� �Ǵ�
		if(!IsInSight(pNpc->m_sCurX, pNpc->m_sCurY, pNpc->m_sCurZ)) return;

		if(m_dwDexUpTime) iMyDex = m_sMagicDEX + 10;
		else iMyDex = m_sMagicDEX;

		iDexHitRate = (int)( 30.0 * ( (double)iMyDex/(iMyDex + pNpc->m_sDEX) ) + 15.0 );
//		iDexHitRate = (int)( 30.0 * ( (double)m_sMagicDEX/(m_sMagicDEX + pNpc->m_sDEX) ) + 15.0 );
		iLevelHitRate = (int)( 70.0 * ( (double)m_sLevel/(pNpc->m_byClassLevel + m_sLevel) ) + 15.0 );

		determine = iDexHitRate + iLevelHitRate + nHit + m_Hit;

		if(determine < ATTACK_MIN) determine = ATTACK_MIN;			// �ּ�
		else if(determine > ATTACK_MAX) determine = ATTACK_MAX;		// �ִ�

		iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5); //�پ缺�� �����ϱ� ����(1, 100)

		if(iRandom < determine)	bIsHit = TRUE;		// ����
		else bIsHit = FALSE;

		pUser = GetUser(m_uid);

		if(pNpc->m_tNpcType == NPCTYPE_GUILD_GUARD)	pNpc->ChangeTarget(pUser, m_pCom);

		// ���� �̽�
		if(bIsHit == FALSE)					
		{
			SendAttackMiss(nTargetID);
			return;
		}
		// NPC �� 
		nDefense = pNpc->GetDefense();

		// �����̸� //Damage ó�� ----------------------------------------------------------------//
		//yskang 0.3 nDamage = GetNormalInitDamage(tWeaponClass, NULL);// �ʱ� �����
		nDamage = GetNormalInitDamage(tWeaponClass, NULL,bIsCritical);//yskang 0.3 �ʱ� �����
		if(nDamage < 0) nDamage = 0;

		// �����̸� //Damage ó�� ----------------------------------------------------------------//
		if(bIsHit == TRUE)
		{
			nVCHit = 0.0;
			nVCHit = GetCriticalHit(tWeaponClass,0);
			if(nVCHit > 0.0) bIsCritical = TRUE; //yskang 0.3

			nFinalDamage = (int)((double)nDamage - ((double)nDefense * nVCHit));
			if(m_byClass == FIREARMS) // �ѱ��� ������ ���� - �ӽ��ڵ�  
			{
				double dMin = (double)myrand(90, 100) / 100;
				nFinalDamage = (int)((double)nFinalDamage * dMin);
			}
			
			if(nFinalDamage < 0) nFinalDamage = 15;
		}
		
		// ������ ������/��Ƽ���� ����
		if(tWeaponClass != 255)	// �Ǽ��� �ƴѰ��
		{
			// ������
			sNewDuration = (int)myrand(1,10);
			sOldDuration = m_UserItem[RIGHT_HAND].sDuration;

			if(tWeaponClass != FIREARMS) 
				SendDuration(RIGHT_HAND, sNewDuration);
			//if(tWeaponClass != FIREARMS) m_UserItem[RIGHT_HAND].sDuration -= sNewDuration;
			else
			{
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if(m_UserItem[RIGHT_HAND].sSid >= 0 && m_UserItem[RIGHT_HAND].sSid < g_arItemTable.GetSize())
				{
					if(iRandom < g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_byErrorRate)
					{
						//iFireErrDec = 10 - GetBreakDec();
						iFireErrDec=4;
						if(iFireErrDec < 0) iFireErrDec = 0;

						//m_UserItem[RIGHT_HAND].sDuration -= iFireErrDec;
						SendDuration(RIGHT_HAND, iFireErrDec);
					}
				}
			}

//			if(m_UserItem[RIGHT_HAND].sDuration < 0) m_UserItem[RIGHT_HAND].sDuration = 0;
//			if(sOldDuration != m_UserItem[RIGHT_HAND].sDuration) SendDuration(RIGHT_HAND);
		}

		// Calculate Target HP -------------------------------------------------------//
		short sOldNpcHP = pNpc->m_sHP;
//���ͶԹ����˺���ֵ
		
	//	CString strMsg;
	//	strMsg.Format("������������ %d,���� %s �����˺��� %d",nDamage,pNpc->m_strName,nFinalDamage);
	//	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		nFinalDamage = nFinalDamage +m_DynamicMagicItem[7];
//		if(pNpc->SetDamage(nFinalDamage, m_strUserID, m_uid + USER_BAND, m_pCom) == FALSE)
		if(pNpc->SetDamage(nFinalDamage, m_uid + USER_BAND, m_pCom) == FALSE)
		{
			pNpc->SendExpToUserList(m_pCom); // ����ġ �й�!!
			pNpc->SendDead(m_pCom);

			if(m_tGuildHouseWar == GUILD_WARRING && pNpc->m_NpcVirtualState == NPC_WAIT)
			{
				CheckGuildHouseWarEnd();
			}
			else
			{
				if(m_tQuestWar == GUILD_WARRING) CheckQuestEventZoneWarEnd();
				int diffLevel = abs(m_sLevel - pNpc->m_byClassLevel);
				if(diffLevel < 40)
				{
					m_iCityValue=m_iCityValue+250;
					GetCityRank();
					CheckMaxValue(m_dwXP, 1);		// ���� �������� 1 ����!	
					SendXP();
				}
			}
		}
		if(m_ShowHP==1){
			CString strMsg;
			strMsg.Format("%s(%d) HP= %d \\ %d",pNpc->m_strName,pNpc->m_sNid + NPC_BAND,pNpc->m_sHP,pNpc->m_sMaxHP);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			
		}
		// ���� ��� ����
		SendAttackSuccess(nTargetID, bIsCritical, pNpc->m_sHP, pNpc->m_sMaxHP);//yskang 0.3

		m_dwLastAttackTime = GetTickCount();
	}
}


///////////////////////////////////////////////////////////////////////////////////
//	����� ���� ������ ������ ��ȭ�� ����
//	
int USER::SendDamagedItem(int totalDamage)
{
	int iRandom = XdY(1, 1000);

	int iDur = (int)((double)totalDamage * 0.1/10 + 0.5);

	if(iRandom < 200) 
	{ 
//		m_UserItem[HEAD].sDuration -= iDur; 
//		if(m_UserItem[HEAD].sDuration < 0) m_UserItem[HEAD].sDuration = 0; 
		SendDuration(HEAD, iDur);
	}
	else if(iRandom < 450)
	{ 
//		m_UserItem[PANTS].sDuration -= iDur; 
//		if(m_UserItem[PANTS].sDuration < 0) m_UserItem[PANTS].sDuration = 0; 
		SendDuration(PANTS, iDur);
	}
	else if(iRandom < 850)
	{ 
//		m_UserItem[BODY].sDuration -= iDur; 
//		if(m_UserItem[BODY].sDuration < 0) m_UserItem[BODY].sDuration = 0; 
		SendDuration(BODY, iDur);
	}
	else
	{ 
//		m_UserItem[SHOES].sDuration -= iDur; 
//		if(m_UserItem[SHOES].sDuration < 0) m_UserItem[SHOES].sDuration = 0; 
		SendDuration(SHOES, iDur);
	}

	return iDur;
}


///////////////////////////////////////////////////////////////////////////////////
//	�ݰ��� ������ �ݰ�����
//	
int USER::GetCounterAttack()
{
	BYTE	tWeaponClass = 255;
	int		iLevel = 0;
	int		CounterAttack = 0;

/*	if(IsCanUseWeaponSkill(tWeaponClass) == FALSE)	// ���� �ڽ��� �����տ� �� ������ ��ų�� ����� �� ������ �� �Ǽ��̸�
	{
		return CounterAttack;
	}

	int tClass = tWeaponClass * SKILL_NUM; 

	// �ڱ� ���� ��ų��... 
	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		if(i == MAGIC_BACK_ATTACK) // �ݰ�
		{
			iLevel = m_UserSkill[i].tLevel;		// ���� ��ų ���� 
			if(iLevel <= 0) return 0;
												// �����ۿ� ���� ��ų ���� ����
			iLevel += m_DynamicUserData[m_DynamicSkillInfo[i]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];

			if(i >= g_arSkillTable.GetSize()) return 0;
			if(iLevel >= g_arSkillTable[i]->m_arSuccess.GetSize()) return 0;

			CounterAttack += g_arSkillTable[i]->m_arSuccess.GetAt(iLevel);
		}
	}

	CounterAttack += m_DynamicUserData[MAGIC_BACK_ATTACK];// �����ۿ����� ��ü ȸ���� ����
	
	CounterAttack = CounterAttack/100;
*/
	return (int)CounterAttack;	
}

///////////////////////////////////////////////////////////////////////////////////
//	���� PK ����� ���� ���� �������̸� ����
//	
BOOL USER::CheckBuddyMember(TCHAR *pID)
{
	int nLen = 0;
	nLen = strlen(pID);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return FALSE;

	for(int i = 0; i < MAX_BUDDY_USER_NUM; i++)
	{
		if(m_MyBuddy[i].uid < USER_BAND) continue;

		nLen = 0;
		nLen = strlen(m_MyBuddy[i].m_strUserID);
		if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) continue;

		if(strcmp(m_MyBuddy[i].m_strUserID, pID) == 0) return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////
//	�ӽ������� ������ ����ڰ� ����ġ�� ���´�. 01_5_9
//	iNpcExp : ���ڰ����� ����ġ�� ������.
void USER::GetExp(DWORD iNpcExp)					
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if( m_iDisplayType == 6) return; //yskang 0.5
#endif

	int iRate = 0;
	short sTempLevel = 0;
	short sLevel = m_sLevel;
	DWORD dwExp = m_dwExp;
	DWORD dwExpNext = m_dwExpNext;

	if(iNpcExp <= 0) return;

	GetCityRank();								// �ùε���� ���ϸ� ������ �˸���.

	if(m_sLevel >= MAX_LEVEL) 
	{ 
		m_sLevel = MAX_LEVEL;
		//CString strMsg = _T("");
		//strMsg.Format( IDS_USER_HIGHEST_LEVEL, m_strUserID);

		//UpdateHighEventLevelUser(sLevel);
		//SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ANNOUNCE, TO_ALL);
		return; 
	}// �������� 120
															
	CheckMaxValue((DWORD &)dwExp, (DWORD)iNpcExp);// Overflow���� �˻�

	while(dwExp >= dwExpNext)					// Level UP
	{
		if(sLevel == MAX_LEVEL) { dwExp = dwExpNext; break; }
		if(sLevel < MAX_LEVEL)
		{
			sLevel += 1;		
			if(sLevel == 100)
			{
//				CString strMsg = _T("");
//				strMsg.Format( IDS_USER_HIGHEST_LEVEL, m_strUserID);

				UpdateHighEventLevelUser(sLevel);
//				SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ANNOUNCE, TO_ALL);
			}
		}

		dwExp = dwExp - dwExpNext;
		dwExpNext = GetNextLevel(sLevel);
	} 

	if((sLevel - m_sLevel) > 0)					// �������� ���
	{
		BYTE tType = 2;
		sTempLevel = m_sLevel;

		m_dwExp = dwExp;						// ���� ����ġ
		m_dwExpNext = dwExpNext;				// ���� ���� �䱸����ġ
		m_sLevel = sLevel;						// ������

		while(sTempLevel < sLevel)				// ����ġ�� �ش�.
		{
			sTempLevel++;
			CheckMaxValue((short &)m_sPA, (short)1);		// ������ ���������� 1�� ����.

			if( sTempLevel < 100 )
			{
				if((sTempLevel - 1) >= g_arLevelUpTable.GetSize()) continue;

				if(g_arLevelUpTable[sTempLevel - 1]->m_tBasicUp)
				{
					CheckMaxValue((short &)m_sSkillPoint, (short)1);
				}
			}
			else	// 100�� �̻�
			{
				ResetOver100LevelSkill( sTempLevel );
			}
		}

		// ������ �ϰ�� ��ġ�� �����ǹǷ� �ٽ� ����Ѵ�.
		SetUserToMagicUser();  //�⺻��ġ���� ���������� �������� �ٽ� ����Ѵ�.

		m_sHP = m_sMagicMaxHP;					// ��ä���.
		m_sPP = m_sMagicMaxPP;
		m_sSP = m_sMagicMaxSP;

//		SendHP();								// �������ϰ�� ��ä���� ������. (HP, PP)
//		SendPP();
//		SendSP(TRUE);
		SendExp(tType);							// �������ϰ��		
		SendUserStatusSkill();
	}

#ifdef _ACTIVE_USER

		//if(m_iDisplayType == 6 && m_sLevel > 25)//yskang 0.5
		if(m_iDisplayType == 6) //yskang 0.5
		{			
			CString strMsg = _T("");
			strMsg.Format("�������� �����Ͻ� ü���� �������� �Ϻ� ��ɻ� ������ �ֽ��ϴ�.");
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);
		}
#endif
	
	m_dwExp = dwExp;							// ��� ����ġ��...

	SendExp();

	UpdateUserData();
}

//////////////////////////////////////////////////////////////////////////////////
// ���� �� �����϶� �߰�����ġ�� ��� ��Ƽ������ �߰� ����ġ�� �������ش�
// zinee 02/12/09
void USER::GetExpBuddy(int iNpcExp )
{
	int		i;
	int		nBuddyUsers;
	int		nBuddyLevelSum;
	DWORD	dwBonusExp;
	double	fLevelAdvantage;

	USER*		pUser;
	BuddyList	NearBuddyUser[MAX_BUDDY_USER_NUM];


	if( !m_bNowBuddy ) GetExp( iNpcExp );

	nBuddyUsers = 0;
	nBuddyLevelSum = 0;
	memset( NearBuddyUser, -1, sizeof(BuddyList)*MAX_BUDDY_USER_NUM );

	// ���� �ѹ������� ���� ��ó(2ȭ��)�� �ִ� ������������Ʈ, ������ ���ϱ�.
	for(i = 0; i < MAX_BUDDY_USER_NUM; i++)
	{
		if(m_MyBuddy[i].uid != -1 )
		{
			if( m_MyBuddy[i].uid != (m_uid + USER_BAND) )  // �� �ڽ��� �ƴ� ��Ƽ���̶��..
			{
				pUser = GetUser( m_MyBuddy[i].m_strUserID );
				if( pUser != NULL ) 
				{
					//���������� �������� �ְ� ��ó(2ȭ��)�� ������ �������ġ����Ʈ�� �߰�.
					if( m_curz == pUser->m_curz && GetDistance( pUser->m_curx, pUser->m_cury, (SCREEN_X + SCREEN_Y) ) )
					{
						NearBuddyUser[i].uid = m_MyBuddy[i].uid;
						strcpy( NearBuddyUser[i].m_strUserID, m_MyBuddy[i].m_strUserID );

						nBuddyLevelSum += pUser->m_sLevel;
					}
				}

			}

			nBuddyUsers++;
		}
	}

	// ����� ���� ���� ���ʽ� ����ġ. 2��:10%, 3��:15%, 4��:20%
	dwBonusExp = (DWORD)((double)( iNpcExp * (nBuddyUsers*5) )/100 + 0.5);

	if( dwBonusExp != 0 && nBuddyLevelSum != 0 )	
	for( i=0; i< MAX_BUDDY_USER_NUM; i++ )
	{
		if( NearBuddyUser[i].uid != -1 ) 
		{
			pUser = GetUser( NearBuddyUser[i].m_strUserID );

			if( pUser != NULL )
			{
				// �й�Ǵ� ����ġ = (�߰� ����ġ * �� ������� ���� /����� ������ ����) 
				//					* �� ������� ���� / ����ġ�� ���� ������� ���� ����
				if(pUser->m_bLive == USER_LIVE)
				{
					fLevelAdvantage = (double)pUser->m_sLevel/(double)m_sLevel;
					if( fLevelAdvantage > 1 ) fLevelAdvantage = 1;

/*					TRACE( "NpcExp:%d, bonus: %d, Adv: %f, Exp: %d, fExp: %d\n", iNpcExp, dwBonusExp, fLevelAdvantage,
					(int)(dwBonusExp*fLevelAdvantage*pUser->m_sLevel/nBuddyLevelSum),
					(int)((double)dwBonusExp*fLevelAdvantage*(double)pUser->m_sLevel/(double)nBuddyLevelSum) );							
*/


					pUser->GetExp( (int)(dwBonusExp*fLevelAdvantage*pUser->m_sLevel/nBuddyLevelSum+0.5) );
				}
			}
		}
	}

	GetExp( iNpcExp+dwBonusExp );

}

////////////////////////////////////////////////////////////////////////////////////
// ������ HI �ΰ� ��������ΰ��� ���� ����ġ�� ����Ѵ�.
// zinee 02/12/09
void USER::GetExpCommon( int iNpcExp )
{
	int exp;

	//ȫ������1.5
	//iNpcExp=iNpcExp*1.5;
	// ������ Hi Exp ���¿��� �˻�. TRUE : 2�����ġ
	//ţţ
   if(m_sLevel<= 100){
		iNpcExp=iNpcExp*32;
	 }else if(m_sLevel<120){
		iNpcExp=iNpcExp*16;
	 }else if(m_sLevel<140){
		iNpcExp=iNpcExp*8;
	 }else if(m_sLevel<150){
	 	iNpcExp=iNpcExp*4;  
	 }else if(m_sLevel<160){
	 	iNpcExp=iNpcExp*2;  
	 }else if(m_sLevel<165){
	 	iNpcExp=iNpcExp;  
	 }else if(m_sLevel<170){
	 	iNpcExp=iNpcExp/2;  
	 }else{
		iNpcExp=iNpcExp/10;
 	 }

	 if(m_tIsOP == 2)
	 {
		 iNpcExp = iNpcExp * 1.5;
	 }else if(m_tIsOP == 3)
	 {
		 iNpcExp = iNpcExp * 3;
	 }
	if( m_dwHiExpTime > 0 ) exp = iNpcExp*2;
	else exp = iNpcExp;
	
	if(m_bNowBuddy)	GetExpBuddy( exp );
	else GetExp( exp );
}


///////////////////////////////////////////////////////////////////////////////////
//  ����ġ�� ������. (�������϶� ���� ��ġ�� �ش�)
//
void USER::SendExp(int tType)
{
	CBufferEx TempBuf;

	TempBuf.Add(LEVEL_UP);
	TempBuf.Add((BYTE)tType);
	TempBuf.Add(m_dwExp);

	if(tType == 2)								// ������
	{		
		TempBuf.Add(m_dwExpNext);
		TempBuf.Add(m_sLevel);
		TempBuf.Add(m_sSkillPoint);
		TempBuf.Add(m_sPA);
	}

	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////////////
//	������ PA, MA, Skill Level�� ��ġ ������ ó���Ѵ�.
//
void USER::UserStatusSkillUp(TCHAR *pBuf)
{
	int index = 0;
	int iPlusMinus = 1;
	BYTE tType = GetByte(pBuf, index);			// PA, MA �Ǵ� Skill Level���� �Ǵ�
	BYTE tKinds = GetByte(pBuf, index);			// �� ������ �Ǵ�

	if(tType >= 4) return;
	if(tKinds < 0 || tKinds >= TOTAL_SKILL_NUM) return;
	
	int iNum = 0;

	CBufferEx TempBuf;

	if(tType == PA_MA_STATUS) 
	{
		if(m_sPA <= 0) return;

		switch(tKinds)
		{
		case USER_STR:
			iNum = m_sSTR;
			iNum += iPlusMinus;														// ���ϰų� ���ų�
			if(iNum <= 0 || iNum > 100) return;				// 0~100 ���̸�
			m_sSTR = iNum; m_sPA -= 1;
			break;

		case USER_CON:
			iNum = m_sCON;
			iNum += iPlusMinus;
			if(iNum <= 0 || iNum > 100) return;
			m_sCON = iNum; m_sPA -= 1;
			break;

		case USER_DEX:
			iNum = m_sDEX;
			iNum += iPlusMinus;
			if(iNum <= 0 || iNum > 100) return;
			m_sDEX = iNum; m_sPA -= 1;
			break;

		case USER_VOL:
			iNum = m_sVOL;
			iNum += iPlusMinus;
			if(iNum <= 0 || iNum > 100) return;
			m_sVOL = iNum; m_sPA -= 1;
			break;

		case USER_WIS:
			iNum = m_sWIS;
			iNum += iPlusMinus;
			if(iNum <= 0 || iNum > 100) return;
			m_sWIS = iNum; m_sPA -= 1;
			break;
		}

		SetUserToMagicUser();		// �⺻�ɷ°��� �������Ƿ� �ٽ� ����
									// �⺻�ɷ�ġ �й踦 ���
	}
	else if(tType == SKILL_STATUS)
	{
		if(CheckSkillClass(tKinds) != m_byClass) return;
		if(m_sSkillPoint <= 0 || m_sSkillPoint > 500) return;// ����Ʈ�� 1�̻�
		if(m_UserSkill[tKinds].tLevel >= 20) return;

		m_UserSkill[tKinds].tLevel += 1;
		m_sSkillPoint -= 1;
	}else if (tType == 0x3)
	{
		if(m_sLevel <130)
			return;
		if(tKinds/3!=m_byClass)
			return;
		if(m_sSkillPoint_ <= 0 || m_sSkillPoint_ > 500) 
			return;
		if(m_UserSkill_[tKinds].tLevel >= 20) return;
		m_UserSkill_[tKinds].tLevel += 1;
		m_sSkillPoint_ -= 1;
		CheckMagicItemMove();
		UpdateUserData();
		return ;
	}
	else return;
	SendUserStatusSkill();
}


///////////////////////////////////////////////////////////////////////////////////
//	������ ��ġ ��ȭ���� �˸���.
//
void USER::SendUserStatusSkill()
{
	CBufferEx TempBuf;
	BYTE tWeaponClass = 0;

	short	sTemp = 0;
	short	sMaxDamage = 0, sMaxDefense = 0;

	TempBuf.Add(USER_STATUS_SKILL_UP_RESULT);

	IsCanUseWeaponSkill(tWeaponClass);
	sMaxDamage = GetMaxDamage(tWeaponClass);
	
	sTemp = (short)GetIronSkill();
	sMaxDefense = sTemp + GetDefense(); 

	//�Ǳ乥��+10% �ܱ�+15%��
	//if(m_iSkin==1) sMaxDamags=sMaxDamage+(int)(sMaxDamage*0.1);
	//if(m_iSkin==2) sMaxDefense=sMaxDefense+(int)(sMaxDefense*0.15);

	TempBuf.Add(m_sPA);
	TempBuf.Add(m_sMagicSTR);
	TempBuf.Add(m_sMagicCON);
	TempBuf.Add(m_sMagicDEX);
	TempBuf.Add(m_sMagicVOL);
	TempBuf.Add(m_sMagicWIS);

	if(m_sHP > m_sMagicMaxHP) m_sHP = m_sMagicMaxHP;
	if(m_sPP > m_sMagicMaxPP) m_sPP = m_sMagicMaxPP;
	if(m_sSP > m_sMagicMaxSP) m_sSP = m_sMagicMaxSP;

	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sPP);
	TempBuf.Add(m_sSP);

	TempBuf.Add(m_sMagicMaxHP);
	TempBuf.Add(m_sMagicMaxPP);
	TempBuf.Add(m_sMagicMaxSP);

	TempBuf.Add(sMaxDamage);	// �ִ� ���ݷ�
	TempBuf.Add(sMaxDefense);	// �ִ� ����

	TempBuf.Add(m_sSkillPoint);

	TempBuf.Add(m_byClass);
	
	for(int i = m_byClass * SKILL_NUM; i < m_byClass * SKILL_NUM + SKILL_NUM; i++)
	{
		TempBuf.Add(m_UserSkill[i].sSid);
		TempBuf.Add(m_UserSkill[i].tLevel);
	}
	TempBuf.Add(m_sSkillPoint_);
	for(int i=m_byClass*3;i<m_byClass*3+3;i++){
		TempBuf.Add(m_UserSkill_[i].sSid);
		TempBuf.Add(m_UserSkill_[i].tLevel);
	}
	short spellattack=GetUserSpellAttack();
	short spelldefence=GetUserSpellDefence();
	TempBuf.Add(spellattack);
	TempBuf.Add(spelldefence);

	
	GetRecoverySpeed();						// �������̵�, ������������ �ɷ�ġ������ ȸ���ӵ��� ������.
	Send(TempBuf, TempBuf.GetLength());
//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
}
//////////////////////////////////
//��������Լ���ħ������
/////////////////////////////////
short USER::GetUserSpellAttack()
{
	int iPsiAttackUp =0 ;
	short sBasic	= 0;


//	if(m_sLevel >= ADD_USER_LEVEL) 
//		sBasic += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sDamage;
	

	if(m_UserItem[RIGHT_HAND].tIQ != UNIQUE_ITEM)	
	{
		if(m_UserItem[RIGHT_HAND].sSid >= 0 && m_UserItem[RIGHT_HAND].sSid < g_arItemTable.GetSize())
		{
			if(g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_byClass == STAFF_ITEM)
			{
				int iUpCount = m_UserItem[RIGHT_HAND].tMagic[ITEM_UPGRADE_COUNT];
				if(iUpCount > 0 && iUpCount <= MAX_ITEM_UPGRADE_COUNT)
				{
					iPsiAttackUp = (iUpCount * ATTACK_UPGRADE_PSI_BAND);
				}
			}
		}
	}

	iPsiAttackUp +=sBasic+ m_DynamicUserData[MAGIC_PSI_ATTACK_UP] + (int)((double)m_DynamicEBodyData[EBODY_VOL_TO_PSI_DAMAGE] / 100 * (double)m_sMagicVOL)+m_sMagicVOL/2+m_sLevel;
	iPsiAttackUp += (int)((double)iPsiAttackUp * (double)m_DynamicEBodyData[EBODY_PSI_ATTACK_UP] / 100);
	if(m_sLevel>=70)
		iPsiAttackUp += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sDamage;
	return iPsiAttackUp;
}
//////////////////////////////////
//��������Լ���ħ������
//////////////////////////////////
short USER::GetUserSpellDefence()
{
	int sBasic=m_sMagicVOL;
	if(m_UserItem[0].tIQ==12)//ñ��
		sBasic+=m_UserItem[0].tMagic[ITEM_UPGRADE_COUNT]*3;
	if(m_UserItem[1].tIQ==12)//�·�
		sBasic+=m_UserItem[1].tMagic[ITEM_UPGRADE_COUNT]*3;
	if(m_UserItem[8].tIQ==12)//����
		sBasic+=m_UserItem[8].tMagic[ITEM_UPGRADE_COUNT]*3;
	if(m_UserItem[9].tIQ==12)//Ь��
		sBasic+=m_UserItem[9].tMagic[ITEM_UPGRADE_COUNT]*3;
	

int dItemResist =sBasic+ m_DynamicUserData[MAGIC_PSI_RESIST_UP] + (int)((double)m_DynamicEBodyData[EBODY_VOL_TO_PSI_RESIST] / 100 * (double)m_sMagicVOL);
    dItemResist+=(int)(dItemResist*(double)m_DynamicEBodyData[EBODY_PSI_RESIST_UP] / 100);//��е���ӵ�ħ��
	return dItemResist;
}

///////////////////////////////////////////////////////////////////////////////////
//	�ش� �迭�� ��ų���� �Ǵ�.
//
int USER::CheckSkillClass(short sSid)
{
	if(sSid >= 0 && sSid < SKILL_NUM) return BRAWL;
	else if(sSid < (2 * SKILL_NUM)) return STAFF;
	else if(sSid < (3 * SKILL_NUM)) return EDGED;
	else if(sSid < TOTAL_SKILL_NUM) return FIREARMS;
	else return -1;
}


//////////////////////////////////////////////////////////////////////////////////
//	���ݽ��� ��Ŷ�� ������.
//
void USER::SendAttackFail(BYTE tErrorCode)
{
	CBufferEx TempBuf;
	
	TempBuf.Add(ATTACK_RESULT);
	TempBuf.Add(ATTACK_FAIL);
	TempBuf.Add(tErrorCode);
	
	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	���ݹ̽� ��Ŷ�� ������.
//
void USER::SendAttackMiss(int tuid)
{
	CBufferEx TempBuf;
	
	TempBuf.Add(ATTACK_RESULT);
	TempBuf.Add(ATTACK_MISS);
	TempBuf.Add((int)(m_uid + USER_BAND));
	TempBuf.Add(tuid);

//	SendInsight(TempBuf, TempBuf.GetLength());
	SendExactScreen(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////////////
//	���ݼ��� ��Ŷ�� ������.
//
void USER::SendAttackSuccess(int tuid, BOOL bIsCritical, short sHP, short sMaxHP)//yskang 0.3 //void USER::SendAttackSuccess(int tuid, CByteArray& arAction1, CByteArray& arAction2, short sHP, short sMaxHP)
{
	CBufferEx TempBuf;
//	CByteArray arAction1;
//	CByteArray arAction2;
	
	TempBuf.Add(ATTACK_RESULT);
	//---------------------------------------------------------------
	//yskang 0.3 ��Ŷ�� ���̱� ���Ͽ� ��ų�׼��� �ܼ��ϰ� ����
	//�Լ� ���� ��ü�� �����ؾ� �ұ�?
	//---------------------------------------------------------------
	if(bIsCritical == FALSE) //ũ��Ƽ�� ������ �ƴϳ�
	{
		TempBuf.Add(ATTACK_SUCCESS);//���� ����
	}
	else
	{
		TempBuf.Add(CRITICAL_ATTACK_SUCCESS); //ũ��Ƽ�� ����
	}
	TempBuf.Add((int)(m_uid + USER_BAND));
	TempBuf.Add(tuid);
	//------------------------------------------------------------------

/*	BYTE tAction1 = (BYTE)arAction1.GetSize();
	BYTE tAction2 = (BYTE)arAction2.GetSize();
	int i = 0;

	TempBuf.Add(tAction1);
	if(tAction1 > 0)
	{
		for(i = 0; i < arAction1.GetSize(); i++)
		{
			TempBuf.Add(arAction1[i]);
		}
	}
	TempBuf.Add(tAction2);
	if(tAction2 > 0)
	{
		for(i = 0; i < arAction2.GetSize(); i++)
		{
			TempBuf.Add(arAction2[i]);
		}
	}
*/	
	TempBuf.Add((short)sHP);
	TempBuf.Add((short)sMaxHP);

	// �ְ� �̺�Ʈ������ ����� ��û 02-11-1 
	if(m_curz == 61 || m_curz == 62 || m_curz == 63) SendInsight(TempBuf, TempBuf.GetLength());
	else SendExactScreen(TempBuf, TempBuf.GetLength());	// ��Ŷ ���Ϸ��� ���Ƽ� ��ģ���� 02-10-28 by Youn Gyu
//	SendInsight(TempBuf, TempBuf.GetLength());
}


///////////////////////////////////////////////////////////////////////////////////
//	���� ������ ������ ���ÿ��� �츰��
//
BOOL USER::LiveCity(int x, int y, BYTE type)
{
//	CPoint ptNew;
//	CPoint pt = FindNearAvailablePoint_S(x, y);
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;
	if(m_curx < 0 || m_cury < 0 || m_curx >= g_zone[m_ZoneIndex]->m_sizeMap.cx || m_cury >= g_zone[m_ZoneIndex]->m_sizeMap.cy) return FALSE;
//	if(pt.x != -1 && pt.y != -1) 
//	{
//		if(!SetUid(pt.x, pt.y, m_uid + USER_BAND)) return FALSE;
		long lUid = m_uid + USER_BAND;

		if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, (long)0, (long)lUid) == (long)lUid)
		{
//			m_curx = pt.x;
//			m_cury = pt.y;
			m_curx = x;
			m_cury = y;

			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, lUid);
			SightRecalc();
			return TRUE;
		}
//	}
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////
//	�׾��� ������ �츰��.
//
void USER::LiveReq(TCHAR *pBuf, BYTE tOption)
{
	// tOption == 1 �̸� ������ ������ ��Ƴ��� �����
	// �̰�쿡�� pBuf ���� üũ���� ����� �Ѵ�.
	// ���ݵ� pBuf ���� üũ���� ������ ���߿� �� �Լ��� ������ ���� 
	// �����ؼ� ������ �ֵ��� �Ѵ�.

	if(m_bLive == USER_LIVE) return;

	int i;
	int result = 0;
	int iPotalZone = -1;

	int index = 0, rand = 0;

	int x = 0;		// Ÿ����Ż�� ����� DB��ǥ�� �������� ���ÿ��� ��Ƴ�	
	int y = 0;

	BOOL nResult = FALSE;
	CPoint	ptPotal, pt;	

	if(m_tGuildHouseWar == GUILD_WARRING) CheckGuildUserListInGuildHouseWar(); // �ٸ� �������� ���ϳ� üũ..
/*
	for(i = 0; i < g_TownPotal.GetSize(); i++)
	{
		if(g_TownPotal[i]->iZone == m_curz) { iPotalIndex = i; break; }
	}

	if(iPotalIndex < 0) { nResult = TRUE; goto go_result; }
*/
	pt = GetTownPotal(iPotalZone);
	x = pt.x;
	y = pt.y;

	if(iPotalZone < 0) 
	{ 
		if( m_curz == 56 || m_curz == 57 || m_curz == 58 || m_curz == 59 ) { nResult = TRUE; goto go_result; }

		x = g_TownPotal[0]->iPotalX;
		y = g_TownPotal[0]->iPotalY;
		iPotalZone = g_TownPotal[0]->iPotalZone;
	}
													// �� ü�������� ��ü ��ǥ�� ��ȯ�Ѵ�.			
//	if(g_TownPotal[iPotalIndex]->iPotalZone != m_curz) ZoneMoveReq(g_TownPotal[iPotalIndex]->iPotalZone, x, y);
	if(iPotalZone != m_curz) ZoneMoveReq(iPotalZone, x, y);
	else 
	{	
		ptPotal = ConvertToServer(x, y);			

		i = SEARCH_TOWN_POTAL_COUNT;				// ��ǥ�� ã������ Ƚ��
		pt = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);
		if(pt.x == -1 || pt.y == -1)
		{
			while(TRUE)								// Ÿ����Ż ���� ��ǥ�� ��´�.
			{
				rand = myrand(-TOWN_POTAL_SIZE, TOWN_POTAL_SIZE);
				ptPotal.x += rand; ptPotal.y += rand;

				pt = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);
				if(pt.x != -1 && pt.y != -1) break;

				i--;
				if(i <= 0) // ���߿�
				{
					pt = ConvertToServer(x, y);
					m_curx = pt.x; m_cury = pt.y;
					nResult = TRUE;
					goto go_result;
				}
			}
		}
		x = pt.x; y = pt.y;
		if(LiveCity(x, y) == FALSE)  { nResult = TRUE; goto go_result; }
	}

go_result:
	m_bLive = USER_LIVE;

	if(m_sMagicMaxHP > m_sMaxHP) m_sHP = m_sMagicMaxHP;
	else m_sHP = m_sMaxHP;
	
//	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0) EndGuildWar();	// �ʵ��� ���� �ش� �渶�� �ʱ�ȭ�� �����Ƿ� �ٽ� �Ѵ�.

	if(nResult)
	{
		SoftClose();
		//Close();	// &&& Ȯ������.(NULL�� �ϴ���...)
		return;		// ������ ��ã���� �������� �i�Ƴ���.
	}

	InterlockedExchange(&m_lDeadUsed, (LONG)0);
	CPoint pos = ConvertToClient( m_curx, m_cury );

	CBufferEx TempBuf;
	TempBuf.Add(USER_INFO);
	TempBuf.Add(INFO_MODIFY);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_strUserID);

	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);

	TempBuf.Add(m_iSkin);
	TempBuf.Add(m_iHair);
	TempBuf.Add((BYTE)m_sGender);
	TempBuf.AddData(m_strFace, 10);

	for(i = 0; i < EQUIP_ITEM_NUM; i++) TempBuf.Add(m_UserItem[i].sSid);

	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sMagicMaxHP);

	TempBuf.Add(m_tDir);

//	TempBuf.Add((BYTE)m_strAbnormal[0]);			// Abnormal Info
//	TempBuf.Add(m_tPsiAbnormal);						//&&&&&&&&&&&& Test Code	

	TempBuf.Add(m_dwAbnormalInfo);					// �����̻� ����
	TempBuf.Add(m_dwAbnormalInfo_);
	
	TempBuf.Add((DWORD)0);
	TempBuf.Add((DWORD)0);

	TempBuf.Add(m_sCityRank);
	TempBuf.Add((int)m_dwGuild);					//&&&&&&&&&&&& Test Code
	TempBuf.AddString(m_strGuildName);			// ��� �̸��� �߰�
	TempBuf.Add(m_sGuildVersion);

	TempBuf.Add(m_byClass);
	TempBuf.Add((BYTE)m_bPkStatus);

	TempBuf.AddString(m_strLoveName);//yskang 0.1
	
	for( i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) TempBuf.Add(m_UserItem[i].sSid);	// EBody
	if(m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
		TempBuf.Add((BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]));
		TempBuf.Add((BYTE)0x00);
	}else{
		TempBuf.Add(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]);
		TempBuf.Add((BYTE)0xff);
	}
	TempBuf.AddString(m_PersonalShopName);

	SendInsight(TempBuf, TempBuf.GetLength());

	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.

	//TRACE("%d�� ���� ��Ƴ�\n", m_uid );
}

/////////////////////////////////////////////////////////////////////////////////////////
// ���� ������� ��Ȱ��Ų��.
//
void USER::RevivalReq( TCHAR *pBuf )
{
	int			i;
	int			nBufferPos;
	short		tSlot;
	BYTE		error;
	USER*		pUser;
	TCHAR		strUserID[31];
	ItemList	ReverseItem;
	CBufferEx	TempBuf, TempYourBuf;
	CPoint		ptRevival, ptClientRevival;


	nBufferPos = 0;
	if(!GetVarString(sizeof(strUserID), (char*)strUserID, pBuf, nBufferPos)) return;

	if(m_bLive == USER_LIVE) return;

	pUser = GetUser(strUserID);	// User Pointer �� ��´�.

	// ��Ȱ�����ִ� ������ �������� ����
	if( pUser == NULL )
	{
		error = 1;
		TempBuf.Add( REVIVAL_RESULT );
		TempBuf.Add( error );
		Send( TempBuf, TempBuf.GetLength() );
		return;
	}

	if( !CheckBuddyMember(pUser->m_strUserID) )
	{
		error = 1;
		TempBuf.Add( REVIVAL_RESULT );
		TempBuf.Add( error );
		Send( TempBuf, TempBuf.GetLength() );
		return;
	}

	::ZeroMemory( &ReverseItem, sizeof(ItemList) );
	ReverseItem.sSid = NORMAL_ITEM_REVERSE;
	tSlot = (short)pUser->GetSameItem(ReverseItem, INVENTORY_SLOT);

	//���� �κ��� �������� ���ų� �������� 0�� ���
	if( tSlot == -1 )
	{
		error = 2;
		TempBuf.Add( REVIVAL_RESULT );
		TempBuf.Add( error );
		Send( TempBuf, TempBuf.GetLength() );
		return;
	}
	else if( pUser->m_UserItem[tSlot].sCount <= 0 )
	{
		error = 2;
		TempBuf.Add( REVIVAL_RESULT );
		TempBuf.Add( error );
		Send( TempBuf, TempBuf.GetLength() );
		return;
	}

	
	////////////////////////////////////////////////////////////////
	// ��Ȱ��ġ�� ã�Ƽ� ��Ȱ��Ų��.

	if(m_tGuildHouseWar == GUILD_WARRING) CheckGuildUserListInGuildHouseWar(); // �ٸ� �������� ���ϳ� üũ..

	ptRevival = FindNearAvailablePoint_S( m_curx, m_cury );

	// ��Ȱ�� ��ġ�� ��ã���� �������� ��Ȱ�ϵ��� �Ѵ�.
	if( ptRevival.x == -1 || ptRevival.y == -1 )
	{
//		LiveReq( pBuf );
		TownPotal();
		//Close();	// &&& Ȯ������.(NULL�� �ϴ���...)
		return;		
	}

	// ���⼭ LiveCity ��ƾ�� ������ġ���� ��Ȱ��. 
	// �������� ��Ȱ�ϴ°��� �ƴ�.( �ڵ���Ȱ�� ^^ )
	if( LiveCity( ptRevival.x, ptRevival.y ) == FALSE )
	{
		error = 3;								// ��Ȱ�� ��Ҹ� ��ã��: �˼����� ����
		TempBuf.Add( REVIVAL_RESULT );
		TempBuf.Add( error );
		Send( TempBuf, TempBuf.GetLength() );
		return;
	}

	m_bLive = USER_LIVE;

	InterlockedExchange(&m_lDeadUsed, (LONG)0);
	ptClientRevival = ConvertToClient( m_curx, m_cury );

	if(m_sMagicMaxHP > m_sMaxHP) m_sHP = m_sMagicMaxHP;
	else m_sHP = m_sMaxHP;
//ţţ
	if( m_dwLastDownExp != 0 )
		GetExp( m_dwLastDownExp*50/100 );
		

	error = 0;

	TempBuf.Add( REVIVAL_RESULT );
	TempBuf.Add( error );

	// USER_INFO ���� Mode �� �� ������ ������ �״�� ����
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_strUserID);

	TempBuf.Add((short)ptClientRevival.x);
	TempBuf.Add((short)ptClientRevival.y);

	TempBuf.Add(m_iSkin);
	TempBuf.Add(m_iHair);
	TempBuf.Add((BYTE)m_sGender);
	TempBuf.AddData(m_strFace, 10);

	for( i = 0; i < EQUIP_ITEM_NUM; i++ ) TempBuf.Add(m_UserItem[i].sSid);

	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sMagicMaxHP);

	TempBuf.Add(m_tDir);

	TempBuf.Add(m_dwAbnormalInfo);				// �����̻� ����
	TempBuf.Add(m_dwAbnormalInfo_);
	/*TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	TempBuf.Add((BYTE)0x00);
	*/
	TempBuf.Add(m_sCityRank);

	TempBuf.Add((int)m_dwGuild);				//&&&&&&&&&&&& Test Code
	TempBuf.AddString(m_strGuildName);			// ��� �̸��� �߰�
	TempBuf.Add(m_sGuildVersion);
	TempBuf.Add(m_byClass);
	TempBuf.Add((BYTE)m_bPkStatus);

	TempBuf.AddString(m_strLoveName);			//yskang 0.1

	for( i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) TempBuf.Add(m_UserItem[i].sSid);	// EBody
	if(m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
		TempBuf.Add((BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]));
		TempBuf.Add((BYTE)0x00);
	}else{
		TempBuf.Add(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]);
		TempBuf.Add((BYTE)0xff);
	}
	TempBuf.AddString(m_PersonalShopName);

	SendInsight(TempBuf, TempBuf.GetLength());

	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.


	//////////////////////////////////////////////////////////////////////////
	// ����� ������ ���������� �������� ��� ������ ������.

	// ������������ ������ ���ҽ�Ų��.
	pUser->m_UserItem[tSlot].sCount -= 1;
	if(pUser->m_UserItem[tSlot].sCount <= 0) pUser->m_UserItem[tSlot].sSid = -1;

	// ���� ���Կ��� ��ǥ �Ѱ� ���Ը� ���ҽ�Ų��.
	pUser->m_iCurWeight -= g_arItemTable[NORMAL_ITEM_STAMP]->m_byWeight;
	if( pUser->m_iCurWeight < 0 ) pUser->m_iCurWeight = 0;

	pUser->UpdateUserItemDN();

	error = 1;
	TempYourBuf.Add( ITEM_GIVE_RESULT );
	TempYourBuf.Add( error );
	TempYourBuf.Add( (BYTE)tSlot );
	TempYourBuf.Add( pUser->m_UserItem[tSlot].sLevel );
	TempYourBuf.Add( pUser->m_UserItem[tSlot].sSid );
	TempYourBuf.Add( pUser->m_UserItem[tSlot].sDuration );
	TempYourBuf.Add( pUser->m_UserItem[tSlot].sBullNum );
	TempYourBuf.Add( pUser->m_UserItem[tSlot].sCount );
	for(i = 0; i < MAGIC_NUM; i++) TempYourBuf.Add( pUser->m_UserItem[tSlot].tMagic[i]);
	TempYourBuf.Add( pUser->m_UserItem[tSlot].tIQ );

	pUser->Send(TempYourBuf, TempYourBuf.GetLength());

		//��BOSS����ʹ����֮�Ὣ�Զ��س�
	if(m_curz==409){
		ReturnTown(1, 33);
		return;
	}


}


//######################################################################################
//	Cell Functions
//######################################################################################

////////////////////////////////////////////////////////////////////////////
//	CELL �� ��ġ index setting
//
/*
BOOL USER::SetCellIndex(int zone, int xpos, int ypos)
{
	CPoint pt;

	for(int i = 0; i < g_cell.GetSize(); i++)
	{
		if( g_cell[i]->m_Zone == zone )
		{
			pt = g_cell[i]->GetCellIndex(xpos, ypos);
			if(pt.x < 0 || pt.y < 0) return FALSE;
			
			m_ptCell = pt;
			m_nCellZone = i;

			break;
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//	Cell �� ������ ����Ѵ�.
//
void USER::AddCellUid()
{
	g_cell[m_nCellZone]->AddCell(m_ptCell, m_uid + USER_BAND);
}

////////////////////////////////////////////////////////////////////////////
//	Cell ���� ������ �����Ѵ�.
//
void USER::DeleteCellUid()
{
	g_cell[m_nCellZone]->DeleteCell(m_ptCell, m_uid + USER_BAND);
}

////////////////////////////////////////////////////////////////////////////
//	Cell �� ������� ����� ó��
//
void USER::ChangeCellUid(CPoint ptOld, CPoint ptNew)
{
	g_cell[m_nCellZone]->ChangeCell(ptOld, ptNew, m_uid + USER_BAND);
}
*/
//-------------------------------------------------------------------------------//

/////////////////////////////////////////////////////////////////////////
//	��ų���̺��� �ش� ��ų���̵𿡴��� ��ų���� ���´�.
//
BOOL USER::GetSkillName(int nSid, CString &strName)
{
/*	int nSize = g_arSkillTable.GetSize();

	if(nSid > nSize || nSid < 0) return FALSE;

	strName = g_arSkillTable[nSid]->m_strName;
*/
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//	��ų���ۿ��� �Ҽ� Ŭ������ ��ų��� ������ ���´�.
//
BOOL USER::GetSkillNameLevel(BYTE byClass, TCHAR* pSkill, TCHAR* pBuf)
{
	if(pSkill == NULL) return FALSE;

	SkillList UserSkill[TOTAL_SKILL_NUM];

	int i, index = 0;
	for(i = 0; i < TOTAL_SKILL_NUM; i++)
	{
		UserSkill[i].sSid		= GetShort(pSkill, index);
		UserSkill[i].tLevel		= GetByte(pSkill, index);
	}

	index = 0;
	for(i = 0; i < TOTAL_SKILL_NUM; i++)
	{
		SetShort(pBuf,UserSkill[i].sSid, index); 
		SetByte(pBuf, UserSkill[i].tLevel, index);
	}

//int nOffset = byClass * 5;
//	short sSid;
//	CString strSkillName;
/*	TCHAR	szTemp[SKILL_NAME_LENGTH];

	index = 0;
	for(i = 0; i < SKILL_NUM; i++)
	{
		sSid = UserSkill[nOffset + i].sSid;
		
//		if(!GetSkillName(sSid, strSkillName))return FALSE;

		SpaceCopy(szTemp, strSkillName, SKILL_NAME_LENGTH);

		SetString(pBuf, szTemp, SKILL_NAME_LENGTH, index);
		SetByte(pBuf, UserSkill[nOffset + i].tLevel, index);
	}
*/
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
//	ItemTable ���� �ش� �����۾��̵� ���� �׸���ȣ�� �����´�.
//
BOOL USER::GetEquipItemPid(TCHAR *pItemBuf, TCHAR *pBuf)
{
	if(pItemBuf == NULL) return FALSE;

	int nSize = g_arItemTable.GetSize();
	if(EQUIP_ITEM_NUM > nSize) return FALSE;

	ItemList UserItem[EQUIP_ITEM_NUM];

	int index = 0;
	int i, j;
	for(i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		UserItem[i].sLevel		= GetShort(pItemBuf, index);
		UserItem[i].sSid		= GetShort(pItemBuf, index);
		UserItem[i].sDuration	= GetShort(pItemBuf, index);
		UserItem[i].sBullNum	= GetShort(pItemBuf, index);
		UserItem[i].sCount		= GetShort(pItemBuf, index);

		for(j = 0; j < MAGIC_NUM; j++) UserItem[i].tMagic[j] = GetByte(pItemBuf, index);

		UserItem[i].tIQ			= GetByte(pItemBuf, index);

		// Ŭ���̾�Ʈ�� ������ ���� ������ ���鶧 �ø����ȣ�� ������ �����Ƿ� �о ������.
		for( j = 0; j < 8; j++ )
		{
			GetByte( pItemBuf, index );
		}
	}	
	
	short sSid = 0;
	short sPid = 0;

	index = 0;
	for(i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		sSid = UserItem[i].sSid;
		if(sSid >= nSize) return FALSE;

		if(sSid != -1)
		{
			sPid = sSid;//g_arItemTable[sSid]->m_sPid;
		}
		else sPid = -1;
		
		SetShort(pBuf, sPid, index);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
//	����ִ� ��ų������ �����.
//
void USER::MakeEmptySkill(TCHAR *pBuf, BYTE tClass)
{
	int index = 0;
	int i;

	for(i = 0; i < CLASS_NUM * SKILL_NUM; i++)
	{	
			SetShort(pBuf, i, index);				// sid
			SetByte(pBuf, 0, index);				// skill level
/*
		if(i == (tClass * 5))						// �ְ迭�� ����/ȸ�ǽ�ų �ϳ��� �⺻������ �ش�.
			SetByte(pBuf, 1, index);				// skill level
		else 
			SetByte(pBuf, 0, index);		

		SetByte(pBuf, 0 , index);					// OnOff
*/	}
}

//////////////////////////////////////////////////////////////////////////
//	����ִ� ������ ������ �����.(���ο� ĳ���� ���鶧 �⺻ ���⸦ �ֱ�����)
//
void USER::MakeInintItem(TCHAR *pBuf, BYTE tClass)
{
	int index = 0;
	int i, j;
	int sSid = 0, sSid2 = 0, sSid3 = 0, sSid4 = 0, sSubSid = 0;

	InitUserItem();

	for(i = 0; i < g_arInitItem.GetSize(); i++)
	{
		if(tClass == g_arInitItem[i]->tClass) 
		{ 
			sSid = g_arInitItem[i]->iSid; 
			sSubSid = g_arInitItem[i]->iSubSid;
			break; 
		}
	}

	m_UserItem[EQUIP_ITEM_NUM].sLevel = g_arItemTable[sSid]->m_byRLevel;
	m_UserItem[EQUIP_ITEM_NUM].sSid = g_arItemTable[sSid]->m_sSid;
	m_UserItem[EQUIP_ITEM_NUM].sDuration = g_arItemTable[sSid]->m_sDuration;
	m_UserItem[EQUIP_ITEM_NUM].sBullNum = g_arItemTable[sSid]->m_sBullNum;
	m_UserItem[EQUIP_ITEM_NUM].sCount = 1;
	for(j = 0; j < MAGIC_NUM; j++) m_UserItem[EQUIP_ITEM_NUM].tMagic[j] = 0;
	m_UserItem[EQUIP_ITEM_NUM].tIQ = 0;
	m_UserItem[EQUIP_ITEM_NUM].iItemSerial = 0;

	switch(tClass)
	{
	case EDGED:
		m_UserItem[EQUIP_ITEM_NUM].sBullNum = g_arItemTable[sSubSid]->m_sBullNum;
	break;

	case FIREARMS:
		m_UserItem[EQUIP_ITEM_NUM].sBullNum = g_arItemTable[sSubSid]->m_sBullNum;
/*		m_UserItem[EQUIP_ITEM_NUM + 1].sLevel = g_arItemTable[sSid]->m_byRLevel;
		m_UserItem[EQUIP_ITEM_NUM + 1].sSid = g_arItemTable[sSubSid]->m_sSid;
		m_UserItem[EQUIP_ITEM_NUM + 1].sDuration = g_arItemTable[sSubSid]->m_sDuration;
		m_UserItem[EQUIP_ITEM_NUM + 1].sBullNum = g_arItemTable[sSubSid]->m_sBullNum;
		m_UserItem[EQUIP_ITEM_NUM + 1].sCount = 1;
		for(j = 0; j < MAGIC_NUM; j++) m_UserItem[EQUIP_ITEM_NUM  +1].tMagic[j] = 0;
		m_UserItem[EQUIP_ITEM_NUM + 1].tIQ = 0;
*/		break;
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++)
	{	
		SetShort(pBuf, 0, index);		// Level
		SetShort(pBuf, -1, index);		// sid
		SetShort(pBuf, 0, index);		// BullNum
		SetShort(pBuf, 0, index);		// Count
		SetShort(pBuf, 0, index);		// Duration

		for(j = 0; j < MAGIC_NUM; j++) 
			SetByte(pBuf, 0, index);	// Magic Attribute

		SetByte(pBuf, 0, index);		// Item Quality

		for(j = 0; j < 8; j++ )			// Item Serial
		{
			SetByte(pBuf, 0, index );
		}
	}

	UserItemToStr(pBuf);
}

//////////////////////////////////////////////////////////////////////////
//	����ִ� ���̿��� ������ �����.
//
void USER::MakeEmptyPsi(TCHAR *pBuf)
{
	int index = 0;

	SetShort(pBuf, -1, index);		// sid
									// ���̿��� ���۴� -1�� ���������� �д´�. �������� ��� 0�� �ʱ�ȭ
	SetByte(pBuf, 0 , index);		// OnOff
}

//////////////////////////////////////////////////////////////////////////
//	PA or MA Point �� ��� ���� ���� ����ġ�� ���Ѵ�.
//
DWORD USER::GetNextPAMAExp(DWORD dwExp)
{
	int i;
	int nSize = g_arPAMAExp.GetSize();

	for(i = 0; i < nSize; i++)
	{
		if(g_arPAMAExp[i]->m_dwPAMAExp > dwExp) return g_arPAMAExp[i]->m_dwPAMAExp;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//	���� ������ �Ǳ� ���� ����ġ�� ���Ѵ�.
//
DWORD USER::GetNextLevel(int iCurrLevel)
{
	int nSize = g_arLevelUpTable.GetSize();

	if(iCurrLevel >= nSize) return 0;

	return (DWORD)(g_arLevelUpTable[iCurrLevel]->m_dwExp);
}

////////////////////////////////////////////////////////////////////////////
//	�ش� ��ų�� ������ ���Ѵ�. (����� �������� ���Ѵ�.)
//
void USER::GetSkillInfo(int sid, BYTE& tRate)
{
/*	int i = 0;
	int nOnSkill = 0;

	if(m_UserSkill[sid].tOnOff == 0)
	{
		tRate = 0;
	}
	else
	{
		for(i = 0; i < SKILL_NUM; i++)
		{
			if(m_UserSkill[m_byClass * SKILL_NUM + i].tOnOff == 1) nOnSkill++;
		}

		tRate = (BYTE)((float)m_sWIS * 0.5 + m_UserSkill[sid].tLevel * 2 - nOnSkill * 25 + 50);
	}
*/
}

////////////////////////////////////////////////////////////////////////////
//	�ش� ��ų�� �������� ���Ѵ�. (�����. ���� �Ⱦ�)
//
BYTE USER::GetSkillRate(BYTE byClass, int sid, int nDefusion)
{
	int nSelect = 0;
/*
	for(int i = 0; i < SKILL_NUM; i++)
	{
		if(m_UserSkill[byClass * SKILL_NUM + i].tOnOff == 1) nSelect++;
	}
*/
//!	BYTE byRate = (BYTE)((double)m_sWIS * 0.3 + (double)m_UserSkill[sid].sLevel * 3.5 - nSelect * 25 + 40 - nDefusion);
	BYTE byRate = 1;

	return byRate;
}

////////////////////////////////////////////////////////////////////////////
//	Trade Mode �� On/Off �Ѵ�
//
void USER::SetTradeMode(TCHAR *pBuf)
{
/*	int index = 0;
	BOOL bMode = (BOOL)GetByte(pBuf, index);

	m_bTradeMode = bMode;

	CBufferEx TempBuf;

	TempBuf.Add(TRADE_MODE_RESULT);
	TempBuf.Add((BYTE)m_bTradeMode);

	Send(TempBuf, TempBuf.GetLength());
*/
}

////////////////////////////////////////////////////////////////////////////
//	�������� EQUIP, INVENTORY, QUICK ���԰��� �̵� ��Ų��.
//
void USER::ItemMoveReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return;//yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int	index = 0;
	int i;
	short sOldSid = 0, sNewSid = 0;
	int nOldPos = 0, nNewPos = 0;
	BYTE byOldWear = 255, byNewWear = 255;

	BYTE tType = GetByte(pBuf, index);			// �Ϲ� �����۰� �� �����۰��� ����

	if(tType == 1 || tType == 2) return;		// �������� ���� ó������ �ʴ´�
	
	BYTE byOldSlot = GetByte(pBuf, index);		// ���������� ��ġ
	BYTE byNewSlot = GetByte(pBuf, index);		// �ű���� ������ ��ġ

	if(byOldSlot < 0 || byOldSlot >= TOTAL_ITEM_NUM || byNewSlot < 0 || byNewSlot >= TOTAL_ITEM_NUM) 
	{
		SendItemMoveFail();
		return;
	}
	
	sOldSid = m_UserItem[byOldSlot].sSid;
	sNewSid = m_UserItem[byNewSlot].sSid;
	
	if(sOldSid <= -1)							// �ҽ��� ������� ����
	{
		SendItemMoveFail();
		return;
	}
	
	if(sOldSid >= 0 && sOldSid < g_arItemTable.GetSize()) 
	{
		byOldWear = g_arItemTable[sOldSid]->m_byWear;
	}
	
	if(sNewSid >= 0 && sNewSid < g_arItemTable.GetSize()) 
	{
		byNewWear = g_arItemTable[sNewSid]->m_byWear;
	}
	
	nOldPos = GetSlotClass(byOldSlot);
	nNewPos = GetSlotClass(byNewSlot);
	
	if(nOldPos == -1 || nNewPos == -1)			// �ش��ϴ� ������ ���� ���
	{
		SendItemMoveFail();
		return;
	}

	short	sChangeSlot[3];
	for(i = 0; i < 3; i++)
	{
		sChangeSlot[i] = 255;
	}

	ItemList	TempItem;
	BOOL		bChangeEquip = FALSE;
	int			iSearchSlot = -1;
	int			iSameSlot = -1;
	BOOL		bEmpty01 = FALSE, bEmpty02 = FALSE;
	CPoint		ptEquipPos(-1, -1);

	int			iItemIndex = 0, iMagicIndex = 0;

	int			iEBodyPos = -1;
	
	switch (nOldPos)
	{
	case 0:						// EQUIP
		switch(nNewPos)
		{
		case 0:					// E -> E
			if(byOldWear != byNewWear)	// ���� ��������� �������� �ƴϸ�
			{
				if(byOldWear != 1 && byOldWear != 8)
				{
					SendItemMoveFail();
					return;
				}
				if(
					(byOldSlot == 4 && byNewSlot != 5) ||
				    (byOldSlot == 5 && byNewSlot != 4) ||
				    (byOldSlot == 6 && byNewSlot != 7) ||
				    (byOldSlot == 7 && byNewSlot != 6)	)
				{
					SendItemMoveFail();
					return;
				}
			}
		
			iItemIndex = GetItemIndex(m_UserItem[byOldSlot].sSid);
			if(iItemIndex < 0) { SendItemMoveFail(); return; }
			m_UserItem[byOldSlot].sLevel = g_arItemTable[iItemIndex]->m_byRLevel;

			if(m_UserItem[byOldSlot].tIQ > NORMAL_ITEM)
			{
				if(m_UserItem[byOldSlot].tIQ == UNIQUE_ITEM) iItemIndex = MAGIC_NUM;	// �ִ� �˻� Ƚ��
				else iItemIndex = 4;								// ����ũ�� ��Ʈ �������� �� �������� �ִ� 4�������� ���� ���

				for(i = 0; i < iItemIndex; i++)
				{
					iMagicIndex = m_UserItem[byOldSlot].tMagic[i];
					if(iMagicIndex >= 0 && iMagicIndex < g_arMagicItemTable.GetSize())
					{
						m_UserItem[byOldSlot].sLevel += g_arMagicItemTable[iMagicIndex]->m_tLevel;
					}
				}
			}

			if(m_UserItem[byOldSlot].sLevel > m_sLevel) { SendItemMoveFail(); return; }

			if(!IsCanEquipItem((int)byOldSlot)) { SendItemMoveFail(); return; }

			TempItem = m_UserItem[byOldSlot];
			m_UserItem[byOldSlot] = m_UserItem[byNewSlot];
			m_UserItem[byNewSlot] = TempItem;

			sChangeSlot[0] = byOldSlot;			
			sChangeSlot[1] = byNewSlot;

			if(byOldWear == 1 || byNewWear == 1) bChangeEquip = TRUE;
			break;
		case 1:		// E -> I
			if(byOldWear >= 9 || m_UserItem[byOldSlot].sCount > 1)		// if( count > 1 ) FAIL;
			{
				m_UserItem[byOldSlot].sCount = 1;
				SendItemMoveFail();
				return;
			}

			if(byOldWear >= 6)							// �������� ���� ��� �κ��� ��ĥ�� �ִ�.	
			{
				iSameSlot = GetSameItem(m_UserItem[byOldSlot], INVENTORY_SLOT);	 
				if(iSameSlot != -1)						// �κ��丮�� ���� �������� �����ϸ�
				{
					if(m_UserItem[iSameSlot].sCount <= 0 || m_UserItem[iSameSlot].sCount >= _MAX_SHORT) { SendItemMoveFail(); return; }

					ReSetItemSlot(&m_UserItem[byOldSlot]);
					CheckMaxValue((short &)m_UserItem[iSameSlot].sCount, (short)1);

					sChangeSlot[0] = byOldSlot;			
					sChangeSlot[1] = iSameSlot;
					if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
					break;
				}
			}
														// �� ������ �����ϸ� ������ �� ���Կ� �ϳ��� ����.
			if(m_UserItem[byNewSlot].sSid == -1) iSearchSlot = byNewSlot;	
			else	iSearchSlot = GetEmptySlot(INVENTORY_SLOT);

			if(iSearchSlot == -1)						// ������ ��û�� ������ �������..
			{
				SendItemMoveFail();
				return;
			}

			m_UserItem[iSearchSlot] = m_UserItem[byOldSlot];
//			ReSetItemSlot(byOldSlot);
			ReSetItemSlot(&m_UserItem[byOldSlot]);

			sChangeSlot[0] = byOldSlot;			
			sChangeSlot[1] = iSearchSlot;
			if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
			break;
		case 2:		// E -> EBody
			SendItemMoveFail();
			return;
		default:
			SendItemMoveFail();
			return;
		};
		break;
	case 1:			//INVENTORY
		switch(nNewPos)
		{
		case 0:		// I -> E
			if(byOldWear >= 1 && byOldWear <= 8)	// ���â���� �ű� �� �ִ� �������̸�
			{
				if(!IsCanEquipItem((int)byOldSlot)) 
				{ 
					SendItemMoveFail(); 
					return; 
				}

				if(m_UserItem[byOldSlot].sCount > 1)	// �ҽ��� 1���� �ʰ��ϸ�
				{
					bEmpty01 = bEmpty02 = FALSE;

					GetEquipPos(byOldWear, ptEquipPos);

					if(ptEquipPos.x != -1)
					{
						if(m_UserItem[ptEquipPos.x].sSid == -1) bEmpty01 = TRUE;
					}
					if(ptEquipPos.y != -1)
					{
						if(m_UserItem[ptEquipPos.y].sSid == -1) bEmpty02 = TRUE;
					}

					iItemIndex = GetItemIndex(m_UserItem[byOldSlot].sSid);
					if(iItemIndex < 0) { SendItemMoveFail(); return; }
					m_UserItem[byOldSlot].sLevel = g_arItemTable[iItemIndex]->m_byRLevel;

					if(m_UserItem[byOldSlot].tIQ > NORMAL_ITEM)
					{
						if(m_UserItem[byOldSlot].tIQ == UNIQUE_ITEM) iItemIndex = MAGIC_NUM;	// �ִ� �˻� Ƚ��
						else iItemIndex = 4;								// ����ũ�� ��Ʈ �������� �� �������� �ִ� 4�������� ���� ���

						for(i = 0; i < iItemIndex; i++)
						{
							iMagicIndex = m_UserItem[byOldSlot].tMagic[i];
							if(iMagicIndex >= 0 && iMagicIndex < g_arMagicItemTable.GetSize())
							{
								m_UserItem[byOldSlot].sLevel += g_arMagicItemTable[iMagicIndex]->m_tLevel;
							}
						}
					}

					if(m_UserItem[byOldSlot].sLevel > m_sLevel) { SendItemMoveFail(); return; }

					if(bEmpty01)
					{
						m_UserItem[ptEquipPos.x] = m_UserItem[byOldSlot];
						m_UserItem[ptEquipPos.x].sCount = 1;

						m_UserItem[byOldSlot].sCount--;
						
						sChangeSlot[0] = byOldSlot;			
						sChangeSlot[1] = (BYTE)ptEquipPos.x;
						
						if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
						break;
					}

					if(bEmpty02)
					{
						m_UserItem[ptEquipPos.y] = m_UserItem[byOldSlot];
						m_UserItem[ptEquipPos.y].sCount = 1;

						m_UserItem[byOldSlot].sCount--;
						
						sChangeSlot[0] = byOldSlot;			
						sChangeSlot[1] = (BYTE)ptEquipPos.y;
						
						if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
						break;
					}

					if(byNewSlot == (BYTE)ptEquipPos.y) ptEquipPos.x = ptEquipPos.y;
					iSearchSlot = GetEmptySlot(INVENTORY_SLOT);
					iSameSlot = GetSameItem(m_UserItem[ptEquipPos.x], INVENTORY_SLOT);

					if(iSearchSlot == -1 && iSameSlot == -1) 
					{
						SendItemMoveFail();
						return;
					}
					
					if(iSameSlot != -1)
					{
						m_UserItem[iSameSlot].sCount++;

						m_UserItem[ptEquipPos.x] = m_UserItem[byOldSlot];
						m_UserItem[ptEquipPos.x].sCount = 1;

						m_UserItem[byOldSlot].sCount--;

						sChangeSlot[0] = byOldSlot;			
						sChangeSlot[1] = (BYTE)iSameSlot;
						sChangeSlot[2] = (BYTE)ptEquipPos.x;
						
						if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
						break;
					}
					
					if(iSearchSlot != -1)
					{
						m_UserItem[iSearchSlot] = m_UserItem[ptEquipPos.x];
						
						m_UserItem[ptEquipPos.x] = m_UserItem[byOldSlot];
						m_UserItem[ptEquipPos.x].sCount = 1;
						
						m_UserItem[byOldSlot].sCount--;
						
						sChangeSlot[0] = byOldSlot;			
						sChangeSlot[1] = (BYTE)iSearchSlot;
						sChangeSlot[2] = (BYTE)ptEquipPos.x;
						
						if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
						break;
					}
				}
				else									// �ҽ��� 1����
				{
					bEmpty01 = bEmpty02 = FALSE;

					GetEquipPos(byOldWear, ptEquipPos);

					if(ptEquipPos.x != -1)
					{
						if(m_UserItem[ptEquipPos.x].sSid == -1) bEmpty01 = TRUE;
					}
					if(ptEquipPos.y != -1)
					{
						if(m_UserItem[ptEquipPos.y].sSid == -1) bEmpty02 = TRUE;
					}

					iItemIndex = GetItemIndex(m_UserItem[byOldSlot].sSid);
					if(iItemIndex < 0) { SendItemMoveFail(); return; }
					m_UserItem[byOldSlot].sLevel = g_arItemTable[iItemIndex]->m_byRLevel;

					if(m_UserItem[byOldSlot].tIQ > NORMAL_ITEM)
					{
						if(m_UserItem[byOldSlot].tIQ == UNIQUE_ITEM) iItemIndex = MAGIC_NUM;	// �ִ� �˻� Ƚ��
						else iItemIndex = 4;								// ����ũ�� ��Ʈ �������� �� �������� �ִ� 4�������� ���� ���

						for(i = 0; i < iItemIndex; i++)
						{
							iMagicIndex = m_UserItem[byOldSlot].tMagic[i];
							if(iMagicIndex >= 0 && iMagicIndex < g_arMagicItemTable.GetSize())
							{
								m_UserItem[byOldSlot].sLevel += g_arMagicItemTable[iMagicIndex]->m_tLevel;
							}
						}
					}

					if(m_UserItem[byOldSlot].sLevel > m_sLevel) { SendItemMoveFail(); return; }

					if(bEmpty01)
					{
						TempItem = m_UserItem[byOldSlot];
						m_UserItem[byOldSlot] = m_UserItem[ptEquipPos.x];
						m_UserItem[ptEquipPos.x] = TempItem;
						
						sChangeSlot[0] = byOldSlot;			
						sChangeSlot[1] = (BYTE)ptEquipPos.x;
						
						if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
						break;
					}
					if(bEmpty02)
					{
						TempItem = m_UserItem[byOldSlot];
						m_UserItem[byOldSlot] = m_UserItem[ptEquipPos.y];
						m_UserItem[ptEquipPos.y] = TempItem;
						
						sChangeSlot[0] = byOldSlot;			
						sChangeSlot[1] = (BYTE)ptEquipPos.y;
						
						if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
						break;
					}

					TempItem = m_UserItem[byOldSlot];
					m_UserItem[byOldSlot] = m_UserItem[ptEquipPos.x];
					m_UserItem[ptEquipPos.x] = TempItem;
					
					sChangeSlot[0] = byOldSlot;			
					sChangeSlot[1] = (BYTE)ptEquipPos.x;
					
					if(byOldWear >=1 && byOldWear <= 8) bChangeEquip = TRUE;
				}
			}
			else									// ���â���� �ű� �� ���� �������̸�
			{
				SendItemMoveFail();
				return;
			}
			break;
		case 1:		// I -> I
			TempItem = m_UserItem[byOldSlot];
			m_UserItem[byOldSlot] = m_UserItem[byNewSlot];
			m_UserItem[byNewSlot] = TempItem;
			
			sChangeSlot[0] = byOldSlot;			
			sChangeSlot[1] = byNewSlot;		
			break;
		case 2:		// I -> EBody (�κ����� EBody �������� �� �ű��)
			if(m_iDisplayType == 5)
			{
				SendSystemMsg("���� ����ڴ� EBody�� ����� �� �����ϴ�.", SYSTEM_NORMAL, TO_ME);
				SendItemMoveFail();
				return;
			}
			if((byOldWear!=130) && FindEvent(16) == FALSE) 
			{
				SendSystemMsg(IDS_NEED_QUEST, SYSTEM_ERROR, TO_ME);
				SendItemMoveFail();
				return;
			}
			//��е���ػ�
			if((byOldWear >= 122 && byOldWear <= 125)||(byOldWear==130))	// EBody �� �ű� �� �ִ� �������̸�
			{
				if(byOldWear==130)
				{
					m_UserItem[byOldSlot].sLevel=40;
				}
				if(m_UserItem[byOldSlot].sCount != 1)	// �ҽ��� 1���� �ƴϸ�
				{
					SendItemMoveFail();
					return;
				}

				bEmpty01 = bEmpty02 = FALSE;
				iEBodyPos = GetEBodyPos(byOldWear);
				if(iEBodyPos == -1)
				{
					SendItemMoveFail();
					return;
				}
				
				iItemIndex = m_UserItem[byOldSlot].sSid;
				if(iItemIndex < 0 || iItemIndex >= g_arItemTable.GetSize()) 
				{
					SendItemMoveFail(); 
					return;
				}
				m_UserItem[byOldSlot].sLevel = g_arItemTable[iItemIndex]->m_byRLevel;
				
				if(m_UserItem[byOldSlot].tIQ == MAGIC_ITEM)	// EBody �� ���� ��� ����
				{
					iItemIndex = 5; // EBody �� �ɼ��� 5��
					
					for(i = 1; i < iItemIndex; i++)		// ���̽� �ɼ��� üũ���� �ʴ´�.
					{
						iMagicIndex = m_UserItem[byOldSlot].tMagic[i];
						if(iMagicIndex >= 0 && iMagicIndex < g_arEBodyTable.GetSize())
						{
							m_UserItem[byOldSlot].sLevel += g_arEBodyTable[iMagicIndex]->m_tLevel;
						}
					}
				}
				
				if(m_UserItem[byOldSlot].sLevel > m_sLevel) { SendItemMoveFail(); return; }
				
				TempItem = m_UserItem[byOldSlot];
				m_UserItem[byOldSlot] = m_UserItem[iEBodyPos];
				m_UserItem[iEBodyPos] = TempItem;
					
				sChangeSlot[0] = byOldSlot;			
				sChangeSlot[1] = iEBodyPos;
					
				bChangeEquip = TRUE;
				break;
			}
			else SendItemMoveFail();
			break;
		default:
			SendItemMoveFail();
			return;
		};
		break;

	case 2:			// EBody
		switch(nNewPos)
		{
		case 0:		// EBody -> Equip
			SendItemMoveFail();
			return;
			break;
		case 1:		// EBody -> Inven
			if(m_UserItem[byNewSlot].sSid == -1) iSearchSlot = byNewSlot;	
			else	iSearchSlot = GetEmptySlot(INVENTORY_SLOT);

			if(iSearchSlot == -1)	// ����ִ� ������ �߰����� ��������
			{
				SendItemMoveFail();
				return;
			}

			m_UserItem[iSearchSlot] = m_UserItem[byOldSlot];
			ReSetItemSlot(&m_UserItem[byOldSlot]);

			sChangeSlot[0] = byOldSlot;			
			sChangeSlot[1] = iSearchSlot;
			bChangeEquip = TRUE;

			break;
		case 2:		// EBody -> EBody
			SendItemMoveFail();
			return;
		default:
			break;
		};
		break;
	default:
		SendItemMoveFail();
		return;
	};	

	BYTE nCount = 0;
	for(i = 0; i < 3; i++)
	{
		if(sChangeSlot[i] != 255) nCount++;
	}

	BYTE bySlot;
	CBufferEx TempBuf;

	TempBuf.Add(ITEM_MOVE_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(tType);

	if(bChangeEquip)
	{
		
		SendMyWeaponChangeInfo();
		//���¼���װ������..
		CheckMagicItemMove();
		SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	}

	if(tType == 0)
	{
		TempBuf.Add(nCount);

		for(i = 0; i < 3; i++)
		{
			bySlot = (BYTE)sChangeSlot[i];
			
			if(bySlot == 255) break;
			TempBuf.Add(bySlot);
			TempBuf.Add(m_UserItem[bySlot].sLevel);
			TempBuf.Add(m_UserItem[bySlot].sSid);
			TempBuf.Add(m_UserItem[bySlot].sDuration);
			TempBuf.Add(m_UserItem[bySlot].sBullNum);
			TempBuf.Add(m_UserItem[bySlot].sCount);

			for(int j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);

			TempBuf.Add(m_UserItem[bySlot].tIQ); 
		}
	}

	UpdateUserItemDN();

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////
//	�ش� �������� �����ɶ� �����Ӽ��� ���� ���� �ٲ���� üũ
//
void USER::CheckMagicItemMove()			//^^ ���߿� ����ȭ����...
{
	int i, j,t=0;
	int nCount = 4;						// ����, �����϶� ���� Ƚ���� 4������
	int iMagic = 0;
	int iMagicCount = 0;
	int ebody[5]={0,0,0,0,0};
	BOOL ebody_skill=0;
	BOOL bOldShield = CheckAbnormalInfo(ABNORMAL_SHIELD) || CheckAbnormalInfo(ABNORMAL_BIGSHIELD);
	CBufferEx TempBuf;

	for(i = 0; i < MAGIC_COUNT; i++) m_DynamicUserData[i] = 0;
	for(i = 0; i < EBODY_MAGIC_COUNT; i++) m_DynamicEBodyData[i] = 0;
	for(i = 0; i < 10; i++) m_DynamicMagicItem[i]= 0;

	for(i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		if(m_UserItem[i].sSid < 0 || m_UserItem[i].sSid >= g_arItemTable.GetSize()) continue;	// ����ڵ�

		if(i == BODY || i == PANTS)
		{
			if(m_byClass == 3)
			{
				if(g_arItemTable[m_UserItem[i].sSid]->m_byRLevel == 80 && g_arItemTable[m_UserItem[i].sSid]->m_byClass == 1)
				{
					m_DynamicUserData[MAGIC_MAX_HP_UP] += g_arItemTable[m_UserItem[i].sSid]->m_byMPP;
				}
			}
		}

		if(i == LEFT_HAND) continue;

		if((m_UserItem[i].tIQ >= REMODEL_ITEM && m_UserItem[i].tIQ <= REMODEL_MAGIC_ITEM)||m_UserItem[i].tIQ <= 12)  // �ϴ� ����� �ƴϸ�...
		{
			switch(m_UserItem[i].tIQ)
			{
			case MAGIC_ITEM:
				nCount = 2;
				break;
			case RARE_ITEM:
				nCount = 4;
				break;
			case UNIQUE_ITEM:
				nCount = MAGIC_NUM;
				break;
			case SET_ITEM:
				nCount = 4;
				break;
			case REMODEL_ITEM:
			case REMODEL_MAGIC_ITEM:
				nCount = 3;
				break;
			case 12:
				nCount= 5;
				GetMagicItem_100(m_UserItem[i].tMagic[6]);
				GetMagicItem_100(m_UserItem[i].tMagic[7]);
				break;
			default:
				nCount = 4;
				break;
			}

			for(j = 0; j < nCount; j++)
			{
				iMagic = m_UserItem[i].tMagic[j];
				if(iMagic < 0 || iMagic >= g_arMagicItemTable.GetSize()) continue;

				iMagicCount = g_arMagicItemTable[iMagic]->m_sSubType;
				if(iMagicCount < 0 || iMagicCount >= MAGIC_COUNT) continue;
				
				m_DynamicUserData[iMagicCount] += g_arMagicItemTable[iMagic]->GetMagicItemValue();
			}
		}
	}	
//�����е����
	nCount = 5;
	BYTE tValue = 0;
	for(i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++)	// E-Body Magic �Ӽ� �߰�
	{
		if(m_UserItem[i].sSid >= 0)			
		{
				
			if(m_UserItem[i].tIQ == MAGIC_ITEM )		// �� ���� �Ӽ��� �پ� ������...
			{
				for(j = 0; j < nCount; j++)				// ���� ����ŭ ������. 
				{				
					iMagic = m_UserItem[i].tMagic[j];
					if(iMagic < 0 || iMagic >= g_arEBodyTable.GetSize()) continue;
					
					iMagicCount = g_arEBodyTable[iMagic]->m_sSubType;
					if(iMagicCount < 0 || iMagicCount >= EBODY_MAGIC_COUNT) continue;
					
					tValue = (BYTE)g_arEBodyTable[iMagic]->m_sChangeValue;
					m_DynamicEBodyData[iMagicCount] += g_arEBodyTable[iMagic]->m_sChangeValue;
				}
				if(m_UserItem[i].sSid==987){
					//����������
					m_DynamicMagicItem[7]=m_DynamicMagicItem[7] + m_UserItem[i].tMagic[5]*6;
				}else{ //�����˼���
					m_DynamicMagicItem[5]=m_DynamicMagicItem[5] + m_UserItem[i].tMagic[5]*2;
				}
			}
		}
	}
	//�����ػ�����
	GetMagicItemSetting_38();
	GetSkillSetting_130();//����130����

m_DynamicUserData[MAGIC_DEFENSE_UP]=m_DynamicUserData[MAGIC_DEFENSE_UP]+m_DynamicMagicItem[0];
m_DynamicUserData[MAGIC_PSI_RESIST_UP]=m_DynamicUserData[MAGIC_PSI_RESIST_UP]+m_DynamicMagicItem[1];
m_DynamicUserData[MAGIC_MAX_HP_UP]=m_DynamicUserData[MAGIC_MAX_HP_UP]+m_DynamicMagicItem[2];

	// �Ҹ� �Ǽ��縮 üũ
	m_bRingOfLife		= FALSE;
	m_bNecklaceOfShield	= FALSE;
	m_bEarringOfProtect = FALSE;
	for(i = 0; i < MAX_ACCESSORI; i++)
	{
		if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_RING_OF_LIFE)		m_bRingOfLife = TRUE;
		if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_NECKLACE_OF_SHIELD)	m_bNecklaceOfShield = TRUE;
		if(m_UserItem[g_iAccessoriSlot[i]].sSid == SID_EARRING_OF_PROTECT)	m_bEarringOfProtect = TRUE;
	}

	if(m_bNecklaceOfShield == TRUE) 
	{
		AddAbnormalInfo(ABNORMAL_SHIELD);
		if(bOldShield == FALSE)
		{
			TempBuf.Add(SET_USER_STATE);
			TempBuf.Add(m_uid + USER_BAND);
			TempBuf.Add(m_dwAbnormalInfo);
			TempBuf.Add(m_dwAbnormalInfo_);

			SendInsight(TempBuf, TempBuf.GetLength());
//			SendExactScreen(TempBuf, TempBuf.GetLength());
		}
	}
	else
	{
		if(bOldShield == TRUE && m_dwShieldTime == 0)
		{
			DeleteAbnormalInfo(ABNORMAL_SHIELD);
			TempBuf.Add(SET_USER_STATE);
			TempBuf.Add(m_uid + USER_BAND);
			TempBuf.Add(m_dwAbnormalInfo);
			TempBuf.Add(m_dwAbnormalInfo_);
//			SendInsight(TempBuf, TempBuf.GetLength());
			SendExactScreen(TempBuf, TempBuf.GetLength());
		}
	}
	
	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP)) SetUserToMagicUser(1);	// MAX_HP_UP�̸� ��� ���������ش�.
	else									  SetUserToMagicUser();

	SendUserStatusSkill();
}

/////////////////////////////////////////////////////////////////////////////
//	�ش� ������ ������ �������� ���� Ŭ�������� ����Ҽ� �ִ� �������� �Ǵ��Ѵ�.
//
BOOL USER::IsCanEquipItem(int iSlot)
{
	int i;
	BOOL bflag = FALSE;
	CString strMsg;

	int sid = m_UserItem[iSlot].sSid;
	int armClass = g_arItemTable[sid]->m_byClass;

	if(sid < 0 || sid >= g_arItemTable.GetSize()) return FALSE;

	short sNeedStr = g_arItemTable[sid]->m_byRstr;
	short sNeedDex = g_arItemTable[sid]->m_byRdex;
	short sNeedVol = g_arItemTable[sid]->m_byRvol;

	switch(m_byClass)
	{
	case BRAWL:
		for(i = 0; i < USE_CLASS_TYPE; i++) 
		{ 
			if(g_BrawlClass[i] == armClass)
			{
				if(m_sMagicSTR < sNeedStr || m_sMagicDEX < sNeedDex || m_sMagicVOL < sNeedVol) { bflag = TRUE; goto go_result;}
				else return TRUE; 
			}
		}		
		break;
	case STAFF:
		for(i = 0; i < USE_CLASS_TYPE; i++) 
		{ 
			if(g_StaffClass[i] == armClass) 
			{
				if(m_sMagicSTR < sNeedStr || m_sMagicDEX < sNeedDex || m_sMagicVOL < sNeedVol) { bflag = TRUE; goto go_result;}
				else return TRUE; 
			}
		}
		break;
	case EDGED:
		for(i = 0; i < USE_CLASS_TYPE; i++) 
		{ 
			if(g_EdgedClass[i] == armClass)
			{
				if(m_sMagicSTR < sNeedStr || m_sMagicDEX < sNeedDex || m_sMagicVOL < sNeedVol) { bflag = TRUE; goto go_result;}
				else return TRUE; 
			}
		}
		break;
	case FIREARMS:
		for(i = 0; i < USE_CLASS_TYPE; i++) 
		{ 
			if(g_FireArmsClass[i] == armClass)
			{
				if(m_sMagicSTR < sNeedStr || m_sMagicDEX < sNeedDex || m_sMagicVOL < sNeedVol) { bflag = TRUE; goto go_result;}
				else return TRUE; 
			}
		}
		break;
	}

go_result:
	if(bflag)
	{
		SendSystemMsg( IDS_USER_ITEM_TOO_LOW_ABILITY, SYSTEM_NORMAL, TO_ME);
		return FALSE;
	}

	strMsg.Format( IDS_USER_ITEM_OTHER_CLASS );
	/*for(i = 0; i < USE_CLASS_TYPE; i++)
	{
		if(g_BrawlClass[i] == armClass) strMsg.Insert(0, _ID(IDS_USER_BRAWL));
		if(g_StaffClass[i] == armClass) strMsg.Insert(0, _ID(IDS_USER_STAFF));
		if(g_EdgedClass[i] == armClass) strMsg.Insert(0, _ID(IDS_USER_EDGED));
		if(g_FireArmsClass[i] == armClass) strMsg.Insert(0, _ID(IDS_USER_FIREARMS)); 
	} Finito Removed not Needed */
	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//	�ش� ������ ������ �ʱ�ȭ�Ѵ�.
//
void USER::ReSetItemSlot(ItemList *pItem)
{
	if(!pItem) return;

	pItem->sLevel = 0;
	pItem->sSid		= -1;
	pItem->sDuration  = 0;
	pItem->sBullNum	= 0;
	pItem->sCount		= 0;
	::ZeroMemory(pItem->tMagic, MAGIC_NUM);
	pItem->tIQ = 0;
	pItem->iItemSerial = 0;

	pItem->dwTime = 0;
	pItem->uid[0] = pItem->uid[1] = pItem->uid[2] = -1;
	pItem->SuccessRate[0] = pItem->SuccessRate[1] = pItem->SuccessRate[2] = 0;
}

/////////////////////////////////////////////////////////////////////////////
//	������ �ű�� ���� ��Ŷ ����
//
void USER::SendItemMoveFail()
{
	CBufferEx TempBuf;

	TempBuf.Add(ITEM_MOVE_RESULT);
	TempBuf.Add((BYTE)FAIL);

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////
//	�������� EQUIP, INVENTORY, EBody Slot �� ��� ���ϴ��� �Ǵ��Ѵ�.
//	UpDate : 2001. 4. 18
int USER::GetSlotClass(int nSlot, int nType, BOOL flag)
{
	if(nSlot >= 0 && nSlot < EQUIP_ITEM_NUM) return 0;						// Equip
	else if(nSlot >= EQUIP_ITEM_NUM && nSlot < TOTAL_INVEN_MAX) return 1;	// Inven
	else if(nSlot >= TOTAL_INVEN_MAX && nSlot < TOTAL_ITEM_NUM) return 2;	// EBody
	else return -1;
}

//////////////////////////////////////////////////////////////////////////////
//	����ִ� ������ ������ ã�´�.
//
int USER::GetEmptySlot(int nSlot)
{
	int i = 0;

	switch(nSlot)
	{
	case EQUIP_SLOT:
		for(i = 0; i < EQUIP_ITEM_NUM; i++)
		{
			if(m_UserItem[i].sSid == -1) return i;
		}
		break;
	case INVENTORY_SLOT:
		for(i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
		{
			if(m_UserItem[i].sSid == -1) return i;
		}
		break;
	case BANK_SLOT:
		for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
		{
			if(m_UserBankItem[i].sSid == -1) return i;
		}
		break;
	case ACCOUNT_BANK_SLOT:
		for(i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++)
		{
			if(m_AccountBankItem[i].sSid == -1) return i;
		}
		break;
	case GUILD_SLOT:
		for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
		{
			if(m_GuildItem[i].sSid == -1) return i;
		}
		break;
	default:
		break;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
//	���� ������ �������� �ִ��� ã�´�.
//
int USER::GetSameItem(ItemList Item, int nSlot)
{
	int i = 0;
	int j = 0;
	BOOL nFlag = FALSE;

	if(Item.sSid < 0 || Item.sSid >= g_arItemTable.GetSize()) return -1;
	if(g_arItemTable[Item.sSid]->m_sDuration > 0) return -1;		// �������� ������ ���� ��ġ�� �ʴ´�.
	
	switch(nSlot)
	{
	case INVENTORY_SLOT:
		for(i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
		{
			if(m_UserItem[i].sSid == Item.sSid &&
			   m_UserItem[i].sDuration == Item.sDuration)  
			{
				for(j = 0; j < MAGIC_NUM; j++)
				{
					if(m_UserItem[i].tMagic[j] != Item.tMagic[j]) { nFlag = TRUE; break; }
				}
					
				if(nFlag) break;

				if(m_UserItem[i].tIQ == Item.tIQ) return i;
			}
		}
		break;
	case TRADE_SLOT:
		EnterCriticalSection( &m_CS_ExchangeItem );
		for(i = 0; i < m_arExchangeItem.GetSize(); i++)
		{
			if ( m_arExchangeItem[i] == NULL ) continue;

			if(m_arExchangeItem[i]->changedList.sSid == Item.sSid &&
			   m_arExchangeItem[i]->changedList.sDuration == Item.sDuration &&
			   m_arExchangeItem[i]->changedList.sBullNum == Item.sBullNum)
			{
				for(j = 0; j < MAGIC_NUM; j++)
				{
					if(m_arExchangeItem[i]->changedList.tMagic[j] != Item.tMagic[j]) { nFlag = TRUE; break; }
				}
					
				if(nFlag) break;

				if ( m_arExchangeItem[i]->changedList.tIQ == Item.tIQ)
				{
					LeaveCriticalSection( &m_CS_ExchangeItem );
					return i;
				}
			}
		}
		LeaveCriticalSection( &m_CS_ExchangeItem );
		break;
	case BANK_SLOT:
		for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)				// 
		{
			if(m_UserBankItem[i].sSid == Item.sSid &&
			   m_UserBankItem[i].sDuration == Item.sDuration &&
			   m_UserBankItem[i].sBullNum == Item.sBullNum)  
			{
				for(j = 0; j < MAGIC_NUM; j++)
				{
					if(m_UserBankItem[i].tMagic[j] != Item.tMagic[j]) { nFlag = TRUE; break; }
				}
					
				if(nFlag) break;

				if(m_UserBankItem[i].tIQ == Item.tIQ) return i;
			}
		}
		break;
	case ACCOUNT_BANK_SLOT:
		for(i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++)				// 
		{
			if(m_AccountBankItem[i].sSid == Item.sSid &&
			   m_AccountBankItem[i].sDuration == Item.sDuration &&
			   m_AccountBankItem[i].sBullNum == Item.sBullNum)  
			{
				for(j = 0; j < MAGIC_NUM; j++)
				{
					if(m_AccountBankItem[i].tMagic[j] != Item.tMagic[j]) { nFlag = TRUE; break; }
				}
					
				if(nFlag) break;

				if(m_AccountBankItem[i].tIQ == Item.tIQ) return i;
			}
		}
		break;
	case GUILD_SLOT:
		for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)				// 
		{
			if(m_GuildItem[i].sSid == Item.sSid &&
			   m_GuildItem[i].sDuration == Item.sDuration &&
			   m_GuildItem[i].sBullNum == Item.sBullNum)  
			{
				for(j = 0; j < MAGIC_NUM; j++)
				{
					if(m_GuildItem[i].tMagic[j] != Item.tMagic[j]) { nFlag = TRUE; break; }
				}
					
				if(nFlag) break;

				if(m_GuildItem[i].tIQ == Item.tIQ) return i;
			}
		}
		break;

	default:
		break;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////
//	�������� ��� ��Ҹ� ã�Ƽ� ������. 
//
void USER::SendQuickChange()
{
/*	int i, j;
	int iChangeNum = 0;
	BOOL bChange = TRUE;
	int tChangeSlot[QUICK_ITEM];
	memset(tChangeSlot, -1, sizeof(tChangeSlot));

	int iQuick = TOTAL_ITEM_NUM;

	CBufferEx TempBuf;

	for(i = 0; i < QUICK_ITEM; i++)
	{
		if(m_UserItem[iQuick].sSid != -1)
		{
			for(j = EQUIP_ITEM_NUM; j < TOTAL_ITEM_NUM; j++)
			{
				if(m_UserItem[iQuick].sSid == m_UserItem[j].sSid) { bChange = FALSE; break; }
			}

			if(bChange == TRUE) 
			{ 
				m_UserItem[i+TOTAL_ITEM_NUM].sSid = -1; 
				tChangeSlot[i] = i; iChangeNum++; 
			}
		}
		iQuick++;
		bChange = TRUE;
	}

	if(iChangeNum == 0) return;

	short sCount = -1;
	TempBuf.Add(ITEM_MOVE_RESULT);
	TempBuf.Add((BYTE)0x01);			// ����
	TempBuf.Add((BYTE)0x02);			// �������� ��� ���

	TempBuf.Add((BYTE)iChangeNum);

	for(i = 0; i < QUICK_ITEM; i++)
	{
		if(tChangeSlot[i] != -1)
		{
			TempBuf.Add((BYTE)i);
			TempBuf.Add(sCount);
		}
	}

	Send(TempBuf, TempBuf.GetLength());
*/
}

/////////////////////////////////////////////////////////////////////////////////
//	�� �������� ����Ѵ�...
//
void USER::ItemUseReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if( m_iDisplayType == 6) return; //yskang 0.5
#endif

	int iMode = 0;
	int index = 0;
	short sSid = 0;
	short sValue = 0;
	int iRandom = 0;

	short sSP = -1;	
	short sHP = -1;	
	short sPP = -1;	

	CString strMsg = _T("");

	BYTE tResult = SUCCESS;
	BOOL bRes = FALSE, bWeight = FALSE;
	int iInvenSlot = 0;

	DWORD dwDN = 0;

	int type = 0;

	BYTE tType = GetByte(pBuf, index);
	BYTE tSlot = GetByte(pBuf, index);
//	sSid = GetShort(pBuf, index);

	BYTE tUsePotion = 0;

	CBufferEx TempBuf,TempBuf1,TempBuf2, PotionBuf;

	TempBuf.Add(ITEM_USE_RESULT);
	
	/************************����ó��****************************************/
	if(tType != 1 && tType != 2) { bRes = TRUE; goto go_result; } 
	if(tSlot < EQUIP_ITEM_NUM || tSlot >= TOTAL_INVEN_MAX) { bRes = TRUE; goto go_result; } 

	sSid = m_UserItem[tSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { bRes = TRUE; goto go_result; } 
																		// ���� �Ӽ� ���� �ٿ��������� ITEM_MAX_USE_WEAR�� ���
	if(g_arItemTable[sSid]->m_byWear < 100 || g_arItemTable[sSid]->m_byWear > ITEM_MAX_USE_WEAR) 
	{
		if(g_arItemTable[sSid]->m_byWear != 15) { bRes = TRUE; goto go_result; } // �̺�Ʈ ����...
	} 

	if(m_UserItem[tSlot].sCount <= 0) { bRes = TRUE; goto go_result; } 

	if(g_arItemTable[sSid]->m_sBullNum != 0)
	{
		if(m_UserItem[RIGHT_HAND].sSid < 0) 
		{
			strMsg.Format( IDS_USER_CANT_USE_THIS, g_arItemTable[sSid]->m_strName);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			bRes = TRUE; 
			goto go_result; 
		} 
		if(g_arItemTable[sSid]->m_byBullType != g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_byBullType)
		{
			strMsg.Format( IDS_USER_CANT_USE_THIS, g_arItemTable[sSid]->m_strName);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			bRes = TRUE; 
			goto go_result;
		}
	}

	/************************���� ��ƾ***************************************/
	iMode = GetItemFieldNum((int)sSid);
	TempBuf.Add(tResult);
	TempBuf.Add((BYTE)iMode);

	switch(iMode)												//@@@@@@@@���� �ݿ� �ʿ�!!!!!
	{
	case QUICK_ITEM_SP:											//	SP			
		sValue = g_arItemTable[sSid]->m_sRepairSP;
		m_sSP =  m_sSP + sValue;
		if(m_sMagicMaxSP <= m_sSP) m_sSP = m_sMagicMaxSP;

		sSP = m_sSP;
		TempBuf.Add(sSP);										// ȸ�� ��ġ
		TempBuf.Add(sHP);
		TempBuf.Add(sPP);
		TempBuf.Add((short)-1);
		bWeight = TRUE;
		tUsePotion = SP_POTION;
		break;

	case QUICK_ITEM_PP:											//	PP
		sValue = g_arItemTable[sSid]->m_sRepairPP;
		sValue += (short)((double)sValue * m_DynamicEBodyData[EBODY_PP_RECOVERY_UP] / 100);
		m_sPP =  m_sPP + sValue;
		if(m_sMagicMaxPP <= m_sPP) m_sPP = m_sMagicMaxPP;

		sPP = m_sPP;
		TempBuf.Add(sSP);										// ȸ�� ��ġ
		TempBuf.Add(sHP);
		TempBuf.Add(sPP);
		TempBuf.Add((short)-1);
		bWeight = TRUE;
		tUsePotion = PP_POTION;
		break;

	case QUICK_ITEM_HP:											//	HP
		sValue = g_arItemTable[sSid]->m_sRepairHP;
		sValue += (short)((double)sValue * m_DynamicEBodyData[EBODY_HP_RECOVERY_UP] / 100);
		m_sHP =  m_sHP + sValue;
		if(m_sMagicMaxHP <= m_sHP) m_sHP = m_sMagicMaxHP;

		sHP = m_sHP;
		TempBuf.Add(sSP);										// ȸ�� ��ġ
		TempBuf.Add(sHP);
		TempBuf.Add(sPP);
		TempBuf.Add((short)-1);
		bWeight = TRUE;
		tUsePotion = HP_POTION;

		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
		break;

	case QUICK_ITEM_ALLREPAIR:									//	SP, PP, HP ��� ȸ��
		m_sSP += g_arItemTable[sSid]->m_sRepairSP;
		m_sPP += g_arItemTable[sSid]->m_sRepairPP;
		m_sHP += g_arItemTable[sSid]->m_sRepairHP;

		if(m_sMagicMaxSP <= m_sSP) m_sSP = m_sMagicMaxSP;
		if(m_sMagicMaxPP <= m_sPP) m_sPP = m_sMagicMaxPP;
		if(m_sMagicMaxHP <= m_sHP) m_sHP = m_sMagicMaxHP;
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
		sSP = m_sSP; sHP = m_sHP; sPP = m_sPP;
		TempBuf.Add(sSP);										// ȸ�� ��ġ
		TempBuf.Add(sHP);
		TempBuf.Add(sPP);
		TempBuf.Add((short)-1);

		bWeight = TRUE;
		tUsePotion = ALL_POTION;
		break;

	case QUICK_ITEM_BULL:										// �ش� �ѱ� ź�˸� ä��
		m_UserItem[RIGHT_HAND].sBullNum = g_arItemTable[sSid]->m_sBullNum;
		SendBullNum(RIGHT_HAND);
		bWeight = TRUE;
		break;
	case QUICK_ITEM_TELEPORT:
		sSid = GetByte(pBuf, index);
		GetHanyGate(tSlot, sSid);
		bWeight = TRUE;
		return;

	case QUICK_ITEM_POTAL:
		if(g_arItemTable[sSid]->m_sPid == 44)			//���� ȸ�� ������
		{
			DWORD dwCurrTick = GetTickCount();
			if (dwCurrTick - m_dwLastItem44Time < 5*1000)
			{
				return ;
			}else{
				m_dwLastItem44Time = dwCurrTick;
			}
			type = 0, index = 0;
			int iRepeat = 0;
			int rank = m_sCityRank + CITY_RANK_INTERVAL;

			if(rank <= DEATH_RANK || m_sKillCount >= 100) 
			{
				SendSystemMsg( IDS_USER_TOO_LOW_CITY_VALUE, SYSTEM_ERROR, TO_ME);
				return;
			}
			if(m_curz==409) 
			{
				SendSystemMsg( "BOSS���޷�ʹ������ҩˮ.", SYSTEM_ERROR, TO_ME);
				return;
			}

			if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) { bRes = TRUE; goto go_result; }

			MAP *pMap = g_zone[m_ZoneIndex];
			if(!pMap) { bRes = TRUE; goto go_result; } 

			type = CheckInvalidMapType();
			if(!CheckInvalidZoneState(type)) { bRes = TRUE; goto go_result; } 

			do
			{
				int min_x = m_curx - 12; if(min_x < 0) min_x = 0;
				int max_x = m_curx + 12; if(max_x >= pMap->m_sizeMap.cx) max_x = pMap->m_sizeMap.cx - 1;
				int min_y = m_cury - 12; if(min_y < 0) min_y = 0;
				int max_y = m_cury + 12; if(max_y >= pMap->m_sizeMap.cy) max_y = pMap->m_sizeMap.cy - 1;
				
				int move_x = 0, move_y =0;
				
				move_x = myrand(min_x, max_x);
				move_y = myrand(min_y, max_y);

//				type = CheckInvalidMapType();
				type = ((g_zone[m_ZoneIndex]->m_pMap[move_x][move_y].m_dwType & 0xFF00 ) >> 8);
				
				if(!CheckInvakidZoneChangeState(type)) { bRes = TRUE; goto go_result; } 
	//			if(type > 1)		// ���ø� �����...
/*				if(type > 1 && type < 8)
				{
					index = g_arMapTable[type]->m_sStoreIndex;
					if(index < 0 || index >= g_arStore.GetSize()) { bRes = TRUE; goto go_result; } 
					if(g_arStore[index]->m_lUsed == 1)	{ bRes = TRUE; goto go_result; } 
				}
*/				
				if(Teleport(move_x, move_y) == FALSE) iRepeat++;
				else break;
				
			}while(iRepeat < 6);

			if( iRepeat >= 6 )
			{
				bRes = TRUE; goto go_result;
			}
		}
		else					// ��ȯ �ֹ���
		{
			if(m_tGuildHouseWar == GUILD_WARRING) { bRes = TRUE; goto go_result; } 
			if(IsCity()) { bRes = TRUE; goto go_result; } 
//			TownPotal();
			if(m_tQuestWar == GUILD_WARRING) g_QuestEventZone.CheckUserStateInEventZone(m_curz);

			if( !ReturnTown( tType, tSlot ) ) return;
		}

		bWeight = TRUE;
		break;

/*	case QUICK_ITEM_CONFUSE:									//  ȥ�� ġ��
		break;
	case QUICK_ITEM_PALSY:										//  ���� ġ��
		break;
	case QUICK_ITEM_POISON:										//  �ߵ� ġ��
		break;
	case QUICK_ITEM_VISION:										//	�÷� ġ��
		break;
	case QUICK_ITEM_ALLCURE:									//	��� �����̻� ġ��
		break;
*/
	case QUICK_ITEM_SALUTE:					// ����
		if(sSid == NORMAL_SALUTE_SID)			tUsePotion = SALUTE;
		else if(sSid == XMAS_SALUTE_SID)		tUsePotion = XMAS_SALUTE;
		else if(sSid == YEAR_SALUTE_SID)		tUsePotion = YEAR_SALUTE;
		else tUsePotion = SALUTE;

		bWeight = TRUE;
		break;

	case EVENT_ITEM_LOTTERY:
		dwDN = CheckLottery();		
		ReSetItemSlot(&m_UserItem[tSlot]);
		CheckMaxValue(m_dwDN, dwDN);
		MakeMoneyLog( dwDN, ITEMLOG_MONEY_LOTTERY );
		FlushItemLog( TRUE );
		bWeight = TRUE;
		break;

	case EVENT_ITEM_DEADPOISON:
		tUsePotion = DEAD_POTION;//ʥҩ
		{			
			int skill_point = 0;
			if( m_sLevel <= 60 )
				skill_point = (int)( (double)m_sLevel / 2 + 0.5 );
			else if( m_sLevel > 60 && m_sLevel <= 99 )
				skill_point = (int)( 30 + (double)m_sLevel - 60 + 0.5 );
			else if( m_sLevel > 99 )
				skill_point = (int)( 30 + 99 - 60 + 0.5 );
			else
			{
				return;
			}
			if(sSid==1022){
				switch (m_byClass){
					case 0://ȭ
						m_sSTR = 14;
						m_sCON = 15;
						m_sDEX = 10;
						m_sVOL = 9;	
						m_sWIS = 9;
						break;
					case 1:
						m_sSTR = 9;
						m_sCON = 10;
						m_sDEX = 9;
						m_sVOL = 17;
						m_sWIS = 12;
						break;
					case 2:
						m_sSTR = 15;
						m_sCON = 14;
						m_sDEX = 10;
						m_sVOL = 9;
						m_sWIS = 9;
						break;
					case 3:
						m_sSTR = 10;
						m_sCON = 12;
						m_sDEX = 17;
						m_sVOL = 9;
						m_sWIS = 9;
						break;
				}
				m_sPA = (short)(m_sLevel-1)+8;
			}else{
				m_sSTR = m_sTempSTR;					// ��
				m_sCON = m_sTempCON;					// �ǰ�
				m_sDEX = m_sTempDEX;					// ��ø��
				m_sVOL = m_sTempVOL;					// ����
				m_sWIS = m_sTempWIS;// ����
				m_sPA = (short)(m_sLevel-1);
			}
			DelMyEventNum(251);
			for(int i = 0; i < TOTAL_SKILL_NUM; i++)
			{
				m_UserSkill[i].sSid=i;
				m_UserSkill[i].tLevel = 0;
			}
	
			m_UserSkill[4].sSid = 4;
			m_UserSkill[9].sSid = 9;
			m_UserSkill[14].sSid = 14;
			m_UserSkill[19].sSid = 19;

//			CheckMaxValue((short &)m_sSkillPoint, (short)skill_point);
			m_sSkillPoint = (short)skill_point;
			if(m_sLevel >= 130){
				m_sSkillPoint_= m_sLevel - 129;
				if(m_sSkillPoint_ >60)
					m_sSkillPoint_=60;
				for(int i = 0; i < 12; i++)
				{
					m_UserSkill_[i].sSid=i;
					m_UserSkill_[i].tLevel = 0;
				}
			}
			SetUserToMagicUser();

			UpdateUserData();

			SendUserStatusSkill();

			bWeight = TRUE;
		}
		break;

	case EVENTITEM_WEAR_MOON:
		if(MoonEvent(tSlot)) bWeight = TRUE;
		else bRes = TRUE;

		break;

	case EVENTITEM_WEAR_INVASION:	// ħ�� �̺�Ʈ�� ��ȯ ������
		if(InvaderSummon(tSlot)) bWeight = TRUE;
		else bRes = TRUE;
		break;

	case SPECIAL_POTION_WEAR:
		tUsePotion = UseSpecialPotion(tSlot);
		if(tUsePotion)
		{
			MakeItemLog( &m_UserItem[tSlot], ITEMLOG_SPECIAL_USE_POTION );
			FlushItemLog( TRUE );

			bWeight = TRUE;
		}
		else bRes = TRUE;

		break;

	case PSI_STONE_WEAR:
		if(UsePsiStone(tSlot) == TRUE)
		{
			MakeItemLog( &m_UserItem[tSlot], ITEMLOG_SPECIAL_USE_POTION );
			FlushItemLog( TRUE );

			bWeight = TRUE;
		}
		else bRes = TRUE;

		break;
	case 132://�ػ���Դ
		if(m_UserItem[38].sSid== -1) return;
		tUsePotion = DEAD_POTION;
		m_UserItem[38].sDuration=m_UserItem[38].sDuration+360;
		if(m_UserItem[38].sDuration>1800)
			m_UserItem[38].sDuration=1800;
		TempBuf1.Add(ITEM_DURATION);
		TempBuf1.Add((byte)38);
		TempBuf1.Add(m_UserItem[38].sDuration);
		Send(TempBuf1, TempBuf1.GetLength());
		bWeight = TRUE;

		break;
	default:
		bRes = TRUE;
		break;
	}

go_result:

	if(bWeight) m_iCurWeight -= g_arItemTable[sSid]->m_byWeight;		// ����ŭ ���Ը� ���δ�.

	if(bRes)
	{
		tResult = FAIL;
		TempBuf2.Add(ITEM_USE_RESULT);
		TempBuf2.Add(tResult);
		Send(TempBuf2, TempBuf2.GetLength());
		return;
	}

	UpdateUserItemDN();							// ������ ���� üũ...

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

	TempBuf.Add((BYTE)tSlot);

	if(iMode == EVENT_ITEM_LOTTERY) { TempBuf.Add(dwDN); Send(TempBuf, TempBuf.GetLength()); return; }

	m_UserItem[tSlot].sCount -= 1;		//
	if(m_UserItem[tSlot].sCount <= 0) { m_UserItem[tSlot].sSid = -1; sSid = -1; }//SendQuickChange(); }

	TempBuf.Add(m_UserItem[tSlot].sLevel);
	TempBuf.Add(sSid);
	TempBuf.Add(m_UserItem[tSlot].sDuration);
	TempBuf.Add(m_UserItem[tSlot].sBullNum);
	TempBuf.Add(m_UserItem[tSlot].sCount);
	for(int i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tSlot].tMagic[i]);
	TempBuf.Add(m_UserItem[tSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());

	type = CheckInvalidMapType();
									
	if(CheckInvalidZoneState(type))		// �Ϲ� �ʵ��̸�
	{
		if(tUsePotion)
		{
			if(g_PotionViewOnOff == 1) return;	// ��ڰ� ���� ȿ���� ������ �ʵ��� �����Ѵ�

			PotionBuf.Add(USE_POTION);
			PotionBuf.Add(m_uid + USER_BAND);
			if(tUsePotion==101||tUsePotion==102){
				PotionBuf.Add((BYTE)0xf);
				PotionBuf.Add((int)(tUsePotion-100));
			
			}else{
				PotionBuf.Add((BYTE)tUsePotion);
			}
			if(m_curz == 61 || m_curz == 62 || m_curz == 63) SendInsight( PotionBuf, PotionBuf.GetLength() );
			else SendExactScreen( PotionBuf, PotionBuf.GetLength() );
		}
	}

//	else Send(PotionBuf, PotionBuf.GetLength());	// ������ �Ǵ� �ʵ� �������϶��� ������ �ʴ´�.

//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
}

/////////////////////////////////////////////////////////////////////////////////
//	���� ��÷���θ� üũ�Ѵ�. 
//
DWORD USER::CheckLottery()
{
	int type = myrand(1, 10);
	DWORD dwDN = 0;

	int iMax = 0;

	switch(type)
	{
	case 1:
		iMax = myrand(1, _MAX_INT);
		if(iMax == 1) dwDN = 10000000;
		break;
	case 2:
		iMax = myrand(1, 1000000000);
		if(iMax == 1) dwDN = 8000000;
		break;
	case 3:
		iMax = myrand(1, 100000000);
		if(iMax == 1) dwDN = 6000000;
		break;
	case 4:
		iMax = myrand(1, 10000000);
		if(iMax == 1) dwDN = 4000000;
		break;
	case 5:
		iMax = myrand(1, 1000000);
		if(iMax == 1) dwDN = 2000000;
		break;
	case 6:
		iMax = myrand(1, 100000);
		if(iMax == 1) dwDN = 1000000;
		break;
	case 7:
		iMax = myrand(1, 10000);
		if(iMax == 1) dwDN = 500000;
		break;
	case 8:
		iMax = myrand(1, 1000);
		if(iMax == 1) dwDN = 250000;
		break;
	case 9:
		iMax = myrand(1, 100);
		if(iMax == 1) dwDN = 100000;
		break;
	case 10:
		iMax = myrand(1, 10);
		if(iMax == 1) dwDN = 10000;
		break;
	}

	return dwDN;
}

/////////////////////////////////////////////////////////////////////////////////
//	������ �ʵ忡�� �ش� Wear ��ȣ�� �˷��ش�.
//
int USER::GetItemFieldNum(int iNum)
{
	BYTE tCure = 0;
	BYTE tHP = 0;
	BYTE tPP = 0;
	BYTE tSP = 0;

	BYTE tResult = 0;

	switch(g_arItemTable[iNum]->m_byWear)
	{
	case 100:			// ġ���� ����..
//		return g_arItemTable[iNum]->m_bySubCure;
		break;
	case 101:			// ȸ���� ����
		if(g_arItemTable[iNum]->m_bySubCure) { tCure = 1; tCure <<= 3; }
		if(g_arItemTable[iNum]->m_sRepairHP) { tHP = 1; tHP <<= 2; }
		if(g_arItemTable[iNum]->m_sRepairPP) { tPP = 1; tPP <<= 1; }
		if(g_arItemTable[iNum]->m_sRepairSP) tSP = 1; 

		tResult = tCure + tHP + tPP + tSP;
		break;

	case 102: case 103:	// �ѱ� ���� ��
		tResult = QUICK_ITEM_BULL;
		break;

	case 104:			// �������� ����, ���̵�
		tResult = QUICK_ITEM_POTAL;
		break;

	case 14:			// 
		break;

	case 15:			// ����
		tResult = EVENT_ITEM_LOTTERY;
		break;

	case 105:			// �ڵ����Ʈ
		tResult = QUICK_ITEM_TELEPORT;
		break;

	case 111:
		tResult = EVENT_ITEM_DEADPOISON;
		break;

	case 113:
		tResult = QUICK_ITEM_SALUTE;
		break;
		
	case 115:
		tResult = EVENTITEM_WEAR_MOON;
		break;

	case 116:
		tResult = SPECIAL_POTION_WEAR;
		break;

	case 117:
		tResult = PSI_STONE_WEAR;
		break;
	case 120:
		tResult = EVENTITEM_WEAR_INVASION;
		break;
	case 132:
		tResult= 132; //�ػ���Դ
		break;
	}

	return tResult;
/*
	if(g_arItemTable[iNum]->m_byWear == 13) return QUICK_ITEM_POTAL;
	if(g_arItemTable[iNum]->m_byWear == 15) return EVENT_ITEM_LOTTERY;
	if(g_arItemTable[iNum]->m_sBullNum)	return QUICK_ITEM_BULL;

	if(g_arItemTable[iNum]->m_bySubCure) { tCure = 1; tCure <<= 3; }
	if(g_arItemTable[iNum]->m_sRepairHP) { tHP = 1; tHP <<= 2; }
	if(g_arItemTable[iNum]->m_sRepairPP) { tPP = 1; tPP <<= 1; }
	if(g_arItemTable[iNum]->m_sRepairSP) tSP = 1; 

	tResult = tCure + tHP + tPP + tSP;
		
	if(tResult == 8) return g_arItemTable[iNum]->m_bySubCure;

	return tResult;
*/
/*
	SubCure : RepairHP : RepairPP : RepairSP : RETURN
		0		1			0			0		4
		0		0			1			0		2
		0		0			0			1		1
		1		0			0			0		m_bySubCure;
		0		1			1			1		7
*/
}

/////////////////////////////////////////////////////////////////////////////////
//	�� �Ǵ� ��� �������� ��������...
//
void USER::ItemGiveReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int		index = 0;

	BYTE tType = GetByte(pBuf, index);
	BYTE tObject = GetByte(pBuf, index);

	if(tType == 1)		// ����
	{
		if(tObject == 0) GiveMoneyToUser(pBuf + index);

		if(tObject == 1) GiveItemToUser(pBuf + index);
	}
	else if(tType == 0) // ��
	{
		if(tObject == 0) GiveMoneyToMap(pBuf + index);

		if(tObject == 1) GiveItemToMap(pBuf + index);
	}

}

/////////////////////////////////////////////////////////////////////////////////
//	��� �������� ����...
//
void USER::GiveMoneyToUser(TCHAR *pBuf)
{
	int		index = 0;

	BOOL nFlag = TRUE;
	BYTE error_code = 0;

	BOOL bBuddyMember;
	int  nMeMapType, nYouMapType;

	CString strMsg;
	USER *pUser = NULL;

	int uid = GetInt(pBuf, index);

	if( (uid - USER_BAND) == m_uid ) return;

	pUser = GetUser(uid - USER_BAND);
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;

	// ���������� ��� �� ȭ��, ���������� ��� ��ȭ���� ����� �������̳� ���� �ټ� ����. zinee 02/12/10
	// �� ������� ���������� ���� ���� ���붧ó�� ��ȭ������ ���ѵȴ�.
	bBuddyMember = CheckBuddyMember(pUser->m_strUserID);

	nMeMapType = CheckInvalidMapType();
	nYouMapType = pUser->CheckInvalidMapType();

	if( !bBuddyMember )
	{
		if(!IsInSight(pUser->m_curx, pUser->m_cury, pUser->m_curz)) return;			
	}
	else 
	if( (nMeMapType == 8 || nMeMapType == 10 || nMeMapType == 15)
		||(nMeMapType == 9 || nMeMapType == 11 || nMeMapType == 16)
		||(nYouMapType == 8 || nYouMapType == 10 || nYouMapType == 15)
		||(nYouMapType == 9 || nYouMapType == 11 || nYouMapType == 16) )
	{
			if( !CheckInvalidZoneState(nMeMapType) || !pUser->CheckInvalidZoneState(nYouMapType) )
			{
				if( m_curz != pUser->m_curz || !GetDistance( pUser->m_curx, pUser->m_cury, 2 ) )
					return;
			}

	}
	else if( m_curz != pUser->m_curz || !GetDistance( pUser->m_curx, pUser->m_cury, (SCREEN_X + SCREEN_Y) ) ) return;

	// �ο����� ������̶��
#ifndef _EVENT_RR 
	if( m_curz == g_RR.m_iZoneNum )
	{
		int metype = CheckInvalidMapType();
		int type = pUser->CheckInvalidMapType();

		if( metype != type ) return;
	}
#else		// �ӽ÷� �ְ� �̺�Ʈ������
	if( m_curz == g_RR.m_iZoneNum || m_curz == 61 || m_curz == 62 || m_curz ==  63)
	{
		int metype = CheckInvalidMapType();
		int type = pUser->CheckInvalidMapType();

		if( metype != type ) return;
	}
#endif

	DWORD	dwDN = GetDWORD(pBuf, index);

	DWORD dwMyDN = 0, dwYourDN = 0, dwTempDN = 0;

	if(pUser->m_bRefuse) { error_code = 0x03; goto go_result; }
	if(dwDN > m_dwDN) { error_code = 0x04; goto go_result; }
	if(dwDN <= 0 || dwDN == _MAX_DWORD) { error_code = 255; goto go_result; }

	dwYourDN = dwTempDN = pUser->m_dwDN;	// �����	
	dwMyDN = m_dwDN;

	if(!CheckMaxValueReturn(dwTempDN, dwDN))// �ִ� DWORD���� �Ѿ���.	
	{
		CheckMaxValue(dwTempDN, dwDN);		// �������� �ǵ��� �ش�.
		if( ((double)dwTempDN - (double)dwDN) < 0 ) return;

		dwDN = dwTempDN - dwDN;				// ������ ���� ������
	}
	else dwTempDN += dwDN;					// �ظ�ŭ ���Ѵ�.

	//----------------[ �ݾ��� ������Ų��.]----------------------------------//
	pUser->m_dwDN = dwTempDN;				// ���ϰ�

	if(m_dwDN <= dwDN) m_dwDN = 0;
	else		       m_dwDN -= dwDN;							// ����

	MakeMoneyLog( dwDN, ITEMLOG_MONEY_GIVE, pUser->m_strUserID, pUser->m_dwDN );

	// ----------------[ Update Databace ] ----------------------------------//
	if(UpdateUserExchangeData(pUser) == FALSE)
	{
		m_dwDN = dwMyDN;
		pUser->m_dwDN = dwYourDN;
		FlushItemLog( FALSE );
		return;
	}

	nFlag = FALSE;							// ���Ǹ� �� �������״�.
	FlushItemLog( TRUE );

go_result:
	if(nFlag)
	{
		CBufferEx TempBuf;

		BYTE result = FAIL;
		TempBuf.Add(ITEM_GIVE_RESULT);
		TempBuf.Add(result);
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return ;
	}

	UpdateUserItemDN();						// ������ ���� üũ...
	SendMoneyChanged();
	pUser->SendMoneyChanged();	

	strMsg = _T("");
	strMsg.Format( IDS_USER_DINA_RECEIVE_FROM, m_strUserID, dwDN);
	pUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
}

/////////////////////////////////////////////////////////////////////////////////
//	���濡�� �������� �ش�.
//
void USER::GiveItemToUser(TCHAR *pBuf)
{
	CBufferEx TempMyBuf, TempYourBuf;
	ItemList MyItem[TOTAL_ITEM_NUM], YourItem[TOTAL_ITEM_NUM];	// �κ� ����� 

	BOOL nFlag = TRUE;
//	BOOL bQuickChange = FALSE;

	BOOL bBuddyMember;

	int  nMeMapType, nYouMapType;

	int iWeight = 0;
	int iSlot = -1;
	int i = 0, index = 0; 
	BYTE error_code = 0;
	int sSid = -1;
	BYTE tSlot = 0;
	short sCount = 0;

	CString strMsg, strMsg1;

	USER *pUser = NULL;

	int uid = GetInt(pBuf, index);

	if( (uid - USER_BAND) == m_uid ) return;

	pUser = GetUser(uid - USER_BAND);
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	if(pUser->m_bRefuse) { error_code = 0x03; goto go_result; }			// ��������� ������ �ޱ� ���� ���¸� ����ó��

#ifdef _ACTIVE_USER
	if(m_iDisplayType == 6) return;				// �ڱ��ڽ��� ���������̰ų� ��� ������ ���������̸� ����
	if(pUser->m_iDisplayType == 6) return; 
#endif

	if(pUser->m_bNowTrading == TRUE || pUser->m_bNoItemMove == TRUE) return;	// Trade �߿��� �ٸ� �������Լ� �������� ���ְ� ���޴´�.

	// ���������� ��� �� ȭ��, ���������� ��� ��ȭ���� ����� �������̳� ���� �ټ� ����. zinee 02/12/10
	// �� ������� ���������� ���� ���� ���붧ó�� ��ȭ������ ���ѵȴ�.
	bBuddyMember = CheckBuddyMember(pUser->m_strUserID);

	nMeMapType = CheckInvalidMapType();
	nYouMapType = pUser->CheckInvalidMapType();

	if( !bBuddyMember )
	{
		if(!IsInSight(pUser->m_curx, pUser->m_cury, pUser->m_curz)) return;			
	}
	else if(  (nMeMapType == 8 || nMeMapType == 10 || nMeMapType == 15)		// ��Ʈ���� ����, ��Ʈ������ ����
			||(nMeMapType == 9 || nMeMapType == 11 || nMeMapType == 16)
			||(nYouMapType == 8 || nYouMapType == 10 || nYouMapType == 15)
			||(nYouMapType == 9 || nYouMapType == 11 || nYouMapType == 16) )
	{
			if( !CheckInvalidZoneState(nMeMapType) || !pUser->CheckInvalidZoneState(nYouMapType) )
			{
				if( m_curz != pUser->m_curz || !GetDistance( pUser->m_curx, pUser->m_cury, 2 ) ) return;
			}

	}
	else if( m_curz != pUser->m_curz || !GetDistance( pUser->m_curx, pUser->m_cury, (SCREEN_X + SCREEN_Y) ) ) return;

	// �ο����� ������̶��
#ifndef _EVENT_RR 
	if( m_curz == g_RR.m_iZoneNum )
	{
		int metype = CheckInvalidMapType();
		int type = pUser->CheckInvalidMapType();

		if( metype != type ) return;
	}
#else		// �ӽ÷� �ְ� �̺�Ʈ������
	if( m_curz == g_RR.m_iZoneNum || m_curz == 61 || m_curz == 62 || m_curz ==  63)
	{
		int metype = CheckInvalidMapType();
		int type = pUser->CheckInvalidMapType();

		if( metype != type ) return;
	}
#endif

	tSlot = GetByte(pBuf, index);
	if(tSlot < EQUIP_ITEM_NUM || tSlot >= EQUIP_ITEM_NUM + INVENTORY_NUM) { error_code = 0x05; goto go_result; }	// �κ��� �ִ°͸� �ټ� ����
	
	sCount = GetShort(pBuf, index);
	if(m_UserItem[tSlot].sCount < sCount || sCount <= 0) { error_code = 0x05; goto go_result; }	// ���ڰ� ���������ΰ� üũ
	if(sCount > MAX_ITEM_NUM) { error_code = 0x02; goto go_result; }	// �κ��� �ִ� ������ 255 (jjs07 03-01-03)

	sSid = m_UserItem[tSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { error_code = 0x05; goto go_result; }
	

	// ���� �������� �����������̰� �ش���������� �׾����� ��Ȱ���������� ���.
	// �� �������� �ϰ��ִ� �����̶�� ��Ȱ��ų�� ����.
	if( sSid == NORMAL_ITEM_REVERSE && pUser->m_bLive == USER_DEAD && bBuddyMember )
	{
		if( CheckInvalidZoneState(nMeMapType) && pUser->CheckInvalidZoneState(nYouMapType) )
		{
			TempMyBuf.Add(REVIVAL_REQ);
			TempMyBuf.AddString( m_strUserID );
			pUser->Send(TempMyBuf, TempMyBuf.GetLength());
			return;
		}
		else 
			SendSystemMsg(IDS_CANNOT_REVERSE, SYSTEM_NORMAL, TO_ME);
			//SendSystemMsg( "��Ʈ�������� ������������ ����� �� �����ϴ�.", SYSTEM_NORMAL, TO_ME);
	}

	if( pUser->m_bLive == USER_DEAD ) return;

	if(g_arItemTable[sSid]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND)
	{
		SendSystemMsg( IDS_USER_CANT_GIVE_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
		error_code = 0x05; 
		goto go_result;
	}
	// �������� �ִµ� ������ 1���� ũ�� ����
	if(g_arItemTable[sSid]->m_sDuration > 0 && m_UserItem[tSlot].sCount >= 2)	{ error_code = 255; goto go_result; }	// if( count > 1 ) FAIL;

	iWeight = sCount * g_arItemTable[sSid]->m_byWeight;
	if(pUser->m_iMaxWeight <= pUser->m_iCurWeight + iWeight)
	{
		SendSystemMsg( IDS_USER_OVER_WEIGHT_OTHER, SYSTEM_ERROR, TO_ME);
		error_code = 0x05; 
		goto go_result;
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];	
		YourItem[i] = pUser->m_UserItem[i];
	}

//	if(g_arItemTable[sSid]->m_byWear <= 5 || g_arItemTable[sSid]->m_byWear == 15)// �������� �־ ������ ������ 1
	if(g_arItemTable[sSid]->m_sDuration > 0)// �������� �־ ������ ������ 1
	{
		iSlot = pUser->GetEmptySlot(INVENTORY_SLOT);
		if(iSlot < 0) { error_code = 0x02; goto go_result; }			// ���� �κ��� ������ ����.

		pUser->m_UserItem[iSlot].sLevel = m_UserItem[tSlot].sLevel;		// ���� ���� �κ��� ä���ְ�...	
		pUser->m_UserItem[iSlot].sSid = m_UserItem[tSlot].sSid;		// ���� ���� �κ��� ä���ְ�...	
		pUser->m_UserItem[iSlot].sCount = 1;
		pUser->m_UserItem[iSlot].sDuration = m_UserItem[tSlot].sDuration;
		pUser->m_UserItem[iSlot].sBullNum = m_UserItem[tSlot].sBullNum;
		pUser->m_UserItem[iSlot].tIQ = m_UserItem[tSlot].tIQ;

		for(i = 0; i < MAGIC_NUM; i++) pUser->m_UserItem[iSlot].tMagic[i] = m_UserItem[tSlot].tMagic[i];

		pUser->m_UserItem[iSlot].iItemSerial = m_UserItem[tSlot].iItemSerial;

		MakeItemLog( &(pUser->m_UserItem[iSlot]), ITEMLOG_GIVE, pUser->m_strUserID );
		ReSetItemSlot(&m_UserItem[tSlot]);							// ���� �� �κ��� �� �ش�.		
	}
	else															// �������� ���� �������� �Ѵ� �������..(ȿ���������� ����)		
	{
		iSlot = pUser->GetSameItem(m_UserItem[tSlot], INVENTORY_SLOT);
		if(iSlot < 0)
		{
			iSlot = pUser->GetEmptySlot(INVENTORY_SLOT);			// ���� �󽽷��� ã��...
			if(iSlot < 0) { error_code = 0x02; goto go_result; }		// ���� �κ��� ������ ����.

			pUser->m_UserItem[iSlot].sLevel = m_UserItem[tSlot].sLevel;	// ���� ���� �κ��� ä���ְ�...	
			pUser->m_UserItem[iSlot].sSid = m_UserItem[tSlot].sSid;	// ���� ���� �κ��� ä���ְ�...	
			pUser->m_UserItem[iSlot].sCount = sCount;				// �� �����̹Ƿ� 
			pUser->m_UserItem[iSlot].sDuration = m_UserItem[tSlot].sDuration;
			pUser->m_UserItem[iSlot].sBullNum = m_UserItem[tSlot].sBullNum;
			pUser->m_UserItem[iSlot].tIQ = m_UserItem[tSlot].tIQ;

			for(i = 0; i < MAGIC_NUM; i++) pUser->m_UserItem[iSlot].tMagic[i] = m_UserItem[tSlot].tMagic[i];

			pUser->m_UserItem[iSlot].iItemSerial = m_UserItem[tSlot].iItemSerial;
		}
		else
		{
//			if(pUser->m_UserItem[iSlot].sCount + sCount > MAX_ITEM_NUM) { error_code = 0x02; goto go_result; }
			pUser->m_UserItem[iSlot].sCount += sCount;				// ���� ����...
		}

		if((m_UserItem[tSlot].sCount - sCount) <= 0)				// ���� �� �κ��� �� �ش�.		
		{															// �������� ��ȭ�� ������		
//			if(g_arItemTable[sSid]->m_byWear >= 6) bQuickChange = TRUE;

			ReSetItemSlot(&m_UserItem[tSlot]);
		}
		else m_UserItem[tSlot].sCount -= sCount;

	}

	// ----------------[ Update Databace ] ----------------------------------//		
	if(UpdateUserExchangeData(pUser) == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)//������Ʈ�� �����ϸ� �������´�.
		{
			m_UserItem[i] = MyItem[i];
			pUser->m_UserItem[i] = YourItem[i];
		}
		FlushItemLog( FALSE );
		return;
	}

	nFlag = FALSE;

go_result:
	BYTE result = SUCCESS;
	TempMyBuf.Add(ITEM_GIVE_RESULT);

	if(nFlag)
	{
		result = FAIL;
		TempMyBuf.Add(result);
		TempMyBuf.Add(error_code);
		Send(TempMyBuf, TempMyBuf.GetLength());
		FlushItemLog( FALSE );
		return ;
	}

	FlushItemLog( TRUE );

	//	���Ը� ������Ų��.
	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	pUser->m_iCurWeight += iWeight;

	UpdateUserItemDN();								// ������ ���� üũ...
	GetRecoverySpeed();								// �ٽ� ȸ���ӵ��� ���
	pUser->GetRecoverySpeed();

	TempMyBuf.Add(result);							// ������ ������.
	TempMyBuf.Add(tSlot);
	TempMyBuf.Add(m_UserItem[tSlot].sLevel);
	TempMyBuf.Add(m_UserItem[tSlot].sSid);
	TempMyBuf.Add(m_UserItem[tSlot].sDuration);
	TempMyBuf.Add(m_UserItem[tSlot].sBullNum);
	TempMyBuf.Add(m_UserItem[tSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempMyBuf.Add(m_UserItem[tSlot].tMagic[i]);
	TempMyBuf.Add(m_UserItem[tSlot].tIQ);

	Send(TempMyBuf, TempMyBuf.GetLength());
//	if(bQuickChange) SendQuickChange();				// �������� ������������...

													// ���濡 ���� ������ ������.		
	TempYourBuf.Add(ITEM_GIVE_RESULT);
	TempYourBuf.Add(result);
	TempYourBuf.Add((BYTE)iSlot);
	TempYourBuf.Add(pUser->m_UserItem[iSlot].sLevel);
	TempYourBuf.Add(pUser->m_UserItem[iSlot].sSid);
	TempYourBuf.Add(pUser->m_UserItem[iSlot].sDuration);
	TempYourBuf.Add(pUser->m_UserItem[iSlot].sBullNum);
	TempYourBuf.Add(pUser->m_UserItem[iSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempYourBuf.Add(pUser->m_UserItem[iSlot].tMagic[i]);
	TempYourBuf.Add(pUser->m_UserItem[iSlot].tIQ);

	strMsg = _T("");
	strMsg.Format( IDS_USER_ITEM_RECEIVE_FROM, g_arItemTable[sSid]->m_strName, m_strUserID); //���߿� ����ȭ_sid
	pUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

	pUser->Send(TempYourBuf, TempYourBuf.GetLength());

	strMsg1 = _T("");
	strMsg1.Format( IDS_USER_ITEM_GIVE_TO, g_arItemTable[sSid]->m_strName, pUser->m_strUserID); //���߿� ����ȭ_sid
	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg1, SYSTEM_NORMAL, TO_ME);

//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
//	pUser->SendItemWeightChange();				// ���� ������ ���Ը� ������.
}

/////////////////////////////////////////////////////////////////////////////////
//	�ʿ� ���� ����...
//
void USER::GiveMoneyToMap(TCHAR *pBuf)
{
	int		index = 0;
	long	lUsed = 0;
	CPoint	ptCell;
	int		iIsItem = 11;	// ���� ���� ��ǥ ��Ϲ���

	BOOL nFlag = TRUE;

	ItemList *pMapItem = NULL;

	DWORD dwMyDN = 0;
	BYTE error_code = 0;

	short sWill_X = GetShort(pBuf, index);
	short sWill_Y = GetShort(pBuf, index);

	DWORD	dwDN = GetDWORD(pBuf, index);

	if(m_tGuildHouseWar == GUILD_WARRING || m_tQuestWar == GUILD_WARRING) 
	{
		SendSystemMsg( IDS_USER_CANT_THROW_IN_QUEST, SYSTEM_ERROR, TO_ME);
		return;
	}

	ptCell = ConvertToServer( sWill_X, sWill_Y );
	if(ptCell.x < 0 || ptCell.y < 0) { error_code = 0x04; goto go_result; }

	ptCell = FindNearRandomPoint(ptCell.x, ptCell.y);							// ���� �ڱ���ǥ�� ������ 24ĭ
	if(ptCell.x < 0 || ptCell.y < 0) { error_code = 0x04; goto go_result; }
	if(ptCell.x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || ptCell.y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) { error_code = 0x04; goto go_result; }

	if(dwDN > m_dwDN) { error_code = 0x04; goto go_result; }
	if(dwDN <= 0 || dwDN == _MAX_DWORD) { error_code = 255; goto go_result; }

	// ���� �ִ� ���� �ο����� ���̶��
#ifndef _EVENT_RR 
	if( m_curz == g_RR.m_iZoneNum )
	{
		int nRet = ((g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_dwType & 0xFF00 ) >> 8);
		int mytype = CheckInvalidMapType();

		if( nRet != mytype ) { error_code = 0x04; goto go_result; }
	}
#else		// �ӽ÷� �ְ� �̺�Ʈ������
	if( m_curz == g_RR.m_iZoneNum || m_curz == 61 || m_curz == 62 || m_curz ==  63)
	{
		int nRet = ((g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_dwType & 0xFF00 ) >> 8);
		int mytype = CheckInvalidMapType();

		if( nRet != mytype ) { error_code = 0x04; goto go_result; }
	}
#endif

	if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, (long)1, (long)0) == (long)0)
	{
		pMapItem = new ItemList;

		if(pMapItem != NULL)											// �޸𸮰� ����� �Ҵ� �Ǿ��ٸ�
		{																		
			pMapItem->tType = TYPE_MONEY;
			pMapItem->dwMoney = 0;

			dwMyDN = m_dwDN; 

			if(m_dwDN <= dwDN) m_dwDN = 0;
			else m_dwDN -= dwDN; 

			MakeMoneyLog( dwDN, ITEMLOG_MONEY_THROW );

			CheckMaxValue((DWORD &)pMapItem->dwMoney, (DWORD)dwDN);
			// ----------------[ Update Databace ] ----------------------------------//
//			if(UpdateUserData() == FALSE)
			if(UpdateUserItemDN() == FALSE)
			{
				m_dwDN = dwMyDN;
				::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0);

				error_code = 255; goto go_result;
			}

			nFlag = FALSE;

			FlushItemLog( TRUE );

			//m_pCom->DelThrowItem();
			m_pCom->SetThrowItem( pMapItem, ptCell.x, ptCell.y, m_ZoneIndex );
		}
		else error_code = 255;

		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0);
	}

go_result:
	if(nFlag)
	{
		CBufferEx TempBuf;

		BYTE result = FAIL;
		TempBuf.Add(ITEM_GIVE_RESULT);
		TempBuf.Add(result);
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());

		if(pMapItem) delete pMapItem;

		FlushItemLog( FALSE );

		return ;
	}

	UpdateUserItemDN();								// ������ ���� üũ...
	SendMoneyChanged();
}

/////////////////////////////////////////////////////////////////////////////////
//	�ʿ� ��������...
//
void USER::GiveItemToMap(TCHAR *pBuf, BOOL bChao)
{
	CBufferEx TempBuf;
	ItemList MyItem[TOTAL_ITEM_NUM];	// �κ� ����� 

	BOOL nFlag = TRUE;
//	BOOL bQuickChange = FALSE;

	int i, index = 0; 
	BYTE error_code = 0;

	CPoint	ptCell;
	int sSid = -1;

	short sWill_X = GetShort(pBuf, index);
	short sWill_Y = GetShort(pBuf, index);

	BYTE tSlot = GetByte(pBuf, index);
	short sCount = GetShort(pBuf, index);

	ItemList *pMapItem = NULL;

	if(m_tGuildHouseWar == GUILD_WARRING || m_tQuestWar == GUILD_WARRING) 
	{
		SendSystemMsg( IDS_USER_CANT_THROW_IN_QUEST, SYSTEM_ERROR, TO_ME);
		return;
	}

	// �� �κ��� ����.
	if(tSlot < 0 || tSlot >= TOTAL_ITEM_NUM) { error_code = 0x05; goto go_result; }
	if( !bChao && (tSlot < EQUIP_ITEM_NUM || tSlot >= TOTAL_INVEN_MAX) ) { error_code = 0x05; goto go_result; }

	sSid = m_UserItem[tSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { error_code = 0x05; goto go_result; }

	if(g_arItemTable[sSid]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND)
	{
		SendSystemMsg( IDS_USER_CANT_THROW_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
		error_code = 0x05; goto go_result;
	}

	if(m_UserItem[tSlot].sCount < sCount || sCount <= 0) { error_code = 0x05; goto go_result; }

	ptCell = ConvertToServer( sWill_X, sWill_Y );
	ptCell = FindNearRandomPoint(ptCell.x, ptCell.y);							// ���� �ڱ���ǥ�� ������ 24ĭ
	if(ptCell.x < 0 || ptCell.y < 0) { error_code = 0x04; goto go_result; }
	if(ptCell.x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || ptCell.y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) { error_code = 0x04; goto go_result; }

	// ���� �ִ� ���� �ο����� ���̶��
#ifndef _EVENT_RR 
	if( m_curz == g_RR.m_iZoneNum )
	{
		int nRet = ((g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_dwType & 0xFF00 ) >> 8);
		int mytype = CheckInvalidMapType();

		if( nRet != mytype ) { error_code = 0x04; goto go_result; }
	}
#else		// �ӽ÷� �ְ� �̺�Ʈ������
	if( m_curz == g_RR.m_iZoneNum || m_curz == 61 || m_curz == 62 || m_curz ==  63)
	{
		int nRet = ((g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_dwType & 0xFF00 ) >> 8);
		int mytype = CheckInvalidMapType();

		if( nRet != mytype ) { error_code = 0x04; goto go_result; }
	}
#endif

	for(i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];		
	}

	if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, (long)1, (long)0) == (long)0)
	{
		pMapItem = new ItemList;

		if(pMapItem != NULL)											// �޸𸮰� ����� �Ҵ� �Ǿ��ٸ�
		{
			pMapItem->tType = TYPE_ITEM;
			pMapItem->sLevel = m_UserItem[tSlot].sLevel;
			pMapItem->sSid = m_UserItem[tSlot].sSid;
			pMapItem->sDuration = m_UserItem[tSlot].sDuration;
			pMapItem->sCount = sCount;
			pMapItem->sBullNum = m_UserItem[tSlot].sBullNum;
			pMapItem->tIQ = m_UserItem[tSlot].tIQ;
			for(i = 0; i < MAGIC_NUM; i++) pMapItem->tMagic[i] = m_UserItem[tSlot].tMagic[i];
			pMapItem->iItemSerial = m_UserItem[tSlot].iItemSerial;

			if(g_arItemTable[sSid]->m_sDuration <= 0)
			{
				if((m_UserItem[tSlot].sCount - sCount) <= 0) 
				{ 
					// ������ �α׸� ���ܾ� �Ұ� ���� �ѵ�... ��ġ�� �������̶�, �ּ�ó����
					//if( !bChao )	MakeItemLog( pMapItem, ITEMLOG_THROW );
					//else			MakeItemLog( pMapItem, ITEMLOG_DEAD_THROW );
					ReSetItemSlot(&m_UserItem[tSlot]);
				}
				else m_UserItem[tSlot].sCount -= sCount;
			}
			else
			{
				if( !bChao )	MakeItemLog( pMapItem, ITEMLOG_THROW );
				else			MakeItemLog( pMapItem, ITEMLOG_DEAD_THROW );
				ReSetItemSlot(&m_UserItem[tSlot]);
			}

			// ----------------[ Update Databace ] ----------------------------------//		
//			if(UpdateUserData() == FALSE)
			if(UpdateUserItemDN() == FALSE)
			{
				for(i = 0; i < TOTAL_ITEM_NUM; i++)//������Ʈ�� �����ϸ� �������´�.
				{
					m_UserItem[i] = MyItem[i];
				}
				::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0);

				error_code = 255; 
				goto go_result; 
			}

			//m_pCom->DelThrowItem();
			m_pCom->SetThrowItem( pMapItem, ptCell.x, ptCell.y, m_ZoneIndex );
	
			nFlag = FALSE;
		}
		else error_code = 255;

		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0);
	}

go_result:
	BYTE result = SUCCESS;
	TempBuf.Add(ITEM_GIVE_RESULT);

	if(!pMapItem) nFlag = TRUE;

	if(nFlag)
	{
		result = FAIL;
		TempBuf.Add(result);
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());

		if(pMapItem) delete pMapItem;

		FlushItemLog( FALSE );
		return ;
	}

	FlushItemLog( TRUE );

	m_iCurWeight -= g_arItemTable[pMapItem->sSid]->m_byWeight * sCount;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();			// �ٽ� ȸ���ӵ��� ���

	TempBuf.Add(result);
	TempBuf.Add(tSlot);
	TempBuf.Add(m_UserItem[tSlot].sLevel);
	TempBuf.Add(m_UserItem[tSlot].sSid);
	TempBuf.Add(m_UserItem[tSlot].sDuration);
	TempBuf.Add(m_UserItem[tSlot].sBullNum);
	TempBuf.Add(m_UserItem[tSlot].sCount);
	for(int j = 0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[tSlot].tMagic[j]);

	TempBuf.Add(m_UserItem[tSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////////
//	�� �Ǵ� ���� �ִ� �����۸� ��´�.
//
void USER::ItemLoadReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return;

	BYTE tType;				// 1 : ��
							// 2 : ��
	int i, j;
	short sX = 0, sY = 0;
	short sSid = -1;

	MAP* pMap = g_zone[m_ZoneIndex];
	if(!pMap) return;

	ItemList* pItem = NULL;

	int iWeight = 0;
	int mapIndex = -1;
	short Diff = 0;
	int index = 0;

	BYTE	error_code = UNKNOWN_ERR;

	DWORD dwDN = 0, dwMyDN = 0;

	BYTE result = SUCCESS;
	BYTE tRight = FAIL;

	CBufferEx TempBuf, TempBuf2;

	ItemList MyItem[TOTAL_ITEM_NUM];	// �κ� ����� 

	dwMyDN = m_dwDN;					// �� ���� ���
	for(i = 0; i < TOTAL_ITEM_NUM; i++)	// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];		
	}

	tType = GetByte(pBuf, index);

	TempBuf.Add(ITEM_LOAD_RESULT);			// ��Ŷ�� ����

	if(tType == 2)						//@@@@@@@@@@@ ���ʿ��� �κ�...
	{
		sX = GetShort(pBuf, index);
		sY = GetShort(pBuf, index);

		CPoint ptCell = ConvertToServer( sX, sY );

		if(ptCell.x < 0 || ptCell.y < 0) { error_code = ERR_2; goto go_result; }
		if(ptCell.x >= pMap->m_sizeMap.cx || ptCell.y >= pMap->m_sizeMap.cy) { error_code = ERR_2; goto go_result; }

		if(GetDistance(ptCell.x, ptCell.y, 2) == FALSE) { error_code = ERR_1; goto go_result; }
				
		mapIndex = pMap->m_pMap[ptCell.x][ptCell.y].iIndex;
		if(mapIndex < 0) { error_code = ERR_6; goto go_result; }
		
		// IKING 2001.1.
		EnterCriticalSection( &m_pCom->m_critThrowItem );
		if ( m_pCom->m_ThrowItemArray[mapIndex] == NULL )
		{
			LeaveCriticalSection( &m_pCom->m_critThrowItem );
			error_code = ERR_6;
			goto go_result;
		}
		pItem = m_pCom->m_ThrowItemArray[mapIndex]->m_pItem;
		LeaveCriticalSection( &m_pCom->m_critThrowItem );
		//

		if( !pItem )
		{
			pMap->m_pMap[ptCell.x][ptCell.y].iIndex = -1;
			error_code = ERR_6;
			goto go_result;
		}

		//------------------------------------------------------------------------
		//	�����ۿ����� ������ �Ǹ��� �����Ѵ�.
		if( pItem->dwTime > 0 )
		{
			BYTE tNum = 0;
			DWORD curr = ConvertCurTimeToSaveTime();

			if( (curr - pItem->dwTime) <= 62 )
			{
				for(i = 0; i < ITEM_USER_RIGHT_NUM; i++)
				{
					if( pItem->uid[i] >= 0 && pItem->uid[i] + USER_BAND < NPC_BAND )
					{
						if(m_uid == pItem->uid[i]) { tRight = SUCCESS; break; }
					}
					else if( pItem->uid[i] <= -1 ) tNum += 1;
				}

				if(tRight)
				{
					int iRandom = myrand(0, 100);
					if(iRandom > pItem->SuccessRate[i])
					{
						error_code = ERR_6;
						goto go_result;
					}
				}
				else if( tNum != 3 ) 
				{
					error_code = ERR_3;
					goto go_result;
				}
			}
		}
/*		COleDateTime checkTime = COleDateTime(pItem->ThrowTime);
		
		if(checkTime.m_status == COleDateTime::valid)
		{
			CTime cur = CTime::GetCurrentTime();
			CTime last(pItem->ThrowTime);
			CTimeSpan DiffTime;
			DiffTime = cur - last;
			
			if( DiffTime.GetMinutes() <= 1)
			{
				for(i = 0; i < ITEM_USER_RIGHT_NUM; i++)
				{
					if( pItem->uid[i] >= 0 && pItem->uid[i] + USER_BAND < NPC_BAND )
					{
						if(m_uid == pItem->uid[i]) { tRight = SUCCESS; break; }
					}
				}

				if(tRight)
				{
					int iRandom = myrand(0, 100);
					if(iRandom > pItem->SuccessRate[i])
					{
						error_code = ERR_6;
						goto go_result;
					}
				}
				else
				{
					error_code = ERR_3;
					goto go_result;
				}
			}
		}
*/
		tType = pItem->tType;
		if(pItem->tType == TYPE_MONEY)
		{
			if(pItem->dwMoney <= 0 || pItem->dwMoney == _MAX_DWORD) goto go_result;
			if(m_dwDN == _MAX_DWORD) goto go_result;
		}
		else if(pItem->tType == TYPE_ITEM)
		{
			sSid = pItem->sSid;		// ��ϵ��� ���� ������
			if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { error_code = ERR_6; goto go_result; }
			if(pItem->sCount < 0 || pItem->sCount > _MAX_SHORT) { error_code = ERR_6; goto go_result; }
//			if(g_arItemTable[sSid]->m_byWear < 6 && pItem->sCount >= 2) goto go_result;	// �������ִ°��� ���� �ϳ�
			if(g_arItemTable[sSid]->m_sDuration > 0 && pItem->sCount >= 2) { error_code = ERR_6; goto go_result; }	// �������ִ°��� ���� �ϳ�
			
/*			iWeight = g_arItemTable[sSid]->m_byWeight * pItem->sCount;
			if(m_iMaxWeight <= m_iCurWeight + iWeight) 
			{
				SendSystemMsg( IDS_USER_OVER_WEIGHT, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
*/		}
		else goto go_result;

		if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse,(long)1, (long)0) == (long)0)
		{
			if(pItem->tType == TYPE_MONEY)
			{
				if(!CheckMaxValueReturn(m_dwDN, pItem->dwMoney))
				{									// ��, MAX���̸� ������ �ʵ��...
					DWORD iDN = 0;
					CheckMaxValue(m_dwDN, pItem->dwMoney);

					iDN = (DWORD)m_dwDN;
					dwDN = (DWORD)abs((long)(iDN - (DWORD)pItem->dwMoney));
				}

				CheckMaxValue(m_dwDN, pItem->dwMoney);

				MakeMoneyLog( pItem->dwMoney, ITEMLOG_MONEY_PICKUP );
			}
			else
			{
				index = GetSameItem(*pItem, INVENTORY_SLOT);

				if(index != -1)
				{					
					for(i = 1; i < pItem->sCount + 1; i++)
					{
						iWeight = g_arItemTable[pItem->sSid]->m_byWeight * i;
						if(m_iMaxWeight < m_iCurWeight + iWeight) 
						{
							iWeight = g_arItemTable[pItem->sSid]->m_byWeight * (i - 1);
							if(iWeight == 0)
							{
//								SendSystemMsg( IDS_USER_ITEM_OVERFLOW, SYSTEM_ERROR, TO_ME);
								error_code = ERR_4;
								::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 			
								goto go_result;
							}
							Diff = i - 1;
							break;
/*							if(pItem->sCount >= i && iWeight > 0)
							{
								Diff = pItem->sCount - i; // �ʿ� ������ ������ ����
								pItem->sCount -= i;
								break;
							}
*/						}
						Diff = i;
					}

					if(m_iMaxWeight >= m_iCurWeight + iWeight) 
					{
						if(pItem->sCount > Diff && Diff >= 0)
						{
							Diff = pItem->sCount - Diff; // �ʿ� ������ ������ ����
							pItem->sCount -= Diff;
						}
						else Diff = 0;

						CheckMaxValue((short &)m_UserItem[index].sCount, pItem->sCount);
					}
					else
					{
//						SendSystemMsg(IDS_USER_ITEM_OVERFLOW, SYSTEM_ERROR, TO_ME);
						error_code = ERR_4;
						::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 			
						goto go_result;
					}
/*					Diff = MAX_ITEM_NUM - m_UserItem[index].sCount;
					if(Diff == 0) 
					{
						SendSystemMsg( IDS_USER_ITEM_OVERFLOW, SYSTEM_ERROR, TO_ME);
						::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 			
						goto go_result;
					}
					//	�ʿ� �ִ� ������ �κ��� �� �ִ뺸�� ������				
					if(pItem->sCount > Diff && Diff >= 0)
					{
						Diff = pItem->sCount - Diff; // �ʿ� ������ ������ ����
						pItem->sCount -= Diff;
					}
					else Diff = 0;
*/
//					CheckMaxValue((short &)m_UserItem[index].sCount, pItem->sCount);
				}
				else
				{
					index = GetEmptySlot(INVENTORY_SLOT);
					if(index == -1) 
					{
//						SendSystemMsg( IDS_USER_ITEM_OVERFLOW, SYSTEM_ERROR, TO_ME);
						error_code = ERR_5;
						::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 			
						goto go_result;
					} 

					for(i = 1; i < pItem->sCount + 1; i++)
					{
						iWeight = g_arItemTable[pItem->sSid]->m_byWeight * i;
						if(m_iMaxWeight < m_iCurWeight + iWeight) 
						{
							iWeight = g_arItemTable[pItem->sSid]->m_byWeight * (i - 1);
							if(iWeight == 0)
							{
//								SendSystemMsg( IDS_USER_ITEM_OVERFLOW, SYSTEM_ERROR, TO_ME);
								error_code = ERR_4;
								::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 			
								goto go_result;
							}
							Diff = i;
							break;
						}
						Diff = i;
					}

					if(m_iMaxWeight >= m_iCurWeight + iWeight) 
					{
						if(pItem->sCount > Diff && Diff >= 0)
						{
							Diff = pItem->sCount - Diff; // �ʿ� ������ ������ ����
							pItem->sCount -= Diff;
						}
						else Diff = 0;

						CheckMaxValue((short &)m_UserItem[index].sCount, pItem->sCount);
					}
					else
					{
//						SendSystemMsg( IDS_USER_ITEM_OVERFLOW, SYSTEM_ERROR, TO_ME);
						error_code = ERR_4;
						::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 			
						goto go_result;
					}				

					m_UserItem[index].sLevel = pItem->sLevel;
					m_UserItem[index].sSid = pItem->sSid;
					m_UserItem[index].sCount = pItem->sCount;
					m_UserItem[index].sDuration = pItem->sDuration;
					m_UserItem[index].sBullNum = pItem->sBullNum;
					m_UserItem[index].tIQ = pItem->tIQ;
					for(j = 0; j < MAGIC_NUM; j++) m_UserItem[index].tMagic[j] = pItem->tMagic[j];
					m_UserItem[index].iItemSerial = pItem->iItemSerial;
					for(j = 0; j < ITEM_USER_RIGHT_NUM; j++) m_UserItem[index].uid[j] = -1;
					m_UserItem[index].dwTime = 0;

					MakeItemLog( &(m_UserItem[index]), ITEMLOG_PICKUP );
				}
			}
			
//			if(UpdateUserData() == FALSE)
			if(UpdateUserItemDN() == FALSE)
			{
				m_dwDN = dwMyDN;
				for(i = 0; i < TOTAL_ITEM_NUM; i++)//������Ʈ�� �����ϸ� �������´�.
				{
					m_UserItem[i] = MyItem[i];
				}

				::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 
				error_code = ERR_7;
				goto go_result;
			}

			if(pItem->tType == TYPE_MONEY && dwDN > 0)
			{
				pItem->dwMoney = dwDN;
				SendItemFieldInfo(ITEM_INFO_PICKUP, TO_INSIGHT, pItem, sX, sY);

				// IKING 2001.1.
				EnterCriticalSection( &m_pCom->m_critThrowItem );
				if ( m_pCom->m_ThrowItemArray[mapIndex] != NULL )
				{
					m_pCom->m_ThrowItemArray[mapIndex]->m_pItem->dwMoney = dwDN;
				}
				LeaveCriticalSection( &m_pCom->m_critThrowItem );
			}
			else if(pItem->tType == TYPE_ITEM && Diff > 0)
			{
				pItem->sCount = Diff;
				SendItemFieldInfo(ITEM_INFO_PICKUP, TO_INSIGHT, pItem, sX, sY);

				// IKING 2001.1.
				EnterCriticalSection( &m_pCom->m_critThrowItem );
				if ( m_pCom->m_ThrowItemArray[mapIndex] != NULL )
				{
					m_pCom->m_ThrowItemArray[mapIndex]->m_pItem->sCount = Diff;
				}
				LeaveCriticalSection( &m_pCom->m_critThrowItem );
			}
			else
			{
				pItem->sSid = -1;
				pItem->dwMoney = 0;
				pItem->sCount = 0;
				SendItemFieldInfo(ITEM_INFO_PICKUP, TO_INSIGHT, pItem, sX, sY);
				pMap->m_pMap[ptCell.x][ptCell.y].iIndex = -1;

				// IKING 2001.1.
				EnterCriticalSection( &m_pCom->m_critThrowItem );
				if ( m_pCom->m_ThrowItemArray[mapIndex] != NULL )
				{
					if(m_pCom->m_ThrowItemArray[mapIndex]->m_pItem)
					{
						delete m_pCom->m_ThrowItemArray[mapIndex]->m_pItem;
						m_pCom->m_ThrowItemArray[mapIndex]->m_pItem = NULL;
						m_pCom->ThrowItemDel(m_pCom->m_ThrowItemArray[mapIndex]);
					}
				}
				LeaveCriticalSection( &m_pCom->m_critThrowItem );
				//
			}

			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0); 			
		}
	}
	else goto go_result;

	// ----------------[ Send Packet ] ----------------------------------//
	
	if(tType == TYPE_ITEM)
	{
		TempBuf.Add(result);
		TempBuf.Add((BYTE)0x01);

		TempBuf.Add((BYTE)index);
		TempBuf.Add(m_UserItem[index].sLevel);
		TempBuf.Add(m_UserItem[index].sSid);
		TempBuf.Add(m_UserItem[index].sDuration);
		TempBuf.Add(m_UserItem[index].sBullNum);
		TempBuf.Add(m_UserItem[index].sCount);
		for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_UserItem[index].tMagic[j]);

		TempBuf.Add((BYTE)m_UserItem[index].tIQ);

		Send(TempBuf, TempBuf.GetLength());

		m_iCurWeight += iWeight;
		GetRecoverySpeed();					// ������ ���Կ� ������ ����� ȸ���ӵ� ��ȯ
//		SendItemWeightChange();				// ���� ������ ���Ը� ������.
	}
	else SendMoneyChanged();

	FlushItemLog( TRUE );
	return;

go_result:

	result = FAIL;
	TempBuf.Add(result);
	TempBuf.Add(error_code);
	Send(TempBuf, TempBuf.GetLength());
	//TRACE("�ݱ� ����\n");

	FlushItemLog( FALSE );
}

/////////////////////////////////////////////////////////////////////////////////
//	�ʿ� �ִ� ������ ������ ������.
//
void USER::SendItemFieldInfo(BYTE type, BYTE towho, ItemList *pItem, int x, int y)
{
	if(!pItem) return;

	CBufferEx TempBuf;

	TempBuf.Add(FIELD_ITEM_INFO);
	TempBuf.Add((short)0x01);
	TempBuf.Add(type);
	TempBuf.Add((short)x);
	TempBuf.Add((short)y);

	if(pItem->tType == TYPE_ITEM)	
	{
		TempBuf.Add(pItem->sSid);
		TempBuf.Add((DWORD)pItem->sCount);
		TempBuf.Add((BYTE)pItem->tIQ);
	}
	else
	{
		TempBuf.Add(TYPE_MONEY_SID);
		TempBuf.Add(pItem->dwMoney);
		TempBuf.Add((BYTE)0);
	}

	if(type == ITEM_INFO_PICKUP) TempBuf.Add((int)(m_uid + USER_BAND));

	switch( towho )
	{

	case TO_ALL:
		SendAll(TempBuf, TempBuf.GetLength());
		break;

	case TO_ME:
		Send(TempBuf, TempBuf.GetLength());
		break;

	case TO_ZONE:
		SendZone(TempBuf, TempBuf.GetLength());
		break;

	case TO_INSIGHT:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;

	default:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;

	}
}

/////////////////////////////////////////////////////////////////////////////////
//	�ʵ�� �� ����(�ݴ� ����� �������� ������.)
//
void USER::GetMapMoneyInfo(DWORD dwDN, CPoint pt, int uid)
{
/*	CBufferEx TempBuf;

	short nPosX = 0;
	short nPosY = 0;

	nPosX = (short)pt.x;
	nPosY = (short)pt.y;

	TempBuf.Add(FIELD_ITEM_INFO);

	if(uid == -1)
	{
		TempBuf.Add(MONEY_INFO_MODIFY);
		TempBuf.Add(nPosX);
		TempBuf.Add(nPosY);
		TempBuf.Add(dwDN);
	}

	if(uid >= 0)
	{
		TempBuf.Add(MONEY_PICK_UP);
		TempBuf.Add(nPosX);
		TempBuf.Add(nPosY);
		TempBuf.Add(uid);
		TempBuf.Add(dwDN);
	}

	Send(TempBuf, TempBuf.GetLength());
	*/
}

/////////////////////////////////////////////////////////////////////////////////
//	���â�� ��� �� �� �ִ� ��ġ�� ���Ѵ�.
//
void USER::GetEquipPos(BYTE byWear, CPoint &pt)
{
	pt.x = -1;
	pt.y = -1;

	if(byWear < 1 || byWear > 8) return;

	switch(byWear)
	{
	case 1:
		pt.x = 4; pt.y = 5;
		break;
	case 2:
		pt.x = 0; pt.y = -1;
		break;
	case 3:
		pt.x = 1; pt.y = -1;
		break;
	case 4:
		pt.x = 8; pt.y = -1;
		break;
	case 5:
		pt.x = 9; pt.y = -1;
		break;
	case 6:
		pt.x = 2; pt.y = -1;
		break;
	case 7:
		pt.x = 3; pt.y = -1;
		break;
	case 8:
		pt.x = 6; pt.y = 7;
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	E-Body â�� �� ��ġ��  ���Ѵ�.
//
int USER::GetEBodyPos(BYTE byWear)
{
//	if((byWear < 122 || byWear > 125)||byWear!=130)return -1;

	switch(byWear)
	{
	case 122:
		return TOTAL_INVEN_MAX;
	case 123:
		return TOTAL_INVEN_MAX + 1;
	case 124:
		return TOTAL_INVEN_MAX + 2;
	case 125:
		return TOTAL_INVEN_MAX + 3;
	case 130:
		return TOTAL_INVEN_MAX + 4;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
//	UserItem �� Buffer�� ������ Copy �Ѵ�. --> ���� ������ ������ ���Ը� �����Ѵ�.
//
void USER::StrToUserItem(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	int temp = 0, value = 0;
	int iWear = -1;
	MYINT64 itemserial;

	for(i = 0; i < TOTAL_ITEM_NUM; i++)
	{
		m_UserItem[i].sLevel	= GetShort(pBuf, index);
		m_UserItem[i].sSid		= GetShort(pBuf, index);
		m_UserItem[i].sDuration = GetShort(pBuf, index);
		m_UserItem[i].sBullNum	= GetShort(pBuf, index);
		m_UserItem[i].sCount	= GetShort(pBuf, index);

		if(m_UserItem[i].sCount <= 0) m_UserItem[i].sSid = -1;

		for(j = 0; j < MAGIC_NUM; j++) 
		{
			m_UserItem[i].tMagic[j] = GetByte(pBuf, index);
		}
		m_UserItem[i].tIQ = GetByte(pBuf, index);

		if( m_UserItem[i].sSid == 625 || m_UserItem[i].sSid == 626 )	// �ں��, Į�θ�
		{
			m_UserItem[i].sDuration = 0;
		}

		for( j = 0; j < 8; j++ )
		{
			itemserial.b[j] = GetByte( pBuf, index );
		}
		m_UserItem[i].iItemSerial = itemserial.i;

		for( j = 0; j < ITEM_USER_RIGHT_NUM; j++ )
		{
			m_UserItem[i].uid[j] = -1;
			m_UserItem[i].SuccessRate[j] = 0;
		}

		m_UserItem[i].dwTime = 0;

		// ������ ��ȯ ���� ������ �þ �ڵ� (������ ����) --------------------------------------------------->>
		if(m_UserItem[i].sSid >= 0 && m_UserItem[i].sSid < g_arItemTable.GetSize())
		{
			if(m_UserItem[i].sDuration > g_arItemTable[m_UserItem[i].sSid]->m_sDuration)
			{
				m_UserItem[i].sDuration = g_arItemTable[m_UserItem[i].sSid]->m_sDuration;
			}

			if(m_UserItem[i].sDuration > 0 && m_UserItem[i].tIQ != UNIQUE_ITEM)
			{
				if(m_UserItem[i].tMagic[ITEM_UPGRADE_COUNT] > 0)		// 6��° ���� Ƚ��
				{
					iWear = g_arItemTable[m_UserItem[i].sSid]->m_byWear;

					if(iWear == ATTACK_ITEM)	// ���ݿ� ����
					{
						 if(m_UserItem[i].tMagic[ITEM_UPGRADE_COUNT] > MAX_ITEM_UPGRADE_COUNT + 1)		// ������ 10���̻��̸� ������ ��������
						 {
							 ReSetItemSlot(&m_UserItem[i]);
							 continue;
						 }

					}
					else if(iWear >= 2 && iWear <= 5)
					{
						 if(m_UserItem[i].tMagic[ITEM_UPGRADE_COUNT] > MAX_ITEM_UPGRADE_COUNT + 1)		// ������ 10���̻��̸� ������ ��������
						 {
							 ReSetItemSlot(&m_UserItem[i]);
							 continue;
						 }
					}
				}
			}

			if( m_UserItem[i].sSid == g_RR.m_iItemSid )				// �� �������� �ξⷳ�� ��ǰ�̰�
			{
				if( g_strARKRRWinner.CompareNoCase( m_strUserID ) )	// ���� ���ڰ� �ƴ� ���
				{
					ReSetItemSlot( &(m_UserItem[i]) );					// ���´�.
				}
			}
		}
		else	// ������ ���̺� ���� �������� �����.
		{
			ReSetItemSlot(&m_UserItem[i]);
		}
		//-----------------------------------------------------------------------------------------------------<<

		if(m_UserItem[i].sSid >= 0 && m_UserItem[i].sSid < g_arItemTable.GetSize())
		{
			if(m_UserItem[i].sCount > 0) m_iCurWeight += (g_arItemTable[m_UserItem[i].sSid]->m_byWeight * m_UserItem[i].sCount);
			else m_iCurWeight += g_arItemTable[m_UserItem[i].sSid]->m_byWeight;
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////
//	Buffer �� UserItem ������ Copy �Ѵ�.
//
void USER::UserItemToStr(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	MYINT64 itemserial;

	for(i = 0; i < TOTAL_ITEM_NUM; i++)
	{
		SetShort(pBuf, m_UserItem[i].sLevel,	index);
		SetShort(pBuf, m_UserItem[i].sSid,		index);
		SetShort(pBuf, m_UserItem[i].sDuration,	index);
		SetShort(pBuf, m_UserItem[i].sBullNum,	index);
		SetShort(pBuf, m_UserItem[i].sCount,	index);

		for(j = 0; j < MAGIC_NUM; j++) SetByte(pBuf, m_UserItem[i].tMagic[j], index);

		SetByte(pBuf, m_UserItem[i].tIQ, index);

		itemserial.i = m_UserItem[i].iItemSerial;

		for( j = 0; j < 8; j++ )
		{
			SetByte( pBuf, itemserial.b[j], index );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Buffer �� �������� ���� UserItem ������ Copy �Ѵ�.
//
void USER::UserItemToStrForSend(TCHAR *pBuf)
{
	int index = 0;
	int i, j;

	for(i = 0; i < TOTAL_ITEM_NUM; i++)
	{
		SetShort(pBuf, m_UserItem[i].sLevel,	index);
		SetShort(pBuf, m_UserItem[i].sSid,		index);
		SetShort(pBuf, m_UserItem[i].sDuration,	index);
		SetShort(pBuf, m_UserItem[i].sBullNum,	index);
		SetShort(pBuf, m_UserItem[i].sCount,	index);

		for(j = 0; j < MAGIC_NUM; j++) SetByte(pBuf, m_UserItem[i].tMagic[j], index);

		SetByte(pBuf, m_UserItem[i].tIQ, index);
	}
}

///////////////////////////////////////////////////////////////////////////////
//	UserSkill �� Buffer �� ������ ī���Ѵ�.
//
void USER::StrToUserSkill(TCHAR *pBuf)
{
	int index = 0,i;
	for(i = 0; i < TOTAL_SKILL_NUM; i++)
	{
		m_UserSkill[i].sSid		= GetShort(pBuf, index);
		m_UserSkill[i].tLevel	= GetByte (pBuf, index);
//		m_UserSkill[i].tOnOff	= GetByte (pBuf, index);
	}
	m_sSkillPoint_=GetByte (pBuf, index);
	//��ȡ130������
	for(i = 0; i < 12; i++){
		m_UserSkill_[i].sSid=i;
		m_UserSkill_[i].tLevel	= GetByte (pBuf, index);
	}
	//��ȡ�ܱ��Ǳ�ʱ��
	m_dwBSTime=GetDWORD(pBuf, index);
	m_dwDNMoney = GetDWORD(pBuf,index);

	if(m_dwBSTime>3600*1000) m_dwBSTime=0;
	if(m_dwBSTime==0)
		m_iSkin=0;
	//�ػ���ʹ

}	

///////////////////////////////////////////////////////////////////////////////
//	Buffer �� UserSkill ������ Copy �Ѵ�.
//
void USER::UserSkillToStr(TCHAR *pBuf)
{
	int	index = 0;
	for(int i = 0; i < TOTAL_SKILL_NUM; i++)
	{
		SetShort(pBuf, m_UserSkill[i].sSid,		index);
		SetByte (pBuf, m_UserSkill[i].tLevel,	index);
	}
	SetByte (pBuf,(byte)m_sSkillPoint_,index);
	for(int i = 0; i < 12; i++)
	{
		//SetShort(pBuf, m_UserSkill_[i].sSid,index);
		SetByte (pBuf, m_UserSkill_[i].tLevel,	index);
	}
	//�����ܱ��Ǳ�ʱ��
	DWORD LostTime=GetTickCount()-m_dwBSLaseTime;
	if(LostTime >=m_dwBSTime)
		m_dwBSTime=0;
	else{
		m_dwBSTime=m_dwBSTime-LostTime;
		m_dwBSLaseTime=GetTickCount();
	}
	SetDWORD(pBuf,m_dwBSTime,index);
	SetDWORD(pBuf,m_dwDNMoney,index);

	//�ػ���ʹ
}

///////////////////////////////////////////////////////////////////////////////
//	UserPsi �� Buffer �� ������ ī���Ѵ�.
//
void USER::StrToUserPsi(TCHAR *pBuf)
{
	int index = 0;

	m_nHavePsiNum = 0;
	for(int i = 0; i < TOTAL_PSI_NUM; i++)
	{

		m_UserPsi[i].sSid	= GetShort(pBuf, index);
		m_UserPsi[i].tOnOff	= GetByte (pBuf, index);
		
		if(m_UserPsi[i].sSid == -1)	break; 
		m_nHavePsiNum++;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Buffer �� UserPsi ������ Copy �Ѵ�.
//
void USER::UserPsiToStr(TCHAR *pBuf)
{
	int	index = 0;

	for(int i = 0; i < TOTAL_PSI_NUM; i++)
	{
		SetShort(pBuf, m_UserPsi[i].sSid,	index);
		SetByte (pBuf, m_UserPsi[i].tOnOff,	index);
		if(m_UserPsi[i].sSid == -1) break;		
		// DB�� Update�� ���
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Buffer �� UserSkillInfo ������ Copy �Ѵ�.
//
void USER::UserSkillInfoToStr(TCHAR *pBuf)
{
	int	index = 0;
	for(int i = 0; i < TOTAL_SKILL_NUM; i++)
	{
		SetByte (pBuf, m_UserSkillInfo[i],	index);
	}
}

///////////////////////////////////////////////////////////////////////////////
//	�ý��� �޼����� �����Ѵ�.
//
void USER::SendSystemMsg(TCHAR *pMsg, BYTE type, int nWho)
{
	CBufferEx TempBuf;

	TempBuf.Add(SYSTEM_MSG);
	TempBuf.Add(type);
	TempBuf.Add(pMsg, _tcslen(pMsg));

	switch(nWho)
	{
	case TO_ALL:
		SendAll(TempBuf, TempBuf.GetLength());
		break;

	case TO_ME:
		Send(TempBuf, TempBuf.GetLength());
		break;

	case TO_ZONE:
		SendZone(TempBuf, TempBuf.GetLength());
		break;

	case TO_INSIGHT:
	default:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;

	}
}

///////////////////////////////////////////////////////////////////////////////
//	Trade Request ���� ó��
//
void USER::SendTradeSuccess(int uid, TCHAR *pstrID)//(int uid)
{
	CBufferEx TempBuf;

	TempBuf.Add(ITEM_TRADE_RESULT);
	TempBuf.Add(uid);
	TempBuf.AddString(pstrID);
	TempBuf.Add(SUCCESS);

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	Trade Request ����ó��
//
void USER::SendTradeFail(int uid, BYTE error_code)
{
	CBufferEx TempBuf;

	TempBuf.Add(ITEM_TRADE_RESULT);
	TempBuf.Add(uid);
	TempBuf.AddString(m_strUserID);
	TempBuf.Add(FAIL);
	TempBuf.Add(error_code);

	Send(TempBuf, TempBuf.GetLength());
/*
	m_bTradeWaiting = FALSE;		// Trade ��û��
	m_bNowTrading = FALSE;			// ���� �ŷ���
	m_bExchangeOk = FALSE;			// ��ȯ�³�
	m_bTradeItem = FALSE;			// �������� �ϳ��� �÷�����...
	m_TradeMoney = 0;   			// �ŷ� ���� �ݾ�

	for(int i = 0; i < TOTAL_ITEM_NUM; i++) // �ŷ��� �κ��� �ٲ�� ������ ���� üũ����
	{ 
		m_TradeItemNum[i].sSid = -1; 
		m_TradeItemNum[i].sNum = 0; 
	}
*/
}

/////////////////////////////////////////////////////////////////////////////////
//	�ŷ���û ó���� �Ѵ�.
//
void USER::ItemTradeReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	//  02-10-12 by Youn Gyu
	int type = CheckInvalidMapType();
	if(!CheckInvalidZoneState(type)) return;

	BYTE result = FAIL;
	BYTE error_code = UNKNOWN_ERR;

	TCHAR szUserName[CHAR_NAME_LENGTH + 1];
	::ZeroMemory(szUserName, sizeof(szUserName));

	USER* pSendUser = NULL;

	int i, index = 0;
	BYTE tMode = 0;

	int uid = GetInt(pBuf, index);

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) // �߸��� �������̵�
	{
		SendTradeFail(uid, ERR_4);
		return;
	}
/*
	if(!CheckGetVarString(CHAR_NAME_LENGTH, szUserName, pBuf, sizeof(BYTE), index))		// �߸��� �����̸��̸� ����
	{
		SendTradeFail(uid, ERR_4);
		return;
	}
*/
	tMode = GetByte(pBuf, index);

	pSendUser = GetUser(uid - USER_BAND);

	if(pSendUser == NULL || pSendUser->m_state != STATE_GAMESTARTED)	// �߸��� �������̵��̸� ����
	{
		SendTradeFail(uid, ERR_4);
		return;
	}
	
/*	if(pSendUser->m_bTradeMode == FALSE)	// ������� �ŷ��غ� �ȵ������� ����
	{
		SendTradeFail(uid, ERR_2);
		return;
	}
*/	
	if(pSendUser->m_bNowTrading == TRUE)	// ������� �ŷ����̸� ����
	{
		SendTradeFail(uid, ERR_3);
		return;
	}
/*	
	if(!GetDistance(pSendUser->m_curx, pSendUser->m_cury, 3)) // ������� �ʹ� �ָ� ������ ������ ����
	{
		SendTradeFail(uid, ERR_5);
		return;
	}
*/
	if(IsThereUser(pSendUser) == FALSE)
	{
		SendTradeFail(uid, ERR_2);
		return;
	}

/*	if(m_bTradeMode == FALSE)				// �ڽ��� �غ� �ȵǾ� �־ ����
	{
		SendTradeFail(uid, UNKNOWN_ERR);
		return;
	}
*/
	switch(tMode)
	{
	case 0:			// Trade ��û�� ����ϴ� ���
		if(m_bTradeWaiting == TRUE && pSendUser->m_bNowTrading == FALSE)					
		{											// ���� �³���, ��û������ �ٽ� ��� �Ҷ�		
			pSendUser->SendTradeFail(m_uid + USER_BAND, (BYTE)0x00);
			m_bTradeWaiting = FALSE;
			return;
		}
		if(pSendUser->m_bNowTrading == TRUE)		// �³��ѵ� �����϶�
		{
			SendTradeFail(uid, (BYTE)0x00);
			pSendUser->SendTradeFail(m_uid + USER_BAND, (BYTE)0x00);
			return;
		}
		return;
	case 1:			// Trade ��û
		m_bTradeWaiting = TRUE;
		break;
	case 2:			// Trade �³�
		if(pSendUser->m_bTradeWaiting != TRUE)
		{
			SendTradeFail(uid, UNKNOWN_ERR);
			return;
		}
		_tcscpy(m_strTradeUser, pSendUser->m_strUserID);
		_tcscpy(pSendUser->m_strTradeUser, m_strUserID);

					// �ŷ� �³��� �ʱ�ȭ �ʿ�
		m_TradeMoney = 0;
		pSendUser->m_TradeMoney = 0;

		m_iTradeUid = uid;
		pSendUser->m_iTradeUid = m_uid + USER_BAND;
		
		m_bNowTrading = TRUE;
		pSendUser->m_bNowTrading = TRUE;

		SendTradeSuccess(uid, pSendUser->m_strUserID);
		pSendUser->SendTradeSuccess(m_uid + USER_BAND, m_strUserID);
//		SendTradeSuccess(uid);
//		pSendUser->SendTradeSuccess(m_uid + USER_BAND);

		for(i = 0; i < TOTAL_ITEM_NUM; i++) 
		{
			m_TradeItemNum[i].sSid = m_UserItem[i].sSid;
			m_TradeItemNum[i].sNum = m_UserItem[i].sCount;
			pSendUser->m_TradeItemNum[i].sSid = pSendUser->m_UserItem[i].sSid;
			pSendUser->m_TradeItemNum[i].sNum = pSendUser->m_UserItem[i].sCount;
		}
		return;
	case 3:			// Trade ����
		SendTradeFail(uid, ERR_6);
		pSendUser->SendTradeFail(uid, ERR_1);
		return;
	default:
		SendTradeFail(uid, UNKNOWN_ERR);
		return;
	}

	CBufferEx TempBuf;

	TempBuf.Add(ITEM_TRADE_REQ);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_strUserID);
	TempBuf.Add(tMode);	
	
	pSendUser->Send(TempBuf, TempBuf.GetLength());

	//yskang 0.4 ��� �α� ����ϱ�
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	sprintf(strOP,"�������ֱ�:ACCOUNT%s : CharID%s",pSendUser->m_strAccount, pSendUser->m_strUserID);
	WriteOpratorLog(strOP,ITEM_LOG);
}

/////////////////////////////////////////////////////////////////////////////
//	������ ��ȯ��û ó���� �Ѵ�.
//
void USER::ExchangeReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	BOOL flag = FALSE;
	int j, iNum = 0;
	int index = 0;
	int uid = 0;
	BYTE byType = 0, bySlot = 0;
	DWORD dwCount = 0;
	BYTE result = 0;
	BYTE error_code = UNKNOWN_ERR;

	short sCount = 0;

	USER* pSendUser = NULL;
	USER* pTempUser = NULL;

//	if(m_bTradeMode == FALSE || m_bNowTrading != TRUE)
	if(m_bNowTrading != TRUE)
	{
		if(m_iTradeUid != -1) 	pSendUser = GetUser(m_iTradeUid - USER_BAND);
		if(pSendUser != NULL) 
		{
			SendExchangeFail((BYTE)0x00, (BYTE)0x02);
			pSendUser->SendExchangeFail((BYTE)0x00, (BYTE)0x02);
		}
		return;
	}

	ItemList	TempItem;
	ExchangeItem* pNewItem = NULL;

	uid		= GetInt(pBuf, index);
	byType	= GetByte(pBuf, index);

	CBufferEx TempBuf;
	CBufferEx TempMyBuf;

	if(uid < 0 || uid >= NPC_BAND)				// �߸��� ����� uid�� �ŷ� ���
	{
		result = 0x00;
		error_code = ERR_1;
		SendExchangeFail(result, error_code);

		pTempUser = GetUser(m_iTradeUid - USER_BAND);

		if(pTempUser) 
		{
			pTempUser->SendExchangeFail(result, UNKNOWN_ERR);
		}

		return;
	}

	pSendUser = GetUser(uid - USER_BAND);		// �߸��� ���� 
	if(m_bPShopOpen==TRUE||pSendUser == NULL || pSendUser->m_state != STATE_GAMESTARTED || pSendUser->m_bPShopOpen==TRUE)
	{
		result = 0x00;
		error_code = ERR_1;
		SendExchangeFail(result, error_code);

		pTempUser = GetUser(m_iTradeUid - USER_BAND);

		if(pTempUser) 
		{
			pTempUser->SendExchangeFail(result, UNKNOWN_ERR);
		}

		return;
	}

	if(_tcscmp(m_strTradeUser, pSendUser->m_strUserID) != 0)		// ó���� �ŷ��ϴ� ������ �ƴ�
	{
		result = 0x00;
		error_code = ERR_2;
		SendExchangeFail(result, error_code);

		pTempUser = GetUser(m_iTradeUid - USER_BAND);

		if(pTempUser) 
		{
			pTempUser->SendExchangeFail(result, UNKNOWN_ERR);
		}

		return;
	}

	switch(byType)
	{
	case 0:				// ��ȯ���
		result = 0x00;
		SendExchangeFail(result, (BYTE)0x04);
		pSendUser->SendExchangeFail(result, (BYTE)0x05);
		return;
	case 1:				// ��ȯ�³�
		if(m_bExchangeOk == FALSE) 
		{
			m_bExchangeOk = TRUE;

			index = 0;
			SetByte(m_TempBuf, EXCHANGE_REQ, index);
			SetInt(m_TempBuf, uid, index);
			SetByte(m_TempBuf, 0x01, index);
			Send(m_TempBuf, index);

			index = 0;
			SetByte(TempBuf, EXCHANGE_REQ, index);
			SetInt(TempBuf, m_uid + USER_BAND, index);
			SetByte(TempBuf, 0x02, index);
			pSendUser->Send(TempBuf, index);

		}
		if(m_bExchangeOk == TRUE && pSendUser->m_bExchangeOk == TRUE)
		{
			EnterCriticalSection( &m_CS_ExchangeItem );
			for(j = 0; j < m_arExchangeItem.GetSize(); j++)		// ���� ���ѿ� �ɸ��� üũ...
			{
				if ( m_arExchangeItem[j] )
				{
					index = 0;
					index = m_arExchangeItem[j]->changedList.sSid;
					if(index < 0 || index >= g_arItemTable.GetSize()) continue;

					if(g_arItemTable[index]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND) 
					{
						LeaveCriticalSection( &m_CS_ExchangeItem );

						result = 0x00;
						SendExchangeFail(result, 0x07);
						pSendUser->SendExchangeFail(result, 0x06);
						SendSystemMsg( IDS_USER_CANT_BUYSELL_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
						pSendUser->SendSystemMsg( IDS_USER_CANT_BUYSELL_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
						return;
					}

					bySlot = m_arExchangeItem[j]->bySlot;				
					sCount = m_arExchangeItem[j]->changedList.sCount;
					iNum += (g_arItemTable[index]->m_byWeight * sCount);
					if(pSendUser->m_iMaxWeight <= pSendUser->m_iCurWeight + iNum)
					{
						LeaveCriticalSection( &m_CS_ExchangeItem );

						result = 0x00;
						SendExchangeFail(result, 0x07);
						pSendUser->SendExchangeFail(result, 0x06);
						SendSystemMsg( IDS_USER_OVER_WEIGHT_OTHER, SYSTEM_ERROR, TO_ME);
						pSendUser->SendSystemMsg( IDS_USER_OVER_WEIGHT_SAVE, SYSTEM_ERROR, TO_ME);
						return;
					}
				}
			}
			LeaveCriticalSection( &m_CS_ExchangeItem );


			EnterCriticalSection( &pSendUser->m_CS_ExchangeItem );
			for(j = 0; j < pSendUser->m_arExchangeItem.GetSize(); j++)		// ���� ���ѿ� �ɸ��� üũ...
			{			
				if ( pSendUser->m_arExchangeItem[j] )
				{
					index = 0;
					index = pSendUser->m_arExchangeItem[j]->changedList.sSid;
					if(index < 0 || index >= g_arItemTable.GetSize()) continue;

					if(g_arItemTable[index]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND) 
					{
						LeaveCriticalSection( &pSendUser->m_CS_ExchangeItem );

						result = 0x00;
						SendExchangeFail(result, 0x07);
						pSendUser->SendExchangeFail(result, 0x06);
						SendSystemMsg( IDS_USER_CANT_BUYSELL_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
						pSendUser->SendSystemMsg( IDS_USER_CANT_BUYSELL_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
						return;
					}

					bySlot = pSendUser->m_arExchangeItem[j]->bySlot;
					sCount = pSendUser->m_arExchangeItem[j]->changedList.sCount;
					iNum += (g_arItemTable[index]->m_byWeight * sCount);
					if(m_iMaxWeight < m_iCurWeight + iNum)
					{
						LeaveCriticalSection( &pSendUser->m_CS_ExchangeItem );

						result = 0x00;
						SendExchangeFail(result, 0x06);
						pSendUser->SendExchangeFail(result, 0x07);
						SendSystemMsg( IDS_USER_OVER_WEIGHT_SAVE, SYSTEM_ERROR, TO_ME);
						pSendUser->SendSystemMsg( IDS_USER_OVER_WEIGHT_SAVE_OTHER, SYSTEM_ERROR, TO_ME);
						return;
					}
				}
			}
			LeaveCriticalSection( &pSendUser->m_CS_ExchangeItem );

			RunExchange(pSendUser);
		}

		break;

	case 2:			// Ÿ���� ��ȯ �³�(�������� �� �� �ִ�)
		result = 0x00;
		error_code = UNKNOWN_ERR;
		SendExchangeFail(result, error_code);
		pSendUser->SendExchangeFail(result, UNKNOWN_ERR);
		return;

	case 3:			// �ڽ��� ������ �ø�
		if(m_bExchangeOk == TRUE) return;		// �����ϸ� ���̻��� ������ �߰��� �ź��Ѵ�.

		bySlot	= GetByte(pBuf, index);
		sCount	= GetShort(pBuf, index);

		if(bySlot < EQUIP_ITEM_NUM || bySlot >= TOTAL_INVEN_MAX) //|| sCount > MAX_ITEM_NUM)
		{
			result = 0x00;
			error_code = ERR_3;

			SendExchangeFail(result, error_code);
			pSendUser->SendExchangeFail(result, error_code);

			return;
		}

		if(sCount <= 0 || sCount >= _MAX_SHORT)
		{
			result = 0x00;
			error_code = ERR_3;

			SendExchangeFail(result, error_code);
			pSendUser->SendExchangeFail(result, error_code);

			return;
		}

		if(m_TradeItemNum[bySlot].sSid < 0 || m_TradeItemNum[bySlot].sNum < sCount)
		{
			result = 0x00;
			error_code = ERR_3;

			SendExchangeFail(result, error_code);
			pSendUser->SendExchangeFail(result, error_code);
			return;
		}
		m_bTradeItem = TRUE;		//�������� �÷�����. 
		ReSetItemSlot(&TempItem);	// ���� �������� ��ġ������ �ʱ�ȭ	

		iNum = m_TradeItemNum[bySlot].sNum;
		m_TradeItemNum[bySlot].sNum = iNum - sCount;
		if(m_TradeItemNum[bySlot].sNum < 0) m_TradeItemNum[bySlot].sNum = 0;

		TempItem = m_UserItem[bySlot];
		TempItem.sCount = sCount;

		if(TempItem.sSid < 0 || TempItem.sSid >= g_arItemTable.GetSize())		// �ٽ��ѹ� Ȯ��
		{
			result = 0x00;
			error_code = ERR_3;
			SendExchangeFail(result, error_code);
			pSendUser->SendExchangeFail(result, error_code);
			return;
		}

//		if(g_arItemTable[TempItem.sSid]->m_byWear >= 6)
		if(g_arItemTable[TempItem.sSid]->m_sDuration <= 0)
		{
			iNum = GetSameItem(TempItem, TRADE_SLOT);
			//ţţADD 
			if(iNum != -1 && iNum == bySlot)
			{
				if(m_arExchangeItem[iNum]->changedList.sCount + sCount >= _MAX_SHORT)
				{
					result = 0x00;
					error_code = ERR_3;
					SendExchangeFail(result, error_code);
					pSendUser->SendExchangeFail(result, error_code);
					return;
				}

				EnterCriticalSection( &m_CS_ExchangeItem );
				m_arExchangeItem[iNum]->changedList.sCount += sCount;
				LeaveCriticalSection( &m_CS_ExchangeItem );

				SendExchangeItem(pSendUser, bySlot, (DWORD)sCount);
				return;
			}
			iNum = pSendUser->GetSameItem(TempItem, INVENTORY_SLOT);
			if(iNum != -1)
			{
				if(pSendUser->m_UserItem[iNum].sCount + TempItem.sCount > MAX_ITEM_NUM)
				{
					result = 0x02;
					error_code = ERR_3;
					SendExchangeFail(result, error_code);
					pSendUser->SendExchangeFail(result, error_code);
					SendSystemMsg( IDS_USER_OVER_COUNT_SAVE_OTHER, SYSTEM_ERROR, TO_ME);
					return;
				}
			}
		}

		EnterCriticalSection( &m_CS_ExchangeItem );
		if(m_arExchangeItem.GetSize() >= INVENTORY_NUM)
		{
			LeaveCriticalSection( &m_CS_ExchangeItem );

			result = 0x00;
			SendExchangeFail(result, 0x06);
			pSendUser->SendExchangeFail(result, 0x07);
			return;
		}
		LeaveCriticalSection( &m_CS_ExchangeItem );

		pNewItem = new ExchangeItem;
		pNewItem->bySlot = bySlot;
		pNewItem->changedList.sLevel = m_UserItem[bySlot].sLevel;
		pNewItem->changedList.sSid = m_UserItem[bySlot].sSid;
		pNewItem->changedList.sDuration = m_UserItem[bySlot].sDuration;
		pNewItem->changedList.sBullNum = m_UserItem[bySlot].sBullNum;
		pNewItem->changedList.sCount = sCount;
		for(j =0; j < MAGIC_NUM; j++) pNewItem->changedList.tMagic[j] = m_UserItem[bySlot].tMagic[j];
		pNewItem->changedList.tIQ = m_UserItem[bySlot].tIQ;
		pNewItem->changedList.iItemSerial = m_UserItem[bySlot].iItemSerial;

		EnterCriticalSection( &m_CS_ExchangeItem );
		m_arExchangeItem.Add(pNewItem);
		LeaveCriticalSection( &m_CS_ExchangeItem );

		SendExchangeItem(pSendUser, bySlot, (DWORD)sCount);
		break;

	case 4:			// Ÿ���� ������ �ø�(�������� �� �� �ִ�)
		result = 0x00;
		error_code = UNKNOWN_ERR;
		SendExchangeFail(result, error_code);
		pSendUser->SendExchangeFail(result, UNKNOWN_ERR);
		return;

	case 5:			// �ڽ��� �� �ø�
		dwCount	= GetDWORD(pBuf, index);
		if(dwCount <= 0 || dwCount > m_dwDN)
		{
			result = 0x00;
			error_code = ERR_3;
			SendExchangeFail(result, error_code);
			pSendUser->SendExchangeFail(result, error_code);
			return;
		}

//		m_TradeMoney = m_TradeMoney + dwCount;
		CheckMaxValue((DWORD &)m_TradeMoney, (DWORD)dwCount);
		if(m_TradeMoney > m_dwDN)
		{
			result = 0x00;
			error_code = ERR_3;
			SendExchangeFail(result, error_code);
			pSendUser->SendExchangeFail(result, error_code);
			return;
		}

		bySlot = 255;
		SendExchangeItem(pSendUser, bySlot, m_TradeMoney);
//		SendExchangeItem(pSendUser, bySlot, dwCount);
		break;

	case 6:			// Ÿ���� �� �ø�(�������� �� �� �ִ�)
	default:
		result = 0x00;
		error_code = UNKNOWN_ERR;
		SendExchangeFail(result, error_code);
		pSendUser->SendExchangeFail(result, UNKNOWN_ERR);
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////
//	������ ��ȯ ���� ��Ŷ�� ������.
//
void USER::SendExchangeFail(BYTE result, BYTE error_code)
{
	if ( m_UserStatus == 0 && m_UserFlag == FALSE ) return;

	USER* pTempUser = NULL;
	CBufferEx TempBuf, TempMyBuf;
	int i, iNum = 0;

	BYTE bySlot = 0;
	
	TempBuf.Add(EXCHANGE_RESULT);
	TempBuf.Add(result);
	TempBuf.Add(error_code);

	Send(TempBuf, TempBuf.GetLength());

	if((m_bTradeItem == TRUE || m_TradeMoney > 0) && result == 0)			// ���� ��ȯ���̰�, �������� �÷�������� 
	{
		EnterCriticalSection( &m_CS_ExchangeItem );
		iNum = m_arExchangeItem.GetSize();				// �÷����� ������ ����ŭ ����ڿ��� �ٽ� �˷���
		if(iNum > 0)
		{
			TempMyBuf.Add(ITEM_MOVE_RESULT);
			TempMyBuf.Add(SUCCESS);
			TempMyBuf.Add((BYTE)0x00);
			TempMyBuf.Add((BYTE)iNum);

			for(i = 0; i < iNum; i++)
			{
				if ( m_arExchangeItem[i] == NULL ) continue;

				bySlot = m_arExchangeItem[i]->bySlot;

				TempMyBuf.Add(bySlot);	
				TempMyBuf.Add(m_UserItem[bySlot].sLevel);
				TempMyBuf.Add(m_UserItem[bySlot].sSid);
				TempMyBuf.Add(m_UserItem[bySlot].sDuration);
				TempMyBuf.Add(m_UserItem[bySlot].sBullNum);
				TempMyBuf.Add(m_UserItem[bySlot].sCount);

				for(int j =0; j < MAGIC_NUM; j++) TempMyBuf.Add(m_UserItem[bySlot].tMagic[j]);

				TempMyBuf.Add(m_UserItem[bySlot].tIQ); 
			}
			Send(TempMyBuf, TempMyBuf.GetLength());
		}
		LeaveCriticalSection( &m_CS_ExchangeItem );

		if(m_TradeMoney > 0) SendMoneyChanged();
	}

	if(result == 0 || result == 2)
	{
		_tcscpy(m_strTradeUser, _T(""));// ������ Ȯ���ϱ� ���� �����̸��� �����.
		m_bNowTrading = m_bTradeWaiting = FALSE;
		m_bExchangeOk = m_bTradeItem	= FALSE;// ��ȯ�� ���¸� �ʱ�ȭ
		m_iTradeUid = -1;
		m_TradeMoney = 0;				// �ŷ� �ݾ��� �ʱ�ȭ

		for(i = 0; i < TOTAL_ITEM_NUM; i++) // �ŷ��� �κ��� �ٲ�� ������ ���� üũ����
		{ 
			m_TradeItemNum[i].sSid = -1; 
			m_TradeItemNum[i].sNum = 0; 
		}

		EnterCriticalSection( &m_CS_ExchangeItem );
		for(i = 0; i < m_arExchangeItem.GetSize(); i++)
		{
			if ( m_arExchangeItem[i] )
			{
				delete m_arExchangeItem[i];
				m_arExchangeItem[i] = NULL;
			}
		}
		m_arExchangeItem.RemoveAll();
		LeaveCriticalSection( &m_CS_ExchangeItem );
	}
}

///////////////////////////////////////////////////////////////////////////////
//	��ȯâ�� ������ �ø�
//
void USER::SendExchangeItem(USER *pUser, BYTE bySlot, DWORD dwCount)
{
	int j;
	short sCount;
	short sSid = -1;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;

	CBufferEx TempBuf, TempMyBuf;

	//----------------------------[�ݾ� ]------------------------------//
	if(bySlot == 255)
	{
		DWORD tempDN = 0;
		if(m_dwDN < dwCount) { tempDN = 0; m_dwDN = 0; }
		else  tempDN = m_dwDN - dwCount;

		TempBuf.Add(EXCHANGE_REQ);				
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add((BYTE)0x06);				// ������� ���� ������.
		TempBuf.Add(dwCount);

		TempMyBuf.Add(EXCHANGE_REQ);			// �ڱ⿡�� ������
		TempMyBuf.Add(pUser->m_uid + USER_BAND);
		TempMyBuf.Add((BYTE)0x05);
		TempMyBuf.Add(dwCount);
		TempMyBuf.Add(tempDN);
		goto go_result;
	}

	//----------------------------[����]------------------------------//
	TempBuf.Add(EXCHANGE_REQ);					// ������� ������.
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((BYTE)0x04);

	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add((short)dwCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ); 

	//----------------------------[�ڱ� ]------------------------------//
	TempMyBuf.Add(EXCHANGE_REQ);				// Ʈ���̵忡 �ö� ������
	TempMyBuf.Add(pUser->m_uid + USER_BAND);	
	TempMyBuf.Add((BYTE)0x03);				
	
	TempMyBuf.Add(m_UserItem[bySlot].sLevel);
	TempMyBuf.Add(m_UserItem[bySlot].sSid);
	TempMyBuf.Add(m_UserItem[bySlot].sDuration);
	TempMyBuf.Add(m_UserItem[bySlot].sBullNum);
	TempMyBuf.Add((short)dwCount);
	for(j =0; j < MAGIC_NUM; j++) TempMyBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempMyBuf.Add(m_UserItem[bySlot].tIQ); 
	
	sCount = m_TradeItemNum[bySlot].sNum;// - (short)dwCount;//m_UserItem[bySlot].sCount - (short)dwCount;
	TempMyBuf.Add(bySlot);						// �κ����� ��ȭ�� ������
	
	TempMyBuf.Add(m_UserItem[bySlot].sLevel);

	if(sCount <= 0) TempMyBuf.Add(sSid);
	else			TempMyBuf.Add(m_UserItem[bySlot].sSid);

	TempMyBuf.Add(m_UserItem[bySlot].sDuration);
	TempMyBuf.Add(m_UserItem[bySlot].sBullNum);
	TempMyBuf.Add(sCount);
	for(j =0; j < MAGIC_NUM; j++) TempMyBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempMyBuf.Add(m_UserItem[bySlot].tIQ); 

go_result:
	pUser->Send(TempBuf, TempBuf.GetLength());
	Send(TempMyBuf, TempMyBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////////
//	���� ��ȯó���� �Ѵ�.
//
void USER::RunExchange(USER* pUser)
{
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) 
	{
		SendExchangeFail((BYTE)0x00, ERR_1);
		return;
	}

	if ( pUser->m_UserStatus == 0 && pUser->m_UserFlag == FALSE )
	{
		SendExchangeFail((BYTE)0x00, ERR_1);
		return;
	}

	BYTE	result		= 0x01;				// ��ȯ����
	BYTE	tNumberMy	= m_arExchangeItem.GetSize();
	BYTE	tNumberYour	= pUser->m_arExchangeItem.GetSize();

	ItemList		MyItem[TOTAL_ITEM_NUM], YourItem[TOTAL_ITEM_NUM];
	ItemList		TempItem;

	ItemList*		pNewItemList = NULL;
	ItemListArray	arItemListMy, arItemListYour;
	CByteArray		arSlotMy, arSlotYour;

	int i = 0, j = 0;
	int iMyWeight = 0, iYourWeight = 0;

	for(i = 0; i < TOTAL_ITEM_NUM; i++)			// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
		YourItem[i] = pUser->m_UserItem[i];
	}

	iMyWeight = m_iCurWeight;							// ���� ������ ���Ը� ���
	iYourWeight = pUser->m_iCurWeight;

	//----------------------------[init values]------------------------------//
	CBufferEx	TempBuf;
	CBufferEx	TempBuf2;
	CBufferEx	TempBuf3;
	CBufferEx	TempBuf4;

	int			iWear = -1;
	int			nSizeYour = 0;
	int			nSizeMy = 0;
	BOOL		bIsOk = TRUE;
	int			iQuickChange = 0, yQuickChange = 0;
	int			iSameSlot = -1, iEmptySlot = -1;
	short		sSid = -1;
	BYTE		tSlot = 0;
	short		sCount = 0;
	DWORD		dwMyTempDN = 0, dwYourTempDN = 0;
	BOOL		bChangeMyEquip = TRUE, bChangeYourEquip = TRUE;
	//----------------------------[ User ]-----------------------------------//

	if((tNumberYour == 0 && tNumberMy == 0) && (m_TradeMoney <= 0 && pUser->m_TradeMoney <= 0))
	{
		SendExchangeFail(0x00, ERR_3);			// �Ѵ� ������, �� �� �ȿø����
		pUser->SendExchangeFail(0x00, ERR_3);
		goto go_result;
	}
	
	EnterCriticalSection( &m_CS_ExchangeItem );
	//tNumberMy = m_arExchangeItem.GetSize();
	for(i = 0; i < m_arExchangeItem.GetSize(); i++)				// �ڽ��� �ִ� ������ ó��
	{
		if ( m_arExchangeItem[i] == NULL ) continue;

		tSlot = (BYTE)m_arExchangeItem[i]->bySlot;

		if(tSlot < EQUIP_ITEM_NUM || tSlot >= TOTAL_INVEN_MAX) { bIsOk = FALSE; break;}
		sCount = m_arExchangeItem[i]->changedList.sCount;
		if(sCount < 0)  { bIsOk = FALSE; break;}	// Add jjs07

		if(m_UserItem[tSlot].sSid < 0 || m_UserItem[tSlot].sSid >= g_arItemTable.GetSize())	{ bIsOk = FALSE; break;}
		
		m_iCurWeight -= (g_arItemTable[m_UserItem[tSlot].sSid]->m_byWeight * sCount);
		if(m_iCurWeight < 0) m_iCurWeight = 0;

		// �ִ� �������� �α��Ѵ�
		// �������� ����Ǳ� ���� �α��� �����
		MakeItemLog( &m_UserItem[tSlot], ITEMLOG_EXCHANGE_GIVE, pUser->m_strUserID );

		m_UserItem[tSlot].sCount -= sCount;
		if(m_UserItem[tSlot].sCount <= 0) 
		{
			m_UserItem[tSlot].sCount = 0;
			m_UserItem[tSlot].sSid = -1;
		}

		pNewItemList = new ItemList;
		pNewItemList->sSid = m_UserItem[tSlot].sSid;
		pNewItemList->sLevel = m_UserItem[tSlot].sLevel;
		pNewItemList->sBullNum = m_UserItem[tSlot].sBullNum;
		pNewItemList->sDuration = m_UserItem[tSlot].sDuration;
		pNewItemList->sCount = m_UserItem[tSlot].sCount;

		for(j =0; j < MAGIC_NUM; j++) pNewItemList->tMagic[j] = m_UserItem[tSlot].tMagic[j];
		pNewItemList->tIQ = m_UserItem[tSlot].tIQ; 
		pNewItemList->iItemSerial = m_UserItem[tSlot].iItemSerial;

		arItemListMy.Add(pNewItemList);
		arSlotMy.Add(tSlot);
	}
	LeaveCriticalSection( &m_CS_ExchangeItem );

	EnterCriticalSection( &pUser->m_CS_ExchangeItem );
	//tNumberYour	= pUser->m_arExchangeItem.GetSize();
	for(i = 0; i < pUser->m_arExchangeItem.GetSize(); i++)		// ���� �ִ� ������ ó��
	{
		if ( pUser->m_arExchangeItem[i] == NULL ) continue;

		tSlot = (BYTE)pUser->m_arExchangeItem[i]->bySlot;
		if(tSlot < EQUIP_ITEM_NUM || tSlot >= TOTAL_INVEN_MAX) { bIsOk = FALSE; break;}

		sCount = pUser->m_arExchangeItem[i]->changedList.sCount;
		if(sCount < 0) { bIsOk = FALSE; break;}		// Add jjs07

		ReSetItemSlot(&TempItem);
		TempItem = pUser->m_UserItem[tSlot];
		TempItem.sCount = sCount;

		if(TempItem.sSid < 0 || TempItem.sSid >= g_arItemTable.GetSize()) { bIsOk = FALSE; break;}
		iSameSlot = GetSameItem(TempItem, INVENTORY_SLOT);

		if(iSameSlot == -1)					// ���� ������ �������� ���� ���
		{
			iEmptySlot = GetEmptySlot(INVENTORY_SLOT);
			if(iEmptySlot == -1)			// ���� ������ �����۵� ���� �󽽷Ե� ���� ���
			{
				bIsOk = FALSE;
				break;
			}

			// �󽽷��� ���� ���
			m_UserItem[iEmptySlot] = TempItem;

			MakeItemLog( &m_UserItem[iEmptySlot], ITEMLOG_EXCHANGE_RECEIVE, pUser->m_strUserID );

			pNewItemList = new ItemList;
			pNewItemList->sSid = m_UserItem[iEmptySlot].sSid;
			pNewItemList->sLevel = m_UserItem[iEmptySlot].sLevel;
			pNewItemList->sBullNum = m_UserItem[iEmptySlot].sBullNum;
			pNewItemList->sDuration = m_UserItem[iEmptySlot].sDuration;
			pNewItemList->sCount = m_UserItem[iEmptySlot].sCount;
	
			for(j =0; j < MAGIC_NUM; j++) pNewItemList->tMagic[j] = m_UserItem[iEmptySlot].tMagic[j];
			pNewItemList->tIQ = m_UserItem[iEmptySlot].tIQ; 
			pNewItemList->iItemSerial = m_UserItem[iEmptySlot].iItemSerial;
			
			arItemListMy.Add(pNewItemList);
			arSlotMy.Add((BYTE)iEmptySlot);
		}
		else								// ���� ������ �������� ���� ���
		{
			m_UserItem[iSameSlot].sCount += TempItem.sCount;

			pNewItemList = new ItemList;
			pNewItemList->sSid = m_UserItem[iSameSlot].sSid;
			pNewItemList->sLevel = m_UserItem[iSameSlot].sLevel;
			pNewItemList->sBullNum = m_UserItem[iSameSlot].sBullNum;
			pNewItemList->sDuration = m_UserItem[iSameSlot].sDuration;
			pNewItemList->sCount = m_UserItem[iSameSlot].sCount;
		
			for(j =0; j < MAGIC_NUM; j++) pNewItemList->tMagic[j] = m_UserItem[iSameSlot].tMagic[j];
			pNewItemList->tIQ = m_UserItem[iSameSlot].tIQ; 
			pNewItemList->iItemSerial = m_UserItem[iSameSlot].iItemSerial;

			arItemListMy.Add(pNewItemList);
			arSlotMy.Add((BYTE)iSameSlot);
		}

		if(TempItem.sSid >= 0) m_iCurWeight += g_arItemTable[TempItem.sSid]->m_byWeight * TempItem.sCount;
		if(m_iCurWeight < 0) m_iCurWeight = 0;
	}
	LeaveCriticalSection( &pUser->m_CS_ExchangeItem );

	if(bIsOk == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)		// Item Information Backup
		{
			m_UserItem[i] = MyItem[i];
			pUser->m_UserItem[i] = YourItem[i];
		}
		SendExchangeFail(0x00, ERR_6);
		pUser->SendExchangeFail(0x00, ERR_7);
		goto go_result;
	}

	//-----------------------------[Send User]----------------------------------//		
	EnterCriticalSection( &pUser->m_CS_ExchangeItem );
	//tNumberYour	= pUser->m_arExchangeItem.GetSize();
	for(i = 0; i < pUser->m_arExchangeItem.GetSize(); i++)			// �ڽ��� �ִ� ������ ó��
	{
		if ( pUser->m_arExchangeItem[i] == NULL ) continue;

		tSlot = (BYTE)pUser->m_arExchangeItem[i]->bySlot;
		if(tSlot < EQUIP_ITEM_NUM || tSlot >= TOTAL_INVEN_MAX) { bIsOk = FALSE; break;}

		sCount = pUser->m_arExchangeItem[i]->changedList.sCount;
		if(sCount < 0) { bIsOk = FALSE; break;}		// Add jjs07

		if(pUser->m_UserItem[tSlot].sSid < 0 || pUser->m_UserItem[tSlot].sSid >= g_arItemTable.GetSize()) { bIsOk = FALSE; break;}

		pUser->m_iCurWeight -= g_arItemTable[pUser->m_UserItem[tSlot].sSid]->m_byWeight * sCount;
		if(pUser->m_iCurWeight < 0) pUser->m_iCurWeight = 0;

		pUser->m_UserItem[tSlot].sCount -= sCount;
		if(pUser->m_UserItem[tSlot].sCount <= 0)
		{
			pUser->m_UserItem[tSlot].sCount = 0;
			pUser->m_UserItem[tSlot].sSid = -1;
		}

		pNewItemList = new ItemList;
		pNewItemList->sSid = pUser->m_UserItem[tSlot].sSid;
		pNewItemList->sLevel = pUser->m_UserItem[tSlot].sLevel;
		pNewItemList->sBullNum = pUser->m_UserItem[tSlot].sBullNum;
		pNewItemList->sDuration = pUser->m_UserItem[tSlot].sDuration;
		pNewItemList->sCount = pUser->m_UserItem[tSlot].sCount;

		for(j =0; j < MAGIC_NUM; j++) pNewItemList->tMagic[j] = pUser->m_UserItem[tSlot].tMagic[j];
		pNewItemList->tIQ = pUser->m_UserItem[tSlot].tIQ;
		pNewItemList->iItemSerial = pUser->m_UserItem[tSlot].iItemSerial;

		arItemListYour.Add(pNewItemList);
		arSlotYour.Add(tSlot);
	}
	LeaveCriticalSection( &pUser->m_CS_ExchangeItem );

	EnterCriticalSection( &m_CS_ExchangeItem );
	//tNumberMy = m_arExchangeItem.GetSize();
	for(i = 0; i < m_arExchangeItem.GetSize(); i++)		// ���� �ִ� ������ ó��--> arItemListMy���� �޾ƾ��Ѵ�.
	{
		if ( m_arExchangeItem[i] == NULL ) continue;

		tSlot = (BYTE)m_arExchangeItem[i]->bySlot;

		// ������ �ִ� �������� ������ �����Ƿ� �ݿ��Ѵ�.
		TempItem.sLevel		= m_arExchangeItem[i]->changedList.sLevel;
		TempItem.sSid		= m_arExchangeItem[i]->changedList.sSid;
		TempItem.sCount		= m_arExchangeItem[i]->changedList.sCount;
		TempItem.sBullNum	= m_arExchangeItem[i]->changedList.sBullNum;
		TempItem.sDuration	= m_arExchangeItem[i]->changedList.sDuration;

		for(j =0; j < MAGIC_NUM; j++) TempItem.tMagic[j] = m_arExchangeItem[i]->changedList.tMagic[j];
		TempItem.tIQ = m_arExchangeItem[i]->changedList.tIQ;
		TempItem.iItemSerial = m_arExchangeItem[i]->changedList.iItemSerial;

		if(TempItem.sSid < 0 || TempItem.sSid >= g_arItemTable.GetSize()) { bIsOk = FALSE; break;}
		iSameSlot = pUser->GetSameItem(TempItem, INVENTORY_SLOT);

		if(iSameSlot == -1)					// ���� ������ �������� ���� ���
		{
			iEmptySlot = pUser->GetEmptySlot(INVENTORY_SLOT);
			if(iEmptySlot == -1)			// ���� ������ �����۵� ���� �󽽷Ե� ���� ���
			{
				bIsOk = FALSE;
				break;
			}

			// �󽽷��� ���� ���
			pUser->m_UserItem[iEmptySlot] = TempItem;

			pNewItemList = new ItemList;
			pNewItemList->sLevel = pUser->m_UserItem[iEmptySlot].sLevel;
			pNewItemList->sSid = pUser->m_UserItem[iEmptySlot].sSid;
			pNewItemList->sBullNum = pUser->m_UserItem[iEmptySlot].sBullNum;
			pNewItemList->sDuration = pUser->m_UserItem[iEmptySlot].sDuration;
			pNewItemList->sCount = pUser->m_UserItem[iEmptySlot].sCount;

			for(j =0; j < MAGIC_NUM; j++) pNewItemList->tMagic[j] = pUser->m_UserItem[iEmptySlot].tMagic[j];
			pNewItemList->tIQ = pUser->m_UserItem[iEmptySlot].tIQ; 
			pNewItemList->iItemSerial = pUser->m_UserItem[iEmptySlot].iItemSerial;
			
			arItemListYour.Add(pNewItemList);
			arSlotYour.Add((BYTE)iEmptySlot);
		}
		else								// ���� ������ �������� ���� ���
		{
			pUser->m_UserItem[iSameSlot].sCount += TempItem.sCount;
			
			pNewItemList = new ItemList;
			pNewItemList->sLevel = pUser->m_UserItem[iSameSlot].sLevel;
			pNewItemList->sSid = pUser->m_UserItem[iSameSlot].sSid;
			pNewItemList->sBullNum = pUser->m_UserItem[iSameSlot].sBullNum;
			pNewItemList->sDuration = pUser->m_UserItem[iSameSlot].sDuration;
			pNewItemList->sCount = pUser->m_UserItem[iSameSlot].sCount;

			for(j =0; j < MAGIC_NUM; j++) pNewItemList->tMagic[j] = pUser->m_UserItem[iSameSlot].tMagic[j];
			pNewItemList->tIQ = pUser->m_UserItem[iSameSlot].tIQ; 
			pNewItemList->iItemSerial = pUser->m_UserItem[iSameSlot].iItemSerial;
			
			arItemListYour.Add(pNewItemList);
			arSlotYour.Add((BYTE)iSameSlot);
		}

		if(TempItem.sSid >= 0) pUser->m_iCurWeight += g_arItemTable[TempItem.sSid]->m_byWeight * TempItem.sCount;
	}
	LeaveCriticalSection( &m_CS_ExchangeItem );

	if(bIsOk == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)		// Item Information Backup
		{
			m_UserItem[i] = MyItem[i];
			pUser->m_UserItem[i] = YourItem[i];
		}
		SendExchangeFail(0x00, ERR_7);
		pUser->SendExchangeFail(0x00, ERR_6);
		goto go_result;
	}

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED)// ���������� ������� ���� ����Ʈ���� �����Ǿ�����...
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)		// Item Information Backup
		{
			m_UserItem[i] = MyItem[i];
		}
		SendExchangeFail(0x00, ERR_5);
		goto go_result;
	}


	//----------------[ �ݾ��� ������Ų��.]----------------------------------//
	dwMyTempDN = m_dwDN;
	dwYourTempDN = pUser->m_dwDN;
	CheckMaxValue((DWORD &)m_dwDN, (DWORD)pUser->m_TradeMoney);

	MakeMoneyLog( pUser->m_TradeMoney, ITEMLOG_EXCHANGE_RECEIVE_MONEY, pUser->m_strUserID );

	if(m_dwDN <= m_TradeMoney) m_dwDN = 0;
	else m_dwDN = m_dwDN - m_TradeMoney;

	CheckMaxValue((DWORD &)pUser->m_dwDN, (DWORD)m_TradeMoney);

	if(pUser->m_dwDN <= pUser->m_TradeMoney) pUser->m_dwDN = 0;
	else  pUser->m_dwDN = pUser->m_dwDN - pUser->m_TradeMoney;

	MakeMoneyLog( m_TradeMoney, ITEMLOG_EXCHANGE_GIVE_MONEY, pUser->m_strUserID, pUser->m_dwDN );

	// ----------------[ Update Databace ] ----------------------------------//
	if(UpdateUserExchangeData(pUser) == FALSE)
	{
		m_iCurWeight = iMyWeight;
		pUser->m_iCurWeight = iYourWeight;

		m_dwDN = dwMyTempDN;
		pUser->m_dwDN = dwYourTempDN;
		for(i = 0; i < TOTAL_ITEM_NUM; i++)		// ����� ������Ʈ�� �����ϸ� �������´�.
		{
			m_UserItem[i] = MyItem[i];
			pUser->m_UserItem[i] = YourItem[i];
		}
		SendExchangeFail(0x00, UNKNOWN_ERR);
		pUser->SendExchangeFail(0x00, UNKNOWN_ERR);
		goto go_result;
	}

	//-----------------[ Send Packet ]---------------------------------------//
	nSizeMy = arSlotMy.GetSize();

	TempBuf.Add(EXCHANGE_RESULT);
	TempBuf.Add((BYTE)0x01);
	TempBuf.Add(m_dwDN);
	TempBuf.Add((BYTE)nSizeMy);

	for(i = 0; i < nSizeMy; i++)
	{
		TempBuf.Add((BYTE)arSlotMy[i]);
		TempBuf.Add(arItemListMy[i]->sLevel);
		TempBuf.Add(arItemListMy[i]->sSid);
		TempBuf.Add(arItemListMy[i]->sDuration);
		TempBuf.Add(arItemListMy[i]->sBullNum);
		TempBuf.Add(arItemListMy[i]->sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)arItemListMy[i]->tMagic[j]);
		TempBuf.Add((BYTE)arItemListMy[i]->tIQ); 
	}

	Send(TempBuf, TempBuf.GetLength());

	nSizeYour = arSlotYour.GetSize();

	TempBuf2.Add(EXCHANGE_RESULT);
	TempBuf2.Add((BYTE)0x01);
	TempBuf2.Add(pUser->m_dwDN);
	TempBuf2.Add((BYTE)nSizeYour);

	for(i = 0; i < nSizeYour; i++)
	{
		TempBuf2.Add((BYTE)arSlotYour[i]);
		TempBuf2.Add(arItemListYour[i]->sLevel);
		TempBuf2.Add(arItemListYour[i]->sSid);
		TempBuf2.Add(arItemListYour[i]->sDuration);
		TempBuf2.Add(arItemListYour[i]->sBullNum);
		TempBuf2.Add(arItemListYour[i]->sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf2.Add((BYTE)arItemListYour[i]->tMagic[j]);
		TempBuf2.Add((BYTE)arItemListYour[i]->tIQ); 
	}

	pUser->Send(TempBuf2, TempBuf2.GetLength());

	// ���������� ���̻� �������� �ٷ��� �ʴ´�. -------------------------------//
/*
	if(iQuickChange >= 1)
	{
		sCount = -1;
		TempBuf3.Add(ITEM_MOVE_RESULT);
		TempBuf3.Add((BYTE)0x01);
		TempBuf3.Add((BYTE)0x02);

		TempBuf3.Add((BYTE)iQuickChange);
		for(i = 0; i < QUICK_ITEM; i++)
		{
			if(MyTempQuickItem[i] != -1)
			{
				TempBuf3.Add((BYTE)i);
				TempBuf3.Add(sCount);
			}
		}
		Send(TempBuf3, TempBuf3.GetLength());
	}

	if(yQuickChange >= 1)
	{
		sCount = -1;
		TempBuf4.Add(ITEM_MOVE_RESULT);
		TempBuf4.Add((BYTE)0x01);
		TempBuf4.Add((BYTE)0x02);

		TempBuf4.Add((BYTE)yQuickChange);
		for(i = 0; i < QUICK_ITEM; i++)
		{
			if(YourTempQuickItem[i] != -1)
			{
				TempBuf4.Add((BYTE)i);
				TempBuf4.Add(sCount);
			}
		}
		pUser->Send(TempBuf4, TempBuf4.GetLength());
	}
*/
/*	//	�� ��ȭ�� ������...^^
	if(pUser->m_TradeMoney > 0 || m_TradeMoney > 0)
	{
		SendMoneyChanged();
		pUser->SendMoneyChanged();
	}
*/
	GetRecoverySpeed();					//	���� ��ȭ�� ���� ȸ���ӵ��� �����Ѵ�.
	pUser->GetRecoverySpeed();

	SendSystemMsg( IDS_USER_EXCHANGE_COMPLETED, SYSTEM_NORMAL, TO_ME);
	if(bChangeMyEquip) SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	if(bChangeYourEquip) pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);

	FlushItemLog( TRUE );

//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
//	pUser->SendItemWeightChange();				// ���� ������ ���Ը� ������.
	//---------------[ Delete Memory ]------------------------------------//
go_result:
	for(i = 0; i < arItemListMy.GetSize(); i++)
	{
		if(arItemListMy[i] != NULL) delete arItemListMy[i];
	}
	arItemListMy.RemoveAll();

	for(i = 0; i < arItemListYour.GetSize(); i++)
	{
		if(arItemListYour[i] != NULL) delete arItemListYour[i];
	}
	arItemListYour.RemoveAll();

	EnterCriticalSection( &m_CS_ExchangeItem );
	for(i = 0; i < m_arExchangeItem.GetSize(); i++)
	{
		if ( m_arExchangeItem[i] != NULL )
		{
			delete m_arExchangeItem[i];
			m_arExchangeItem[i] = NULL;
		}
	}
	m_arExchangeItem.RemoveAll();
	LeaveCriticalSection( &m_CS_ExchangeItem );

	EnterCriticalSection( &pUser->m_CS_ExchangeItem );
	for(i = 0; i < pUser->m_arExchangeItem.GetSize(); i++)
	{
		if(pUser->m_arExchangeItem[i] != NULL)
		{
			delete pUser->m_arExchangeItem[i];
			pUser->m_arExchangeItem[i] = NULL;
		}
	}
	pUser->m_arExchangeItem.RemoveAll();
	LeaveCriticalSection( &pUser->m_CS_ExchangeItem );

	//---------------[ �ŷ����� �ʱ�ȭ ]-------------------------------//
	m_TradeMoney = 0;
	pUser->m_TradeMoney = 0;
	m_bNowTrading = m_bExchangeOk = m_bTradeItem	= FALSE;
	pUser->m_bNowTrading = pUser->m_bExchangeOk = pUser->m_bTradeItem = FALSE;

	_tcscpy(m_strTradeUser, _T(""));		// ������ Ȯ���ϱ� ���� �����̸��� �����.
	_tcscpy(pUser->m_strTradeUser, _T(""));	// ������ Ȯ���ϱ� ���� �����̸��� �����.
	m_iTradeUid = -1;						// ���� uid����
	pUser->m_iTradeUid = -1;				// ���� uid����

	for(i = 0; i < TOTAL_ITEM_NUM; i++) // �ŷ��� �κ��� �ٲ�� ������ ���� üũ����
	{ 
		m_TradeItemNum[i].sSid = -1; 
		m_TradeItemNum[i].sNum = 0; 
	}

	FlushItemLog( FALSE );
}

//////////////////////////////////////////////////////////////////////////////
//	�� ü���� �Ҷ� ���� ��ȭ�� �˸���.
//
void USER::SendWeatherInMoveZone()
{
	int tType = 2;
	int iWeather = 1;

	// �̵��Ϸ��� ���� �񰡿ü� �ִ� �����̸�	
	for(int i = 0; i < g_WeatherZone.GetSize(); i++)	
	{
		if(g_WeatherZone[i]->iZone == m_curz)
		{
			if(g_WeatherZone[i]->bRaining == TRUE) SetWeather(tType, iWeather); 
			return ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//	�ش� ���� �ִ� �������� ���� ��ȭ�� �˸���.
//
//void USER::SetWeather(TCHAR *pBuf)
void USER::SetWeather(int tType, int tOnOff)
{
	CBufferEx TempBuf;
	TempBuf.Add(SET_CLIENT_WORLD);
	TempBuf.Add((BYTE)tType);

	if(tType == 2)	TempBuf.Add((BYTE)tOnOff);
	else			TempBuf.Add((BYTE)g_GameTime);

	Send(TempBuf, TempBuf.GetLength());
}

/*
//////////////////////////////////////////////////////////////////////////////
//	
//
void USER::SendWeatherToZone(BYTE tType, BYTE tOnOff)
{
	CBufferEx TempBuf;
	
	TempBuf.Add(SET_CLIENT_WORLD);

	TempBuf.Add(tType);
	TempBuf.Add(tOnOff);

	Send(TempBuf, TempBuf.GetLength());
}
*/
//##############################################################################
//	Character Event
//##############################################################################

////////////////////////////////////////////////////////////////////////////
//	Client NPC ���� �̺�Ʈ ������ ��û�Ѵ�.
//
void USER::NpcEvent(TCHAR *pBuf)
{
	int event_zone_index = -1;
	int i, j;
	CNpc*	pNpc = NULL;
	EVENT*	pEvent = NULL;

	int index = 0;
	int nUid = GetInt(pBuf, index);

	if(nUid < 0 || nUid >= INVALID_BAND) return;	// �߸��� uid�� ����
	
	if(nUid >= USER_BAND && nUid < NPC_BAND)		// ������ ���� �̺�Ʈ
	{
		return;
	}
	
	if(nUid >= NPC_BAND && nUid < INVALID_BAND)		// NPC �� ���� �̺�Ʈ
	{
		pNpc = GetNpc(nUid - NPC_BAND);
		if(!pNpc ) return;

		if(pNpc->m_sEvent <= 0 ) return;

		for(i = 0; i < g_event.GetSize(); i++)
		{
			if(g_event[i]->m_Zone == m_curz)
			{				
				event_zone_index = i;
				break;
			}
		}

		if(event_zone_index == -1) return;			// ���� Zone ���� define �� �̺�Ʈ�� ����.

		pEvent = g_event[event_zone_index];

		if(pEvent == NULL) return;
		if(pNpc->m_sEvent >= pEvent->m_arEvent.GetSize()) return;
		if(pEvent->m_arEvent[pNpc->m_sEvent] == NULL) return;

		if(!CheckEventLogic(pEvent->m_arEvent[pNpc->m_sEvent])) return;	// ���ǰ˻�

		for(j = 0; j < pEvent->m_arEvent[pNpc->m_sEvent]->m_arExec.GetSize(); j++)
		{
			if(RunNpcEvent(pNpc, pEvent->m_arEvent[pNpc->m_sEvent]->m_arExec[j]) == NULL) break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//	Client HyperText���� �̺�Ʈ ������ ��û�Ѵ�.
//
void USER::HyperTextEvent(TCHAR *pBuf)
{
	int event_zone_index = -1;
	int index = 0, i, j;
	EVENT*	pEvent = NULL;

	short sZone = GetShort(pBuf, index);
	short sEventNum = GetShort(pBuf, index);
	TRACE( "ţţDEGBUG NPC �����¼��� %d\n", sEventNum);
	if(sZone < 0 || sEventNum < 0) return;

	for(i = 0; i < g_event.GetSize(); i++)
	{
		if(g_event[i]->m_Zone == sZone)
		{			
			if(sZone != m_curz)
			{
				return;
//				if(sZone == 2 && (sEventNum >= 600 && sEventNum <= 700)) { event_zone_index = i; break; }
//				else return;
			}
			event_zone_index = i;
			break;
		}
	}

	if(event_zone_index == -1) return;			// ���� Zone ���� define �� �̺�Ʈ�� ����.
	
	pEvent = g_event[event_zone_index];

	if(pEvent == NULL) return;
	if(sEventNum >= pEvent->m_arEvent.GetSize()) return;
	if(pEvent->m_arEvent[sEventNum] == NULL) return;

	if(!CheckEventLogic(pEvent->m_arEvent[sEventNum])) return;	// ���ǰ˻�

	for(j = 0; j < pEvent->m_arEvent[sEventNum]->m_arExec.GetSize(); j++)
	{
		if(RunNpcEvent(NULL, pEvent->m_arEvent[sEventNum]->m_arExec[j]) == NULL) break;
	}
}

////////////////////////////////////////////////////////////////////////////
//	�̺�Ʈ ����Ʈ�� ���� ����ϰ� �ִ� �̺�Ʈ�� �����Ѵ�.
//
void USER::RunQuestEvent(CNpc *pNpc, int iZone, int iEventNum)
{
	if(!pNpc) return;

	int event_zone_index = -1;
	int index = 0, i, j;
	EVENT*	pEvent = NULL;

	if(iZone < 0) return;		// �̺�Ʈ ��ȣ�� 1���� 10000������ 
	if(iEventNum <= 0 || iEventNum > NPC_QUEST_MOP) return;

	for(i = 0; i < g_event.GetSize(); i++)
	{
		if(g_event[i]->m_Zone == iZone)
		{			
			if(iZone != m_curz)	return;

			event_zone_index = i;
			break;
		}
	}

	if(event_zone_index == -1) return;			// ���� Zone ���� define �� �̺�Ʈ�� ����.
	
	pEvent = g_event[event_zone_index];

	if(pEvent == NULL) return;
	if(iEventNum >= pEvent->m_arEvent.GetSize()) return;
	if(pEvent->m_arEvent[iEventNum] == NULL) return;

	if(!CheckEventLogic(pEvent->m_arEvent[iEventNum])) return;	// ���ǰ˻�

	for(j = 0; j < pEvent->m_arEvent[iEventNum]->m_arExec.GetSize(); j++)
	{
		if(RunNpcEvent(pNpc, pEvent->m_arEvent[iEventNum]->m_arExec[j]) == NULL) break;
	}
}

////////////////////////////////////////////////////////////////////////////
//	Client ���� �̺�Ʈ ������ ��û�Ѵ�.
//
void USER::ClientEvent(TCHAR *pBuf)
{
	int index = 0;

	BYTE tType = GetByte(pBuf, index);

	if(tType == 1) NpcEvent(pBuf + index);

	if(tType == 2) HyperTextEvent(pBuf + index);

}

/////////////////////////////////////////////////////////////////////////
//	�̺�Ʈ�� ������ �ִ��� �˻��Ѵ�.
//
BOOL USER::CheckEventLogic(EVENT_DATA *pEventData)
{
	if(pEventData == NULL) return FALSE;

	BOOL bExact = FALSE;

	for( int i = 0; i < pEventData->m_arLogicElse.GetSize(); i++)
	{
		bExact = FALSE;

		LOGIC_ELSE* pLE = pEventData->m_arLogicElse[i];		if( !pLE ) return FALSE;

		switch( pLE->m_LogicElse )
		{
		case LOGIC_LEVEL:
			if( CheckLevel(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1]) ) bExact = TRUE; 
			break;
	
		case LOGIC_EXIST_EVENT:
			if( FindEvent( pLE->m_LogicElseInt[0] ) ) bExact = TRUE;
			break;

		case LOGIC_ABSENT_EVENT:
			if( !FindEvent( pLE->m_LogicElseInt[0] ) ) bExact = TRUE;
			break;

		case LOGIC_CLASS:
			if( CheckClass( pLE->m_LogicElseInt[0] ) ) bExact = TRUE;
			break;

		case LOGIC_EXIST_ITEM:
			if( FindItem( pLE->m_LogicElseInt[0]) >= pLE->m_LogicElseInt[1] ) bExact = TRUE;
			break;

		case LOGIC_EXIST_EVENT_ITEM:
			if( FindEventItem( pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1] ) ) bExact = TRUE;
			break;
		case LOGIC_EXIST_SKILL:
			if( FindSkill( pLE->m_LogicElseInt[0] ) ) bExact = TRUE;
			break;

		case LOGIC_ABSENT_ITEM:
			if( !FindItem( pLE->m_LogicElseInt[0]) ) bExact = TRUE;
			break;

		case LOGIC_WEIGHT:
			if( CheckItemWeight( pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1], pLE->m_LogicElseInt[2],
													pLE->m_LogicElseInt[3], pLE->m_LogicElseInt[4],
													pLE->m_LogicElseInt[5], pLE->m_LogicElseInt[6],
													pLE->m_LogicElseInt[7], pLE->m_LogicElseInt[8],
													pLE->m_LogicElseInt[9], pLE->m_LogicElseInt[10]	)) bExact = TRUE;
			break;

		case LOGIC_CHECK_ITEMSLOT:
			if( CheckItemSlot( pLE->m_LogicElseInt[0], pLE) ) bExact = TRUE;
			break;

		case LOGIC_EXIST_GM:
			if( CheckGuildMaster() ) bExact = TRUE;
			break;

		case LOGIC_OVER_GUILD_USER_COUNT:
			if( CheckOverGuildUserCount( pLE->m_LogicElseInt[0] ) ) bExact = TRUE;
			break;

		case LOGIC_ZONE_WHO:
			if( CheckZoneWho(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1], pLE->m_LogicElseInt[2]) ) bExact = TRUE;
			break;

		case LOGIC_KILL_GH:
			break;

		case LOGIC_MASS_ZONE_MOVE:
			break;

		case LOGIC_TERM:
			if( CheckTerm( pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1], pLE->m_LogicElseInt[2]) ) bExact = TRUE; 
			break;
/*		case LOGIC_DN:		// ���� DN���� �������
			if(m_dwDN >= (DWORD)pLE->m_LogicElseInt[0] ) bExact = TRUE;
			break;
*/
		case LOGIC_MONEY:
			if( CheckMoneyMinMax( pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1] ) ) bExact = TRUE;
			break;

		case LOGIC_CHECK_CV:
			if( CheckUserCityValue( pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1]) ) bExact = TRUE;
			break;

		case LOGIC_GUILD_HOUSE_NUMBER:
			if(CheckGuildHouseUser(pLE->m_LogicElseInt[0])) bExact = TRUE;
			break;

		case LOGIC_RAND:
			if( CheckRandom( pLE->m_LogicElseInt[0] ) ) bExact = TRUE;
			break;

		case LOGIC_RR_TIME:
			if( CheckRoyalRumbleEnterTime() ) bExact = TRUE;
			break;

		case LOGIC_RR_MAX_USER:
			if( CheckRoyalRumbleEnterMaxUser() ) bExact = TRUE;
			break;

		default:
			return FALSE;
		}

		if( !pLE->m_bAnd )		// OR ������ ��� bExact�� TRUE�̸� ��ü�� TRUE�̴�
		{
			if( bExact )
			{
				return TRUE;
			}
		}
		else					// AND ������ ��� bExact�� FALSE�̸� ��ü�� FALSE�̴�
		{
			if( !bExact )
			{
				return FALSE;
			}
		}
	}

	return TRUE;		// �ƹ��� ������ ������ TRUE ����
}

////////////////////////////////////////////////////////////////////////////
//	�ش� ������ ���� �ù� ������� �Ǵ�
//
BOOL USER::CheckUserCityValue(int min, int max)
{
	if(m_sCityRank >= min && m_sCityRank < max ) return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//	�ش� ������ ���� ������ �Ǵ���
//
BOOL USER::CheckLevel(int min, int max)
{
	if(m_sLevel >= min && m_sLevel < max ) return TRUE;

//	SendSystemMsg( IDS_USER_NOT_THOS_LEVEL, SYSTEM_ERROR, TO_ME);

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//	�ش� ������ �渶���� üũ
//
BOOL USER::CheckGuildMaster()
{
	if( m_bGuildMaster ) return TRUE;

	SendSystemMsg( IDS_USER_ONLY_GUILD_MASTER_USE, SYSTEM_ERROR, TO_ME);

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//	���� ��¥���� ����Ǵ��� üũ
//
BOOL USER::CheckTerm(int start, int end, int say)
{
	SYSTEMTIME guildTime;
	GetLocalTime(&guildTime);

	if( guildTime.wDay >= start	&& guildTime.wDay <= end ) return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//	�ִ� 20���� �̵� ��Ų��.
//
BOOL USER::MassZoneMove(int cityNum)
{
/*	InterlockedExchange(&g_arGuildHouseWar[cityNum]->m_CurrentGuild.lUsed, 0);

	g_arGuildHouseWar[cityNum]->m_CurrentGuild.arUserList.RemoveAll();
	g_arGuildHouseWar[cityNum]->m_CurrentGuild.lGuild = 0;
	g_arGuildHouseWar[cityNum]->m_CurrentGuild.iCurValue = 0;
	g_arGuildHouseWar[cityNum]->m_CurrentGuild.dwTimer = 0;

*/
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//	������ ������ Event�� �ش� Event�� �ִ��� ã�´�.
//
BOOL USER::FindEvent(int event_num)
{
	for(int i = 0; i < m_arEventNum.GetSize(); i++)
	{
		TRACE("�����:%d \n", *(m_arEventNum[i]));
		if( *(m_arEventNum[i]) == event_num )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL USER::FindSkill(int Skill_num)
{
	int i;
	int iIndex=m_byClass *SKILL_NUM;
	
	if(m_sSkillPoint != 0)  //������м��ܵ�û�мӵĻ�..Ҳ���е�....
		return false;
	for(i = iIndex; i < iIndex + SKILL_NUM; i++)
	{
		if(m_UserSkill[i].sSid==Skill_num)
			return true;
	}
	return false;
}
void USER::AddSkill (int Skill_num)
{
	m_SKill=Skill_num;
}
// 120����ת��
void USER::RunSkill120(int Skill)
{
	int i,count=0,sLevel=m_sLevel;
	int iIndex=m_byClass *SKILL_NUM;
	
	if(Skill!=20&&Skill!=21&&Skill!=22&&Skill!=23&&Skill!=24)
		return;
	for(i = iIndex; i < iIndex + SKILL_NUM; i++)
	{
		if(m_UserSkill[i].sSid>=20)
			count++;
	}
	if(count>=2)
		return ;

	for(i = iIndex; i < iIndex + SKILL_NUM; i++)
	{
		if(m_UserSkill[i].sSid==m_SKill)
			break;
	}
	if(i >= iIndex + SKILL_NUM)
		return;

	if(sLevel>130)
		sLevel=129;
	m_UserSkill[i].sSid=Skill;
	m_UserSkill[i].tLevel=m_UserSkill[i].tLevel/2;
	if(sLevel>=120)
		m_UserSkill[i].tLevel=m_UserSkill[i].tLevel+(sLevel-119);
	SendUserStatusSkill();
	return;

}
////////////////////////////////////////////////////////////////////////////
//	������ ������ Item�� �ش� Item�� �ִ��� ã�´�.
//
int USER::FindItem(int sid)
{
	if(sid < 0 || sid >= g_arItemTable.GetSize()) return FALSE;

	int i, j, iNum = -1;
	ItemList TempItem;

	ReSetItemSlot(&TempItem);

	TempItem.sSid		= g_arItemTable[sid]->m_sSid;
	TempItem.sBullNum	= g_arItemTable[sid]->m_sBullNum;
	TempItem.sDuration	= g_arItemTable[sid]->m_sDuration;

	for(i =0; i < MAGIC_NUM; i++) TempItem.tMagic[i] = 0;
	TempItem.tIQ = NORMAL_ITEM;
	if(sid==755){
		TempItem.tIQ=2;
		TempItem.tMagic[0]=137;
	}
	
//	iNum = GetSameItem(TempItem, INVENTORY_SLOT);
	for(i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
	{
		if(m_UserItem[i].sSid == TempItem.sSid)
		{
			if( m_UserItem[i].sSid != 796 )		// �̺�Ʈ ������ ��� ���� ��ȣ�� �ɼ��� üũ���� �ʴ´�.
			{
				for(j = 0; j < MAGIC_NUM; j++)
				{
					if(m_UserItem[i].tMagic[j] != TempItem.tMagic[j]) break;
				}
					
				if(m_UserItem[i].tIQ == TempItem.tIQ) { iNum = i; break; } 
			}
			else
			{
				iNum = i; break;
			}
		}
	}

	if(iNum != -1)	return m_UserItem[iNum].sCount;
	
	return 0;
}

BOOL USER::FindEventItem(int sid, int quality)
{
	if(sid < 0 || sid >= g_arItemTable.GetSize()) return FALSE;

	int i, j, iNum = -1;
	ItemList TempItem;

	ReSetItemSlot(&TempItem);

	TempItem.sSid		= g_arItemTable[sid]->m_sSid;
	TempItem.sBullNum	= g_arItemTable[sid]->m_sBullNum;
	TempItem.sDuration	= g_arItemTable[sid]->m_sDuration;

	for(i =0; i < MAGIC_NUM; i++) TempItem.tMagic[i] = 0;
	TempItem.tIQ = (BYTE)quality;
	
//	iNum = GetSameItem(TempItem, INVENTORY_SLOT);
	for(i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
	{
		if(m_UserItem[i].sSid == TempItem.sSid)
		{
			for(j = 0; j < MAGIC_NUM; j++)
			{
				if(m_UserItem[i].tMagic[j] != TempItem.tMagic[j]) break;
			}
				
			if(m_UserItem[i].tIQ == TempItem.tIQ) { iNum = i; break; } 
		}
	}

	if(iNum != -1)	return TRUE;
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
//
//	�̺�Ʈ���� �������� �ٶ� ���� üũ
//
BOOL USER::CheckItemWeight(int say, int iSid1, int iNum1, int iSid2, int iNum2, int iSid3, int iNum3, int iSid4, int iNum4, int iSid5, int iNum5)
{
	int iWeight = 0;
//	int iMax = g_arItemTable.GetSize();

	if( iSid1 >= 0 && iSid1 < g_arItemTable.GetSize() ) iWeight += g_arItemTable[iSid1]->m_byWeight * iNum1;
	if( iSid2 >= 0 && iSid2 < g_arItemTable.GetSize() ) iWeight += g_arItemTable[iSid2]->m_byWeight * iNum2;
	if( iSid3 >= 0 && iSid3 < g_arItemTable.GetSize() ) iWeight += g_arItemTable[iSid3]->m_byWeight * iNum3;
	if( iSid4 >= 0 && iSid4 < g_arItemTable.GetSize() ) iWeight += g_arItemTable[iSid4]->m_byWeight * iNum4;
	if( iSid5 >= 0 && iSid5 < g_arItemTable.GetSize() ) iWeight += g_arItemTable[iSid5]->m_byWeight * iNum5;

	if(m_iMaxWeight < m_iCurWeight + iWeight) 
	{
//		SendNpcSay(NULL, say);
		SendSystemMsg( IDS_USER_OVER_WEIGHT_RECEIVE_ITEM, SYSTEM_NORMAL, TO_ME);
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	�̺�Ʈ���� �������� �ٶ� ����ִ� ������ �ִ��� üũ
//
BOOL USER::CheckItemSlot(int say, LOGIC_ELSE* pLE)
{
	int sid = 0;
	int iNum = -1;
	ItemList TempItem;

	for(int i = 1; i < 10; i += 2)
	{
		sid = pLE->m_LogicElseInt[i];

		if( sid < 0 || sid >= g_arItemTable.GetSize() ) continue;

		ReSetItemSlot(&TempItem);

		TempItem.sSid		= g_arItemTable[sid]->m_sSid;
		TempItem.sBullNum	= g_arItemTable[sid]->m_sBullNum;
		TempItem.sDuration	= g_arItemTable[sid]->m_sDuration;

		for(int j =0; j < MAGIC_NUM; j++) TempItem.tMagic[j] = 0;
		TempItem.tIQ = NORMAL_ITEM;
	
		iNum = GetSameItem(TempItem, INVENTORY_SLOT);
		if(iNum == -1)							// ���� �������� ����
		{
			iNum = GetEmptySlot(INVENTORY_SLOT);
			if(iNum == -1)						// �� ���Ե� ������..
			{
//				SendNpcSay(NULL, say);
				SendSystemMsg( IDS_USER_INVEN_FULL_RECEIVE_ITEM, SYSTEM_NORMAL, TO_ME);
				return FALSE;
			}
		}
	}

	return TRUE;	
}

//////////////////////////////////////////////////////////////////////
//
//	���� �迭���� üũ
//
BOOL USER::CheckClass(int iClass)
{
	if(m_byClass == iClass) return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
//
//	�̺�Ʈ���� �ش� �������� ���´�.
//
void USER::RobItem(int sid, int num)
{
	if(sid < 0 || sid >= g_arItemTable.GetSize()) return;

	int i, j;
	int iSlot = -1;
	CBufferEx TempBuf;
//	BOOL bQuickChange = FALSE;
	BYTE result = SUCCESS;

	ItemList	TempItem;
	ReSetItemSlot(&TempItem);

	TempItem.sSid		= g_arItemTable[sid]->m_sSid;
	TempItem.sBullNum	= g_arItemTable[sid]->m_sBullNum;
	TempItem.sDuration	= g_arItemTable[sid]->m_sDuration;

	for(i =0; i < MAGIC_NUM; i++) TempItem.tMagic[i] = 0;
	TempItem.tIQ = NORMAL_ITEM;
	if(sid==755){
		TempItem.tIQ=2;
		TempItem.tMagic[0]=137;
	}
	
	for(i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
	{
		if(m_UserItem[i].sSid == TempItem.sSid)
		{
			if( m_UserItem[i].sSid != 796 )
			{
				for(j = 0; j < MAGIC_NUM; j++)
				{
					if(m_UserItem[i].tMagic[j] != TempItem.tMagic[j]) break;
				}
					
				if(m_UserItem[i].tIQ == TempItem.tIQ) { iSlot = i; break; } 
			}
			else
			{
				iSlot = i; break;
			}
		}
	}

	if(iSlot == -1) return;

	if(m_UserItem[iSlot].sSid < 0) return;
	if(m_UserItem[iSlot].sCount < num) return;

	if((m_UserItem[iSlot].sCount - num) <= 0)				// ���� �� �κ��� �� �ش�.		
	{														// �������� ��ȭ�� ������		
//		if(g_arItemTable[iSlot]->m_byWear >= 6) bQuickChange = TRUE;

		ReSetItemSlot(&m_UserItem[iSlot]);
	}
	else m_UserItem[iSlot].sCount -= num;

	int iWeight = num * g_arItemTable[iSlot]->m_byWeight;

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();			// �ٽ� ȸ���ӵ��� ���

	TempBuf.Add(ITEM_GIVE_RESULT);
	TempBuf.Add(result);
	TempBuf.Add((BYTE)iSlot);
	TempBuf.Add(m_UserItem[iSlot].sLevel);
	TempBuf.Add(m_UserItem[iSlot].sSid);
	TempBuf.Add(m_UserItem[iSlot].sDuration);
	TempBuf.Add(m_UserItem[iSlot].sBullNum);
	TempBuf.Add(m_UserItem[iSlot].sCount);
	for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[iSlot].tMagic[j]);

	TempBuf.Add(m_UserItem[iSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());
//	if(bQuickChange) SendQuickChange();				// �������� ������������...
}


//	��ȭ ��ǰ��, ��ȭ�� ��ǰ���� �ϳ��� ���ֱ� ���� ���� �Լ��̴�.
//	���� ��ũ��Ʈ�� �Ҹ��� ���� �ƴ϶� RecvAddress������ �θ���.
//	��ũ��Ʈ�� �θ���, �������� ����ϰ� ���������� ������ �ʿ��Ҳ�?
BOOL USER::EventRobItem(int item_index, int quality)
{
	int sid = item_index;
	int num = 1;
	if(sid < 0 || sid >= g_arItemTable.GetSize()) return FALSE;

	int i, j;
	int iSlot = -1;

	CBufferEx TempBuf;
	BYTE result = SUCCESS;

	ItemList	TempItem;
	ReSetItemSlot(&TempItem);

	TempItem.sSid		= g_arItemTable[sid]->m_sSid;
	TempItem.sBullNum	= g_arItemTable[sid]->m_sBullNum;
	TempItem.sDuration	= g_arItemTable[sid]->m_sDuration;

	for(i =0; i < MAGIC_NUM; i++) TempItem.tMagic[i] = 0;
	TempItem.tIQ = quality;
	
	for(i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
	{
		if(m_UserItem[i].sSid == TempItem.sSid)
		{
			for(j = 0; j < MAGIC_NUM; j++)
			{
				if(m_UserItem[i].tMagic[j] != TempItem.tMagic[j]) break;
			}
				
			if(m_UserItem[i].tIQ == TempItem.tIQ) { iSlot = i; break; } 
		}
	}

	if(iSlot == -1) return FALSE;

	if(m_UserItem[iSlot].sSid < 0) return FALSE;
	if(m_UserItem[iSlot].sCount < num) return FALSE;

	if((m_UserItem[iSlot].sCount - num) <= 0)				// ���� �� �κ��� �� �ش�.		
	{														// �������� ��ȭ�� ������		
//		if(g_arItemTable[iSlot]->m_byWear >= 6) bQuickChange = TRUE;

		ReSetItemSlot(&m_UserItem[iSlot]);
	}
	else m_UserItem[iSlot].sCount -= num;

	int iWeight = num * g_arItemTable[iSlot]->m_byWeight;

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();			// �ٽ� ȸ���ӵ��� ���

	TempBuf.Add(ITEM_GIVE_RESULT);
	TempBuf.Add(result);
	TempBuf.Add((BYTE)iSlot);
	TempBuf.Add(m_UserItem[iSlot].sLevel);
	TempBuf.Add(m_UserItem[iSlot].sSid);
	TempBuf.Add(m_UserItem[iSlot].sDuration);
	TempBuf.Add(m_UserItem[iSlot].sBullNum);
	TempBuf.Add(m_UserItem[iSlot].sCount);
	for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[iSlot].tMagic[j]);

	TempBuf.Add(m_UserItem[iSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());
//	if(bQuickChange) SendQuickChange();				// �������� ������������...

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//	�̺�Ʈ���� XP�� �÷��ش�.
//
void USER::AddXPUp(int iNum)
{
	if(iNum <= 0) return;

	CheckMaxValue(m_dwXP, iNum);		// ���� �������� 1 ����!	
	SendXP();
}

////////////////////////////////////////////////////////////////////////////
//	NPC Event �� EXEC ����� �����Ѵ�.
//
BOOL USER::RunNpcEvent(CNpc *pNpc, EXEC *pExec)
{
	int nDist = 0;
	int result = 0;
	short sID = 0;
	CBufferEx test;
	test.Add(sID);
	
	if(pNpc)
	{
		if(pNpc->m_sCurZ != m_curz) return TRUE;
	}

	switch( pExec->m_Exec )
	{
	case EXEC_RETURN:
		return FALSE;

	case EXEC_BBS_OPEN:
		BBSOpen( pExec->m_ExecInt[0] );
		break;

	case EXEC_ZONE_MOVE:			
		if(pNpc) nDist = abs(m_curx - pNpc->m_sCurX) + abs(m_cury - pNpc->m_sCurY);
		if(nDist > SCREEN_X + SCREEN_Y) return TRUE;
		ZoneMoveReq( pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2] );
		break;

	case EXEC_RR_ENTER:
		if( !g_RR.Enter( this ) ) return TRUE;
		ZoneMoveReq( pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2] );
		break;

	case EXEC_FT_MOVE:
		FortressMove(pNpc,  pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2], pExec->m_ExecInt[3], pExec->m_ExecInt[4] );
		break;

	case EXEC_GUARD_BOW:			// ��� �λ��ϱ�
		GuardBow(pNpc);
		break;

	case EXEC_MOVE:					// ���������� Ư�� ��ҷ� �̵���Ų��.(���߿� �ʿ����� ���!)
		EventMove( pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2] );
		break;

	case EXEC_ROB_DN:
		RobDN( pExec->m_ExecInt[0] );
/*		if(m_dwDN < (DWORD)pExec->m_ExecInt[0])
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_DINA, SYSTEM_NORMAL, TO_ME);
			return FALSE;
		}
		m_dwDN -= pExec->m_ExecInt[0];
		SendMoneyChanged();	*/
		break;

	case EXEC_SELL_OPEN:			// ������ �ȱ�
		SellItemOpenReq( pExec->m_ExecInt[0] );
		break;

	case EXEC_BANK_OPEN:
	    	BankOpenReq();
		break;

	case EXEC_ACCOUNT_BANK_OPEN:
	    	AccountBankOpenReq( pExec->m_ExecInt[0] );
		break;

	case EXEC_GUILD_WAREHOUSE_OPEN:
		GuildWharehouseOpenReq();
//		GuildWharehouseOpenReqWithThread();

		break;

	case EXEC_REPAIR_OPEN:			// ������ �����ϱ�
		RepairItemOpenReq( pExec->m_ExecInt[0] );
		break;

	case EXEC_STORE_OPEN:			// ������ ���
		SendStoreOpen( pExec->m_ExecInt[0], pExec->m_ExecInt[1] );
		break;

	case EXEC_FIELD_STORE_OPEN:		// �ʵ忡�� ���� ������ ��������...
		SendFieldStoreOpen( pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2] );
		break;

	case EXEC_GUILD_SAY:
		SendFieldGuildSay( pNpc, pExec->m_ExecInt[0], pExec->m_ExecInt[1] );
		break;

	case EXEC_GUILD_TAX:
		SendStoreTax( pExec->m_ExecInt[0] );
		break;

	case EXEC_GUILD_SCHEME:
		SendGuildWarScheme( pExec->m_ExecInt[0] );
		break;

	case EXEC_GUILD_APPLY:
		GuildWarApply( pExec->m_ExecInt[0] );
		break;

	case EXEC_PSI_STORE_OPEN:
		SendPsiStoreOpen( pExec->m_ExecInt[0] );
		break;

	case EXEC_SAY:
		SendNpcSay( pNpc, pExec->m_ExecInt[0] );
		break;

	case EXEC_ALL_CURE:				// ��� hp, sp, pp�� ��ä���.
		SendAllCure( pExec->m_ExecInt[0],  pExec->m_ExecInt[1] );
		break;

	case EXEC_CARE:					// ��� �����̻� ġ���Ѵ�.
		SendSystemMsg(IDS_USER_UNDER_CONSTRUCTION, SYSTEM_NORMAL, TO_ME);
		break;

	case EXEC_GUILD_OPEN:
		GuildOpen();
		break;

	case EXEC_EBODY_OPEN:
//		SendEBodyOpen( pExec->m_ExecInt[0] );
		break;

	case EXEC_EBODY_DEFINE:
		EBodyIdentifyOpen( pExec->m_ExecInt[0] );
		break;

	case EXEC_UPGRADE_OPEN:
		UpgradeItemOpen(pExec->m_ExecInt[1]);
		break;

	case EXEC_ADD_EVENT:
		AddMyEventNum( pExec->m_ExecInt[0] );
		break;
	case EXEC_ADD_SKILL://120����ת��ʱ..�õ�����
		AddSkill(pExec->m_ExecInt[0]);
		break;

	case EXEC_DEL_EVENT:
		DelMyEventNum( pExec->m_ExecInt[0] );
		break;

	case EXEC_ROB_ITEM:
		RobItem( pExec->m_ExecInt[0], pExec->m_ExecInt[1] );
		break;

	case EXEC_EVENT_ROB_ITEM:
//		EventRobItem( pExec->m_ExecInt[0], pExec->m_ExecInt[1] );
		break;

	case EXEC_XP_UP:
		AddXPUp( pExec->m_ExecInt[0] );
		break;

	case EXEC_CHECK_GUILD_USER:
		SendGuildUserInfo( pExec->m_ExecInt[0] );
		break;

	case EXEC_CHECK_GUILD_RANK:
		SendGuildHouseRank( pExec->m_ExecInt[0], pExec->m_ExecInt[1] );
		break;

	case EXEC_FT_REPAIR:
		SendRepairItem( pExec->m_ExecInt[0] );
		break;

	case EXEC_FT_PEOPLE:
		SendPeopleSay( pExec->m_ExecInt[0] );
		break;

	case EXEC_GIVE_ITEM:
		GiveItem( pExec->m_ExecInt[0], pExec->m_ExecInt[1] );
		break;

	case EXEC_GIVE_MAGIC_ITEM:
		GiveMagicItem( pExec->m_ExecInt[0], pExec->m_ExecInt[1] );
		break;

	case EXEC_GIVE_EVENT_ITEM:
		GiveEventItem( pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2], pExec->m_ExecInt[3] );
		break;

	case EXEC_ADDRESS_WINDOW:
		AddressWindowOpen(pExec->m_ExecInt[0], pExec->m_ExecInt[1]);
		break;
	case EXEC_INPUT_WINDOW:				
		EventItemSerialWindowOpen();
		break;

	case EXEC_INPUTGAME_WINDOW:
		EventItemSerialForGameRoomWindowOpen();
		break;

	case EXEC_ADD_GAMETIME_ITEM:
		GiveEventGameTime(pExec->m_ExecInt[0], pExec->m_ExecInt[1]);
		break;

	case EXEC_DRESSING:
		DressingWindowOpen();
		break;

	case EXEC_GIVE_DN:
		GiveDN( pExec->m_ExecInt[0] );
		break;

	case EXEC_SEND_MSG:
		SendEventMsg( pExec->m_ExecChar );
		break;

	case EXEC_CHANGE_UPGRADE_ACC:
		ChangeUpgradeAcc();
		break;

	case EXEC_APPLY_GUILD_RUN:
		ApplyGuildRun();
		break;

	case EXEC_MON_SAY:
		SendMonsterSay( pNpc, pExec->m_ExecInt[0], pExec->m_ExecChar );
		break;

	case EXEC_SUMMON:
		if(!ExcuteSingleQuestEvent( pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2], pExec->m_ExecInt[3], 
								pExec->m_ExecInt[4], pExec->m_ExecInt[5], pExec->m_ExecInt[6])) return FALSE;
		break;

	case EXEC_RUN_EVENT:
		{
			EVENT* pEvent = GetEventInCurrentZone();	if( !pEvent ) break;

			if( !pEvent->m_arEvent[pExec->m_ExecInt[0]] ) break;

			if( !CheckEventLogic( pEvent->m_arEvent[pExec->m_ExecInt[0]] ) ) break;

			for( int i = 0; i < pEvent->m_arEvent[pExec->m_ExecInt[0]]->m_arExec.GetSize(); i++)
			{
				if( !RunNpcEvent( pNpc, pEvent->m_arEvent[pExec->m_ExecInt[0]]->m_arExec[i] ) )
				{
					return FALSE;
				}
			}
		}
		break;
	case EXEC_HELPERCURE:
		SendHelperCure( );
		break;
	case EXEC_GIVE_PSI:
		if(!GivePSI( pExec->m_ExecInt[0], pExec->m_ExecInt[1])) return FALSE;
		break;
	case EXEC_100_1://100������ת��
		break;
	case EXEC_100_2://�����������
		break;
	case EXEC_100_3:// ��Ӽ��������
		break;
	case EXEC_100_4://��������
		break;
	case EXEC_100_5://���߲��ϻ�
		break;
	case EXEC_100_6://������ϻ�
		break;
	case RUN_SKILL:
		RunSkill120( pExec->m_ExecInt[0]);
		break;
	case EXEC_GIVE_MAGIC:////����Ƿ����ѧϰ��ħ����ħ�� ���ԵĻ����͸�һ��
		GiveMagic_30();
		break;
	case EXEC_GIVE_ITEM_ALL: //����һ����Ʒ
		GiveAllItem(pExec->m_ExecInt[0],pExec->m_ExecInt[1],pExec->m_ExecInt[2],pExec->m_ExecInt[3],
		            pExec->m_ExecInt[4],pExec->m_ExecInt[5],pExec->m_ExecInt[6],pExec->m_ExecInt[7]);
		break;
	case EXEC_TRANSFORM:
		TransformWindow( );
		break;
	default:
		break;
	}

	return TRUE;
}
//code ��������������ɫ�����ԣ������ԣ������ԣ������ԣ�
void USER::GiveAllItem(int sSid,int sCount,int iUp,int sIQ,int a1,int a2,int a3,int a4)
{
		CWordArray		arEmptySlot, arSameSlot;
		int iSlot = GetEmptySlot( INVENTORY_SLOT );

		if( iSlot < 0 ) return;

		ReSetItemSlot( &m_UserItem[iSlot]);

		m_UserItem[iSlot].sLevel = g_arItemTable[a1]->m_byRLevel;
		m_UserItem[iSlot].sSid = sSid;
		m_UserItem[iSlot].sCount = sCount;
		m_UserItem[iSlot].sDuration = g_arItemTable[a1]->m_sDuration;
		m_UserItem[iSlot].sBullNum = 0;
		m_UserItem[iSlot].tMagic[0] = a1;
		m_UserItem[iSlot].tMagic[1] = a2;
		m_UserItem[iSlot].tMagic[2] = a3;
		m_UserItem[iSlot].tMagic[3] = a4;
		m_UserItem[iSlot].tMagic[4] = 0;
		m_UserItem[iSlot].tMagic[5] = iUp;
		m_UserItem[iSlot].tIQ = sIQ;
		m_UserItem[iSlot].iItemSerial = 0;

	//	MakeItemLog( &m_UserItem[iSlot], ITEMLOG_EVENT_GIVE );

		arEmptySlot.Add(iSlot); 
		UpdateInvenSlot(&arEmptySlot, &arSameSlot);

}
void USER::GiveMagic_30()
{
	CBufferEx TempBuf;

	if(IsHavePsionic(6)==false){
		SendNpcSay( NULL, 665);
		return;
	}
	if(m_dwXP<8000){
		SendNpcSay( NULL, 666);
		return;
	}else{
		m_dwXP=m_dwXP-8000;
	}
	for(int j = 0; j < m_nHavePsiNum; j++)
	{
		if(m_UserPsi[j].sSid == 6) 
			m_UserPsi[j].sSid=30;
	}

	AddMyEventNum(202);
	SendNpcSay( NULL, 664);

	TempBuf.Add(BUY_PSY_RESULT);
	TempBuf.Add((byte)1);
	short sPsiCount = 1;
	TempBuf.Add(sPsiCount);
	for(int i = 0; i < sPsiCount; i++)
	{
		short sSid = (short)30;
		TempBuf.Add(sSid);
	}
	RobItem(1005,1);
	SendXP();
	Send(TempBuf, TempBuf.GetLength());


}
/////////////////////////////////////////////////////////////////////////
//	�������� �̵��� �����ϰ� ����ü�� �������� ȿ������...
//
void USER::FortressMove(CNpc *pNpc, int sid, int x1, int y1, int x2, int y2) // ��-->����, ����-->�� 
{
//	BOOL bSuc = TRUE;
	int nDist = 0;

	if(!pNpc) return;

	nDist = abs(m_curx - pNpc->m_sCurX) + abs(m_cury - pNpc->m_sCurY);
	if(nDist > SCREEN_X + SCREEN_Y) return;

	CGuildFortress* pFort = NULL;
	pFort = GetFortress(sid);

	if(!pFort || pFort->m_iGuildSid <= 0) return;

	int type = CheckInvalidMapType();

	if(pNpc->m_tGuildWar == GUILD_WARRING && pNpc->m_NpcState == NPC_DEAD) return;

	if(pFort->m_iGuildSid != m_dwGuild)
	{
		SendSystemMsg( IDS_USER_NOT_THIS_GUILD_MEMBER, SYSTEM_ERROR, TO_ME);
		return;
	}
//	if(!pFort->m_bHaveGuild && pFort->m_lUsed == 0) bSuc = TRUE;

//	if(bSuc)
//	{
		switch(type)
		{
		case 8: case 10: case 15:
			EventMove( pNpc->m_sCurZ, x2, y2 );
			break;
		case 9: case 11: case 16:						
			EventMove( pNpc->m_sCurZ, x1, y1 );
			break;
		}		
		return;
//	}
//	else 
//	{
//		SendSystemMsg( IDS_USER_NOT_THOS_GUILD_MEMBER, SYSTEM_ERROR, TO_ME);
//	}
}

/////////////////////////////////////////////////////////////////////////
//	���� ���ִ� ������ �߻��ϴ� �̺�Ʈ�� �����´�.
//
EVENT* USER::GetEventInCurrentZone()
{
	int i;//, event_index = -1;
	EVENT *pEvent;

	for( i = 0; i < g_event.GetSize(); i++)
	{
		pEvent = g_event[i];
		if( !pEvent ) continue;
		if( pEvent->m_Zone == m_curz )
		{
//			event_index = i;
//			break;
			return g_event[i];
		}
	}

//	if(event_index == -1) return NULL;

//	return g_event[event_index];
	return NULL;
}

/////////////////////////////////////////////////////////////////////////
//	���� ������ �̺�Ʈ�� ����Ѵ�.
//
void USER::AddMyEventNum(int seventnum)
{
	int i;
	int* senum;

	if(MAX_EVENT_NUM <= m_arEventNum.GetSize()) return;

	for( i = 0; i < m_arEventNum.GetSize(); i++)
	{
		if( !m_arEventNum[i] ) continue;

		if( *(m_arEventNum[i]) == seventnum )
		{
			return;
		}
	}

	senum = new int;

	*senum = seventnum;

	m_arEventNum.Add( senum );
}

/////////////////////////////////////////////////////////////////////////
//	������ �̺�Ʈ�� �����Ѵ�.
//
void USER::DelMyEventNum(int seventnum)
{
	int i;

	for( i = 0; i < m_arEventNum.GetSize(); i++)
	{
		if( !m_arEventNum[i] ) continue;

		if( *(m_arEventNum[i]) == seventnum )
		{
			delete m_arEventNum[i];
			m_arEventNum[i] = NULL;
			m_arEventNum.RemoveAt( i, 1 );
			DelMyEventNum( seventnum );
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////
//	��� �λ� �̺�Ʈ�� �����Ѵ�.
//
void USER::GuardBow(CNpc *pNpc)
{
	if(!pNpc) return;
	if(m_sCityRank < 0) return;

	CString strMsg =_T("");
	int nMax = 0, index = 0;

	CBufferEx TempBuf, TempBuf2;

	CPoint startPt, pt;

	startPt.x = pNpc->m_sCurX;
	startPt.y = pNpc->m_sCurY;
	pt.x = m_curx;
	pt.y = m_cury;

	index = GetDirection(startPt, pt);		// ���� ������ ǥ���Ѵ�.;

	TempBuf2.Add(GUARD_BOW);
	TempBuf2.Add((int)pNpc->m_sNid + NPC_BAND);
	TempBuf2.Add((BYTE)index);

	SendScreen(TempBuf2, TempBuf2.GetLength());

	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(NORMAL_CHAT);
	TempBuf.Add(pNpc->m_sNid + NPC_BAND);
	TempBuf.AddString(pNpc->m_strName);

	nMax = g_arNpcSayEventTable.GetSize();

	index = myrand(0, nMax - 1);

	strMsg = g_arNpcSayEventTable[index]->Say;
	strMsg.Replace("%s", m_strUserID);

	TempBuf.AddString((LPTSTR)(LPCTSTR)strMsg);

	SendScreen(TempBuf, TempBuf.GetLength());

}

/////////////////////////////////////////////////////////////////////////
//	�ش� �ʵ� ���ο� ���õ� ������ �����ؼ� �˷��ش�.
//
void USER::SendFieldGuildSay(CNpc *pNpc, int nStore, int say)
{
	int nChatNum = -1;
	CStore* pStore = NULL;
	CGuildFortress* pFort = NULL;

	if(nStore >= FORTRESS_BAND)						// ����� ���� �����̸�...
	{
		pFort = GetFortress(nStore);
		if(pFort == NULL) return;

		if(pNpc)
		{
			if(pNpc->m_tNpcType == NPCTYPE_GUILD_GUARD)
			{
				if(pFort->m_tWarType != GUILD_WAR_DECISION)
				{
					if(pFort->m_sFortressID == 1000) nChatNum = 145;
					else if(pFort->m_sFortressID == 1001) nChatNum = 165;
					else if(pFort->m_sFortressID == 1002) nChatNum = 318;
				}
				else
				{
					if(pFort->m_iGuildSid != m_dwGuild && m_bGuildMaster)
					{
						for(int i = 0; i < GUILDFORTRESS_ATTACK_MAX_NUM; i++)
						{
							if(pFort->m_arAttackGuild[i].lGuild == m_dwGuild)
							{
								if(pFort->m_sFortressID == 1000) nChatNum = 144;
								else if(pFort->m_sFortressID == 1001) nChatNum = 164;
								else if(pFort->m_sFortressID == 1002) nChatNum = 317;
							}
						}

						if(nChatNum == -1)
						{
							if(pFort->m_iGuildSid == 900000)
							{
								if(pFort->m_sFortressID == 1000) nChatNum = 146;
								else if(pFort->m_sFortressID == 1001) nChatNum = 166;
								else if(pFort->m_sFortressID == 1002) nChatNum = 319;
							}
							else 
							{
								if(pFort->m_sFortressID == 1000) nChatNum = 143;
								else if(pFort->m_sFortressID == 1001) nChatNum = 163;
								else if(pFort->m_sFortressID == 1002) nChatNum = 316;
							}
						}
					}
					else 
					{
						if(pFort->m_sFortressID == 1000) nChatNum = 144;
						else if(pFort->m_sFortressID == 1001) nChatNum = 164;
						else if(pFort->m_sFortressID == 1002) nChatNum = 317;
					}
				}
			}
			else
			{
				nChatNum = say;
			}
		}
		else
		{
			nChatNum = say;
		}
/*
		if(m_bGuildMaster)
		{
			if(pFort->m_iGuildSid == m_dwGuild) nChatNum = say;
			for(int i = 0; i < GUILDFORTRESS_ATTACK_MAX_NUM; i++)
			{
				if(pFort->m_arAttackGuild[i].lGuild == m_dwGuild) { nChatNum = 144; break; }
			}

			if(nChatNum == -1 && pFort->m_tWarType == GUILD_WAR_DECISION) nChatNum = 143;
			if(nChatNum == -1) nChatNum = 145;
		}
		else
		{
			if(pFort->m_tWarType == GUILD_WAR_DECISION)
			{
				if(pFort->m_iGuildSid == 900000)
				{
					if(pFort->m_sFortressID == 1000) nChatNum = 146;
					else if(pFort->m_sFortressID == 1001) nChatNum = 166;
				}
				else nChatNum = 144;
			}
			else 
			{
				SendSystemMsg( IDS_USER_NOT_YET_FIX_SCHEDULE, SYSTEM_NORMAL, TO_ME);
				return;
			}
		}
*/
		EditHyperFortressText(pFort, nChatNum);
	}
/*	else
	{
		pStore = GetStore(nStore);
		if(pStore == NULL) return;

		if(say == 1)								// ��峻�� �������θ�...
		{
			if(pStore->m_sStoreID == 4)
			{
				if(strcmp(pStore->m_strMasterName, m_strUserID) == 0) nChatNum = 33;		// 
				else if(pStore->m_tWarType == GUILD_WAR_PREPARE) nChatNum = 36;
				else if(pStore->m_tWarType == GUILD_WAR_DECISION) nChatNum = 34;
			}
			else if(pStore->m_sStoreID == 5)
			{
				if(strcmp(pStore->m_strMasterName, m_strUserID) == 0) nChatNum = 43;		// 
				else if(pStore->m_tWarType == GUILD_WAR_PREPARE) nChatNum = 46;
				else if(pStore->m_tWarType == GUILD_WAR_DECISION) nChatNum = 44;
			}
			else if(pStore->m_sStoreID == 6)
			{
				if(strcmp(pStore->m_strMasterName, m_strUserID) == 0) nChatNum = 53;		// 
				else if(pStore->m_tWarType == GUILD_WAR_PREPARE) nChatNum = 56;
				else if(pStore->m_tWarType == GUILD_WAR_DECISION) nChatNum = 54;
			}
		}
		else
		{
			if(say == 37 && pStore->m_iGuildSid >= 900000) nChatNum = 30;	// ��尡 ó�� �о��Ҷ� 37�� SAY�� ����ȣ�� 900000�϶���...
			else if(say == 47 && pStore->m_iGuildSid >= 900000) nChatNum = 40;
			else if(say == 57 && pStore->m_iGuildSid >= 900000) nChatNum = 50;
			else nChatNum = say;// ������ �ܿ� ����� ª�� ��
		}

		EditHyperText(pStore, nChatNum);
	}
*/
}

/////////////////////////////////////////////////////////////////////////
//	�������� ��� ���� ��û�Ѵ�.
//
void USER::GuildWarApply(int nStore)
{
	int nChatNum = -1;
	int nCount = 0;
	CStore* pStore = NULL;
	CGuildFortress* pFort = NULL;

	if(nStore >= FORTRESS_BAND)						// ����� ���� �����̸�...
	{
		pFort = GetFortress(nStore);
		if(pFort == NULL) return;

		if(pFort->m_tWarType == GUILD_WAR_DECISION) 
		{	
			if(m_bGuildMaster) 
			{
				CBufferEx TempBuf;

				TempBuf.Add(GUILD_WAR_APPLY_OPEN);
				TempBuf.Add((short)pFort->m_sFortressID);
				TempBuf.AddString(pFort->m_strGuildName);

				Send(TempBuf, TempBuf.GetLength());
				return;
			}		
		}

		nChatNum = 145;

		EditHyperFortressText(pFort, nChatNum);
	}
/*	else
	{
		pStore = GetStore(nStore);
		if(pStore == NULL) return;

		if(pStore->m_tWarType == GUILD_WAR_DECISION) 
		{
			if(strcmp(pStore->m_strMasterName, m_strUserID) == 0)	// �ش� ��帶���͸�...
			{
				if(nStore == 4) nChatNum = 38;
				else if(nStore == 5) nChatNum = 48;
				else if(nStore == 6) nChatNum = 58;
			}
	//		else if(!m_bGuildMaster) nChatNum = 38;					// �Ϲ�����..
*/	/*		else if(m_bGuildMaster)									// �渶....
			{
				for(i = 0; i < GUILD_ATTACK_MAX_NU8M; i++)
				{
					if(pStore->m_arAttackGuild[i] == m_dwGuild)
					{
						nChatNum = 38;
						break;
					}
				}
	*/
/*				if(nChatNum < 0) 
				{
					CBufferEx TempBuf;

					TempBuf.Add(GUILD_WAR_APPLY_OPEN);
					TempBuf.Add((short)pStore->m_sStoreID);
					TempBuf.AddString(pStore->m_strGuildName);

					Send(TempBuf, TempBuf.GetLength());
				}		
	//		}
		}
		else SendSystemMsg( IDS_USER_NOT_YET_FIX_SCHEDULE, SYSTEM_NORMAL, TO_ME);

		EditHyperText(pStore, nChatNum);
	}
*/
}

/////////////////////////////////////////////////////////////////////////
//	�������� ��� ���� ��û�Ѵ�.
//
void USER::GuildWarApplyReq(TCHAR *pBuf)
{
	int index = 0;

	if(!m_bGuildMaster || m_dwGuild <= 0)
	{
		SendSystemMsg( IDS_USER_ONLY_GUILD_MASTER_WAR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	CString strMsg;
	short sStore = GetShort(pBuf, index);

	if(sStore < 0) return;

	if(sStore >= FORTRESS_BAND)	GuildFortressWarApplyReq(sStore);		
//	else						GuildFieldStoreWarApplyReq(sStore);
}

void USER::GuildFieldStoreWarApplyReq(int sid)
{
/*	int i, j;

	CString strMsg;
	
	CStore *pStore = NULL;
	CGuildFortress *pFort = NULL;

	pStore = GetStore(sid);

	if(!pStore) return;

	if(pStore->m_lUsed == 1)
	{
		SendSystemMsg( IDS_USER_CANT_APPLY_IN_WAR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(pStore->m_iGuildSid == m_dwGuild)
	{
		SendSystemMsg( IDS_USER_CANT_APPLY_TO_MY_GUILD, SYSTEM_NORMAL, TO_ME);
		return;
	}

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(g_arGuildFortress[i] == NULL) continue;

		pFort = g_arGuildFortress[i];

		if(pFort->m_iGuildSid == m_dwGuild)
		{
			SendSystemMsg( IDS_USER_CANT_APPLY_HAVE_FORTRESS, SYSTEM_NORMAL, TO_ME);
			return;
		}
	}

	for(i = 0; i < g_arStore.GetSize(); i++)
	{
		if(g_arStore[i] == NULL) continue;

		if(InterlockedCompareExchange((LONG*)&g_arStore[i]->m_lListUsed, (PVOID*)1, (PVOID*)0) == 0)
		{
			for(j = 0; j < GUILD_ATTACK_MAX_NUM; j++)
			{
				if(g_arStore[i]->m_arAttackGuild[j] == m_dwGuild)
				{
					strMsg.Format( IDS_USER_ALREADY_APPLY_OTHER_GUILD, g_arStore[i]->m_strGuildName);
					SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);					
					InterlockedExchange(&g_arStore[i]->m_lListUsed, 0);
					return;
				}
			}
			InterlockedExchange(&g_arStore[i]->m_lListUsed, 0);
		}
	}

	if(pStore->m_tWarType == GUILD_WAR_DECISION)
	{
		for(i = 0; i < g_arStore.GetSize(); i++)
		{
			if(g_arStore[i] == NULL) continue;

			if(g_arStore[i]->m_sStoreID == sid)
			{
				if(g_arStore[i]->m_lListCount >= GUILD_ATTACK_MAX_NUM) 
				{
					strMsg.Format( IDS_USER_OVERFLOW_GUILD_WAR_ENEMY, pStore->m_strGuildName);
					SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);					
					return;
				}

				if(InterlockedCompareExchange((LONG*)&g_arStore[i]->m_lListUsed, (PVOID*)1, (PVOID*)0) == 0)
				{
					g_arStore[i]->m_arAttackGuild[g_arStore[i]->m_lListCount] = m_dwGuild;
					InterlockedIncrement(&g_arStore[i]->m_lListCount);

					UpdateMemStoreGuildList(g_arStore[i]);		// �޸�DB�� ����.

					SendSystemMsg( IDS_USER_APPLIED_GUILD_WAR, SYSTEM_NORMAL, TO_ME);					

					InterlockedExchange(&g_arStore[i]->m_lListUsed, 0);
				}
			}
		}
	}
	else SendSystemMsg( IDS_USER_NOT_FIX_SCHEDULE, SYSTEM_NORMAL, TO_ME);
*/
}

void USER::GuildFortressWarApplyReq(int sid)
{
	int i, j, nLen = 0;

	CString strMsg;

	CGuild *pGuild = NULL;
	CGuildFortress *pFort = NULL;
	CGuildFortress *pTempFort = NULL;

	pFort = GetFortress(sid);

	if(!pFort) return;

	if(m_sLevel < 70)
	{
		SendSystemMsg( IDS_USER_APPLY_CONDITION_LEVEL, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(pFort->m_lUsed == 1)
	{
		SendSystemMsg( IDS_USER_CANT_APPLY_IN_WAR, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(pFort->m_iGuildSid == m_dwGuild)
	{
		SendSystemMsg( IDS_USER_CANT_APPLY_TO_MY_GUILD, SYSTEM_NORMAL, TO_ME);
		return;
	}

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) 
	{
		ReleaseGuild();				// ����...
		return;
	}

	j = 0;
	CGuildUser pGUser;

	for(i = 0; i < MAX_GUILD_USER; i++)
	{
		pGUser = pGuild->m_arUser[i];
		if(pGUser.m_lUsed != 0) j++;
	}

	ReleaseGuild();

	if(j < 20)
	{
		SendSystemMsg( IDS_USER_APPLY_CONDITION_GMEMBER, SYSTEM_NORMAL, TO_ME);
		return;
	}
/*
	for(i = 0; i < g_arGuildHouse.GetSize(); i++)
	{
		if(g_arGuildHouse[i]->iGuild == m_dwGuild)
		{
			SendSystemMsg( IDS_USER_CANT_APPLY_HAVE_HOUSE, SYSTEM_NORMAL, TO_ME);
			return;
		}
	}
*/
	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(g_arGuildFortress[i] == NULL) continue;

		pTempFort = g_arGuildFortress[i];

		if(pTempFort->m_iGuildSid == m_dwGuild)
		{
			SendSystemMsg( IDS_USER_CANT_APPLY_HAVE_FORTRESS, SYSTEM_NORMAL, TO_ME);
			return;
		}
	}

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(g_arGuildFortress[i] == NULL) continue;

		pTempFort = g_arGuildFortress[i];

		if(InterlockedCompareExchange((LONG*)&g_arGuildFortress[i]->m_lListUsed, (long)1, (long)0) == (long)0)
		{
			for(j = 0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)
			{
				if(pTempFort->m_arAttackGuild[j].lGuild == m_dwGuild)
				{
					if(pTempFort->m_sFortressID == 1000) strMsg.Format( IDS_USER_ALREADY_LUINET_FORTRESS );
					else if(pTempFort->m_sFortressID == 1001) strMsg.Format( IDS_USER_ALREADY_SANAD_FORTRESS );
					else if(pTempFort->m_sFortressID == 1002) strMsg.Format( IDS_USER_ALREADY_ZAMELLYA_FORTRESS );

					SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);					
					InterlockedExchange(&g_arGuildFortress[i]->m_lListUsed, 0);
					return;
				}
			}
			InterlockedExchange(&g_arGuildFortress[i]->m_lListUsed, 0);
		}
	}

	if(pFort->m_tWarType == GUILD_WAR_DECISION)
	{
		for(i = 0; i < g_arGuildFortress.GetSize(); i++)
		{
			if(g_arGuildFortress[i] == NULL) continue;

			if(g_arGuildFortress[i]->m_sFortressID == sid)
			{
				if(g_arGuildFortress[i]->m_lListCount >= GUILDFORTRESS_ATTACK_MAX_NUM) 
				{
					strMsg.Format( IDS_USER_OVERFLOW_GUILD_WAR_ENEMY, pFort->m_strGuildName);
					SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);					
					return;
				}

				if(InterlockedCompareExchange((LONG*)&g_arGuildFortress[i]->m_lListUsed, (long)1, (long)0) == (long)0)
				{
/*					nLen = strlen(m_strGuildName);
					g_arGuildFortress[i]->m_arAttackGuild[g_arGuildFortress[i]->m_lListCount].lGuild = m_dwGuild;
					strncpy(g_arGuildFortress[i]->m_arAttackGuild[g_arGuildFortress[i]->m_lListCount].strGuildName, m_strGuildName, nLen);
					InterlockedIncrement(&g_arGuildFortress[i]->m_lListCount);
*/					
					if(UpdateFortress(g_arGuildFortress[i]) == FALSE)
					{
						SendSystemMsg( IDS_USER_APPLY_FAIL, SYSTEM_NORMAL, TO_ME);
						InterlockedExchange(&g_arGuildFortress[i]->m_lListUsed, 0);
						return;
					}

//					UpdateMemAttackListNpc(g_arGuildFortress[i]->m_sFortressID);// Agent���� ���̻� ������Ѵ�.
					pTempFort = g_arGuildFortress[i];

					for(j = 0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)
					{
						if(InterlockedCompareExchange((LONG*)&g_arGuildFortress[i]->m_arAttackGuild[j].lUsed, (long)1, (long)0) == (long)0)
						{
							nLen = strlen(m_strGuildName);
							g_arGuildFortress[i]->m_arAttackGuild[j].lGuild = m_dwGuild;
							strncpy(g_arGuildFortress[i]->m_arAttackGuild[j].strGuildName, m_strGuildName, nLen);

							SendSystemMsg( IDS_USER_APPLY_COMPLETED, SYSTEM_NORMAL, TO_ME);
							break;
						}
					}										

					InterlockedExchange(&g_arGuildFortress[i]->m_lListUsed, 0);
				}
			}
		}
	}
	else SendSystemMsg( IDS_USER_NOT_FIX_SCHEDULE, SYSTEM_NORMAL, TO_ME);
}

/////////////////////////////////////////////////////////////////////////
//	���������� Ư����ҷ� �̵���Ų��.
//
void USER::EventMove(int zone, int moveX, int moveY)
{
	BYTE result = FAIL;
	int index = 0;

	if(zone != m_curz) return;

	CPoint pt = FindNearAvailablePoint_C(moveX, moveY);
	CPoint pc;
	if(pt.x != -1 && pt.y != -1) 
	{
		pc = ConvertToServer(pt.x, pt.y);
		if(pc.x < 0 || pc.y < 0 || pc.x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || pc.y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) 
		{
			result = FAIL;
		}
		else
		{
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[pc.x][pc.y].m_lUser, USER_BAND + m_uid);
			m_curx = pc.x;
			m_cury = pc.y;

			result = SUCCESS;
		}
	}

	index = 0;
	SetByte(m_TempBuf, MOVE_CHAT_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		Send(m_TempBuf, index);
		return;
	}

	SetInt(m_TempBuf, m_uid + USER_BAND, index);
	SetShort(m_TempBuf, pt.x, index);
	SetShort(m_TempBuf, pt.y, index);
	Send(m_TempBuf, index);

	m_dwNoDamageTime = NO_DAMAGE_TIME;			// �����ð� ����
	m_dwLastNoDamageTime = GetTickCount();

	SendInsight(m_TempBuf, index);

	SightRecalc();
}

/////////////////////////////////////////////////////////////////////////
//	��� HP, PP, SP�� ��ä���.
//
void USER::SendAllCure(int iStoreID, int iCost)
{
	BYTE result = SUCCESS;

	CBufferEx TempBuf;

	DWORD dwCost = (DWORD)iCost;

	CStore *pStore = NULL;
	pStore = GetStore(iStoreID);

	if(!pStore) return;

	// �̺�Ʈ ��ġ�� �˻� ----------------------------------------//
	CPoint pt = ConvertToClient(m_curx, m_cury);
	if(!g_pEventBlock->CheckUserEvent(m_curz, pt.x, pt.y, CURE_BLOCK)) return;

	DWORD dwTax = (DWORD)( dwCost * ((double)pStore->m_sRate/100) );
	dwCost = (DWORD)(dwCost + dwTax + 0.999);			// �ø�(DWORD �� ����� ���ؼ�)

	UpdateFortressTax(iStoreID, dwTax);					// ��Ʈ���� �����̸� ����...

	if(m_sHP >= m_sMagicMaxHP && m_sPP >= m_sMagicMaxPP && m_sSP >= m_sMagicMaxSP)
	{
		SendSystemMsg( IDS_USER_NEED_NOT_CURE, SYSTEM_NORMAL, TO_ME);
		result = FAIL;
		goto go_result;
	}

	if(dwCost > m_dwDN) 
	{ 
		SendSystemMsg( IDS_USER_NOT_ENOUGH_DINA_CURE, SYSTEM_NORMAL, TO_ME); 
		result = FAIL;
		goto go_result;
	}
	else m_dwDN = m_dwDN - dwCost;

	m_sHP = m_sMagicMaxHP;
	m_sPP = m_sMagicMaxPP;
	m_sSP = m_sMagicMaxSP;

go_result:
	TempBuf.Add(CURE_RESULT);

	if(result == FAIL) 
	{
		TempBuf.Add(result);			// ����
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	TempBuf.Add((BYTE)0x01);			// ȸ��

	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sPP);
	TempBuf.Add(m_sSP);

	SendMoneyChanged();
	Send(TempBuf, TempBuf.GetLength());
	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.

	SendSystemMsg( IDS_USER_CURE_COMPLETED, SYSTEM_NORMAL, TO_ME);
}

/////////////////////////////////////////////////////////////////////////
//	Zone Change�� �����Ѵ�.
//	//^^ �� �Լ� ��ü�� BOOL �����̾�� �� �� ����.
void USER::ZoneMoveReq(int zone, int moveX, int moveY)
{
	BOOL bSuccess = FALSE;							// ����, ������ ������� ����
	BOOL bFoundDBIndex = FALSE;

	int i, j;

	if( !IsZoneInThisServer(zone) )
	{
		SERVERINFO* pNewData;
		int iNum, sZone, find_flag = FALSE;
		CString strIP;
		int nPort;

		// ���� �ش��ϴ� IP, PORT ã��...
		for( i = 0; i < g_server.GetSize() && find_flag == FALSE; i++)
		{
			BOOL bTrue =FALSE;
			pNewData = g_server[i];

			if ( pNewData == NULL ) continue;
																		// -1�̸� ��ΰ� �� �� �ִ� �����̴ϱ� üũ�ϰ� �ϰ�
			if( pNewData->m_sDBIndex != -1 )							// �ƴϸ� ���� �� �� �ִ� �������� üũ�ϰ� �Ѵ�
			{
				if( pNewData->m_sDBIndex != m_iMyServer ) 
				{
					if(pNewData->m_sDBIndex == 11 &&(m_iMyServer == 1 || m_iMyServer == 2 ||m_iMyServer == 3 ||m_iMyServer == 4 ||m_iMyServer == 5))
						bTrue = TRUE;
					else if(pNewData->m_sDBIndex == 12 &&(m_iMyServer == 6 || m_iMyServer == 7 ||m_iMyServer == 8 ||m_iMyServer == 9 ||m_iMyServer == 10))
						bTrue = TRUE;
					else 
						continue;		// �� ���� ������ ���� �����͸� ������ �ִ� �������� �Ѵ�.
				}
			}
			
			// -1�̰ų� �� �� �ִ� ������� �Ǵܵɶ�
			iNum = pNewData->m_zone.GetSize();
			for ( j = 0; j < iNum; j++ )
			{
				sZone = *(pNewData->m_zone[j]);
				if ( sZone == zone )				// ���� ������ �ϴ� ���� �ִ��� üũ
				{
					find_flag = TRUE;				// ������ �� ������ �ּҸ� �����ش�.
					m_strZoneIP = strIP = pNewData->m_szAddr;
					m_nZonePort = nPort = pNewData->m_nPort;

					break;
				}
			}
		}

		if ( find_flag == TRUE )
		{
			TRACE("INTER COMPUTER ZONE CHANGE STARTED[]...\n");

			if( m_bZoneLogOut ) return;

			m_bZoneLogOut = TRUE;

			g_pMainDlg->BridgeServerUserZoneLogOut( m_uid, m_strUserID, zone, moveX, moveY );
			return;

			CPoint pt = ConvertToServerByZone( zone, moveX, moveY );

			if( pt.x < 0 || pt.y < 0 ) return;

			int save_z = zone;
			int save_x = pt.x;
			int save_y = pt.y;

			m_strZoneIP = strIP;
			m_nZonePort	= nPort;

			// ����Ÿ ����...
			TRACE("Zone Change Data Saved...\n");
			ZoneLogOut(save_z, save_x, save_y);
			//

			m_curz = zone;
			m_curx = save_x;
			m_cury = save_y;

			CBufferEx TempBuf;
			BYTE result = FAIL;
			
			TempBuf.Add(ZONE_CHANGE_RESULT);

			result = SUCCESS_ZONE_CHANGE;
			TempBuf.Add(result);
			TempBuf.AddString((char *)LPCTSTR(strIP));
			TempBuf.Add(nPort);

			Send(TempBuf, TempBuf.GetLength());
			return;
		}
		//

		SendZoneChange(FALSE);
		return;
	}

	// ���� �������� �� ��ȯ...
	bSuccess = ZoneChangeProcess(zone, moveX, moveY);
	
	if(bSuccess)
	{
		SendWeatherInMoveZone();			// �̵� ���� ������ȭ�� �˸���.
		SightRecalc();						// �� �þ߾��� ������ ������ ������.
	}

	TRACE("SAVE-COMPUTER ZONE CHANGE...\n");
}


/////////////////////////////////////////////////////////////////////////
//	��ü������ ���� ó���� ����Ѵ�.
//
BOOL USER::ZoneChangeProcess(int new_z, int moveX, int moveY)
{
	int i, rand;
	BYTE result = SUCCESS;

	MAP* pMap = NULL;

	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;

	pMap = g_zone[m_ZoneIndex];		if( !pMap ) return FALSE;

	if( m_curx < 0 || m_curx >= pMap->m_sizeMap.cx ) return FALSE;
	if( m_cury < 0 || m_cury >= pMap->m_sizeMap.cy ) return FALSE;

//	if( pMap->m_pMap[m_curx][m_cury].m_lUser != USER_BAND + m_uid ) return FALSE;

//	if( g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser != USER_BAND + m_uid ) return FALSE;		
					
	CPoint ptPotal, pt;
	m_NowZoneChanging = TRUE;					// Zone Change �ϰ� �ִ�.

	/*************************[ Back Up ]***************************************/	
	int org_x = m_curx;
	int org_y = m_cury;
	int org_z = m_curz;

	/*************************[ Zone Change Init]*******************************/
	SetZoneIndex(new_z);

	ptPotal = ConvertToServer(moveX, moveY);		// �̺�Ʈ ��ǥ�� Ŭ���̾�Ʈ �����̹Ƿ�...

	if( ptPotal.x == -1 || ptPotal.y == -1 )
	{
		SetZoneIndex(org_z);
		return FALSE;
	}

	i = SEARCH_TOWN_POTAL_COUNT;				// ��ǥ�� ã������ Ƚ��
	
	pt = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);

	if(pt.x == -1 || pt.y == -1) 
	{
		while(TRUE)								// Ÿ����Ż ���� ��ǥ�� ��´�.
		{
			rand = myrand(-3, 3);
			ptPotal.x += rand; ptPotal.y += rand;
			pt = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);
			if(pt.x != -1 && pt.y != -1) break;
			i--;
			if(i <= 0) // ���߿�
			{ 
				pt.x = -1; pt.y = -1;
				break;
			}
		}
	}

	if(pt.x == -1 || pt.y == -1) 
	{
		SetZoneIndex(org_z);
		m_NowZoneChanging = FALSE;
		return FALSE;
	}
	else									// �������������̸�...
	{
		int type = ((g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_dwType & 0xFF00 ) >> 8);
		if(!CheckInvakidZoneChangeState(type)) 
		{
			SetZoneIndex(org_z);
			m_NowZoneChanging = FALSE;
			return FALSE;
		}
	}

	SetZoneIndex(org_z);					// ���� �� ��������  �˸���.

	if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 
			(long)0, (long)(m_uid + USER_BAND)) == (long)m_uid + USER_BAND)
	{
//		SetUid(m_curx, m_cury, 0);				
		SendMyInfo(TO_INSIGHT, INFO_DELETE);
		
		SetZoneIndex(new_z);						
		m_curx = pt.x;
		m_cury = pt.y;
		m_curz = new_z;
		SetUid(m_curx, m_cury, m_uid + USER_BAND );// ������ �̵��� �ڸ��� Ȯ��, ��ġ���� ����

		SendZoneChange(TRUE);	// ��ü������  ���������� ���� NPC������ client���� ����ִ�.

		m_presx = -1;
		m_presy = -1;
		SightRecalc();
		SendMyInfo( TO_INSIGHT, INFO_MODIFY );
		
		m_NowZoneChanging = FALSE;

		return TRUE;
	}
	else
	{
		SendMyInfo(TO_INSIGHT, INFO_DELETE);

		SetZoneIndex(new_z);						
		m_curx = pt.x;
		m_cury = pt.y;
		m_curz = new_z;
		SetUid(m_curx, m_cury, m_uid + USER_BAND );// ������ �̵��� �ڸ��� Ȯ��, ��ġ���� ����

		SendZoneChange(TRUE);	// ��ü������  ���������� ���� NPC������ client���� ����ִ�.

		m_presx = -1;
		m_presy = -1;
		SightRecalc();
		SendMyInfo( TO_INSIGHT, INFO_MODIFY );
		
		m_NowZoneChanging = FALSE;

		return TRUE;

//		m_NowZoneChanging = FALSE;
//		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////
//	��ü������ �������� ������.
//
void USER::SendZoneChange(BOOL bResult)
{
	CBufferEx TempBuf;
	BYTE result = FAIL;
	
	TempBuf.Add(ZONE_CHANGE_RESULT);
	
	if(bResult == FALSE)
	{
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());			// ���и� ����
		return;
	}

	CPoint pos = ConvertToClient( m_curx, m_cury );
	if( pos.x == -1 || pos.y == -1 ) { pos.x = 1; pos.y = 1; }	// �̺κ��� ���߿� �ٲ���

	result = SUCCESS;
	TempBuf.Add(result);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);
	TempBuf.Add((short)m_curz);

	Send(TempBuf, TempBuf.GetLength());

	m_dwNoDamageTime = NO_DAMAGE_TIME;
	m_dwLastNoDamageTime = GetTickCount();
}

/////////////////////////////////////////////////////////////////////////
//	NPC ��ȭ������ ������.
//
void USER::SendNpcSay(CNpc *pNpc, int nChatNum)
{
TRACE( "ţţDEGBUG NPC SAY %d\n", nChatNum);
	CBufferEx TempBuf;

	TempBuf.Add(CLIENT_EVENT_SAY);
	TempBuf.Add((BYTE)SUCCESS);
	TempBuf.Add((short)nChatNum);
	TempBuf.Add((BYTE)0x00);		


	Send(TempBuf, TempBuf.GetLength());
}


//////////////////////////////////////////////////////////////////////////////////
//	���� �̺�Ʈ�� �����Ѵ�.
//
void USER::HelpStatusOpenReq()
{
	SendNpcSay(NULL, 7);
}

//////////////////////////////////////////////////////////////////////////////////
//	����Ŭ������ �����͸� ��´�.
//
CStore* USER::GetStore(int nStore)
{
	for(int i = 0; i < g_arStore.GetSize(); i++)
	{
		if(g_arStore[i] == NULL) continue;

		if(g_arStore[i]->m_sStoreID == (short)nStore)
		{
//			int temp = g_arStore[i]->m_sRate;
			return g_arStore[i];
		}
	}

	return NULL;	
}

//////////////////////////////////////////////////////////////////////////////////
//	���Ŭ������ �����͸� ��´�.
//
CGuildFortress* USER::GetFortress(int nFort)
{
	for(int i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(g_arGuildFortress[i] == NULL) continue;

		if(g_arGuildFortress[i]->m_sFortressID == (short)nFort)
		{
			return g_arGuildFortress[i];
		}
	}

	return NULL;	
}

//////////////////////////////////////////////////////////////////////////////////
//	����Ŭ������ �����͸� ��´�.
//
CStore* USER::GetPsiStore(int nStore)
{
	for(int i = 0; i < g_arPsiStore.GetSize(); i++)
	{
		if(g_arPsiStore[i] == NULL) continue;

		if(g_arPsiStore[i]->m_sStoreID == (short)nStore)
		{
			return g_arPsiStore[i];
		}
	}

	return NULL;	
}

//////////////////////////////////////////////////////////////////////////////////
//	����Ŭ������ �����͸� ��´�.
//
CStore* USER::GetEbodyStore(int nStore)
{
	for(int i = 0; i < g_arEBodyStore.GetSize(); i++)
	{
		if(g_arEBodyStore[i] == NULL) continue;

		if(g_arEBodyStore[i]->m_sStoreID == (short)nStore)
		{
			return g_arEBodyStore[i];
		}
	}

	return NULL;	
}
//////////////////////////////////////////////////////////////////////////////////
//	������ �����Ѵ�.
//
void USER::SendStoreOpen(int nStore, int nRate)
{
	int rank = m_sCityRank + CITY_RANK_INTERVAL;

	if(rank == CHAONISE_RANK && IsCity()) 
	{
		SendSystemMsg( IDS_USER_CANT_USE_YOUR_CITY_RANK, SYSTEM_NORMAL, TO_ME);
		return;
	}

	CStore* pStore = NULL;

	pStore = GetStore(nStore);
	if(pStore == NULL) return;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Test Code
//	if(pStore->m_iGuildSid <= 0) pStore->m_sRate = (short)nRate;
	
	CBufferEx TempBuf;
	int i = 0;
	short sid = 0;
	DWORD dwCost = 0;

	TempBuf.Add(STORE_OPEN);
	TempBuf.Add((short)nStore);
	TempBuf.Add((short)pStore->m_sRate);
	TempBuf.Add((short)(pStore->m_arItems.GetSize()));

	for(i = 0; i < pStore->m_arItems.GetSize(); i++)
	{
		sid = (short)pStore->m_arItems[i];
									// �ʵ� ���������� NPC�� �ҼӵǾ� �����Ƿ� �Ǵ�
		if(pStore->m_arNpcList.GetSize() > 0)
		{
			int iCost = (int)(g_arItemTable[sid]->m_iDN * 0.9);
			dwCost = iCost + (int)( iCost * ((double)(pStore->m_sRate)/100) );
		}
		else dwCost = GetItemCostByCityRank(sid, pStore->m_sRate);

		TempBuf.Add(sid);
		TempBuf.Add(dwCost);
	}

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	��尡 ���� �������� �Ǵ��ؼ� ������ �����Ѵ�.
//
void USER::SendFieldStoreOpen(int nStore, int nHave, int nEmpty)
{
	int nChatNum = -1;
	CStore* pStore = NULL;
	CGuildFortress* pFort = NULL;

	if(nStore >= FORTRESS_BAND)						// ����� ���� �����̸�...
	{
		pFort = GetFortress(nStore);
		if(pFort == NULL) return;

		if(pFort->m_lUsed == 1) 
		{
			SendSystemMsg(IDS_USER_CANT_USE_IN_GUILD_WAR, SYSTEM_NORMAL, TO_ME);
			return;
		}

		nChatNum = nEmpty;

		if(pFort->m_bHaveGuild)
		{
			if(pFort->m_iGuildSid > 0) nChatNum = nHave;
		}

		EditHyperFortressText(pFort, nChatNum);
	}
	else											// �ƴϸ� �ʵ忡 ���� ����
	{
		pStore = GetStore(nStore);
		if(pStore == NULL) return;

		if(pStore->m_lUsed == 1) 
		{
			SendSystemMsg( IDS_USER_CANT_USE_IN_GUILD_WAR, SYSTEM_NORMAL, TO_ME);
			return;
		}

		nChatNum = nEmpty;

		if(pStore->m_bHaveGuild)
		{
			if(pStore->m_iGuildSid > 0) nChatNum = nHave;
		}
	
		EditHyperText(pStore, nChatNum);
	}
}

/////////////////////////////////////////////////////////////////////////
//	�ش� SAY�� ������ ���� �������� ������ �Ǵ��Ͽ� ��ȭ��Ų��.
//
void USER::EditHyperText(CStore *pStore, int nNum)
{
	CString temp;

	int i, j, talknum = -1;
	BYTE tCount = 0;

	CHyper hyperText;
	
	if(!pStore) return;
	if(nNum < 0) return;

	CBufferEx TempBuf, TempSayBuf; 

	for(i = 0; i < g_arNpcChat.GetSize(); i++)
	{
		if(g_arNpcChat[i]->m_sCid == nNum)
		{
			talknum = i;
			break;
		}
	}

	if(talknum < 0) return;

	hyperText.GetHyperText(g_arNpcChat[talknum]->m_strTalk, g_arNpcChat[talknum]->m_sSize);
	hyperText.load();

	for(i = 0; i < hyperText.m_nCountLine; i++)
	{
		for(j = 0; j < hyperText.m_HyperText[i].m_strText.GetLength()-7; j++){

			temp = hyperText.m_HyperText[i].m_strText.Mid(j, 2);
			if(temp == "%S")
			{
				if("GM" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))		// ��� �������̸��� �ִ´�.
				{	
					tCount++;
					TempSayBuf.AddString(pStore->m_strMasterName);
//					hyperText.m_HyperText[i].m_strText.Replace("%SGM%", pStore->m_strMasterName);
				}
				else if("GN" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// ��� �̸��� �ִ´�.
				{
//					hyperText.m_HyperText[i].m_strText.Replace("%SGN%", pStore->m_strGuildName);
					tCount++;
					TempSayBuf.AddString(pStore->m_strGuildName);
				}
				else if("GL" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					tCount++;
					temp.Format("%d", pStore->m_wPlanWarTime.wMonth);
//					hyperText.m_HyperText[i].m_strText.Replace("%SGL%", temp);
					TempSayBuf.AddString((LPTSTR)(LPCTSTR)temp);
				}
				else if("GD" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					tCount++;
					temp.Format("%d", pStore->m_wPlanWarTime.wDay);
//					hyperText.m_HyperText[i].m_strText.Replace("%SGD%", temp);
					TempSayBuf.AddString((LPTSTR)(LPCTSTR)temp);
				}
				else if("GT" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					tCount++;
					temp.Format("%d", pStore->m_wPlanWarTime.wHour);
//					hyperText.m_HyperText[i].m_strText.Replace("%SGT%", temp);
					TempSayBuf.AddString((LPTSTR)(LPCTSTR)temp);
				}
/*				else if("STM" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					if(iGHWarNum < 0 || iGHWarNum >= g_arGuildHouseWar.GetSize()) return;
					
					nTime = (int)g_arGuildHouseWar[iGHWarNum]->m_CurrentGuild.dwTimer/1000;
					temp.Format("%d", nTime);
					hyperText.m_HyperText[i].m_strText.Replace("%STM%", temp);
				}
*/			}		
		}
		
	}

	TempBuf.Add(CLIENT_EVENT_SAY);
	TempBuf.Add((BYTE)SUCCESS);
	TempBuf.Add((short)nNum);
	TempBuf.Add(tCount);
	TempBuf.AddData(TempSayBuf, TempSayBuf.GetLength());

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////
//	�ش� SAY�� �������� ���� �������� ������ �Ǵ��Ͽ� ��ȭ��Ų��.
//
void USER::EditHyperFortressText(CGuildFortress *pFort, int nNum)
{
	CString temp;

	int i, j, talknum = -1;
	BYTE tCount = 0;

	CHyper hyperText;
	
	if(!pFort) return;
	if(nNum < 0) return;

	CBufferEx TempBuf, TempSayBuf; 

	for(i = 0; i < g_arNpcChat.GetSize(); i++)
	{
		if(g_arNpcChat[i]->m_sCid == nNum)
		{
			talknum = i;
			break;
		}
	}

	if(talknum < 0) return;

	hyperText.GetHyperText(g_arNpcChat[talknum]->m_strTalk, g_arNpcChat[talknum]->m_sSize);
	hyperText.load();

	for(i = 0; i < hyperText.m_nCountLine; i++)
	{
		for(j = 0; j < hyperText.m_HyperText[i].m_strText.GetLength()-7; j++){

			temp = hyperText.m_HyperText[i].m_strText.Mid(j, 2);
			if(temp == "%S")
			{
				if("GM" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))		// ��� �������̸��� �ִ´�.
				{	
					tCount++;
					TempSayBuf.AddString(pFort->m_strMasterName);
				}
				else if("GN" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// ��� �̸��� �ִ´�.
				{
					tCount++;
					TempSayBuf.AddString(pFort->m_strGuildName);
				}
				else if("GL" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					tCount++;
					temp.Format("%d", pFort->m_wPlanWarTime.wMonth);
					TempSayBuf.AddString((LPTSTR)(LPCTSTR)temp);
				}
				else if("GD" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					tCount++;
					temp.Format("%d", pFort->m_wPlanWarTime.wDay);
					TempSayBuf.AddString((LPTSTR)(LPCTSTR)temp);
				}
				else if("GT" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					tCount++;
					temp.Format("%d", pFort->m_wPlanWarTime.wHour);
					TempSayBuf.AddString((LPTSTR)(LPCTSTR)temp);
				}
				else if("GG" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					tCount++;
					temp.Format("%d", pFort->m_dwRepairCost);
					TempSayBuf.AddString((LPTSTR)(LPCTSTR)temp);
				}

/*				else if("STM" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{
					if(iGHWarNum < 0 || iGHWarNum >= g_arGuildHouseWar.GetSize()) return;
					
					nTime = (int)g_arGuildHouseWar[iGHWarNum]->m_CurrentGuild.dwTimer/1000;
					temp.Format("%d", nTime);
					hyperText.m_HyperText[i].m_strText.Replace("%STM%", temp);
				}
*/			}		
		}
		
	}

	TempBuf.Add(CLIENT_EVENT_SAY);
	TempBuf.Add((BYTE)SUCCESS);
	TempBuf.Add((short)nNum);
	TempBuf.Add(tCount);
	TempBuf.AddData(TempSayBuf, TempSayBuf.GetLength());

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////
//	���̿��� ������ ����� ������.
//
void USER::SendPsiStoreOpen(int iStoreID)
{
	CStore* pStore = NULL;

	pStore = GetPsiStore(iStoreID);
	if(pStore == NULL) return;

	CBufferEx TempBuf;

	short	sSid[TOTAL_PSI_NUM];// �ӽ÷� �ش� ���̿����� �����Ѵ�.
	memset(sSid, -1, sizeof(sSid));

	int i, j;
	int iStart, iEnd;
	int iClass = -1;
	BOOL bflag;	
	short sCount = 0;

	CWordArray arPsi;			
	CByteArray arChangeClass;
	arPsi.RemoveAll();
	arChangeClass.RemoveAll();

	arChangeClass.Add(m_byClass);
	arChangeClass.Add( NOCLASS ); // ���迭 ���̿����� ��� �� �ְ� �Ѵ�

	for(i = 0; i < _CHANGE_DB; i++)// ����, ���� Ŭ������ �����Ѵ�.
	{
		if(m_sChangeClass[i] > 0 && i != m_byClass) arChangeClass.Add(i);
	}

	iStart = arChangeClass.GetSize();

	for(i = 0; i < arChangeClass.GetSize(); i++)
	{
		iStart = pStore->IsCanBuyPsi(pStore, (int)arChangeClass[i]);

		if(iStart >= 0)			// �������� ����ϴ� ����� ������ �ִ°͸� ����
		{
			switch(arChangeClass[i])			
			{
			case BRAWL:			// �ش� Ŭ���� �Ǵ� ���� Ŭ������ �ش��ϴ� ���̿��� ��ϸ� ����
				iEnd = iStart + BRAWL_PSI_NUM;
				break;
			case STAFF:			// ���࿡ �̹� ������ ���̿����� �ִ��� �˻�
				iEnd = iStart + STAFF_PSI_NUM;	
				break;
			case EDGED:			// �ִٸ� �̹� ������ ���̿����� ����
				iEnd = iStart + EDGED_PSI_NUM;	
				break;
			case FIREARMS:
				iEnd = iStart + FIREARMS_PSI_NUM;
				break;
			case NOCLASS:
				iEnd = iStart + NOCLASS_PSI_NUM;
				break;
			}

			for(j = iStart;  j < iEnd; j++) 
			{ 
				sSid[sCount] = (short)j; sCount++;
			}
			
		}
	}

	for(i = 0; i < sCount; i++)	// ������ �ִ� ��ϸ� ���� �����Ѵ�.
	{
		bflag = TRUE;
		for(j = 0; j < m_nHavePsiNum; j++) if(m_UserPsi[j].sSid == sSid[i]) { bflag = FALSE; break; }
		if(bflag) arPsi.Add(sSid[i]);
	}
/*
	for(i = 0; i < sCount; i++)	// ������ �ִ� ��ϸ� ���� �����Ѵ�.
	{
		bflag = TRUE;
		for(j = 0; j < m_nHavePsiNum; j++) if(m_UserPsi[j].sSid == sSid[i]) { bflag = FALSE; break; }
		if(bflag) arPsi.Add(sSid[i]);
	}
*/
	sCount = arPsi.GetSize();
	if(sCount == 0) 
	{
		arChangeClass.RemoveAll();
		arPsi.RemoveAll();
		SendNpcSay( NULL, 4 );		// say��ȣ 4�� "���̻� ���� ����"�� �޼�����
		return;
	}

	TempBuf.Add(BUY_PSY_OPEN_RESULT);
	TempBuf.Add((short)iStoreID);
	TempBuf.Add(sCount);

	for(i = 0; i < sCount; i++) TempBuf.Add(arPsi[i]);

	Send(TempBuf, TempBuf.GetLength());

	arChangeClass.RemoveAll();
	arPsi.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////////////
//	���̿��и� �����Ѵ�.
//
void USER::BuyPsi(TCHAR *pBuf)
{
	int i, j;
	int index = 0;
	int iClass = 0;

	BOOL bSame;
	BYTE tError = 1;	
	BYTE tResult = SUCCESS;

	short sStoreID;			// ���� ���̵�
	short sPsiCount;		// �� Psi ��
	short sSid;

	DWORD dwDN = m_dwDN;
	DWORD dwXP = m_dwXP;

	CBufferEx TempBuf;
	CWordArray	arPsi;
	CByteArray arChangeClass;

	arChangeClass.RemoveAll();
	arPsi.RemoveAll();

	CStore* pStore = NULL;

	sStoreID = GetShort(pBuf, index);

	pStore = GetPsiStore(sStoreID);
	if(pStore == NULL) { tResult = FAIL; goto go_result; }

	sPsiCount = GetShort(pBuf, index);

	if(sPsiCount <= 0 || sPsiCount >= g_arPsiTable.GetSize()) return;

	arChangeClass.Add(m_byClass);
	arChangeClass.Add( NOCLASS );			//	������ ������� ���迭 ���̿����� ��� �� �ְ� ���ش�
	for(i = 0; i < _CHANGE_DB; i++)			//	������ ���� ó���� ��� �������� ����� Ŭ������ ������ 		
	{										//	�˾ƾ��Ѵ�.
		if(m_sChangeClass[i] > 0 && i != m_byClass) arChangeClass.Add(i);
	}
											//	ó���� ��� �ش� Ŭ������ ����ִ´�.
	for(i = 0; i < sPsiCount; i++)
	{
		bSame = FALSE;
		sSid = GetShort(pBuf, index);
		/****************************** ���� ó�� *****************************/
		if(sSid < 0 || sSid >= g_arPsiTable.GetSize()) { tResult = FAIL; tError = 05; goto go_result; }

											//	�̹� ������ ���̿����� ����
		for(j = 0; j < m_nHavePsiNum; j++)
		{
			if(m_UserPsi[j].sSid == sSid) { tResult = FAIL; tError = 06; goto go_result; }
		}

		iClass = CheckPsiClass(sSid);
		if(iClass < 0) { tResult = FAIL; goto go_result; }
											//	���� �Ұ����� �迭�� ���̿��е� ����
		for(j = 0;  j < arChangeClass.GetSize(); j++)
		{
			if(arChangeClass[j] == iClass) bSame = TRUE;
		}									//	���� ���̿����� �ڱ� Ŭ������ �´°� ����.
		if(bSame == FALSE) { tResult = FAIL; tError = 01; goto go_result; }
											//	���Է����� ���Ƶ� ����
		if(g_arPsiTable[sSid]->m_tNeedLevel > m_sLevel) { tResult = FAIL; tError = 02; goto go_result; }
											//	XP�� ���ڶ� ����
		if(g_arPsiTable[sSid]->m_iNeedXP > dwXP) { tResult = FAIL; tError = 03; goto go_result; }
											//	DN�� ���ڶ� ����
		if(g_arPsiTable[sSid]->m_iNeedDN > dwDN) { tResult = FAIL; tError = 04; goto go_result; }
											//	�ִ밪�� �Ѿ ����
		if(m_nHavePsiNum >= TOTAL_PSI_NUM) { tResult = FAIL; tError = 05; goto go_result; }
		
		dwDN = dwDN - g_arPsiTable[sSid]->m_iNeedDN;
		dwXP = dwXP - g_arPsiTable[sSid]->m_iNeedXP;
		arPsi.Add(sSid);
	}

	/*************************** User Psi ���****************************/
	for(i = 0; i < arPsi.GetSize(); i++)
	{
		sSid = arPsi[i];
		m_UserPsi[m_nHavePsiNum].sSid = sSid;
		m_UserPsi[m_nHavePsiNum].tOnOff = FALSE;
		m_nHavePsiNum++;
	}

	m_dwDN = dwDN; m_dwXP = dwXP; 
//	m_nHavePsiNum = m_nHavePsiNum + sPsiCount;

	/******************************** DB ���****************************/
//	InterlockedIncrement(&m_lRequireNum);		// DB�� ������ ��ȸ�� �Դ�. 
/*	if(UpdateUserData() == FALSE)
	{
		m_nHavePsiNum = m_nHavePsiNum - sPsiCount;

		for(i = m_nHavePsiNum; i < (m_nHavePsiNum + sPsiCount); i++)
		{
			m_UserPsi[i].sSid = -1;
			m_UserPsi[i].tOnOff = FALSE;
		}
		CheckMaxValue((DWORD &)m_dwDN, (DWORD)dwDN);
		CheckMaxValue((DWORD &)m_dwXP, (DWORD)dwXP);
		tResult = FAIL;
	}
*/
go_result:
	TempBuf.Add(BUY_PSY_RESULT);
	TempBuf.Add(tResult);

	if(tResult == FAIL) 
	{ 
		TempBuf.Add(tError);
		Send(TempBuf, TempBuf.GetLength()); 
		return; 
	}

	sPsiCount = arPsi.GetSize();

	TempBuf.Add(sPsiCount);

	for(i = 0; i < sPsiCount; i++)
	{
		sSid = (short)arPsi[i];
		TempBuf.Add(sSid);
	}

	SendXP();
	SendMoneyChanged();
	Send(TempBuf, TempBuf.GetLength());

	arPsi.RemoveAll();
	arChangeClass.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////////////
//	���� ������ Psi serial id�� �ش��ϴ� Ŭ������ �����Ѵ�. 
//
int USER::CheckPsiClass(short sSid)
{
	if(sSid < BRAWL_PSI_NUM) return BRAWL;
	else if(sSid < BRAWL_PSI_NUM + STAFF_PSI_NUM) return STAFF;
	else if(sSid < BRAWL_PSI_NUM + STAFF_PSI_NUM + EDGED_PSI_NUM) return EDGED;
	else if(sSid < BRAWL_PSI_NUM + STAFF_PSI_NUM + EDGED_PSI_NUM + FIREARMS_PSI_NUM) return FIREARMS;
	else if(sSid < TOTAL_PSI_NUM) return NOCLASS;
	else return -1;
}

////////////////////////////////////////////////////////////////////////////////////
//	�κ��� �� ������ ã�Ƽ� ����, ������ -1 
//
int USER::GetEmptyInvenSlot(int *pEmpty)
{
	int i, j = 0;

	for(i = EQUIP_ITEM_NUM; i < TOTAL_INVEN_MAX; i++)
	{
		if(m_UserItem[i].sSid == -1) 
		{
			pEmpty[j] = i;
			j++;
		}
	}

	return j;
}

////////////////////////////////////////////////////////////////////////////////////
//	���ο� �����۵��� ������...(��: �ʿ��� �������� �ֿﶧ, �����Լ� ������, �������� �춧)   
//
void USER::UpdateInvenSlot(CWordArray *arEmptySlot,CWordArray *arSameSlot, BYTE tType)
{
	int i, j;
	int iEmpty = 0;
	int iSame = 0;
	CBufferEx TempBuf;

	if(arEmptySlot != NULL) iEmpty = arEmptySlot->GetSize();
	if(arSameSlot != NULL) iSame = arSameSlot->GetSize();

	BYTE bySlot = 0;
	BYTE iCount = (BYTE)(iEmpty + iSame); 

	TempBuf.Add(ITEM_MOVE_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(tType);

	TempBuf.Add(iCount);

	for(i = 0; i < iEmpty; i++)
	{
		bySlot = (BYTE)arEmptySlot->GetAt(i);
		TempBuf.Add(bySlot);
		TempBuf.Add(m_UserItem[bySlot].sLevel);
		TempBuf.Add(m_UserItem[bySlot].sSid);
		TempBuf.Add(m_UserItem[bySlot].sDuration);
		TempBuf.Add(m_UserItem[bySlot].sBullNum);
		TempBuf.Add(m_UserItem[bySlot].sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);

		TempBuf.Add(m_UserItem[bySlot].tIQ); 
	}

	for(i = 0; i < iSame; i++)
	{
		bySlot = (BYTE)arSameSlot->GetAt(i);
		TempBuf.Add(bySlot);
		TempBuf.Add(m_UserItem[bySlot].sLevel);
		TempBuf.Add(m_UserItem[bySlot].sSid);
		TempBuf.Add(m_UserItem[bySlot].sDuration);
		TempBuf.Add(m_UserItem[bySlot].sBullNum);
		TempBuf.Add(m_UserItem[bySlot].sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);

		TempBuf.Add(m_UserItem[bySlot].tIQ); 
	}
	
	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////////
//	�������� ������� ��� �뺸
//
void USER::SendMoneyChanged()
{
	CBufferEx TempBuf;

	TempBuf.Add(MONEY_CHANGED);
	TempBuf.Add((BYTE)0x01);
	TempBuf.Add(m_dwDN);

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	�������� �������� �����Ѵ�.
//
void USER::BuyItem(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if( m_iDisplayType == 6) return; //yskang 0.5
#endif

	int rank = m_sCityRank + CITY_RANK_INTERVAL;

	if(rank == CHAONISE_RANK && IsCity()) 
	{
		SendSystemMsg(IDS_USER_CANT_USE_YOUR_CITY_RANK, SYSTEM_NORMAL, TO_ME);
		return;
	}

	CStore* pStore = NULL;
	ItemListArray	arEmptyBuy, arSameBuy;
	CWordArray		arEmptySlot, arSameSlot;
	ItemList		MyItem[TOTAL_ITEM_NUM], SameItem[TOTAL_ITEM_NUM];
	int				pTotalEmpty[INVENTORY_NUM];

	int index = 0;
	int i, j, k; 
	short sid = -1, num = -1; 
	int iSlot, iEmptyNum = 0;
	int iWeight = 0;

	short sStoreID = GetShort(pBuf, index);
	short sCount = GetShort(pBuf, index);

	if(sStoreID < 0 || sCount <= 0 || sCount >= _MAX_SHORT) return;

	pStore = GetStore(sStoreID);
	if(pStore == NULL) return;

	// �̺�Ʈ ��ġ�� �˻� -----------------------------------------//
	CPoint pt = ConvertToClient(m_curx, m_cury);
	if(!g_pEventBlock->CheckUserEvent(m_curz, pt.x, pt.y, BUY_BLOCK)) return;

	if(sCount > 255) return;				// �ѹ��� ����ִ� �ִ뷮�� ������ ���Ѵ�.(MAX_BYTE)

	for(i = 0; i < TOTAL_ITEM_NUM; i++)		// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
		SameItem[i] = m_UserItem[i];
	}

	// ���� �ʱ�ȭ ------------------------------------------------//
	BOOL bFlag = FALSE;
	DWORD dwCost = 0;
	short sRate = pStore->m_sRate;

	DWORD dwTemp = m_dwDN;
//	double TempCost;
	DWORD dwTax = 0, dwBasicTax = 0;

	arSameSlot.RemoveAll();
	arSameBuy.RemoveAll();
	arEmptyBuy.RemoveAll();
	arEmptySlot.RemoveAll();

	ItemList	*pBuyItem = NULL;
	::ZeroMemory(pTotalEmpty, sizeof(pTotalEmpty));

	GetEmptyInvenSlot(pTotalEmpty);

	int iItemSize = g_arItemTable.GetSize();
	// ���Ը�� ---------------------------------------------------//
	for(i = 0; i < sCount; i++)
	{
		iSlot = -1;
		sid = GetShort(pBuf, index);
		num = GetShort(pBuf, index);

		if(iEmptyNum > INVENTORY_NUM) { bFlag = TRUE; goto go_result; }
		if(sid < 0 || num <= 0 || sid > iItemSize || num >= _MAX_SHORT) { bFlag = TRUE; goto go_result; }

		if(!pStore->IsCanBuy(sid)) { bFlag = TRUE; goto go_result; }
//		if(g_arItemTable[sid]->m_byWear <= 5 || g_arItemTable[sid]->m_byWear == 15)	// ���� ������
		if(g_arItemTable[sid]->m_sDuration > 0)	// ���� ������
		{
			for(j = 0; j < num; j++)
			{
				iSlot = pTotalEmpty[iEmptyNum];
				if(iSlot == 0) { bFlag = TRUE; goto go_result; }

				pBuyItem = NULL;
				pBuyItem = new ItemList;
				if(pBuyItem == NULL) { bFlag = TRUE; goto go_result; }

				ReSetItemSlot(pBuyItem);

				pBuyItem->sLevel = g_arItemTable[sid]->m_byRLevel;
				pBuyItem->sSid = sid;
				pBuyItem->sCount = 1;
				pBuyItem->sDuration = g_arItemTable[sid]->m_sDuration;
				pBuyItem->sBullNum = g_arItemTable[sid]->m_sBullNum;
				for(k =0; k< MAGIC_NUM; k++) pBuyItem->tMagic[k] = 0;
				pBuyItem->tIQ = 0;
				pBuyItem->iItemSerial = 0;

				arEmptyBuy.Add(pBuyItem);
				arEmptySlot.Add(iSlot);
				iEmptyNum++; pBuyItem = NULL;

				if(iEmptyNum > INVENTORY_NUM) { bFlag = TRUE; goto go_result; }
			}
		}
		else 
		{
//			if(num > MAX_ITEM_NUM) num = MAX_ITEM_NUM;
			pBuyItem = NULL;
			pBuyItem = new ItemList;
			if(pBuyItem == NULL) { bFlag = TRUE; goto go_result; }

			ReSetItemSlot(pBuyItem);

			pBuyItem->sLevel = g_arItemTable[sid]->m_byRLevel;
			pBuyItem->sSid = sid;
			pBuyItem->sCount = num;
			pBuyItem->sDuration = g_arItemTable[sid]->m_sDuration;
			pBuyItem->sBullNum = g_arItemTable[sid]->m_sBullNum;
			for(k =0; k< MAGIC_NUM; k++) pBuyItem->tMagic[k] = 0;
			pBuyItem->tIQ = 0;
			pBuyItem->iItemSerial = 0;

			iSlot = GetSameItem(*pBuyItem, INVENTORY_SLOT);
			if(iSlot != -1)	
			{ 
/*				if(SameItem[iSlot].sCount + num > MAX_ITEM_NUM)
				{
					num = MAX_ITEM_NUM - SameItem[iSlot].sCount;
					SameItem[iSlot].sCount = MAX_ITEM_NUM;
					if(num < 0) num = 0;
				}
*/				if(num != 0)
				{
					pBuyItem->sCount = num;
					arSameBuy.Add(pBuyItem); 
					arSameSlot.Add(iSlot); 
				}
			}
			else			
			{
				iSlot = pTotalEmpty[iEmptyNum];
				if(iSlot == 0) { bFlag = TRUE; goto go_result; }
				arEmptyBuy.Add(pBuyItem); 
				arEmptySlot.Add(iSlot); 
				iEmptyNum++;
			}			
		}

		iWeight += g_arItemTable[sid]->m_byWeight * num;

		if(pStore->m_arNpcList.GetSize() > 0)
		{
			int iCost = (int)(g_arItemTable[sid]->m_iDN * 0.9);
//			iCost += (int)( iCost * ((double)sRate/100) );

			dwCost += num * iCost;
		}
		else dwCost += num * GetItemCostByCityRank(sid, 0);//sRate);
	}

	// ���Ժ�� ��� �� ������ �˻� -------------------------------//
	dwTax = (DWORD)( dwCost * ((double)sRate/100) );
	dwCost = (DWORD)(dwCost + dwTax + 0.999);			// �ø�(DWORD �� ����� ���ؼ�)
//	TempCost = (double)dwCost;
//	TempCost = TempCost + TempCost * (double)sRate / 100.0;			// �ü�����
//	dwCost = (DWORD)(TempCost + 0.999);			// �ø�(DWORD �� ����� ���ؼ�)

	if(dwCost > dwTemp)		// ���� �����ϸ�
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_DINA1, SYSTEM_ERROR, TO_ME);
		goto go_result;
	}

	if(m_iMaxWeight < m_iCurWeight + iWeight)
	{
		SendSystemMsg( IDS_USER_OVER_WEIGHT1, SYSTEM_ERROR, TO_ME);
		goto go_result;
	}

	// ���Ը���� �κ��� -------------------- --------------------//
	for(i = 0; i < arSameBuy.GetSize(); i++)
	{
		CheckMaxValue((short &)m_UserItem[arSameSlot[i]].sCount, (short)arSameBuy[i]->sCount); 
	}

	for(i = 0; i < arEmptyBuy.GetSize(); i++)
	{
		m_UserItem[arEmptySlot[i]].sLevel = arEmptyBuy[i]->sLevel;
		m_UserItem[arEmptySlot[i]].sSid = arEmptyBuy[i]->sSid;
		m_UserItem[arEmptySlot[i]].sCount = arEmptyBuy[i]->sCount;
		m_UserItem[arEmptySlot[i]].sBullNum = arEmptyBuy[i]->sBullNum;
		m_UserItem[arEmptySlot[i]].sDuration = arEmptyBuy[i]->sDuration;
		m_UserItem[arEmptySlot[i]].tIQ = arEmptyBuy[i]->tIQ;
		for(j =0; j < MAGIC_NUM; j++) m_UserItem[arEmptySlot[i]].tMagic[j] = arEmptyBuy[i]->tMagic[j];
		m_UserItem[arEmptySlot[i]].iItemSerial = arEmptyBuy[i]->iItemSerial;

		MakeItemLog( &(m_UserItem[arEmptySlot[i]]), ITEMLOG_BUY );
	}

	if(dwTemp <= dwCost) m_dwDN = 0;
	else m_dwDN = dwTemp - dwCost;					// ������ ��ȭ

	MakeMoneyLog( dwCost, ITEMLOG_BUY_MONEY );

	if(UpdateUserItemDN() == FALSE)
	{
		m_dwDN = dwTemp;						// ������ ȯ��
		for(i = 0; i < TOTAL_ITEM_NUM; i++)		// ������ ���� ȯ��
		{
			m_UserItem[i] = MyItem[i];
		}
			SendSystemMsg( IDS_USER_BUY_FAIL, SYSTEM_ERROR, TO_ME);
		FlushItemLog( FALSE );
		goto go_result;
	}

	if(dwTax < dwCost && pStore->m_bHaveGuild) 
	{
		pStore->m_iGuildDN += dwTax;	// �ʵ� �����̸� ���â�� ����
		UpdateMemStoreDN(sStoreID, pStore->m_iGuildDN);
	}
	else
	{
		UpdateFortressTax(sStoreID, dwTax); // ��Ʈ���� �����̸� ����...
	}

	m_iCurWeight += iWeight;
	GetRecoverySpeed();

	UpdateInvenSlot(&arEmptySlot, &arSameSlot);
	SendMoneyChanged();
//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
	FlushItemLog( TRUE );

go_result:
	if(bFlag) SendSystemMsg( IDS_USER_FULL_INVEN, SYSTEM_ERROR, TO_ME);

	// �޸� ���� -------------------- --------------------//
	for(i = 0; i < arSameBuy.GetSize(); i++)
	{
		if(arSameBuy[i] != NULL) delete arSameBuy[i];
	}
	arSameBuy.RemoveAll();
	arSameSlot.RemoveAll();
	for(i = 0; i < arEmptyBuy.GetSize(); i++)
	{
		if(arEmptyBuy[i] != NULL) delete arEmptyBuy[i];
	}
	arEmptyBuy.RemoveAll();
	arEmptySlot.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////////////
//	��Ʈ������ �Ҽӵ� �����̸� ������ ����ؼ� ���� 
//
void USER::UpdateFortressTax(int iStoreID, DWORD dwCost)
{
	int i, j;

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i]) continue;

		for(j = 0; j < g_arGuildFortress[i]->arStoreTax.GetSize(); j++)
		{
			if(g_arGuildFortress[i]->arStoreTax[j] == iStoreID)
			{
				g_arGuildFortress[i]->m_iGuildDN += dwCost; 
//				InterlockedExchange(&g_arGuildFortress[i]->m_iGuildDN, (long)dwCost);
				UpdateMemStoreDN(g_arGuildFortress[i]->m_sFortressID, g_arGuildFortress[i]->m_iGuildDN);
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//	������ �� �� �ִ� ������ ��ϰ� ������ �����ش�.
//
void USER::SellItemOpenReq(int sStoreID)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int rank = m_sCityRank + CITY_RANK_INTERVAL;

	if(rank == CHAONISE_RANK && IsCity()) 
	{
		SendSystemMsg(IDS_USER_CANT_USE_YOUR_CITY_RANK, SYSTEM_NORMAL, TO_ME);
		return;
	}

	int i;
	int index = 0;

	int iStart = EQUIP_ITEM_NUM;
	int iEnd = iStart + INVENTORY_NUM;

	CStore* pStore = GetStore(sStoreID);
	if(pStore == NULL) return;

	CBufferEx CostBuf;
	short sSlot = 0;
	DWORD dwSellCost = 0;
	short sCount = 0;
	for(i = iStart; i < iEnd; i++)
	{
		if(pStore->IsCanSell(m_UserItem[i].sSid))
		{
			sSlot = (short)i;
			dwSellCost = GetSellCost(sSlot);

			if(dwSellCost > 0)
			{
				CostBuf.Add(sSlot);
				CostBuf.Add((short)m_UserItem[sSlot].sDuration);
				CostBuf.Add(dwSellCost);
				sCount++;
			}
		}
	}

	CBufferEx TempBuf;
	TempBuf.Add(SELL_ITEM_OPEN_RESULT);
	TempBuf.Add((short)sStoreID);
	TempBuf.Add(sCount);
	if(sCount > 0) TempBuf.AddData(CostBuf, CostBuf.GetLength());

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	������ �� ������ ���Ѵ�.
//
DWORD USER::GetSellCost(short sSlot)
{
	short sSid = m_UserItem[sSlot].sSid;

	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return (DWORD)0;

	DWORD	dwDN = g_arItemTable[sSid]->m_iDN;
	BYTE    tClass = g_arItemTable[sSid]->m_byClass;
	BYTE	tIQ = m_UserItem[sSlot].tIQ;
	int		iMagic = 0;
	int		i = 0;

	// ���� �Ǵ� ���� �������� ���
	if(tIQ == MAGIC_ITEM)
	{
		for(i = 0; i < MAGIC_NUM; i++)
		{
			if(m_UserItem[sSlot].tMagic[i] != 0) iMagic++;
		}
		dwDN = (DWORD)(dwDN * (1.2 + (double)(iMagic * 0.07)));
	}
	if(tIQ == RARE_ITEM)
	{
		for(i = 0; i < MAGIC_NUM; i++)
		{
			if(m_UserItem[sSlot].tMagic[i] != 0) iMagic++;
		}
		dwDN = (DWORD)(dwDN * (1.6 + (double)(iMagic * 0.07)));
	}

	short sMaxDuration = g_arItemTable[sSid]->m_sDuration;
	short sCurDuration = m_UserItem[sSlot].sDuration;

	if(sMaxDuration <= 0) { sMaxDuration = sCurDuration = 1; }

	//----------------------------------------------------------------------------------------------------------------
	//--yskang 0.6 ���� ����� �����̾� - ���ƶ� �ұ�? �߰����� ���� �ش�. 0.15�� 
	double duTempCost =0.0;
	if(m_iDisplayType == 5 || m_iDisplayType == 6)
		duTempCost = dwDN * (0.1 - 0.01 * (( 1 - ((double)sCurDuration / (double)sMaxDuration) ) / 0.1)) + 1;
	else
		duTempCost = dwDN * (0.15 - 0.01 * (( 1 - ((double)sCurDuration / (double)sMaxDuration) ) / 0.1)) + 1;
//	double duTempCost = dwDN * (0.15 - 0.01 * (( 1 - ((double)sCurDuration / (double)sMaxDuration) ) / 0.1)) + 1;
	//----------------------------------------------------------------------------------------------------------------

	if(tClass == FIREARMS && (tIQ - 100) >= 0) duTempCost = duTempCost/2;

	return (DWORD)(duTempCost);
}

//##################################################################################
//	Main UI
//##################################################################################

//////////////////////////////////////////////////////////////////////////////
//	Slot1 �� Slot2�� �������� �ٲ۴�.
//
void USER::SwapItem(int nSlot1, int nSlot2)
{
	ItemList temp;

	temp = m_UserItem[nSlot1];
	m_UserItem[nSlot1] = m_UserItem[nSlot2];
	m_UserItem[nSlot2] = temp;
}

///////////////////////////////////////////////////////////////////////////////
//	���⸦ �ٲ۴�.
//
void USER::ChangeWeaponReq(TCHAR *pBuf)
{
	int j;
	BYTE result = FAIL;
	BYTE tEnable = FALSE;
	BYTE tWeaponClass = 0;

	if(m_UserItem[LEFT_HAND].sSid == -1 && m_UserItem[RIGHT_HAND].sSid == -1) goto send_result;

	SwapItem(RIGHT_HAND, LEFT_HAND);

	if(IsCanUseWeaponSkill(tWeaponClass) == TRUE)
	{
		tEnable = TRUE;
	}
	
	result = SUCCESS;

send_result:

	CBufferEx TempBuf, TempBuf2;
	TempBuf.Add(CHANGE_WEAPON_RESULT);
	TempBuf.Add(result);

	if(result == SUCCESS)
	{
		TempBuf.Add(tEnable);
		
		if(tWeaponClass == 255) tWeaponClass = m_byClass;

		TempBuf.Add(tWeaponClass);

		for(int i = tWeaponClass * SKILL_NUM; i < tWeaponClass * SKILL_NUM + SKILL_NUM; i++)
		{
			TempBuf.Add(m_UserSkill[i].sSid);
			TempBuf.Add(m_UserSkill[i].tLevel);
		}
		Send(TempBuf, TempBuf.GetLength());

		TempBuf2.Add(ITEM_MOVE_RESULT);
		TempBuf2.Add((BYTE)0x01);				// ����
		TempBuf2.Add((BYTE)0x00);				// �Ϲ� ������
		TempBuf2.Add((BYTE)0x02);				// ��ȭ�� ������ �� = 2

		TempBuf2.Add((BYTE)LEFT_HAND);
		TempBuf2.Add(m_UserItem[LEFT_HAND].sLevel);
		TempBuf2.Add(m_UserItem[LEFT_HAND].sSid);
		TempBuf2.Add(m_UserItem[LEFT_HAND].sDuration);
		TempBuf2.Add(m_UserItem[LEFT_HAND].sBullNum);
		TempBuf2.Add(m_UserItem[LEFT_HAND].sCount);
		for(j =0; j < MAGIC_NUM; j++) TempBuf2.Add(m_UserItem[LEFT_HAND].tMagic[j]);
		TempBuf2.Add(m_UserItem[LEFT_HAND].tIQ); 

		TempBuf2.Add((BYTE)RIGHT_HAND);
		TempBuf2.Add(m_UserItem[RIGHT_HAND].sLevel);
		TempBuf2.Add(m_UserItem[RIGHT_HAND].sSid);
		TempBuf2.Add(m_UserItem[RIGHT_HAND].sDuration);
		TempBuf2.Add(m_UserItem[RIGHT_HAND].sBullNum);
		TempBuf2.Add(m_UserItem[RIGHT_HAND].sCount);
		for(j =0; j < MAGIC_NUM; j++) TempBuf2.Add(m_UserItem[RIGHT_HAND].tMagic[j]);
		TempBuf2.Add(m_UserItem[RIGHT_HAND].tIQ); 
		Send(TempBuf2, TempBuf2.GetLength());		// ���� ���� �뺸

		CheckMagicItemMove();			// ���Ⱑ �ٲ�� ���� �Ӽ��� ���� ���� �ٲ�
	}else{
		Send(TempBuf, TempBuf.GetLength());
	}
	

	SendMyWeaponChangeInfo();						// client�� �޸��� ���� ���⸦ �ٲٸ� ������������ ��ǥ�� ���ԵǾ� ���ڸ��� ���ְ� �Ǵ°��� �����ϱ����� ���� ������.
}

///////////////////////////////////////////////////////////////////////////////
//	���� ��ȭ�� �� �ֺ� ������� �˸���.
//
void USER::SendMyWeaponChangeInfo()
{
	int i;
	CBufferEx TempBuf;

	TempBuf.Add(CHANGE_ITEM_INFO);
	TempBuf.Add(m_uid + USER_BAND);
	
	for(i = 0; i < EQUIP_ITEM_NUM; i++) TempBuf.Add(m_UserItem[i].sSid);
	for(i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) TempBuf.Add(m_UserItem[i].sSid);

	if(m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
		TempBuf.Add((BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]));
		TempBuf.Add((BYTE)0x00);
	}else{
		TempBuf.Add(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]);
		TempBuf.Add((BYTE)0xff);
	}

	SendInsight(TempBuf, TempBuf.GetLength());
//	SendExactScreen(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	Skill �� On/Off �Ѵ�
//
void USER::SkillOnOffReq(TCHAR *pBuf)
{
/*	BYTE result = FAIL;
	int index = 0;
	short sSid = GetShort(pBuf, index);

	if(sSid < 0 || sSid >= TOTAL_SKILL_NUM) goto send_result;
	if(m_UserSkill[sSid].tLevel <= 0) goto send_result;

	if(m_UserSkill[sSid].tOnOff == 0) m_UserSkill[sSid].tOnOff = 1;
	else m_UserSkill[sSid].tOnOff = 0;

	result = SUCCESS;

send_result:

	CBufferEx TempBuf;
	TempBuf.Add(SKILL_ONOFF_RESULT);
	TempBuf.Add(result);

	if(result == SUCCESS)
	{
		TempBuf.Add(sSid);
		TempBuf.Add(m_UserSkill[sSid].tLevel);
		TempBuf.Add(m_UserSkill[sSid].tOnOff);
	}

	Send(TempBuf, TempBuf.GetLength());
*/
}

////////////////////////////////////////////////////////////////////////////////
//	Psionic �� �ٲ۴�.
//
void USER::PsiChangeReq(TCHAR *pBuf)
{
	BYTE result = FAIL;
	int index = 0;
	int i;

	BYTE	tWeaponClass = 0;
	int		bSuccessSkill[SKILL_NUM] = {FALSE, FALSE, FALSE, FALSE, FALSE};

	short sNeedPP = 0;

	short sSid = GetShort(pBuf, index);

	if(sSid < 0 || sSid >= TOTAL_PSI_NUM) goto send_result;

//	if(m_UserPsi[sSid].sSid == -1) goto send_result;
	for(i = 0;  i < TOTAL_PSI_NUM; i++) m_UserPsi[i].tOnOff = 0;

	for(i = 0;  i < m_nHavePsiNum; i++)
	{
		if(m_UserPsi[i].sSid == sSid)
		{
			result = SUCCESS;
			m_UserPsi[i].tOnOff = 1;

			IsCanUseWeaponSkill(tWeaponClass);	
			// ���� PP üũ ---------------------------------------------------------//
			sNeedPP = GetNeedPP(bSuccessSkill, tWeaponClass, sSid);
			break;
		}		
	}

send_result:

	CBufferEx TempBuf;
	TempBuf.Add(PSI_CHANGE_RESULT);
	TempBuf.Add(result);

	if(result == SUCCESS)
	{
		TempBuf.Add(sSid);
//		TempBuf.Add((BYTE)sNeedPP);
	}

	Send(TempBuf, TempBuf.GetLength());
}


//////////////////////////////////////////////////////////////////////////////////
//	������ ����� �������� �Ǵ�.
//
void USER::SellItem(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int rank = m_sCityRank + CITY_RANK_INTERVAL;

	if(rank == CHAONISE_RANK && IsCity()) 
	{
		SendSystemMsg( IDS_USER_CANT_USE_YOUR_CITY_RANK, SYSTEM_NORMAL, TO_ME);
		return;
	}

	int i;//, j;
	int index = 0;
	int iWeight = 0;
//	int iSlotNum = 0;
	short sSlot, sNum; 
	DWORD dwSellCost, dwResultCost = 0;
	DWORD dwMaxCost = 0, tempCost = 0;

	DWORD dwMyDN = 0;

	short sStoreID = GetShort(pBuf, index);	
	short sCount = GetShort(pBuf, index);	

	BOOL bError = FALSE;

	CBufferEx	TempBuf;
	ItemList	MyItem[TOTAL_ITEM_NUM];
	CWordArray	arSlot;

	arSlot.RemoveAll();
	CStore* pStore = NULL;

	CPoint pt;

	if(sCount <= 0 || sCount >= _MAX_SHORT) 
	{
		goto go_result;
	}

	pStore = GetStore(sStoreID);
	if(pStore == NULL) 
	{
		SendSystemMsg(IDS_USER_NOT_THIS_STORE_BUYSELL, SYSTEM_ERROR, TO_ME);
		goto go_result;
	}

	// �̺�Ʈ ��ġ�� �˻� -----------------------------------------//
	pt = ConvertToClient(m_curx, m_cury);
	if(!g_pEventBlock->CheckUserEvent(m_curz, pt.x, pt.y, SELL_BLOCK)) return;

	dwMaxCost = m_dwDN;
	dwMyDN = m_dwDN;						// ������ ���
	for(i = 0; i < TOTAL_ITEM_NUM; i++)		// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	for(i = 0; i < sCount; i++)				// ����Ʈ ��Ͽ� ���� 
	{
		sSlot = GetShort(pBuf, index);
		sNum = GetShort(pBuf, index);

		if(sSlot < EQUIP_ITEM_NUM || sSlot >= TOTAL_INVEN_MAX) { bError = TRUE; break;}

		if(m_UserItem[sSlot].sSid < 0 || m_UserItem[sSlot].sSid >= g_arItemTable.GetSize())
		{
			bError = TRUE;
			break;
		}

		if(g_arItemTable[m_UserItem[sSlot].sSid]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND)
		{
			SendSystemMsg(IDS_USER_CANT_SELL_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
			bError = TRUE;
			break;
		}

		if(m_UserItem[sSlot].sCount < sNum || sNum <= 0) { bError = TRUE; break;	}

//		if(g_arItemTable[m_UserItem[sSlot].sSid]->m_byWear < 6 && sNum >= 2)
		if(g_arItemTable[m_UserItem[sSlot].sSid]->m_sDuration > 0 && sNum >= 2)
		{
			bError = TRUE;
			break;
		}
		
		if(pStore->IsCanSell(m_UserItem[sSlot].sSid))
		{
			dwSellCost = GetSellCost(sSlot);
			if(dwSellCost > 0)
			{
				tempCost = dwResultCost + dwSellCost * sNum;

				if(!CheckMaxValueReturn((DWORD &)tempCost, (DWORD)(dwResultCost + dwSellCost * sNum)))
				{  
					bError = TRUE;
					break; 
				}

				iWeight += g_arItemTable[m_UserItem[sSlot].sSid]->m_byWeight * sNum;

				if(m_UserItem[sSlot].sCount ==  sNum)
				{
					MakeItemLog( &(m_UserItem[sSlot]), ITEMLOG_SELL );
					ReSetItemSlot(&m_UserItem[sSlot]);//ReSetItemSlot(sSlot);
				}
				else m_UserItem[sSlot].sCount = m_UserItem[sSlot].sCount - sNum;
				
//				iSlotNum = i;
				arSlot.Add((BYTE)sSlot);
//				sSlotList[i] = sSlot;
 				dwResultCost = tempCost;
			}
			else { bError = TRUE; break; }
		}		
	}

	if(!CheckMaxValueReturn((DWORD &)m_dwDN, (DWORD)dwResultCost)) bError = TRUE;

go_result:
	if(bError)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)  m_UserItem[i] = MyItem[i];
		TempBuf.Add(ITEM_MOVE_RESULT);
		TempBuf.Add((BYTE)0x00);			// ����
		Send(TempBuf, TempBuf.GetLength());
		FlushItemLog( FALSE );
		return;
	}

//	m_dwDN += dwResultCost;					// �������� ����
	CheckMaxValue((DWORD &)m_dwDN, (DWORD)dwResultCost);
	MakeMoneyLog( dwResultCost, ITEMLOG_SELL_MONEY );

											// �����ϸ� ���� ����
	if(UpdateUserItemDN() == FALSE)
	{
		m_dwDN = dwMyDN;
		for(i = 0; i < TOTAL_ITEM_NUM; i++)  m_UserItem[i] = MyItem[i];

		SendSystemMsg( IDS_USER_SELL_FAIL, SYSTEM_ERROR, TO_ME);

		FlushItemLog( FALSE );
		return;
	}

	// ����� ������ �α��Ѵ�
	FlushItemLog( TRUE );

	//-----------------[ Send Packet ]---------------------------------------//
	BYTE tType = 4;

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;
	GetRecoverySpeed();

	UpdateInvenSlot(&arSlot, NULL, tType);
//	SendQuickChange();
	SendMoneyChanged();
//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
}

//////////////////////////////////////////////////////////////////////////////////
//	���������� �����Ҽ� �ִ� ����� ������ ����Ʈ�� �����.
//
void USER::RepairItemOpenReq(int iStoreID)
{
	int rank = m_sCityRank + CITY_RANK_INTERVAL;

	if(rank == CHAONISE_RANK && IsCity()) 
	{
		SendSystemMsg( IDS_USER_CANT_USE_YOUR_CITY_RANK, SYSTEM_NORMAL, TO_ME);
		return;
	}

	int i;
	short sStoreID = iStoreID;

	int iStart = 0;
	int iEnd = EQUIP_ITEM_NUM + INVENTORY_NUM;

	CStore* pStore = GetStore(sStoreID);
	if(pStore == NULL) return;

	CBufferEx CostBuf;
	short sSlot = 0;
	DWORD dwRepairCost = 0;
	short sCount = 0;
	for(i = iStart; i < iEnd; i++)
	{
		if(pStore->IsCanRepair(m_UserItem[i].sSid))
		{
			sSlot = (short)i;
			dwRepairCost = GetRepairCost(sSlot);

			if(dwRepairCost > 0)
			{				
				CostBuf.Add(sSlot);
				CostBuf.Add((short)m_UserItem[sSlot].sDuration);
				CostBuf.Add(dwRepairCost);
				sCount++;
			}
		}
	}

	CBufferEx TempBuf;
	TempBuf.Add(REPAIR_ITEM_OPEN_RESULT);
	TempBuf.Add(sStoreID);
	TempBuf.Add(sCount);
	if(sCount > 0) TempBuf.AddData(CostBuf, CostBuf.GetLength());

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	������ �������� ����� ����Ѵ�.
//
DWORD USER::GetRepairCost(short sSlot)
{
	short sSid = m_UserItem[sSlot].sSid;
 
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 0;
//	if(g_arItemTable[sSid]->m_byWear >= 9) return 0;
	if(g_arItemTable[sSid]->m_sDuration <= 0) return 0;

	DWORD	dwDN = g_arItemTable[sSid]->m_iDN;
	BYTE    tClass = g_arItemTable[sSid]->m_byClass;
	BYTE	tIQ = m_UserItem[sSlot].tIQ;
	int		iMagic = 0;
	int		i = 0;

	if(GetItemFieldNum(sSid) == QUICK_ITEM_TELEPORT)	// �ڵ����Ʈ�� 1������ ���̿� 3000DN��
	{
		int dif = g_arItemTable[sSid]->m_sDuration - m_UserItem[sSlot].sDuration;
		if(dif < 0) return  0;							// �ڵ����Ʈ ������ �ʹ� ��� ����ó���� ��	
		dwDN = dif * 2000;
		return dwDN;
	}

	// ���� �Ǵ� ���� �������� ���
	if(tIQ == MAGIC_ITEM)
	{
		for(i = 0; i < MAGIC_NUM; i++)
		{
			if(m_UserItem[sSlot].tMagic[i] != 0) iMagic++;
		}
		dwDN = (DWORD)(dwDN * (2.2 + (double)(iMagic * 0.1)));
	}
	if(tIQ == RARE_ITEM)
	{
		for(i = 0; i < MAGIC_NUM; i++)
		{
			if(m_UserItem[sSlot].tMagic[i] != 0) iMagic++;
		}
		dwDN = (DWORD)(dwDN * (4.3 + (double)(iMagic * 0.1)));
	}

	short sMaxDuration = g_arItemTable[sSid]->m_sDuration;
	short sCurDuration = m_UserItem[sSlot].sDuration;

	if(sCurDuration > sMaxDuration)
	{
		m_UserItem[sSlot].sDuration = sMaxDuration;	// ����ڵ�
//		BREAKPOINT();
		return 0;
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//--yskang 0.6 ���� ����� �����̾� - ������ �����. ���� ����ڴ� �̰����� �������� ���ڴ�. ^^ ^^
	double duTempCost = 0;
	if(m_iDisplayType == 5 || m_iDisplayType == 6) //�������ڴ� �״��
		duTempCost = (double)( (double)(sMaxDuration - sCurDuration) * ( ((double)dwDN/32767.0) * 0.5 ) );	// ������ 10�� �ø��� (2002.12.10)
	else //���� ����ڴ� �����.
		duTempCost = (double)( (double)(sMaxDuration - sCurDuration) * ( ((double)dwDN/32767.0) * 0.25 ) );	//�������� ���� �ݰ��� ����
	//double duTempCost = (double)( (double)(sMaxDuration - sCurDuration) * ( ((double)dwDN/32767.0) * 0.05 ) );//���� �ڵ�
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	
	if(duTempCost < 0) duTempCost = 0;

	if(tClass == FIREARMS && (tIQ - 100) >= 0) duTempCost = duTempCost * 1.5;

	if((sMaxDuration - sCurDuration) > 0 && duTempCost < 1) return 1;

	return (DWORD)(duTempCost);
}

//////////////////////////////////////////////////////////////////////////////////
//	������ �������� ������.
//
void USER::RepairItem(TCHAR *pBuf)
{
	int index = 0, i, j;
	CStore* pStore = NULL;

	DWORD dwRepairCost = 0;
	DWORD dwTempCost = 0;
	CWordArray	arSlot;

	int	  iSid = 0;
	short sSlot = 0;
	short sStoreID = GetShort(pBuf, index);

	if(sStoreID < 0) return;

	// �̺�Ʈ ��ġ�� �˻� -----------------------------------------//
	CPoint pt = ConvertToClient(m_curx, m_cury);
	if(!g_pEventBlock->CheckUserEvent(m_curz, pt.x, pt.y, REPAIR_BLOCK) && m_curz != 409) return;

	short sCount = GetShort(pBuf, index);

	if(sCount <= 0 || sCount >= TOTAL_ITEM_NUM) return;

	for(i = 0; i < sCount; i++)
	{
		sSlot = GetShort(pBuf, index);
		if(sSlot < 0 || sSlot >= TOTAL_ITEM_NUM) return;

		iSid = m_UserItem[sSlot].sSid;
		if(iSid < 0 || iSid >= g_arItemTable.GetSize()) return;

		if(m_UserItem[sSlot].sCount >= 2) return;
		if(g_arItemTable[iSid]->m_sDuration <= 0) return;

		// ���� ��� ��� �� ������ �˻� -------------------------------//
//		dwRepairCost += GetRepairCost(sSlot);
		dwTempCost = GetRepairCost(sSlot);
		CheckMaxValue((DWORD &)dwRepairCost, (DWORD)dwTempCost); 

		if(dwRepairCost < 1 || m_dwDN < dwRepairCost)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_DINA1, SYSTEM_ERROR, TO_ME);
			return;
		}

		arSlot.Add(sSlot);
	}

	for(i = 0; i < arSlot.GetSize(); i++)
	{
		sSlot = arSlot[i];
		iSid = m_UserItem[sSlot].sSid;
		m_UserItem[sSlot].sDuration = g_arItemTable[iSid]->m_sDuration;
	}

	//-----------------[ Send Packet ]---------------------------------------//
	if(m_dwDN < dwRepairCost) m_dwDN = 0;
	else m_dwDN = m_dwDN - dwRepairCost;

//	BYTE tType = 5;
//	UpdateInvenSlot((int *)sSlot, NULL, 1, 0, tType);
	CBufferEx TempBuf;

	TempBuf.Add(ITEM_MOVE_RESULT);
	TempBuf.Add((BYTE)0x01);				// ����
	TempBuf.Add((BYTE)0x05);				// �Ϲ� ������
	TempBuf.Add((BYTE)sCount);				// ��ȭ�� ������ ��

	for(i = 0;  i < sCount; i++)
	{
		sSlot = arSlot[i];
		TempBuf.Add((BYTE)sSlot);
		TempBuf.Add(m_UserItem[sSlot].sLevel);
		TempBuf.Add(m_UserItem[sSlot].sSid);
		TempBuf.Add(m_UserItem[sSlot].sDuration);
		TempBuf.Add(m_UserItem[sSlot].sBullNum);
		TempBuf.Add(m_UserItem[sSlot].sCount);
		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSlot].tMagic[j]);
		TempBuf.Add(m_UserItem[sSlot].tIQ); 
	}

	Send(TempBuf, TempBuf.GetLength());
	SendMoneyChanged();

	arSlot.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////////
//	�Ƿ��˹���ʱ��  �����ٶ�
//
BOOL USER::CheckAttackDelay(BOOL *bSuccess, BYTE tWeaponClass)
{
	DWORD dwCurrTick = GetTickCount();
	DWORD dwMinDelay = DEFAULT_AT_DELAY;
	short sSid = m_UserItem[RIGHT_HAND].sSid;
	BOOL bHaste = FALSE;
	double speed1=0,speed2=0;

	if(tWeaponClass == 255)
	{
		if(dwCurrTick - m_dwLastAttackTime >= dwMinDelay)
		{
			m_dwLastAttackTime = dwCurrTick;
			return TRUE;
		}
		else return FALSE;
	}
	
	if(tWeaponClass != 255)	// �Ǽ��� �ƴҰ��
	{
		if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return FALSE;
		dwMinDelay = g_arItemTable[sSid]->m_sAttackDelay;
	}

	if(m_dwHasteTime != 0 || m_dwBerserkerTime != 0) ////ȭ�ļ��� 105ħ������
		speed2 = 0.25;	// Haste or Berserker ���̿��� �������

	dwMinDelay = (DWORD)((double)(dwMinDelay - m_DynamicUserData[MAGIC_ATTACK_DELAY_DOWN] - m_DynamicEBodyData[EBODY_ATTACK_DELAY_DOWN]) );
	dwMinDelay =(DWORD)((double)dwMinDelay *(1-speed2+speed1));
	if(dwMinDelay < DEFAULT_AT_DELAY) dwMinDelay = DEFAULT_AT_DELAY;

	if(m_dwLastAttackTime >= dwCurrTick) return FALSE;

	if(dwCurrTick - m_dwLastAttackTime >= dwMinDelay)
	{
		m_dwLastAttackTime = dwCurrTick;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//	�ж����ʹ��ʲô���͵�����
//
BOOL USER::IsCanUseWeaponSkill(BYTE& tWeaponClass)
{
	short sSid = m_UserItem[RIGHT_HAND].sSid;
	int iSize = g_arItemTable.GetSize();

	tWeaponClass = 255;				// �Ǽ��� ���

	if(sSid < 0 || sSid >= iSize) return FALSE;

	BYTE tClass = g_arItemTable[sSid]->m_byClass;
	
	if(tClass == 1) tWeaponClass = FIREARMS;//ǹ
	else if(tClass == 2) tWeaponClass = EDGED;//��
	else if(tClass == 4) tWeaponClass = STAFF;//��
	else if(tClass == 8) tWeaponClass = BRAWL;//ȭ
	else return FALSE;

	//@jjs ������ ��� ��ų�� ���� ó���� ���־�� �Ѵ�.
	if(tWeaponClass == m_byClass) return TRUE;
	if(m_sChangeClass[tWeaponClass] > 0) return TRUE;
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
//	���ݽø��� ��ų���� ���θ� üũ�Ѵ�.       /@@@@@@@@@@(01_07_15)
//
void USER::IsSkillSuccess(BOOL *bSuccess, BYTE tWeaponClass)
{
	int i;
	
	// �Ǽ��̰ų� ����迭 �Ǵ� ������ �迭�� �ƴϸ� ��ų��� �Ұ�
	if(tWeaponClass == 255)
	{
		for(i = 0 ; i < SKILL_NUM; i++) bSuccess[i] = FALSE;
		return;
	}
	else if(tWeaponClass >= _CHANGE_DB) return;

	if(tWeaponClass != m_byClass)
	{
		if(m_sChangeClass[tWeaponClass] <= 0)	
		{
			for(i = 0 ; i < SKILL_NUM; i++) bSuccess[i] = FALSE;
		}
		return;
	}

	int iRandom;	
	int iLevel = 0;	
	int iSuccess = 0;
	int iMagicSkillNum = 0;
	int	iSkillSid = 0;
	int iIndex = tWeaponClass * SKILL_NUM;	//	�迭(0, 1, 2, 3) * 5�� ��ų)	
											
	for(i = iIndex; i < iIndex + SKILL_NUM; i++)// �� ��ų�� ������ �ִ°��� �˾Ƴ���.
	{
		if(m_UserSkill[i].tLevel == 0) continue;

		iSkillSid = m_UserSkill[i].sSid;
		iLevel = m_UserSkill[i].tLevel;

		if(iSkillSid < 0 || iSkillSid >= MAX_SKILL_MAGIC_NUM) continue;
		iMagicSkillNum = g_DynamicSkillInfo[iSkillSid];//װ���ӵļ���...
		if(iMagicSkillNum < 0 || iMagicSkillNum >= MAGIC_COUNT) continue;

		if(iLevel >= 1) iLevel += m_DynamicUserData[iMagicSkillNum] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];

		if(iSkillSid >= g_arSkillTable.GetSize()) return;
		if(iLevel >= g_arSkillTable[iSkillSid]->m_arSuccess.GetSize()) return;
		if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
											
		iSuccess = g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel);// ������(�� Ŭ�������� ��ų���� : 20��)

		if(iSuccess >= 100 && i >= (tWeaponClass * SKILL_NUM)) bSuccess[i - (tWeaponClass * SKILL_NUM)] = TRUE;
		else
		{
			iRandom = XdY(1, 100);
			if(iRandom < iSuccess && i >= (tWeaponClass * SKILL_NUM)) bSuccess[i - (tWeaponClass * SKILL_NUM)] = TRUE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	�ѱ��� ���� �������� �����´�.
//
int USER::GetBreakRate(short sSid)
{
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 0;

	return g_arItemTable[sSid]->m_byErrorRate;
}

////////////////////////////////////////////////////////////////////////////////////////
//	���� ���۵ �ٴ� ������ ���� ���� �����´�.//@@@@@@@@@@@ (01_7_15)
//
int USER::GetBreakDec()
{
	int nBreakDec = 0;
	int iEnd = 4;

	if(m_UserItem[RIGHT_HAND].tIQ == NORMAL_ITEM) return 0;
	if(m_UserItem[RIGHT_HAND].tIQ == UNIQUE_ITEM) iEnd = MAGIC_NUM;

	for(int i = 0; i < iEnd; i++)
	{
		if(m_UserItem[RIGHT_HAND].tMagic[i] == MAGIC_BROKEN_DOWN)
		{
			nBreakDec += m_DynamicUserData[MAGIC_BROKEN_DOWN];
		}
	}

	return nBreakDec;
}


void USER::SendSroreFail(int iType, BYTE err_code)
{

}

////////////////////////////////////////////////////////////////////////////
//	�������� �ִ� �������� ���Ѵ�.
//
int USER::GetMaxDuration(short sSid)
{
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 0;

	return g_arItemTable[sSid]->m_sDuration;
}

////////////////////////////////////////////////////////////////////////////
//	���� ���ɰŸ��� ���Ѵ�.
//
int USER::GetAttackRange(short sSid)
{
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 1;

	return (int)g_arItemTable[sSid]->m_byRange;
}

////////////////////////////////////////////////////////////////////////////
//	�������� ���Ѵ�.(���� ��ų + ���߷� ���� ��ų + ���� ��ų)
//
int USER::GetHitRate(short sSid, BYTE tWeaponClass)
{
	if(tWeaponClass == 255) return 0; // �Ǽ��� ���⿡���� ������ �������� ����.
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 0;

	int iLevel = 0;
	int AttackRate = 0;//1����==2�㹥
	int iSkillSid = 0;
	int tClass = tWeaponClass * SKILL_NUM;

//	AttackRate = g_arItemTable[sSid]->m_byHitRate;

	// ��ų������ ������ ����
	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid = m_UserSkill[i].sSid;

		if(iSkillSid == SKILL_CONCENTRATION || iSkillSid == SKILL_ATTACK_RATING_UP) // ���� ��ų(10), ���߷� ������ų(17)
		{
			// ���� ��ų ���� 			
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;

			// �����ۿ� ���� ��ų ���� ����
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];

			if(iLevel >= g_arSkillTable[iSkillSid]->m_arSuccess.GetSize()) return 0;
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;

			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			AttackRate += g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel);
		}
	}
	if(tWeaponClass==0){
		
	}
	// �����ۿ� ���� ��ü ������ ����
	AttackRate += m_DynamicUserData[MAGIC_ATTACK_RATING_UP] + (int)((double)m_DynamicEBodyData[EBODY_DEX_TO_ATTACK_RATING] / 100 * (double)m_sMagicDEX);
	AttackRate += (int)((double)AttackRate * (double)m_DynamicEBodyData[EBODY_ATTACK_RATING_UP] / 100);

	return AttackRate;
}


////////////////////////////////////////////////////////////////////////////
//	��п� ���ؼ��� ���ݽ� ���׹̳� ���� /@@@@@@@@  01_7_15
//
int USER::DecAttackSP(BOOL *bSuccessSkill, BYTE tWeaponClass)
{
/*	int iLevel = 0;
	int iSkillSid = 0;
	int tClass = tWeaponClass * SKILL_NUM;

	short sSP = GetSP();

	if(sSP <= 0) return -1;

	short sNeedSP = DEFAULT_SP_DEC;
	short sSid = m_UserItem[RIGHT_HAND].sSid;

//	if(sSid >= 0 && sSid < g_arItemTable.GetSize())	sNeedSP = 1 + g_arItemTable[sSid]->m_byNeedSP; //����

	if(tWeaponClass != 255)
	{
		for(int i = tClass; i < tClass + SKILL_NUM; i++)
		{
			iSkillSid = m_UserSkill[i].sSid;

			if(iSkillSid == SKILL_EXCITE) 
			{
				// ���� ��ų ���� 				
				iLevel = m_UserSkill[i].tLevel;		
				if(iLevel < 0) iLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				if(iLevel > g_arSkillTable[iSkillSid]->m_arSuccess.GetSize()) return 0;
				if(iLevel > SKILL_LEVEL) iLevel = SKILL_LEVEL;
				
				sNeedSP += g_arSkillTable[iSkillSid]->m_arRepair.GetAt(iLevel);
			}
		}
		if(sNeedSP < DEFAULT_SP_DEC) sNeedSP = DEFAULT_SP_DEC;
	}

	if(m_sSP >= sNeedSP)
	{
		m_sSP -= sNeedSP;

		SendSP();
		return m_sSP;
	}
*/	
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////
//	ȸ�ǰ��� ��´�. (���� ��ų�� ȸ�� ��ų������ : ���� ��ų���� + ���� ������ ��ų���� ��ŭ�� ��ų���̺��� ������ 
//
int USER::GetAvoid()
{
	BYTE	tWeaponClass = 255;
	int		iLevel = 0;
	int		iSkillSid = 0;
	int		AvoidRate = 0;

	if(IsCanUseWeaponSkill(tWeaponClass) == FALSE)	// ���� �ڽ��� �����տ� �� ������ ��ų�� ����� �� ������ �� �Ǽ��̸�
	{
		//yskang 0.3 ���Ⱑ ������ ȸ������ ����ϱ� ����
		AvoidRate += (m_DynamicUserData[MAGIC_AVOID_RATING_UP] +(int)((double)m_DynamicEBodyData[EBODY_DEX_TO_AVOID_RATING] / 100 * m_sMagicDEX));
		AvoidRate += (int)((double) AvoidRate * (double)m_DynamicEBodyData[EBODY_AVOID_RATING_UP] / 100);
		return AvoidRate;
	}

	int tClass = tWeaponClass * SKILL_NUM; 

	// �ڱ� ���� ��ų��... 
	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid = m_UserSkill[i].sSid;

		if(iSkillSid == SKILL_AVOID_RATING_UP) // ȸ���� ������ų(18)
		{
			// ���� ��ų ���� 			
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;

			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];
			if(iLevel >= g_arSkillTable[iSkillSid]->m_arSuccess.GetSize()) return 0;
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;

			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			AvoidRate += g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel);
		}
	}

	AvoidRate += (m_DynamicUserData[MAGIC_AVOID_RATING_UP] + (int)((double)m_DynamicEBodyData[EBODY_DEX_TO_AVOID_RATING] / 100 * m_sMagicDEX)); //ȸ���� ����.(������ ȸ�ǽ�ų����, �̰� ��ü ȸ������.)
	AvoidRate += (int)((double) AvoidRate * (double)m_DynamicEBodyData[EBODY_AVOID_RATING_UP] / 100);

	return AvoidRate;
}

//////////////////////////////////////////////////////////////////////////////////////
//	ũ��Ƽ�� ���� ���� ���θ� ��´�.
//
double USER::GetCriticalGuard(BOOL *bSuccessSkill, BYTE& tWeaponClass)
{
	short	sSid = m_UserItem[RIGHT_HAND].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return FALSE;
	
	int		iGS = 0;
	int		iRandom = 0;
	int		iLevel = 0;
	int		CriticalGuard = 0;
	int		iSkillSid = 0;
	int		tClass = tWeaponClass * SKILL_NUM;

	double  nCGuard = 0;

	tWeaponClass = 255;

	if(IsCanUseWeaponSkill(tWeaponClass) == FALSE)		// ���� �ڽ��� �����տ� �� ������ ��ų�� ����� �� ������ �� �Ǽ��̸�
	{
		return 0;
	}

	// �ڱ� ���� ��ų��... 
	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid = m_UserSkill[i].sSid;

		if(iSkillSid == SKILL_CRITICAL_GUARD) // ũ��Ƽ�� ����(11)
		{
			// ���� ��ų ���� 			
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;

			// �����ۿ� ���� ��ų ���� ����
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];

			if(iLevel >= g_arSkillTable[iSkillSid]->m_arSuccess.GetSize()) return 0;
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;

			iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
			if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel)) iGS = 1;

			CriticalGuard += g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel);
		}
	}

	nCGuard = (double)(1.0 - (double)(iGS * CriticalGuard/100.0));
	if(nCGuard < 0) nCGuard = 0;

	return nCGuard;
}

//////////////////////////////////////////////////////////////////////////////////////
//	�ʱ� �Ϲ� �������� ��´�.
//
int USER::GetNormalInitDamage(BYTE tWeaponClass, USER* pTarget,BOOL &bIsCritical, BOOL bMax)//yskang 0.3int USER::GetNormalInitDamage(BYTE tWeaponClass, USER* pTarget, BOOL bMax)
{
	int nFinalDamage = 0;		// ����������
	int xyz = 0;				// ���ⵥ����	

	int tClass = tWeaponClass * SKILL_NUM;

	double iBasic = 0;			// �⺻������
	double iExcite = 0;			// ��е�����
	double iFire = 0;			// ���絥����
	int iMinDam = 0;			// �ּҵ�����
	double dAssault = 0;		// �Ͱ�������
	double dMightyWeapon = 0;	// ���Ⱝȭ ���̿���
	double dBerserker = 0;		// ����ũ ���̿���

	int iExciteLevel = 0;		// ��з���
	int iFireLevel = 0;			// ���緹��
	int iMinLevel = 0;			// �ּҵ��������� ����
	int iAssaultLevel = 0;		// �Ͱ�����

	int iRandom = 0;
	int iSkillSid = 0;
	short sNeedSP = 0;

	int iRandomProtect = (int)((double)XdY(1, 1000) / 10 + 0.5);
	int i = 0;

	if(tWeaponClass == FIREARMS) 
	{
		if(m_dwDexUpTime)	iBasic = (int)((double)(m_sMagicDEX + 10 ) / 2 + 0.5+ (int)((double)m_DynamicEBodyData[EBODY_DEX_TO_DAMAGE] / 100 *(m_sMagicDEX+10)));	
		else				iBasic = (int)((double)(m_sMagicDEX)/2 + 0.5 + (int)((double)m_DynamicEBodyData[EBODY_DEX_TO_DAMAGE] / 100 * m_sMagicDEX) );	
	}
	else iBasic = (int)((double)(m_sMagicSTR  )/2 + 0.5+ (int)((double)m_DynamicEBodyData[EBODY_STR_TO_DAMAGE] / 100 * m_sMagicSTR));

	if(iBasic < 0) iBasic = 0;

	if(tWeaponClass == 255) return (int)iBasic;

	xyz = GetWeaponDamage(m_UserItem[RIGHT_HAND].sSid, bMax);

	// ���, ����, �ּҵ�������
	for(i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid = m_UserSkill[i].sSid;

		if(iSkillSid == SKILL_EXCITE)			//ȭ���˷ܼ���
		{			
			iExciteLevel = m_UserSkill[i].tLevel;		
			if(iExciteLevel < 0) iExciteLevel = 0;

			// �����ۿ� ���� ��ų ���� ����
			if(iExciteLevel >= 1) iExciteLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];

			if(iExciteLevel >= SKILL_LEVEL) iExciteLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) continue;

			sNeedSP += g_arSkillTable[iSkillSid]->m_arRepair.GetAt(iExciteLevel);
			if(sNeedSP <= GetSP())
			{
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if((iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iExciteLevel))|| bMax)
				{
					if(m_sSP >= sNeedSP)
					{
						m_sSP -= sNeedSP;
						SendSP();
					}
					iExcite = ((iBasic + xyz) * (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iExciteLevel)/100.0);
				}
			}
		}

		if(iSkillSid == SKILL_RE_FIRE_UP)			// 16 index
		{
			iFireLevel = m_UserSkill[i].tLevel;		
			if(iFireLevel < 0) iFireLevel = 0;

			// �����ۿ� ���� ��ų ���� ����
			if(iFireLevel >= 1) iFireLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];

			if(iFireLevel >= SKILL_LEVEL) iFireLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) continue;

			iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
			//if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iFireLevel) + 100)
			if((iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iFireLevel))|| bMax)
			{
				iFire = ((iBasic + xyz) * (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iFireLevel)/100.0);
			}
		}
		//---------------------------------------------------
		//yskang 0.3 ����̳� ���缺���̸� ũ��Ƽ�� üũ
		if(iExcite + iFire > 0)
		{
			bIsCritical = TRUE;
		}
		//-----------------------------------------------------

		if(iSkillSid == SKILL_MIN_DAMAGE_UP)				// 15 index
		{
			iMinLevel = m_UserSkill[i].tLevel;		
			if(iMinLevel < 0) iMinLevel = 0;

			// �����ۿ� ���� ��ų ���� ����
			if(iMinLevel >= 1) iMinLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];

			if(iMinLevel >= SKILL_LEVEL) iMinLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) continue;

			iMinDam = g_arSkillTable[iSkillSid]->m_arInc.GetAt(iMinLevel);
		}

		if(iSkillSid == SKILL_ASSAULT)
		{
			iAssaultLevel = m_UserSkill[i].tLevel;
			if(iAssaultLevel < 0) iAssaultLevel = 0;

			// �����ۿ� ���� ��ų ���� ����
			if(iAssaultLevel >= 1) iAssaultLevel += m_DynamicUserData[MAGIC_ALL_SKILL_UP];	// 100���̻� ��ų�� ��罺ų �������� ������ �޴´�

			if(iAssaultLevel >= SKILL_LEVEL) iAssaultLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) continue;

			iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
			if((iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iAssaultLevel))|| bMax)
			{
				dAssault = ((iBasic + xyz) * (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iAssaultLevel)/100.0);
			}
		}
	}

	if( m_dwMightyWeaponTime != 0 )
	{
		dMightyWeapon = (double)( (double)(iBasic+xyz) * 0.1 );
	}
	if( m_dwBerserkerTime != 0 )
	{
		dBerserker = (double)( (double)(iBasic+xyz) *0.15 );
	}
	//�Ǳ��10%����
	if( m_iSkin==1 ){
		dBerserker =dBerserker + (double)( (double)(iBasic+xyz) *0.10 );
	}
	
	if(pTarget != NULL && pTarget->m_state == STATE_GAMESTARTED)
	{
		if(iRandomProtect <= RANDOM_PROTECT && pTarget->m_bEarringOfProtect)	// ����Ʈ �Ͱ���
		{
			if(iExcite + iFire > 0)		// ��� �Ǵ� ���簡 ��������
			{
				nFinalDamage = (int)((iBasic + xyz) + iMinDam + dAssault + dMightyWeapon + dBerserker);
				pTarget->SendAccessoriDuration(SID_EARRING_OF_PROTECT);
			}
		}
		else 
		{
			nFinalDamage = (int)((iBasic + xyz) + iExcite + iFire + iMinDam + dAssault + dMightyWeapon + dBerserker);
		}
	}
	else
	{
		nFinalDamage = (int)((iBasic + xyz) + iExcite + iFire + iMinDam + dAssault + dMightyWeapon + dBerserker);
	}

	if(m_sLevel >= ADD_USER_LEVEL) nFinalDamage += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sDamage;

	return nFinalDamage;
}

///////////////////////////////////////////////////////////////////////////////////////
//	�ʱ� ũ��Ƽ�� �������� ��´�.
//
int USER::GetCriticalInitDamage(BOOL *bSuccessSkill, BYTE tWeaponClass)
{
	int nDamage = 0;
	int xyz = 0;

	if(tWeaponClass == 255) return (int)(m_sMagicSTR / 4);

	xyz = GetWeaponDamage(m_UserItem[RIGHT_HAND].sSid);

	switch(tWeaponClass)
	{
	case BRAWL:		// ����
		if(bSuccessSkill[4] == TRUE)	// ��н�ų����
		{
			nDamage = (int)((1 + 0.01 * (50 + 2.5 * (double)m_UserSkill[tWeaponClass * SKILL_NUM + 4].tLevel)) * (xyz + (double)m_sMagicSTR/4));
		}
		else
		{
			nDamage = (int)(xyz + (double)m_sMagicSTR/4);
		}
		break;

	case EDGED:		// ����
	case STAFF:		// ������
		nDamage = (int)(xyz + (double)m_sMagicSTR / 4);
		break;

	case FIREARMS:	// �ѱ�
		if(bSuccessSkill[3] == TRUE)	// ���� ��ų����
		{
			nDamage = (int)(1.4 * ((double)xyz + m_sMagicDEX / 5));
		}
		else
		{
			nDamage = (int)(2 * (xyz + (double)m_sMagicDEX/5));
		}
		break;
	}

	return nDamage;
}

///////////////////////////////////////////////////////////////////////////////////////
//	���� ��ü�� �������� ����Ѵ�.(������ ���⸸ ���)
//
int USER::GetWeaponDamage(short sSid, BOOL bMax)//(sid, �ִ밪 = �ɼ�)
{
	int iDamage = 0;
	int iTemp = 0;

	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 0;

	int x = g_arItemTable[sSid]->m_byAX;
	int y = g_arItemTable[sSid]->m_byAY;
	int z = g_arItemTable[sSid]->m_byAZ;

	if(bMax == TRUE) iDamage = x * y + z;
	else	         iDamage = XdY(x, y) + z;

	iDamage += (int)((double)iDamage * (double)m_DynamicEBodyData[EBODY_WEAPON_BASE_UP] / 100);

	if(m_UserItem[RIGHT_HAND].tIQ != UNIQUE_ITEM)	// ���� �������� ����
	{
		int up_count = 0;
		up_count = m_UserItem[RIGHT_HAND].tMagic[ITEM_UPGRADE_COUNT];
		if(up_count > 0 && up_count <= MAX_ITEM_UPGRADE_COUNT)
		{
			if(g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_byClass != STAFF_ITEM)
			{
				iTemp += (up_count * ATTACK_UPGRADE_BAND);
			}
		}
	}

	iDamage += m_DynamicUserData[MAGIC_DAMAGE_UP];
	iDamage += iTemp;						  // ���� ���Ⱑ ������ �������� �׳� �߰��ȴ�.

	return iDamage;
}

///////////////////////////////////////////////////////////////////////////////////////
//	ũ��Ƽ�� �������θ� ��´�.	
//
double USER::GetCriticalHit(BYTE tWeaponClass ,int max)
{
	int		iVC = 0;
	int		iRandom = 0;
	int		iLevel = 0;
	int		iSkillSid = 0;
	int		vitalCritical = 0;
	int		vitalHit = 0;
	double  nVCritical = 1.00;

	if(tWeaponClass == 255) return nVCritical;

	int tClass = tWeaponClass * SKILL_NUM;

	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid = m_UserSkill[i].sSid;

		if(iSkillSid == SKILL_VITAL_CRITICAL) // ����Ż ũ��Ƽ��(12)
		{
			iLevel = m_UserSkill[i].tLevel;		// ���� ��ų ����
			if(iLevel < 0) iLevel = 0;

			// �����ۿ� ���� ��ų ���� ����
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;

			iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
			if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel)|| max==1)
			{
				nVCritical = 1.00 - (double)(g_arSkillTable[iSkillSid]->m_arInc.GetAt(iLevel)/100.0);
			}
		}
	}

	//if(nVCritical < 0) nVCritical = 0.0;
	if(nVCritical > 1) nVCritical = 1.0;
	TRACE("�̴�= %f\n",nVCritical);
	return nVCritical;
}

//////////////////////////////////////////////////////////////////////////
//	���� ���� �����´�.
//
short USER::GetItemDefense(short sSid, int slot)
{
	short sDefense = 0;

	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 0;
	if(slot < 0 || slot > EQUIP_ITEM_NUM) return 0;
																	
	if(m_UserItem[slot].tIQ != UNIQUE_ITEM)	// ���� �������� ����
	{
		int up_count = 0;
		up_count = m_UserItem[slot].tMagic[ITEM_UPGRADE_COUNT];
		if(up_count > 0 && up_count <= MAX_ITEM_UPGRADE_COUNT)
		{
			sDefense += (up_count * DEFENSE_UPGRADE_BAND);
		}
	}

	return sDefense += g_arItemTable[sSid]->m_sDefense;
//	return g_arItemTable[sSid]->m_sDefense;
}

//////////////////////////////////////////////////////////////////////////
//	�ѱ��� ������ ���Ұ��� �����´�.
//
short USER::GetDecDuration(short sSid)
{
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return 0;

	return g_arItemTable[sSid]->m_sBullNum;
}

//////////////////////////////////////////////////////////////////////////
//	�������� ������ ���氪�� ������.
//
void USER::SendDuration(BYTE tSlot, int iDamage)
{
	if(tSlot < 0 || tSlot >= TOTAL_ITEM_NUM) return;
	
	if(m_UserItem[tSlot].sSid < 0 || m_UserItem[tSlot].sSid >= g_arItemTable.GetSize()) return;
	
	long lMax = (long)g_arItemTable[m_UserItem[tSlot].sSid]->m_sDuration;

	if(lMax <= 0) return;

	long lCurPer = (long)((m_UserItem[tSlot].sDuration * 100.0)/lMax);

	m_UserItem[tSlot].sDuration -= iDamage;
	if(m_UserItem[tSlot].sDuration < 0) m_UserItem[tSlot].sDuration = 0;

	long lNextPer = (long)((m_UserItem[tSlot].sDuration * 100.0)/lMax);

	if(lNextPer > 20) 
	{
		// ������ 5%����..
		int iCur = (int)(lCurPer/5.0);
		int iNext = (int)(lNextPer/5.0);

		if(iCur == iNext) return;
	}	

	CBufferEx TempBuf;

	TempBuf.Add(ITEM_DURATION);
	TempBuf.Add(tSlot);
	TempBuf.Add(m_UserItem[tSlot].sDuration);

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////
//	�������� ��ź��(�嵥������ ������.)
//
void USER::SendBullNum(BYTE tSlot)
{
	if(tSlot < 0 || tSlot >= TOTAL_ITEM_NUM) return;

	CBufferEx TempBuf;

	TempBuf.Add(ITEM_BULLNUM);
	TempBuf.Add(tSlot);
	TempBuf.Add(m_UserItem[tSlot].sBullNum);

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////////
//	���� HP�� ��´�.
//
short USER::GetHP()
{
	BYTE	tWeaponClass = 255;

	int iSkillSid = 0;
	int iLevel = 0, iRandom = 0, iHP = 0;

	double dPlusHP = 1.0;
	int iAddHP = 0;

	DWORD dwDiffTime = GetTickCount() - m_dwLastHPTime;

	if(dwDiffTime >= m_iHPIntervalTime)
	{
		if(IsCanUseWeaponSkill(tWeaponClass))	// ���� �ڽ��� �����տ� �� ������ ��ų�� ������
		{
			int tClass = tWeaponClass * SKILL_NUM; 

			for(int i = tClass; i < tClass + SKILL_NUM; i++)			// ��ų�� HPȸ�� ��ų�� ������ �ݿ��Ѵ�.
			{
				iSkillSid = m_UserSkill[i].sSid;

				if(iSkillSid == SKILL_HP_RECOVER_UP)					// 3 index
				{
					iLevel = m_UserSkill[i].tLevel;		
					if(iLevel < 0) iLevel = 0;

					// �����ۿ� ���� ��ų ���� ����
					if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]] + m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
					if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
					if(iSkillSid >= g_arSkillTable.GetSize()) continue;

					iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
					if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel)) iHP = 1;

					dPlusHP = 1.0 + (double)iHP * g_arSkillTable[iSkillSid]->m_arRepair.GetAt(iLevel) / 100.0;
				}
			}
		}

		if(m_iHPRate <= 0)	// ������
		{
			m_iHPRate = 1;
			TRACE("ȸ������ ����\n");
		}
		
		// ���ø� ȸ���� �ΰ����� ���ʽ��� ��������	
		if(IsCity() && CheckRecoverTableByClass()) iAddHP = g_arRecoverTable[m_byClass]->m_byTown;

		m_sHP += (int)(dPlusHP * (double)m_sMagicCON/m_iHPRate ) + iAddHP;
		if(m_sHP > m_sMagicMaxHP) m_sHP = m_sMagicMaxHP;

		m_dwLastHPTime = GetTickCount();
	}

	return m_sHP;
}


////////////////////////////////////////////////////////////////////////////////////
//	HP �� Ŭ���̾�Ʈ�� ������.
//
void USER::SendHP()
{
	CBufferEx TempBuf;

	if(m_bLive == USER_DEAD) return;

	TempBuf.Add(SET_HP);
	TempBuf.Add(m_sHP);

	Send(TempBuf, TempBuf.GetLength());
	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
}

/////////////////////////////////////////////////////////////////////////////////////
//	NPC�� Ŭ������ ��´�.
//
BYTE USER::GetNpcClass(CNpc *pNpc)
{
	BYTE tClass = BRAWL;

	if(pNpc == NULL) return 0;

	switch(pNpc->m_byClass)
	{
	case 1:
		tClass = FIREARMS;
		break;
	case 2:
		tClass = EDGED;
		break;
	case 4:
		tClass = STAFF;
		break;
	case 8:
		tClass = BRAWL;
		break;
	}

	return tClass;
}

//////////////////////////////////////////////////////////////////////////////////////
//	Ư�� ���̿����� ������ �����ߴ��� �˻��Ѵ�.
//
BOOL USER::IsHavePsionic(BYTE tSid)
{
	if(tSid < 0 || tSid >= TOTAL_PSI_NUM) return FALSE;

	for(int i = 0; i < TOTAL_PSI_NUM; i++)
	{
		if(m_UserPsi[i].sSid == -1) break;
		if(m_UserPsi[i].sSid == (short)tSid) return TRUE;
//		if(m_UserPsi[i].sSid == (short)tSid && m_UserPsi[i].tOnOff == 1) return TRUE;
	}
	if((tSid==0x17 ||tSid==0x18 ||tSid==0x19 ||tSid==0x1a) &&m_UserItem[34].sSid!=-1&&m_UserItem[35].sSid!=-1&&m_UserItem[36].sSid==987&&m_UserItem[37].sSid!=-1)
		return true;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
//	���̿��� ���ݽ� PP üũ �� ����ó��
//
int USER::DecAttackPP(BOOL *bSuccessSkill, BYTE tWeaponClass, short sSid, BOOL bDec)
{
	short sOldPP = GetPP();
	short sNeedPP = 0;
	short b3 = bSuccessSkill[3];	// ���̿��� ������ų ��������

	if(sNeedPP < 0) return -1;

	if(tWeaponClass == STAFF)
	{
		sNeedPP = (short)((double)sNeedPP * ((double)((100 - (2 * b3 * m_UserSkill[tWeaponClass * SKILL_NUM + 3].tLevel + 20))) / 100));
	}
	
	if(sNeedPP < 0) return -1;
	return sNeedPP;

	if(m_sPP >= sNeedPP)
	{
		if(bDec == FALSE) return 1;

		m_sPP -= sNeedPP;
		if(m_sPP < 0) m_sPP = 0;

		if(sOldPP != m_sPP) SendPP();

		return m_sPP;
	}

	if(sOldPP != m_sPP) SendPP();
	
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
//	���̿��� �����ÿ� �ʿ��� PP���� ���Ѵ�.
//
short USER::GetNeedPP(BOOL *bSuccessSkill, BYTE tWeaponClass, short sSid)
{
	if(sSid < 0 || sSid >= TOTAL_PSI_NUM) return -1;
	if(sSid >= g_arPsiTable.GetSize()) return -1;

	short sNeedPP = g_arPsiTable[sSid]->m_tNeedPP;
//	if(sSid==13 || sSid==14 || sSid==15)  sNeedPP=2000;
	int tClass = tWeaponClass * SKILL_NUM;
	int iLevel = 0;
	int iSkillSid = 0;
	// PP �Ҹ� ���� ��ų�� ���ؼ� ���ҵ� �� ���ϱ�
	int iStaff = 0;
	if(tWeaponClass == STAFF) iStaff = 1;	// �����̸� ���� �ֳ�?

	short sDecPP = 0;
	
	if(iStaff == 1)
	{
		for(int i = tClass; i < tClass + SKILL_NUM; i++)
		{
			iSkillSid  = m_UserSkill[i].sSid;	// ? ��ų�� ����
			if(iSkillSid == SKILL_PP_DOWN) // PP �Ҹ� ����		6 index
			{
				// ���� ��ų ���� 
				iLevel = m_UserSkill[i].tLevel;		
				if(iLevel < 0) iLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
				if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
				if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
				sDecPP += g_arSkillTable[iSkillSid]->m_arRepair.GetAt(iLevel);
			}
		}
	}

	sNeedPP = (short)((double)sNeedPP * (double)(1 - ((double)(sDecPP) / 100)) + 0.5);

	sSid = -1;
	sSid = m_UserItem[RIGHT_HAND].sSid;
	if(sSid >= 0 && sSid < g_arItemTable.GetSize())
	{
		sDecPP = 0;
		sDecPP = g_arItemTable[sSid]->m_byMPP;		// �����ۿ� ���� PP�Ҹ��� ���ҽ�Ų��.
		sNeedPP -= sDecPP;
	}

	if(sNeedPP <= 0) sNeedPP = 2;


	return sNeedPP;
}

////////////////////////////////////////////////////////////////////////////////
//	���̿��� ���� �Ÿ��� ���Ѵ�.
//
int USER::GetPsiRange(short sSid)
{
	if(sSid < 0 || sSid >= TOTAL_PSI_NUM) return -1;
	if(sSid >= g_arPsiTable.GetSize()) return -1;

	int nRange = g_arPsiTable[sSid]->m_tRange;

	return nRange;
}

/////////////////////////////////////////////////////////////////////////////
//	���̿��� ĳ���� �����̸� üũ�Ѵ�.
//
BOOL USER::CheckPsiCastingDelay()
{
	DWORD dwCurr = GetTickCount();
	DWORD dwDiff = dwCurr - m_dwLastPsiAttack;

	if(dwDiff < m_dwCastDelay) return FALSE;
	else 
	{
		m_dwCastDelay = 0;
		return TRUE;
	}
}

////////////////////////////////////////////////////////////////////////////////
//	���̿��� ���� ���θ� �����Ѵ�.
//
//BOOL USER::IsSuccessPsi(BOOL *bSuccessSkill, BYTE tWeaponClass, short sSid)
//{
//	if(sSid < 0 || sSid >= TOTAL_PSI_NUM) return FALSE;
//
//	int iRandom = XdY(1, 100);
//
//	int iStaff = 0;
//	if(tWeaponClass == STAFF) iStaff = 1;
//
//	int iRegi		= g_arPsiTable[sSid]->m_tRegi;			// ���� ���� ����
//	DWORD dwCast	= g_arPsiTable[sSid]->m_sDelayTime;		// ĳ���� ������
//
//	int iLevel = 0;
//	if(iStaff == 1 && bSuccessSkill[2] == TRUE) iLevel = m_UserSkill[tWeaponClass * SKILL_NUM + 2].tLevel; // ���߽�ų ����
//
//	int iComp = (int)((double)m_sVOL * 0.7 + bSuccessSkill[2] * (2 * iLevel + 20) + 8 - iRegi * ((double)m_sMagicVOL / 5));
//
//	if(iRandom <= iComp) 
//	{
//		if(bSuccessSkill[4] == TRUE && iStaff == 1)			// �ż� ���� ��ų
//			m_dwCastDelay = dwCast / 2;
//		else
//			m_dwCastDelay = dwCast;
//
//		m_dwLastPsiAttack = GetTickCount();
//		return TRUE;
//	}
//
//	return FALSE;
//}

////////////////////////////////////////////////////////////////////////////////////
//	ħ���˺�����....
//
BOOL USER::PsionicProcess(int nTarget, short sSid, CPoint pt, short sNeedPP)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;

	USER* pUser = NULL;
	CNpc* pNpc = NULL;

	BOOL bIsUser = FALSE;

	// skill
	double dAttack = 0;			// ���̿��� ������ ����
	double dAssault = 0;		// �Ͱ�
	double dABDefense = 0;		// ���� ���
	double dPsyResist = 0;		// ���̿��� ����
	double dnewAssault = 0;

	// ���̿���
	double dMightyWeapon = 1;	// Mighty Weapon

	int    iAssaultLevel = 0;
    int    iRandom = 0;

	// Item
	double dItemResist = 0;	// �������� ���� + ���̿��� ���� ���׷�
	int	iPsiAttackUp = 0;	// �����̿� �޷��ִ� �絩
	int iUpCount = 0;		// ���׷��̵� ī��Ʈ

	double dDamage = 0;			// �⺻ ������
	double dDefense = 0;		// ����
	double dWideRangeAttackDamage = 0;
	int iExp = 0;

	DWORD dwExp = 0;

	BYTE tWeaponClass = 0;
	CPoint ptTel;

	CString strMsg = _T("");

	TCHAR szTempBuf[1024];	::ZeroMemory(szTempBuf, sizeof(szTempBuf));
	int index =0;
	int iRepeat = 0;
	int iEvadeRange = 14;
	int iEvadeRandom = 1;

	int iTime = 1;

	DWORD dwCurrTime = GetTickCount();

	MAP* pMap = g_zone[m_ZoneIndex];
	if(!pMap) return FALSE;

	if(nTarget >= USER_BAND && nTarget < NPC_BAND)	// USER
	{
		pUser = GetUser(nTarget - USER_BAND);									// User Pointer �� ��´�.
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED)return FALSE;	// �߸��� USER �̸� ����
		if(pUser->m_bLive == USER_DEAD)	return FALSE;							// Target User �� �̹� �׾������� ����

		bIsUser = TRUE;
	}
	else if(nTarget >= NPC_BAND)				// NPC
	{
		pNpc = GetNpc(nTarget - NPC_BAND);				// NPC Point �� ��´�.
		if(pNpc == NULL) return FALSE;					// �߸��� NPC �̸� ����
		if(pNpc->m_NpcState == NPC_DEAD) return FALSE;	// NPC �� �̹� �׾� ������ ����
		if(pNpc->m_sHP <= 0) return FALSE;
		if(pNpc->m_tGuildWar == GUILD_WAR_AFFTER)
		{
			if(pNpc->m_tNpcType == NPCTYPE_GUILD_NPC || pNpc->m_tNpcType == NPCTYPE_GUILD_DOOR) return FALSE;
		}
		if(pNpc->m_tNpcType != NPCTYPE_MONSTER)
		{
			if(pNpc->m_tGuildWar != GUILD_WARRING) return FALSE;	
		}
	}

	if(sSid < 0 || sSid >= TOTAL_PSI_NUM) return FALSE;

	short sHoldTime	= g_arPsiTable[sSid]->m_sHoldTime;
	short sBasic	= g_arPsiTable[sSid]->m_sBasic;
	short sLevelUp	= g_arPsiTable[sSid]->m_sLevelUp;

//	if(m_sLevel >= ADD_USER_LEVEL) 
//		sBasic += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sDamage;
	//sBasic += (int)((double)sBasic * (double)m_DynamicEBodyData[EBODY_PSI_ATTACK_UP] / 100);
	
	// �絩���� ----------------------------------------------------------------------//
	iPsiAttackUp = iUpCount = 0;		
	if(m_UserItem[RIGHT_HAND].tIQ != UNIQUE_ITEM)	// ���� �������� ����
	{
		if(m_UserItem[RIGHT_HAND].sSid >= 0 && m_UserItem[RIGHT_HAND].sSid < g_arItemTable.GetSize())
		{
			if(g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_byClass == STAFF_ITEM)
			{
				iUpCount = m_UserItem[RIGHT_HAND].tMagic[ITEM_UPGRADE_COUNT];
				if(iUpCount > 0 && iUpCount <= MAX_ITEM_UPGRADE_COUNT)
				{
					iPsiAttackUp = (iUpCount * ATTACK_UPGRADE_PSI_BAND);
				}
			}
		}
	}
	iPsiAttackUp +=sBasic + m_DynamicUserData[MAGIC_PSI_RESIST_UP] + (int)((double)m_DynamicEBodyData[EBODY_VOL_TO_PSI_DAMAGE] / 100 * (double)m_sMagicVOL);
	iPsiAttackUp +=(int)(iPsiAttackUp *(double)m_DynamicEBodyData[EBODY_PSI_ATTACK_UP] / 100);
 //daishufeng
	iPsiAttackUp = (int)(iPsiAttackUp*0.6);
	switch(sSid)	// ���� ���̿��� ó��
	{
	case 23://����ְҵ��ɱ.
	case 25:
	case 26: //��ɱ�˺�
		EBODY_Skill(sSid,pUser,pNpc,bIsUser);
		break;
	case 2: // ���
	case 4:	// ����
	case 8: // ���� 8��������
	case 5:	// ����
	case 10:// ��֮��
	case 24://��ʦ��ɱ.
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return FALSE; //yskang 0.5
	if(m_iDisplayType == 6) return FALSE; //yskang 0.5
#endif
	double EBODYSkill=1;
	
		dAttack		= GetPsiAttackUp();		// ȡ�÷�ʦ������..
		dDamage  = (double)( 0.5 + m_sLevel * ((double)sLevelUp / 100)+m_sMagicVOL +iPsiAttackUp);	// �⺻ ������
		if(sSid!=24)
			dAssault	= GetPsyAssault(FALSE);//��ͨħ������
		else{
			dAssault	= GetPsyAssault(true);//��ɱħ�� ����.����10%�˺�
			EBODYSkill=1.1;
		
		}
		dDamage = (dDamage + (double)( dDamage * (double)( dAttack * 0.01 ) )* (double)( dAssault * 0.01 ))*EBODYSkill;
		dDamage=dDamage+dDamage*(myrand(1,100)%10)/100;
		//�����˺����Ӽ���
		dDamage=dDamage+m_DynamicMagicItem[7];

		if( m_dwMightyWeaponTime != 0 )	dMightyWeapon = (double)( (double)(dDamage) * 0.1 );
		dWideRangeAttackDamage = dDamage;
		TRACE( "ţţDEGBUGħ������ %lf\n", dDamage);
	
		if(bIsUser)
		{
			if( pUser->m_strUserID == m_strUserID ) return FALSE;
			if( dwCurrTime - pUser->m_dwLastPsiDamagedTime < PSI_DAMAGE_DELAY) return FALSE;

			dABDefense	= pUser->GetPsyAbsoluteDefense();	// ������ ��ų�� ���� ���̿��� ���� ����
			dItemResist = 50 + pUser->GetUserSpellDefence();
			// ���̿��� ���׷�
			if(pUser->m_dwPsiShieldTime != 0 || pUser->m_dwBigShieldTime !=0)				// ���� ���� ���� ��
			{
				dItemResist = dItemResist * 1.5;
			}
			dItemResist += dItemResist * (double)( dABDefense * 0.01 ) ;//���ܵ�ħ��
			dDefense = (double)( (double)(m_sMagicVOL)/(double)( m_sMagicVOL + pUser->m_sMagicVOL + dItemResist/2) );
			if(dDefense>1)
				dDefense=1.00;
			if( m_dwMightyWeaponTime != 0 )	dMightyWeapon = (double)( (double)(dDamage) * 0.1 );
			dDamage = dDamage * dDefense + iPsiAttackUp - dItemResist/2;
			dDamage=dDamage-(pUser->m_DynamicMagicItem[5]+pUser->m_DynamicMagicItem[4]);
			if(dDamage<15)
				dDamage=15;
			BOOL bIsLegal = IsLegalDefence_isdead(pUser);
			pUser->SetDamage((int)dDamage);	//
		/*CString strMsg;
		strMsg.Format("������������ %lf ŭ���˺��� %lf",dDamage,dAssault);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		*/
			
			if(m_ShowHP==1){
				CString strMsg;
				int iMaxHP=pUser->m_sMaxHP;
				if(m_sMagicMaxHP > m_sMaxHP) iMaxHP = pUser->m_sMagicMaxHP;
				strMsg.Format("%s(%d) HP= %d \\ %d",pUser->m_strUserID,pUser->m_uid + USER_BAND,pUser->m_sHP,iMaxHP);
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			
			}
			if(pUser->m_sHP > 0)			// �������� ��� �߰�������
			{
				if(sSid == 2) pUser->SetColdDamage();	// Cold
				if(sSid == 4) pUser->SetFireDamage();	// Fire
			}
			else if(pUser->m_lDeadUsed == 1)
			{			
				int tempRank = m_sCityRank + CITY_RANK_INTERVAL;
				if(bIsLegal == FALSE)//yskang 0.7 ���� ������� cityrank�� ó�� ���� �ʴ´�.
					IsChangeCityRank(m_sCityRank, pUser);
				pUser->GetLevelDownExp(USER_PK, tempRank, FALSE,m_strUserID);			// ����ġ�� �׿� ��ȭ���� �ݿ��Ѵ�.

			strMsg.Format( IDS_USER_ATTACK_FROM, m_strUserID);

				pUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			}

			// ����Ʈ���ʵ�
			if(sSid == 8) 
			{
				GetWideRangeAttack(pUser->m_curx, pUser->m_cury, (int)dWideRangeAttackDamage);	//������Χ�˺�
			}

			::InterlockedExchange((LPLONG)&pUser->m_dwLastPsiDamagedTime, (LONG)dwCurrTime);
		}
		else	// NPC
		{
			if(pNpc->m_tNpcType != NPCTYPE_MONSTER)
			{
				if(pNpc->m_tGuildWar != GUILD_WARRING) return FALSE;	
			}
			dDamage = dDamage * ((double)myrand(80, 110) / 100);

			if(pNpc->SetDamage((int)dDamage, m_uid + USER_BAND, m_pCom) == FALSE)
			{
				pNpc->SendExpToUserList(m_pCom); // ����ġ �й�!!
				pNpc->SendDead(m_pCom);

				if(m_tGuildHouseWar == GUILD_WARRING && pNpc->m_NpcVirtualState == NPC_WAIT)
				{
					CheckGuildHouseWarEnd();
				}
				else
				{
					if(m_tQuestWar == GUILD_WARRING) CheckQuestEventZoneWarEnd();

					int diffLevel = abs(m_sLevel - pNpc->m_byClassLevel);
					if(diffLevel < 40)
					{
						m_iCityValue=m_iCityValue+250;
						GetCityRank();
						CheckMaxValue(m_dwXP, 1);		// ���� �������� 1 ����!	
						SendXP();
					}
				}
			}
			else								// �������� ��� �߰�������
			{
				if(sSid == 2) pNpc->SetColdDamage();	// Cold
			}
			if(m_ShowHP==1){
				CString strMsg;
				strMsg.Format("%s(%d) HP= %d \\ %d",pNpc->m_strName,pNpc->m_sNid + NPC_BAND,pNpc->m_sHP,pNpc->m_sMaxHP);
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			}
			// ����Ʈ���ʵ�
			if(sSid == 8) 
			{
				GetWideRangeAttack(pNpc->m_sCurX, pNpc->m_sCurY, (int)dWideRangeAttackDamage);	//���� ���̿��п����� ���� ó��
			}
		}
		break;

	}

	switch(sSid)
	{
	// �����迭 -------------------------------------------------------------------------------//
	case 0:	//Recovery	: SP ȸ��
		if(bIsUser) 
		{
			pUser->m_sSP += (int)(0.5 + sBasic + m_sLevel * ((double)sLevelUp / 100));	// jjs07 2001.11.23 
			//pUser->m_sSP += 20 + m_sMagicVOL / 4;		// old
			if(pUser->m_sSP > pUser->m_sMagicMaxSP) pUser->m_sSP = pUser->m_sMagicMaxSP;
			pUser->SendSP();
		}
		// NPC �� ���� ��������� ����
		else return FALSE;
		break;

	case 1:	// ���̽�Ʈ	: ���ݼӵ� ��� (���� ������ ����)
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100));
			pUser->SetHaste(iTime);
		}
		// NPC �� ���� ��������� ����
		else return FALSE;
		break;

	case PSIONIC_FAST_RUN:	// Fast Run	: �̵��ӵ� ���
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100));
			pUser->SetFastRun(iTime);
		}
		// NPC �� ���� ��������� ����
		else return FALSE;
		break;

	// ������ �迭 ----------------------------------------------------------------------------//
	case 3:	// ����
		if(bIsUser) 
		{
			
			pUser->m_sHP += (int)(0.5 + sBasic + m_sLevel * ((double)sLevelUp / 100));	// jjs07 2001.11.23 
			if(pUser->m_sHP > pUser->m_sMagicMaxHP) pUser->m_sHP = pUser->m_sMagicMaxHP;
			pUser->SendHP();
		}
		// NPC �� ���� ��������� ����
		else return FALSE;

		break;

	case 6:	// ����
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100)); // jjs07 2001.11.24
			pUser->SetShield(iTime);	// old
			//pUser->SetShield(m_sMagicVOL);
		}
		// NPC �� ���� ��������� ����
		else return FALSE;
		break;


	case 30:	// ����
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100)); // jjs07 2001.11.24
			pUser->SetBigShield(iTime);	// old
			//pUser->SetShield(m_sMagicVOL);
		}
		// NPC �� ���� ��������� ����
		else return FALSE;
		break;

	case 7: // �ڷ���Ʈ 
		iRepeat = 0;
//		ptTel = ConvertToServer(pt.x, pt.y);
//		if(ptTel.x < 0 || ptTel.y < 0) return FALSE;

		do
		{
//			type = ((g_zone[m_ZoneIndex]->m_pMap[ptTel.x][ptTel.y].m_dwType & 0xFF00 ) >> 8);
//			if(!CheckInvalidZoneState(type)) { iRepeat++; continue; }

//			if(Teleport(ptTel.x, ptTel.y) == FALSE) iRepeat++;
			if(Teleport(pt.x, pt.y) == FALSE) iRepeat++;
			else break;

		}while(iRepeat < 2);

		break;

	case PSIONIC_MIND_SHOCK:	// Mind Shock
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100));
//			if(pUser->SetMindShock(iTime) == TRUE) SendSystemMsg(IDS_MIND_SHOCK_SUCCESS, SYSTEM_NORMAL, TO_ME );
			if(pUser->SetMindShock(iTime) == TRUE) SendSystemMsg("����籩ʹ�óɹ�.", SYSTEM_NORMAL, TO_ME );
		}

		break;

	case PSIONIC_MIND_GUARD:	// Mind Guard
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100));
			pUser->SetMindGuard(iTime);
		}

		break;

	// ���˰迭 ------------------------------------------------------------------------------//
	case 9:	// Ecasion	: ����ȸ��
		if(bIsUser)	// USER
		{
			int type = 0;
			if(pUser->m_strUserID != m_strUserID) return FALSE;	// �ڱ��ڽ��� ��쿡��
			
			iRepeat = 0;
			iEvadeRandom = myrand(1, 5);
			do
			{
				int min_x = m_curx - iEvadeRange * iEvadeRandom; if(min_x < 0) min_x = 0;
				int max_x = m_curx + iEvadeRange * iEvadeRandom; if(max_x >= pMap->m_sizeMap.cx) max_x = pMap->m_sizeMap.cx - 1;
				int min_y = m_cury - iEvadeRange * iEvadeRandom; if(min_y < 0) min_y = 0;
				int max_y = m_cury + iEvadeRange * iEvadeRandom; if(max_y >= pMap->m_sizeMap.cy) max_y = pMap->m_sizeMap.cy - 1;
				
				int move_x = 0, move_y =0;
				
				move_x = myrand(min_x, max_x);
				move_y = myrand(min_y, max_y);
	
				type = ((g_zone[m_ZoneIndex]->m_pMap[move_x][move_y].m_dwType & 0xFF00 ) >> 8);
					
				if(!CheckInvakidZoneChangeState(type)) { iRepeat++; continue; }

				if(Teleport(move_x, move_y) == FALSE) iRepeat++;
				else break;
				
			}while(iRepeat < 6);
		}
		else return FALSE;
		break;

	case PSIONIC_PSI_SHIELD:		// ���̿��� ����
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100)); // jjs07 2001.11.24
			pUser->SetPsiShield(iTime);
		}
		else return FALSE;
		break;

	// �ѱ� -------------------------------------------------------------------------------------------//
	case 11:	// Dex Up
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100)); // jjs07 2001.11.24
			pUser->SetDexUp(iTime);
		}
		else return FALSE;
		break;

	case 12:	// Max HP Up
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100));
			pUser->SetMaxHPUp(iTime);
		}
		else return FALSE;
		break;

	case PSIONIC_PIERCING_SHIELD:	// ���� �ձ�
		if(bIsUser) 
		{
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100)); // jjs07 2001.11.24
			pUser->SetPiercingShield(iTime);
		}
		else return FALSE;

		break;

	// ���迭 -----------------------------------------------------------------------------------------//
	case 13:	// Adamantine
		if(bIsUser)
		{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return FALSE; //yskang 0.5
	if(m_iDisplayType == 6) return FALSE; //yskang 0.5
#endif
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100 ));	// alisia 2002.03.21
			pUser->SetAdamantine( iTime );
		}
		else return FALSE;
		break;

	case 14:	// MightyWeapon
		if(bIsUser)
		{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return FALSE; //yskang 0.5
	if(m_iDisplayType == 6) return FALSE; //yskang 0.5
#endif
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100 ));	// alisia 2002.03.21
			pUser->SetMightyWeapon( iTime );
		}
		else return FALSE;
		break;

	case 15:	// Berserker
		if(bIsUser)
		{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return FALSE; //yskang 0.5
	if(m_iDisplayType == 6) return FALSE; //yskang 0.5
#endif
			iTime = (int)(0.5 + sHoldTime + m_sLevel * ((double)sLevelUp / 100 ));	// alisia 2002.03.21
			pUser->SetBerserker( iTime );///ymc
		}
		else return FALSE;
		break;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//��ɱ�˺�����

void USER::EBODY_Skill(short sSid,USER *pUser ,CNpc* pNpc,BOOL bIsUser)
{
	BOOL	bCanUseSkill = FALSE;
	BOOL	bIsCritical = FALSE;
	byte tWeaponClass;
	int nDamage,nFinalDamage,nDefense;
	double EBODYSkill=1.05;
	CString strMsg;
	
	 bCanUseSkill = IsCanUseWeaponSkill(tWeaponClass);	
	 if(!bCanUseSkill)//���û��57 �򷵻�
		 return ;
	if(m_byClass==0) EBODYSkill=1.10;
	if(m_byClass==3)  EBODYSkill=1;
	if(bIsUser)	// USER
	{
		//����� ----------------------------------------------------------------//
		nDamage =(int)( GetNormalInitDamage(tWeaponClass, pUser, bIsCritical,true)*EBODYSkill);//�����ܹ���.
		if(nDamage < 0) nDamage = 0;
		if(pUser->m_tAbnormalKind == ABNORMAL_BYTE_COLD) nDamage += 10;
		nFinalDamage = GetFinalDamage(pUser, nDamage, tWeaponClass,bIsCritical,1);//���������˺�
		//�����˺����Ӽ���  �������˼���
		nFinalDamage=nFinalDamage+m_DynamicMagicItem[7];
		nFinalDamage=nFinalDamage-(pUser->m_DynamicMagicItem[5]+pUser->m_DynamicMagicItem[6]);
		//________________________________________________
		if(nFinalDamage < 0) nFinalDamage = 0;
		BOOL bIsLegal = IsLegalDefence_isdead(pUser);	
		pUser->SetDamage(nFinalDamage);
		if(m_ShowHP==1){
			
			int iMaxHP=pUser->m_sMaxHP;
			if(m_sMagicMaxHP > m_sMaxHP) iMaxHP = pUser->m_sMagicMaxHP;
			strMsg.Format("%s(%d) HP= %d \\ %d",pUser->m_strUserID,pUser->m_uid + USER_BAND,pUser->m_sHP,iMaxHP);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		if(pUser->m_lDeadUsed == 1)
		{			
			int tempRank = m_sCityRank + CITY_RANK_INTERVAL;
			if(bIsLegal == FALSE)//yskang 0.7 ���� ������� cityrank�� ó�� ���� �ʴ´�.
				IsChangeCityRank(m_sCityRank, pUser);
			pUser->GetLevelDownExp(USER_PK, tempRank, FALSE,m_strUserID);			// ����ġ�� �׿� ��ȭ���� �ݿ��Ѵ�.
			
			strMsg.Format( IDS_USER_ATTACK_FROM, m_strUserID);
			pUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
	}else{
		nDefense = pNpc->GetDefense();
		nDamage = (int)(GetNormalInitDamage(tWeaponClass, NULL,bIsCritical,true)*1.05);
		if(nDamage < 0) nDamage = 0;
		double nVCHit = 0.0;
		nVCHit = GetCriticalHit(tWeaponClass,1);
		if(nVCHit > 0.0) bIsCritical = TRUE;
		nFinalDamage = (int)((double)nDamage - ((double)nDefense * nVCHit));
		//�����˺����Ӽ���  �������˼���
		nFinalDamage=nFinalDamage+m_DynamicMagicItem[7];

		if(nFinalDamage < 0) nFinalDamage = 0;
		short sOldNpcHP = pNpc->m_sHP;
		//���ͶԹ����˺���ֵ
		/*
		CString strMsg;
		strMsg.Format("������������ %d,���� %s �����˺��� %d",nDamage,pNpc->m_strName,nFinalDamage);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		*/
		//		if(pNpc->SetDamage(nFinalDamage, m_strUserID, m_uid + USER_BAND, m_pCom) == FALSE)
		if(pNpc->SetDamage(nFinalDamage, m_uid + USER_BAND, m_pCom) == FALSE)
		{
			pNpc->SendExpToUserList(m_pCom); // ����ġ �й�!!
			pNpc->SendDead(m_pCom);

			if(m_tGuildHouseWar == GUILD_WARRING && pNpc->m_NpcVirtualState == NPC_WAIT)
			{
				CheckGuildHouseWarEnd();
			}
			else
			{
				if(m_tQuestWar == GUILD_WARRING) CheckQuestEventZoneWarEnd();
				int diffLevel = abs(m_sLevel - pNpc->m_byClassLevel);
				if(diffLevel < 40)
				{
					m_iCityValue=m_iCityValue+250;
					GetCityRank();
					CheckMaxValue(m_dwXP, 1);		// ���� �������� 1 ����!	
					SendXP();
				}
			}
		}
		if(m_ShowHP==1){
		//	CString strMsg;
			strMsg.Format("%s(%d) HP= %d \\ %d",pNpc->m_strName,pNpc->m_sNid + NPC_BAND,pNpc->m_sHP,pNpc->m_sMaxHP);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
	}
	
/*	if(nTargetID >= NPC_BAND)				// NPC
	{
	


		

		// �����̸� //Damage ó�� ----------------------------------------------------------------//
		if(bIsHit == TRUE)
		{
			nVCHit = 0.0;
			nVCHit = GetCriticalHit(tWeaponClass);
			if(nVCHit > 0.0) bIsCritical = TRUE; //yskang 0.3

			nFinalDamage = (int)((double)nDamage - ((double)nDefense * nVCHit));
			if(m_byClass == FIREARMS) // �ѱ��� ������ ���� - �ӽ��ڵ�  
			{
				double dMin = (double)myrand(90, 100) / 100;
				nFinalDamage = (int)((double)nFinalDamage * dMin);
			}

			if(nFinalDamage < 0) nFinalDamage = 0;
		}

		// ������ ������/��Ƽ���� ����
		if(tWeaponClass != 255)	// �Ǽ��� �ƴѰ��
		{
			// ������
			sNewDuration = (int)((double)nDefense/10 + 0.5);
			sOldDuration = m_UserItem[RIGHT_HAND].sDuration;

			if(tWeaponClass != FIREARMS) SendDuration(RIGHT_HAND, sNewDuration);
			//if(tWeaponClass != FIREARMS) m_UserItem[RIGHT_HAND].sDuration -= sNewDuration;
			else
			{
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if(m_UserItem[RIGHT_HAND].sSid >= 0 && m_UserItem[RIGHT_HAND].sSid < g_arItemTable.GetSize())
				{
					if(iRandom < g_arItemTable[m_UserItem[RIGHT_HAND].sSid]->m_byErrorRate)
					{
						iFireErrDec = 10 - GetBreakDec();
						if(iFireErrDec < 0) iFireErrDec = 0;

						//m_UserItem[RIGHT_HAND].sDuration -= iFireErrDec;
						SendDuration(RIGHT_HAND, iFireErrDec);
					}
				}
			}

			//			if(m_UserItem[RIGHT_HAND].sDuration < 0) m_UserItem[RIGHT_HAND].sDuration = 0;
			//			if(sOldDuration != m_UserItem[RIGHT_HAND].sDuration) SendDuration(RIGHT_HAND);
		}

		// Calculate Target HP -------------------------------------------------------//
		short sOldNpcHP = pNpc->m_sHP;
		//���ͶԹ����˺���ֵ
		CString strMsg;
		strMsg.Format("������������ %d,���� %s �����˺��� %d",nDamage,pNpc->m_strName,nFinalDamage);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		//		if(pNpc->SetDamage(nFinalDamage, m_strUserID, m_uid + USER_BAND, m_pCom) == FALSE)
		if(pNpc->SetDamage(nFinalDamage, m_uid + USER_BAND, m_pCom) == FALSE)
		{
			pNpc->SendExpToUserList(m_pCom); // ����ġ �й�!!
			pNpc->SendDead(m_pCom);

			if(m_tGuildHouseWar == GUILD_WARRING && pNpc->m_NpcVirtualState == NPC_WAIT)
			{
				CheckGuildHouseWarEnd();
			}
			else
			{
				if(m_tQuestWar == GUILD_WARRING) CheckQuestEventZoneWarEnd();
				int diffLevel = abs(m_sLevel - pNpc->m_byClassLevel);
				if(diffLevel < 30)
				{
					CheckMaxValue(m_dwXP, 1);		// ���� �������� 1 ����!	
					SendXP();
				}
			}
		}

		// ���� ��� ����
		SendAttackSuccess(nTargetID, bIsCritical, pNpc->m_sHP, pNpc->m_sMaxHP);//yskang 0.3

		m_dwLastAttackTime = GetTickCount();
	}
	*/
}
//
///////////////////////////////////////////////////////////////////////////
//	XP ���� ������.
//
void USER::SendXP()
{
	CBufferEx TempBuf;

	TempBuf.Add(SET_XP);
	TempBuf.Add(m_dwXP);

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////
//	������ �̵��Ѵ�.
//
void USER::TownPotal()
{
	int isZone = -1;
	int iPotalIndex = -1;
	BYTE result = SUCCESS;
	BYTE zoneChange = 0;
	CPoint ptTown, pt;
	CPoint ptPotal;
	
	CBufferEx TempBuf;

	pt = GetTownPotal(isZone);

	int i, rand;
	
	MAP* pMap = NULL;

	/*************************[ Back Up ]***************************************/	
//	int org_x = m_curx;
//	int org_y = m_cury;
	int org_z = m_curz;

	if(isZone <0) { result = FAIL; goto go_result; };	// �ƴϸ� ���� �������� �̵�

	if(	!IsZoneInThisServer(isZone) ) { result = FAIL; goto go_result; };

//	ZoneMoveReq( isZone, x, y );
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) { result = FAIL; goto go_result; };

	pMap = g_zone[m_ZoneIndex];		if( !pMap ) { result = FAIL; goto go_result; };

	if( m_curx < 0 || m_curx >= pMap->m_sizeMap.cx ) { result = FAIL; goto go_result; };
	if( m_cury < 0 || m_cury >= pMap->m_sizeMap.cy ) { result = FAIL; goto go_result; };

	m_NowZoneChanging = TRUE;					// Zone Change �ϰ� �ִ�.
	/*************************[ Zone Change Init]*******************************/
	SetZoneIndex(isZone);

	ptPotal = ConvertToServer(pt.x, pt.y);		// �̺�Ʈ ��ǥ�� Ŭ���̾�Ʈ �����̹Ƿ�...

	if( ptPotal.x == -1 || ptPotal.y == -1 )
	{
		SetZoneIndex(org_z);
		result = FAIL; goto go_result;
	}

	i = SEARCH_TOWN_POTAL_COUNT;				// ��ǥ�� ã������ Ƚ��
	
	ptTown = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);

	if(ptTown.x == -1 || ptTown.y == -1) 
	{
		while(TRUE)								// Ÿ����Ż ���� ��ǥ�� ��´�.
		{
			rand = myrand(-20, 20);
			ptPotal.x += rand; ptPotal.y += rand;

			ptTown = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);
			if(ptTown.x != -1 && ptTown.y != -1) break;

			i--;
			if(i <= 0) // ���߿�
			{ 
				ptTown.x = -1; ptTown.y = -1;
				break;
			}
		}
	}

	if(ptTown.x == -1 || ptTown.y == -1) 
	{
		SetZoneIndex(org_z);
		m_NowZoneChanging = FALSE;
		result = FAIL; goto go_result;
	}
	else									// �������������̸�...
	{
		int type = ((g_zone[m_ZoneIndex]->m_pMap[ptTown.x][ptTown.y].m_dwType & 0xFF00 ) >> 8);
		if(!CheckInvakidZoneChangeState(type)) 
		{
			SetZoneIndex(org_z);
			m_NowZoneChanging = FALSE;
			result = FAIL; goto go_result;
		}
	}

	SetZoneIndex(org_z);					// ���� �� ��������  �˸���.

//	if(m_tQuestWar == GUILD_WARRING) CheckQuestEventZoneWarEnd();

	if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 
			(long)0, (long)(m_uid + USER_BAND)) == (long)m_uid + USER_BAND)
	{
//		SetUid(m_curx, m_cury, 0);				
		SendMyInfo(TO_INSIGHT, INFO_DELETE);
		
		SetZoneIndex(isZone);						
		m_curx = ptTown.x;
		m_cury = ptTown.y;
		m_curz = isZone;
		SetUid(m_curx, m_cury, m_uid + USER_BAND );// ������ �̵��� �ڸ��� Ȯ��, ��ġ���� ����

		SendZoneChange(TRUE);	// ��ü������  ���������� ���� NPC������ client���� ����ִ�.

		m_presx = -1;
		m_presy = -1;
		SightRecalc();
		SendMyInfo( TO_INSIGHT, INFO_MODIFY );
		
		m_NowZoneChanging = FALSE;
	}
	else
	{
		SendMyInfo(TO_INSIGHT, INFO_DELETE);

		SetZoneIndex(isZone);						
		m_curx = ptTown.x;
		m_cury = ptTown.y;
		m_curz = isZone;
		SetUid(m_curx, m_cury, m_uid + USER_BAND );// ������ �̵��� �ڸ��� Ȯ��, ��ġ���� ����

		SendZoneChange(TRUE);	// ��ü������  ���������� ���� NPC������ client���� ����ִ�.

		m_presx = -1;
		m_presy = -1;
		SightRecalc();
		SendMyInfo( TO_INSIGHT, INFO_MODIFY );
		
		m_NowZoneChanging = FALSE;
	}

	if(result)
	{
		SendWeatherInMoveZone();					// �̵� ���� ������ȭ�� �˸���.
		SightRecalc();						// �� �þ߾��� ������ ������ ������.
		return;
	}

go_result:

	SendTownPotal(result, zoneChange);
}

////////////////////////////////////////////////////////////////////////////
//	Ÿ�� ��Ż��Ŷ�� ������.
//
void USER::SendTownPotal(BYTE result, BYTE changeZone)
{
	CBufferEx TempBuf;

	TempBuf.Add(PSI_TOWN_POTAL);
	TempBuf.Add(result);

	if(result == FAIL) Send(TempBuf, TempBuf.GetLength());

	CPoint pos = ConvertToClient( m_curx, m_cury );
	if( pos.x == -1 || pos.y == -1 ) { pos.x = 1; pos.y = 1; }	// �̺κ��� ���߿� �ٲ���

	TempBuf.Add(changeZone);

	TempBuf.Add(m_uid + USER_BAND);

	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);
	TempBuf.Add((short)m_curz);

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////
//	���� Ÿ����Ż�� �˸���.
//
void USER::AddCellUidFromTownPotal()
{
//	g_cell[m_nCellZone]->AddCellFromTownPotal(m_ptCell, m_uid + USER_BAND);
}

////////////////////////////////////////////////////////////////////////////
//	���� Ÿ����Ż�� ���� �� ������ �˸���.
//
void USER::DeleteCellUidFromTownPotal()
{
//	g_cell[m_nCellZone]->DeleteCellFromTownPotal(m_ptCell, m_uid + USER_BAND);
}

////////////////////////////////////////////////////////////////////////////
//	SP, PP, �����̻� ó��
//
void USER::UserTimer()
{
	if(m_state != STATE_GAMESTARTED) return;
	if(m_bLive == USER_DEAD) return;

	int index = 0;
	CBufferEx TempBuf;

	//---------------------------------------------------
	//yskang usertimer �ʹ� ���� ȣ����� �ʵ��� 
	DWORD dwCurrTick = GetTickCount();
	if(dwCurrTick - m_dwUserTimerRun < 1000) //1�� �̳��� �ٽ� ȣ��Ȱ��̶�� �����Ѵ�.
		return;
	else
		m_dwUserTimerRun = dwCurrTick;
	//�ػ����;�
	if(dwCurrTick - m_ShouhuTime>120000){
		if(m_UserItem[38].sSid!=0){
			if(m_UserItem[38].sDuration>4)
				SendDuration(38, 3);
			else{
				m_UserItem[38].sDuration=0;
				GetMagicItemSetting();
			}
			m_ShouhuTime=dwCurrTick;
		}
	}
	//�һ�AKR �Զ��Ǿ��� ÿ��300�� �����Զ����� �ȼ�*100
	if(56==m_curz || 402==m_curz){
		if(dwCurrTick - m_ionlineTime > 20*1000){
			int iNum = m_sLevel*150;
			if(m_tIsOP == 2){
				iNum = iNum * 1.5;
			}else if(m_tIsOP == 3)
			{
				iNum = iNum * 3;
			}
			GetExp(iNum);
			CString strMsg;
			strMsg.Format("���þ��� %d ��.",iNum);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			m_ionlineTime=dwCurrTick;
		}
	}else{
		m_ionlineTime=dwCurrTick;
	}
	
//���߹һ���Ԫ��
	int iDN = 0;
	int iTime = 0;
	if(g_Shop.IsZone(m_curz) == TRUE)
	{
		iDN = g_Shop.m_iZoneDnNum;
		iTime = g_Shop.m_iZoneTime;
	}else{
		iDN = g_Shop.m_iAllDnNum;
		iTime = g_Shop.m_iAllTime;
	}

	if ((int)(dwCurrTick - m_dwShopTime) > iTime)
	{
		CString strMsg;
		strMsg.Format("��� %d Ԫ��.",iDN);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		m_dwDNMoney = m_dwDNMoney +iDN;
		m_dwShopTime=dwCurrTick;
	}
//�Ǳ��ܱ�ʱ�����
	if(m_dwBSTime !=0){
		DWORD LostTime=dwCurrTick-m_dwBSLaseTime;
		TRACE("�ܱ��ǹ�ȥ��ʱ��: %d\n",LostTime/1000);
		if(LostTime >=m_dwBSTime){
			m_iSkin=0;
			CheckMagicItemMove();
			SendMyInfo(TO_INSIGHT, INFO_MODIFY);
			m_dwBSTime=0;
		}
	}

	//---------------------------------------------------

	if(dwCurrTick - m_dwServerTick >= 82000)
	{
		TRACE("Speed Hack User Account = %s, ID = %s\n", m_strAccount, m_strUserID);
		SYSTEMTIME time;
		GetLocalTime(&time);
		
		CString str;
		str.Format( IDS_USER_SYSTEM_MSG02, time.wMonth, time.wDay, time.wHour, time.wMinute, m_strAccount, m_strUserID, 999999);
		
		EnterCriticalSection( &m_CS_FileWrite );
		g_fpSpeedHack.Write(str, str.GetLength());
		LeaveCriticalSection( &m_CS_FileWrite );
		
		SendSystemMsg( IDS_USER_SPEED_HACK, SYSTEM_SPECIAL, TO_ME);
		return;
	}

	if(m_bPkStatus)
	{
		if(m_dwPkStatusTime == 0) m_bPkStatus = FALSE;
		else if(dwCurrTick - m_dwPkStatusTime > USER_PK_TICKS)
		{
			index = 0;
			m_bPkStatus = FALSE;				// ī�����¿��� ���.
			m_dwPkStatusTime = 0;
	
			SetByte(m_TempBuf, SET_USER_PK_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetByte(m_TempBuf, 0x02, index);
			SetByte(m_TempBuf, m_bPkStatus, index);
			
			Send(m_TempBuf, index);
			SendInsight(m_TempBuf, index);
//			SendExactScreen(m_TempBuf, index);
		}
	}

	if(dwCurrTick - m_dwLastUpdateTime > 300000)		// DB Update Timer
	{
		// �ð� ������ �����ϱ�...
		// IKING 2002.1.
		if ( m_pIocpBase )
			m_pIocpBase->SetAliveTimeUpdate( m_uid, dwCurrTick );
		//

//		if(m_lRequireNum > 0)
//		{  
		UpdateUserData();
//		InterlockedExchange(&m_lRequireNum, 0);
//		}
		m_dwLastUpdateTime = dwCurrTick;
	}
	if(dwCurrTick - m_dwLastSaveTime > 90000)// Finito : DB Save Timer
	{
		UpdateUserData(TRUE);
		m_dwLastSaveTime = dwCurrTick;
	}

	if(dwCurrTick - m_dwLastTimeCount < 900)
	{
		return;
	}

	// ���� �������� ������ �ð�, �Ⱓ�� �˷��ش�.
	if(m_iDisplayType == 2)
	{
		SYSTEMTIME currTime;
		GetLocalTime(&currTime);

		CTime curr(currTime);
		CTime login(m_LoginTime);

		CTimeSpan DiffTime = curr - login;	

		int tempValue = m_iValidTime - DiffTime.GetMinutes();
//		int tempValue = DiffTime.GetMinutes();

		CString strMsg = _T("");

		if(tempValue <= 50) //����
		{
			if(m_iTimeInterval - tempValue >= 6) 
			{				
				m_iTimeInterval = tempValue;
				if(m_iTimeInterval <= 0) m_iTimeInterval = 0;
			}

			if(tempValue <= 0)
			{
				strMsg.Format( IDS_USER_GAME_TIME_END );
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_DISCONNECT, TO_ME);
//				SoftClose();		//Client���� Log Out���� ó��
			}
			else if(m_iTimeInterval == tempValue)
			{
				if(m_iTimeInterval > 5)
				{
					strMsg.Format( IDS_USER_GAME_TIME_REMAIN, tempValue);
					SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
					m_iTimeInterval -= 5;
				}
				else if(m_iTimeInterval <= 5 && m_iTimeInterval >= 2)
				{
					strMsg.Format( IDS_USER_GAME_TIME_REMAIN, tempValue);
					SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);
					m_iTimeInterval -= 1;
				}
				else
				{
					strMsg.Format( IDS_USER_FORCE_END_LATER_FEW_MINUTE, tempValue);
					SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);
					m_iTimeInterval -= 1;
				}

				if(m_iTimeInterval <= 0) m_iTimeInterval = 0;				
			}
		}
	}
/*	else
	{
		//if(m_iDisplayType == 6 && m_sLevel > 25)
		if(m_iDisplayType == 6)
		{			
			
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_FREE_SERVICE_LEVEL );
			SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_DISCONNECT, TO_ME);
//				SoftClose();
			return;
		}
	}
*/	
	m_dwLastTimeCount = dwCurrTick;

	// SP ȸ��
	short sOldSP = m_sSP;
	GetSP();
	if(m_sSP != sOldSP) SendSP();

	// PP ȸ��
	short sOldPP = m_sPP;
	GetPP();
	if(m_sPP != sOldPP) SendPP();

	// HP ȸ��
	short sOldHP = m_sHP;
	GetHP();
	if(m_sHP != sOldHP) SendHP();

	DWORD dwAbnormalTime = m_dwAbnormalTime;
	DWORD dwRemainTime = 0;
	CString szMsg = _T("");
	DWORD dwDiffTime = 0;

	dwCurrTick = GetTickCount();

	//
	// Haste, Shield, Dex Up, Max HP Up ó�� -----------------------------------------------//
	//
	if(m_dwHasteTime != 0)
	{

		if((dwCurrTick - m_dwLastHasteTime) > m_dwHasteTime)		// Haste �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_HASTE);
			m_dwHasteTime = 0;
			m_dwLastHasteTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);
			
			Send(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwHasteTime - (dwCurrTick - m_dwLastHasteTime);
			m_dwHasteTime = dwRemainTime;
			m_dwLastHasteTime = dwCurrTick;
		}
	}

	if(m_dwShieldTime != 0)
	{
		if((dwCurrTick - m_dwLastShieldTime) > m_dwShieldTime)		// Shield �� Ǯ�� �ð��̸�
		{
			m_dwShieldTime = 0;
			m_dwLastShieldTime = dwCurrTick;

			if(m_bNecklaceOfShield == FALSE)	// ��ȣ�� ����̰� ������
			{
				DeleteAbnormalInfo(ABNORMAL_SHIELD);
				
				index = 0;
				SetByte(m_TempBuf, SET_USER_STATE, index);
				SetInt(m_TempBuf, m_uid + USER_BAND, index);
				SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
				SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);
				
				SendInsight(m_TempBuf, index);
//				SendExactScreen(m_TempBuf, index);
			}
		}
		else
		{
			dwRemainTime = m_dwShieldTime - (dwCurrTick - m_dwLastShieldTime);
			m_dwShieldTime = dwRemainTime;
			m_dwLastShieldTime = dwCurrTick;
		}
	}

	if(m_dwDexUpTime != 0)
	{
		if((dwCurrTick - m_dwLastDexUpTime) > m_dwDexUpTime)		// Dex Up �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_DEX_UP);
			m_dwDexUpTime = 0;
			m_dwLastDexUpTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);		
			Send(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwDexUpTime - (dwCurrTick - m_dwLastDexUpTime);
			m_dwDexUpTime = dwRemainTime;
			m_dwLastDexUpTime = dwCurrTick;
		}
	}

	if(m_dwMaxHPUpTime != 0)
	{
		if((dwCurrTick - m_dwLastMaxHPUpTime) > m_dwMaxHPUpTime)	// Max HP Up �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_MAX_HP_UP);
			m_dwMaxHPUpTime = 0;
			m_dwLastMaxHPUpTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);		
			Send(m_TempBuf, index);
			
			SetUserToMagicUser(-1);
			//-----------------------------------------
			//yskang 0.7 MAX HP UP �ٴ� ���߿� Ǯ���� �հ� �ɸ� �� ó�� ��� ���� ���� ���� ����
			CBufferEx	TempBuf;
			TempBuf.Add(MAX_HP_EXIT);
			TempBuf.Add(m_sHP);
			TempBuf.Add(m_sMagicMaxHP);
			Send(TempBuf,TempBuf.GetLength());
			//SendMyInfo(TO_ME, INFO_MODIFY); //��ü ����Ÿ�� ������ �ʴ´�.
			//--------------------------------------
			if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
		}
		else
		{
			dwRemainTime = m_dwMaxHPUpTime - (dwCurrTick - m_dwLastMaxHPUpTime);
			m_dwMaxHPUpTime = dwRemainTime;
			m_dwLastMaxHPUpTime = dwCurrTick;
		}
	}

	if( m_dwMightyWeaponTime != 0)
	{
		if((dwCurrTick - m_dwLastMightyWeaponTime) > m_dwMightyWeaponTime)		// MightyWeapon �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_MIGHTYWEAPON);
			m_dwMightyWeaponTime = 0;
			m_dwLastMightyWeaponTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);			
			Send(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwMightyWeaponTime - (dwCurrTick - m_dwLastMightyWeaponTime);
			m_dwMightyWeaponTime = dwRemainTime;
			m_dwLastMightyWeaponTime = dwCurrTick;
		}
	}

	if( m_dwAdamantineTime != 0)
	{
		if((dwCurrTick - m_dwLastAdamantineTime) > m_dwAdamantineTime)		// Adamantine �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_ADAMANTINE);
			m_dwAdamantineTime = 0;
			m_dwLastAdamantineTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);		
			Send(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwAdamantineTime - (dwCurrTick - m_dwLastAdamantineTime);
			m_dwAdamantineTime = dwRemainTime;
			m_dwLastAdamantineTime = dwCurrTick;
		}
	}

	if( m_dwBerserkerTime != 0)
	{
		if((dwCurrTick - m_dwLastBerserkerTime) > m_dwBerserkerTime)		// Berserker �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_BERSERKER);
			m_dwBerserkerTime = 0;
			m_dwLastBerserkerTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);	
			SendInsight(m_TempBuf, index);
//			SendExactScreen(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwBerserkerTime - (dwCurrTick - m_dwLastBerserkerTime);
			m_dwBerserkerTime = dwRemainTime;
			m_dwLastBerserkerTime = dwCurrTick;
		}
	}

	if( m_dwFastRunTime != 0)
	{
		if((dwCurrTick - m_dwLastFastRunTime) > m_dwFastRunTime)		// Fast Run �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_FASTRUN);
			m_dwFastRunTime = 0;
			m_dwLastFastRunTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);
//			SendExactScreen(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwFastRunTime - (dwCurrTick - m_dwLastFastRunTime);
			m_dwFastRunTime = dwRemainTime;
			m_dwLastFastRunTime = dwCurrTick;
		}
	}

	if( m_dwMindShockTime != 0)
	{
		if((dwCurrTick - m_dwLastMindShockTime) > m_dwMindShockTime)		// Mind Shock �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_MINDSHOCK);
			m_dwMindShockTime = 0;
			m_dwLastMindShockTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);	
//			SendInsight(m_TempBuf, index);
			SendExactScreen(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwMindShockTime - (dwCurrTick - m_dwLastMindShockTime);
			m_dwMindShockTime = dwRemainTime;
			m_dwLastMindShockTime = dwCurrTick;
		}
	}

	if( m_dwMindGuardTime != 0)
	{
		if((dwCurrTick - m_dwLastMindGuardTime) > m_dwMindGuardTime)		// Mind Guard �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_MINDGUARD);
			m_dwMindGuardTime = 0;
			m_dwLastMindGuardTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);		
//			SendInsight(m_TempBuf, index);
			SendExactScreen(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwMindGuardTime - (dwCurrTick - m_dwLastMindGuardTime);
			m_dwMindGuardTime = dwRemainTime;
			m_dwLastMindGuardTime = dwCurrTick;
		}
	}

	if( m_dwPsiShieldTime != 0)
	{
		if((dwCurrTick - m_dwLastPsiShieldTime) > m_dwPsiShieldTime)		// Psionic Shield �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_PSISHIELD);
			m_dwPsiShieldTime = 0;
			m_dwLastPsiShieldTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);		
			SendInsight(m_TempBuf, index);
//			SendExactScreen(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwPsiShieldTime - (dwCurrTick - m_dwLastPsiShieldTime);
			m_dwPsiShieldTime = dwRemainTime;
			m_dwLastPsiShieldTime = dwCurrTick;
		}
	}
	if( m_dwBigShieldTime != 0)
	{
		if((dwCurrTick - m_dwLastBigShieldTime) > m_dwBigShieldTime)		// Psionic Shield �� Ǯ�� �ð��̸�
		{
				
			DeleteAbnormalInfo(ABNORMAL_BIGSHIELD);
			m_dwBigShieldTime = 0;
			m_dwLastBigShieldTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);			
			SendInsight(m_TempBuf, index);

//			SendExactScreen(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwBigShieldTime - (dwCurrTick - m_dwLastBigShieldTime);
			m_dwBigShieldTime = dwRemainTime;
			m_dwLastBigShieldTime = dwCurrTick;
		}
	}

	if( m_dwPiercingShieldTime != 0)
	{
		if((dwCurrTick - m_dwLastPiercingShieldTime) > m_dwPiercingShieldTime)		// Piercing Shield �� Ǯ�� �ð��̸�
		{
			DeleteAbnormalInfo(ABNORMAL_PIERCING_SHIELD);
			m_dwPiercingShieldTime = 0;
			m_dwLastPiercingShieldTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);			
//			SendInsight(m_TempBuf, index);
			SendExactScreen(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwPiercingShieldTime - (dwCurrTick - m_dwLastPiercingShieldTime);
			m_dwPiercingShieldTime = dwRemainTime;
			m_dwLastPiercingShieldTime = dwCurrTick;
		}
	}

	if( m_dwNoDamageTime != 0)	// Game Start, Zone Change ���� �����ð�
	{
		if((dwCurrTick - m_dwLastNoDamageTime) > m_dwNoDamageTime)		// �����ð��� Ǯ�� �ð��̸�
		{
			m_dwNoDamageTime = 0;
			m_dwLastNoDamageTime = dwCurrTick;
		}
		else
		{
			dwRemainTime = m_dwNoDamageTime - (dwCurrTick - m_dwLastNoDamageTime);
			m_dwNoDamageTime = dwRemainTime;
			m_dwLastNoDamageTime = dwCurrTick;
		}
	}

	if( m_dwHiExpTime != 0)	// ����ġ 2�� ���� ���� �ð� ���
	{
		if((dwCurrTick - m_dwLastHiExpTime) > m_dwHiExpTime)		// ����ġ 2�� �ð��� ��������
		{
			DeleteAbnormalInfo(ABNORMAL_HIEXP);
			m_dwHiExpTime = 0;
			m_dwLastHiExpTime = dwCurrTick;
			
			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);		
			Send(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwHiExpTime - (dwCurrTick - m_dwLastHiExpTime);
			m_dwHiExpTime = dwRemainTime;
			m_dwLastHiExpTime = dwCurrTick;
		}
	}

	if( m_dwMagicFindTime != 0)	// �������� 5�� ���� ���� �ð� ���
	{
		if((dwCurrTick - m_dwLastMagicFindTime) > m_dwMagicFindTime)		// �������� 5�� �ð��� ��������
		{
			DeleteAbnormalInfo(ABNORMAL_MAGICFIND);
			m_dwMagicFindTime = 0;
			m_dwLastMagicFindTime = dwCurrTick;

			index = 0;
			SetByte(m_TempBuf, SET_USER_STATE, index);
			SetInt(m_TempBuf, m_uid + USER_BAND, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo, index);
			SetDWORD(m_TempBuf, m_dwAbnormalInfo_, index);
			Send(m_TempBuf, index);
		}
		else
		{
			dwRemainTime = m_dwMagicFindTime - (dwCurrTick - m_dwLastMagicFindTime);
			m_dwMagicFindTime = dwRemainTime;
			m_dwLastMagicFindTime = dwCurrTick;
		}
	}

	if( m_dwNoChatTime != 0)	// ä�ñ��� ���� �ð� ���
	{
		if((dwCurrTick - m_dwLastNoChatTime) > m_dwNoChatTime)		// ä�ñ��� �ð��� ��������
		{
			m_dwNoChatTime = 0;
			m_dwLastNoChatTime = dwCurrTick;
		}
		else
		{
			dwRemainTime = m_dwNoChatTime - (dwCurrTick - m_dwLastNoChatTime);
			m_dwNoChatTime = dwRemainTime;
			m_dwLastNoChatTime = dwCurrTick;
		}
	}

	// pk count Ǯ���ֱ�
	int rank = m_sCityRank + CITY_RANK_INTERVAL;
	if( rank == SAINT_RANK )		// ���� ����Ʈ���
	{
		if( dwCurrTick - m_dwSaintTimeCount >= 60*1000 )
		{
			CheckMaxValue(m_dwSaintTime, 1);

			if( m_dwSaintTime >= 300 )
			{
				// killcount�� �ϳ� �ٿ��ش�.
				m_sKillCount -= 1;
				if( m_sKillCount < 0 ) m_sKillCount = 0;
				m_dwSaintTime = 0;
			}

			m_dwSaintTimeCount = dwCurrTick;
		}
	}
	else
	{
		m_dwSaintTime = 0;
		m_dwSaintTimeCount = dwCurrTick;
	}

	//�����̻� ó����ƾ ���� --------------------------------------//

	switch(m_tAbnormalKind)
	{
	case ABNORMAL_BYTE_NONE:
		return;
	case ABNORMAL_BYTE_POISON:
		break;
	case ABNORMAL_BYTE_CONFUSION:
		break;
	case ABNORMAL_BYTE_PARALYSIS:
		break;
	case ABNORMAL_BYTE_BLIND:
		break;
	case ABNORMAL_BYTE_LIGHT:
		break;
	case ABNORMAL_BYTE_FIRE:
		dwDiffTime = dwCurrTick - m_dwLastAbnormalTime;

		if(dwDiffTime >= 5000)	
		{
			SetDamage((int)((double)m_sMaxHP * 0.02 + 0.5));
			//if(m_bLive == USER_DEAD) 
			if(m_lDeadUsed == 1)
			{
				GetLevelDownExp(USER_PK, -1, FALSE,m_strUserID);			// ����ġ�� �׿� ��ȭ���� �ݿ��Ѵ�.
				return;
			}
		}

		if((dwCurrTick - m_dwLastAbnormalTime) > dwAbnormalTime)	// Fire �����̻��� Ǯ�� �ð��� ������
		{
			m_tAbnormalKind = ABNORMAL_BYTE_NONE;
			m_dwAbnormalTime = 0;
			m_dwLastAbnormalTime = GetTickCount();

			if(g_bDebug) SendSystemMsg( IDS_USER_FIRE_DAMAGE_RESET, SYSTEM_NORMAL, TO_ME);

			TempBuf.Add(SET_USER_STATE);
			TempBuf.Add(m_uid + USER_BAND);
			DeleteAbnormalInfo(ABNORMAL_FIRE);
			TempBuf.Add(m_dwAbnormalInfo);
			TempBuf.Add(m_dwAbnormalInfo_);
			
			Send(TempBuf, TempBuf.GetLength());
		}
		else
		{
			dwRemainTime = dwAbnormalTime - (dwCurrTick - m_dwLastAbnormalTime);
			m_dwAbnormalTime = dwRemainTime;
			m_dwLastAbnormalTime = dwCurrTick;
		}

		break;

	case ABNORMAL_BYTE_COLD:
		if((dwCurrTick - m_dwLastAbnormalTime) > dwAbnormalTime)	// Cold �����̻��� Ǯ�� �ð��� ������
		{
			m_tAbnormalKind = ABNORMAL_BYTE_NONE;
			m_dwAbnormalTime = 0;
			m_dwLastAbnormalTime = GetTickCount();

			if(g_bDebug) SendSystemMsg( IDS_USER_COLD_DAMAGE_RESET, SYSTEM_NORMAL, TO_ME);

			TempBuf.Add(SET_USER_STATE);
			TempBuf.Add(m_uid + USER_BAND);
			DeleteAbnormalInfo(ABNORMAL_COLD);
			TempBuf.Add(m_dwAbnormalInfo);
			TempBuf.Add(m_dwAbnormalInfo_);
			
			Send(TempBuf, TempBuf.GetLength());
		}
		else
		{
			dwRemainTime = dwAbnormalTime - (dwCurrTick - m_dwLastAbnormalTime);
			m_dwAbnormalTime = dwRemainTime;
			m_dwLastAbnormalTime = dwCurrTick;

			// Test Code !!!!
//			szMsg.Format("Cold - %d", dwRemainTime);
//			if(g_bDebug) SendSystemMsg((LPTSTR)(LPCTSTR)szMsg, SYSTEM_NORMAL, TO_ME);
		}
		break;
	}
}

void USER::SetPsiAbnormalStatus()
{
	if(m_tAbnormalKind != ABNORMAL_BYTE_NONE)
	{
		switch(m_tAbnormalKind)
		{
		case ABNORMAL_BYTE_FIRE:
			AddAbnormalInfo(ABNORMAL_FIRE);
			break;
		case ABNORMAL_BYTE_COLD:
			AddAbnormalInfo(ABNORMAL_COLD);
			break;
		}
	}

	if( m_dwHasteTime != 0 )							AddAbnormalInfo(ABNORMAL_HASTE);
	if( m_dwShieldTime != 0 || m_bNecklaceOfShield)		AddAbnormalInfo(ABNORMAL_SHIELD);
	if( m_dwDexUpTime != 0 )							AddAbnormalInfo(ABNORMAL_DEX_UP);
	if( m_dwMaxHPUpTime != 0 )							AddAbnormalInfo(ABNORMAL_MAX_HP_UP);

	if( m_dwFastRunTime != 0 )			AddAbnormalInfo(ABNORMAL_FASTRUN);
	if( m_dwMindShockTime != 0 )		AddAbnormalInfo(ABNORMAL_MINDSHOCK);
	if( m_dwMindGuardTime != 0 )		AddAbnormalInfo(ABNORMAL_MINDGUARD);
	if( m_dwPsiShieldTime != 0 )		AddAbnormalInfo(ABNORMAL_PSISHIELD);
	if( m_dwBigShieldTime != 0 )		AddAbnormalInfo(ABNORMAL_BIGSHIELD);
	if( m_dwPiercingShieldTime != 0 )	AddAbnormalInfo(ABNORMAL_PIERCING_SHIELD);

	if( m_dwAdamantineTime != 0 )	AddAbnormalInfo(ABNORMAL_ADAMANTINE);
	if( m_dwMightyWeaponTime != 0 )	AddAbnormalInfo(ABNORMAL_MIGHTYWEAPON);
	if( m_dwBerserkerTime != 0 )	AddAbnormalInfo(ABNORMAL_BERSERKER);

	if( m_dwHiExpTime != 0)			AddAbnormalInfo(ABNORMAL_HIEXP);
	if( m_dwMagicFindTime != 0)		AddAbnormalInfo(ABNORMAL_MAGICFIND);
	if( m_dwNoChatTime != 0)		AddAbnormalInfo(ABNORMAL_NOCHAT);

}

BOOL USER::ReturnTown(BYTE type, BYTE slot)
{
	int x, y;
	int isZone = -1;
	int iPotalIndex = -1;
	BYTE result = SUCCESS;
	BYTE zoneChange = 0;
	CPoint ptTown, pt;
	
	CBufferEx TempBuf;

	pt = GetTownPotal( isZone );
	x = pt.x;
	y = pt.y;

	if( isZone < 0 ) // �ƴϸ� ���� �������� �̵�
	{
		SendTownPotal( FAIL, zoneChange );
		return TRUE;
	}

	if( IsZoneInThisServer( isZone ) )	// �̵��Ϸ��� ���� ���� �����ȿ� ������
	{
		TownPotal();
		return TRUE;
	}

	if( slot < EQUIP_ITEM_NUM || slot >= TOTAL_INVEN_MAX )
	{
		SendTownPotal( FAIL, zoneChange );
		return TRUE;
	} 

	int sSid = m_UserItem[slot].sSid;

	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) 
	{
		SendTownPotal( FAIL, zoneChange );
		return TRUE;
	}

	if( m_iCurWeight <= g_arItemTable[sSid]->m_byWeight ) m_iCurWeight = 0;
	else m_iCurWeight -= g_arItemTable[sSid]->m_byWeight;		// ����ŭ ���Ը� ���δ�.

	m_UserItem[slot].sCount -= 1;
	if(m_UserItem[slot].sCount <= 0) { ReSetItemSlot(&m_UserItem[slot]); sSid = -1; }

	UpdateUserItemDN();							// ������ ���� üũ...

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

	ZoneMoveReq( isZone, x, y );
	return FALSE;
}

BOOL USER::GetAbsentGuildInfo(int guildsid)
{
	int i;

	CGuild* pNewGuild = NULL;

	pNewGuild = GetGuild( guildsid );

	if( pNewGuild )
	{
		if( (int)pNewGuild->m_lSid == guildsid )
		{
			ReleaseGuild();
			return TRUE;
		}
	}

	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[2048];

	::ZeroMemory(szSQL, sizeof(szSQL));

	wsprintf(szSQL,TEXT("SELECT * FROM GUILD where iSid = %d"), guildsid);
	
	SQLUINTEGER		iDN;//, iEXP;
	SQLINTEGER		iSID;
	SQLSMALLINT		sVERSION;
	SQLCHAR			strGuildName[CHAR_NAME_LENGTH + 1], strMasterName[CHAR_NAME_LENGTH + 1];
	SQLCHAR			strItem[_BANK_DB], strMark[GUILD_MARK_SIZE], strHaveMap[GUILD_MAP_COUNT];

	SQLINTEGER		sInd;

	iSID = 0;
	iDN = 0;//iEXP = 0;
	sVERSION = 0;

	g_CurrentGuildCount = 0;			// ���� ����� ����� ������ ��ȣ ����

	::ZeroMemory(strGuildName, sizeof(strGuildName));
	::ZeroMemory(strMasterName, sizeof(strMasterName));
	::ZeroMemory(strMark, sizeof(strMark));
	::ZeroMemory(strHaveMap, sizeof(strHaveMap));
	::ZeroMemory(strItem, sizeof(strItem));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB )
	{
		ReleaseGuild();
		return FALSE;
	}

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Load Guild Data !!\n");
		ReleaseGuild();
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS);

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while (TRUE)
		{
			retcode = SQLFetch(hstmt);

			if (retcode ==SQL_SUCCESS || retcode ==SQL_SUCCESS_WITH_INFO)
			{
				i = 1;

				SQLGetData( hstmt, i++, SQL_C_SLONG, &iSID, sizeof(iSID), &sInd );
				SQLGetData( hstmt, i++, SQL_C_CHAR, &strGuildName, sizeof(strGuildName), &sInd );
				SQLGetData( hstmt, i++, SQL_C_CHAR, &strMasterName, sizeof(strMasterName), &sInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY, &strMark, sizeof(strMark), &sInd );
				SQLGetData( hstmt, i++, SQL_C_SSHORT, &sVERSION, sizeof(sVERSION), &sInd );
				SQLGetData( hstmt, i++, SQL_C_ULONG, &iDN, sizeof(iDN), &sInd );

				SQLGetData( hstmt, i++, SQL_C_BINARY, &strItem,	sizeof(strItem), &sInd );
				SQLGetData( hstmt, i++, SQL_C_BINARY, &strHaveMap, sizeof(strHaveMap), &sInd );

				if(iSID <= 0 || iSID >= MAX_GUILD ) 
				{
					TRACE(_T("��� ��ü ũ�Ⱑ MAX_GUILD�� �Ѿ���."));
					continue;
				}

				pNewGuild = NULL;
				pNewGuild = g_arGuildData[iSID];

				pNewGuild->m_lSid = iSID;
				strcpy(pNewGuild->m_strGuildName, (CHAR*)strGuildName);
				strcpy(pNewGuild->m_strMasterName, (CHAR*)strMasterName);
				pNewGuild->m_sVersion = sVERSION;
				pNewGuild->m_dwGuildDN = iDN;

				pNewGuild->StrToGuildItem((LPTSTR)strItem);
				pNewGuild->strToGuildMap((LPTSTR)strHaveMap);
				::CopyMemory(pNewGuild->m_strMark, strMark, sizeof(pNewGuild->m_strMark));

				g_CurrentGuildCount = iSID;

				::ZeroMemory(strGuildName, sizeof(strGuildName));
				::ZeroMemory(strMasterName, sizeof(strMasterName));
				::ZeroMemory(strMark, sizeof(strMark));
				::ZeroMemory(strHaveMap, sizeof(strHaveMap));
				::ZeroMemory(strItem, sizeof(strItem));
			}
			else break;
		}		
	}
	else if (retcode==SQL_NO_DATA)
	{
		g_DB[m_iModSid].ReleaseDB(db_index);
		ReleaseGuild();
		return FALSE;
	}
	else
	{
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		ReleaseGuild();
		return FALSE;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	GetAbsentGuildUserInfo( guildsid );

	ReleaseGuild();

	return TRUE;	
}

void USER::GetAbsentGuildUserInfo(int guildsid)
{
	int i;
	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[2048];

	::ZeroMemory(szSQL, sizeof(szSQL));
	wsprintf(szSQL,TEXT("SELECT * FROM GUILD_USER where iSid = %d"), guildsid);

	SQLINTEGER		iSID;
	SQLCHAR			strUserId[CHAR_NAME_LENGTH + 1];

	SQLINTEGER		sInd;

	iSID = 0;
	::ZeroMemory(strUserId, sizeof(strUserId));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Load Guild User Data !!\n");
		return;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS);
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		while (TRUE)
		{
			retcode = SQLFetch(hstmt);

			if (retcode ==SQL_SUCCESS || retcode ==SQL_SUCCESS_WITH_INFO)
			{
				i = 1;

				SQLGetData( hstmt, i++, SQL_C_SLONG, &iSID, sizeof(iSID), &sInd );
				SQLGetData( hstmt, i++, SQL_C_CHAR, &strUserId, sizeof(strUserId), &sInd );

				if(iSID < 0 || iSID >= MAX_GUILD ) continue;

				if(!g_arGuildData[iSID]) continue;

				g_arGuildData[iSID]->AddUser((TCHAR *)strUserId, iSID);
			}
			else break;
		}		
	}
	else if (retcode==SQL_NO_DATA)
	{
		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}
	else
	{
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	return;
}

BOOL USER::IsMyDBServer(int myserver)
{
	for( int i = 0; i < g_arMyServer.GetSize(); i++ )
	{
		
		if( g_arMyServer[i] )
		{
			TRACE( "g_arMyServer: %d\n", *(g_arMyServer[i]));
			if( *(g_arMyServer[i]) == myserver )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
//��Ϸ��������
void USER::BridgeServerUserLogin(char *strAccount, char *strUserID)
{
	if( g_bShutDown ) return;

	int			index = 0;
	BYTE		result = FAIL, error_code = 0;
	TCHAR		szID[CHAR_NAME_LENGTH+1];
	CPoint		pt;
	CBufferEx	TempBuf;
	int			iMemory = 0;
	int			iMemoryAccountBank = 0;

	strcpy( szID, strUserID );

	InitUser();
	iMemory = CheckMemoryDB( szID );
//	iMemory = 0;		// �Ƶ� ���� �͵� ������, ���� ���� �͵� ������.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ����� ������.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ��͵� �־���.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 2;		// �Ƶڶ� ������ ���� ���� ������, ������ ���� ���� �־���.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�. (���߿� ��â�� MemoryDB���� �������� ��ƾ�� �������̴�.)

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserData( szID ) )
		{
			//AfxMessageBox("User Data Load Fail");
			error_code = ERR_2;
			goto result_send;
		}
	}

	m_state = STATE_CONNECTED;

	// ���� �� �ִ� ���� �ξⷳ�� ���̰� �������� �ƴ� ����� �Ӽ��̶��
#ifndef _EVENT_RR 
	if( m_curz == g_RR.m_iZoneNum && CheckInvalidMapType() == 12 )
	{
		m_curz = 57;
		SetZoneIndex( m_curz );

		pt = ConvertToServer( 20, 34 );
		m_curx = pt.x;
		m_cury = pt.y;
	}
#else		// �ӽ÷� �ְ� �̺�Ʈ������
	if( m_curz == g_RR.m_iZoneNum || m_curz == 61 || m_curz == 62 || m_curz ==  63)
	{
		if( CheckInvalidMapType() == 12 )
		{
			m_curz = 57;
			SetZoneIndex( m_curz );

			pt = ConvertToServer( 20, 34 );
			m_curx = pt.x;
			m_cury = pt.y;
		}
	}
#endif

	pt = FindNearAvailablePoint_S(m_curx, m_cury);	// DB�� ����� ��ǥ�� ������ �� �ִ� ��ǥ���� �Ǵ�
	if(pt.x == -1 || pt.y == -1) 
	{
		// TEST...
		//error_code = ERR_4;
		//goto result_send;
		m_curz = 1;
		m_curx = 192;
		m_cury = 803;

		if( !IsZoneInThisServer( m_curz ) )
		{
			UpdateUserData( TRUE );
			error_code = ERR_4;
			goto result_send;
		}

		SetZoneIndex(m_curz);
		pt = FindNearAvailablePoint_S(m_curx, m_cury);	// DB�� ����� ��ǥ�� ������ �� �ִ� ��ǥ���� �Ǵ�
	}

	m_curx = pt.x; m_cury = pt.y;
	SetUid(m_curx, m_cury, m_uid + USER_BAND );		// ���� ��ġ���� ����
	m_presx = -1;
	m_presy = -1;

	pt = ConvertToClient(m_curx, m_cury);

//	SetUserToMagicUser();							// ���� �ɷ�ġ�� ���� ���������� ���Ѵ�.
	GetMagicItemSetting();							// ���� �������� ���� �Ӽ�, ���������� ���� ������ �ݿ��Ѵ�.

	m_UserFlag = TRUE;

	result = SUCCESS;

	m_ConnectionSuccessTick = 0;

	InitMemoryDB(m_uid);

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
		if( !LoadAccountBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
	}

result_send:
	TempBuf.Add(GAME_START_RESULT);
	TempBuf.Add(result);

	if(result != SUCCESS)
	{
		ReInitMemoryDB();

		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
//		SockCloseProcess();
		return;
	}

					// ������ �� ������ ������.
	//��ӳ����źй���
	m_dwAbnormalInfo_ |= ABNORMAL_POISON;
	m_dwAbnormalInfo_ |=ABNORMAL_COLD;
	//m_dwAbnormalInfo_=0x00000001;
	TempBuf.Add((short)m_curz);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);

	TempBuf.Add(m_tDir);

	TempBuf.Add((BYTE)g_GameTime);
	TempBuf.Add((BYTE)g_GameMinute);
	Send(TempBuf, TempBuf.GetLength());
	SendCharData();		
	
	

	SetGameStartInfo();

	CheckUserLevel();								// ����ü�跹���� 25������ �Ѿ��� ���

	SendSystemMsg( IDS_USER_OPERATOR_MAIL1, SYSTEM_NORMAL, TO_ME);
	SendSystemMsg( IDS_USER_OPERATOR_MAIL2, SYSTEM_NORMAL, TO_ME);

	GetLocalTime(&m_LoginTime);		// �α����� �ð��� ����
	GetCheckValidTime();			// �ð��� ǥ���Ѵ�.
	CBufferEx	TempBuf1;
	TempBuf1.Add((byte)68);
	TempBuf1.Add(m_iSkin);
	TempBuf1.Add(m_iHair);
	TempBuf1.Add((BYTE)m_sGender);
	TempBuf1.AddData(m_strFace, 10);
	Send(TempBuf1, TempBuf1.GetLength());
	
}

void USER::BridgeServerUserZoneLogin(char *strAccount, char *strUserID)
{
	if( g_bShutDown ) return;

	int			index = 0;
	BYTE		result = FAIL, error_code = 0;
	TCHAR		szID[CHAR_NAME_LENGTH+1];
	CPoint		pt;
	CBufferEx	TempBuf;
	int			iMemory = 0;
	int			iMemoryAccountBank = 0;

	strcpy( szID, strUserID );

	InitUser();
//	InitMemoryDB(m_uid);
	iMemory = CheckMemoryDB( szID );
//	iMemory = 0;		// �Ƶ� ���� �͵� ������, ���� ���� �͵� ������.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ����� ������.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ��͵� �־���.
//						// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.

//	iMemory = 2;		// �Ƶڶ� ������ ���� ���� ������, ������ ���� ���� �־���.
//						// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�. (���߿� ��â�� MemoryDB���� �������� ��ƾ�� �������̴�.)

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserData( szID ) )
		{
			error_code = ERR_2;
			goto result_send;
		}
	}

	/*
	if( !IsZoneInThisServer(m_curz) )
	{
		ChangeServer(m_curz);
		
		ReInitMemoryDB();

		SoftClose();
		return;
	}
	*/

	m_state = STATE_CONNECTED;

	// zone, zoneindex�� ��ǥ ��ȯ...
//	ZoneChangeInfoSet(m_curz, m_curx, m_cury);

	pt = FindNearAvailablePoint_S(m_curx, m_cury);	// DB�� ����� ��ǥ�� ������ �� �ִ� ��ǥ���� �Ǵ�
	if(pt.x == -1 || pt.y == -1) 
	{
		error_code = ERR_4;
		goto result_send;
	}

//	CheckUserLevel();					// ����ü�跹���� 25������ �Ѿ��� ��� 

	m_curx = pt.x; m_cury = pt.y;
	SetUid(m_curx, m_cury, m_uid + USER_BAND );		// ���� ��ġ���� ����
	m_presx = -1;
	m_presy = -1;

	pt = ConvertToClient(m_curx, m_cury);

	GetMagicItemSetting();							// ���� �������� ���� �Ӽ�, ���������� ���� ������ �ݿ��Ѵ�.

	m_UserFlag = TRUE;

	result = SUCCESS;

	m_ConnectionSuccessTick = 0;


	InitMemoryDB(m_uid);

	if( iMemory == 0 || iMemory == 2 )
	{
		if( !LoadUserBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
		if( !LoadAccountBank() )
		{
			error_code = ERR_2;
			result = FAIL;
			goto result_send;
		}
	}

	// �� ü���� Ȯ�ι� ���� �ʱ�ȭ...
	m_strZoneIP = "";
	m_nZonePort	= -1;

	SetPsiAbnormalStatus();

result_send:
	TempBuf.Add(GAME_START_RESULT);
	TempBuf.Add(result);

	if(result != SUCCESS)
	{
		ReInitMemoryDB();

		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}
	
	SendCharData();						// ������ �� ������ ������.

	TempBuf.Add((short)m_curz);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);

	TempBuf.Add(m_tDir);

	TempBuf.Add((BYTE)g_GameTime);
	TempBuf.Add((BYTE)g_GameMinute);

	Send(TempBuf, TempBuf.GetLength());

	SetGameStartInfo();	

//	SendSystemMsg( IDS_USER_OPERATOR_MAIL1, SYSTEM_NORMAL, TO_ME);
//	SendSystemMsg( IDS_USER_OPERATOR_MAIL2, SYSTEM_NORMAL, TO_ME);

	GetLocalTime(&m_LoginTime);		// �α����� �ð��� ����
//	GetCheckValidTime();			// �ð��� ǥ���Ѵ�.
}

void USER::BridgeServerUserWhisper(char *strSendID, char *strRecvID, char *strMsg)
{
	if(m_tIsOP == 1)						// ��ڿ��� �Ӹ��� �ȵ�
	{
		return;
	}

	if( !strcmp( strSendID, strRecvID ) )	// �ڱ��ڽſ��� �Ӹ� -> ����
	{
		return;
	}

	CBufferEx TempBuf;

	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(WHISPER_CHAT);
	TempBuf.Add((int)0x01);								// Client���� ���� �����ϱ�����(���� �Ӹ� ���̵�)
	TempBuf.AddString(strSendID);
	TempBuf.AddString(strMsg);			// pBuf == IDLen(1) + ID + MsgLen(1) + Msg
	Send(TempBuf, TempBuf.GetLength());
}

void USER::BridgeServerGuildNewResult(int guildnum, char *strGuildName)
{
	BYTE error_code = 0;
	BOOL bRes = TRUE;

	int i;
	int index = 0;
	int nLength = 0;
	int iGuildId = 0;

	CBufferEx	TempBuf;

	WORD *pwMark = NULL;
	CGuild *pGuild = NULL;

	nLength = strlen( strGuildName );
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) { error_code = ERR_1; goto go_result; } // ��� ��Ī ���ѿ� �ɸ�
	if(!g_arGuildData[guildnum]) { error_code = ERR_1; goto go_result; }

	EnterCriticalSection( &(m_pCom->m_critGuild) );

	pGuild = g_arGuildData[guildnum];

	pGuild->m_lSid = guildnum;

	strcpy(pGuild->m_strGuildName, strGuildName);
	strcpy(pGuild->m_strMasterName, m_strUserID);

	pGuild->m_dwGuildDN = 0;
	pGuild->m_sVersion = -1;

	pwMark = (WORD*)(&pGuild->m_strMark[0]);
	for (i=0; i<GUILD_MARK_SIZE/sizeof(WORD); i++) {
		pwMark[i] = (0x1f<<11 | 0x1f);	// Client���� ����ϴ� ��������� �ʱ�ȭ�Ѵ�.
	}

	if(!InsertGuild(pGuild))
	{
		LeaveCriticalSection( &(m_pCom->m_critGuild) );
		return;
	}

	pGuild->AddUser( m_strUserID, guildnum );

	LeaveCriticalSection( &(m_pCom->m_critGuild) );

	if( m_dwDN <= GUILD_MAKE_DN ) m_dwDN = 0;
	else m_dwDN -= GUILD_MAKE_DN;		

	m_bGuildMaster = TRUE;
	m_dwGuild = guildnum;

	nLength = 0;
	nLength = strlen( strGuildName );
	if(nLength > 0 && nLength < CHAR_NAME_LENGTH)
	{
		strncpy(m_strGuildName, strGuildName, nLength);
	}

	bRes = FALSE;

go_result:
	//////////////////////////////PACKET///////////////////////////////////////
	TempBuf.Add(GUILD_OPEN_RESULT);

	if(bRes)
	{
		TempBuf.Add((BYTE)0x00);		// ����
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	TempBuf.Add((BYTE)0x01);		// ����
	TempBuf.Add((int)iGuildId);
	TempBuf.AddString(m_strGuildName);

	Send(TempBuf, TempBuf.GetLength());

	SendMoneyChanged();
	SendMyGuildInfo();//(TO_INSIGHT, INFO_MODIFY);
}

void USER::BridgeServerGuildDisperseResult(int guildnum)
{
	int i;//, j;
	int index = 0, nLen = 0;
	BYTE error_code = 0;

	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	CStore *pStore = NULL;
	CGuildFortress *pFort = NULL;

	if( guildnum <= 0 || guildnum >= g_arGuildData.GetSize() ) return;

	if( guildnum != (int)m_dwGuild ) return;

	pGuild = GetGuild( m_dwGuild );

	if( !DeleteGuildDB() ) 				// DB ����
	{
		ReleaseGuild();
		::InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, 0);
		return;
	}
										// ������ ���� �ߴٸ� �ʱ�ȭ
/*	for(i = 0; i < g_arStore.GetSize(); i++)
	{
		pStore = g_arStore[i];
		if(!pStore) continue;

		if(pStore->m_iGuildSid == m_dwGuild)
		{
			::ZeroMemory(pStore->m_strGuildName, CHAR_NAME_LENGTH + 1);
			::ZeroMemory(pStore->m_strMasterName, CHAR_NAME_LENGTH + 1);

			pStore->InitStoreInfo(-1);
			SetGuildStoreTex(pStore->m_sStoreID, 0);	// Tax Rate Init
			InitMemStore(pStore);
			break;
		}
	}
*/
	index = m_dwGuild;

	// ����Ͽ콺�� �ִٸ� �̿�Ұ��� ����� �ش�.
	for(i = 0; i < g_arGuildHouse.GetSize(); i++)
	{
		if(m_dwGuild == g_arGuildHouse[i]->iGuild)
		{
			CNpc *pNpc = NULL;				// ���е�...
			pNpc = GetNpc(g_arGuildHouse[i]->iMarkNpc);
			if(pNpc)
			{
				pNpc->m_sPid = 0;
				pNpc->m_sMaxHP = 1;
				::ZeroMemory(pNpc->m_strName, sizeof(pNpc->m_strName));
			}

			g_arGuildHouse[i]->iGuild = 0;
			break;
		}
	}

	// ����� �ִٸ� �̿�Ұ��� ����� �ش�.
	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i]) continue;

		pFort = g_arGuildFortress[i];

		if(pFort->m_iGuildSid == m_dwGuild)
		{
			if(pFort->m_lViolenceUsed == 1)
			{
				pFort->m_wMopPartyTime.wYear = 2030;		// ��尡 �ػ�Ǹ鼭 �������� ��� �ʱ�ȭ��
				pFort->SetNpcToFortressViolenceEnd(m_pCom);
				pFort->GetOutof(m_pCom);
			}

			pFort->SetInitFortress();
			pFort->InitMemFortress(GUILD_WAR_DECISION);
			break;
		}
	}

	g_arGuildData[m_dwGuild]->InitGuild();

	ReleaseGuild();

	for(i = 0; i < MAX_USER; i++)		// ������ ����
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;
		if(pUser->m_dwGuild <= 0) continue;

		if(pUser->m_dwGuild == index)
		{
			pUser->SendSystemMsg( IDS_USER_DISMISS_COMPLETED, SYSTEM_NORMAL, TO_ME);

			pUser->m_dwGuild = -1;							// �ʱ�ȭ�Ѵ�.
			pUser->m_sGuildVersion = -1;					// ��� ���� ����
			::ZeroMemory(pUser->m_strGuildName, sizeof(pUser->m_strGuildName));
			pUser->m_bGuildMaster = FALSE;					// ��� ������
			pUser->m_bRepresentationGuild = FALSE;			// ���� ���� ����

			pUser->m_bFieldWarApply = FALSE;
			pUser->m_tGuildWar = GUILD_WAR_AFFTER;
			pUser->m_tGuildHouseWar = GUILD_WAR_AFFTER;
			pUser->m_tFortressWar = GUILD_WAR_AFFTER;

			pUser->SendMyGuildInfo();
		}
	}
	// DB���� ����..
	// ������ �������� ����...
	// �켱 �޸𸮿��� ����... (Guild, Guild_House_Rank,

	UpdateUserData();
}

void USER::BridgeServerGuildInviteResult(int guid, char *strGuestID)
{
	int index = 0;
	BYTE error_code = 0;

	int nLen = 0;

	int uid = 0;
	int iCount = 0;
	USER* pUser = NULL;
	CGuild *pGuild = NULL;

	BOOL bRes = TRUE;

	uid = guid;

	pUser = GetUser( uid );

	if(!pUser) return;

	nLen = strlen(pUser->m_strUserID);
	if(nLen <= 0) return;

	if(pUser->m_dwGuild > 0) return;

	if(CheckInGuildWarring()) return;								// �����߿��� ����A������.

	pGuild = GetGuild( m_dwGuild );

	if(!pGuild)
	{
		ReleaseGuild();				// ����...
		return;
	}
									// ����...
//	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0)
	if(!pGuild->IsMasterPower(m_strUserID))
	{
		ReleaseGuild();				// ����...
		return;
	}

	index = -1;
	index = pGuild->GetUser( pUser->m_strUserID );
	if(index >= 0)
	{
		ReleaseGuild();				// ����...

		pUser->m_dwGuild = m_dwGuild;			// ��� ��ȣ�� �ٽ� �����Ѵ�.
		SendGuildInfo(pUser);

		error_code = ERR_9;
		goto go_result;
	}// �̹� �ٸ� ��忡 ������ ����

	if(!pGuild->AddUser(pUser->m_strUserID, pGuild->m_lSid))
	{
		ReleaseGuild();				// ����...
		error_code = ERR_11;			//
		goto go_result;
	}

	if(!InsertGuildUser(pGuild->m_lSid, pUser->m_strUserID)) // �ش� ��忡 ������ ����
	{
		pGuild->RemoveUser(pUser->m_strUserID);

		ReleaseGuild();				// ����...
		error_code = 255;			//
		goto go_result;
	}

	pUser->m_dwGuild = pGuild->m_lSid;
	pUser->m_bGuildMaster = FALSE;
	pUser->m_sGuildVersion = pGuild->m_sVersion;
	pUser->m_bRepresentationGuild = FALSE;
	strcpy(pUser->m_strGuildName, pGuild->m_strGuildName);
	bRes = FALSE;

go_result:
	if(bRes)
	{
		CBufferEx TempBuf;
		TempBuf.Add(CHAT_RESULT);
		TempBuf.Add((BYTE)0x00);		//����
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	ReleaseGuild();

	UpdateUserData();

	AddGuildUserInFortress(pUser);

	SendGuildInviteUser(pUser);			// ��� ����Ʈ�� �ְ� �ޱ�
	SendGuildInfo(pUser);				// �þ� ������ ��忡 ������ ����� �����ϱ� ������ ���̶�� �Ѵ�...
}

void USER::BridgeServerGuildOffResult(char *strReqID, char *strOffID)
{
	int index = 0;
	BYTE error_code = 0;
	CString strTemp;

	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	BOOL bRes = TRUE;

	if(m_dwGuild <= 0) return;

	if(m_bGuildMaster)	// ��û�� ����� �渶�϶�
	{
		if( !strcmp( strReqID, strOffID ) )	// ��û�� ���̵�� Ż���� ���̵� ���ٸ�... ������
		{
			return;
		}
	}

	if(m_tGuildHouseWar == GUILD_WARRING || m_tGuildWar == GUILD_WARRING || m_tFortressWar == GUILD_WARRING)
	{
		return;
	}

	if(CheckInGuildWarring()) return;								// �����߿��� ����A������.

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)
	{
		ReleaseGuild();				// ����...
		return;
	}

	index = -1;
	index = pGuild->GetUser( strOffID );

	if(index < 0)					// �ش� ��忡�� ������ ������...
	{
		ReleaseGuild();
		return;
	}

	if(!pGuild->RemoveUser( strOffID ))
	{
		ReleaseGuild();
		error_code = ERR_8;
		goto go_result;
	}

	if(!DeleteGuildUser(strOffID))		// ���̺��� �����.
	{
		pGuild->AddUser(strOffID, m_dwGuild);

		ReleaseGuild();
		error_code = 255;
		goto go_result;
	}

	if(pGuild->m_lSubMaster == 1)
	{
		if( !strcmp(pGuild->m_strSubMasterName, strOffID) )
		{
			CString strMsg = _T("");
			strMsg.Format(IDS_USER_GUILD_SUBMASTER_OFF, pGuild->m_strSubMasterName);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

			pGuild->RemoveSubGuildMaster();
		}
	}

	ReleaseGuild();
	bRes = FALSE;

go_result:
	if(bRes)
	{
		CBufferEx TempBuf;
		TempBuf.Add(CHAT_RESULT);
		TempBuf.Add((BYTE)0x00);
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	if( !strcmp( strReqID, strOffID ) )		// �ڰ� Ż��
	{
		SendGuildOffUser( this );				// �ٸ� �������� ���� Ż���� ����� �˸���.

		DelGuildUserInFortress( strOffID, m_dwGuild );

		m_dwGuild = -1;							// �ʱ�ȭ�Ѵ�.
		m_sGuildVersion = -1;					// ��� ���� ����
		::ZeroMemory(m_strGuildName, sizeof(m_strGuildName));
		//----------------------------------------------------
		//yskang 0.1 ��� Ż��� ȣĪ ����
		ZeroMemory(m_strLoveName, sizeof(m_strLoveName));
		ZeroMemory(m_pMD->m_strLoveName,sizeof(m_pMD->m_strLoveName));
		//----------------------------------------------------
		//ȣĪ�� �ٸ� ����鿡�� �˸���(���� ��ũ���� �ִ»���鿡 ���ؼ�?)
		CBufferEx TempBuf;
		TempBuf.Add(LOVE_NAME);
		TempBuf.Add(m_uid+USER_BAND);
		TempBuf.AddString(m_strLoveName);
		SendExactScreen(TempBuf, TempBuf.GetLength());
		//--------------------------------------------------------------
		m_bGuildMaster = FALSE;					// ��� ������
		m_bRepresentationGuild = FALSE;			// ���� ���� ����

		SendMyGuildInfo();						// ��� Ż�� �˸���.

		UpdateUserData();
	}
	else									// ���� �߹�
	{
		pUser = GetUser( strOffID );

		if( pUser )
		{
			if( pUser->m_state == STATE_GAMESTARTED )
			{
				pUser->m_dwGuild = -1;
				pUser->m_sGuildVersion = -1;		// ��� ���� ����
				::ZeroMemory(pUser->m_strGuildName, sizeof(pUser->m_strGuildName));
				//----------------------------------------------------
				//yskang 0.1 ��� Ż��� ȣĪ ����
				ZeroMemory(pUser->m_strLoveName, sizeof(pUser->m_strLoveName));
				ZeroMemory(pUser->m_pMD->m_strLoveName,sizeof(pUser->m_pMD->m_strLoveName));
				//ȣĪ�� �ٸ� ����鿡�� �˸���(���� ��ũ���� �ִ»���鿡 ���ؼ�?)
				CBufferEx TempBuf;
				TempBuf.Add(LOVE_NAME);
				TempBuf.Add(pUser->m_uid + USER_BAND);
				TempBuf.AddString(pUser->m_strLoveName);
				pUser->SendExactScreen(TempBuf, TempBuf.GetLength());
				//----------------------------------------------------
				pUser->m_bGuildMaster = FALSE;			// ��� ������
				pUser->m_bRepresentationGuild = FALSE;	// ���� ���� ����
				pUser->SendMyGuildInfo();
				pUser->SendSystemMsg( IDS_USER_KICKOUT_GUILD, SYSTEM_NORMAL, TO_ME);

				SendGuildOffUser( pUser );

				pUser->UpdateUserData();
			}
		}

		strTemp.Format( IDS_USER_KICKOUT_MEMBER, strOffID);
		SendSystemMsg((LPTSTR)(LPCTSTR)strTemp, SYSTEM_NORMAL, TO_ME);
	}
}

void USER::BridgeDeputeGuildMasterOfPowerResult(int guid)
{
	int index = 0;
	BYTE error_code = 0;

	int nLen = 0;

	int uid = 0;
	int iCount = 0;
	USER* pUser = NULL;
	CGuild *pGuild = NULL;

	BOOL bRes = TRUE;

	if(!m_bGuildMaster) return; 	// ��� ������ ����.

	uid = guid;

	pUser = GetUser( uid );

	if(!pUser || pUser->m_state != STATE_GAMESTARTED) return;	// ������ ����.

	nLen = strlen(pUser->m_strUserID);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;

	if(pUser->m_dwGuild != m_dwGuild) return;

	if(CheckInGuildWarring()) return;							// �����߿��� ����A������.

	CString strMsg = _T("");

	pGuild = GetGuild( m_dwGuild );

	if(!pGuild)
	{
		ReleaseGuild();				// ����...
		return;
	}
									// ����...
	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0)
	{
		ReleaseGuild();				// ����...
		return;
	}

	index = -1;
	index = pGuild->GetUser(pUser->m_strUserID);
	
	if(index < 0)
	{	
		ReleaseGuild();					// ����...
		error_code = ERR_8;			// 
		goto go_result;		
	}

	if(pGuild->CheckGuildSubMaster())
	{
		if(UpdateGuildSubMaster(pUser->m_strUserID, TRUE) <= 0)
		{
			pGuild->RemoveSubGuildMaster();
			ReleaseGuild();					// ����...
			return;
		}

		pGuild->SetSubGuildMaster(pUser->m_strUserID);
		ReleaseGuild();					// ����...

		pUser->m_bRepresentationGuild = TRUE;
		bRes = FALSE;
	}
	else
	{
		strMsg = _T("");
		strMsg.Format(IDS_USER_GUILD_SUBMASTER, pGuild->m_strSubMasterName);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		ReleaseGuild();				// ����...
		return;
	}

go_result:
	if(bRes)
	{
		CBufferEx TempBuf;
		TempBuf.Add(CHAT_RESULT);
		TempBuf.Add((BYTE)0x00);		//����
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	USER *pGUser = NULL;			// �޼����� �˸���.

	CBufferEx TempBuf;

	strMsg = _T("");
	strMsg.Format( IDS_USER_GUILD_SUBMASTER_ON, pUser->m_strUserID);

	for(int i = 0; i < MAX_USER; i++)
	{
		pGUser = m_pCom->GetUserUid(i);

		if(pGUser == NULL || pGUser->m_state != STATE_GAMESTARTED) continue;		

		if(m_dwGuild == pGUser->m_dwGuild)
		{
			pGUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
	}
}

void USER::BridgeRemoveGuildMasterOfPowerResult()
{
	int index = 0;
	int nLen = 0;

	int uid = 0;
	USER* pUser = NULL;
	CGuild *pGuild = NULL;

	if(!m_bGuildMaster && !m_bRepresentationGuild) return; 	// ��� ������ ����.

	TCHAR strName[CHAR_NAME_LENGTH + 1];
	::ZeroMemory(strName, sizeof(strName));

	BYTE error_code = 0;
	BOOL bRes = TRUE;

	if(CheckInGuildWarring()) return;// �����߿��� ����Ҽ�����.

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) 
	{
		ReleaseGuild();				// ����...
		error_code = ERR_7;			// �ش� ��尡 ����.
		goto go_result;				
	}
									// ����...
//	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0) 
	if( !pGuild->IsMasterPower(m_strUserID) )
	{
		ReleaseGuild();				// ����...
		error_code = ERR_10;		// 
		goto go_result;
	}

	nLen = 0;
	nLen = strlen(pGuild->m_strSubMasterName);
	if(pGuild->m_lSubMaster == 0 || (nLen <= 0 || nLen > CHAR_NAME_LENGTH) )		// ���� �α渶���ٰ� �˷��ش�.
	{
		SendSystemMsg( IDS_USER_GUILD_SUBMASTER_NOT, SYSTEM_ERROR, TO_ME);
		ReleaseGuild();				// ����...
		return;
	}

	if(UpdateGuildSubMaster(NULL, FALSE) <= 0)
	{
		ReleaseGuild();				// ����...
		return;
	}
	
	pUser = GetUser(pGuild->m_strSubMasterName);
	if(pUser && pUser->m_state == STATE_GAMESTARTED)
	{
		pUser->m_bRepresentationGuild = FALSE;
	}

	::CopyMemory(strName, pGuild->m_strSubMasterName, nLen);
	pGuild->RemoveSubGuildMaster();

	ReleaseGuild();				// ����...

	bRes = FALSE;

go_result:
	if(bRes)
	{
		CBufferEx TempBuf;
		TempBuf.Add(CHAT_RESULT);
		TempBuf.Add((BYTE)0x00);		//����
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	USER *pGUser = NULL;			// �޼����� �˸���.

	CBufferEx TempBuf;

	CString strMsg = _T("");
	strMsg.Format( IDS_USER_GUILD_SUBMASTER_OFF, strName);

	for(int i = 0; i < MAX_USER; i++)
	{
		pGUser = m_pCom->GetUserUid(i);

		if(pGUser == NULL || pGUser->m_state != STATE_GAMESTARTED) continue;		

		if(m_dwGuild == pGUser->m_dwGuild)
		{
			pGUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
	}		
}

void USER::BridgeServerGuildMarkResult(TCHAR *pGuildMark)
{
	CBufferEx TempBuf;

	BOOL bRes = TRUE;
	BYTE error_code = 0;

	CGuild *pGuild = NULL;

	if(!m_bGuildMaster) return;

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)
	{
		ReleaseGuild();
		return;
	}

	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0)
	{
		ReleaseGuild();
		return;
	}

	if(pGuild->m_sVersion >= 0) CheckMaxValue((short &)pGuild->m_sVersion, (short)1);
	else pGuild->m_sVersion += 1;

	if(!UpdateGuildMark(pGuildMark, pGuild))	// DB�� ���� ���и� ����..
	{
		ReleaseGuild();
		return;
	}

	m_sGuildVersion = pGuild->m_sVersion;	// ��� ���� ����

	::CopyMemory(pGuild->m_strMark, pGuildMark, GUILD_MARK_SIZE);

	bRes = FALSE;

	ReleaseGuild();				// ����...

	TempBuf.Add(GUILD_MARK_ADD_RESULT);

	TempBuf.Add((BYTE)0x01);				// ���ο� ������ ��ϵǾ����� ���� ����� ��� ���� ��ο��� ������ �ϳ�
	TempBuf.Add(pGuild->m_sVersion);		// �ƴ� ������ �������༭ Ʋ���� ��� ������ �ϳ�...
											// ������ ������ ������...
	Send(TempBuf, TempBuf.GetLength());

	SendMyGuildInfo();
											// �ش� ����Ͽ콺 ������ �����Ѵ�.
	for(int i = 0; i < g_arGuildHouse.GetSize(); i++)
	{
		if(pGuild->m_lSid == g_arGuildHouse[i]->iGuild)
		{
			int modify_index = 0;
			char modify_send[2048];

			CNpc *pNpc = NULL;
			pNpc = GetNpc(g_arGuildHouse[i]->iMarkNpc);
			if(!pNpc) return;

			modify_index = 0;
			pNpc->m_sMaxHP = m_sGuildVersion;
			::ZeroMemory(modify_send, sizeof(modify_send));
			pNpc->FillNpcInfo(modify_send, modify_index, INFO_MODIFY);
			pNpc->SendInsight(m_pCom, modify_send, modify_index);

			break;
		}
	}
}

void USER::BridgeServerUserRestartResult()
{
	if(m_bLogOut == TRUE) return;
	if(m_state != STATE_GAMESTARTED) return;	// �̿� STATE_CONNECTED�� �϶��� �ش� �ʱ�ȭ�� �޸𸮿������Ƿ� �����ؾ��Ѵ�.

	m_bLogOut = TRUE;

	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0)
	{									// �ʵ��� ���̸� �˷��ش�.
		if(m_bGuildMaster)
		{
			CString strMsg = _T("");
			strMsg.Format( IDS_USER_GUILD_DEFEAT, m_strGuildName);
			SendGuildWarFieldEnd((LPTSTR)(LPCTSTR)strMsg);// �׺�
		}
	}

	if(m_bNowBuddy == TRUE)				// �������̸� �뺸�Ѵ�.
	{
		for(int i = 0; i < MAX_BUDDY_USER_NUM; i++)
		{
			if(strcmp(m_MyBuddy[i].m_strUserID, m_strUserID) == 0) SendBuddyUserLeave(i);
		}
	}

	if(m_bNowTrading == TRUE)
	{
		BYTE result = 0x00;
		USER *pTradeUser = NULL;
		if(m_iTradeUid != -1)	pTradeUser = GetUser(m_iTradeUid - USER_BAND);

		if(pTradeUser != NULL)	pTradeUser->SendExchangeFail(result, (BYTE)0x05);
	}

	if(m_tGuildHouseWar == GUILD_WARRING) CheckGuildUserListInGuildHouseWar(); // �ٸ� �������� ���ϳ� üũ..

	// �ξⷳ�� ó��
	ExitRoyalRumble();

	if(m_bLive == USER_DEAD) m_sHP = m_sMaxHP;

	if(!UpdateMemBankDataOnly()) return;
	if(!UpdateMemAccountBankDataOnly()) return;
	if(!UpdateUserData(TRUE)) return;
	ReInitMemoryDB();

	// alisia
//	if(SendRestartLoginResult() == FALSE) return;
	int		index = 0;
	m_state = STATE_LOGOUT;

	index = 0;
	SetByte(m_TempBuf, RESTART_RESULT, index );
	SetByte(m_TempBuf, SUCCESS, index );
	Send(m_TempBuf, index);
	// alisia


	m_nHavePsiNum = 0;
	m_tIsOP = 0;
	MAP *pMap = NULL;

	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) goto go_result;

	pMap = g_zone[m_ZoneIndex];
	if( m_curx < 0 || m_curx >= pMap->m_sizeMap.cx ) goto go_result;
	if( m_cury < 0 || m_cury >= pMap->m_sizeMap.cy ) goto go_result;

	if( g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser == USER_BAND + m_uid ) // �ʿ��� �������.
		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);

go_result:
	::ZeroMemory(m_strUserID, sizeof(m_strUserID));

	SendMyInfo(TO_INSIGHT, INFO_DELETE);

//	m_state = STATE_GAMERESTART;		// ���� ���¸� �ٲ۴�.
}

void USER::BridgeServerUserZoneLogOutResult(int z, int x, int y)
{
	CPoint pt = ConvertToServerByZone( z, x, y );

	if( pt.x < 0 || pt.y < 0 ) return;

	int save_z = z;
	int save_x = pt.x;
	int save_y = pt.y;

	// ����Ÿ ����...
	TRACE("Zone Change Data Saved...\n");
	ZoneLogOut(save_z, save_x, save_y);
	//

	m_curz = z;
	m_curx = save_x;
	m_cury = save_y;

	CBufferEx TempBuf;
	BYTE result = FAIL;
	
	TempBuf.Add(ZONE_CHANGE_RESULT);

	result = SUCCESS_ZONE_CHANGE;
	TempBuf.Add(result);
	TempBuf.AddString((char *)LPCTSTR(m_strZoneIP));
	TempBuf.Add(m_nZonePort);

	Send(TempBuf, TempBuf.GetLength());
	return;
}

// ���� �ξ� ���� ���� ������ �ð����� �˻��Ѵ�.
BOOL USER::CheckRoyalRumbleEnterTime()
{
	return g_RR.CheckEnteringByTime();
}

// ���� �ξ� ���� �߰� ���� �������� �˻��Ѵ�.
BOOL USER::CheckRoyalRumbleEnterMaxUser()
{
	return g_RR.CheckEnteringByMaxUser();
}

void USER::ExitRoyalRumble()
{
	if( m_curz != g_RR.m_iZoneNum ) return;

	if(CheckInvalidMapType() != 12) return;		// �ο������� ����� �Ӽ��� ������ ������ �������.

	g_RR.Exit( this );
}


int USER::GetGuildMapIndex(int maptype)
{
	if(maptype < 0) return -1;

	for(int i = 0; i < g_arMapTable.GetSize(); i++)
	{
		if(g_arMapTable[i])
		{
			if( maptype == g_arMapTable[i]->m_sMapIndex ) return i;
		}
	}

	return -1;
}

void USER::GuildUserCallReq(TCHAR *pBuf)//yskang 0.2 ��Ʈ���� �渶�� ��� ��ȯ
{
	BOOL bOwnerFortress = FALSE;//��Ʈ������ �����ϰ� �ִ� ����ΰ� Ȯ���ϴ� ����
	BYTE result = FAIL;
	int index = 0;
	char szUserName[255];
	USER* pUser = NULL;
	CPoint pt, ptNew;

	if(!m_bGuildMaster)//�渶�� �ƴϸ� �����ع���
		return;

	for(int i = 0; i < g_arGuildFortress.GetSize(); i++)// ���� Fortress�� ������ ������� üũ�Ѵ�.
	{
		if(!g_arGuildFortress[i]) continue;

		if(CheckGuildHouseUser(g_arGuildFortress[i]->m_sFortressID)) 
		{
			bOwnerFortress = TRUE;//�����ϰ� �ִ�.
			break;
		}
	}
	if(!bOwnerFortress) //��Ʈ������ �����ϰ� ���� �ʴٸ� ����
		return;

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength == 0 || nLength > CHAR_NAME_LENGTH) return;		// �߸��� �������̵� 
	
	pUser = GetUser(szUserName);
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return; //���������� ������ ����
	
	if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;	// �ڱ��ڽ��� �ȵ�
	
//	pt = FindNearAvailablePoint_S(m_curx, m_cury);
//	if(pt.x <= -1 && pt.y <= -1) return;						// �渶 ������ �� ��ǥ�� ����

	if(pUser->m_curz != m_curz)									// ������ �渶�� �������� ���� ���� ���
	{
		if( !IsZoneInThisServer(pUser->m_curz) ) return;		// �� �����ȿ� �ִ����� �ƴ� ����

		ptNew = ConvertToClient(m_curx, m_cury);
		BOOL bSuccess = pUser->ZoneChangeProcess(m_curz, ptNew.x, ptNew.y);	//^^ Check ���
		
		if(bSuccess)
		{
//			pUser->SendZoneChange(bSuccess);
			pUser->SendWeatherInMoveZone();				// �̵� ���� ������ȭ�� �˸���.
		}
		return;
	}
	else
	{													// ���� ���̸� �̵�����...		
/*
		ptNew = ConvertToClient(pt.x, pt.y);
		if(ptNew.x == -1 || ptNew.y == -1) return;

		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pUser->m_curx][pUser->m_cury].m_lUser, 0);
		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + pUser->m_uid);
		pUser->m_curx = pt.x;
		pUser->m_cury = pt.y;
		
		result = SUCCESS;
*/
		pt = pUser->FindNearAvailablePoint_S( m_curx, m_cury );
		ptNew = ConvertToClient( pt.x, pt.y );

		if( ptNew.x == -1 || ptNew.y == -1 ) return;

		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pUser->m_curx][pUser->m_cury].m_lUser, 0);
		::InterlockedExchange(&g_zone[pUser->m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + pUser->m_uid);
		pUser->m_curx = pt.x;
		pUser->m_cury = pt.y;
		
		result = SUCCESS;
	}
	
	index = 0;
	SetByte(m_TempBuf, MOVE_CHAT_RESULT, index);
	SetByte(m_TempBuf, result, index);

	if(result == FAIL) 
	{
		Send(m_TempBuf, index);
		return;
	}

	SetInt(m_TempBuf, pUser->m_uid + USER_BAND, index);
	SetShort(m_TempBuf, ptNew.x, index);
	SetShort(m_TempBuf, ptNew.y, index);
	pUser->Send(m_TempBuf, index);	// �������Դ� ���ο� ��ǥ����...
	
	pUser->SendInsight(m_TempBuf, index);
	
	pUser->SightRecalc();
}



int USER::GetIntAddr(CString strPeerIp)//yskang 0.04 string�� �����Ǹ� int�� �����Ƿ�...
{
	int nRetAddr = 0;
	CString strTemp;
	BYTE *pByte = (BYTE *)&nRetAddr;
	int i = 0;
	while (i < strPeerIp.GetLength ())
	{
		if (strPeerIp[i] == '.')
		{
			strTemp = strPeerIp.Left (i);
			strPeerIp = strPeerIp.Right (strPeerIp.GetLength () - (i + 1));
			*pByte = (BYTE)atoi (strTemp);
			pByte++;
			i = 0;
			if (strPeerIp.Find ('.') == -1)
			{
				*pByte = (BYTE)atoi (strPeerIp);
				break;
			}
			continue;
		}
		i++;
	}	
	return nRetAddr;
}


TCHAR *USER::GetStrAddr(int nPeerIp, TCHAR *strPeerIp)
{
	//TCHAR strPeerIp[1024];
	ZeroMemory(strPeerIp,sizeof(strPeerIp));
	char buffer[20];
	for(int i=0; i<4; i++)
	{
		BYTE a = nPeerIp&0x000000ff;
		_itoa(a,buffer,10);
		strcat(strPeerIp,(const char*)buffer);
		if(i!=3)
			strcat(strPeerIp,".");
		nPeerIp = nPeerIp>>8;
	}
	return (TCHAR *)strPeerIp;
}

