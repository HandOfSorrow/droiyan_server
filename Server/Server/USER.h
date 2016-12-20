// USER.h: interface for the USER class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER_H__24E85DFA_7A9E_4707_8A6A_BDEB26C34932__INCLUDED_)
#define AFX_USER_H__24E85DFA_7A9E_4707_8A6A_BDEB26C34932__INCLUDED_

#include "Define.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OV.h"
#include "Npc.h"

#include "EVENT_DATA.h"
#include "LOGIC_ELSE.h"
#include "EXEC.h"

#include "Store.h"
#include "Guild.h"
#include "GuildUSer.h"

#include "SharedMemory.h"
#include "MemUser.h"
#include "Compress.h"
#include "Cbsocket.h"
#include "IOCPSocket.h"

#include "Serverinfo.h"
#include "JvCryption.h"

#define PSI_LEN				((sizeof(short)) + (sizeof(BYTE)))

//#define _SKILL_DB			100
//#define _ITEM_DB			600
//#define _PSI_DB			100
#define _HISTORY_DB			500
#define _BHISTORY_DB		1000

//#define _CHANGE_DB		4
//#define _QUICKITEM_DB		10

#define MAX_LEVEL_DN		2000		// ������ ��� ������ 01_4_24
#define MAX_LEVEL			200	// ������ ��� ������ 01_4_24
#define HEAD				0
#define BODY				1
#define RIGHT_HAND			4
#define LEFT_HAND			5
#define PANTS				8
#define SHOES				9

#define PA_MA_STATUS		1			// PA, MA ��ġ���� üũ
#define SKILL_STATUS		2			// ��ų���� ��ġ���� üũ

#define POISON_TIME			(60 * 1000)
#define CONFUSION_TIME		(30 * 1000)
#define PARALYSIS_TIME		(20 * 1000)
#define BLIND_TIME			(60 * 1000)
#define LIGHT_TIME			(2 * 1000)
#define FIRE_TIME			(60 * 1000)
#define COLD_TIME			(10 * 1000)

#define BANK_ITEM_INPUT		1
#define BANK_ITEM_OUTPUT	2	
#define BANK_DN_INPUT		3
#define BANK_DN_OUTPUT		4

#define GUILD_ITEM_INPUT	1
#define GUILD_ITEM_OUTPUT	2	
#define GUILD_DN_INPUT		3
#define GUILD_DN_OUTPUT		4

#define BUDDY_JOIN			1
#define BUDDY_LEAVE			2
#define BUDDY_CHANGE		3

#define USE_CLASS_TYPE		8			// DB�󿡼� ����Ҽ� �ִ� ���� ������(1 Ŭ������ �ִ� m_byClass��)

#define MAX_BUDDY_USER_NUM	8			//����������..

#define DIR_DOWN			0			// �� �����ִ� ������ �����Ѵ�.
#define	DIR_DOWNLEFT		1
#define DIR_LEFT			2
#define	DIR_UPLEFT			3
#define DIR_UP				4
#define DIR_UPRIGHT			5
#define DIR_RIGHT			6
#define	DIR_DOWNRIGHT		7

#define TOWN_POTAL_SIZE		6			// ������ �� ���� ������ (3�� * 2)
#define SEARCH_TOWN_POTAL_COUNT	40			// ��Ż ��ġ�� �ִ� 40�� ���� ã�� ��ȸ�� �ش�.

#define EVENT_HELP_BAND		600
#define COUNTERATTACK_MAX_NUM 8			// �ִ� 8���⿡�� �ݰ��� �Ͼ�������Ƿ�

#define CLIENT_WALK_TICK	500
#define CLIENT_RUN_TICK		350

#define ITEM_UPGRADE_COUNT	5			// ������ �����϶� �� ���� Ƚ��
#define ITEM_UPGRADE_VALUE	4			// ������ ���� ��ȭ�Ǵ� ���ɰ�

#define ADD_USER_LEVEL		70

#define TEL_MAX_COUNT		31
#define TEL_ADD_EDIT		1	
#define TEL_DEL_EDIT		2

#define CORE_BUFF_SIZE		6000

#define RIPEL_TOP			798
#define RIPEL_BOTTOM		799
#define RIPEL_LEFT			800
#define RIPEL_RIGHT			801
#define RIPEL_CREST			802

#define PSI_DAMAGE_DELAY	0			// ���ÿ� ������ ���̿��� �������� �����ϴ� �ð�
#define NO_DAMAGE_TIME		7000		// GameStart, Zone Change ������ �����ð�

#define HIEXP_TIME			(3600 * 2 * 1000)	// ����ġ 2�� ���ӽð� - 3 �ð�
#define MAGICFIND_TIME		(3600 * 2 * 1000)	// �������ε� ���ӽð� - 3 �ð�

const short	SID_RING_OF_LIFE		= 702;
const short SID_NECKLACE_OF_SHIELD	= 703;
const short SID_EARRING_OF_PROTECT	= 704;

class EVENT;
class CHyper;  
class COM;
class Item;
class CCircularBuffer;
class CVirtualRoom;

typedef  CTypedPtrArray <CPtrArray, Item*> ItemArray;

struct  SkillList
{
	short	sSid;
	BYTE	tLevel;
//	BYTE	tOnOff;
};

struct	PsionicList
{
	short sSid;
	BYTE  tOnOff;
};

struct ExchangeItem
{
	BYTE  bySlot;
	ItemList changedList;
};

struct ShopItem
{
	short  sSlot;
	DWORD  dwItemPrice;
	ItemList ShopList;
};

struct BuyItemList
{
	short sSid;
	short sNum;
};

struct BuddyList
{
	int		uid;								// ���� uid
	TCHAR	m_strUserID[CHAR_NAME_LENGTH + 1];		// ID	
};

struct CounterAttack
{
	DWORD	dwLastAttackTime;					// ���� �ݰݰ���� ��û�� �ð�
	int		iDamage;							// �ݰ� �����
	int		iNpcUid;							// �ش� ��
	long	lSid;								// �����忡���� ������ ����..
};

struct GuildUserList
{
	TCHAR	GuildUser[CHAR_NAME_LENGTH + 1];	// ���� ID 
	short	sLevel;								// �������� Level
};

struct TeleportList
{
	int		iSid;
	int		x;
	int		y;
	int		z;
	TCHAR	TelName[TEL_NAME_LENGTH + 1];
};

typedef CTypedPtrArray <CPtrArray, int*>			EventNumArray;
typedef CTypedPtrArray <CPtrArray, ExchangeItem*>	ExchangeArray;
typedef CTypedPtrArray <CPtrArray, ShopItem*>		ShopArray;
typedef CTypedPtrArray <CPtrArray, ItemList*>		ItemListArray;
typedef CTypedPtrArray <CPtrArray, ITEMLOG*>		ItemLogArray;

class USER : public CBSocket
{
public:
	USER();
	virtual ~USER();

public:
	TCHAR	m_RData[CORE_BUFF_SIZE+1];
	volatile DWORD m_Recv_value;
	volatile DWORD m_Send_value;

	T_KEY m_public_key;

	CJvCryption m_cryptfn;
	int m_isCryptionFlag;

	void SetPublicKey();

// zi_gi Session Time
public:
	BOOL m_bSessionLogOut;
	int	m_iDisplayType;			// �޼��� Ÿ��
	int	m_iValidTime;			// ��ȿ �ð�
	int m_iTimeInterval;		// �޼��� ����
	SYSTEMTIME m_validDate;		// ��ȿ �ð�
	SYSTEMTIME m_LoginTime;		//  

// IKING 2001.1.
public:
	CString				m_strZoneIP;
	int					m_nZonePort;
	int					m_iModSid;
	BOOL				m_UserFlag;
//	long				m_Recv_value;
	int					m_UserStatus;
	CRITICAL_SECTION	m_CS_ExchangeItem;
	CRITICAL_SECTION	m_CS_ShopItem;
	long				m_ConnectionSuccessTick;

public:
	void DBGameStartResult( char *pBuf, int s_index, int dlength );
	void DBAccountLoginResult(TCHAR *pBuf, int index, int dlength );
	void DBUpdateUserData(char *temp_buffer, int &temp_index );
	void LogOutWithDbServer();

	void SoftClose();
	int SocketDisConnect();

	void ChatReq(TCHAR* pBuf);
	void ChatReqWithLength(TCHAR *pBuf, int iLen);		// ���ڰ��� �޸��ش�.
	void RestartReq(TCHAR *pBuf);
	void GameStart(TCHAR* pBuf);
	void GuildDisperse(TCHAR *pBuf);
	void GuildOff(TCHAR *pBuf);
	void GuildKickOut(TCHAR *pBuf);
	void GuildWharehouseOpenReq();
	void GuildItemMoveReq(TCHAR *pBuf);

	void BBSDelete(TCHAR* pBuf);
	void BBSEdit(TCHAR *pBuf);
	void BBSWrite(TCHAR *pBuf);
	void BBSRead(TCHAR *pBuf);
	void BBSNext(TCHAR *pBuf);

	// zinee 02-11-15 
	void MailInfoReq( TCHAR* pBuf );
	void MailSendReq( TCHAR* pBuf );
	void MailListReq( TCHAR* pBuf );
	void MailContentReq( TCHAR* pBuf );
	void MailDeleteReq( TCHAR* pBuf );
	int  MailGetList( char* pBuf, int nStartIndex, int nCount, int& index );
	BYTE MailGetInfo( char* pBuf, int& index );

	void DeleteCharReq(TCHAR* pBuf);
//	void GuildInvite(TCHAR *pBuf);
	void GuildMarkAddReq(TCHAR *pBuf);
	BOOL DeleteGuildUser(TCHAR *strUserID);
	void NewGuild(TCHAR *pBuf);
	void NewCharReq(TCHAR* pBuf);
	void AccountLoginReq(TCHAR* pBuf);
	// IKING 2002.1.
	void ZoneLoginReq(TCHAR *pBuf);
	void ZoneLogOut(int save_z, int save_x, int save_y);
	//
	void StopAction();
	int AcceptProcess();
	int SockCloseProcess(int nError=0);
//	void RemoveReceivedData();

	virtual void OnClose(int nErrorCode);
	virtual void OnSend(int nErrorCode);

	int NoEncodeSend(int length, char *pBuf);
	void ProcessData(char *pBuf, int length);
//

// Member Variable
public:
	COM*		m_pCom;
	//SOCKET		m_socket;
	int			m_uid;
	BYTE		m_state;

	//OV			m_ovRecv;
	//OV			m_ovSend;
	//OV			m_ovClose;

	// Memory DB
	CSharedMemory*	m_pSharedMemory;
	CMemUser*		m_pMD;

	//
	//	Zone Changing 
	//
	BOOL		m_NowZoneChanging;			// �ٸ� ������ �̵��Ҷ� ������ ����Ʈ 
											// ������ �˷����ʿ����.
	//
	//	Cell Info
	//
	CPoint		m_ptCell;					// ���� Cell ��ġ
	int			m_nCellZone;

	//
	//	Send, Receive Buffer
	//
	//TCHAR		m_RecvBuf[RECV_BUF_SIZE];
	//TCHAR		m_SendBuf[SEND_BUF_SIZE];
	TCHAR		m_TempBuf[SEND_BUF_SIZE];

	//
	//	Account Data
	//
	int			m_iMyServer;
	TCHAR		m_strAccount[ACCOUNT_LENGTH + 1];		//�˺�
	int			m_nCharNum;								//��������
	TCHAR		m_strChar[3][CHAR_NAME_LENGTH + 1];		// ��������

	//
	//	Game Data
	//
	TCHAR		m_strUserID[CHAR_NAME_LENGTH + 1];			// ���̵�(ĳ���� �̸�)
	TCHAR		m_strTradeUser[CHAR_NAME_LENGTH + 1];		// �ŷ����� ĳ���� �̸�
	int			m_iTradeUid;
//	int			m_iTradeSlotNum;						// �������� �������� �÷������� �ش� �ŷ� ���Թ�ȣ 01_4_20


	//	
	//	NPC�� ���� �����ʿ��� �ݰݽ�ų ó��
	//													// �ִ� 8���� ������Ʈ�� ����		
	CounterAttack	m_CounterAttackList[COUNTERATTACK_MAX_NUM];


	//
	//	Item Info Data
	//
	int			m_ItemFieldInfoCount;					// 
	int			m_ItemFieldInfoIndex;
	TCHAR		m_ItemFieldInfoBuf[8192];

	ExchangeArray	m_arExchangeItem;	// ��ȯ�� ������ ���
	ShopArray		m_arShopItem;		// Personal Shop Item Array
	//
	//	UpdateUserData
	//
	DWORD		m_dwLastUpdateTime;		// ���������� ������Ʈ�� �ð�
	DWORD		m_dwLastSaveTime;		// Last User save time
	long		m_lRequireNum;

	DWORD		m_dwLastSpeedTime;		// ���ǵ����� �����ϱ�����
	DWORD		m_dwLastMoveAndRun;		// SP : ���������� ������ �ð�
	DWORD		m_dwLastAttackTime;		// ���� �ֱٿ� ������ �ð�
	DWORD		m_dwLastPsiAttack;		// ���� �ֱٿ� ���̿������� ������ �ð�
	DWORD		m_dwLastMaxAttack;      //�ϴ�ʹ�ñ�ɱ��ʱ��
	DWORD		m_dwLastPsiTime;		// ���� �ֱٿ� ���̿������� ������ �ð�
	DWORD		m_dwCastDelay;			// ���̿��� ĳ���� ������
	DWORD		m_dwLastTimeCount;		// ���� �ֱٿ� Ÿ�̸Ӹ� �� �ð�
	DWORD		m_dwLastAbnormalTime;	// �����̻��� �ɸ��ð�
	DWORD		m_dwLastHPTime;			// HP�� ȸ���� ������ �ð�

	DWORD		m_dwLastPsiDamagedTime;	// ���� �ֱٿ� ���̿��� �������� ���� �ð�

	DWORD		m_dwLastHasteTime;		// ���������� ���̽�Ʈ�� �ɸ��ð�
	DWORD		m_dwLastShieldTime;		// ���������� ���带 �� �ð�
	DWORD		m_dwLastBigShieldTime;
	DWORD		m_dwLastDexUpTime;		// ���������� DEX UP �� �� �ð�
	DWORD		m_dwLastMaxHPUpTime;	// ���������� MAX HP UP �� �� �ð�
	DWORD		m_dwLastAdamantineTime;	// ���������� Adamantine�� �� �ð�
	DWORD		m_dwLastMightyWeaponTime;//���������� MightyWeapon�� �� �ð�
	DWORD		m_dwLastBerserkerTime;	//���������� Berserker�� �� �ð�

	DWORD		m_dwBSTime;			//���ܱ��ʱ��
	DWORD		m_dwBSLaseTime;			// ���ܱ�Ŀ�ʼʱ��
	DWORD       m_ShouhuTime;
	DWORD       m_dwDNMoney;                 //Ԫ����
	DWORD       m_dwLastItem44Time;          //��ҩˮʱ��


	DWORD		m_dwHasteTime;			// Haste
	DWORD		m_dwShieldTime;			// Shield
	DWORD		m_dwBigShieldTime;	
	DWORD		m_dwDexUpTime;			// Dex Up
	DWORD		m_dwMaxHPUpTime;		// Max HP Up
	DWORD		m_dwAdamantineTime;		// Adamantine
	DWORD		m_dwMightyWeaponTime;	// Mighty Weapon
	DWORD		m_dwBerserkerTime;		// Berserker
	
	DWORD		m_dwLastFastRunTime;		// ���������� Fast Run �� �� �ð�
	DWORD		m_dwLastMindShockTime;		// ���������� Mind Shock �� �� �ð�
	DWORD		m_dwLastMindGuardTime;		// ���������� Mind Guard �� �� �ð�
	DWORD		m_dwLastPsiShieldTime;		// ���������� Psionic Shield �� �� �ð�
	DWORD		m_dwLastPiercingShieldTime;	// ���������� Piercing Shield �� �� �ð�

	DWORD		m_dwFastRunTime;			// Fast Run �� ���� ���� �ð�
	DWORD		m_dwMindShockTime;			// Mind Shock �� ���� ���� �ð�
	DWORD		m_dwMindGuardTime;			// Mind Guard �� ���� ���� �ð�
	DWORD		m_dwPsiShieldTime;			// Psionic Shield �� ���� ���� �ð�
	DWORD		m_dwPiercingShieldTime;		// Piercing Shield �� ���� ���� �ð�

	BYTE		m_tPsiOneKind;
	BYTE		m_tPsiTwoKind;
	BYTE		m_tPsiThreeKind;

	DWORD		m_dwPsiOneTime;
	DWORD		m_dwPsiTwoTime;
	DWORD		m_dwPsiThreeTime;

	DWORD		m_dwLastHiExpTime;		// ���������� ����ġ ������ ���� �ð�
	DWORD		m_dwLastMagicFindTime;	// ���������� �������� ������ ���� �ð�
	DWORD		m_dwLastNoChatTime;		// ���������� ä���� ���� �ð�

	DWORD		m_dwHiExpTime;			// ����ġ 2�� ������ ������ ���� �ð� 
	DWORD		m_dwMagicFindTime;		// �������� 5�� ������ ������ ���� �ð�
	DWORD		m_dwNoChatTime;			// ä���� ������ ���� �ð�

	DWORD		m_dwAbnormalInfo;		// �����̻� ����
	DWORD		m_dwAbnormalInfo_;		// �ڶ���״̬

	DWORD		m_dwNoDamageTime;		// Game Start, Zone Change �� �������� ���� �ʴ� �ð�
	DWORD		m_dwLastNoDamageTime;	// ���������� Game Start, Zone Change ���� �� �ð�
	
	DWORD		m_iHPIntervalTime;		// HP ȸ�� �ð� ����
	DWORD		m_iSPIntervalTime;		// SP ȸ�� �ð� ����
	DWORD		m_iPPIntervalTime;		// PP ȸ�� �ð� ����
	DWORD       m_ionlineTime;
	DWORD       m_dwShopTime;
	
	int			m_iHPRate;				// HP ȸ�� ����
	int			m_iPPRate;				// PP ȸ�� ����
	int			m_iSPRate;				// SP ȸ�� ����
	
	BOOL		m_bWhisper;				// �Ӹ� ���/�ź�

//	BOOL		m_bTradeMode;			// Trade ���/�ź�
	BOOL		m_bTradeWaiting;		// Trade ��û��
	BOOL		m_bNowTrading;			// ���� �ŷ���
	BOOL		m_bExchangeOk;			// ��ȯ�³�
	BOOL		m_bTradeItem;			// �������� �ϳ��� �÷�����...
	DWORD		m_TradeMoney;			// �ŷ� ���� �ݾ�

	BOOL		m_bPShopOpen;			// Personal shop open or closed
	BOOL		m_bPShopView;			// Personal shop being viewed
	int			m_iPShopViewuid;		// Personal shop view uid
	BOOL		m_bViewingAShop;		// User viewing a personal shop

	BOOL		m_bNoItemMove;			// ���׷��̵�, ����, �������� �������� �ű� �� ���� ����϶� ����

	BuyItemList	m_TradeItemNum[TOTAL_ITEM_NUM];

	BOOL		m_bLogOut;				// Logout ���ΰ�?
	BOOL		m_bZoneLogOut;			// ZoneLogOut ���ΰ�?
	int			m_presx;				// �þ� ���� x (���� �þ�)
	int			m_presy;				// �þ� ���� y (���� �þ�)

	//
	//	Buddy Data
	BOOL		m_bBuddyMode;			// Buddy ���/�ź�
	BOOL		m_bNowBuddy;			// ���� ������
	BOOL		m_bMakeBuddy;			// ���� �߱���
	BuddyList	m_MyBuddy[MAX_BUDDY_USER_NUM]; // ���� �Ҽ��ִ� ���� ������ ���

	//
	//	Guild Data
	short		m_sGuildVersion;		// ��� ���� ����
	TCHAR		m_strGuildName[CHAR_NAME_LENGTH + 1];	
	BOOL		m_bGuildMaster;			// ��� ������
	BOOL		m_bRepresentationGuild;	// ���� ���� ����	 
	ItemList	m_GuildItem[TOTAL_BANK_ITEM_NUM];// Guild Item DB
	DWORD		m_dwGuildDN;					// Guild DN DB
	GuildUserList m_MyGuildUser[MAX_GUILD_USER];

	BYTE		m_tFortressWar;			// �������϶�...
	BYTE		m_tGuildWar;			// ���� ������϶�...
	BYTE		m_tGuildHouseWar;		// War at Virtual Room �϶�...
	BYTE		m_tQuestWar;			// ����Ʈ�� �������϶�

	BOOL		m_bFieldWarApply;		// �ʵ����� ��û���ʸ� ����
	int			m_FieldWarGMUid;		// �ʵ����϶� ���� �渶 uid
	int			m_dwFieldWar;			// �ʵ��� �����̸� ����� ��� ��ȣ�� ���´�.
//	TCHAR		m_FieldWarGMaster[CHAR_NAME_LENGTH + 1];	// �ʵ����϶� ����� �渶 ���̵�
	//--yskang 0.1
	TCHAR		m_strLoveName[LOVE_NAME_LENGTH];//��� ����� ȣĪ�� �ο� �ϱ����� ����....

	//
	//	�ڷ���Ʈ ��ġ�� MAX��ŭ �����Ѵ�. 
	//
	TeleportList m_strTeleport[TEL_MAX_COUNT];
	long		m_lCopyUsed;				// 1 �̸� ���� ī��Ĩ�� ����Ǿ� �ִ� 
	int			m_CopyUid;
	BYTE		m_tIndex;
	TCHAR		m_strCopyUser[CHAR_NAME_LENGTH + 1];	

	//
	//	CityRank Data
	BOOL		m_bPkStatus;			// ��õ��� ī������������ ����
	DWORD		m_dwPkStatusTime;		// ī�������϶� ���� �ð��� ����
	int			m_iCityValue;			// ���� ���� ���Ⱚ
	short		m_sCityRank;			// �ù� ���
	short		m_sKillCount;			// PK�� Ƚ��

	//
	//  EVENT Data
	EventNumArray	m_arEventNum;		// ������ �̺�Ʈ ��ȣ

	//
	//  Item Weight Data
	int			m_iMaxWeight;			// ������ ����ִ� �ִ� ���Է� (con+str)*10 + �⺻���� + m_sLevel * 6;
	int			m_iCurWeight;			// ���� ����
	BOOL		m_bRefuse;				// ������ �ޱ� ���� �ɼ� TRUE = ����, FALSE = �ޱ� �³�  

	// ĳ���� �⺻ �Ӽ�
	BYTE		m_bLive;				// �׾���? ��Ҵ�?

	short		m_sSTR;					// ��
	short		m_sCON;					// �ǰ�
	short		m_sDEX;					// ��ø��
	short		m_sVOL;					// ����
	short		m_sWIS;					// ����
	
//	short		m_sAbilitySum;			// �ɷ�ġ �հ�

	DWORD		m_iSkin;				// �Ǻλ�
	DWORD		m_iHair;				// �Ӹ���
	short		m_sGender;				// ����
	TCHAR		m_strFace[10];			// �󱼸��

	int			m_curx;					// ���� X ��ǥ
	int			m_cury;					// ���� Y ��ǥ
	int			m_curz;					// ���� ��
	int			m_ZoneIndex;			// ���� ���� �ε���

	DWORD		m_dwBuddy;				// �����ȣ
	long		m_dwGuild;				// ����ȣ
//	DWORD		m_dwGuild;				// ����ȣ

	DWORD		m_dwExp;				// ��������ġ
	DWORD		m_dwExpNext;			// ���� ��������ġ	(����ġ���̺��� ��´�.)

	DWORD		m_dwLastDownExp;		// ���������� �׾����� ����߸� ����ġ

	short		m_sPA;					// PA Point
	short		m_sSkillPoint;			// Skill Point
	short		m_sSkillPoint_;
	
	DWORD		m_dwXP;					// X Point

	short		m_sMaxHP;				// �ִ� HP
	short		m_sHP;					// ���� HP
	short		m_sMaxPP;				// �ִ� PP
	short		m_sPP;					// ���� PP
	short		m_sMaxSP;				// �ִ� SP
	short		m_sSP;					// ���� SP

	DWORD		m_dwDN;					// ������

	short		m_sLevel;				// �迭����
	BYTE		m_byClass;				// Ŭ����

	TCHAR		m_strSkill[_SKILL_DB];	// User Skill DB
	TCHAR		m_strItem[_ITEM_DB];	// User Item DB
	TCHAR		m_strPsi[_PSI_DB];		// User Psionic DB

	short		m_sChangeClass[_CHANGE_DB];	// ������ Ŭ�������� ����

	BYTE		m_tAbnormalKind;
	DWORD		m_dwAbnormalTime;

	BYTE		m_tIsOP;					// ������� �Ǵ�
											// 0: Normal User
											// 1: Game Operator
											// 2: Server Operator

	// ĳ���� �⺻ �Ӽ� (Reroll ��)
	BYTE		m_sTempSTR;					// ��
	BYTE		m_sTempCON;					// �ǰ�
	BYTE		m_sTempDEX;					// ��ø��
	BYTE		m_sTempINT;					// ����
	BYTE		m_sTempVOL;					// ����
	BYTE		m_sTempWIS;					// ����

	BYTE		m_tTempClass;				// �ӽ� Ŭ����

	// ������ ���� �Ӽ�	
	short		m_DynamicUserData[MAGIC_COUNT];			// ���� �Ӽ��� ������� ��ȭ�Ǵ� �ɷ�ġ
	short		m_DynamicEBodyData[EBODY_MAGIC_COUNT];	// EBody �Ӽ��� ������� ��ȭ�Ǵ� �ɷ�ġ
	short       m_DynamicMagicItem[10];


//	BYTE		m_DynamicSkillInfo[TOTAL_SKILL_NUM];// ���� �Ӽ� ��ųǥ���������� m_UserSkill[]�� ��Ī���� ��ȯ 

	DWORD		m_dwSaintTime;

//-------------------------[ End DB ] ---------------------------------------------//

	int			m_nHavePsiNum;			// ���̿��� ������(�� �������� �ٸ�)
	BYTE		m_tDir;					// ������ ���� �ִ� ����( move(), Attack(), PsiAttack() : �Լ��� ����)
	
	SkillList	m_UserSkill[TOTAL_SKILL_NUM];
	SkillList	m_UserSkill_[12];
	BYTE		m_UserSkillInfo[TOTAL_SKILL_NUM];
	short		m_UserChangeSkillInfo;// �����Ұ�� �ٸ� �迭���� ��ų�� �������� �ش� ��ų �ε���

	ItemList	m_UserItem[TOTAL_ITEM_NUM];

	ItemList	m_UserBankItem[TOTAL_BANK_ITEM_NUM];// User Bank Item DB
	DWORD		m_dwBankDN;							// User Bank DN DB

	ItemList	m_AccountBankItem[TOTAL_ACCOUNT_BANK_ITEM_NUM];	//Account Bank Item DB
	DWORD		m_dwAccountBankDN;								//Account Bank DN DB

	char		m_PersonalShopName[SHOP_NAME_LENGTH];

	PsionicList	m_UserPsi[TOTAL_PSI_NUM];

	CCircularBuffer *m_pCB;
	CCircularBuffer *m_pCBCore;

	// Speed Hack Check �� ������
	int m_iTickCount;
	DWORD m_dwServerTickBasic;
	DWORD m_dwServerTick;
	DWORD m_dwClientTick;
    DWORD m_dLastCheckTick;
	DWORD m_dCheckTick;
    DWORD m_dTotCheckTick;

	// alisia
	CCompressManager	m_CompMng;
	int					m_CompCount;
	TCHAR				m_CompBuf[10240];
	int					m_iCompIndex;
	DWORD				m_dwSaintTimeCount;

	// User Dead�϶� �ٸ� �ʿ��� ���� ���ϵ���..
	long				m_lDeadUsed;

	// �Ҹ� �Ǽ��縮�� ������
	BOOL				m_bRingOfLife;
	BOOL				m_bNecklaceOfShield;
	BOOL				m_bEarringOfProtect;

	// Member Function
public:
	void SetFastRun(int iTime);
	BOOL SetMindShock(int iTime);
	void SetMindGuard(int iTime);
	void SetPsiShield(int iTime);
	void SetPiercingShield(int iTime);
	void QuestWindowOpenReq(TCHAR *pBuf);

	void SendAccessoriDuration(short sSid);
	void ClearAbnormalTime(DWORD dwType);
	BOOL SummonMonster(CString szName, CPoint pt);
	void GetResource();
	int FindItem(int sid);
	BOOL FindEvent(int event_num);
	void UserHaveEventDataToStr(TCHAR *pBuf);
	void StrToHaveEventData(TCHAR *pBuf);
	void InitEventSlot();
	void GetWideRangeAttack(int x, int y, int damage);
	int GetPsiAttackUp();
	BOOL UpdateUserStatus();
	void UpdateUser();
	BOOL UpdateUserItemDN();
	void DropItemFromDeadUser(BOOL bPK, int enemyrank = -1);
	DWORD GetItemCostByCityRank(int sid, int nRate);
	void GetCityRank();
	void IsChangeCityRank(int iMyCityRank, USER *pUser);
	void IsLegalDefence(USER *pUser);
	BOOL IsLegalDefence_isdead(USER *pUser); //yskang 0.7 ������� ��� ���� ���̸� ī�� �Ǵ°� ���� ���Ͽ� 
	BOOL CheckRunSpeedHack(DWORD checkTick, BOOL bTwo);
	BOOL CheckMoveSpeedHack(DWORD checkTick);
	void MopCallReq(TCHAR *pBuf);
	void SetCounterAttack(int uid, int damage);
	void SendMyWeaponChangeInfo();
	BOOL UpdateUserExchangeData(USER *pUser);
	void GetRecoverySpeed();
	int GetMaxDamage(BYTE tWeaponClass);
	void SendItemFieldInfoToMe();
	void AddItemFieldInfoToMe(ItemList *pItem, BYTE type, int x, int y);
	CPoint ConvertToServer(int x, int y);
	CPoint ConvertToServerByZone(int z, int x, int y);
	void SendToRange(char* pBuf, int index, int min_x, int min_y, int max_x, int max_y);
	void SendRangeInfoToMe(int min_x, int min_y, int max_x, int max_y, BYTE flag);
	void FillUserInfo(char* pBuf, int& index, BYTE flag);
	void SendUserInfoBySightChange(int dir_x, int dir_y, int prex, int prey);
	CPoint ConvertToClient(int x, int y);
	BOOL IsMovable_C(int x, int y);
	BOOL IsMovable_S(int x, int y);
	BOOL IsMovable_S(int zone, int x, int y);
	CPoint FindNearAvailablePoint_C(int x, int y);
	CPoint FindNearAvailablePoint_S(int x, int y);
	CPoint FindNearAvailablePoint_S(int zone,int x, int y);
	BOOL IsReservedID(char* szName);
	int GetFinalDamage(USER* pUser, int nInitDamage, BYTE tMyWeapon,BOOL &bIsCritical,int max=0);//yskang 0.3
	void IsDeadUser();
	BOOL Teleport(int xpos, int ypos);
	void SetHide(int iTime);
	void SetShield(int iTime);
	void SetBigShield(int iTime);
	void SetFireDamage();
	void SetHaste(int iTime);
	short GetSkillLevel(BYTE tWeaponClass, short sSid);
	int SendDamagedItem(int totalDamage);
	int GetCounterAttack();
	double GetIronSkill();
	void SetUserToMagicUser(int iMaxHpUp = 0);
	void CheckMagicItemMove();
	void HelpStatusOpenReq();
	void GetMagicItemSetting();
	void GiveItemToUser(TCHAR *pBuf);
	void GiveMoneyToUser(TCHAR *pBuf);
	BOOL IsThereUser(USER *pUser);
	int GetDirection(CPoint ptStart, CPoint ptEnd, int nDirCount = 8);
	BOOL IsCity();
	BOOL IsException(int iTargetID, BYTE tPsi, CPoint ptPsi);
	int IsPKZone(USER *pUser);
	BOOL ISUserInSight();
	BOOL SendRestartLoginResult();
	//void RestartReq(TCHAR *pBuf);
	void RestartReqWithThread(TCHAR *pBuf);
	void BuddyChat(TCHAR *pBuf);
	BOOL CheckBuddyMember(TCHAR *pID);
	void SendBuddyUserLeave(int iCount);
	void BuddyUserChange(TCHAR *pBuf, BYTE tType);
	void SetBuddyMode(TCHAR *pBuf);
	void SendBuddyUserChange(BYTE type, int iCount = 1);
	void BankOutPutDN(TCHAR *pBuf);
	void BankInPutDN(TCHAR *pBuf);
	void BankOutPut(TCHAR *pBuf);
	void BankInPut(TCHAR *pBuf);
	void BankItemMoveReq(TCHAR *pBuf);
	void InitUserBankItem();
	void MakeInintItem(TCHAR *pBuf, BYTE tClass);
	BOOL IsCanEquipItem(int iSlot);
	void SendUserTownPotal(USER *pUser, BYTE tMode	/*INFO_MODIFY*/);
	void SendMyTownPotal(BYTE towho, BYTE type);
	void InitUserItem();
	BOOL UpdateUserBank();
	void UserBankItemToStr(TCHAR *pBuf);
	void StrToUserBankItem(TCHAR *pBuf);
	BOOL LoadUserBank();
	void BankOpenReq();
	void SendAllCure(int iStoreID, int iCost);
	short GetNeedPP(BOOL* bSuccessSkill, BYTE tWeaponClass, short sSid);
	short GetHP();
	CPoint FindNearRandomPoint(int xpos, int ypos);
	void DeleteCellUidFromTownPotal();
	void AddCellUidFromTownPotal();
	void SendTownPotal(BYTE result, BYTE changeZone);
	void SendUserStatusSkill();
	void SetColdDamage();
	void SetDecBullNum(BYTE tSlot, short sDec);
	void UserTimer();
	BOOL CheckPsiCastingDelay(/*short sSid*/);
	void TownPotal();
	void GetMapMoneyInfo(DWORD dwDN, CPoint pt, int uid = -1);

//	void GetMapItemInfo(ItemListArray &arList, CPoint pt, int uid = -1, BYTE tMode = ITEM_INFO_MODIFY);
	void SendItemFieldInfo(BYTE type, BYTE towho, ItemList *pItem, int x, int y);

	void GiveItemToMap(TCHAR *pBuf, BOOL bChao = FALSE);
	void GiveMoneyToMap(TCHAR *pBuf);
	int CheckSkillClass(short sSid);
	void SendBBSError(BYTE tError);
	//yskang 0.3 void SendAttackSuccess(int tuid, CByteArray& arAction1, CByteArray& arAction2, short sHP, short sMaxHP);
	void SendAttackSuccess(int tuid, BOOL bIsCritical, short sHP, short sMaxHP);//yskang 0.3
	void SendAttackMiss(int tuid);
	void SendAttackFail(BYTE tErrorCode);
	void SendXP();
	void SendBullNum(BYTE tSlot);
	BOOL PsionicProcess(int nTarget, short sSid, CPoint pt, short sNeedPP);
	BOOL IsSuccessPsi(BOOL* bSuccessSkill, BYTE tWeaponClass, short sSid);
	int GetPsiRange(short sSid);
	int DecAttackPP(BOOL *bSuccessSkill, BYTE tWeaponClass, short sSid, BOOL bDec = TRUE);
	void SendPP(BOOL bLevelDown = FALSE);
	short GetPP();
	BOOL IsHavePsionic(BYTE tSid);
	int CheckPsiClass(short sSid);
	void BuyPsi(TCHAR *pBuf);
	CStore* GetPsiStore(int nStore);
	void HyperTextEvent(TCHAR *pBuf);
	void NpcEvent(TCHAR *pBuf);
	BYTE GetNpcClass(CNpc* pNpc);
	void SendHP();
	void SendDuration(BYTE tSlot, int iDamage);
	int GetItemFieldNum(int iNum);
	void ItemUseReq(TCHAR *pBuf);
	void SendPsiOpen(int nStore);
	void SendWeatherInMoveZone();
//	void SendWeatherToZone(BYTE tType, BYTE tOnOff);
	void UserBasicAbility();
	void UserStatusSkillUp(TCHAR *pBuf);
	void SendExp(int tType = 1);
	// TRUE�̸� ���������� ����
	void GetLevelDownExp(BOOL bPK = FALSE, int enemyrank = -1, BOOL bEventSummon = FALSE,TCHAR *strPKName = NULL);//yskang 0.8 dead_log�� �ڽ��� ���� ����ڸ� ����� ���Ͽ�
	void GetExp(DWORD NpcExp);
	void GetExpBuddy(int iNpcExp );
	void GetExpCommon( int iNpcExp );
	void SendPsiStoreOpen(int iStoreID);
	void ZoneMoveReq(int zone, int moveX, int moveY);
	BOOL LiveCity(int x, int y, BYTE type = INFO_MODIFY);
	BOOL ZoneChangeProcess(int zone, int moveX, int moveY);
	void SendZoneChange(BOOL bResult);
	short GetDecDuration(short sSid);
	short GetItemDefense(short sSid, int slot);
	int GetCriticalInitDamage(BOOL *bSuccessSkill, BYTE tWeaponClass);
	double GetCriticalHit(BYTE tWeaponClass,int max);
//	BOOL GetCriticalHit(BOOL* bSuccessSkill, BYTE tWeaponClass);
//	int GetWeaponDamage(short sSid);
	int GetWeaponDamage(short sSid, BOOL bMax = FALSE);
//	int GetNormalInitDamage(BOOL* bSuccessSkill, BYTE tWeaponClass);
//yskang 0.3	int GetNormalInitDamage(BYTE tWeaponClass, USER* pTarget, BOOL bMax = FALSE);
	int GetNormalInitDamage(BYTE tWeaponClass, USER* pTarget,BOOL &bIsCritical, BOOL bMax = FALSE);
	double GetCriticalGuard(BOOL *bSuccessSkill, BYTE& tWeaponClass);
//	BOOL GetCriticalGuard(BOOL *bSuccessSkill, BYTE& tWeaponClass);
//	int GetAvoid(BYTE tClass);
	int GetAvoid();
	int GetHitRate(short sSid, BYTE tArmClass);
	int GetAttackRange(short sSid);
	int DecAttackSP(BOOL* bSuccessSkill, BYTE tWeaponClass);
	int GetMaxDuration(short sSid);
	void SendQuickChange();
	void ItemLoadReq(TCHAR *pBuf);
	void ItemGiveReq(TCHAR *pBuf);
	void SendSroreFail(int iType, BYTE err_code);
	int GetBreakDec();
	int GetBreakRate(short sSid);
	BOOL CheckAttackDelay(BOOL *bSuccess, BYTE tWeaponClass);
	void SendSP(BOOL bLevelDown = FALSE);
	short DecSP(short sDec);
	short GetSP(void);
	void RepairItem(TCHAR *pBuf);
	DWORD GetRepairCost(short sSlot);
	void RepairItemOpenReq(int iStoreID);
	void SellItem(TCHAR *pBuf);
	BOOL IsCanUseWeaponSkill(BYTE& tWeaponClass);
	void SwapItem(int nSlot1, int nSlot2);
	int GetEmptyInvenSlot(int *pEmpty);
	void UpdateInvenSlot(CWordArray *arEmptySlot,CWordArray *arSameSlot = NULL, BYTE tType = 3);

	DWORD GetSellCost(short sSlot);
	void SellItemOpenReq(int sStoreID);
	void UserInfoReq(TCHAR* pBuf);
	void SendMoneyChanged(void);
	void ReSetItemSlot(ItemList *pItem);
	void UserRemoteCallReq(TCHAR *pBuf);
	void UserCallReq(TCHAR *pBuf);
	void ChangeDir(TCHAR* pBuf);
	void SetWeather(int tType, int tOnOff);
	void SendExchangeItem(USER* pUser, BYTE bySlot, DWORD dwCount);
	void SendExchangeFail(BYTE result, BYTE error_code);
	void PsiAttack(TCHAR* pBuf);
	void MopCatchReq(TCHAR* pBuf);
//	void SendTradeSuccess(int uid);
	void SendTradeSuccess(int uid, TCHAR *pstrID);
	void SendTradeFail(int uid, BYTE error_code);
	void UserSkillInfoToStr(TCHAR* pBuf);
	void UserPsiToStr(TCHAR* pBuf);
	void StrToUserPsi(TCHAR* pBuf);
	void UserSkillToStr(TCHAR* pBuf);
	void StrToUserSkill(TCHAR* pBuf);
	void UserItemToStr(TCHAR* pBuf);
	void StrToUserItem(TCHAR* pBuf);
	void SendSystemMsg(TCHAR *pMsg, BYTE type, int nWho);
	void GetEquipPos(BYTE byWear, CPoint& pt);
	int GetEBodyPos(BYTE byWear);
	int GetSameItem(ItemList Item, int nSlot);
	int GetEmptySlot(int nSlot);
	int GetSlotClass(int nSlot, int nType = 0, BOOL flag = TRUE);
	BYTE GetSkillRate(BYTE byClass, int sid, int nDefusion = 0);
	void GetSkillInfo(int sid, BYTE& tRate);
	DWORD GetNextLevel(int iCurrLevel);
	DWORD GetNextPAMAExp(DWORD dwExp);
	void MakeEmptyPsi(TCHAR *pBuf);
	void MakeEmptySkill(TCHAR* pBuf, BYTE tClass);
	BOOL IsDoubleAccount(char* account);
	void SendRemainSight(TCHAR* pBuf, int nLength, CPoint pt);
	BOOL GetDistance(int xpos, int ypos, int dist, int* ret = NULL);

//	void ChangeCellUid(CPoint ptOld, CPoint ptNew);
//	void DeleteCellUid();
//	void AddCellUid();
//	BOOL SetCellIndex(int zone, int xpos, int ypos);

	void LiveReq(TCHAR *pBuf, BYTE tOption = 0);
	void RevivalReq( TCHAR *pBuf );
	void SendPsiAttackResult(BYTE result, int tuid, BYTE psi);
	int GetAttack();
	void SetDamage(int nDamage);
	int GetDefense(short* sItemDefense = NULL);
	void SendNpcInfo(CNpc* pNpc, BYTE tMode = INFO_MODIFY);
	CNpc* GetNpc(int nid);
	CNpc* GetNpc(TCHAR* pNpcName);
	void SendRange(TCHAR *pBuf, int nLength, CRect rect);
	void SendScreen(TCHAR *pBuf, int nLength);
	void SendInsight(TCHAR* pBuf, int nLength);
	void SendExactScreen(TCHAR* pBuf, int nLength);
	void SendZone(TCHAR* pBuf, int nLength);
	void SendAll(TCHAR* pBuf, int nLength);
	void SendMyInfo(BYTE towho, BYTE type);
	void SendCharData();
	void SendUserInfo(USER *pUser, BYTE tMode = INFO_MODIFY);
//	void SendUserInfo(int nUid, BYTE tMode = INFO_MODIFY);

	BOOL		InitUser();
	int			Init( int bufCreateFlag = 0 );
	void		Send(TCHAR* pBuf, int nLength);
	void		LogOut();

	//void		Close();
	//void		Receive();

	// Inline Function
	USER*	GetUser(int uid);
	USER*	GetUser(TCHAR* id);
	int		GetUid(int x, int y);
	BOOL	SetUid(int x, int y, int id);
	void Attack(TCHAR* pBuf);

	void	AddAbnormalInfo(DWORD dwAbnormal);
	void	DeleteAbnormalInfo(DWORD dwAbnormal);
	BOOL	CheckAbnormalInfo(DWORD dwAbnormal);
	void	ClearAbnormalGroup(DWORD dwAbnormal);

	BOOL UpdateUserData(BOOL bLogOut = FALSE);

protected:

	void TransformReq(TCHAR* pBuf);
	void TransformWindow( );
	BOOL GetTransformOption(ItemList* pItem);
	void PersonalShopBuy(TCHAR* pBuf);
	void ViewPersonalShop(BYTE mode, int uid);
	void PersonalShopOpen();
	void PersonalShopClose();
	void PersonalShopReq(TCHAR* pBuf);
	BOOL GivePSI(BYTE tType, int iTime);
	void TogglePKButton(TCHAR* pBuf);
	void SendHelperCure();
	void ExpressionReq(TCHAR* pBuf);
	void ExpressionOpen();
	void ScreenCaptureReq();
	void EBodyIdentifyReq(TCHAR* pBuf);
	void EBodyIdentifyOpen(int iStore);
	void EBodyUpgradeReq(TCHAR* pBuf);
	BOOL CheckClassWear(int iItemSid, int iMagicSid);
	BOOL GetMagicOption(ItemList* pItem, BYTE tIQ, BYTE tSame = FALSE);
	void RemagicItemReq(TCHAR* pBuf, BOOL bEvent = FALSE);
	BOOL InvaderSummon(BYTE tSlot);
	void SetPsiOne(BYTE tKind, DWORD dwTime);
	void SetPsiTwo(BYTE tKind, DWORD dwTime);
	void SetPsiThree(BYTE tKind, DWORD dwTime);

	BOOL UsePsiStone(short sSlot);
	void CheckIDReq(TCHAR* pBuf);
	void RemodelingItemReq(TCHAR* pBuf);
	BOOL RemodelingItem(int iStage, ItemList* pItem, short sPlanzing = 0);	
	BOOL MoonEvent(short sSlot);
	BYTE UseSpecialPotion(short sSlot);

	BOOL CheckRecoverTableByClass();
	void ShowCurrentUser(void);
	void SetMaxHPUp(int iTime);
	void SetDexUp(int iTime);
	void CheckSpeedHack();
	void UpdateBankMem2DB(CMemUser *pMD);
	void UpdateMem2DB(CMemUser *pMD);
	void UpdateMemStoreWarType(CStore* pStore);
	void UpdateMemStoreTax(int iSid, int iRate);
	void InitMemStore(CStore* pStore);
	void UpdateMemStoreDN(int iStoreID, DWORD dwDN);
	BOOL CheckJuminNumber(LPCTSTR szAccount, LPCTSTR szJumin);
	BOOL UpdateMemBankDataOnly();
	BOOL LoadMemUserBank(void);
	void SetMemUserBank(LPCTSTR strItem);
	void IsSkillSuccess(BOOL* bSuccess, BYTE tWeaponClass);
	void PsiChangeReq(TCHAR* pBuf);
	void SkillOnOffReq(TCHAR* pBuf);
	void ChangeWeaponReq(TCHAR* pBuf);
	void BuyItem(TCHAR* pBuf);
	void SendStoreOpen(int nStore, int nRate);
	void SendNpcSay(CNpc* pNpc, int nChatNum);
	BOOL RunNpcEvent(CNpc *pNpc, EXEC *pExec);
	BOOL CheckEventLogic(EVENT_DATA *pEventData);
	void ClientEvent(TCHAR* pBuf);
	void RunExchange(USER* pUser);
	void ExchangeReq(TCHAR *pBuf);
	void ItemTradeReq(TCHAR* pBuf);
	void SendItemMoveFail();
	void ItemMoveReq(TCHAR* pBuf);
	void SetTradeMode(TCHAR *pBuf);
	BOOL GetSkillName(int nSid, CString& strName);
	BOOL GetSkillNameLevel(BYTE byClass, TCHAR* pSkill, TCHAR* pBuf);
	BOOL GetEquipItemPid(TCHAR* pItemBuf, TCHAR* pBuf);
	void RerollReq(TCHAR *pBuf);
	void MoveChatReq(TCHAR* pBuf, BOOL bPsi = FALSE);
//	CPoint FindNearAvailablePoint(int xpos, int ypos);
	void SetGameStartInfo();
	void Dead();
/*	void BBSDelete(TCHAR* pBuf);
	void BBSEdit(TCHAR *pBuf);
	void BBSWrite(TCHAR *pBuf);
	void BBSRead(TCHAR *pBuf);
	void BBSNext(TCHAR *pBuf);
*/
	void BBSDeleteWithThread(TCHAR* pBuf);
	void BBSEditWithThread(TCHAR *pBuf);
	void BBSWriteWithThread(TCHAR *pBuf);
	void BBSReadWithThread(TCHAR *pBuf);
	void BBSNextWithThread(TCHAR *pBuf);

	void BBSOpen(int bbsnum);
	void WhisperOnOff(TCHAR* pBuf);
	void FindUserReq(TCHAR* pBuf);
	void ShoutChat(TCHAR* pBuf);
	void WhisperChat(TCHAR* pBuf);
	void NormalChat(TCHAR* pBuf);//@@@@@@@@@@@@@@@@@@@@@@@@@2
//	void ChatReq(TCHAR* pBuf);
	void ChatReqWithThread(int length, TCHAR* pBuf);
	void RunEndReq(TCHAR* pBuf);
	void RunReq(TCHAR* pBuf);
	void RunFirstReq(TCHAR* pBuf);
	void MoveEndReq(TCHAR* pBuf);
	void MoveReq(TCHAR* pBuf);
	void SightRecalc();
	BOOL Move_C(int x, int y, int nDist = 1);
	void MoveFirstReq(TCHAR* pBuf);
//	BOOL UpdateUserData(BOOL bLogOut = FALSE);
	void SetZoneIndex(int zone);
	void ChangeServer(int zone);
	BOOL IsZoneInThisServer(int zone);
	BOOL LoadUserData(TCHAR *szID);
	//void GameStart(TCHAR* pBuf);
	void GameStartWithThread(TCHAR *pBuf);
	void GameStartWithDbServer(TCHAR *pBuf);

//	void DeleteCharReq(TCHAR* pBuf);
	void DeleteCharReqWithThread(TCHAR *pBuf);

	BOOL IsExistCharId(TCHAR* id);
	//void NewCharReq(TCHAR* pBuf);
	void NewCharReqWithThread(TCHAR *pBuf);

	BOOL SendCharInfo(TCHAR* strChar, TCHAR* pBuf, int& index);
	BOOL LoadCharData(TCHAR* id);
	//void AccountLoginReq(TCHAR* pBuf);
	void AccountLoginReqWithThread(TCHAR *pBuf);
	void AccountLoginReqWithDbServer(TCHAR *pBuf);

	void NewAccountReq(TCHAR* pBuf);
	BOOL CheckSessionLoginReq(TCHAR* strAccount, TCHAR* strPassword, int& nCount);
//	void DisplayErrorMsg(SQLHANDLE hstmt);
	void SessionLoginReq(TCHAR* pBuf);
//	void Parse(int len, TCHAR *pBuf);
//	BOOL PullOutCore();
//	BOOL USER::PullOutCore(char *&data, int &length);
	void Parse(int len, TCHAR *pBuf, BOOL &bEncryption);
	BOOL USER::PullOutCore(int &length, BOOL &bEncryption);

public:
	DWORD m_dwUserTimerRun;
	BOOL m_bMGame;//yskang 0.5 ������ ��������� �Ǵ�
	TCHAR *GetStrAddr(int nPeerIp, TCHAR *strPeerIp);//yskang 0.4
	int GetIntAddr(CString strPeerIp);
	void SendCharDataToOPUser(USER *pUser);
	BOOL CheckZoneWho(int zone, int passtype, int zonetype);
	void WriteOpratorLog(TCHAR *strContents,int nOption);//yskang 0.4 nOption 0:���Ӿ����� 1:��ȭ 2:������ �������� 3:PK����
	BYTE m_nGuildUserInfoType;
	void CheckQuestEventZoneWarEnd();
	BOOL ExcuteSingleQuestEvent(int time, int type, int sid, int count, int z, int x, int y);
	int SummonQuestMonster(int sid, int z, int x, int y);
	void SendMonsterSay(CNpc *pNpc, int iType, char *strSay);
	void RunQuestEvent(CNpc *pNpc, int iZone, int iEventNum);
	void GuildUserCallReq(TCHAR *pBuf);//yskang 0.2
	BOOL m_bPseudoString;//yskang 0.1
	//--yskang 0.1
	void InsertLoveName(TCHAR *pBuf);
	void LoveName(TCHAR *pBuf);
	int GetGuildMapIndex(int maptype);
	int GetItemIndex(int sid);
//	void UpdateUserDeadLog(DWORD minus, DWORD preExp);
	void UpdateUserDeadLog(DWORD minus, DWORD preExp, TCHAR *strPKName);
	void UpdateUserDeadLog(TCHAR *pBuf);
	BOOL CheckClassItem(BYTE tMyClass, BYTE tNeedClass);
	int SetBlessingUpgradeItem(short sSlot, int type);
	void UpgradeBlessingItemReq(TCHAR *pBuf, int type);
	void GuildInviteResult(TCHAR *pBuf);
	void SendGuildInviteReq(TCHAR *pBuf);
	void EncryptionStartReq(TCHAR *pBuf);
	void ExitRoyalRumble();
	BOOL CheckRoyalRumbleEnterMaxUser();
	BOOL CheckRoyalRumbleEnterTime();
	void RemoveGuildMasterOfPower(TCHAR *pBuf);
	int UpdateGuildSubMaster(TCHAR *strSubMaster, BOOL bPower);
	void DeputeGuildMasterOfPower(TCHAR *pBuf);

	void BridgeRemoveGuildMasterOfPowerResult();
	void BridgeDeputeGuildMasterOfPowerResult(int guid);
	void BridgeServerUserZoneLogOutResult(int z, int x, int y);
	void BridgeServerUserRestartResult();
	void BridgeServerGuildMarkResult(TCHAR* pGuildMark);
	void BridgeServerGuildOffResult(char* strReqID, char* strOffID);
	void BridgeServerGuildInviteResult(int guid, char* strGuestID);
	void BridgeServerGuildDisperseResult(int guildnum);
	void BridgeServerGuildNewResult(int guildnum, char* strGuildName);
	void BridgeServerUserWhisper(char* strSendID, char* strRecvID, char* strMsg);
	void BridgeServerUserZoneLogin(char* strAccount, char* strUserID);
	void BridgeServerUserLogin(char* strAccount, char* strUserID);
	BOOL IsMyDBServer(int myserver);
	void GetAbsentGuildUserInfo(int guildsid);
	BOOL GetAbsentGuildInfo(int guildsid);
	BOOL ReturnTown(BYTE type, BYTE slot);
	void SetPsiAbnormalStatus();
	BOOL CheckApplyGuildRun();
	void ApplyGuildRun();
	BOOL CheckOverGuildUserCount(int count);
	void ChangeUpgradeAcc();
	void SendSystemMsg(UINT strID, BYTE type, int nWho);
	int UpdateGameTimeForEvent(TCHAR *pstrSerial);
	void GiveEventGameTime(int item_index, int quality);
	void EventItemSerialForGameRoomWindowOpen();
	void RecvEventItemSerialForGameRoom(TCHAR *pBuf);
	int UpdateEventItemSerialForGameRoom(TCHAR *pstrSerial, TCHAR *pstrSocNum, int &item);

	BOOL CheckUserLevel();

	int UpdateEventItemSerial(TCHAR *pstrSerial, int &item);
	void EventItemSerialWindowOpen();
	void RecvEventItemSerial(TCHAR *pBuf);

	BOOL GiveMagicItem(int sid, int iCount);
	BOOL Mem2GameAccountBank(CMemUser* pMD);
	int CheckMemoryAccountBankDB(char* strAccount);
	BOOL Mem2Game(CMemUser *pMD);
	void MakeMoneyLog(DWORD dwAddMoney, int type, char* strEtc = NULL, DWORD dwChangeMoney = 0);
	int GetDropItemRankByAttackerCityRank(int enemyrank);
	int GetMinusExpRatioByCityRank(int enemyrank);
	void FlushItemLog(BOOL bSave);
	void MakeItemLog(ItemList* pItem, int type, char* strEtc = NULL);
	BOOL UpdateMemItemLog(ITEMLOG* pItemLog);
	BOOL CheckItemLog(ItemList* pItem);
	void WriteItemLog(ItemList* pItem, int type, char* strEtc = NULL);
	void SendEventMsg(char* pMsg);
	void Collect(TCHAR* pBuf);
	void RobDN(int dn);
	void GiveDN(int dn);
	BOOL CheckRandom(int rand);
	BOOL CheckMoneyMinMax(int min, int max);
	void AccountBankItemMoveReq(TCHAR* pBuf);
	void ResetOver100LevelSkill(int sLevel);
	int GetPsyPsyResist();
	int GetPsyAbsoluteDefense();
	int GetPsyAssault(bool bMax);
	void SetBerserker(int iTime);
	void SetMightyWeapon(int iTime);
	void SetAdamantine(int iTime);
	void SelectOver100Skill(TCHAR* pBuf);
	void AccountBankInPutDN(TCHAR* pBuf);
	void AccountBankOutPut(TCHAR* pBuf);
	void AccountBankInPut(TCHAR* pBuf);
	BOOL LoadAccountBank();
	void AccountBankOpenReq(int nStoreID);
	void InitAccountBankItem();
	BOOL UpdateMemAccountBankDataOnly();
	void StrToAccountBankItem(TCHAR* pBuf);
	BOOL LoadMemAccountBank();
	void SetMemAccountBank(LPCTSTR strItem);
	void UserAccountBankItemToStr(TCHAR* pBuf);
	void UpdateAccountBankMem2DB(CMemUser* pMD);
	void GetCheckValidTime();
	void UpdateCurrentUserTable();
	ZONEINFO* GetZoneInfo(int zone);
	BOOL CheckInvalidZoneInFreeFight(USER* pUser);
	void UserItemToStrForSend(TCHAR* pBuf);
	void ItemDataVersionCheck();
	void DressingWindowOpen();
	void DressingReq(TCHAR* pBuf);
	UINT64 GenerateItemSerial(ItemList* pItem);
	CStore* GetStore(int nStore);

	void OpenFortressDoor();
	BOOL CheckGuildWarArea();
	void DelGuildUserInFortress(TCHAR *strUserID, int iGuild);
	void AddGuildUserInFortress(USER *pUser);
	void CheckGuildUserInFortress();
	void UpdateMemAttackListNpc(int iSid);
	BOOL FindEventItem(int sid, int quality);
	BOOL EventRobItem(int item_index, int quality);
	void RecvAddress(TCHAR* pBuf);
	void AddressWindowOpen(int sid, int quality);
	BOOL IsInSight(int x, int y, int z);
	void SetTempStoreDN(DWORD dwDN, int sid);
	void UpdateHighEventLevelUser(short sLevel);
	void GuildUserAllInforChat();
	void SessionLogOut();
	BOOL CheckInvakidZoneChangeState(int type);
	void UpdateFortressTax(int iStoreID, DWORD dwCost);
	BOOL CheckInGuildWarring();
	void UpdateMemRepairNpc(int iSid);
	void SendPeopleSay(int sid);
	void SendRepairItem(int sid);
	void UpdateMemFortressWarType(CGuildFortress *pFort);
	BOOL UpdateGuildFortressWarTime(CGuildFortress *pFort);
	void SetFortressGuildWarScheme(TCHAR *pBuf, int nStore);
	void SetStoreGuildWarScheme(TCHAR *pBuf, int nStore);
	void CheckGuildWar();
	void SendFortressAttackGuildList(TCHAR *pBuf);
//	void GuildUserAllInforChat();
//	void SendGuildMsgForAnnounceFortressWarEnd(CGuildFortress *pFort);
	BOOL UpdateFortress(CGuildFortress *pFort);
	void StoppingTheFortressWar(CGuildFortress *pFort);
	void FortressMove(CNpc *pNpc, int sid, int x1, int y1, int x2, int y2);
//	void EndFortressWar();
	void BeginFortressWar();
	BOOL CheckInvalidZoneState(int type);
	void GuildFieldStoreWarApplyReq(int sid);
	void GuildFortressWarApplyReq(int sid);
	CGuildFortress* GetFortress(int nFort);
	void EditHyperFortressText(CGuildFortress *pFort, int nNum);
	BOOL GiveItem(int sid, int iCount);
	void GiveEventItem(int EventItemIndex, int tIQ, int ItemIndex, int Count);
	void ExecuteChatCommand(char* pBuf);
	void ExecuteChatCommand(char *pBuf, int ilength);		// ���ڰ��� �޸��Ѵ�.
	BOOL UpdateEventItem(int sid);
	int CheckEventItem(int &index, int iSlot, BOOL bPotion = FALSE);
	void EventUpgradeItemReq(TCHAR *pBuf);
	int IsMovableTel_S(int z, int x, int y);
	void DoubleCopyTel(int z, int x, int y, BOOL zone);
	void GuardBow(CNpc *pNpc);
	BOOL CheckUserCityValue(int min, int max);
	int CheckCopyChip();
	BOOL SendCopyTel(int z, int x, int y, BOOL bSuccess = FALSE);
	void TeleportCopyResult(TCHAR *pBuf);
	void TeleportCopyReq(TCHAR *pBuf);
	BOOL CheckHandyGate();
	void GetHanyGate(int slot, int sid);
	void ChangeUserSpeed(TCHAR *pBuf);
	void TelportEdit(TCHAR *pBuf);
	void InitTelList(TeleportList *strTel);
	void UserTelToStr(TCHAR *pBuf);
	void StrToUserTel(TCHAR *pBuf);
	void TeleportReq();
	BOOL CheckGuildHouseUser(int num);
	BOOL DeleteGuildDB();
	CPoint GetTownPotal(int &potal);
//	void CheckMemoryDB(char* strAccount, char* strUserId);
	int CheckMemoryDB(char* strUserId);
	void CheckInvalidGuildZone();
	void CheckGuildUserListInGuildHouseWar();
	void CheckGuildHouseWarEnd();
	void SendGuildHouseRank(int nNum, int iCity);
	int GetVirtualRoomNum(int zone);
	int GetCityNumForVirtualRoom(int zone);
	void MassZoneMove(TCHAR *pBuf);
	void SendGuildUserInfo(int zone);
	BOOL CheckLevel(int min, int max);
	BOOL MassZoneMove(int zone);
	BOOL CheckTerm(int start, int end, int say);
	BOOL CheckGuildMaster();
	DWORD ConvertCurTimeToSaveTime();
	void SendCityRank(int iCityRank);
	void SendPKCount();
	void AddXPUp(int iNum);
	void RobItem(int sid, int num);
	void AddMyEventNum(int seventnum);
	void DelMyEventNum(int seventnum);
	EVENT* GetEventInCurrentZone();
	BOOL CheckClass(int iClass);
	BOOL CheckItemSlot(int say, LOGIC_ELSE* pLE);
	void AddRangeInfoToMe(CNpc* pNpc, BYTE tMode);
	void AddRangeInfoToMe(USER* pUser, BYTE tMode);
	void SendCompressedRangeInfoToMe();
	int MakeRangeInfoToMe(USER* pUser, BYTE tMode, TCHAR *pData);
	int MakeRangeInfoToMe(CNpc* pNpc, BYTE tMode, TCHAR *pData);
	BOOL CheckItemWeight(int say, int iSid1, int iNum1, int iSid2, int iNum2, int iSid3, int iNum3, int iSid4, int iNum4, int iSid5, int iNum5);
	BOOL CheckInvalidZoneInGuildWar(USER *pUser);

	void AnnounceNotice(TCHAR *pBuf);
	void CheckServerTest();
	void SendGuildWarFieldEnd(TCHAR *pBuf);
	void SendGuildFieldWarBegin();
	void SendGuildWarFieldApplyResult(TCHAR *pBuf);
	void SendGuildWarFieldApply(TCHAR *pBuf);
	BOOL UpdateGuildStoreWarTime(CStore *pStore);
	void SendServerTick();
	int CheckInvalidMapType();
	void UpdateMemStoreGuildList(CStore *pStore);
	void SendGuildMsgForAnnounceGuildWarEnd(CStore *pStore);
	void EndGuildWar(BYTE tWarType);
	void BeginGuildWar();
	void GuildWarApplyReq(TCHAR *pBuf);
	void GuildWarApply(int nStore);
	void SetGuildStoreTex(int iSid, int iRate);
	void SetGuildWar(BOOL nFlag);
	BOOL UpdateGuildStore(int nSid, TCHAR *strGuildMasterName);
	BOOL StoppingTheGuildWar(CStore *pStore);
	BOOL CheckGuildWarZone();
	int DayCalculation(int iYear,int iMonth, int iDay);
	void GetGuildWarScheme(TCHAR *pBuf);
	void GetStoreTax(TCHAR *pBuf);
	void SendGuildWarScheme(int nStore);
	void SendStoreTax(int nStore);
	void SendFieldGuildSay(CNpc *pNpc, int nStore, int say);
	void EditHyperText(CStore *pStore, int nNum);
	void SendFieldStoreOpen(int nStore, int nHave, int nEmpty);
	void GuildWharehouseCloseReq();
	void SendAllChat(TCHAR *pBuf);
//	void DelGuildUserIndex(USER *pUser);
	void SendGuildOffUser(USER *pUser);
//	int SetGuildUserIndex(USER *pUser);
	void SendGuildInviteUser(USER *pUser);
	void SetGuildVariable();
	void GuildChat(TCHAR *pBuf);
	int SetUpgeadeItem(short sSlot);
	void UpgradeItemReq(TCHAR *pBuf);
	void UpgradeAccessoriReq(TCHAR *pBuf);
	void UpgradeItemOpen(int event_num);
	void UpgradeItemReqBegin(TCHAR* pBuf);
	void GuildUserInfoReq(TCHAR *pBuf);
	void SetItemMode(TCHAR *pBuf);
	void GuildOpen();
	CStore* GetEbodyStore(int nStore);
	void EBodyBuyReq(TCHAR *pBuf);
	void SendEBodyOpen(int nStore);
	//void GuildKickOut(TCHAR *pBuf);
	void GuildKickOutWithThread(TCHAR *pBuf);
	void InitGuildItem();
	BOOL UpdateGuildWarehouse();
	void GuildOutPutDN(TCHAR *pBuf);
	void GuildInPutDN(TCHAR *pBuf);
	void GuildOutPut(TCHAR *pBuf);
	void GuildInPut(TCHAR *pBuf);
//	void GuildItemMoveReq(TCHAR *pBuf);
	void GuildItemMoveReqWithThread(TCHAR *pBuf);

	void GuildItemToStr(TCHAR *pBuf);
	void StrToGuildItem(TCHAR *pBuf);
	BOOL LoadGuildWarehouse();
//	void GuildWharehouseOpenReq();
	void GuildWharehouseOpenReqWithThread();

	//void GuildDisperse(TCHAR *pBuf);
	void GuildDisperseWithThread(TCHAR *pBuf);
	DWORD CheckLottery();
	void SendMyGuildInfo();
	void SendGuildInfo(USER *pUser);
//	BOOL DeleteGuildUser(TCHAR *strUserID);
	BOOL DeleteGuildUserWithThread(TCHAR *strUserID);

	void ReInitMemoryDB(void);
	BOOL UpdateBankDataOnly(void);
	BOOL UpdateMemUserBank(void);
	BOOL UpdateMemUserAll(BOOL bBank = FALSE);
	BOOL InitMemoryDB(int uid);
	//void GuildOff(TCHAR *pBuf);
	void GuildOffWithThread(TCHAR *pBuf);
	BOOL InsertGuildUser(int guildID, TCHAR *strUserID);
//	void GuildInvite(TCHAR *pBuf);
	void GuildInvite(int uid);
	void GuildInviteWithThread(TCHAR *pBuf);
	void SendItemWeightChange();
	void GuildAddReq(TCHAR *pBuf);
//	void GuildMarkAddReq(TCHAR *pBuf);
	void GuildMarkAddReqWithThread(TCHAR *pBuf);
	BOOL UpdateGuildMark(TCHAR *pMark, CGuild *pGuild);
	BOOL InsertGuild(CGuild *pGuild);
//	void NewGuild(TCHAR *pBuf);
	void NewGuildWithThread(TCHAR *pBuf);
	void ReleaseGuild();
	int GetEmptyGuildNum();
	BOOL GetGuildName(int guildnum, TCHAR* guildname);
	CGuild* GetGuildByName(TCHAR *guildname);
	CGuild* GetGuild(int num);
	
	void SendHideOff();
	CNpc* GetEventNpc();
	void GetNpcData(CNpc *pNpc, int x, int y);
	void init_encode(__int64 key2);
	void Encode_Decode(char* lpTarget, char* lpSource, int nLen,int f);
	void GetMagicItemSetting_38();
	void GetSkillSetting_130();
	int GetMagicItem_100(byte tMagic);
	void EBODY_Skill(short sSid,USER *pUser ,CNpc* pNpc,BOOL bIsUser);
	void RunSkill120(int Skill);
	BOOL FindSkill(int Skill_num);
	void AddSkill (int Skill_num);
	void	EventMove(int zone, int moveX, int moveY);
	void RemagicItem_100_1(TCHAR *pBuf);
	void RemagicItem_100_2(TCHAR *pBuf);
	void RemagicItem_100_3(TCHAR *pBuf);
	void RemagicItem_UpgradeReq(TCHAR *pBuf);
	void DownAccessoriReq(TCHAR *pBuf);
	void ItemExchange(TCHAR *pBuf);
	void RemagicItem1_UpgradeReq(TCHAR *pBuf);
	void RemagicItem_HuanShi(TCHAR *pBuf);
	void ItemConvert(TCHAR *pBuf);
	int FindItemSid(short sSid);
	void ShouHouUpgradeReq(TCHAR *pBuf);
	void ShouHouMake(TCHAR *pBuf);
	void ShouHouAdd(TCHAR *pBuf);
	short GetUserSpellAttack();
	short GetUserSpellDefence();
	void Face_painting(TCHAR *pBuf,int len);
	void DianGuangBan(TCHAR *pBuf,int len);
	void GiveMagic_30();
	void GiveAllItem(int sSid,int sCount,int iUp,int sIQ,int a1,int a2,int a3,int a4);
	int FindInventItem(int sid);
	short     m_SKill;//120����ת��ʱ�õ�.
	int  m_Hit;
	int m_Avoid;
	int m_ShowHP;
	short		m_sMagicSTR;
	short		m_sMagicCON;
	short		m_sMagicDEX;
	short		m_sMagicVOL;
	short		m_sMagicWIS;

	short		m_sMagicMaxHP;
	short		m_sMagicMaxPP;
	short		m_sMagicMaxSP;

	ItemLogArray m_arItemLog;

	int			m_server;
	char key[8];
};


#include "Extern.h"

inline int USER::GetUid(int x, int y )
{
	MAP* pMap = g_zone[m_ZoneIndex];
	return pMap->m_pMap[x][y].m_lUser;
}

inline BOOL USER::SetUid(int x, int y, int id)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;
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
	pMap->m_pMap[x][y].m_lUser = id;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	�����̻� ���� �ϳ��� �߰��Ѵ�.
//
inline void USER::AddAbnormalInfo(DWORD dwAbnormal)
{
	m_dwAbnormalInfo |= dwAbnormal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	�����̻� ���� �ϳ��� ���ش�.
//
inline void USER::DeleteAbnormalInfo(DWORD dwAbnormal)
{

	m_dwAbnormalInfo &= (~dwAbnormal);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	�ش� �����̻� ������ ���õǾ� �ִ��� Ȯ���Ѵ�.
//
inline BOOL USER::CheckAbnormalInfo(DWORD dwAbnormal)
{
	if((m_dwAbnormalInfo & dwAbnormal) == dwAbnormal) return TRUE;
	else return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	����, ���̿���100�� ����, ���̿��� 100�� ���� �׷��� �����̻� ������ Ŭ���� �Ѵ�.
//
inline void USER::ClearAbnormalGroup(DWORD dwAbnormal)
{
	m_dwAbnormalInfo &= dwAbnormal;
}

//-------------------------------------------------------------------------------//

#endif // !defined(AFX_USER_H__24E85DFA_7A9E_4707_8A6A_BDEB26C34932__INCLUDED_)

