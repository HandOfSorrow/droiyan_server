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
#include "ShopSystem.h"

extern ShopSystem g_Shop;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "Extern.h"		//	�ܺκ��� ����

extern CUserManager *g_pUQM;
extern CSearch *g_pUserList;
extern CServerDlg *g_pMainDlg;

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


extern CPtrList RecvSqlData;
extern CRITICAL_SECTION m_CS_SqlData;
extern long nSqlDataCount;	
extern int g_ChatEnable[];
extern  struct convert_table convert[80];

extern CHATDATAPACKET *g_WaitRecvDataChat[DATA_BUF_SIZE_FOR_THREAD+1][AUTOMATA_THREAD+1];
extern int g_nChatDataHead[];
extern int g_nChatDataTail[];

extern BYTE g_ServerId;
extern CRoyalRumble g_RR;

extern short		g_sHPConst[CLASS_NUM];
extern short		g_sPPConst[CLASS_NUM];
extern short		g_sSPConst[CLASS_NUM];

extern short		g_sHPLV[CLASS_NUM];
extern short		g_sPPLV[CLASS_NUM];
extern short		g_sSPLV[CLASS_NUM];

extern short		g_sHPAdd[CLASS_NUM];
extern short		g_sPPAdd[CLASS_NUM];
extern short		g_sSPAdd[CLASS_NUM];

extern TCHAR g_arServerIPAddr[16];
extern CString g_strARKRRWinner;

// Quest Event Class
extern CEventZone	g_QuestEventZone;

extern long	g_PotionViewOnOff;

const char* g_pszReservedID[] = 
{
	"��ī�罺",
	"Icarus",
	"�糪��",
	"Sanad",
	"G.O.D",
	"��",
	"Queen",
	"����",
	"Edin",
	"�ں񿡸�",
	"Zabier",
	"���ĵ��",
	"Elpadro",
	"�Ǹ�",
	"����",
	"Wes",
	"Guilty",
	"G-77",
	"��������",
	"Ganezia",
	"���ߵ���",
	"���",
	"Mono",
	"���Ϸ�",
	"Teiler",
	"��������",
	"Angeleter",
	"�������Ʈ",
	"Azrebest",
	"���丶��",
	"Automaton",
	"�����ϸ�Ʈ",
	"Ÿ���� ",
	"Tarsha",
	"�����ļ�",
	"�ʸ���",
	"Pilippo",
	"������Ƽ��",
	"��",
	"Vigsen",
	"���",
	"�������Ʈ",
	"krg",
	"krgsoft",
	"wizgate",
	"�������Ʈ����",
	"krg����",
	"�������Ʈ����",
	"krg����",
	"krgsoft����",
	"����̾�",
	"����̾���",
	"droiyan",
	"������",
	"����",
	"�츮��",
	"����ũ",
	"��ũ",
	"���̳�",
	"ũ����",
	"��Ƶ帰",
	"��",
	"Ǯ��",
	"���̳�",
	"������",
	"ī�̸�",
	"Ÿ�̷ν�",
	"�򸣳�",
	"����",
	"��糪",
	"�����",
	"����ÿ",
	"�ٸ��",
	"�����",
	"������",
	"�丣��",
	"��ÿ",
	"Ÿ���۽�",
	"���̾�������",
	"Wizgate����",
	"���̾���",
	"Wizgatesajang",
	"KRGsajang",
	"���̾�������Ʈ����",
	"���̾�������Ʈ",
	"���",

	"����Ű��",
	"��Ű��",
	"�Ƴ��˻�츣��",
	"���α�",
	"��ġī",
	"���-X",
	"������",
	"Ŭ������",
	"��ư�",
	"��Ű",
	"����-A1",
	"�׸� ��Ŀ",
	"��ǲ",
	"����",
	"��",
	"�׷���",
	"Ʈ�콺��츣��",
	"������Ͼ�",
	"D2",
	"Ŭ�����Ϳ��",
	"�ñ׸�-01",
	"��ī����",
	"���̾�Ʈ ��ǲ",
	"�ű׳ʽ�",
	"�ٷ�",
	"Ʈ��Ŀ",
	"ī�Ϸ�",
	"��Ű��",
	"������",
	"����",
	"Ŭ����",
	"�Ž�",
	"�ñ׸�-02",
	"�ٸ��",
	"��ũ��",
	"�̷���",
	"ī����",
	"�ٺ���",
	"G-����2F",
	"��Ű���Ʈ",
	"�ֻ糪",
	"������",
	"ī�̳�",
	"G-����",
	"������",
	"�ٸ���", 
	"���ν�",
	"G-����V",
	"Q-Q98",
	"��Ż����-N",
	"��Ż����-G",
	"��Ż����-S",
	"��Ż����-B",
	"Ÿ��ī",
	"�ڸḮ��",
	"�����ÿ�",
	"�Ž���",
	"����-F",
	"��ũ�þ�", 
	"�ڸḮ�", 
	"�ٷ���",
	"���̸� ��ǲ",
	"��Ƽ��",
	"���-X2",
	"G�ӽ���",
	"Ķ������",
	"���̷�",
	"�ڸ��",
	"���̳�",
	"�ƶ���ī",
	"��Ű�������",
	"Ʈī��",
	"����",
	"G-����2S",
	"��ī�̳�",
	"�����̶�",
	"Į����",
	"���� Mk-II",
	"�췹��", 
	"�غ����-N",
	"�غ����-G",
	"�غ����-S",
	"�غ����-B",
	"�ѽ�",
	"�츮��",
	"��ũ",
	"����ũ",
	"������",
	"����",
	"���",
	"����",
	"���̳�",
	"����ī",
	"���",
	"Į����",
	"���۽�",

	"������", 
	"������", 
	"������", 
	"������", 
	"��������", 
	"���ֻ���", 
	"��������", 
	"���ֻ���", 
	"���ֳ�", 
	"������", 
	"����", 
	"���۳�", 
	"���ۻ���", 
	"���ۼ���", 
	"������", 
	"����", 
	"���ȳ�", 
	"���η�", 
	"���ζ�", 
	"����", 
	"�û�", 
	"�û���", 
	"�û���", 
	"���ֱ���", 
	"���ޱ���", 
	"�ʽ���", 
	"�ʾ���", 
	"��ģ��", 
	"�����", 
	"��", 
	"��", 
	"��", 
	"��", 
	"��", 
	"������", 
	"������", 
	"�ʺ���", 
	"���ٸ�", 
	"�ʹٸ�", 
	"����", 
	"����", 
	"�ù�", 
	"����", 
	"�����ϳ�", 
	"�����ϳ�", 
	"����", 
	"����", 
	"�˻���", 
	"���ڽ�", 
	"���ڽ�", 
	"�˹�", 
	"����", 
	"�û�", 
	"����", 
	"����", 
	"����", 
	"�ɴ�", 
	"�ö���", 
	"�Žñ�", 
	"��", 
	"�ɤ�", 
	"�ɤ�", 
	"�ɤ�", 
	"����", 
	"����", 
	"����", 
	"�ù�", 
	"����", 
	"���Ծ��", 
	"������", 
	"����", 
	"����", 
	"��", 
	"����", 
	"����", 
	"����", 
	"���ڽ�", 
	"������", 
	"������", 
	"�ɰ�", 
	"�ױ��", 
	"�ϱ��", 
	"��������", 
	"������", 
	"���̹�", 
	"������",

	"���",
	"����",
	"����",
	"��ģ",
	"��ƾ",
	"����",
	"����",
	"����",
	"�Ϲ�",
	"���η�",
	"�ú�",
	"�ư���",
	"ȣ��",
	"�ֹ�",
	"�ֺ�",
	"������",
	"������",
	"����",
	"����",
	"�Խ�",
	"�Ծ�",
	"�Ԝ�",
	"�Խ�",
	"���",
};

//

void USER::AccountLoginReqWithDbServer(TCHAR *pBuf)
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

	if ( IsDoubleAccount(id) )
	{
		error_code = ERR_2;
		index = 0;
		SetByte(m_TempBuf, ACCOUNT_LOGIN_RESULT, index );
		SetByte(m_TempBuf, result, index );

		SetByte(m_TempBuf, error_code, index);
		Send( m_TempBuf, index );
		SoftClose();
		return;
	}

	strcpy( m_strAccount, id );

	char TempBuf[1024];
	index = 0;
	SetByte(TempBuf, g_ServerId, index );
	SetByte(TempBuf, ACCOUNT_LOGIN_REQ, index );
	SetShort(TempBuf, m_uid, index );
	SetString(TempBuf, id, ACCOUNT_LENGTH, index);
	g_pMainDlg->Send2DBsvr( TempBuf, index );

}

void USER::GameStartWithDbServer(TCHAR *pBuf)
{
	int			index = 0;
	BYTE		result = FAIL;
	TCHAR		szID[CHAR_NAME_LENGTH+1];

	int	nIDLength = GetVarString(sizeof(szID), szID, pBuf, index);
	if ( nIDLength == 0 || nIDLength > CHAR_NAME_LENGTH )
	{
		CBufferEx TempBuf;

		ReInitMemoryDB();

		TempBuf.Add(GAME_START_RESULT);
		TempBuf.Add(result);
		TempBuf.Add(ERR_1);
		Send(TempBuf, TempBuf.GetLength());
		SoftClose();
		return;
	}

	memcpy( m_strUserID, szID, CHAR_NAME_LENGTH );

	char TempBuf[1024];
	index = 0;
	SetByte(TempBuf, g_ServerId, index );
	SetByte(TempBuf, GAME_START_REQ, index );
	SetShort(TempBuf, m_uid, index );
	SetString(TempBuf, szID, CHAR_NAME_LENGTH, index);
	g_pMainDlg->Send2DBsvr( TempBuf, index );
}

void USER::LogOutWithDbServer()
{
	if(m_bLogOut == TRUE) return;
	if(m_state != STATE_GAMESTARTED) return;	// �̿� STATE_CONNECTED�� �϶��� �ش� �ʱ�ȭ�� �޸𸮿������Ƿ� �����ؾ��Ѵ�.

	m_bLogOut = TRUE;
	USER *pUser = NULL;
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

	{
	// 
	//UpdateMemBankDataOnly();
	//UpdateUserData(TRUE);
	char send_buffer[RECEIVE_BUF_SIZE+1];
	int send_index = 0, c_index = 0;;
	TCHAR strBankItem[_BANK_DB];

	SetByte(send_buffer, g_ServerId, send_index );
	SetByte( send_buffer, (BYTE)LOGOUT_REQ, send_index );
	SetShort( send_buffer, m_uid, send_index );

	// UpdateMemBankDataOnly() PART...
	::ZeroMemory(strBankItem, sizeof(strBankItem));
	::CopyMemory(strBankItem, m_pMD->m_UB.m_UserBankItem, _BANK_DB);

	c_index = send_index;

	SetString( send_buffer, m_strUserID, CHAR_NAME_LENGTH, send_index );
	SetDWORD( send_buffer, m_pMD->m_UB.m_dwBankDN, send_index );
	SetString( send_buffer, strBankItem, sizeof(strBankItem), send_index );
	//

	// UpdateUserData(TRUE) PART...
	DBUpdateUserData( send_buffer, send_index );
	//

	// �����ϱ�...
	m_CompMng.FlushAddData();
	m_CompMng.AddData( &send_buffer[c_index], (send_index-c_index) );

	m_CompMng.PreCompressWork();
	m_CompMng.Compress();

	int comp_data_len = m_CompMng.GetCompressedDataCount();
	int org_data_len = m_CompMng.GetUnCompressDataLength();
	DWORD crc_value = m_CompMng.GetCrcValue();
	char *packet_buffer;
	packet_buffer = m_CompMng.GetExtractedBufferPtr();

	SetShort( send_buffer, comp_data_len, c_index );
	SetShort( send_buffer, org_data_len, c_index );
	SetDWORD( send_buffer, crc_value, c_index );
	SetString( send_buffer, packet_buffer, comp_data_len, c_index );
	m_CompMng.FlushAddData();

	// ����.
	g_pMainDlg->Send2DBsvr( send_buffer, c_index );
	}

	m_pMD->m_UB.m_uid = -1;

	ReInitMemoryDB();

	if( g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser == USER_BAND + m_uid ) 
		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);

	SendMyInfo(TO_INSIGHT, INFO_DELETE);

	m_state = STATE_LOGOUT;
}

void USER::DBUpdateUserData(char *temp_buffer, int &temp_index )
{
	TCHAR			szSQL[8000];		
	TCHAR			strFace[10], strSkill[_SKILL_DB], strItem[_ITEM_DB], strPsi[_PSI_DB], strTel[_TEL_DB];
	TCHAR			strQuickItem[_QUICKITEM_DB];
	TCHAR			strHaveEvent[_EVENT_DB];

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

	int sql_len = strlen(szSQL);
	SetShort( temp_buffer, sql_len, temp_index );
	SetString( temp_buffer, szSQL, sql_len, temp_index );

	//SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strFace),	0, (TCHAR*)strFace,		0, &sFaceLen );
	SetString( temp_buffer, strFace, sizeof(strFace), temp_index );

	//SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strSkill),	0, (TCHAR*)strSkill,	0, &sSkillLen );
	SetString( temp_buffer, strSkill, sizeof(strSkill), temp_index );

	//SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );
	SetString( temp_buffer, strItem, sizeof(strItem), temp_index );

	//SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strPsi),		0, (TCHAR*)strPsi,		0, &sPsiLen );
	SetString( temp_buffer, strPsi, sizeof(strPsi), temp_index );

	//SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strHaveEvent),	0, (TCHAR*)strHaveEvent,	0, &sEventLen );
	SetString( temp_buffer, strHaveEvent, sizeof(strHaveEvent), temp_index );

	//SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strQuickItem),	0, (TCHAR*)strQuickItem,	0, &sQuickLen );
	SetString( temp_buffer, strQuickItem, sizeof(strQuickItem), temp_index );

	//SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strTel),			0, (TCHAR*)strTel,			0, &sTelLen );
	SetString( temp_buffer, strTel, sizeof(strTel), temp_index );
}


//-----------------------------------------------------------------------------
// DB PHASER PART...
//-----------------------------------------------------------------------------


void USER::DBAccountLoginResult(TCHAR *pBuf, int s_index, int dlength )
{
	int		index = 0, i;
	BYTE	result = FAIL, error_code = 0;
	int		old_index = 0;
	int		s_result, length;

	strcpy(m_strChar[0], "");
	strcpy(m_strChar[1], "");
	strcpy(m_strChar[2], "");
	m_nCharNum = 0;

	s_result = GetByte( pBuf, s_index );
	if ( s_result == FALSE )
	{
		// Load Character Data Fail...
		error_code = UNKNOWN_ERR;
		goto result_send;
	}

	m_state = STATE_CONNECTED;
	result = SUCCESS;

	// LOGIN SERVER���� ���� ĳ���� ����+�̸� ����...
	m_nCharNum = GetByte( pBuf, s_index );
	for ( i = 0; i < 3; i++ )
	{
		GetString(m_strChar[i], pBuf, 20, s_index );
	}
	//
	length = dlength - s_index;
	if ( length <= 0 ) result = FAIL;

result_send:
	index = 0;
	SetByte(m_TempBuf, ACCOUNT_LOGIN_RESULT, index );
	SetByte(m_TempBuf, result, index );

	if ( result == FAIL )
	{
		SetByte(m_TempBuf, error_code, index);
		Send( m_TempBuf, index );
		SoftClose();
		return;
	}

	// LOGIN SERVER���� ���� ĳ���� ����+���� ����...
	SetString(m_TempBuf, (pBuf+s_index), length, index);

	Send(m_TempBuf, index);
}

void USER::DBGameStartResult( char *pBuf, int s_index, int dlength )
{

}

void USER::DressingWindowOpen()
{
	CBufferEx TempBuf;

	TempBuf.Add(DRESSING_WINDOW_OPEN);

	Send(TempBuf, TempBuf.GetLength());
}

void USER::DressingReq(TCHAR *pBuf)
{
	int index = 0;

	int itemslot = (int)GetShort( pBuf, index );
	int itemcount = (int)GetShort( pBuf, index );

	int i = 0;

	CItemTable* pItem = NULL;
	ItemList* pOrgItem = NULL;

	DRESSING_DATA* pDS = NULL;

	int makecount = itemcount / 10;
	int termcount = 0;
	int randseed[200];
	int randcount = 0;
	int randindex = 0;
	int randresult = -1;
	int j = 0;
	int k = 0;

	DressingResultArray arResult;	
	DRESSING_RESULT* pNewDRResult = NULL;
	BOOL bSameExist = FALSE;

	ItemListArray	arEmpty, arSame;
	CWordArray		arEmptySlot, arSameSlot;
	ItemList		MyItem[TOTAL_ITEM_NUM], SameItem[TOTAL_ITEM_NUM];
	int				pTotalEmpty[INVENTORY_NUM];

	short sid = -1, num = -1; 
	int iSlot, iEmptyNum = 0;
	int iWeight = 0;
	int iBasicItemWeight = 0;

	// ���� �ʱ�ȭ ------------------------------------------------//
	BOOL bFlag = FALSE;
	DWORD dwCost = 0;

	DWORD dwTemp = m_dwDN;

	ItemList	*pDSItem = NULL;

	CBufferEx TempBuf;

	int iItemSize = g_arItemTable.GetSize();

	if( itemslot < EQUIP_ITEM_NUM || itemslot >= TOTAL_INVEN_MAX )
	{
		// ���� �ڵ� ���� - ������ �ִ� �κ��� ���� ��ȣ�� �߸��Ǿ���
		SendSystemMsg( IDS_USER_CANT_DRESSING_ITEM, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}

	if( itemcount >= 32767 || itemcount < 0 )
	{
		// ���� �ڵ� ���� - �����Ϸ��� ������ ���� Overflow
		SendSystemMsg( IDS_USER_INVALID_DRESSING_COUNT, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}

	if( itemcount % 10 )
	{
		// ���� �ڵ� ���� - �����Ϸ��� ������ ������ 10�������� �ƴ�
		SendSystemMsg( IDS_USER_DRESSING_COUNT_FORMAT, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}

	pOrgItem = &m_UserItem[itemslot];

	if( !pOrgItem )
	{
		// ���� �ڵ� ���� - �κ����� ������ ã�� �� ����
		SendSystemMsg( IDS_USER_CANT_DRESSING, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}
	
	if( m_UserItem[itemslot].sSid >= 0 && m_UserItem[itemslot].sSid < g_arItemTable.GetSize() )
	{
		pItem = g_arItemTable[m_UserItem[itemslot].sSid];
	}

	if( !pItem )
	{
		// ���� �ڵ� ���� - ������ ���̺��� ������ ã�� �� ����
		SendSystemMsg( IDS_USER_CANT_DRESSING_ITEM, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}

	if( pItem->m_byWear != 107 )
	{
		// ���� �ڵ� ���� - ������ �� ���� ������
		SendSystemMsg( IDS_USER_CANT_DRESSING_ITEM, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}

	if( m_UserItem[itemslot].sCount < itemcount )
	{
		// ���� �ڵ� ���� - ������ ���̺��� ������ ã�� �� ����
		SendSystemMsg( IDS_USER_INVALID_DRESSING_COUNT, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}

	iBasicItemWeight = pItem->m_byWeight * itemcount;

	for( i = 0; i < g_arDressingData.GetSize(); i++ )
	{
		if( g_arDressingData[i] )
		{
			if( g_arDressingData[i]->sItemSid == pItem->m_sSid )
			{
				pDS = g_arDressingData[i];
				break;
			}
		}
	}

	if( !pDS )
	{
		// ���� �ڵ� ���� - ���� ���̺��� �ش� ������ ã�� �� ����
		SendSystemMsg( IDS_USER_CANT_DRESSING_ITEM, SYSTEM_ERROR, TO_ME);
		bFlag = TRUE; goto go_dressing_result;
	}

	for( i = 0; i < makecount; i++ )
	{
		termcount = myrand( pDS->sCountMin, pDS->sCountMax );

		// ������ ���� ���� �õ� �ʱ�ȭ
		for( j = 0; j < 200; j++ )
		{
			randseed[j] = -1;
		}

		randindex = 0;
		for( j = 0; j < 10; j++ )
		{
			for( k = 0; k < pDS->sItemRatio[j]; k++ )
			{
				if( randindex >= 100 ) break;

				randseed[randindex] = pDS->sItem[j];
				randindex++;
			}
		}

		for( j = 0; j < termcount; j++ )
		{
			randcount = myrand( 0, 99 );

			randresult = randseed[randcount];

			if( randresult < 0 )
			{
				// ���� �ڵ� ���� - ���̺� ������ �߸� �ԷµǾ� �ִ�(������ ���� Ȯ���� ��Ż 100�� �ƴϴ�)
				SendSystemMsg( IDS_USER_CANT_DRESSING, SYSTEM_ERROR, TO_ME);
				bFlag = TRUE; goto go_dressing_result;
			}

			bSameExist = FALSE;
			for( k = 0; k < arResult.GetSize(); k++ )
			{
				if( arResult[k] )
				{
					if( arResult[k]->sSid == randresult )
					{
						arResult[k]->sCount++;
						bSameExist = TRUE;
						break;
					}
				}
			}

			if( !bSameExist )
			{
				pNewDRResult = new DRESSING_RESULT;
				pNewDRResult->sSid = randresult;
				pNewDRResult->sCount = 1;
				arResult.Add( pNewDRResult );
			}
		}
	}

	if( arResult.GetSize() == 0 )
	{
		// ���� �ڵ� ���� - ������� ���� ����
		SendSystemMsg( IDS_USER_DRESSING_NO_RESULT, SYSTEM_ERROR, TO_ME);
//		bFlag = TRUE; goto go_dressing_result;
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++)		// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
		SameItem[i] = m_UserItem[i];
	}

	arSameSlot.RemoveAll();
	arSame.RemoveAll();
	arEmpty.RemoveAll();
	arEmptySlot.RemoveAll();

	::ZeroMemory(pTotalEmpty, sizeof(pTotalEmpty));

	GetEmptyInvenSlot(pTotalEmpty);

	char strResult[1024];
	char strTempResult[128];
	sprintf( strResult, _ID(IDS_USER_DRESSING_RESULT) );

	// ���� ��� ��� ---------------------------------------------------//
	for( i = 0; i < arResult.GetSize(); i++ )
	{
		if( !arResult[i] ) continue;

		iSlot = -1;

		sid = (short)arResult[i]->sSid;
		num = (short)arResult[i]->sCount;

		if(iEmptyNum > INVENTORY_NUM) { bFlag = TRUE; goto go_dressing_result; }
		if(sid < 0 || num <= 0 || sid > iItemSize) { bFlag = TRUE; goto go_dressing_result; }

		pDSItem = NULL;
		pDSItem = new ItemList;
		if(pDSItem == NULL) { bFlag = TRUE; goto go_dressing_result; }

		ReSetItemSlot(pDSItem);

		pDSItem->sLevel = g_arItemTable[sid]->m_byRLevel;
		pDSItem->sSid = sid;
		pDSItem->sCount = num;
		pDSItem->sDuration = g_arItemTable[sid]->m_sDuration;
		pDSItem->sBullNum = g_arItemTable[sid]->m_sBullNum;
		for( k =0; k< MAGIC_NUM; k++ ) pDSItem->tMagic[k] = 0;
		pDSItem->tIQ = 0;
		pDSItem->iItemSerial = 0;

		sprintf( strTempResult, "%s(%d) ", g_arItemTable[sid]->m_strName, num );
		strcat( strResult, strTempResult );

		iSlot = GetSameItem(*pDSItem, INVENTORY_SLOT);

		if(iSlot != -1)
		{ 
			if(num != 0)
			{
				pDSItem->sCount = num;
				arSame.Add(pDSItem); 
				arSameSlot.Add(iSlot); 

				iWeight += g_arItemTable[sid]->m_byWeight * num;
			}
		}
		else			
		{
			iSlot = pTotalEmpty[iEmptyNum];
//			if(iSlot == 0) { bFlag = TRUE; goto go_dressing_result; }
			if(iSlot == 0) 
			{ 
				CPoint ptCell = FindNearRandomPoint(m_curx, m_cury);

				if(ptCell.x < 0 || ptCell.y < 0) { if(pDSItem) { delete pDSItem; pDSItem = NULL; } continue; }
				if(ptCell.x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || ptCell.y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) { if(pDSItem) { delete pDSItem; pDSItem = NULL; } continue; }

				if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, (long)1, (long)0) == (long)0)
				{
					if(m_iMaxWeight < m_iCurWeight + iWeight - iBasicItemWeight)
					{
						::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0);

						if(pDSItem) { delete pDSItem; pDSItem = NULL; }

						SendSystemMsg( IDS_USER_OVER_WEIGHT1, SYSTEM_ERROR, TO_ME);
						goto go_dressing_result;
					}

					pDSItem->tType = TYPE_ITEM;
					pDSItem->uid[0] = m_uid;				// �켱 ����
					pDSItem->SuccessRate[0] = (BYTE)100;	// �켱 ���� ����
					pDSItem->dwTime = ConvertCurTimeToSaveTime();

					//m_pCom->DelThrowItem();
					m_pCom->SetThrowItem( pDSItem, ptCell.x, ptCell.y, m_ZoneIndex );

					::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[ptCell.x][ptCell.y].m_FieldUse, 0);
				}
				else 
				{
					if(pDSItem) { delete pDSItem; pDSItem = NULL; }
				}
			}
			else
			{
				arEmpty.Add(pDSItem); 
				arEmptySlot.Add(iSlot); 
				iEmptyNum++;

				iWeight += g_arItemTable[sid]->m_byWeight * num;
			}
		}		
	}

	if(m_iMaxWeight < m_iCurWeight + iWeight - iBasicItemWeight)
	{
		SendSystemMsg( IDS_USER_OVER_WEIGHT1, SYSTEM_ERROR, TO_ME);
		goto go_dressing_result;
	}

	// ����� �κ��� -------------------- --------------------//
	for( i = 0; i < arSame.GetSize(); i++ )
	{
		CheckMaxValue((short &)m_UserItem[arSameSlot[i]].sCount, (short)arSame[i]->sCount); 
	}

	for(i = 0; i < arEmpty.GetSize(); i++)
	{
		m_UserItem[arEmptySlot[i]].sLevel = arEmpty[i]->sLevel;
		m_UserItem[arEmptySlot[i]].sSid = arEmpty[i]->sSid;
		m_UserItem[arEmptySlot[i]].sCount = arEmpty[i]->sCount;
		m_UserItem[arEmptySlot[i]].sBullNum = arEmpty[i]->sBullNum;
		m_UserItem[arEmptySlot[i]].sDuration = arEmpty[i]->sDuration;
		m_UserItem[arEmptySlot[i]].tIQ = arEmpty[i]->tIQ;
		for(j =0; j < MAGIC_NUM; j++) m_UserItem[arEmptySlot[i]].tMagic[j] = arEmpty[i]->tMagic[j];
	}

	// ������ ���ִ� �ڵ� �ʿ��� !!!!
	if( pOrgItem->sCount ==  itemcount ) ReSetItemSlot( &m_UserItem[itemslot] );//ReSetItemSlot(sSlot);
	else m_UserItem[itemslot].sCount = m_UserItem[itemslot].sCount - (short)itemcount;

	if(UpdateUserItemDN() == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)		// ������ ���� ȯ��
		{
			m_UserItem[i] = MyItem[i];
		}
		SendSystemMsg( IDS_USER_DRESSING_FAIL, SYSTEM_NORMAL, TO_ME);
		bFlag = TRUE;
		goto go_dressing_result;
	}

	m_iCurWeight += ( iWeight - iBasicItemWeight );
	GetRecoverySpeed();

	UpdateInvenSlot(&arEmptySlot, &arSameSlot);

	SendSystemMsg( strResult, SYSTEM_ERROR, TO_ME);

	TempBuf.Add(DRESSING);
	TempBuf.Add(SUCCESS);

	TempBuf.Add( (short)itemslot );
	TempBuf.Add(pOrgItem->sLevel);
	TempBuf.Add(pOrgItem->sSid);
	TempBuf.Add(pOrgItem->sDuration);
	TempBuf.Add(pOrgItem->sBullNum);
	TempBuf.Add(pOrgItem->sCount);
	for( i = 0; i < MAGIC_NUM; i++ ) TempBuf.Add(pOrgItem->tMagic[i]);
	TempBuf.Add(pOrgItem->tIQ);

	Send(TempBuf, TempBuf.GetLength());

go_dressing_result:
	int dddd = 0;

	if( bFlag )
	{
		TempBuf.Add(DRESSING);
		TempBuf.Add(FAIL);

		Send(TempBuf, TempBuf.GetLength());
	}

	// �޸� ���� -------------------- --------------------//
	for(i = 0; i < arSame.GetSize(); i++)
	{
		if(arSame[i] != NULL) delete arSame[i];
	}
	arSame.RemoveAll();
	arSameSlot.RemoveAll();
	for(i = 0; i < arEmpty.GetSize(); i++)
	{
		if(arEmpty[i] != NULL) delete arEmpty[i];
	}
	arEmpty.RemoveAll();
	arEmptySlot.RemoveAll();

	for(i = 0; i < arResult.GetSize(); i++ )
	{
		if( arResult[i] ) delete arResult[i];
	}
	arResult.RemoveAll();
}

void USER::ItemDataVersionCheck()
{
/*
	int i;

	switch( m_tItemVer )
	{
	case	0x00:
		{
			for(i = 0; i < TOTAL_ITEM_NUM; i++)
			{
				m_UserItem[i].iItemSerial = GenerateItemSerial( &(m_UserItem[i]) );
			}

			for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
			{
				m_UserBankItem[i].iItemSerial = GenerateItemSerial( &(m_UserBankItem[i]) );
			}

			m_tItemVer = 0x01;

			UpdateMemUserAll( TRUE );
		}

	case	0x01:
	default:
		break;
	}

	switch( m_tAccountBankItemVer )
	{
	case	0X00:
		{
			for(i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++)
			{
				m_AccountBankItem[i].iItemSerial = GenerateItemSerial( &(m_AccountBankItem[i]) );
			}

			m_tAccountBankItemVer = 0x01;

			UpdateMemUserAll( TRUE );
		}
	}
*/	
}

void USER::SelectOver100Skill(TCHAR *pBuf)
{
	if( m_sLevel < 100 ) return;		// 100�� �̻� ���� �� �� �ִ�.

	int emptyskillslot = -1;

/*
#define BRAWL				0
#define STAFF				1
#define	EDGED				2
#define FIREARMS			3
*/
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

	if( emptyskillslot == -1 ) return;	// �� �迭���� ������� �ʰ� �ִ� ������ ��ų

	int skillsid = m_UserSkill[emptyskillslot].sSid;

	if(skillsid < 0 || skillsid >= g_arSkillTable.GetSize()) return;

	if( g_arSkillTable[skillsid]->m_sSid != -1 )		// �̹� 100�� �̻� ��ų�� �������
	{
		if( emptyskillslot != skillsid ) return;
	}

	int index = 0;

	int selectedskill = GetByte( pBuf, index );

	if( selectedskill < 20 || selectedskill >= 24 ) return;	// Ŭ���̾�Ʈ���� �Ѿ���� ��ų ��ȣ�� 0-3�̴�

/*	20 - �Ͱ�
	21 - ������
	22 - ����ű ����
	23 - ���潺 �� */

/*	if(selectedskill == 20)
	{
		if(m_byClass != 0)
		{
			SendUserStatusSkill();
			return;
		}
	} */

	int backupskill = m_UserSkill[emptyskillslot].sSid;
	int backupskilllevel = m_UserSkill[emptyskillslot].tLevel;

	m_UserSkill[emptyskillslot].sSid = selectedskill;
	m_UserSkill[emptyskillslot].tLevel = (BYTE)((m_sLevel % 100) + 1);
//niuniuadd
	if(m_UserSkill[emptyskillslot].tLevel>20)
			m_UserSkill[emptyskillslot].tLevel=20;
	if( !UpdateUserData() )
	{
		m_UserSkill[emptyskillslot].sSid = backupskill;
		m_UserSkill[emptyskillslot].tLevel = backupskilllevel;
		return;
	}

	SendUserStatusSkill();
}

UINT64 USER::GenerateItemSerial(ItemList* pItem)
{
	/*if( pItem->tIQ == MAGIC_ITEM ||		// �����������̰ų�
		pItem->tIQ == RARE_ITEM ||		// ����������̰ų�
		pItem->sLevel >= 20 ||			// ���� 20�̻� �������̰ų�
		pItem->tMagic[5] >= 1 ||		// ���׷��̵尡 1���̻� �� �������̶��
		pItem->sSid == 756 )			// �ؾ�
	{
		CTime t;

		t = CTime::GetCurrentTime();

		MYINT upper;		// �ð����� ���� 4����Ʈ�� �����
		MYINT under;		// �ø���� ���� 3����Ʈ�� ������ �ѹ���Ʈ�� ���� �Ϸù�ȣ

		memcpy( &(upper), &t, sizeof(CTime) );
		TRACE("111111: %d   =? %d\n", sizeof(CTime),sizeof(upper.i) );
		under.i = (int)(g_dwItemSerial);
		under.b[3] = g_ItemSerialIndex;

		g_dwItemSerial++;

		if( g_dwItemSerial >= 255*255*255 ) g_dwItemSerial = 0;

		MYINT64 total;

		total.b[7] = upper.b[3];
		total.b[6] = upper.b[2];
		total.b[5] = upper.b[1];
		total.b[4] = upper.b[0];

		total.b[3] = under.b[3];
		total.b[2] = under.b[2];
		total.b[1] = under.b[1];
		total.b[0] = under.b[0];

		TRACE("GenerateSerial - %I64d\n", total.i );

		return total.i;

	}
	*/

	return 0;
}

void USER::AccountBankItemMoveReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int index = 0;
	BYTE type = GetByte(pBuf, index);

	switch(type)
	{
	case BANK_ITEM_INPUT:
		AccountBankInPut(pBuf + index);
		break;
	case BANK_ITEM_OUTPUT:
		AccountBankOutPut(pBuf + index);
		break;
	case BANK_DN_INPUT:
		AccountBankInPutDN(pBuf + index);
		break;
	}
}

void USER::AccountBankInPut(TCHAR *pBuf)
{
	int i;
	int tDestSlot;
	int index = 0;
	int iOver = 0;
	short sSid = -1;
	short sHaveCount = 0;
	DWORD dwCost = 150;

	BYTE result = SUCCESS;

	CBufferEx TempBuf;

	ItemList MyItem[TOTAL_ITEM_NUM], BackItem;

	BYTE tSourceSlot = GetByte(pBuf, index);	// ����� �������� �ִ� ���� ��ġ
	short sCount = GetShort(pBuf, index);		// �󸶸�ŭ ����
												// ������ �κ��� �����ϴ�.			
	if(tSourceSlot < EQUIP_ITEM_NUM || tSourceSlot >= EQUIP_ITEM_NUM + INVENTORY_NUM) { result = FAIL; goto go_result; }

	sSid = m_UserItem[tSourceSlot].sSid;
	sHaveCount = m_UserItem[tSourceSlot].sCount;
	if(sHaveCount <= 0) { result = FAIL; goto go_result; }	// �������� ����.
												
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { result = FAIL; goto go_result; }	// �߸��� sSid

	if(g_arItemTable[sSid]->m_sEvent >= EVENT_RR_ITEM_BAND)		// �ο����� ��ǰ�� ����â�� ���� �� ����.
	{
		SendSystemMsg( IDS_USER_CANT_SHARE_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
		result = FAIL; goto go_result; 
	}

	if(sCount > ACCOUNT_BANK_MAX_ITEM) 
	{
		SendSystemMsg( IDS_USER_SAME_ITEM_COUNT_MAX, SYSTEM_ERROR, TO_ME);
		result = FAIL; goto go_result;
	}
												
	if(sCount <= 0 || sCount > sHaveCount) { result = FAIL; goto go_result; }	// ������ ������ ������ ���� ���
												//������ üũ�� wear������ �ߴ�.
	//if(g_arItemTable[sSid]->m_byWear <= 5 && sCount > 1) { result = FAIL; goto go_result; }
	if(g_arItemTable[sSid]->m_sDuration > 0 && sCount > 1) { result = FAIL; goto go_result; }	// ��ĥ�� ���µ� ������ 1���� Ŭ�� ����.
	
	for(i = 0; i < TOTAL_ITEM_NUM; i++)	// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	ReSetItemSlot(&BackItem);					// DB���п� ����� ����� �ʱ�ȭ
												// �ϴ� ��ĥ�� �ִ���, ��ġ�� ���� �������� �ִ��� ã�ƺ���.
	tDestSlot = GetSameItem(m_UserItem[tSourceSlot], ACCOUNT_BANK_SLOT);
	
	/***************************���� ���� ó��********************************************/
	if(tDestSlot >= 0)							// �׻� ������ �߰� �Ǵ°��� �������� ó��
	{											
		BackItem = m_AccountBankItem[tDestSlot];

		if(m_AccountBankItem[tDestSlot].sCount >= ACCOUNT_BANK_MAX_ITEM)
		{
			SendSystemMsg( IDS_USER_SAVE_MAX_COUNT, SYSTEM_ERROR, TO_ME);
			result = FAIL; goto go_result;
		}

		if((m_AccountBankItem[tDestSlot].sCount + sCount) > ACCOUNT_BANK_MAX_ITEM)
		{										// MAX���� ������ ��ä��� ������ ó��			
			iOver = m_AccountBankItem[tDestSlot].sCount + sCount - ACCOUNT_BANK_MAX_ITEM;
			if(iOver <= 0) { result = FAIL; goto go_result; }

			m_AccountBankItem[tDestSlot].sCount = ACCOUNT_BANK_MAX_ITEM;
			sCount = sCount - iOver;
		}
		else m_AccountBankItem[tDestSlot].sCount += sCount;
	}
	else
	{											//	�߰�
		tDestSlot = GetEmptySlot(ACCOUNT_BANK_SLOT);

		if(tDestSlot == -1) 
		{
			result = FAIL; goto go_result; 
		}

		m_AccountBankItem[tDestSlot].sLevel = m_UserItem[tSourceSlot].sLevel;
		m_AccountBankItem[tDestSlot].sSid = m_UserItem[tSourceSlot].sSid;
		m_AccountBankItem[tDestSlot].sDuration = m_UserItem[tSourceSlot].sDuration;
		m_AccountBankItem[tDestSlot].sBullNum = m_UserItem[tSourceSlot].sBullNum;
		m_AccountBankItem[tDestSlot].sCount = sCount;
		for(i = 0; i < MAGIC_NUM; i++) m_AccountBankItem[tDestSlot].tMagic[i] = m_UserItem[tSourceSlot].tMagic[i];
		m_AccountBankItem[tDestSlot].tIQ = m_UserItem[tSourceSlot].tIQ;
		m_AccountBankItem[tDestSlot].iItemSerial = m_UserItem[tSourceSlot].iItemSerial;
	}

	index = 0;
	index = g_arItemTable[m_UserItem[tSourceSlot].sSid]->m_byWeight * sCount;
	/**************************���� �κ� ó��*********************************************/
	if(sCount >= sHaveCount && iOver == 0)
	{
		MakeItemLog( &m_AccountBankItem[tDestSlot], ITEMLOG_ACCOUNT_BANKIN );
		ReSetItemSlot(&m_UserItem[tSourceSlot]);	
	}
	else m_UserItem[tSourceSlot].sCount -= sCount;
	
	/**************************DB Update ó��*********************************************/
	if(UpdateUserBank() == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ����
		{
			m_UserItem[i] = MyItem[i];
		}
		m_AccountBankItem[tDestSlot] = BackItem;

		result = FAIL;
		FlushItemLog( FALSE );
		goto go_result;
	}

	FlushItemLog( TRUE );

go_result:
	TempBuf.Add(ACCOUNT_BANK_ITEM_MOVE_RESULT);

	if(result == FAIL)
	{
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	result = (BYTE)0x01;
	TempBuf.Add(result);

	TempBuf.Add((BYTE)tDestSlot);				// �����ﶫ����
	TempBuf.Add( dwCost );
	TempBuf.Add(m_AccountBankItem[tDestSlot].sLevel);
	TempBuf.Add(m_AccountBankItem[tDestSlot].sSid);
	TempBuf.Add(m_AccountBankItem[tDestSlot].sDuration);
	TempBuf.Add(m_AccountBankItem[tDestSlot].sBullNum);
	TempBuf.Add(m_AccountBankItem[tDestSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_AccountBankItem[tDestSlot].tMagic[i]);
	TempBuf.Add(m_AccountBankItem[tDestSlot].tIQ);

	TempBuf.Add((BYTE)tSourceSlot);				// ���ϵĶ���
	TempBuf.Add(m_UserItem[tSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[tSourceSlot].sSid);
	TempBuf.Add(m_UserItem[tSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[tSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[tSourceSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tSourceSlot].tMagic[i]);
	TempBuf.Add(m_UserItem[tSourceSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());

	m_iCurWeight -= index;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...
}

void USER::AccountBankOutPut(TCHAR *pBuf)
{
	int i, j, iWeight = 0;
	int tDestSlot;
	int index = 0;
	int iOver = 0;
	short sSid = -1;
	short sHaveCount = 0;

	short myslot;
	BYTE result = SUCCESS;

	CWordArray		arChangeMy, arChangeBank;
	ItemList		MyItem[TOTAL_ITEM_NUM], BankItem[TOTAL_ACCOUNT_BANK_ITEM_NUM];
	BYTE			arSlot[TOTAL_ACCOUNT_BANK_ITEM_NUM];
	short			arCount[TOTAL_ACCOUNT_BANK_ITEM_NUM];

	for( i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++ ) arSlot[i] = -1;
	for( i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++ ) arCount[i] = -1;

	CBufferEx TempBuf;

	CStore* pStore = NULL;
	short sStoreID = GetShort(pBuf, index);
	if(sStoreID < 0) return;
	pStore = GetStore(sStoreID);
	if(pStore == NULL) return;
	short sRate = pStore->m_sRate;

	DWORD OutCost = 0;
	DWORD dwTax = 0;

	DWORD OutputDN = GetDWORD(pBuf, index);

	if(OutputDN > m_dwAccountBankDN)			// �ʹ� ũ��
	{
		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	if( OutputDN > 0 ) OutCost += 150;

	DWORD BackMyDN = m_dwDN;					// ���
	DWORD BackBankDN = m_dwAccountBankDN;

	short slotcount = GetShort( pBuf, index );

	if( slotcount < 0 || slotcount >= TOTAL_ACCOUNT_BANK_ITEM_NUM ) { result = FAIL; goto go_result; }

	for( i = 0; i < slotcount; i++ )
	{
		arSlot[i] = GetShort( pBuf, index );
		arCount[i] = GetShort( pBuf, index );

		if(arSlot[i] >= TOTAL_ACCOUNT_BANK_ITEM_NUM) { result = FAIL; goto go_result; }

		sSid = m_AccountBankItem[arSlot[i]].sSid;
		sHaveCount = m_AccountBankItem[arSlot[i]].sCount;

		if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { result = FAIL; goto go_result; }

		if(arCount[i] <= 0 || arCount[i] > sHaveCount) { result = FAIL; goto go_result; }

		if(g_arItemTable[sSid]->m_sDuration > 0 && arCount[i] > 1) { result = FAIL; goto go_result; }

		iWeight += g_arItemTable[sSid]->m_byWeight * arCount[i];

		if(m_iMaxWeight < m_iCurWeight + iWeight)
		{
			SendSystemMsg( IDS_USER_OVER_WEIGHT1, SYSTEM_ERROR, TO_ME);
			result = FAIL; 
			goto go_result;
		}

		OutCost += 150;
	}

	if( OutCost > m_dwDN )
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY_FOR_OUT, SYSTEM_ERROR, TO_ME);
		result = FAIL; 
		goto go_result;
	}
	else m_dwDN -= OutCost;

	for( i = 0; i < TOTAL_ITEM_NUM; i++ )	// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	for( i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++ )
	{
		BankItem[i] = m_AccountBankItem[i];
	}

	for( i = 0; i < slotcount; i++ )
	{
		tDestSlot = GetSameItem( m_AccountBankItem[arSlot[i]], INVENTORY_SLOT );

		if( tDestSlot >= 0 )
		{
			m_UserItem[tDestSlot].sCount += arCount[i];
		}
		else
		{
			tDestSlot = GetEmptySlot( INVENTORY_SLOT );

			if( tDestSlot == -1 )
			{
				for( j = 0; j < TOTAL_ITEM_NUM; j++)// ������ ���� ����
				{
					m_UserItem[j] = MyItem[j];
				}
				for( j = 0; j < TOTAL_ACCOUNT_BANK_ITEM_NUM; j++ )
				{
					m_AccountBankItem[j] = BankItem[j];
				}
				m_dwDN = BackMyDN;
				m_dwAccountBankDN = BackBankDN;

				result = FAIL; goto go_result;
			}

			m_UserItem[tDestSlot].sLevel = m_AccountBankItem[arSlot[i]].sLevel;
			m_UserItem[tDestSlot].sSid = m_AccountBankItem[arSlot[i]].sSid;
			m_UserItem[tDestSlot].sDuration = m_AccountBankItem[arSlot[i]].sDuration;
			m_UserItem[tDestSlot].sBullNum = m_AccountBankItem[arSlot[i]].sBullNum;
			m_UserItem[tDestSlot].sCount = arCount[i];
			for(j = 0; j < MAGIC_NUM; j++) m_UserItem[tDestSlot].tMagic[j] = m_AccountBankItem[arSlot[i]].tMagic[j];
			m_UserItem[tDestSlot].tIQ = m_AccountBankItem[arSlot[i]].tIQ;
			m_UserItem[tDestSlot].iItemSerial = m_AccountBankItem[arSlot[i]].iItemSerial;

			MakeItemLog( &m_UserItem[tDestSlot], ITEMLOG_ACCOUNT_BANKOUT );
		}

		arChangeMy.Add( tDestSlot );

		if( arCount[i] >= m_AccountBankItem[arSlot[i]].sCount ) ReSetItemSlot( &m_AccountBankItem[arSlot[i]] );
		else m_AccountBankItem[arSlot[i]].sCount -= arCount[i];

		arChangeBank.Add( arSlot[i] );
	}

	if(!CheckMaxValueReturn(m_dwDN, OutputDN))
	{									// ��, MAX���̸� ������...
		CheckMaxValue(m_dwDN, OutputDN);
		if(m_dwDN < OutputDN) OutputDN = 0;
		else OutputDN = m_dwDN - OutputDN;
	}
	else m_dwDN += OutputDN;
										// ���࿡�� ����.
	if(OutputDN >= m_dwAccountBankDN) m_dwAccountBankDN = 0;
	else m_dwAccountBankDN -= OutputDN;

	if( OutputDN > 0 )
		MakeMoneyLog( OutputDN, ITEMLOG_ACCOUNT_BANKOUT_MONEY, NULL, m_dwAccountBankDN );

	// ��Ʈ������ ������ �������� �ش�.
	dwTax = (DWORD)( OutCost * ((double)sRate/100) );
	UpdateFortressTax(sStoreID, dwTax); // ��Ʈ���� �����̸� ����...


	/**************************DB Update ó��*********************************************/
	if(UpdateUserBank() == FALSE)
	{
		for( i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ����
		{
			m_UserItem[i] = MyItem[i];
		}
		for( i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++ )
		{
			m_AccountBankItem[i] = BankItem[i];
		}
		m_dwDN = BackMyDN;
		m_dwAccountBankDN = BackBankDN;

		result = FAIL;

		goto go_result;
	}

	FlushItemLog( TRUE );

go_result:
	TempBuf.Add(ACCOUNT_BANK_ITEM_MOVE_RESULT);

	if(result == FAIL)
	{
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		FlushItemLog( FALSE );
		return;
	}

	result = (BYTE)0x02;
	TempBuf.Add(result);

	TempBuf.Add( m_dwDN );
/*	TempBuf.Add( m_dwAccountBankDN );

	TempBuf.Add( (short)(arChangeBank.GetSize()) );

	for( i = 0; i < arChangeBank.GetSize(); i++ )
	{
		bankslot = arChangeBank[i];

		TempBuf.Add( (BYTE)bankslot );
		TempBuf.Add(m_AccountBankItem[bankslot].sLevel);
		TempBuf.Add(m_xxxxxxxxeiAccountBankItem[bankslot].sSid);
		TempBuf.Add(m_AccountBankItem[bankslot].sDuration);
		TempBuf.Add(m_AccountBankItem[bankslot].sBullNum);
		TempBuf.Add(m_AccountBankItem[bankslot].sCount);
		for( j = 0; j < MAGIC_NUM; j++) TempBuf.Add(m_AccountBankItem[bankslot].tMagic[j]);
		TempBuf.Add(m_AccountBankItem[bankslot].tIQ);
	}
*/
	TempBuf.Add( (short)(arChangeMy.GetSize()) );

	for( i = 0; i < arChangeMy.GetSize(); i++ )
	{
		myslot = arChangeMy[i];

		TempBuf.Add( (BYTE)myslot );
		TempBuf.Add( m_UserItem[myslot].sLevel );
		TempBuf.Add( m_UserItem[myslot].sSid);
		TempBuf.Add( m_UserItem[myslot].sDuration);
		TempBuf.Add( m_UserItem[myslot].sBullNum);
		TempBuf.Add( m_UserItem[myslot].sCount);
		for( j = 0; j < MAGIC_NUM; j++ ) TempBuf.Add( m_UserItem[myslot].tMagic[j] );
		TempBuf.Add( m_UserItem[myslot].tIQ );
	}

	Send(TempBuf, TempBuf.GetLength());

	m_iCurWeight += iWeight;
	GetRecoverySpeed();							// ȸ���ӵ� üũ...
}

void USER::AccountBankInPutDN(TCHAR *pBuf)
{
	CBufferEx TempBuf;

	BYTE result; 
	int index = 0;
	DWORD BackBankDN = 0, BackMyDN = 0;

	DWORD InputDN = GetDWORD(pBuf, index);

	TempBuf.Add(ACCOUNT_BANK_ITEM_MOVE_RESULT);

	if(InputDN == 0 || InputDN > m_dwDN) 
	{
		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}
	
	BackMyDN = m_dwDN;
	BackBankDN = m_dwAccountBankDN;
										// ���࿡ �Ա�
	if(!CheckMaxValueReturn(m_dwAccountBankDN, InputDN))
	{									// ��, MAX���̸� ������...
		CheckMaxValue(m_dwAccountBankDN, InputDN);
		if(m_dwAccountBankDN < InputDN) InputDN = 0;
		else InputDN = m_dwAccountBankDN - InputDN;
	}
	else m_dwAccountBankDN += InputDN;
										// ������ �ִ� �����ݿ��� ����.
	if(m_dwDN <= InputDN) m_dwDN = 0;
	else m_dwDN -= InputDN;

	MakeMoneyLog( InputDN, ITEMLOG_ACCOUNT_BANKIN_MONEY, NULL, m_dwAccountBankDN );

	if(UpdateUserBank() == FALSE)		// DB UpDate
	{
		m_dwDN = BackMyDN;
		m_dwBankDN = BackBankDN;

		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());

		FlushItemLog( FALSE );
		return;
	}

	result = (BYTE)0x03;				// 3 : DN �Ա�
	TempBuf.Add(result);

	TempBuf.Add(m_dwAccountBankDN);			
	TempBuf.Add(m_dwDN);				

	Send(TempBuf, TempBuf.GetLength());

	FlushItemLog( TRUE );
}

void USER::AccountBankOpenReq(int nStoreID)
{
	CStore* pStore = NULL;
	pStore = GetStore(nStoreID);
	if(pStore == NULL) return;
	DWORD dwCost = 150;

	int i, j;
	CBufferEx TempBuf;

	CByteArray arItemSlotList;

//	m_dwAccountBankDN = 0;

//	if(!LoadMemAccountBank())
//	{
//		if(!LoadAccountBank()) return;						// ������ ������ ���� �������� ó�� �����Ҷ� ������ �´�.
//	}

	for(i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++)
	{												// ���� ������ �����۸� �����ֱ����� �����Ѵ�. 
		if(m_AccountBankItem[i].sSid >= 0)
		{
			arItemSlotList.Add(i);
		}
	}

	TempBuf.Add(ACCOUNT_BANK_OPEN);

	TempBuf.Add((short)nStoreID);

	TempBuf.Add((DWORD)m_dwAccountBankDN);
	TempBuf.Add((BYTE)arItemSlotList.GetSize());

	for(i = 0; i < arItemSlotList.GetSize(); i++)
	{
		BYTE tempSlot = 0;
		tempSlot = arItemSlotList[i];
		TempBuf.Add(tempSlot);
		TempBuf.Add((short)m_AccountBankItem[tempSlot].sLevel);
		TempBuf.Add((short)m_AccountBankItem[tempSlot].sSid);
		TempBuf.Add((short)m_AccountBankItem[tempSlot].sDuration);
		TempBuf.Add((short)m_AccountBankItem[tempSlot].sBullNum);
		TempBuf.Add((short)m_AccountBankItem[tempSlot].sCount);

		for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_AccountBankItem[tempSlot].tMagic[j]);

		TempBuf.Add((BYTE)m_AccountBankItem[tempSlot].tIQ);
		TempBuf.Add( dwCost );
	}

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	���̿��� ������ �Ͱ��� ���� �������� ���Ѵ�.
//
int USER::GetPsyAssault(bool bMax)
{
	int iLevel = 0;
	int iSkillSid = 0;

	int iAttackUp = 0;
	int tClass = 0;

	BYTE tWeaponClass = 0;

	IsCanUseWeaponSkill(tWeaponClass);
	tClass = tWeaponClass * SKILL_NUM;

	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid  = m_UserSkill[i].sSid;
		if(iSkillSid == SKILL_ASSAULT) // �Ͱ�			// 20 index
		{
			// ���� ��ų ���� 
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;
			
			// �����ۿ� ���� ��ų ���� ����
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];
			
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			int iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
			if((iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel))|| bMax)
			{
				iAttackUp = g_arSkillTable[iSkillSid]->m_arInc.GetAt(iLevel);
			}
		}
	}

	return iAttackUp;	
}

////////////////////////////////////////////////////////////////////////////////
//	���̿��� ������ ������ ��ų�� ���� �������� ���Ѵ�.
//
int USER::GetPsyAbsoluteDefense()
{
	int iLevel = 0;
	int iSkillSid = 0;

	int iAttackUp = 0;
	int tClass = 0;

	BYTE tWeaponClass = 0;

	IsCanUseWeaponSkill(tWeaponClass);
	tClass = tWeaponClass * SKILL_NUM;

	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid  = m_UserSkill[i].sSid;
		if(iSkillSid == SKILL_ABSOLUTE_DEFENSE) //���Է���			21 index
		{
			// ���� ��ų ���� 
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;
			
			// �����ۿ� ���� ��ų ���� ����
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];
			
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			iAttackUp = g_arSkillTable[iSkillSid]->m_arInc.GetAt(iLevel);
		}
		if(iSkillSid == SKILL_PSYCHIC_RESIST) //ħ������			22 index
		{
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;
			
			
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];
			
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			iAttackUp += g_arSkillTable[iSkillSid]->m_arInc.GetAt(iLevel);
		}
	}

	return iAttackUp;	
}

////////////////////////////////////////////////////////////////////////////////
//	���̿��� ������ ���̿��� ���� ��ų�� ���� �������� ���Ѵ�.
//
int USER::GetPsyPsyResist()
{
	int iLevel = 0;
	int iSkillSid = 0;

	int iAttackUp = 0;
	int tClass = 0;

	BYTE tWeaponClass = 0;

	IsCanUseWeaponSkill(tWeaponClass);
	tClass = tWeaponClass * SKILL_NUM;

	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid  = m_UserSkill[i].sSid;
		if(iSkillSid == SKILL_PSYCHIC_RESIST) // ���̿��� ����			// 22 index
		{
			// ���� ��ų ���� 
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;
			
			// �����ۿ� ���� ��ų ���� ����
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];
			
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;
			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			iAttackUp = g_arSkillTable[iSkillSid]->m_arInc.GetAt(iLevel);
		}
	}

	return iAttackUp;	
}

void USER::ResetOver100LevelSkill(int sLevel)
{
	int emptyskillslot = -1;
	int skillsid = 0;

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

	skillsid = m_UserSkill[emptyskillslot].sSid;

	if( sLevel <= 99 )		// ���� 99������ �����϶�� ������
	{
		m_UserSkill[emptyskillslot].sSid = emptyskillslot;		// ���� 100�� ��ų ������ �����
		m_UserSkill[emptyskillslot].tLevel = 0;

		return;
	}
	else
	{
		if( skillsid < 0 || skillsid >= g_arSkillTable.GetSize() ) return;

		if( g_arSkillTable[skillsid]->m_sSid == -1 ) return;

		m_UserSkill[emptyskillslot].tLevel = (BYTE)((sLevel % 100) + 1);
		//niuniuadd
		if(m_UserSkill[emptyskillslot].tLevel>20)
			m_UserSkill[emptyskillslot].tLevel=20;
	}
	
	if(sLevel >=120 && sLevel<130){//120����ת��������..�ӵĵ�...
		int iIndex=m_byClass *SKILL_NUM;
		int i;
		for(i = iIndex; i < iIndex + SKILL_NUM-1; i++)
		{
			if(m_UserSkill[i].sSid>=20){
				m_UserSkill[i].tLevel++;
				if(m_UserSkill[i].tLevel>=20)
					m_UserSkill[i].tLevel=20;
			}
		}
	}
	if(sLevel >=130){
		CheckMaxValue((short &)m_sSkillPoint_, (short)1);
	}
}

BOOL USER::CheckMoneyMinMax(int min, int max)
{
	if( min < -1 || max < -1 ) return FALSE;
	if( min == -1 && max == -1 ) return FALSE;

	DWORD dwMin = 0;
	DWORD dwMax = 0;

	if( min == -1 && max > 0 )		// max�̸��� ���� ������ TRUE
	{
		dwMax = max;

		if( m_dwDN < dwMax )
		{
			return TRUE;
		}
	}
	else if( min > 0 && max == -1 )	// min�̻��� ���� ������ TRUE
	{
		dwMin = min;

		if( m_dwDN >= dwMin )
		{
			return TRUE;
		}
	}
	else if( min > 0 && max > 0 )
	{
		dwMin = min;
		dwMax = max;

		if( m_dwDN >= dwMin && m_dwDN < dwMax )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL USER::CheckRandom(int rand)
{
	if( rand <= 0 ) return FALSE;

	if( rand >= 100 ) return TRUE;

	int temp_rand = rand * 100;

	int rand_result = myrand( 0, 10000 );

	if( rand_result <= temp_rand ) return TRUE;

	return FALSE;
}

void USER::GiveDN(int dn)
{
	if( dn <= 0 ) return;

	DWORD dwBackup = m_dwDN;
	DWORD dwGiveDN = dn;

	CheckMaxValue( (DWORD &)m_dwDN, dwGiveDN );

	if( !UpdateUserItemDN() )
	{
		m_dwDN = dwBackup;
		return;
	}

	SendMoneyChanged();
}

void USER::RobDN(int dn)
{
	if( dn <= 0 ) return;

	DWORD dwBackup = m_dwDN;
	DWORD dwRobDN = dn;
	DWORD dwTemp = m_dwDN;

	if( m_dwDN <= dwRobDN )
	{
		m_dwDN = 0;
	}
	else
	{
		m_dwDN = dwTemp - dwRobDN;
	}

	if( !UpdateUserItemDN() )
	{
		m_dwDN = dwBackup;
		return;
	}

	SendMoneyChanged();
}

void USER::Collect(TCHAR *pBuf)
{
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

	int		iFireErrDec = 0;
	int	determine = 0;
	int iDexHitRate = 0, iLevelHitRate = 0;
	int iMyDex = 0, iYourDex = 0;

	CByteArray	arSkillAction1, arSkillAction2;							// Skill Action Array

	DWORD dwExp = 0;
	int index = 0;
	int nTargetID = GetShort(pBuf, index);								// Target ID �� ��´�.

	if(nTargetID < USER_BAND || nTargetID >= INVALID_BAND) return;		// �߸��� Target �̸� return

	int sSid = m_UserItem[RIGHT_HAND].sSid;

	if( sSid < 0 || sSid >= g_arItemTable.GetSize() ) return;

	// ���� Delay üũ ---------------------------------------------------------//
	DWORD dwCurrTick = GetTickCount();
	DWORD dwMinDelay = (DWORD)g_arItemTable[sSid]->m_sAttackDelay;
	if(dwMinDelay < DEFAULT_AT_DELAY) dwMinDelay = DEFAULT_AT_DELAY;

	if(m_dwLastAttackTime >= dwCurrTick) return;

	if(dwCurrTick - m_dwLastAttackTime >= dwMinDelay)
	{
		m_dwLastAttackTime = dwCurrTick;
	}

	// ������ �˻� -------------------------------------------------------------//
	if(m_UserItem[RIGHT_HAND].sDuration <= 0)
	{
//		SendAttackFail(ERR_BROKEN);
		SendSystemMsg( IDS_USER_DAMAGED_ITEM, SYSTEM_NORMAL, TO_ME);
		return;
	}

	// Target ���� �Ÿ���� ----------------------------------------------------//
	if(nTargetID >= USER_BAND && nTargetID < NPC_BAND)	// USER : ä�� ������ ���� ������ ������ �� ����
	{
		SendAttackMiss(nTargetID);
		return;
	}
	else if(nTargetID >= NPC_BAND)				// NPC
	{
		pNpc = GetNpc(nTargetID - NPC_BAND);	// NPC Point �� ��´�.
		if(pNpc == NULL) return;				// �߸��� NPC �̸� ����
		if(pNpc->m_NpcState == NPC_DEAD || pNpc->m_sHP <= 0)
		{
			//pNpc->m_Delay = pNpc->SendDead(m_pCom, 0);
			return;// NPC �� �̹� �׾� ������ ����
		}
		if(pNpc->m_tNpcType != 0) return;

		nTPosX = pNpc->m_sCurX;
		nTPosY = pNpc->m_sCurY;
	}

	nAttackRange = GetAttackRange(m_UserItem[RIGHT_HAND].sSid);
	nAttackRange += 1;	// �������� Ÿ�������Ҷ��� +1�� ���ش�.(�̵� �ӵ������� ���� ���� ���ؼ� ��������...)

	if(GetDistance(nTPosX, nTPosY, nAttackRange, &nDist) == FALSE)		// ���ݰ��ɰŸ� ���̸� ����
	{
		if(g_bDebug) SendSystemMsg(_T("### Too Long Distance!!!"), SYSTEM_NORMAL, TO_ME);
		return;
	}

	// ���� �� -------------------------------------------------------//
	int nBasicSuccess = 70;
	int nTotalSuccess = 0;

	if( m_UserItem[RIGHT_HAND].sSid == 699 )
	{
		nBasicSuccess = 80;
	}
	else if( m_UserItem[RIGHT_HAND].sSid == 670 )
	{
		nBasicSuccess = 100;
	}

	nTotalSuccess = (int)( (double)m_sMagicDEX * 0.1 + 0.5 );
	nTotalSuccess += nBasicSuccess;

	if( nTotalSuccess < 100 )
	{
		if( nTotalSuccess < myrand( 1, 100 ) )
		{
			SendAttackMiss(nTargetID);
			return;
		}
	}

	// ȸ�ǰ�/��������/������ ���/������ ���/���� ���ݽð� ����----------//
	if(nTargetID >= USER_BAND && nTargetID < NPC_BAND)	// USER : ������ ä������� �ƴϴ�
	{
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

		pUser = GetUser(m_uid);

		if(pNpc->m_tNpcType == NPCTYPE_GUILD_GUARD)	pNpc->ChangeTarget(pUser, m_pCom);

		// NPC �� 
		nDefense = pNpc->GetDefense();

		nFinalDamage = 1;		// ä�� ������ ĥ���� damage�� 1�̴�

		// ������ ������ ����
		// ������
		sNewDuration = (int)((double)nDefense/10 + 0.5);
		sOldDuration = m_UserItem[RIGHT_HAND].sDuration;

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

		// ������ ���� �����Ƿ� �̺�Ʈ�� ã�Ƽ� ���� ---------------------------------//
		int i, j;
		int event_zone_index = -1;
		int real_event = -1;
		EVENT*	pEvent = NULL;

		if( pNpc->m_sEvent >= MONSTER_ATTACK_EVENT_BAND && m_iDisplayType != 6 )	// ���Ͱ� ������ �ִ� �̺�Ʈ�� ���� �̺�Ʈ���� Ȯ��
		{
			real_event = pNpc->m_sEvent % MONSTER_ATTACK_EVENT_BAND;

			for(i = 0; i < g_event.GetSize(); i++)
			{
				if(g_event[i]->m_Zone == m_curz)
				{				
					event_zone_index = i;
					break;
				}
			}

			if(event_zone_index != -1)// return;			// ���� Zone ���� define �� �̺�Ʈ�� ����.
			{
				pEvent = g_event[event_zone_index];

				if(pEvent)
				{
					if(real_event < pEvent->m_arEvent.GetSize() && real_event > 0)
					{
						if(pEvent->m_arEvent[real_event])
						{
							if(CheckEventLogic(pEvent->m_arEvent[real_event]))// return;	// ���ǰ˻�
							{
								for(j = 0; j < pEvent->m_arEvent[real_event]->m_arExec.GetSize(); j++)
								{
									if(RunNpcEvent(pNpc, pEvent->m_arEvent[real_event]->m_arExec[j]) == NULL) break;
								}
							}
						}
					}
				}
			}
		}

		// Calculate Target HP -------------------------------------------------------//
		short sOldNpcHP = pNpc->m_sHP;

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
				int diffLevel = abs(m_sLevel - pNpc->m_byClassLevel);
				if(diffLevel < 30)
				{
					CheckMaxValue(m_dwXP, 1);		// ���� �������� 1 ����!	
					SendXP();
				}
			}
		}

		// ���� ��� ����
		SendAttackSuccess(nTargetID, bIsCritical, pNpc->m_sHP, pNpc->m_sMaxHP);

		m_dwLastAttackTime = GetTickCount();
	}
}

void USER::SendEventMsg(char *pMsg)
{
	if( strlen( pMsg ) >= 128 ) return;

	SendSystemMsg( pMsg, SYSTEM_ERROR, TO_ME);
}

void USER::WriteItemLog(ItemList *pItem, int type, char *strEtc)
{
	if( !pItem ) return;
	if( !CheckItemLog( pItem ) ) return;

	ITEMLOG* pItemLog = new ITEMLOG;
	pItemLog->t = CTime::GetCurrentTime();
	pItemLog->m_sLogType = type;
	memset( pItemLog->m_strUserId, NULL, CHAR_NAME_LENGTH+1 );
	memcpy( pItemLog->m_strUserId, m_strUserID, CHAR_NAME_LENGTH );

	memset( pItemLog->m_strEtc, NULL, CHAR_NAME_LENGTH+1 );
	if( strEtc != NULL )	memcpy( pItemLog->m_strEtc, strEtc, CHAR_NAME_LENGTH );
	else					memcpy( pItemLog->m_strEtc, m_strUserID, CHAR_NAME_LENGTH );

	pItemLog->m_sLevel		= pItem->sLevel;
	pItemLog->m_sSid		= pItem->sSid;
	pItemLog->m_sDuration	= pItem->sDuration;
	pItemLog->m_sBullNum	= pItem->sBullNum;
	pItemLog->m_sCount		= pItem->sCount;

	for( int i = 0; i < MAGIC_NUM; i++ )
	{
		pItemLog->m_tMagic[i] = pItem->tMagic[i];
	}

	pItemLog->m_tIQ			= pItem->tIQ;

	pItemLog->m_iItemSerial	= pItem->iItemSerial;

	if( !UpdateMemItemLog( pItemLog ) )
	{
	}

	delete pItemLog;
}

BOOL USER::CheckItemLog(ItemList *pItem)
{
	if( !pItem ) return FALSE;

	if( pItem->iItemSerial == 0 ) pItem->iItemSerial = GenerateItemSerial( pItem );
	if( pItem->iItemSerial == 0 ) return FALSE;

	return TRUE;
}

BOOL USER::UpdateMemItemLog(ITEMLOG *pItemLog)
{
	if( !m_pSharedMemory )				return FALSE;
	if( !m_pSharedMemory->m_hMapping )	return FALSE;
	if( !m_pMD )						return FALSE;
	if( m_pMD->m_uid == -1 )			return FALSE;

	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return FALSE;

	for( int i = 0; i < 100; i++ )
	{
		if( m_pMD->m_arItemLogData[i][0] != NULL ) continue;
		memcpy( m_pMD->m_arItemLogData[i], pItemLog, sizeof(ITEMLOG) );
		break;
	}

	return TRUE;
}

void USER::MakeItemLog(ItemList *pItem, int type, char *strEtc)
{
	if( !pItem ) return;
	if( !CheckItemLog( pItem ) ) return;

	ITEMLOG* pItemLog = new ITEMLOG;
	pItemLog->t = CTime::GetCurrentTime();
	pItemLog->m_sLogType = type;
	memset( pItemLog->m_strUserId, NULL, CHAR_NAME_LENGTH+1 );
	memcpy( pItemLog->m_strUserId, m_strUserID, CHAR_NAME_LENGTH );

	memset( pItemLog->m_strEtc, NULL, CHAR_NAME_LENGTH+1 );
	if( strEtc != NULL )	memcpy( pItemLog->m_strEtc, strEtc, CHAR_NAME_LENGTH );
	else					memcpy( pItemLog->m_strEtc, m_strUserID, CHAR_NAME_LENGTH );

	pItemLog->m_sLevel		= pItem->sLevel;
	pItemLog->m_sSid		= pItem->sSid;
	pItemLog->m_sDuration	= pItem->sDuration;
	pItemLog->m_sBullNum	= pItem->sBullNum;
	pItemLog->m_sCount		= pItem->sCount;

	for( int i = 0; i < MAGIC_NUM; i++ )
	{
		pItemLog->m_tMagic[i] = pItem->tMagic[i];
	}

	pItemLog->m_tIQ			= pItem->tIQ;

	pItemLog->m_iItemSerial	= pItem->iItemSerial;

	pItemLog->m_dwResultDN	= 0;
	pItemLog->m_dwInOutDN	= 0;
	pItemLog->m_dwRelateDN	= 0;

	m_arItemLog.Add( pItemLog );
}

void USER::MakeMoneyLog(DWORD dwAddMoney, int type, char *strEtc, DWORD dwChangeMoney)
{
	if( dwAddMoney < 10000 ) return;

	ITEMLOG* pItemLog = new ITEMLOG;
	pItemLog->t = CTime::GetCurrentTime();
	pItemLog->m_sLogType = type;
	memset( pItemLog->m_strUserId, NULL, CHAR_NAME_LENGTH+1 );
	memcpy( pItemLog->m_strUserId, m_strUserID, CHAR_NAME_LENGTH );

	memset( pItemLog->m_strEtc, NULL, CHAR_NAME_LENGTH+1 );
	if( strEtc != NULL )	memcpy( pItemLog->m_strEtc, strEtc, CHAR_NAME_LENGTH );
	else					memcpy( pItemLog->m_strEtc, m_strUserID, CHAR_NAME_LENGTH );

	pItemLog->m_sLevel		= 0;
	pItemLog->m_sSid		= -1;
	pItemLog->m_sDuration	= 0;
	pItemLog->m_sBullNum	= 0;
	pItemLog->m_sCount		= 0;

	for( int i = 0; i < MAGIC_NUM; i++ )
	{
		pItemLog->m_tMagic[i] = 0;
	}

	pItemLog->m_tIQ			= 0;

	pItemLog->m_iItemSerial	= 0;

	pItemLog->m_dwResultDN	= m_dwDN;
	pItemLog->m_dwInOutDN	= dwAddMoney;
	pItemLog->m_dwRelateDN	= dwChangeMoney;

	m_arItemLog.Add( pItemLog );
}

void USER::FlushItemLog(BOOL bSave)
{
	if( !m_arItemLog.GetSize() ) return;

	int i = 0;

	if( bSave )
	{
		for( i = 0; i < m_arItemLog.GetSize(); i++ )
		{
			if( m_arItemLog[i] )
			{
				UpdateMemItemLog( m_arItemLog[i] );
			}
		}
		m_arItemLog.RemoveAll();
	}
	else
	{
		for( i = 0; i < m_arItemLog.GetSize(); i++ )
		{
			if( m_arItemLog[i] )
			{
				delete m_arItemLog[i];
			}
		}
		m_arItemLog.RemoveAll();
	}
}

int USER::GetMinusExpRatioByCityRank(int enemyrank)
{
	if( enemyrank == -1 ) return 10;

	int otherrank = enemyrank;
	int merank = m_sCityRank + CITY_RANK_INTERVAL;

	if( merank < 0 || merank > 11 ) return 10;
	if( otherrank < 0 || otherrank > 11 ) return 10;

	//			 ��  ��
	//			 ��  ��
	int exptable[11][11] = 
	{//                ���� <-  c  -> ����
		{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10	},
		{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10	},
		{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10	},
		{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10	},// ����
		{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10	},//  |
		{	0,  2,	3,	4,	6, 10, 10, 10, 10, 10, 10	},//  c
		{	0,  2,	3,	4,	6, 10, 10, 10, 10, 10, 10	},//  |
		{	0,  2,	3,	4,	6, 10, 10, 10, 10, 10, 10	},// ����
		{	0,  2,	3,	4,	6, 10, 10, 10, 10, 10, 10	},
		{	0,	1,	2,	3,	5, 10, 10, 10, 10, 10, 10	},
		{	0,	1,	2,	3,	5, 10, 10, 10, 10, 10, 10	},
	};

	return exptable[merank][otherrank];
}

int USER::GetDropItemRankByAttackerCityRank(int enemyrank)
{
	if( enemyrank == -1 )
	{
		return m_sCityRank + CITY_RANK_INTERVAL;	// ��븦 ���� �� ���� ��쿣 ���� ��ũ�� �����Ѵ�
	}

	int otherrank = enemyrank;
	int merank = m_sCityRank + CITY_RANK_INTERVAL;

	if( merank < COMMOM_RANK )	// �� ������ ���� ���϶�� ���� ���� ��ũ�� ������� ���� ��ũ�� ����
	{
		return merank;
	}

	if( otherrank == CHAONISE_RANK )	// �� ���� ���� ī���Ͻ���� ���� �������� �ȶ���߸���
	{
		return SAINT_RANK + 1;			// ����Ʈ�� ������ ������ �Ǿ� �����Ƿ� ����Ʈ���� ���� ����� �����Ѵ�
	}

	if( otherrank >= DEATH_RANK && otherrank < COMMOM_RANK )	// ������� ������ ���� ��Ƽ�� �̻��̴�. �׷��� �� ���γ���
	{															// ���� �̻�, ���� �����̸� ������ ����߸� Ȯ���� ����Ʈ�� �����ȴ�
		return SAINT_RANK;
	}

	return merank;		// ���� ��Ƽ�� �̻��̰�, �� ���� �ѵ� ��Ƽ�� �̻��̴�. �׷��ٸ� ���� ��ũ��� ����߸���
}

///////////////////////////////////////////////////////////////////////////////////////
//	Memory DB �� �ִ� ������ ���Ӻ����� �ű��.
//
BOOL USER::Mem2Game(CMemUser *pMD)
{
	if(IsBadReadPtr((CONST VOID*)pMD, sizeof(CMemUser))) return FALSE;

	int i;

	// Copy User Data
	::ZeroMemory( m_strUserID, CHAR_NAME_LENGTH + 1 );
	::CopyMemory(m_strUserID, pMD->m_strUserID, strlen(pMD->m_strUserID));	// User ID
	
	m_sSTR = pMD->m_sSTR;				// ��
	m_sCON = pMD->m_sCON;				// �ǰ�
	m_sDEX = pMD->m_sDEX;				// ��ø
	m_sVOL = pMD->m_sVOL;				// ����
	m_sWIS = pMD->m_sWIS;				// ����

	m_sTempSTR = pMD->m_sTempSTR;		// �ʱ� �Ѱ��� �޾ƿ´�.(�����ٿ��϶� ��)
	m_sTempCON = pMD->m_sTempCON;
	m_sTempDEX = pMD->m_sTempDEX;
	m_sTempVOL = pMD->m_sTempVOL;
	m_sTempWIS = pMD->m_sTempWIS;

	m_iSkin = pMD->m_iSkin;			// �Ǻλ�
	m_iHair = pMD->m_iHair;			// �Ӹ���

	m_curz = pMD->m_curz;				// Zone
	m_curx = pMD->m_curx;				// ��ġ x
	m_cury = pMD->m_cury;				// ��ġ y

	m_sGender = pMD->m_sGender;		// ����

	::CopyMemory(m_strFace, pMD->m_strFace, 10);	// �Ӹ����

	m_dwBuddy = pMD->m_dwBuddy;			// Buddy
	m_dwGuild = pMD->m_dwGuild;			// Guild

	m_tFortressWar		= GUILD_WAR_AFFTER;
	m_tGuildWar			= GUILD_WAR_AFFTER;
	m_tGuildHouseWar	= GUILD_WAR_AFFTER;

	if(m_dwGuild >= 0)			// ��忡 ���� �����͸� �ʱ�ȭ�Ѵ�.
	{
		SetGuildVariable();
	}

	CheckGuildWar();			// ���� ������ ���̶�� ��� �������� �˸���.(������ ��������)

	m_bFieldWarApply = FALSE;
	m_FieldWarGMUid = 0;		// �ʵ����϶� ���� �渶 uid
	m_dwFieldWar = 0;			// �ʵ��� �����̸� ����� ��� ��ȣ�� ���´�.

	m_sLevel	= pMD->m_sLevel;		// ����

	if(pMD->m_dwExp > GetNextLevel(m_sLevel)) pMD->m_dwExp = GetNextLevel(m_sLevel);	// ���� �ڵ�.
	m_dwExp = pMD->m_dwExp;		// Experience

	m_sPA = pMD->m_sPA;			// PA Point

	m_sSkillPoint = pMD->m_sSkillPoint;	// Skill Point

	m_dwXP = pMD->m_dwXP;		// X Point
	
	m_byClass	= pMD->m_byClass;		// Ŭ����

	m_sMaxHP	= g_sHPConst[m_byClass] * m_sCON + g_sHPLV[m_byClass] * (m_sLevel - 1) + g_sHPAdd[m_byClass];
	m_sMaxPP	= g_sPPConst[m_byClass] * m_sWIS + g_sPPLV[m_byClass] * (m_sLevel - 1) + g_sPPAdd[m_byClass];
	m_sMaxSP	= g_sSPConst[m_byClass] * m_sCON + g_sSPLV[m_byClass] * (m_sLevel - 1) + g_sSPAdd[m_byClass];

	m_bLive		= pMD->m_bLive;	// �׾�����, ��Ҵ���?

	m_sHP		= pMD->m_sHP;	// ���� HP
	m_sPP		= pMD->m_sPP;	// ���� PP
	m_sSP		= pMD->m_sSP;	// ���� SP

	if(m_sHP <= 0 || m_bLive == USER_DEAD) 
	{
		m_bLive = USER_DEAD;
		SetZoneIndex(m_curz);
		IsDeadUser();		// ���� ������ ������...

		m_bLive = USER_LIVE;
		m_sHP = m_sMaxHP;	
	}
	else SetZoneIndex(m_curz);

	if(m_sHP > m_sMaxHP) m_sHP = m_sMaxHP;
	if(m_sPP > m_sMaxPP) m_sPP = m_sMaxPP;
	if(m_sSP > m_sMaxSP) m_sSP = m_sMaxSP;

	if(pMD->m_dwDN < 0) pMD->m_dwDN = 0;
	m_dwDN		= pMD->m_dwDN;			// ������

	m_iCityValue = pMD->m_iCityValue;	// �ù� ������
	m_sKillCount = pMD->m_sKillCount;	// PK���� Ƚ��

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

	::CopyMemory(m_strSkill, pMD->m_strSkill, sizeof(m_strSkill));	// ��ų
	::CopyMemory(m_strItem, pMD->m_strItem, sizeof(m_strItem));		// ������
	::CopyMemory(m_strPsi, pMD->m_strPsi, sizeof(m_strPsi));		// ���̿���

	StrToUserSkill((LPTSTR)pMD->m_strSkill);
	StrToUserItem((LPTSTR)pMD->m_strItem);
	StrToUserPsi((LPTSTR)pMD->m_strPsi);
	StrToHaveEventData((LPTSTR)pMD->m_strHaveEvent);
	StrToUserTel((LPSTR)pMD->m_strTel);

	m_dwExpNext	= GetNextLevel(m_sLevel);						// ���������� �Ǳ����� ����ġ

	for(i = 0; i < TOTAL_SKILL_NUM; i++)						// ��ų ������ 
	{
		GetSkillInfo(i, m_UserSkillInfo[i]);
	}
//	m_UserChangeSkillInfo;										// ������ ������� ?(5)��ų�� ����

	for(i = 0; i < CLASS_NUM; i++)
	{
		m_sChangeClass[i] = pMD->m_sChangeClass[i];
	}

	// �����̻� ����
	m_tAbnormalKind = pMD->m_tAbnormalKind;	
	m_dwAbnormalTime = pMD->m_dwAbnormalTime;

//	dwMaxDN = m_sLevel * MAX_LEVEL_DN;									// �������� �̻����� üũ
//	if( m_dwDN > dwMaxDN ) m_dwDN = dwMaxDN; 
	m_tDir = myrand(0, 7);												// ���� �����ִ� ������ �������� �ʱ�ȭ�Ѵ�.
		
	m_tIsOP = pMD->m_tIsOP;
	if(pMD->m_tIsOP == 1) AddAbnormalInfo(OPERATION_MODE);				// �⺻�� ��������...

	CheckInvalidGuildZone();

	m_dwSaintTime = pMD->m_dwSaintTime;

	m_dwHiExpTime		= pMD->m_dwHiExpTime;
	m_dwMagicFindTime	= pMD->m_dwMagicFindTime;
	m_dwNoChatTime		= pMD->m_dwNoChatTime;

	m_dwHasteTime = m_dwShieldTime = m_dwDexUpTime = m_dwMaxHPUpTime = 0;					// Psi One
	m_dwFastRunTime = m_dwMindShockTime = m_dwBigShieldTime = m_dwPsiShieldTime = m_dwPiercingShieldTime = 0;	// Psi One

	m_dwAdamantineTime	= m_dwMightyWeaponTime = m_dwBerserkerTime = 0;						// Psi Two

	m_dwMindGuardTime = 0;																	// Psi Three

	if(pMD->m_tPsiOneKind == PSIONIC_HASTE)				m_dwHasteTime		= pMD->m_dwPsiOneTime;
	if(pMD->m_tPsiOneKind == PSIONIC_SHIELD)			m_dwShieldTime		= pMD->m_dwPsiOneTime;
	if(pMD->m_tPsiOneKind == PSIONIC_DEXUP)				m_dwDexUpTime		= pMD->m_dwPsiOneTime;
	if(pMD->m_tPsiOneKind == PSIONIC_HPUP)				m_dwMaxHPUpTime		= pMD->m_dwPsiOneTime;
	if(pMD->m_tPsiOneKind == PSIONIC_FAST_RUN)			m_dwFastRunTime		= pMD->m_dwPsiOneTime;
	if(pMD->m_tPsiOneKind == PSIONIC_MIND_SHOCK)		m_dwMindShockTime	= pMD->m_dwPsiOneTime;
	if(pMD->m_tPsiOneKind == PSIONIC_PSI_SHIELD)		m_dwPsiShieldTime	= pMD->m_dwPsiOneTime;
    if(pMD->m_tPsiOneKind == 30 )                       m_dwBigShieldTime   = pMD->m_dwPsiOneTime;
	if(pMD->m_tPsiOneKind == PSIONIC_PIERCING_SHIELD)	m_dwPiercingShieldTime = pMD->m_dwPsiOneTime;

	if(pMD->m_tPsiTwoKind == PSIONIC_ADAMANTINE)		m_dwAdamantineTime		= pMD->m_dwPsiTwoTime;
	if(pMD->m_tPsiTwoKind == PSIONIC_MIGHTYWEAPON)		m_dwMightyWeaponTime	= pMD->m_dwPsiTwoTime;
	if(pMD->m_tPsiTwoKind == PSIONIC_BERSERKER)			m_dwBerserkerTime		= pMD->m_dwPsiTwoTime;
	
	if(pMD->m_tPsiThreeKind == PSIONIC_MIND_GUARD)		m_dwMindGuardTime		= pMD->m_dwPsiThreeTime;
	m_dwBSTime=pMD->m_dwBSTime;
	m_dwDNMoney = pMD->m_dwDNMoney;

	DWORD dwCurrTime = GetTickCount();
	m_dwLastSpeedTime = dwCurrTime;
	m_dwLastMoveAndRun = dwCurrTime;
	m_dwLastAttackTime = dwCurrTime;
	m_dwLastPsiAttack = dwCurrTime;
	m_dwCastDelay = 0;
	m_dwLastTimeCount = dwCurrTime;
	m_dwLastAbnormalTime = dwCurrTime;
	m_dwLastHPTime = dwCurrTime;

	m_dwLastHasteTime = dwCurrTime;
	m_dwLastShieldTime = dwCurrTime;
	m_dwLastDexUpTime = dwCurrTime;
	m_dwLastMaxHPUpTime = dwCurrTime;

	m_dwLastHiExpTime		= dwCurrTime;
	m_dwLastMagicFindTime	= dwCurrTime;
	m_dwLastNoChatTime		= dwCurrTime;

	m_dwLastAdamantineTime	= dwCurrTime;
	m_dwLastMightyWeaponTime= dwCurrTime;
	m_dwBerserkerTime		= dwCurrTime;

	m_iMyServer = pMD->m_iMyServer;

	GetRecoverySpeed();													// ���� Ŭ������ ȸ���ӵ��� �����Ѵ�.

	// User Bank --------------------------------
	StrToUserBankItem((LPTSTR)pMD->m_UB.m_UserBankItem);
	m_dwBankDN = pMD->m_UB.m_dwBankDN;

	// Account Bank -----------------------------
	StrToAccountBankItem((LPTSTR)pMD->m_AB.m_AccountBankItem);
	m_dwAccountBankDN = pMD->m_AB.m_dwBankDN;

	return TRUE;
}

int USER::CheckMemoryAccountBankDB(char *strAccount)
{
	CSharedMemory*	pSharedMemory;
	CMemUser*		pMD;
	int				mem_index = -1;
	int				nCount = 0;
	CString			str;

	SYSTEMTIME time;
	GetLocalTime(&time);

	for( int i = 0; i < MAX_USER; i++ )
	{
		pSharedMemory = NULL;
		pMD = NULL;

		pSharedMemory = g_arSharedMemory[i];
		if(!pSharedMemory) continue;

		pMD = (CMemUser*)pSharedMemory->m_lpData;
		if(!pMD) continue;

		if(pMD->m_uid == -1 || pMD->m_sSTR == 0) continue;		// ������ �� ������ �ٽ� �������� �ʴ´�.
		if(pMD->m_strUserID[0] == 0) continue;

		if( _stricmp( pMD->m_strAccount, strAccount ) == 0 )
		{
			nCount++;
			// ���Ͽ� ����
			if(nCount >= 1)
			{
				GetLocalTime(&time);
				str.Format("(%04d-%02d-%02d %02d:%02d:%02d)\tuid-%d str-%d ubuid-%d count-%d id-%s (AccountBank)\r\n",
					time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
					pMD->m_uid, pMD->m_sSTR, pMD->m_AB.m_uid, nCount, pMD->m_strAccount );
				
				// IKING 2002.1.
				EnterCriticalSection( &m_CS_FileWrite );
				g_fpBackServer.Write( str, str.GetLength() );
				LeaveCriticalSection( &m_CS_FileWrite );
			}

//			if( !Mem2GameAccountBank(pMD) )			// Shared Memory -> Game ������
//			{
//				nCount--;
//			}

			UpdateMem2DB(pMD);		// ������ ���ؼ� DB���� ������
			UpdateBankMem2DB(pMD);
			UpdateAccountBankMem2DB(pMD);

			pMD->m_uid = -1;
			pMD->m_sSTR = 0;			
			pMD->m_UB.m_uid = -1;
			pMD->m_AB.m_uid = -1;
			pMD->m_dwSaveTime = 0;
			::ZeroMemory(pMD->m_strUserID, sizeof(pMD->m_strUserID));
			::ZeroMemory(pMD->m_strAccount, sizeof(pMD->m_strAccount));
		}
	}

	return nCount;
}

BOOL USER::Mem2GameAccountBank(CMemUser *pMD)
{
	if(IsBadReadPtr((CONST VOID*)pMD, sizeof(CMemUser))) return FALSE;

	// Account Bank -----------------------------
	StrToAccountBankItem((LPTSTR)pMD->m_AB.m_AccountBankItem);
	m_dwAccountBankDN = pMD->m_AB.m_dwBankDN;

	return TRUE;
}

BOOL USER::GiveMagicItem(int sid, int iCount)
{
	int tMagic[12]={36,81,141,33,76,132,97,102,100,130,79,139};
	if( sid < 0 || sid >= g_arItemTable.GetSize() ) return FALSE;
	if( iCount <= 0 ) return FALSE;

	ItemList GiveItem;
	ReSetItemSlot( &GiveItem );
	if(sid==809||sid==811||sid==812||sid==810){ //������װ 50Ѫ10�� �ر���6�� 5��
		int iRandom1 =(myrand(0,15)%12);
		int iRandom2 =(myrand(0,15)%12);
		CWordArray		arEmptySlot, arSameSlot;
		int iSlot = GetEmptySlot( INVENTORY_SLOT );
		
		if( iSlot < 0 ) return FALSE;

		ReSetItemSlot( &m_UserItem[iSlot]);

		m_UserItem[iSlot].sLevel = g_arItemTable[sid]->m_byRLevel;
		m_UserItem[iSlot].sSid = sid;
		m_UserItem[iSlot].sCount = iCount;
		m_UserItem[iSlot].sDuration = g_arItemTable[sid]->m_sDuration;
		m_UserItem[iSlot].sBullNum = 3;
		m_UserItem[iSlot].tMagic[0] = tMagic[iRandom1];
		m_UserItem[iSlot].tMagic[1] = tMagic[iRandom2];
		m_UserItem[iSlot].tIQ = MAGIC_ITEM;
		m_UserItem[iSlot].iItemSerial = 0;

		MakeItemLog( &m_UserItem[iSlot], ITEMLOG_EVENT_GIVE );

		arEmptySlot.Add(iSlot); 
		UpdateInvenSlot(&arEmptySlot, &arSameSlot);
		return true;
	}
	if( g_arItemTable[sid]->m_bySpecial == 0 ) return FALSE;		// �������� ���� �Ӽ��� ����

	GiveItem.sLevel			= g_arItemTable[sid]->m_byRLevel;
	GiveItem.sSid			= sid;
	GiveItem.sCount			= iCount;
	GiveItem.sDuration		= g_arItemTable[sid]->m_sDuration;
	GiveItem.sBullNum		= g_arItemTable[sid]->m_sBullNum;
	GiveItem.tMagic[0]		= g_arItemTable[sid]->m_bySpecial;
	GiveItem.tIQ			= MAGIC_ITEM;

	CWordArray		arEmptySlot, arSameSlot;

	int iSlot = GetSameItem( GiveItem, INVENTORY_SLOT);

	if(iSlot != -1)	
	{ 
		if(iCount != 0)
		{
			CheckMaxValue((short &)m_UserItem[iSlot].sCount, (short)iCount); 
			arSameSlot.Add(iSlot); 
		}
//		return FALSE;		// ���� �������� ��ġ�Ⱑ �ȵǴ� ������ �Ǵ��Ѵ�
	}
	else			
	{
		iSlot = GetEmptySlot( INVENTORY_SLOT );

		if( iSlot < 0 ) return FALSE;

		ReSetItemSlot( &m_UserItem[iSlot] );

		m_UserItem[iSlot].sLevel = g_arItemTable[sid]->m_byRLevel;
		m_UserItem[iSlot].sSid = sid;
		m_UserItem[iSlot].sCount = iCount;
		m_UserItem[iSlot].sDuration = g_arItemTable[sid]->m_sDuration;
		m_UserItem[iSlot].sBullNum = g_arItemTable[sid]->m_sBullNum;
		m_UserItem[iSlot].tMagic[0] = g_arItemTable[sid]->m_bySpecial;
		m_UserItem[iSlot].tIQ = MAGIC_ITEM;
		m_UserItem[iSlot].iItemSerial = 0;

		MakeItemLog( &m_UserItem[iSlot], ITEMLOG_EVENT_GIVE );

		arEmptySlot.Add(iSlot); 
	}			

	UpdateInvenSlot(&arEmptySlot, &arSameSlot);

//	GetMagicItemSetting();		// ���� ���� �������� �ִ� �� �����δ� ���� �ɼ��� �ȹٲ�� : jjs07
/*
	int iSlot = GetEmptySlot(INVENTORY_SLOT);

	if(iSlot < 0) return FALSE;

	ReSetItemSlot(&m_UserItem[iSlot]);

	m_UserItem[iSlot].sLevel = g_arItemTable[sid]->m_byRLevel;
	m_UserItem[iSlot].sSid = sid;
	m_UserItem[iSlot].sCount = iCount;
	m_UserItem[iSlot].sDuration = g_arItemTable[sid]->m_sDuration;
	m_UserItem[iSlot].sBullNum = g_arItemTable[sid]->m_sBullNum;
	m_UserItem[iSlot].tIQ = NORMAL_ITEM;
*/

	FlushItemLog( TRUE );

	return TRUE;
}

void USER::EventItemSerialWindowOpen()
{
	CBufferEx TempBuf;
	
	TempBuf.Add(PRE_REG_USER_EVENT_REQ);

	Send(TempBuf, TempBuf.GetLength());
}

void USER::RecvEventItemSerial(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int j;
	int iSlot = 0;
	int iRet = 0;
	int index = 0;
	int iEventItemSid = 0, iEvent = 0;

	TCHAR strSerial[30];	::ZeroMemory(strSerial,	sizeof(strSerial));
	
	iSlot = GetEmptySlot(INVENTORY_SLOT);

	CString strMsg;

	if(iSlot <= -1)
	{
		strMsg.Format( IDS_USER_NEED_EMPTY_SLOT_CHANGE );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;		// �κ��� ��� �ִ��� Ȯ���Ѵ�.
	}

	if(!GetVarString(sizeof(strSerial), strSerial, pBuf, index)) return;

	iRet = UpdateEventItemSerial(strSerial, iEventItemSid);

	if(iRet < 0)
	{
		// DB�� ������ ���� �ʽ��ϴ�.
		strMsg.Format( IDS_USER_UNSTABLE_CONNECTION );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}
	else if(iRet == 1)
	{
		if(iEventItemSid == 0)
		{
			strMsg.Format( IDS_USER_EVENT_COMMON_CURE );

			m_UserItem[iSlot].tType = TYPE_ITEM;
			m_UserItem[iSlot].sLevel = g_arItemTable[NPC_EVENT_B_ITEM]->m_byRLevel;
			m_UserItem[iSlot].sSid = g_arItemTable[NPC_EVENT_B_ITEM]->m_sSid;
			m_UserItem[iSlot].sCount = 1000;
			m_UserItem[iSlot].sDuration = g_arItemTable[NPC_EVENT_B_ITEM]->m_sDuration;
			m_UserItem[iSlot].sBullNum = g_arItemTable[NPC_EVENT_B_ITEM]->m_sBullNum;
			m_UserItem[iSlot].tIQ = 0;
			m_UserItem[iSlot].iItemSerial = 0;

			for(j = 0; j < MAGIC_NUM; j++)
			{
				m_UserItem[iSlot].tMagic[j] = 0;
			}

			CBufferEx TempBuf;

			TempBuf.Add(ITEM_LOAD_RESULT);
			TempBuf.Add(SUCCESS);
			TempBuf.Add((BYTE)0x01);

			TempBuf.Add((BYTE)iSlot);
			TempBuf.Add(m_UserItem[iSlot].sLevel);
			TempBuf.Add(m_UserItem[iSlot].sSid);
			TempBuf.Add(m_UserItem[iSlot].sDuration);
			TempBuf.Add(m_UserItem[iSlot].sBullNum);
			TempBuf.Add(m_UserItem[iSlot].sCount);
			for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_UserItem[iSlot].tMagic[j]);

			TempBuf.Add((BYTE)m_UserItem[iSlot].tIQ);

			Send(TempBuf, TempBuf.GetLength());
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

			m_iCurWeight += g_arItemTable[NPC_EVENT_B_ITEM]->m_byWeight * 1000;
			GetRecoverySpeed();			// ������ ���Կ� ������ ����� ȸ���ӵ� ��ȯ
			return;
		}
		else if(iEventItemSid > 100 && iEventItemSid <= 255)
		{
			switch(iEventItemSid)
			{
			case EVENT_ATT7_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_ATT7_CHANGE );
				break;
			case EVENT_DEF7_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_DEF7_CHANGE );
				break;
			case EVENT_ATT6_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_ATT6_CHANGE );
				break;
			case EVENT_DEF6_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_DEF6_CHANGE );
				break;
			case EVENT_ATT_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_ATT5_CHANGE );
				break;
			case EVENT_DEF_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_DEF5_CHANGE );
				break;
			case EVENT_INIT_STAT_ITEM:
				iEvent = NPC_EVENT_INIT_STAT; 
				strMsg.Format( IDS_USER_EVENT_RESET_STAT );
				break;
			default:
				return;
				break;
			}

			m_UserItem[iSlot].tType = TYPE_ITEM;
			m_UserItem[iSlot].sLevel = g_arItemTable[iEvent]->m_byRLevel;
			m_UserItem[iSlot].sSid = g_arItemTable[iEvent]->m_sSid;
			m_UserItem[iSlot].sCount = 1;
			m_UserItem[iSlot].sDuration = g_arItemTable[iEvent]->m_sDuration;
			m_UserItem[iSlot].sBullNum = g_arItemTable[iEvent]->m_sBullNum;
			m_UserItem[iSlot].tIQ = (BYTE)iEventItemSid;

			for(j = 0; j < MAGIC_NUM; j++)
			{
				m_UserItem[iSlot].tMagic[j] = 0;
			}

			CBufferEx TempBuf;

			TempBuf.Add(ITEM_LOAD_RESULT);
			TempBuf.Add(SUCCESS);
			TempBuf.Add((BYTE)0x01);

			TempBuf.Add((BYTE)iSlot);
			TempBuf.Add(m_UserItem[iSlot].sLevel);
			TempBuf.Add(m_UserItem[iSlot].sSid);
			TempBuf.Add(m_UserItem[iSlot].sDuration);
			TempBuf.Add(m_UserItem[iSlot].sBullNum);
			TempBuf.Add(m_UserItem[iSlot].sCount);
			for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_UserItem[iSlot].tMagic[j]);

			TempBuf.Add((BYTE)m_UserItem[iSlot].tIQ);

			Send(TempBuf, TempBuf.GetLength());
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

			m_iCurWeight += g_arItemTable[iEvent]->m_byWeight;
			GetRecoverySpeed();			// ������ ���Կ� ������ ����� ȸ���ӵ� ��ȯ

			return;
		}
	}
	else	// �ø����� Ʋ���ų� ��Ͽ� ���� �����ų� DB�� ������ ���� ���
	{
		if(iRet == 10)
		{
			strMsg.Format( IDS_USER_CHECK_INPUT_NUMBER );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		else if(iRet == 11)
		{
			strMsg.Format( IDS_USER_NOT_SUBSCRIPTION );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		else if(iRet == 12)
		{
			strMsg.Format( IDS_USER_ONE_MORE_APPLY );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		else if(iRet == 13)
		{
			strMsg.Format( IDS_USER_ALREADY_LOTTO );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		else if(iRet == 14)
		{
			strMsg.Format( IDS_USER_CANCEL_LOTTO );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
	}
}

int USER::UpdateEventItemSerial(TCHAR *pstrSerial, int &item)
{
	short sItemType = 0;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	SWORD	sParam1 = 0;
	SDWORD	cbParam1 = SQL_NTS, cbParam2 = SQL_NTS;

	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_PRE_USER_EVENT (\'%s\', \'%s\', \'%s\',\'%s\',?,?)}"), 
		m_strAccount, pstrSerial, g_arServerIPAddr, m_strUserID, sParam1, sItemType);

	hstmt = NULL;
	retcode = 0;

	int db_index = -1;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) 
	{
		return -1;
	}
	
	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
		return -1;
	}
	
	if( retcode != SQL_SUCCESS )
	{
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		return -1;
	}

	retcode = SQLBindParameter(hstmt, 1,SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT,0,0,&sParam1,0,&cbParam1);
	retcode = SQLBindParameter(hstmt, 2,SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT,0,0,&sItemType,0,&cbParam2);

    retcode = SQLExecDirect (hstmt, (unsigned char *)(LPCTSTR)szSQL, SQL_NTS);
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

	item = (int)sItemType;
	return sParam1;
}


void USER::EventItemSerialForGameRoomWindowOpen()
{
	CBufferEx TempBuf;
	
	TempBuf.Add(PRE_REG_GAMEROOM_EVENT_REQ);

	Send(TempBuf, TempBuf.GetLength());
}

int USER::UpdateEventItemSerialForGameRoom(TCHAR *pstrSerial, TCHAR *pstrSocNum, int &item)
{
	short sItemType = 0;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	SWORD	sParam1 = 0;
	SDWORD	cbParam1 = SQL_NTS, cbParam2 = SQL_NTS;

	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_PRE_GAMEROOM_EVENT (\'%s\', \'%s\', \'%s\', \'%s\',\'%s\',?,?)}"), 
		pstrSocNum, m_strAccount, pstrSerial, g_arServerIPAddr, m_strUserID, sParam1, sItemType);

	hstmt = NULL;
	retcode = 0;

	int db_index = -1;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) 
	{
		return -1;
	}
	
	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
		return -1;
	}
	
	if( retcode != SQL_SUCCESS )
	{
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		return -1;
	}

	retcode = SQLBindParameter(hstmt, 1,SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT,0,0,&sParam1,0,&cbParam1);
	retcode = SQLBindParameter(hstmt, 2,SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT,0,0,&sItemType,0,&cbParam2);

    retcode = SQLExecDirect (hstmt, (unsigned char *)(LPCTSTR)szSQL, SQL_NTS);
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

	item = (int)sItemType;
	return sParam1;
}

void USER::RecvEventItemSerialForGameRoom(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int j;
	int iSlot = 0;
	int iRet = 0;
	int index = 0;
	int iEventItemSid = 0, iEvent = 0;

	TCHAR strSerial[30];		::ZeroMemory(strSerial,	sizeof(strSerial));
	TCHAR strSocNum[30];		::ZeroMemory(strSocNum,	sizeof(strSocNum));
	
	iSlot = GetEmptySlot(INVENTORY_SLOT);

	CString strMsg;

	if(iSlot <= -1)
	{
		strMsg.Format( IDS_USER_NEED_EMPTY_SLOT_CHANGE );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;		// �κ��� ��� �ִ��� Ȯ���Ѵ�.
	}
	
	if((m_iMaxWeight + m_iMaxWeight) < m_iCurWeight) // ���� ���԰� �ִ� ������ 2���̻��̸�...
	{
		strMsg.Format( IDS_USER_OVER_WEIGHT2 );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;		// �κ��� ��� �ִ��� Ȯ���Ѵ�.
	}

	if(!GetVarString(sizeof(strSerial),	strSerial, pBuf, index))
	{
		strMsg.Format(IDS_USER_CHECK_INPUT_NUMBER);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}
	if(!GetVarString(sizeof(strSocNum), strSocNum,	pBuf, index))
	{
		strMsg.Format(IDS_USER_CHECK_INPUT_NUMBER);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}

	iRet = UpdateEventItemSerialForGameRoom(strSerial, strSocNum, iEventItemSid);

	if(iRet < 0)
	{
		// DB�� ������ ���� �ʽ��ϴ�.
		strMsg.Format( IDS_USER_UNSTABLE_CONNECTION );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}
	else if(iRet == 1)
	{
		if(iEventItemSid == 0)
		{
			strMsg.Format( IDS_USER_EVENT_COMMON_CURE );

			m_UserItem[iSlot].tType = TYPE_ITEM;
			m_UserItem[iSlot].sLevel = g_arItemTable[NPC_EVENT_B_ITEM]->m_byRLevel;
			m_UserItem[iSlot].sSid = g_arItemTable[NPC_EVENT_B_ITEM]->m_sSid;
			m_UserItem[iSlot].sCount = 1000;
			m_UserItem[iSlot].sDuration = g_arItemTable[NPC_EVENT_B_ITEM]->m_sDuration;
			m_UserItem[iSlot].sBullNum = g_arItemTable[NPC_EVENT_B_ITEM]->m_sBullNum;
			m_UserItem[iSlot].tIQ = 0;

			for(j = 0; j < MAGIC_NUM; j++)
			{
				m_UserItem[iSlot].tMagic[j] = 0;
			}

			CBufferEx TempBuf;

			TempBuf.Add(ITEM_LOAD_RESULT);
			TempBuf.Add(SUCCESS);
			TempBuf.Add((BYTE)0x01);

			TempBuf.Add((BYTE)iSlot);
			TempBuf.Add(m_UserItem[iSlot].sLevel);
			TempBuf.Add(m_UserItem[iSlot].sSid);
			TempBuf.Add(m_UserItem[iSlot].sDuration);
			TempBuf.Add(m_UserItem[iSlot].sBullNum);
			TempBuf.Add(m_UserItem[iSlot].sCount);
			for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_UserItem[iSlot].tMagic[j]);

			TempBuf.Add((BYTE)m_UserItem[iSlot].tIQ);

			Send(TempBuf, TempBuf.GetLength());
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

			m_iCurWeight += g_arItemTable[NPC_EVENT_B_ITEM]->m_byWeight * 1000;
			GetRecoverySpeed();			// ������ ���Կ� ������ ����� ȸ���ӵ� ��ȯ
			return;
		}
		else if(iEventItemSid > 100 && iEventItemSid <= 255)
		{
			switch(iEventItemSid)
			{
			case EVENT_ATT7_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_ATT7_CHANGE );
				break;
			case EVENT_DEF7_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_DEF7_CHANGE );
				break;
			case EVENT_ATT6_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_ATT6_CHANGE );
				break;
			case EVENT_DEF6_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_DEF6_CHANGE );
				break;
			case EVENT_ATT_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_ATT5_CHANGE );
				break;
			case EVENT_DEF_ITEM:
				iEvent = NPC_EVENT_ITEM; 
				strMsg.Format( IDS_USER_EVENT_DEF5_CHANGE );
				break;
			case EVENT_INIT_STAT_ITEM:
				iEvent = NPC_EVENT_INIT_STAT; 
				strMsg.Format( IDS_USER_EVENT_RESET_STAT );
				break;
			default:
				return;
				break;
			}

			m_UserItem[iSlot].tType = TYPE_ITEM;
			m_UserItem[iSlot].sLevel = g_arItemTable[iEvent]->m_byRLevel;
			m_UserItem[iSlot].sSid = g_arItemTable[iEvent]->m_sSid;
			m_UserItem[iSlot].sCount = 1;
			m_UserItem[iSlot].sDuration = g_arItemTable[iEvent]->m_sDuration;
			m_UserItem[iSlot].sBullNum = g_arItemTable[iEvent]->m_sBullNum;
			m_UserItem[iSlot].tIQ = (BYTE)iEventItemSid;

			for(j = 0; j < MAGIC_NUM; j++)
			{
				m_UserItem[iSlot].tMagic[j] = 0;
			}

			CBufferEx TempBuf;

			TempBuf.Add(ITEM_LOAD_RESULT);
			TempBuf.Add(SUCCESS);
			TempBuf.Add((BYTE)0x01);

			TempBuf.Add((BYTE)iSlot);
			TempBuf.Add(m_UserItem[iSlot].sLevel);
			TempBuf.Add(m_UserItem[iSlot].sSid);
			TempBuf.Add(m_UserItem[iSlot].sDuration);
			TempBuf.Add(m_UserItem[iSlot].sBullNum);
			TempBuf.Add(m_UserItem[iSlot].sCount);
			for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_UserItem[iSlot].tMagic[j]);

			TempBuf.Add((BYTE)m_UserItem[iSlot].tIQ);

			Send(TempBuf, TempBuf.GetLength());
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

			m_iCurWeight += g_arItemTable[iEvent]->m_byWeight;
			GetRecoverySpeed();			// ������ ���Կ� ������ ����� ȸ���ӵ� ��ȯ

			return;
		}
	}
	else	// �ø����� Ʋ���ų� ��Ͽ� ���� �����ų� DB�� ������ ���� ���
	{
		if(iRet == 10)
		{
			strMsg.Format( IDS_USER_CHECK_LOTTO_NUMBER );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		else if(iRet == 11)
		{
			strMsg.Format( IDS_USER_CHECK_SOCIAL_NUMBER );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		else if(iRet == 12)
		{
			strMsg.Format( IDS_USER_ONE_MORE_APPLY );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
		else if(iRet == 13)
		{
			strMsg.Format( IDS_USER_ALREADY_LOTTO );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//
// ���� ������ ���� ü�� �Ⱓ�� �Ѿ����� üũ�Ѵ�.
BOOL USER::CheckUserLevel()
{
//	if(m_iDisplayType == 6 && m_sLevel > 25)//yskang 0.5
	if(m_iDisplayType == 6)
	{			
		
		CString strMsg = _T("");
		strMsg.Format("TEST �������� �����Ͻ� ü���� �������� ��ɻ� ������ �ֽ��ϴ�.");
//		SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_NORMAL, TO_ME);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);

	}

	return FALSE;
}

void USER::GiveEventGameTime(int item_index, int quality)
{
	int sid = item_index;
	int num = 1;

	int i, j;
	int iSlot = -1;

	CBufferEx TempBuf;
	BYTE result = SUCCESS;

	if(sid < 0 || sid >= g_arItemTable.GetSize()) return;

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
//			for(j = 0; j < MAGIC_NUM; j++)
//			{
//				if(m_UserItem[i].tMagic[j] != TempItem.tMagic[j]) break;
//			}
				
			if(m_UserItem[i].tIQ == TempItem.tIQ) { iSlot = i; break; } 
		}
	}

	CString strMsg = _T("");

	if(iSlot == -1)
	{
		strMsg.Format( IDS_USER_FAIL_CHECK_INVEN );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(m_UserItem[iSlot].sSid < 0)
	{
		strMsg.Format( IDS_USER_FAIL_CHECK_INVEN );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}

	if(m_UserItem[iSlot].sCount < num)		
	{
		strMsg.Format( IDS_USER_FAIL_CHECK_INVEN );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}

	int nRet = 0;
	CString strSerial = _T("");
	strSerial.Format("%d-%d-%d-%d-%d-%d", m_UserItem[iSlot].tMagic[0],m_UserItem[iSlot].tMagic[1],m_UserItem[iSlot].tMagic[2],m_UserItem[iSlot].tMagic[3],m_UserItem[iSlot].tMagic[4],m_UserItem[iSlot].tMagic[5]);

	nRet = UpdateGameTimeForEvent((LPTSTR)(LPCTSTR)strSerial);

	if(nRet != 1)
	{
		if(nRet == 2)
		{
			strMsg.Format( IDS_USER_ONE_MORE_APPLY );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			return;
		}
		else if(nRet == 3)
		{
			strMsg.Format( IDS_USER_ALREADY_REG_NUMBER );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			return;
		}
		else
		{
			strMsg.Format( IDS_USER_FAIL_CHECK_INVEN );
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			return;
		}
	}

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

	strMsg.Format( IDS_USER_YOUR_ACCOUNT_REG, m_strAccount);
	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
}

//////////////////////////////////////////////////////////////////////
//
//	�̺�Ʈ���� ���� ���ױ� ��ǰ���� ȸ���Ѵ�.
//
int USER::UpdateGameTimeForEvent(TCHAR *pstrSerial)
{
	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	SWORD	sParam1 = 0;
	SDWORD	cbParam1 = SQL_NTS;

	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_EVENT_GAMETIME_USER(\'%s\', \'%s\', \'%s\', \'%s\',?)}"), 
		m_strAccount, pstrSerial, g_arServerIPAddr, m_strUserID, sParam1);
 
	hstmt = NULL;
	retcode = 0;

	int db_index = -1;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) 
	{
		return -1;
	}
	
	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
		return -1;
	}
	
	if( retcode != SQL_SUCCESS )
	{
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		return -1;
	}

	retcode = SQLBindParameter(hstmt, 1,SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT,0,0,&sParam1,0,&cbParam1);

    retcode = SQLExecDirect (hstmt, (unsigned char *)(LPCTSTR)szSQL, SQL_NTS);
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

	return sParam1;
}

void USER::SendSystemMsg(UINT strID, BYTE type, int nWho)
{
	CBufferEx TempBuf;

	char pMsg[1280];	memset( pMsg, NULL, 1280 );
	strcpy( pMsg, _ID( strID ) );

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

////////////////////////////////////////////////////////////////////////////////
//	������ Ư���� �ٲٴ� �������̽� Ȱ��ȭ
//
void USER::UpgradeItemOpen(int event_num)
{
	CBufferEx TempBuf;
	//static int a=0;

	TempBuf.Add(UPGRADE_ITEM_DIALOG);
	TempBuf.Add(BYTE(event_num));
	//TempBuf.Add(BYTE(a));
	//a++;
	Send(TempBuf, TempBuf.GetLength());
}

void USER::UpgradeItemReqBegin(TCHAR *pBuf)
{
	int index = 0;

	BYTE tType = GetByte(pBuf, index);	// ���׷��̵� Ÿ��
	m_bNoItemMove = TRUE;
	switch(tType)
	{
	case 0:				// ��ͨ����
		UpgradeItemReq(pBuf + index);
		break;
	case 1:				////���κϳ�
		UpgradeAccessoriReq(pBuf + index);
		break;

	case 2: case 3: case 12: case 9: 		// 2 ������ʯ ������ʯ   3��ʯ��ʯ  12 ����ʯͷ���� 9ף����ʯ
		UpgradeBlessingItemReq(pBuf + index, tType);
		break;

	case 4:				// 1�ܰ���, 2�ܰ���, 3�ܰ���
		RemodelingItemReq(pBuf + index);
		break;

	case 5:				//ħ��ʥװ
		RemagicItemReq(pBuf + index);
		break;

	case 6:				// ������ (�ߺ��ɼ� �����)
		RemagicItemReq(pBuf + index, TRUE);
		break;

	case 7:				//ǿ����е 
		EBodyUpgradeReq(pBuf + index);
		break;
	case 8: //�ػ�����.
		ShouHouUpgradeReq(pBuf + index);
		break;
	case 11:
		DownAccessoriReq(pBuf + index);
		break;
	case 14://��������оƬ.
		ShouHouMake(pBuf + index);
		break;
	case 15:
		ShouHouAdd(pBuf + index);
		break;
	case 16://�ټ�ת��
		ItemConvert(pBuf + index);
		break;
	case 17:
		RemagicItem_100_3(pBuf + index);
		break;
	case 18:
		RemagicItem_UpgradeReq(pBuf + index);//�������
		break;
	case 19:
		RemagicItem1_UpgradeReq(pBuf + index);//�������
		break;
	case 20:
		RemagicItem_HuanShi(pBuf + index);
		break;
	case 21: //���߲��ϻ�
		RemagicItem_100_1(pBuf + index);
		break;
	case 22://������߲��ϻ�
		RemagicItem_100_2(pBuf + index);
		break;
	case 37: //��Ʒ�ϳ�  5����ʯ�ϳ�һ��������  ����10W  2��е�ϳ�һ����ʯ,����200W  2��ʯ�ϳ�һ��ѩ�����100W
		ItemExchange(pBuf + index);
		break;
	default: 
		break;
	}
	m_bNoItemMove = FALSE;
}

void USER::ItemExchange(TCHAR *pBuf)
{
	int index=0,sSid;
	short Slot = GetShort(pBuf, index);//
	short Slot1 = GetShort(pBuf, index);//
	short Slot2 = GetShort(pBuf, index);//
	short Slot3 = GetShort(pBuf, index);//
	short Slot4 = GetShort(pBuf, index);//
	short newSlot= GetEmptySlot(INVENTORY_SLOT);
	CUIntArray arMaterial;

	if(Slot < EQUIP_ITEM_NUM || Slot >= TOTAL_INVEN_MAX) return;
	if((Slot1!=-1) && (Slot1 < EQUIP_ITEM_NUM || Slot1 >= TOTAL_INVEN_MAX))return;
	if((Slot2!=-1) && (Slot2 < EQUIP_ITEM_NUM || Slot2 >= TOTAL_INVEN_MAX))return;
	if((Slot3!=-1) && (Slot3 < EQUIP_ITEM_NUM || Slot3 >= TOTAL_INVEN_MAX))return;
	if((Slot4!=-1) && (Slot4 < EQUIP_ITEM_NUM || Slot4 >= TOTAL_INVEN_MAX))return;
	if(m_UserItem[Slot].sSid==-1) return;
	if((Slot1!=-1) && (m_UserItem[Slot1].sSid==-1)) return;
//5����ʯ�ϳ�һ��������  ����10W
	if(m_UserItem[Slot].sSid==847 || m_UserItem[Slot].sSid==848){
		if(m_UserItem[Slot].sCount<5) return ;
		if(m_UserItem[Slot].sCount==5) newSlot=Slot;
		if(newSlot==-1) {SendSystemMsg( "û���㹻λ�ô����Ʒ", SYSTEM_NORMAL, TO_ME); return ;};
		
		if(m_dwDN < 100000)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
			return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
		}
		if( m_dwDN <= 100000 ) m_dwDN = 0;
		else m_dwDN = m_dwDN - 100000;
		SendMoneyChanged();

		sSid=m_UserItem[Slot].sSid-2;
		m_UserItem[Slot].sCount=m_UserItem[Slot].sCount-5;
		
		if(m_UserItem[Slot].sCount==0){
				ReSetItemSlot(&m_UserItem[Slot]);
		}
		ReSetItemSlot(&m_UserItem[newSlot]);
		m_UserItem[newSlot].sLevel = g_arItemTable[sSid]->m_byRLevel;
		m_UserItem[newSlot].sSid = sSid;
		m_UserItem[newSlot].sCount = 1;
		m_UserItem[newSlot].sDuration = g_arItemTable[sSid]->m_sDuration;
		m_UserItem[newSlot].sBullNum = 0;
		m_UserItem[newSlot].tIQ = 0;
		m_UserItem[newSlot].iItemSerial = 0;
		
		arMaterial.Add(Slot);
		if(Slot!=newSlot) arMaterial.Add(newSlot);
	}else if(g_arItemTable[m_UserItem[Slot].sSid]->m_byWear>=122 && g_arItemTable[m_UserItem[Slot].sSid]->m_byWear<=125 ){
		if(Slot1==-1) return ;
		if(g_arItemTable[m_UserItem[Slot1].sSid]->m_byWear<122 || g_arItemTable[m_UserItem[Slot1].sSid]->m_byWear>125)
			return ;
		if(m_dwDN < 2000000)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
			return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
		}
		if( m_dwDN <= 2000000 ) m_dwDN = 0;
		else m_dwDN = m_dwDN - 2000000;
		SendMoneyChanged();
		ReSetItemSlot(&m_UserItem[Slot]);
		ReSetItemSlot(&m_UserItem[Slot1]);
		newSlot=Slot;
		
		int tEBodySid,i;
		int iRandom = myrand(1, 1000);
		for(i = 0; i < g_arEBodyTable.GetSize(); i++)
		{
			if(iRandom <= g_arEBodyTable[i]->m_sRandom) 
			{
					tEBodySid = g_arEBodyTable[i]->m_tSid;
					break;
			}	
		}
		m_UserItem[newSlot].sLevel = g_arItemTable[907]->m_byRLevel;
		m_UserItem[newSlot].sSid = 907;
		m_UserItem[newSlot].sCount = 1;
		m_UserItem[newSlot].sDuration = g_arItemTable[907]->m_sDuration;
		m_UserItem[newSlot].sBullNum = 0;
		m_UserItem[newSlot].tMagic[0]=tEBodySid;
		m_UserItem[newSlot].tIQ = 2;
		m_UserItem[newSlot].iItemSerial = 0;
		arMaterial.Add(Slot);
		arMaterial.Add(Slot1);
		
	}else if(g_arItemTable[m_UserItem[Slot].sSid]->m_byWear==126){
	//2��ʯ�ϳ�һ��ѩ�����100W
		if(Slot1==-1) return;
		if(g_arItemTable[m_UserItem[Slot1].sSid]->m_byWear!=126) return ;
		if(m_dwDN < 500000)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
			return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
		}
		if( m_dwDN <= 500000 ) m_dwDN = 0;
		else m_dwDN = m_dwDN - 500000;
		SendMoneyChanged();
		ReSetItemSlot(&m_UserItem[Slot]);
		ReSetItemSlot(&m_UserItem[Slot1]);
		newSlot=Slot;
		
		m_UserItem[newSlot].sLevel = g_arItemTable[908]->m_byRLevel;
		m_UserItem[newSlot].sSid = 908;
		m_UserItem[newSlot].sCount = 1;
		m_UserItem[newSlot].sDuration = g_arItemTable[908]->m_sDuration;
		m_UserItem[newSlot].sBullNum = 0;
		m_UserItem[newSlot].tIQ = 0;
		m_UserItem[newSlot].iItemSerial = 0;
		arMaterial.Add(Slot);
		arMaterial.Add(Slot1);

	}else{
		return ;
	}
	//���ͽ��
	
	CBufferEx TempBuf;
	int i,j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	index = arMaterial.GetSize();
	TempBuf.Add((BYTE)1);
	TempBuf.Add((BYTE)index);
	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		BYTE bySlot = (BYTE)arMaterial[i];
		TempBuf.Add((BYTE)bySlot);
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
//����ˮ�����ѣ��Ľ��ɣ��ĵ�
void USER::DownAccessoriReq(TCHAR *pBuf)
{
	int index=0;
	short Slot = GetShort(pBuf, index);//
	short Slot1 = GetShort(pBuf, index);//����ˮ��

	if(Slot < EQUIP_ITEM_NUM || Slot >= TOTAL_INVEN_MAX) return;
	if(Slot1 < EQUIP_ITEM_NUM || Slot1 >= TOTAL_INVEN_MAX) return;
	m_UserItem[Slot].tMagic[1]=0;
	m_UserItem[Slot].tMagic[2]=0;
	m_UserItem[Slot].tMagic[3]=0;
	m_UserItem[Slot].tMagic[4]=0;
	m_UserItem[Slot].tMagic[5]=5;
	m_UserItem[Slot].tIQ=MAGIC_ITEM;
	ReSetItemSlot(&m_UserItem[Slot1]);

	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)1);
	TempBuf.Add((BYTE)2);
	BYTE bySlot = (BYTE)Slot;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ);
	
	
	bySlot = (BYTE)Slot1;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ);
	
	Send(TempBuf, TempBuf.GetLength());


}
//Ϊ�ټ�װ��������� 100W
void USER::RemagicItem_UpgradeReq(TCHAR *pBuf)
{
	int index=0,iSuccess=2;
	short Slot = GetShort(pBuf, index);//װ��
	short Slot1 = GetShort(pBuf, index);//��ʯ
	
	
	if(Slot1 < EQUIP_ITEM_NUM || Slot1 >= TOTAL_INVEN_MAX) return;
	if(Slot < EQUIP_ITEM_NUM || Slot >= TOTAL_INVEN_MAX) return;
	if(m_UserItem[Slot].tIQ!=12) return;
	if(m_UserItem[Slot1].sSid!=1096) return ;

	if(m_dwDN < 1000000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}
	if( m_dwDN <= 1000000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 1000000;
	UpdateUserItemDN();						
	SendMoneyChanged();
	
	int rand=myrand(0,100)%100;
	int s=(m_UserItem[Slot1].tMagic[0]-3)%10*7+10;
	if(rand>s){
		iSuccess=1;
		if(((m_UserItem[Slot1].tMagic[0]-2)%10) ==1)
		{
			if(m_UserItem[Slot].tMagic[6]==1)
				m_UserItem[Slot].tMagic[6]=m_UserItem[Slot1].tMagic[0];
			else if(m_UserItem[Slot].tMagic[7]==1)
				m_UserItem[Slot].tMagic[7]=m_UserItem[Slot1].tMagic[0];
		}else{
			int a=(m_UserItem[Slot1].tMagic[0]-3)/10;
			int b=(m_UserItem[Slot].tMagic[6]-3) /10;
			int c=(m_UserItem[Slot].tMagic[7]-3) /10;
			
			if(a==b)
				m_UserItem[Slot].tMagic[6]=m_UserItem[Slot1].tMagic[0];
			else if(a==c)
				m_UserItem[Slot].tMagic[7]=m_UserItem[Slot1].tMagic[0];
		}

	}
	ReSetItemSlot(&m_UserItem[Slot1]);
	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)iSuccess);
	TempBuf.Add((BYTE)2);
	BYTE bySlot = (BYTE)Slot;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ); 
	
	bySlot = (BYTE)Slot1;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ);
	
	Send(TempBuf, TempBuf.GetLength());
}
//Ϊ�ټ�װ��������� 100W
void USER::RemagicItem1_UpgradeReq(TCHAR *pBuf)
{
	int index=0,iSuccess=2;
	short Slot = GetShort(pBuf, index);//װ��
	short Slot1 = GetShort(pBuf, index);//��ʯ
	
	
	if(Slot1 < EQUIP_ITEM_NUM || Slot1 >= TOTAL_INVEN_MAX) return;
	if(Slot < EQUIP_ITEM_NUM || Slot >= TOTAL_INVEN_MAX) return;
	if(m_UserItem[Slot].tIQ!=12) return;
	if(m_UserItem[Slot1].sSid!=1096) return ;
	if(m_UserItem[Slot1].tMagic[5]!=1) return ;

	if(m_dwDN < 1000000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}
	if( m_dwDN <= 1000000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 1000000;
	UpdateUserItemDN();						
	SendMoneyChanged();
	
	int rand=myrand(0,100)%100;
	int s=(m_UserItem[Slot1].tMagic[0]-2)%10*7+10;
	if(rand>s){
		iSuccess=1;
		m_UserItem[Slot].tMagic[6]=m_UserItem[Slot1].tMagic[0];
	}
	ReSetItemSlot(&m_UserItem[Slot1]);
	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)iSuccess);
	TempBuf.Add((BYTE)2);
	BYTE bySlot = (BYTE)Slot;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ); 
	
	bySlot = (BYTE)Slot1;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ);
	
	Send(TempBuf, TempBuf.GetLength());
}
//��ʯ����������������
void USER::RemagicItem_HuanShi(TCHAR *pBuf)
{
	int index=0;
	short Slot = GetShort(pBuf, index);//��
	short Slot1 = GetShort(pBuf, index);//��ʯ
	short Slot2 = GetShort(pBuf, index);//��ʯ

	if(Slot1 < EQUIP_ITEM_NUM || Slot1 >= TOTAL_INVEN_MAX) return;
	if(Slot2 < EQUIP_ITEM_NUM || Slot2 >= TOTAL_INVEN_MAX) return;
	if(m_UserItem[Slot1].sSid!=1096 || m_UserItem[Slot2].sSid!=1096)
		return ;
	if(m_UserItem[Slot1].tMagic[0]!=m_UserItem[Slot2].tMagic[0]) return ;
	if(m_UserItem[Slot1].tMagic[5]!=m_UserItem[Slot2].tMagic[5]) return;
	if(m_dwDN < 500000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}
	if( m_dwDN <= 500000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 500000;
	UpdateUserItemDN();						
	SendMoneyChanged();
	ReSetItemSlot(&m_UserItem[Slot2]);
	m_UserItem[Slot1].tMagic[5]++;
	m_UserItem[Slot1].tMagic[0]++;

	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)1);
	TempBuf.Add((BYTE)2);
	BYTE bySlot = (BYTE)Slot1;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ); 
	
	bySlot = (BYTE)Slot2;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ);
	
	Send(TempBuf, TempBuf.GetLength());


}
void USER::ShouHouAdd(TCHAR *pBuf)
{
	int index=0,iSuccess=0,s;
	int Magic[3]={0,0,0};
	int newMagic=-1,up=1;

	short Slot = GetShort(pBuf, index);//��Ƭ
	short Slot1 = GetShort(pBuf, index);//�ػ�
	if(Slot < EQUIP_ITEM_NUM || Slot >= TOTAL_INVEN_MAX) return;

	if(Slot1 < EQUIP_ITEM_NUM || Slot1 >= TOTAL_INVEN_MAX) return;
	if(m_UserItem[Slot].sSid==-1 ||m_UserItem[Slot1].sSid==-1)
		return ;
	if(m_UserItem[Slot].sSid!=1051)
		return ;
	if(g_arItemTable[m_UserItem[Slot1].sSid]->m_byWear!=130)
		return ;
	
	for(int i=1;i<4;i++){
		switch (m_UserItem[Slot1].tMagic[i]){//�����ػ�666����
			case 0:
				newMagic=i;
				break;
			case 1:case 2:case 3:case 4:case 5://ÿ�μӵ�3��
				Magic[0]=i;
				break;
			case 6:
				Magic[0]=-1;
				break;
			case 7:case 8:case 9:case 10:case 11:
				Magic[1]=i;
				break;
			case 12://ÿ����15Ѫ
				Magic[1]=-1;
				break;
			case 13:case 14:case 15:case 16:case 17:
				Magic[2]=i;
				break;
			case 18://ÿ������5��AGIC_DEFENSE_UP
				Magic[2]=-1;
				break;
		 default:
		 	break;
		}
	}
	int rand=myrand(0,100)%100;

	switch(m_UserItem[Slot].tMagic[0]){
		case 5://ÿ�μӵ�3��
			if(Magic[0]==-1) return;
			if(Magic[0]!=0) 
				newMagic=Magic[0];
			else
				up=1;
			
			break;
		case 12://ÿ����15Ѫ
		if(Magic[1]==-1) return;
			if(Magic[1]!=0) 
				newMagic=Magic[1];
			else
				up=7;
			break;
			
		case 6://ÿ������5��
			if(Magic[2]==-1) return;
			if(Magic[2]!=0) 
				newMagic=Magic[2];
			else 
				up=13;
			break;
		default:
			return;
	}
	s=(m_UserItem[Slot1].tMagic[newMagic]%6)*10+10;
	if(rand>s){
		iSuccess=1;
		m_UserItem[Slot1].tMagic[newMagic]+=up;
	}
	//ȥ����Ƭ
	ReSetItemSlot(&m_UserItem[Slot]);

	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	if(iSuccess)
		TempBuf.Add((BYTE)6);
	else
		TempBuf.Add((BYTE)7);
	TempBuf.Add((BYTE)2);
	BYTE bySlot = (BYTE)Slot;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ); 
	
	bySlot = (BYTE)Slot1;
	TempBuf.Add((BYTE)bySlot);
	TempBuf.Add(m_UserItem[bySlot].sLevel);
	TempBuf.Add(m_UserItem[bySlot].sSid);
	TempBuf.Add(m_UserItem[bySlot].sDuration);
	TempBuf.Add(m_UserItem[bySlot].sBullNum);
	TempBuf.Add(m_UserItem[bySlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);
	TempBuf.Add(m_UserItem[bySlot].tIQ);
	
	Send(TempBuf, TempBuf.GetLength());


}
void USER::ShouHouMake(TCHAR *pBuf)
{
	int iWeight=0;
	int index=0;
	BYTE tMagic=0;
	CUIntArray arMaterial;

	short Slot = GetShort(pBuf, index);//����
	short Slot1 = GetShort(pBuf, index);//����
	short Slot2 = GetShort(pBuf, index);//�����ػ�
	short Slot3 = GetShort(pBuf, index);//�����ػ�
	short Slot4 = GetShort(pBuf, index);//�����ػ�
	short Slot5 = GetShort(pBuf, index);//�����ػ�
	if(Slot < EQUIP_ITEM_NUM || Slot >= TOTAL_INVEN_MAX) return;
	if(Slot1 < EQUIP_ITEM_NUM || Slot1 >= TOTAL_INVEN_MAX) return;
	if(m_UserItem[Slot].sSid!=1050)
		return;
	arMaterial.Add(Slot);
	arMaterial.Add(Slot1);
	if(m_UserItem[Slot1].sSid==665||m_UserItem[Slot1].sSid==668||m_UserItem[Slot1].sSid==663){
		if(m_UserItem[Slot1].sCount<100)
			return;
		if(m_UserItem[Slot1].sSid==665)//����3
			tMagic=5;
		if(m_UserItem[Slot1].sSid==668)//Ѫ��15
			tMagic=6;
		if(m_UserItem[Slot1].sSid==663)//����5
			tMagic=12;
	}else{
		return;
	}
	if(Slot2!=-1){
		if(Slot2 < EQUIP_ITEM_NUM || Slot2 >= TOTAL_INVEN_MAX) return;
		iWeight=iWeight+g_arItemTable[m_UserItem[Slot2].sSid]->m_byWeight;
		ReSetItemSlot(&m_UserItem[Slot2]);
		arMaterial.Add(Slot2);
		
	}
	if(Slot3!=-1){
		if(Slot3 < EQUIP_ITEM_NUM || Slot3 >= TOTAL_INVEN_MAX) return;
		iWeight=iWeight+g_arItemTable[m_UserItem[Slot3].sSid]->m_byWeight;
		ReSetItemSlot(&m_UserItem[Slot3]);
		arMaterial.Add(Slot3);
		
	}
	if(Slot4!=-1){
		if(Slot4 < EQUIP_ITEM_NUM || Slot4 >= TOTAL_INVEN_MAX) return;
		iWeight=iWeight+g_arItemTable[m_UserItem[Slot4].sSid]->m_byWeight;
		ReSetItemSlot(&m_UserItem[Slot4]);
		arMaterial.Add(Slot4);
		
	}
	if(Slot5!=-1){
		if(Slot5 < EQUIP_ITEM_NUM || Slot5 >= TOTAL_INVEN_MAX) return;
		iWeight=iWeight+g_arItemTable[m_UserItem[Slot5].sSid]->m_byWeight;
		ReSetItemSlot(&m_UserItem[Slot5]);
		arMaterial.Add(Slot5);
		
	}
	iWeight=iWeight+g_arItemTable[m_UserItem[Slot1].sSid]->m_byWeight;
	if(m_UserItem[Slot1].sCount>100){
		m_UserItem[Slot1].sCount=m_UserItem[Slot1].sCount-100;
	}else
		ReSetItemSlot(&m_UserItem[Slot1]);
	ReSetItemSlot(&m_UserItem[Slot]);
	int sSid=1051;
	m_UserItem[Slot].sLevel = g_arItemTable[sSid]->m_byRLevel;
	m_UserItem[Slot].sSid = sSid;
	m_UserItem[Slot].sCount = 1;
	m_UserItem[Slot].sDuration = g_arItemTable[sSid]->m_sDuration;
	m_UserItem[Slot].sBullNum = 1;
	m_UserItem[Slot].tMagic[0] = tMagic;
	m_UserItem[Slot].tMagic[1] = 0;
	m_UserItem[Slot].tMagic[2] = 0;
	m_UserItem[Slot].tMagic[3] = 0;
	m_UserItem[Slot].tMagic[4] = 0;
	m_UserItem[Slot].tMagic[5] = 0;
	m_UserItem[Slot].tIQ = 2;
	m_UserItem[Slot].iItemSerial = 0;
	
	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	CBufferEx TempBuf;
	int i,j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	index = arMaterial.GetSize();
	TempBuf.Add((BYTE)5);
	TempBuf.Add((BYTE)index);
	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		BYTE bySlot = (BYTE)arMaterial[i];
		TempBuf.Add((BYTE)bySlot);
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
void USER::ShouHouUpgradeReq(TCHAR *pBuf)
{
	int index=0;
	int n;
	int iSuccess = 2;
	int iWeight=0;

	short sSrcSlot = GetShort(pBuf, index);//����
	short sDstSlot = GetShort(pBuf, index);//����
	if(sSrcSlot < EQUIP_ITEM_NUM || sSrcSlot >= TOTAL_INVEN_MAX) return;
	if(sDstSlot < EQUIP_ITEM_NUM || sDstSlot >= TOTAL_INVEN_MAX) return;
	if(m_UserItem[sDstSlot].sSid!=964)//��������Ʒ���Ǿ���.
		return;
	if(m_UserItem[sSrcSlot].tIQ!=9)
		return;
	n=m_UserItem[sSrcSlot].tMagic[5];
	if(n>=2){
		m_UserItem[sSrcSlot].tMagic[5]=2;
		return;
	}
	int rand=myrand(0,100)%100;
	if(n==0&&rand<40)
		iSuccess=1;
	if(n==1&&rand<15)
		iSuccess=1;
	if(iSuccess==1){//�ɹ���.....
		m_UserItem[sSrcSlot].tMagic[0]++;
		m_UserItem[sSrcSlot].tMagic[5]++;
	}else{//ʧ����.....
		 iWeight+= g_arItemTable[m_UserItem[sSrcSlot].sSid]->m_byWeight;
		ReSetItemSlot(&m_UserItem[sSrcSlot]);
		
	}
	iWeight+= g_arItemTable[m_UserItem[sDstSlot].sSid]->m_byWeight;
	ReSetItemSlot(&m_UserItem[sDstSlot]);
	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;
	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)iSuccess);
	TempBuf.Add((BYTE)2);
	TempBuf.Add((BYTE)sSrcSlot);
	TempBuf.Add(m_UserItem[sSrcSlot].sLevel);
	TempBuf.Add(m_UserItem[sSrcSlot].sSid);
	TempBuf.Add(m_UserItem[sSrcSlot].sDuration);
	TempBuf.Add(m_UserItem[sSrcSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSrcSlot].sCount);

	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSrcSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSrcSlot].tIQ); 

	TempBuf.Add((BYTE)sDstSlot);
	TempBuf.Add(m_UserItem[sDstSlot].sLevel);
	TempBuf.Add(m_UserItem[sDstSlot].sSid);
	TempBuf.Add(m_UserItem[sDstSlot].sDuration);
	TempBuf.Add(m_UserItem[sDstSlot].sBullNum);
	TempBuf.Add(m_UserItem[sDstSlot].sCount);

	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sDstSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sDstSlot].tIQ); 

	Send(TempBuf, TempBuf.GetLength());
}
int USER::FindItemSid(short sSid)
{
	int i;
	for(i=0;i<60;i++){
		if(convert[i].Ssid==sSid)
			return i;
	}
	return -1;

}
////�ټ�ת��
void USER::ItemConvert(TCHAR *pBuf)
{
	int index=0,n;
	int i,count1=0,count2=0;
	short sid;
	short sSrcSlot = GetShort(pBuf, index);//����
	short sDstSlot = GetShort(pBuf, index);//����
	if(sSrcSlot < EQUIP_ITEM_NUM || sSrcSlot >= TOTAL_INVEN_MAX) return;
	if(sDstSlot < EQUIP_ITEM_NUM || sDstSlot >= TOTAL_INVEN_MAX) return;
	for(i=EQUIP_ITEM_NUM;i<TOTAL_INVEN_MAX;i++){
		if(1093==m_UserItem[i].sSid)
			break;
	}
	if(i>=TOTAL_INVEN_MAX)
		return;
	if(m_UserItem[sSrcSlot].sSid==-1&&m_UserItem[sDstSlot].sSid==-1)
		return;
	if( g_arItemTable[m_UserItem[sSrcSlot].sSid]->m_byWear !=  g_arItemTable[m_UserItem[sDstSlot].sSid]->m_byWear)
		return ;
	if(m_UserItem[sSrcSlot].tMagic[5]!=m_UserItem[sDstSlot].tMagic[5])
		return;
	if(m_UserItem[sSrcSlot].tMagic[5]<7)
		return;
	for(i=0;i<4;i++){
		if(m_UserItem[sSrcSlot].tMagic[i]!=0) count1++;
		if(m_UserItem[sDstSlot].tMagic[i]!=0) count2++;
	}
	if(count1!=count2||count1==0)
		return;

	n=FindItemSid(m_UserItem[sDstSlot].sSid);
	if(n==-1)
		return;
	n=FindItemSid(m_UserItem[sSrcSlot].sSid);
	if(n==-1)
		return;
	sid=convert[n].Dsid;
	//��Ǯ1000W
	if(m_dwDN < 10000000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}
	if( m_dwDN <= 10000000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 10000000;
	UpdateUserItemDN();						
	SendMoneyChanged();
	//���������
	RobItem(1093, 1);
//��ʼת��

	m_UserItem[sSrcSlot].sSid=sid;
	m_UserItem[sSrcSlot].tIQ=0x0c;
	m_UserItem[sSrcSlot].tMagic[5]=m_UserItem[sSrcSlot].tMagic[5]-7;
	int rand = myrand(1, 100)%count1;
	count2=0;
	for(i=0;i<4;i++){
		if(rand==count2){
			m_UserItem[sSrcSlot].tMagic[4]=m_UserItem[sDstSlot].tMagic[i];
		}
		if(m_UserItem[sDstSlot].tMagic[i]!=0)
			count2++;
	}
	ReSetItemSlot(&m_UserItem[sDstSlot]);
	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)3);
	TempBuf.Add((BYTE)2);
	TempBuf.Add((BYTE)sSrcSlot);
	TempBuf.Add(m_UserItem[sSrcSlot].sLevel);
	TempBuf.Add(m_UserItem[sSrcSlot].sSid);
	TempBuf.Add(m_UserItem[sSrcSlot].sDuration);
	TempBuf.Add(m_UserItem[sSrcSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSrcSlot].sCount);

	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSrcSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSrcSlot].tIQ); 
	
	TempBuf.Add((BYTE)sDstSlot);
	TempBuf.Add(m_UserItem[sDstSlot].sLevel);
	TempBuf.Add(m_UserItem[sDstSlot].sSid);
	TempBuf.Add(m_UserItem[sDstSlot].sDuration);
	TempBuf.Add(m_UserItem[sDstSlot].sBullNum);
	TempBuf.Add(m_UserItem[sDstSlot].sCount);

	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sDstSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sDstSlot].tIQ); 

	Send(TempBuf, TempBuf.GetLength());

}
void USER::RemagicItem_100_3(TCHAR *pBuf)
{
	int index=0,i;
	short sSourceSlot = GetShort(pBuf, index);	
	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;
	if( m_UserItem[sSourceSlot].tMagic[5]<3){
		return;
	}
	if( m_UserItem[sSourceSlot].tIQ!=12){
		return;
	}
	for(i=EQUIP_ITEM_NUM;i<TOTAL_INVEN_MAX;i++){
		if(1094==m_UserItem[i].sSid)
			break;
	}
	if(i>=TOTAL_INVEN_MAX)
		return;
	if(m_dwDN < 1000000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}
	if(m_UserItem[sSourceSlot].tMagic[6]==1)
		return ;
	if( m_dwDN <= 1000000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 1000000;

	m_UserItem[sSourceSlot].tMagic[6]=1;

	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();
	//���������֮��
	RobItem(1094, 1);
	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)1);
	TempBuf.Add((BYTE)1);
	TempBuf.Add((BYTE)sSourceSlot);
	TempBuf.Add(m_UserItem[sSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[sSourceSlot].sSid);
	TempBuf.Add(m_UserItem[sSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[sSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSourceSlot].sCount);

	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSourceSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSourceSlot].tIQ); 
	Send(TempBuf, TempBuf.GetLength());

}
//���߲��ϻ�
void USER::RemagicItem_100_1(TCHAR *pBuf)
{
	int index=0;
	short sSourceSlot = GetShort(pBuf, index);	
	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;
	if( m_UserItem[sSourceSlot].tMagic[5]<7){
		return;
	}
	if(m_UserItem[sSourceSlot].tIQ!=1&&m_UserItem[sSourceSlot].tIQ!=3&&m_UserItem[sSourceSlot].tIQ!=2)
		return ;
	if(m_dwDN < 100000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}

	if( m_dwDN <= 100000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 100000;

	m_UserItem[sSourceSlot].tMagic[6]=2;

	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();

	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)9);
	TempBuf.Add((BYTE)1);
	TempBuf.Add((BYTE)sSourceSlot);
	TempBuf.Add(m_UserItem[sSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[sSourceSlot].sSid);
	TempBuf.Add(m_UserItem[sSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[sSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSourceSlot].sCount);

	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSourceSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSourceSlot].tIQ); 
	Send(TempBuf, TempBuf.GetLength());

}
//������߲��ϻ�
void USER::RemagicItem_100_2(TCHAR *pBuf)
{
	int index=0;
	short sSourceSlot = GetShort(pBuf, index);	
	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;
	if( m_UserItem[sSourceSlot].tMagic[5]<7){
		return;
	}
	if(m_UserItem[sSourceSlot].tIQ!=1&&m_UserItem[sSourceSlot].tIQ!=3&&m_UserItem[sSourceSlot].tIQ!=2)
		return ;
	if(m_dwDN < 100000)
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return ;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
	}

	if( m_dwDN <= 100000 ) m_dwDN = 0;
	else m_dwDN = m_dwDN - 100000;

	m_UserItem[sSourceSlot].tMagic[6]=0;

	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();

	CBufferEx TempBuf;
	int j;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	TempBuf.Add((BYTE)10);
	TempBuf.Add((BYTE)1);
	TempBuf.Add((BYTE)sSourceSlot);
	TempBuf.Add(m_UserItem[sSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[sSourceSlot].sSid);
	TempBuf.Add(m_UserItem[sSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[sSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSourceSlot].sCount);

	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSourceSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSourceSlot].tIQ); 
	Send(TempBuf, TempBuf.GetLength());
}
void USER::UpgradeItemReq(TCHAR *pBuf)
{

	int iSid = -1;
	int iIQ = -1;
	int iCount = 0, iThings = 0;
	int i, j, index = 0;
	int iWeight = 0;
	short sMaterialSlot;

	int iSuccess = 0;

	BYTE tIQ = 0, bySlot = 0;

	CBufferEx TempBuf;

	ItemList	MyItem[TOTAL_ITEM_NUM];

	CByteArray arMaterial;
	arMaterial.RemoveAll();

	short sSourceSlot = GetShort(pBuf, index);			// ������ ������ ���Թ�ȣ

	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;	// �κ������� ���׷��̵� ����

	iSid = m_UserItem[sSourceSlot].sSid;
	if(iSid < 0 || iSid >= g_arItemTable.GetSize()) return;
	if( iSid == 669 || iSid == 670 )					// ���׷��̵� �Ϸ��� �������� �۷����������
	{
		SendSystemMsg( IDS_USER_CANT_UPGRADE_ITEM, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(g_arItemTable[iSid]->m_byWear > 5 && g_arItemTable[iSid]->m_byWear < 9)
		return ;



	iIQ = m_UserItem[sSourceSlot].tIQ;
	switch(iIQ)
	{
	case NORMAL_ITEM:
		break;
	case MAGIC_ITEM:
		iCount = 2;
		iThings = MATERIAL_MAGIC_UP_ITEM;
		break;
	case RARE_ITEM:
		iCount = 4;
		iThings = MATERIAL_RARE_UP_ITEM;
		break;
	case REMODEL_ITEM:
	case REMODEL_MAGIC_ITEM:
		iCount = 2;
		iThings = MATERIAL_REMODEL_UP_ITEM;
		break;
	case 12: //��ͨ�ټ�
		iCount = 4;
		iThings = MATERIAL_RARE_UP_ITEM;
		break;
	default:

		return;
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++)			// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}
	for(i = 0; i < iCount; i++)
	{
		sMaterialSlot = -1;
		sMaterialSlot = GetShort(pBuf, index);			// �����Ӽ� 1�� ���� ��� ( ������ -1 )
		if(sMaterialSlot >= EQUIP_ITEM_NUM && sMaterialSlot < TOTAL_INVEN_MAX)
		{												// ����߿� �ش� wear������ �ٸ��� ����		
			iSid = MyItem[sMaterialSlot].sSid;
			if(iSid < 0 || iSid >= g_arItemTable.GetSize()) return;
			if(iSid != iThings) return;			
			if(MyItem[sMaterialSlot].sCount <= 0) return;

			iWeight += g_arItemTable[iSid]->m_byWeight;

			MyItem[sMaterialSlot].sCount -= 1;
			arMaterial.Add((BYTE)sMaterialSlot);
		}
		else
		{
			SendSystemMsg(IDS_USER_SHENGSHI, SYSTEM_ERROR, TO_ME);
			return;
		}
	}


	iSuccess = SetUpgeadeItem(sSourceSlot);	
	if(iSuccess == 0) iWeight += g_arItemTable[iSid]->m_byWeight;			// ���Ժ�ȭ�� üũ�Ѵ�.
	if(iSuccess == -1) return;

	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.

	TempBuf.Add(UPGRADE_ITEM_RESULT);
	index = 1 + arMaterial.GetSize();

	if(!iSuccess)	TempBuf.Add((BYTE)0x00); //����
	else			TempBuf.Add((BYTE)0x01);
	
	SendMoneyChanged();

	TempBuf.Add((BYTE)index);

	TempBuf.Add((BYTE)sSourceSlot);					// �� �����۸� ���� 	
	TempBuf.Add(m_UserItem[sSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[sSourceSlot].sSid);
	TempBuf.Add(m_UserItem[sSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[sSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSourceSlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSourceSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSourceSlot].tIQ); 

	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		bySlot = (BYTE)arMaterial[i];
		
		if(m_UserItem[bySlot].sCount <= 1) ReSetItemSlot(&m_UserItem[bySlot]);// �Һ� �̹Ƿ� ���� �ʱ�ȭ�ϰ� 
		else							   m_UserItem[bySlot].sCount -= 1;

		TempBuf.Add(bySlot);
		TempBuf.Add(m_UserItem[bySlot].sLevel);
		TempBuf.Add(m_UserItem[bySlot].sSid);
		TempBuf.Add(m_UserItem[bySlot].sDuration);
		TempBuf.Add(m_UserItem[bySlot].sBullNum);
		TempBuf.Add(m_UserItem[bySlot].sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);

		TempBuf.Add(m_UserItem[bySlot].tIQ); 
	}

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

	Send(TempBuf, TempBuf.GetLength());

	arMaterial.RemoveAll();
}

int USER::SetUpgeadeItem(short sSlot)
{
	BYTE tCount = 0;

	int iSuccess = 0;

	int temp = 0;
	int last = 0;
	int rand = 0;

	if(m_UserItem[sSlot].sSid < 0 || m_UserItem[sSlot].sSid >= g_arItemTable.GetSize()) return -1;
	if(g_arItemTable[m_UserItem[sSlot].sSid]->m_sDuration <= 0) return -1;
	
	tCount = m_UserItem[sSlot].tMagic[5];
	if(tCount >= MAX_ITEM_UPGRADE_COUNT) return -1;

	if(g_arItemTable[m_UserItem[sSlot].sSid]->m_byWear == ATTACK_ITEM)
	{
		if(m_dwDN < ATTACK_UPGRADE_COST)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
			return -1;		// ���ݿ� ���׷��̵� ��뺸�� ������ 
		}

		if( m_dwDN <= ATTACK_UPGRADE_COST ) m_dwDN = 0;
		else m_dwDN = m_dwDN - ATTACK_UPGRADE_COST;			// �ش� ��븦 ����� ������� �̸� ���

		if(g_arItemTable[m_UserItem[sSlot].sSid]->m_byClass == STAFF_ITEM)
		{
			last = ATTACK_UPGRADE_PSI_BAND;					// ���׷��̵� ������ �߰��Ǵ� ��
		}
		else
		{
			last = ATTACK_UPGRADE_BAND;						// ���׷��̵� ������ �߰��Ǵ� ��
		}


		rand = myrand(1, 10000);							// �������� ������ �������� ����
		if(rand <= g_ItemAttUpgrade[tCount]) iSuccess = 1;// �ش� ��°�� ���������� ������

	}
	else
	{
		if(m_dwDN < DEFENSE_UPGRADE_COST)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
			return -1;		// ���� ���׷��̵� ��뺸�� ������ 
		}

		if( m_dwDN <= DEFENSE_UPGRADE_COST ) m_dwDN = 0;
		else m_dwDN = m_dwDN - DEFENSE_UPGRADE_COST;				// �ش� ��븦 ����� ������� �̸� ���

		last = DEFENSE_UPGRADE_BAND;						// ���� ����

//		if(tCount < 6)					// �ӽ÷� 6������ ũ�� ������ ���� 02-05-04 by Youn Gyu
//		{
		rand = myrand(1, 10000);	
		if(m_UserItem[sSlot].tIQ==12){
			if(rand <= g_ItemDefUpgrade_[tCount]) 
				iSuccess = 1;
		}else{
			if(rand <= g_ItemDefUpgrade[tCount]) 
				iSuccess = 1;
		}
	}

	if(iSuccess == 1)						// �ش� ��°�� ���������� ������
	{	
		m_UserItem[sSlot].tMagic[5] = tCount + 1;	// ������ ���Կ� ����Ƚ���� ���� ��Ų��.
		m_UserItem[sSlot].tMagic[4] = (BYTE)((tCount + 1) * last);
		
		MakeItemLog( &m_UserItem[sSlot], ITEMLOG_UPGRADE_SUCCESS );
	}
	else
	{ 
		MakeItemLog( &m_UserItem[sSlot], ITEMLOG_UPGRADE_FAIL );
		ReSetItemSlot(&m_UserItem[sSlot]);	iSuccess = 0; 
	}	

	FlushItemLog( TRUE );
	return iSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////
//	�׼������� ���׷��̵� �Ѵ�.
///////////////////////////////////////////////////////////////////////////////////////
void USER::UpgradeAccessoriReq(TCHAR *pBuf)
{
	if(m_dwDN < ACCESSORI_UPGRADE_COST) 
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return;		// ���׷��̵� �� ���� ������ ����
	}

//	int iSid = 0;
//	int iIQ = -1;
	int iCount = 0;
	int i, j, index = 0;
	int iWeight = 0;
	short sMaterialSlot[4] = {-1, -1, -1, -1};
	short sSid[4] = {-1, -1, -1, -1};
	short sAid = 0;
	short sMagicSid = 0;
	int iSuccess = 0;
	BYTE bySlot = 0;

	int	iRandom, iMagicRandom, iCrestRandom, iCrestCount = 0;
	int iUpgradeCount = 0, iAllSkill = 0;
	BOOL bUseCrest = 0;

	CByteArray arMaterial;
	arMaterial.RemoveAll();

	short sAccessori = -1, sRipel = -1;				// 5�� �̻� ���� ���ؼ� �غ��Ѵ�.

	short sSlot = GetShort(pBuf, index);			// ������ ������ ���Թ�ȣ
//	if(sSlot != -1) return;							// �׼������� �ҽ��� ����.
	if(sSlot != -1)									// 5���̻�����
	{
		short sTemp = -1;
		if(sSlot < EQUIP_ITEM_NUM || sSlot >= TOTAL_INVEN_MAX) return;
		sTemp = m_UserItem[sSlot].sSid;
		if(sTemp < 0 || sTemp >= g_arItemTable.GetSize()) return;

		if(g_arItemTable[sTemp]->m_byWear <= 5 || g_arItemTable[sTemp]->m_byWear >= 9) return;

		if(m_UserItem[sSlot].tMagic[5] <= 4) 
		{
			SendSystemMsg(IDS_USER_ACCESSORI_INVALID, SYSTEM_ERROR, TO_ME);	// 5�� ������ �������� ����������, �������� ���׷��̵� ���� �ʴ´�.
			return;		
		}

		if(m_UserItem[sSlot].tMagic[5] > 7) return; // �� ���ִ� ���׷��̵�� �̹� ���ߴ�.

		if(m_UserItem[sSlot].sDuration == 0)
		{
			SendSystemMsg( IDS_USER_ACCESSORI_OLD, SYSTEM_ERROR, TO_ME);
			return;
		}

		sSid[0] = sTemp;
		sAccessori = sSlot;

		sSlot = -1, sTemp = -1;
		sSlot = GetShort(pBuf, index);
		if(sSlot == -1) return;
		if(sSlot < EQUIP_ITEM_NUM || sSlot >= TOTAL_INVEN_MAX)	return;

		sTemp = m_UserItem[sSlot].sSid;
			if(sTemp != 798 && sTemp != 799 && sTemp != 800 && sTemp != 801 && sTemp != 802 )
					return;
		if(sTemp < 0 || sTemp >= g_arItemTable.GetSize()) 
		{
			SendSystemMsg(IDS_USER_ACCESSORI_RIPEL, SYSTEM_ERROR, TO_ME);	// ������ �����̳� ������ ���� ������.
			return;
		}

		sSid[1] = sTemp;
		sRipel = sSlot;

		iCount = 100;							// 5���� �̻� ���� ���ؼ� 100���� �ο��Ѵ�.
	}
	else
	{	
		int magic_slot = -1;
		int dynamic_slot = -1;

		for(i = 0; i < 4; i++)							
		{
			sMaterialSlot[i] = GetShort(pBuf, index);
			if(sMaterialSlot[i] <= -1) break;
			if(sMaterialSlot[i] < EQUIP_ITEM_NUM || sMaterialSlot[i] >= TOTAL_INVEN_MAX) return;
			sSid[i] = m_UserItem[sMaterialSlot[i]].sSid;
			if(sSid[i] == -1)//����
				return ;
			if(g_arItemTable[sSid[i]]->m_byWear <= 5 || g_arItemTable[sSid[i]]->m_byWear >= 9) 
				if(sSid[i] != 798 || sSid[i] != 799 || sSid[i] != 800 || sSid[i] != 801 )
					return;

			if(sSid[i] < 0 || sSid[i] >= g_arItemTable.GetSize()) return;
			if(sSid[i] == SID_RING_OF_LIFE || sSid[i] == SID_NECKLACE_OF_SHIELD || sSid[i] == SID_EARRING_OF_PROTECT)
			{
				SendSystemMsg(IDS_USER_ACCESSORI_SKILLUP, SYSTEM_ERROR, TO_ME);	// ���׷��̵� �� �� ���� �Ǽ��縮
				return;
			}

			magic_slot = -1;
			dynamic_slot = -1;
			magic_slot = m_UserItem[sMaterialSlot[i]].tMagic[0];
			if(magic_slot < 0 || magic_slot >= g_arMagicItemTable.GetSize()) return;

			dynamic_slot = g_arMagicItemTable[magic_slot]->m_sSubType;	
			if(dynamic_slot < 0 || dynamic_slot >= MAGIC_COUNT) return;
			
			if(dynamic_slot == MAGIC_ALL_SKILL_UP)	// ���ǿ��Ǹ� ���׷��̵� ����
			{
				SendSystemMsg( IDS_USER_ACCESSORI_SKILLUP, SYSTEM_ERROR, TO_ME);				
				return;
			}

			iCount++;
		}

		for(i = 0; i < iCount; i++)
		{
			if(g_arItemTable[sSid[i]]->m_byWear >= 6 && g_arItemTable[sSid[i]]->m_byWear <= 8)
			{
				if(m_UserItem[sMaterialSlot[i]].sDuration == 0)
				{
					SendSystemMsg( IDS_USER_ACCESSORI_OLD, SYSTEM_ERROR, TO_ME);
					return;
				}
			}
		}
	}

	switch(iCount)
	{
	case 100:
/*		if(m_UserItem[sMaterialSlot[0]].tMagic[5] <= 4) 
		{
			SendSystemMsg(IDS_USER_ACCESSORI_INVALID, SYSTEM_ERROR, TO_ME);	// 5�� ������ �������� ����������, �������� ���׷��̵� ���� �ʴ´�.
			return;		
		}
		if(m_UserItem[sMaterialSlot[0]].tMagic[5] > 7) return; // �� ���ִ� ���׷��̵�� �̹� ���ߴ�.
*/
/*		if(g_arItemTable[sSid[0]]->m_byWear >= 6 && g_arItemTable[sSid[0]]->m_byWear <= 8)
		{
			sAccessori = sMaterialSlot[0];
			sRipel = sMaterialSlot[1];
		}
		else
		{
			sAccessori = sMaterialSlot[1];
			sRipel = sMaterialSlot[0];
			sSid[3] = sSid[1];
			sSid[1] = sSid[0];
			sSid[0] = sSid[3];
		}
*/
		if(sAccessori == -1 || sRipel == -1) return;

		iRandom = myrand(1, 10000);
		iUpgradeCount = m_UserItem[sAccessori].tMagic[5] - 5;
		switch(m_UserItem[sRipel].sSid)
		{
		case RIPEL_TOP:
			if(iRandom <= g_ItemAccessoriUpgrade[iUpgradeCount])	// Upgrade Success;
			{
				iMagicRandom = myrand(0, g_Ripel.m_arRipelTop.GetSize() - 1);				
				sMagicSid = g_Ripel.m_arRipelTop[iMagicRandom];
				iSuccess = 1;
			}
			else ReSetItemSlot(&m_UserItem[sAccessori]);		// Ugrade Fail

			break;
		case RIPEL_BOTTOM:
			if(iRandom <= g_ItemAccessoriUpgrade[iUpgradeCount])	// Upgrade Success;
			{
				iMagicRandom = myrand(0, g_Ripel.m_arRipelBottom.GetSize() - 1);				
				sMagicSid = g_Ripel.m_arRipelBottom[iMagicRandom];
				iSuccess = 1;
			}
			else ReSetItemSlot(&m_UserItem[sAccessori]);		// Ugrade Fail

			break;
		case RIPEL_LEFT:
			if(iRandom <= g_ItemAccessoriUpgrade[iUpgradeCount])	// Upgrade Success;
			{
				iMagicRandom = myrand(0, g_Ripel.m_arRipelLeft.GetSize() - 1);				
				sMagicSid = g_Ripel.m_arRipelLeft[iMagicRandom];
				iSuccess = 1;
			}
			else ReSetItemSlot(&m_UserItem[sAccessori]);		// Ugrade Fail

			break;
		case RIPEL_RIGHT:
			if(iRandom <= g_ItemAccessoriUpgrade[iUpgradeCount])	// Upgrade Success;
			{
				iMagicRandom = myrand(0, g_Ripel.m_arRipelRight.GetSize() - 1);				
				sMagicSid = g_Ripel.m_arRipelRight[iMagicRandom];
				iSuccess = 1;
			}
			else ReSetItemSlot(&m_UserItem[sAccessori]);		// Ugrade Fail

			break;
		case RIPEL_CREST:
			if(m_UserItem[sAccessori].tMagic[5] != 5)
			{
				SendSystemMsg(IDS_USER_ACCESSORI_CREST, SYSTEM_ERROR, TO_ME);	// �����ǹ������δ� 5���� �׼������� ���׷��̵� �� ���ֽ��ϴ�.
				return;
			}

			bUseCrest = TRUE;
			if(iRandom <= g_ItemAccessoriUpgrade[2])	// Upgrade Success;
			{
				while(1)
				{
					iCrestRandom = myrand(1, 5);
					if(iCrestRandom == 1) 
					{
						iMagicRandom = myrand(0, g_Ripel.m_arRipelTop.GetSize() - 1);				
						sMagicSid = g_Ripel.m_arRipelTop[iMagicRandom];
						m_UserItem[sAccessori].tMagic[iCrestCount + 1] = (BYTE)sMagicSid;
						iCrestCount++;
					}
					if(iCrestRandom == 2) 
					{
						iMagicRandom = myrand(0, g_Ripel.m_arRipelBottom.GetSize() - 1);				
						sMagicSid = g_Ripel.m_arRipelBottom[iMagicRandom];
						m_UserItem[sAccessori].tMagic[iCrestCount + 1] = (BYTE)sMagicSid;
						iCrestCount++;
					}
					if(iCrestRandom == 3) 
					{
						iMagicRandom = myrand(0, g_Ripel.m_arRipelLeft.GetSize() - 1);				
						sMagicSid = g_Ripel.m_arRipelLeft[iMagicRandom];
						m_UserItem[sAccessori].tMagic[iCrestCount + 1] = (BYTE)sMagicSid;
						iCrestCount++;
					}
					if(iCrestRandom == 4) 
					{
						iMagicRandom = myrand(0, g_Ripel.m_arRipelRight.GetSize() - 1);				
						sMagicSid = g_Ripel.m_arRipelRight[iMagicRandom];
						m_UserItem[sAccessori].tMagic[iCrestCount + 1] = (BYTE)sMagicSid;
						iCrestCount++;
					}
					if(iCrestRandom == 5 && iAllSkill == 0) 
					{
						iAllSkill = 1;
						iMagicRandom = myrand(0, g_Ripel.m_arRipelCrest.GetSize() - 1);
						sMagicSid = g_Ripel.m_arRipelCrest[iMagicRandom];
						m_UserItem[sAccessori].tMagic[iCrestCount + 1] = (BYTE)sMagicSid;
						iCrestCount++;
					}

					if(iCrestCount >= 3) break;
				}

				m_UserItem[sAccessori].tMagic[5] += 3;
				m_UserItem[sAccessori].tIQ = RARE_ITEM;
				iSuccess = 1;
			}
			else ReSetItemSlot(&m_UserItem[sAccessori]);		// Ugrade Fail

			break;
		default:
			SendSystemMsg(IDS_USER_ACCESSORI_RIPEL, SYSTEM_ERROR, TO_ME);	// ������ �����̳� ������ ���� ������.
			return;		
		}

		if(bUseCrest == FALSE && iSuccess == 1)
		{
			m_UserItem[sAccessori].tMagic[5]++;
			if(iUpgradeCount == 0) m_UserItem[sAccessori].tMagic[1] = (BYTE)sMagicSid;
			if(iUpgradeCount == 1) m_UserItem[sAccessori].tMagic[2] = (BYTE)sMagicSid;
			if(iUpgradeCount == 2) m_UserItem[sAccessori].tMagic[3] = (BYTE)sMagicSid;
			m_UserItem[sAccessori].tIQ = RARE_ITEM;
		}

		if(iSuccess == 0)
		{
			iWeight = g_arItemTable[sSid[0]]->m_byWeight + g_arItemTable[sSid[1]]->m_byWeight;
		}
		else if(iSuccess == 1)
		{
			iWeight = g_arItemTable[sSid[1]]->m_byWeight;
		}

		if(m_UserItem[sRipel].sCount <= 1) ReSetItemSlot(&m_UserItem[sRipel]);
		else							   m_UserItem[sRipel].sCount -= 1;
		
		if(m_dwDN > ACCESSORI_UPGRADE_COST) m_dwDN -= ACCESSORI_UPGRADE_COST;
		else m_dwDN = 0;

		arMaterial.Add((BYTE)sAccessori);
		arMaterial.Add((BYTE)sRipel);

		break;
	case 3:
		if(g_arItemTable[sSid[0]]->m_byWear < 6 || g_arItemTable[sSid[0]]->m_byWear > 8) return;	// ���׷��̵� �Ϸ��� �������� �׼������� �ƴϸ� ����
		if(g_arItemTable[sSid[1]]->m_byWear < 6 || g_arItemTable[sSid[1]]->m_byWear > 8) return;	// ���׷��̵� �Ϸ��� �������� �׼������� �ƴϸ� ����
		if(g_arItemTable[sSid[2]]->m_byWear < 6 || g_arItemTable[sSid[2]]->m_byWear > 8) return;	// ���׷��̵� �Ϸ��� �������� �׼������� �ƴϸ� ����

		if(m_UserItem[sMaterialSlot[0]].tMagic[5] > 4) return;	// 5 �̻��� �ɷ�ġ�� ������ �������� �߰� �������� �ʿ�

		if(m_UserItem[sMaterialSlot[0]].tMagic[5] != m_UserItem[sMaterialSlot[1]].tMagic[5] || 
		   m_UserItem[sMaterialSlot[0]].tMagic[5] != m_UserItem[sMaterialSlot[2]].tMagic[5] ||
		   m_UserItem[sMaterialSlot[1]].tMagic[5] != m_UserItem[sMaterialSlot[2]].tMagic[5] ) return;	// ���� ���׷��̵� ���� �ƴϸ� ����

		if(m_UserItem[sMaterialSlot[0]].tMagic[0] != m_UserItem[sMaterialSlot[1]].tMagic[0] || 
		   m_UserItem[sMaterialSlot[0]].tMagic[0] != m_UserItem[sMaterialSlot[2]].tMagic[0] ||
		   m_UserItem[sMaterialSlot[1]].tMagic[0] != m_UserItem[sMaterialSlot[2]].tMagic[0] ) return;	// ���� �ɷ�ġ�� �������� �ƴϸ� ����


		iWeight += (g_arItemTable[sSid[0]]->m_byWeight + g_arItemTable[sSid[1]]->m_byWeight);

		sMagicSid = m_UserItem[sMaterialSlot[0]].tMagic[0];

		sAid = g_arMagicItemTable[sMagicSid]->m_sAid;

		if(sAid + 1 >= g_arAccessoriUpTable.GetSize()) return;

		m_UserItem[sMaterialSlot[0]].tMagic[0] = g_arAccessoriUpTable[sAid + 1]->m_sSid;

		ReSetItemSlot(&m_UserItem[sMaterialSlot[1]]);
		ReSetItemSlot(&m_UserItem[sMaterialSlot[2]]);

		if(m_UserItem[sMaterialSlot[0]].tMagic[5] == 0) m_UserItem[sMaterialSlot[0]].tMagic[5] = 2;
		else m_UserItem[sMaterialSlot[0]].tMagic[5]++;

		if(m_dwDN > ACCESSORI_UPGRADE_COST) m_dwDN -= ACCESSORI_UPGRADE_COST;
		else m_dwDN = 0;

		arMaterial.Add((BYTE)sMaterialSlot[0]);
		arMaterial.Add((BYTE)sMaterialSlot[1]);
		arMaterial.Add((BYTE)sMaterialSlot[2]);

		iSuccess = 1;

		break;
	default:
		return ;
	}

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...
	
	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();

	CBufferEx TempBuf;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	if(iSuccess == 1)	TempBuf.Add(SUCCESS);
	else				TempBuf.Add(FAIL);
	TempBuf.Add((BYTE)arMaterial.GetSize());

	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		bySlot = (BYTE)arMaterial[i];
		
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

	arMaterial.RemoveAll();
}

void USER::UpgradeBlessingItemReq(TCHAR *pBuf, int type)
{
	int iSid = -1;
	int iIQ = -1;
	int iCount = 0, iThings = 0;
	int i, j, index = 0;
	int iWeight = 0;
	short sMaterialSlot;

	int iSuccess = 0;

	BYTE tWear = 0;
	BYTE tIQ = 0, bySlot = 0;

	CBufferEx TempBuf;

	ItemList	MyItem[TOTAL_ITEM_NUM];

	CByteArray arMaterial;
	arMaterial.RemoveAll();

	short sSourceSlot = GetShort(pBuf, index);				// ������ ������ ���Թ�ȣ

	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;

	iSid = m_UserItem[sSourceSlot].sSid;
	if(iSid < 0 || iSid >= g_arItemTable.GetSize()) return;
	if( iSid == 669 || iSid == 670 )						// ���׷��̵� �Ϸ��� �������� �۷����������
	{
		SendSystemMsg( IDS_USER_CANT_UPGRADE_ITEM, SYSTEM_ERROR, TO_ME);
		return;
	}
	
	if(g_arItemTable[iSid]->m_byWear >= 6 && (g_arItemTable[iSid]->m_byWear!=122) && (g_arItemTable[iSid]->m_byWear!=123) 
		 && (g_arItemTable[iSid]->m_byWear!=124) && (g_arItemTable[iSid]->m_byWear!=125))
		 return;

	iIQ = m_UserItem[sSourceSlot].tIQ;
	switch(iIQ)
	{
	case NORMAL_ITEM:
		break;
	case MAGIC_ITEM:
		iCount = 2;
		iThings = MATERIAL_MAGIC_UP_ITEM;
		break;
	case RARE_ITEM:
		iCount = 4;
		iThings = MATERIAL_RARE_UP_ITEM;
		break;
	case REMODEL_ITEM:
	case REMODEL_MAGIC_ITEM:
		iCount = 2;
		iThings = MATERIAL_REMODEL_UP_ITEM;
		break;
	case 12: //��ͨ�ټ�
		iCount = 4;
		iThings = MATERIAL_RARE_UP_ITEM;
		break;
	default:
		return;
	}
	if( (g_arItemTable[iSid]->m_byWear==122) || (g_arItemTable[iSid]->m_byWear==123) 
		|| (g_arItemTable[iSid]->m_byWear==124) || (g_arItemTable[iSid]->m_byWear==125)){
			iCount = 2;
			iThings = 1236;
	}

		
	for(i = 0; i < TOTAL_ITEM_NUM; i++)			// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	sMaterialSlot = -1;									
	sMaterialSlot = GetShort(pBuf, index);
	if(sMaterialSlot >= EQUIP_ITEM_NUM && sMaterialSlot < TOTAL_INVEN_MAX)
	{
		iSid = MyItem[sMaterialSlot].sSid;
		if(iSid < 0 || iSid >= g_arItemTable.GetSize()) return;

		if(type == 2)
		{
			tWear = g_arItemTable[iSid]->m_byWear;
			if(tWear != BLESSING_WEAPONLESS_WEAR && tWear != BLESSING_ARMORLESS_WEAR ) return;
		}
		else if(type == 3)
		{
			if(iIQ==12) //�ټ������þ�ʯ��ʯ����
				return;
			tWear = g_arItemTable[iSid]->m_byWear;
			if(tWear != NORMAL_WEAPONLESS_WEAR && tWear != NORMAL_ARMORLESS_WEAR) return;
		}
		else if(type == 12)
		{
			tWear = g_arItemTable[iSid]->m_byWear;
			if(tWear != BLESSING_WEAPONLESS_WEAR && tWear != BLESSING_ARMORLESS_WEAR) return;
			if(g_arItemTable[iSid]->m_byRLevel>60)
				return;
		}else if(type==9)
		{
			tWear = g_arItemTable[iSid]->m_byWear;
			if(tWear != BLESSING_WEAPONLESS_WEAR && tWear != BLESSING_ARMORLESS_WEAR && tWear!=28) return;
		}
		else return;

		if(MyItem[sMaterialSlot].sCount <= 0) return;

		iWeight += g_arItemTable[iSid]->m_byWeight;

		MyItem[sMaterialSlot].sCount -= 1;
		arMaterial.Add((BYTE)sMaterialSlot);
	}
	else return;
	if(type != 12){//12������ʯͷ��������Ҫ��ʯ�Ȳ��ϡ�
		for(i = 0; i < iCount; i++)
		{
			sMaterialSlot = -1;
			sMaterialSlot = GetShort(pBuf, index);
			if(sMaterialSlot >= EQUIP_ITEM_NUM && sMaterialSlot < TOTAL_INVEN_MAX)
			{											
				iSid = MyItem[sMaterialSlot].sSid;
				if(iSid < 0 || iSid >= g_arItemTable.GetSize()) return;
				if(iSid != iThings) return;			
				if(MyItem[sMaterialSlot].sCount <= 0) return;

				iWeight += g_arItemTable[iSid]->m_byWeight;

				MyItem[sMaterialSlot].sCount -= 1;
				arMaterial.Add((BYTE)sMaterialSlot);
			}
			else
			{
				SendSystemMsg(IDS_USER_SHENGSHI, SYSTEM_ERROR, TO_ME);
				return;
			}
		}
	}
	iSuccess = SetBlessingUpgradeItem(sSourceSlot, type);	
	if(MyItem[sSourceSlot].tMagic[5] == 0 && iSuccess == 0) iWeight += g_arItemTable[iSid]->m_byWeight;			// ���Ժ�ȭ�� üũ�Ѵ�.

	if(iSuccess == -1) return;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	index = 1 + arMaterial.GetSize();

	if(!iSuccess)	TempBuf.Add((BYTE)0x00); //����
	else			TempBuf.Add((BYTE)0x01);
	
	TempBuf.Add((BYTE)index);

	TempBuf.Add((BYTE)sSourceSlot);					// �� �����۸� ���� 	
	TempBuf.Add(m_UserItem[sSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[sSourceSlot].sSid);
	TempBuf.Add(m_UserItem[sSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[sSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSourceSlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSourceSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSourceSlot].tIQ); 

	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		bySlot = (BYTE)arMaterial[i];
	
		MakeItemLog( &m_UserItem[bySlot], ITEMLOG_BLESS_USE );
		FlushItemLog( TRUE );
	
		if(m_UserItem[bySlot].sCount <= 1) ReSetItemSlot(&m_UserItem[bySlot]);// �Һ� �̹Ƿ� ���� �ʱ�ȭ�ϰ� 
		else							   m_UserItem[bySlot].sCount -= 1;

		TempBuf.Add(bySlot);
		TempBuf.Add(m_UserItem[bySlot].sLevel);
		TempBuf.Add(m_UserItem[bySlot].sSid);
		TempBuf.Add(m_UserItem[bySlot].sDuration);
		TempBuf.Add(m_UserItem[bySlot].sBullNum);
		TempBuf.Add(m_UserItem[bySlot].sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);

		TempBuf.Add(m_UserItem[bySlot].tIQ); 
	}

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

	Send(TempBuf, TempBuf.GetLength());

	arMaterial.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//	����, �Ƹ�, ���ۿ���, ���۾Ƹӷ� ���׷��̵� �Ҷ�
//
int USER::SetBlessingUpgradeItem(short sSlot, int type)
{
	BYTE tCount = 0;

	int iSuccess = 0;

	int i = 0;
	int last = 0;
	int iRandom = 0;

	int iValue = 0;

	if(m_UserItem[sSlot].sSid < 0 || m_UserItem[sSlot].sSid >= g_arItemTable.GetSize()) return -1;
	if(g_arItemTable[m_UserItem[sSlot].sSid]->m_sDuration <= 0) return -1;
	
	tCount = m_UserItem[sSlot].tMagic[5];
	if(tCount >= MAX_ITEM_UPGRADE_COUNT) return -1;		// 6��° ���Կ� �ش� ���׷��̵� Ƚ��(0~9)�� ����Ѵ�.

	if(g_arItemTable[m_UserItem[sSlot].sSid]->m_byWear == ATTACK_ITEM)
	{
		if(type==9)//ף����ʯ��������������
			return -1;
		iRandom = myrand(1, 10000);						// �������� ������ �������� ����
		if(g_arItemTable[m_UserItem[sSlot].sSid]->m_byClass == STAFF_ITEM)	iValue = ATTACK_UPGRADE_PSI_BAND;	// ���׷��̵� ������ �߰��Ǵ� ��
		else																iValue = ATTACK_UPGRADE_BAND;						

		if(iRandom <= g_ItemAttUpgrade[tCount])
		{			
			if(type == 2)
			{
				iRandom = myrand(1, 10000);				// ��������ϰ�� +3������ ���ʽ��� �����Ҽ� �ִ�
				for(int i = 0; i < 3; i++)
				{
					last += ATTACK_UPGRADE_BAND;		// �ش� ������ ��ŭ ���� ���ش�.
					iSuccess += 1;						// �� �����ۿ����� ��� �����ߴ��� �˷� �ش�.

					if(iRandom <= g_ItemBlessingUpgrade[i]) break;
					if((tCount + iSuccess) >= MAX_ITEM_UPGRADE_COUNT) break;
				}
			}
			else if(type == 12)
			{
				if(g_arItemTable[m_UserItem[sSlot].sSid]->m_byRLevel > 50) return - 1;
				last = ATTACK_UPGRADE_BAND;
				iSuccess = 1;							// 1���� ����Ǹ�
			}
			else 
			{
				last = ATTACK_UPGRADE_BAND;
				iSuccess = 1;							// 1���� ����Ǹ�
			}
		}
	}
	else
	{
		iRandom = myrand(1, 10000);
		if(type==9)
			iRandom=(int)(iRandom - iRandom*0.18);  //ף����ʯ ���18�ɹ�������
		iValue = DEFENSE_UPGRADE_BAND;
		if(iRandom <= g_ItemDefUpgrade[tCount])
		{
			if(type == 2 || type==9)
			{
				iRandom = myrand(1, 10000);				// ��������ϰ�� +3������ ���ʽ��� �����Ҽ� �ִ�
				for(i = 0; i < 3; i++)
				{	
					last += DEFENSE_UPGRADE_BAND;		// �ش� ������ ��ŭ ���� ���ش�.
					iSuccess += 1;						// �� �����ۿ����� ��� �����ߴ��� �˷� �ش�.

					if(iRandom <= g_ItemBlessingUpgrade[i]) break;
					if((tCount + iSuccess) >= MAX_ITEM_UPGRADE_COUNT) break;
				}
			}
			else if(type == 12)
			{
				if(g_arItemTable[m_UserItem[sSlot].sSid]->m_byRLevel > 50) return - 1;
				iRandom = myrand(1, 10000);				// ��������ϰ�� +3������ ���ʽ��� �����Ҽ� �ִ�
				for(i = 0; i < 3; i++)
				{	
					last += DEFENSE_UPGRADE_BAND;		// �ش� ������ ��ŭ ���� ���ش�.
					iSuccess += 1;						// �� �����ۿ����� ��� �����ߴ��� �˷� �ش�.

					if(iRandom <= g_ItemBlessingUpgrade[i]) break;
					if((tCount + iSuccess) >= MAX_ITEM_UPGRADE_COUNT) break;
				}
			}
			else
			{
				last = DEFENSE_UPGRADE_BAND;
				iSuccess = 1;							// 1���� ����Ǹ�
			}
		}
	}

	if(iSuccess >= 1)									// �ش� ��°�� ���������� ������
	{	
		if(m_UserItem[sSlot].tIQ==12)
			m_UserItem[sSlot].tMagic[5]++;
		else
			m_UserItem[sSlot].tMagic[5] = tCount + iSuccess;
		//m_UserItem[sSlot].tMagic[4] = (BYTE)((tCount + iSuccess) * iValue);
		MakeItemLog( &m_UserItem[sSlot], ITEMLOG_BLESS_UPGRADE_SUCCESS );
	}
	else												// �����Ұ�� ��� �Ϲ����� ����� ����޴´�.		
	{
		BOOL bRemove = FALSE;
		int iUp = m_UserItem[sSlot].tMagic[5];
		int iCur = m_UserItem[sSlot].tMagic[4];

		if(type == 3)									// �Ϲ�
		{
			iRandom = myrand(1, 10000);
		
			if(iRandom <= g_ItemNormalDownUpgrade[0]) 
			{
				iUp -= 1;
				iCur -= iValue;
			}
		}
		else if(type == 12)							// �� �������� ������ 1���� ����	
		{
			iUp -= 1;
			iCur -= iValue;
		}
		else											// �� �������� ������ 1���� ����	
		{
			iUp -= 1;
			iCur -= iValue;
		}

		if(iCur < 0) iCur = 0;
		if(iUp < 0) { iUp = 0; bRemove = TRUE; }

		//m_UserItem[sSlot].tMagic[4] = iCur;
		m_UserItem[sSlot].tMagic[5] = iUp;				

		if(bRemove)			// ����...�ѹ��� ���� ������ ������.
		{
			MakeItemLog( &m_UserItem[sSlot], ITEMLOG_BLESS_UPGRADE_FAIL );
			ReSetItemSlot(&m_UserItem[sSlot]);	iSuccess = 0; 
		}
		else MakeItemLog( &m_UserItem[sSlot], ITEMLOG_BLESS_UPGRADE_FAIL );
	}	

	FlushItemLog( TRUE );
	return iSuccess;
}

void USER::ChangeUpgradeAcc()
{
	int i = 0;
	int k = 0;
	int j = 0;

	int sid = -1;
	int count = 0;
	int emptyslot = -1;

	BOOL bExistResult = FALSE;

	CItemTable* pTable = NULL;
	ItemList changeItem;

	ItemList UserItemBackup[TOTAL_ITEM_NUM];

	CWordArray arPlusSlot;	arPlusSlot.RemoveAll();
	CWordArray arMinusSlot;	arMinusSlot.RemoveAll();

	for( i = 0; i < TOTAL_ITEM_NUM; i++ )
	{
		UserItemBackup[i] = m_UserItem[i];
	}

	for( i = EQUIP_ITEM_NUM; i < TOTAL_INVEN_MAX; i++ )
	{
		sid = m_UserItem[i].sSid;

		if( sid >= 0 && sid < g_arItemTable.GetSize() )
		{
			pTable = g_arItemTable[sid];
		}
		else
		{
			pTable = NULL;
		}

		if( !pTable ) continue;

		if( pTable->m_byWear != 6 && pTable->m_byWear != 7 && pTable->m_byWear != 8 )	// �׼������� �ƴ϶��
		{
			continue;
		}

		if( m_UserItem[i].sDuration != 0 )	// ���׷��̵� ������ �� �������̶��
		{
			continue;
		}

		count = m_UserItem[i].sCount;

		if( count <= 0 )	// �ٲ� �������� ī��Ʈ�� 0���϶�� - �߸��� �������̴�. ���⼱ �ش���� �ʴ� ���������� ó���ϰ� �׳� �Ѿ��.
		{
			continue;
		}

		ReSetItemSlot( &changeItem );

		// ���� �׼������� ��ü�� ������ �����͸� �����.
		changeItem.sLevel		= m_UserItem[i].sLevel;
		changeItem.sSid			= sid;
		changeItem.sCount		= 1;
		changeItem.sDuration	= pTable->m_sDuration;
		changeItem.sBullNum		= m_UserItem[i].sBullNum;
		for( k =0; k < MAGIC_NUM; k++ ) changeItem.tMagic[k] = m_UserItem[i].tMagic[k];
		changeItem.tIQ			= m_UserItem[i].tIQ;

		if( count == 1 )
		{
			m_UserItem[i].sLevel		= changeItem.sLevel;
			m_UserItem[i].sSid			= changeItem.sSid;
			m_UserItem[i].sCount		= changeItem.sCount;
			m_UserItem[i].sDuration		= changeItem.sDuration;
			m_UserItem[i].sBullNum		= changeItem.sBullNum;
			for( k =0; k < MAGIC_NUM; k++ ) m_UserItem[i].tMagic[k] = changeItem.tMagic[k];
			m_UserItem[i].tIQ			= changeItem.tIQ;

			arPlusSlot.Add( i );
		}
		else
		{
			for( j = 0; j < count; j++ )	// ��ó�� ������ŭ ����.
			{
				emptyslot = -1;

				for( k = EQUIP_ITEM_NUM; k < TOTAL_INVEN_MAX; k++ )	// �󽽷��� ã�´�.
				{
					if( m_UserItem[k].sSid == -1 )
					{
						emptyslot = k;
						break;
					}
				}

				if( emptyslot == -1 )	// ���ڸ��� ������
				{
					// ������ ����������.
					break;
				}

				m_UserItem[emptyslot].sLevel		= changeItem.sLevel;
				m_UserItem[emptyslot].sSid			= changeItem.sSid;
				m_UserItem[emptyslot].sCount		= changeItem.sCount;
				m_UserItem[emptyslot].sDuration		= changeItem.sDuration;
				m_UserItem[emptyslot].sBullNum		= changeItem.sBullNum;
				for( k =0; k < MAGIC_NUM; k++ ) m_UserItem[emptyslot].tMagic[k] = changeItem.tMagic[k];
				m_UserItem[emptyslot].tIQ			= changeItem.tIQ;

				arPlusSlot.Add( emptyslot );
			}

			if( j <= 0 )	// ��ó�� ������ �ϳ��� ���ٸ�
			{
				// �ƹ��͵� ���Ѱ� ����.
				break;
			}
			else
			{
				m_UserItem[i].sCount -= j;		// ��ó�� ������ŭ ���� �׼����� �������� ���ش�.

				if( m_UserItem[i].sCount <= 0 )
				{
					ReSetItemSlot( &m_UserItem[i] );
				}

				arMinusSlot.Add( i );
			}
		}
	}

	if( arPlusSlot.GetSize() || arMinusSlot.GetSize() )
	{
		if( UpdateUserItemDN() == FALSE )
		{
			for( i = 0; i < TOTAL_ITEM_NUM; i++ )		// ������ ���� ȯ��
			{
				m_UserItem[i] = UserItemBackup[i];
			}

			return;
		}

		if( arPlusSlot.GetSize() )
		{
			UpdateInvenSlot( &arPlusSlot, NULL, 3 );
			arPlusSlot.RemoveAll();
		}

		if( arMinusSlot.GetSize() )
		{
			UpdateInvenSlot( &arMinusSlot, NULL, 4 );
			arMinusSlot.RemoveAll();
		}

		SendSystemMsg( IDS_USER_CHANGE_UPGRADE_ACC_SUCCESS, SYSTEM_NORMAL, TO_ME);
	}
	else
	{
		SendSystemMsg( IDS_USER_CHANGE_UPGRADE_ACC_FAIL, SYSTEM_NORMAL, TO_ME);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	������ ����ó���� �Ѵ�.
//
void USER::RemodelingItemReq(TCHAR *pBuf)
{
	if(m_dwDN < ITEM_REMODELING_COST) 
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return;		// ���׷��̵� �� ���� ������ ����
	}

	short sSourceSid = -1;
	int i, j, index = 0;
	int iWeight = 0;
	short sMaterialSlot[4]	= {-1, -1, -1, -1};
	short sMaterialSid[4]	= {-1, -1, -1, -1};
	int iSuccess = 0, iMaterialCount = 0;
	BYTE bySlot = 0;
	short sPlanzing = 0;

	BOOL bDeleteSource = FALSE, bRemodelSuccess = FALSE;

	CByteArray arMaterial;
	arMaterial.RemoveAll();

	// Source Item
	short sSourceSlot = GetShort(pBuf, index);			// ������ ������ ���Թ�ȣ
	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;

	sSourceSid = m_UserItem[sSourceSlot].sSid;
	if(sSourceSid < 0 || sSourceSid >= g_arItemTable.GetSize()) return;
	if( sSourceSid == 669 || sSourceSid == 670 )		// ���׷��̵� �Ϸ��� �������� �۷����������
	{
		SendSystemMsg( IDS_CANT_REMODELING, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(g_arItemTable[sSourceSid]->m_byWear < 1 && g_arItemTable[sSourceSid]->m_byWear > 5) // ����, ���� �ƴϸ� ������ �� ����.
	{
		SendSystemMsg( IDS_CANT_REMODELING, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(m_UserItem[sSourceSlot].tMagic[5] >= 1)			// ���׷��̵�� �������� ������ �� ����.
	{
		SendSystemMsg( IDS_CANT_REMODELING, SYSTEM_ERROR, TO_ME);
		return;
	}

	switch(m_UserItem[sSourceSlot].tIQ)
	{
	case RARE_ITEM:				// ���� �� �� ���� ������
	case UNIQUE_ITEM:
	case SET_ITEM:
	case REMODEL_MAGIC_ITEM:
		SendSystemMsg( IDS_CANT_REMODELING, SYSTEM_ERROR, TO_ME);
		return;

	case REMODEL_ITEM:			// �̹� 3�� �������� �� ������
		if(m_UserItem[sSourceSlot].tMagic[0] >= 1 && m_UserItem[sSourceSlot].tMagic[1] >= 1 && m_UserItem[sSourceSlot].tMagic[2] >= 1)
		{
			SendSystemMsg( IDS_CANT_REMODELING, SYSTEM_ERROR, TO_ME);
			return;
		}
		break;
	}

	for(i = 0; i < 4; i++)
	{
		sMaterialSlot[i] = GetShort(pBuf, index);
		if(sMaterialSlot[i] == -1) break;
		if(sMaterialSlot[i] < EQUIP_ITEM_NUM || sMaterialSlot[i] >= TOTAL_INVEN_MAX) return;

		sMaterialSid[i] = m_UserItem[sMaterialSlot[i]].sSid;
		if(sMaterialSid[i] < 0 || sMaterialSid[i] >= g_arItemTable.GetSize()) return;

		iMaterialCount++;
	}

	// Remodeling Start ...
	iSuccess = 0;
	if(g_arItemTable[sSourceSid]->m_byWear == ATTACK_ITEM)	// Weapon
	{
		switch(m_UserItem[sSourceSlot].tIQ)
		{
		case NORMAL_ITEM:									// 1 �� ���� �õ�
			for(i = 0; i < iMaterialCount; i++)
			{
				if(sMaterialSid[i] == 678) iSuccess++;			// ƿ����
				else if(sMaterialSid[i] == 858) iSuccess++;		// Ż�׷�
				else if(sMaterialSid[i] == 682 || sMaterialSid[i] == 679 || sMaterialSid[i] == 680 || sMaterialSid[i] == 681 || sMaterialSid[i] == 683) sPlanzing = sMaterialSid[i];
				else return;
			}
			if(iSuccess != 2) return;

			bRemodelSuccess = RemodelingItem(1, &m_UserItem[sSourceSlot], sPlanzing);
			if(bRemodelSuccess) 
			{
				arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
			}

			break;

		case REMODEL_ITEM:
			if(m_UserItem[sSourceSlot].tMagic[0] > 0 && m_UserItem[sSourceSlot].tMagic[1] <= 0)		// 2 �� ���� �õ�
			{
				for(i = 0; i < iMaterialCount; i++)
				{
					if(sMaterialSid[i] == 677) iSuccess++;			// �����׷�
					else if(sMaterialSid[i] == 858) iSuccess++;		// Ż�׷�
					else if(sMaterialSid[i] == 682 || sMaterialSid[i] == 679 || sMaterialSid[i] == 680 || sMaterialSid[i] == 681 || sMaterialSid[i] == 683) sPlanzing = sMaterialSid[i];
					else return;
				}
				if(iSuccess != 2) return;
				
				bRemodelSuccess = RemodelingItem(2, &m_UserItem[sSourceSlot], sPlanzing);
				if(bRemodelSuccess) 
				{
					arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
				}
			}
			else if(m_UserItem[sSourceSlot].tMagic[1] > 0 && m_UserItem[sSourceSlot].tMagic[2] <= 0)	// 3 �� ���� �õ�
			{
				for(i = 0; i < iMaterialCount; i++)
				{
					if(sMaterialSid[i] == 860) iSuccess++;			// ����ũ��
					else if(sMaterialSid[i] == 858) iSuccess++;		// Ż�׷�
					else if(sMaterialSid[i] == 685)					// D �÷�¡
					{
						sPlanzing = sMaterialSid[i];
						iSuccess++;		
					}
					else return;
				}
				if(iSuccess != 3) return;

				bRemodelSuccess = RemodelingItem(3, &m_UserItem[sSourceSlot], sPlanzing);

				arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
				if(bRemodelSuccess == FALSE) 
				{
					bDeleteSource = TRUE;
				}
			}
			else return;
			
			break;
			
		case MAGIC_ITEM:
			for(i = 0; i < iMaterialCount; i++)
			{
				if(sMaterialSid[i] == 860) iSuccess++;			// ����ũ��
				else if(sMaterialSid[i] == 858) iSuccess++;		// Ż�׷�
				else if(sMaterialSid[i] == 685)					// D �÷�¡
				{
					sPlanzing = sMaterialSid[i];
					iSuccess++;		
				}
				else return;
			}
			if(iSuccess != 3) return;

			bRemodelSuccess = RemodelingItem(3, &m_UserItem[sSourceSlot], sPlanzing);
			
			arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
			if(bRemodelSuccess == FALSE) 
			{
				bDeleteSource = TRUE;
			}
			break;
		}
	}
	else if(g_arItemTable[sSourceSid]->m_byWear >= 2 && g_arItemTable[sSourceSid]->m_byWear <= 5)	// Armor
	{
		switch(m_UserItem[sSourceSlot].tIQ)
		{
		case NORMAL_ITEM:
			for(i = 0; i < iMaterialCount; i++)
			{
				if(sMaterialSid[i] == 678) iSuccess++;			// ƿ����
				else if(sMaterialSid[i] == 859) iSuccess++;		// Ŭ����
				else if(sMaterialSid[i] == 682 || sMaterialSid[i] == 679 || sMaterialSid[i] == 680 || sMaterialSid[i] == 681 || sMaterialSid[i] == 683) sPlanzing = sMaterialSid[i];
				else return;
			}
			if(iSuccess != 2) return;

			bRemodelSuccess = RemodelingItem(1, &m_UserItem[sSourceSlot], sPlanzing);
			if(bRemodelSuccess) 
			{
				arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
			}

			break;

		case REMODEL_ITEM:
			if(m_UserItem[sSourceSlot].tMagic[0] > 0 && m_UserItem[sSourceSlot].tMagic[1] <= 0)		// 2 �� ���� �õ�
			{
				for(i = 0; i < iMaterialCount; i++)
				{
					if(sMaterialSid[i] == 677) iSuccess++;			// �����׷�
					else if(sMaterialSid[i] == 859) iSuccess++;		// Ŭ����
					else if(sMaterialSid[i] == 682 || sMaterialSid[i] == 679 || sMaterialSid[i] == 680 || sMaterialSid[i] == 681 || sMaterialSid[i] == 683) sPlanzing = sMaterialSid[i];
					else return;
				}
				if(iSuccess != 2) return;

				bRemodelSuccess = RemodelingItem(2, &m_UserItem[sSourceSlot], sPlanzing);
				if(bRemodelSuccess) 
				{
					arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
				}
			}
			else if(m_UserItem[sSourceSlot].tMagic[1] > 0 && m_UserItem[sSourceSlot].tMagic[2] <= 0)	// 3 �� ���� �õ�
			{
				for(i = 0; i < iMaterialCount; i++)
				{
					if(sMaterialSid[i] == 860) iSuccess++;			// ����ũ��
					else if(sMaterialSid[i] == 859) iSuccess++;		// Ŭ����
					else if(sMaterialSid[i] == 684) iSuccess++;		// W �÷�¡
					else return;
				}
				if(iSuccess != 3) return;
				
				bRemodelSuccess = RemodelingItem(3, &m_UserItem[sSourceSlot], sPlanzing);
				
				arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
				if(bRemodelSuccess == FALSE) 
				{
					bDeleteSource = TRUE;
				}
			}
			else return;

			break;
			
		case MAGIC_ITEM:
			for(i = 0; i < iMaterialCount; i++)
			{
				if(sMaterialSid[i] == 860) iSuccess++;			// ����ũ��
				else if(sMaterialSid[i] == 859) iSuccess++;		// Ŭ����
				else if(sMaterialSid[i] == 684) iSuccess++;		// W �÷�¡
				else return;
			}
			if(iSuccess != 3) return;

			bRemodelSuccess = RemodelingItem(3, &m_UserItem[sSourceSlot], sPlanzing);
			arMaterial.Add((BYTE)sSourceSlot);	// ��ȭ�� ���� �߰�
			if(bRemodelSuccess == FALSE) 
			{
				bDeleteSource = TRUE;
			}
			break;
		}
	}
	else return;

	if(m_dwDN <= ITEM_REMODELING_COST) m_dwDN = 0;				// ���� ���
	else m_dwDN = m_dwDN - ITEM_REMODELING_COST;				// ���� ���
//	if(m_dwDN < 0) m_dwDN = 0;

	if(bDeleteSource)	// ������ �����ؼ� ���� �������� ���� ���
	{
		iWeight += g_arItemTable[sSourceSid]->m_byWeight;		// ���Ժ�ȭ�� üũ�Ѵ�.
		ReSetItemSlot(&m_UserItem[sSourceSlot]);
		
	}

	// Item Log File Generation
	if(bRemodelSuccess)	MakeItemLog(&m_UserItem[sSourceSlot], ITEMLOG_REMODEL_SUCCESS);
	else MakeItemLog(&m_UserItem[sSourceSlot], ITEMLOG_REMODEL_FAIL);
	FlushItemLog(TRUE);

	for(i = 0; i < iMaterialCount; i++)	
	{
		if(m_UserItem[sMaterialSlot[i]].sCount <= 1)	ReSetItemSlot(&m_UserItem[sMaterialSlot[i]]);
		else											m_UserItem[sMaterialSlot[i]].sCount--;
		
		iWeight += g_arItemTable[sMaterialSid[i]]->m_byWeight;	// ���Ժ�ȭ�� üũ�Ѵ�.

		arMaterial.Add((BYTE)sMaterialSlot[i]);	// ��ȭ�� ���� �߰�
	}

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...
	
	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();

	CBufferEx TempBuf;
	TempBuf.Add(UPGRADE_ITEM_RESULT);
	if(bRemodelSuccess)	TempBuf.Add(SUCCESS);
	else				TempBuf.Add(FAIL);
	TempBuf.Add((BYTE)arMaterial.GetSize());

	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		bySlot = (BYTE)arMaterial[i];
		
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

	arMaterial.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	������ ������ �����Ѵ�.
//
BOOL USER::RemodelingItem(int iStage, ItemList* pItem, short sPlanzing)
{
	int i, j, k;
	int iStart, iEnd;
	BYTE tRandom1, tRandom2;
	UINT uRandomSum, uRandomSumOld;
	
	RemodelingTableArray*	pTable;
	CWordArray*				pRandomIndex;
	int						iArmorIndex;

	short sReplace = 0;

	int iAdd = 0, iDel = 0;
	int iMagicIndex = 0;

	switch(iStage)
	{
	case 1:
		pTable = &g_arRemodelingTable1;
		pRandomIndex = &g_arRemodelingRandomIndex1;
		iArmorIndex = g_iRemodelingArmorIndex1;
		iMagicIndex = 0;
		break;
	case 2:
		pTable = &g_arRemodelingTable2;
		pRandomIndex = &g_arRemodelingRandomIndex2;
		iArmorIndex = g_iRemodelingArmorIndex2;
		iMagicIndex = 1;
		break;
	case 3:
		pTable = &g_arRemodelingTable3;
		pRandomIndex = &g_arRemodelingRandomIndex3;
		iArmorIndex = g_iRemodelingArmorIndex3;
		iMagicIndex = 2;
		break;
	default:
		return FALSE;
	}

	if(g_arItemTable[pItem->sSid]->m_byWear == ATTACK_ITEM)
	{
		iStart = 0;
		iEnd = iArmorIndex + 1;
		if(iStage == 1 || iStage == 2) 
		{
			iAdd = 25;
			iDel = 1;
		}
	}
	else
	{
		iStart = iArmorIndex + 1;
		iEnd = pRandomIndex->GetSize();
		if(iStage == 1 || iStage == 2) 
		{
			iAdd = 25;
			iDel = 1;
		}
	}

	tRandom1 = myrand(1, 100);
	tRandom2 = myrand(1, 100);
	uRandomSum = uRandomSumOld = 0;
	for(i = iStart; i < iEnd; i++)	// ���� �Ӽ� ����
	{
		j = pRandomIndex->GetAt(i);
		uRandomSum = uRandomSum + pTable->GetAt(j)->m_tRandom1 - iDel;
		if(iStage != 0 && sPlanzing != 0 && pTable->GetAt(j)->m_sPlan == sPlanzing) uRandomSum += iAdd;
		
		if(tRandom1 >= uRandomSumOld && tRandom1 <= uRandomSum)	// ������ �Ӽ� ������
		{
			uRandomSumOld = uRandomSum = 0;
			k = 0;
			
			while(1)		// ������ �Ӽ��߿� ����� �߰��ɰ��� ����				
			{
				uRandomSum += pTable->GetAt(j + k)->m_tRandom2;
				if(tRandom2 >= uRandomSumOld && tRandom2 <= uRandomSum)
				{
					// ������ Ŭ������ �����Ӽ� Ŭ������ ��ġ�ϴ��� �˻� 
					if(CheckClassItem(g_arItemTable[pItem->sSid]->m_byClass, pTable->GetAt(j + k)->m_tNeedClass))	// ��ġ�ϸ�
					{
						pItem->tMagic[iMagicIndex] = (BYTE)pTable->GetAt(j + k)->m_sSid;
						if(pItem->tIQ == NORMAL_ITEM) pItem->tIQ = REMODEL_ITEM;
						else if(pItem->tIQ == MAGIC_ITEM) pItem->tIQ = REMODEL_MAGIC_ITEM;
						
						return TRUE;
					}
					else	// ��ġ���� ������ ��ü �Ӽ� �ο�
					{
						sReplace = pTable->GetAt(j + k)->m_tReplace;
						if(pTable->GetAt(sReplace - 1)->m_sRid == sReplace)
						{
							pItem->tMagic[iMagicIndex] = (BYTE)pTable->GetAt(sReplace - 1)->m_sSid;
							if(pItem->tIQ == NORMAL_ITEM) pItem->tIQ = REMODEL_ITEM;
							else if(pItem->tIQ == MAGIC_ITEM) pItem->tIQ = REMODEL_MAGIC_ITEM;
							
							return TRUE;
						}
					}
				}
				uRandomSumOld = uRandomSum + 1;
				if(uRandomSum >= 100) break;	// Safe Code
				k++;
			}
		}
		uRandomSumOld = uRandomSum + 1;
	}

	return FALSE;
}

BOOL USER::CheckOverGuildUserCount(int count)
{
	CString strMsg;

	CGuild *pGuild = NULL;

	pGuild = GetGuild( m_dwGuild );

	if(!pGuild) 
	{
		ReleaseGuild();				// ����...

		strMsg.Format( "��带 ã�� �� �����ϴ�" );
		SendSystemMsg( (LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME );
		return FALSE;
	}

	int i = 0;
	int j = 0;
	CGuildUser pGUser;

	for(i = 0; i < MAX_GUILD_USER; i++)
	{
		pGUser = pGuild->m_arUser[i];
		if(pGUser.m_lUsed != 0) j++;
	}

	ReleaseGuild();

	if( j < count ) 
	{
//		SendSystemMsg( IDS_USER_APPLY_CONDITION_GMEMBER, SYSTEM_NORMAL, TO_ME);
		strMsg.Format( "�ּ� %d�� �̻��� ��忡���� ��û�� �� �ֽ��ϴ�.", count );
		SendSystemMsg( (LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME );
		return FALSE;
	}

	return TRUE;
}

void USER::ApplyGuildRun()
{
	CString			str;

	if( !CheckApplyGuildRun() )
	{
		SendSystemMsg( "�̹� ��û�Ǿ� �ֽ��ϴ�.", SYSTEM_NORMAL, TO_ME);
		return;
	}

	char strGuildName[128];
	char strTitle[128];
	char strContent[1024];
	
	sprintf( strGuildName, "%s", m_strGuildName );
	sprintf( strTitle, "GUILD_RUN" );
	sprintf( strContent, "MasterID : %s, MasterAccount : %s", m_strUserID, m_strAccount );

	SDWORD sTitleLen	= _tcslen(strTitle);
	SDWORD sContentLen	= _tcslen(strContent);
	SDWORD sIDLen		= _tcslen(strGuildName);

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	int bbsnum = 2;		// �̺�Ʈ�� �Խ���

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

	int i = 1;
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20,		0, (TCHAR*)strGuildName,0, &sIDLen );
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

	SendSystemMsg( "��� �޸��� �̺�Ʈ ��û�� �Ϸ�Ǿ����ϴ�.", SYSTEM_NORMAL, TO_ME);

	return;
}

BOOL USER::CheckApplyGuildRun()
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[8000];
	BOOL			bExist = FALSE;

	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("select * from bbs_2 where strWriter = \'%s\'"), m_strGuildName);

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );

	if( retcode != SQL_SUCCESS )
	{
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS);

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		retcode = SQLFetch( hstmt );

		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
		{
			bExist = TRUE;
		}
		else if( retcode == SQL_NO_DATA )
		{
		}
		else
		{
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	if( bExist ) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//	��ź��(��Ƽ���ѷ�)�� �����ϰ� ������ ����� ���������� Ŭ���̾�Ʈ�� ������.
//
void USER::SetDecBullNum(BYTE tSlot, short sDec)
{
	if(tSlot < 0 || tSlot >= TOTAL_ITEM_NUM) return;

	short sOldBullNum = m_UserItem[tSlot].sBullNum;
	
	m_UserItem[tSlot].sBullNum -= sDec;
	if(m_UserItem[tSlot].sBullNum < 0) m_UserItem[tSlot].sBullNum = 0;

	if(sOldBullNum != m_UserItem[tSlot].sBullNum) SendBullNum(tSlot);
}

///////////////////////////////////////////////////////////////////////////
//	�ñ� �������� �����Ѵ�.
//
void USER::SetColdDamage()
{
	if(m_tAbnormalKind != ABNORMAL_BYTE_NONE) return;		// �̹� �����̻��� �ɷ��ִ� �����̸� ����
	
	int iRandom = (int)(myrand(1, 1000) / 10);			
	if(iRandom > 53 || iRandom < 50) return;			// �ñ� �������� �ɸ� Ȯ���� 3%

	ClearAbnormalTime(ABNORMAL_TIME);

	m_tAbnormalKind = ABNORMAL_BYTE_COLD;
	m_dwAbnormalTime = COLD_TIME;
	m_dwLastAbnormalTime = GetTickCount();

	if(g_bDebug) SendSystemMsg( IDS_USER_COLD_DAMAGED, SYSTEM_NORMAL, TO_ME);

	CBufferEx TempBuf;
	TempBuf.Add(SET_USER_STATE);
	TempBuf.Add(m_uid + USER_BAND);
//	TempBuf.Add(ABNORMAL_STATUS);
//	TempBuf.Add(ABNORMAL_COLD);
	AddAbnormalInfo(ABNORMAL_COLD);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////
//	ȭ���������� �����Ѵ�.
//
void USER::SetFireDamage()
{
	if(m_tAbnormalKind != ABNORMAL_BYTE_NONE) return;		// �̹� �����̻��� �ɷ��ִ� �����̸� ����

	int iRandom = (int)(myrand(1, 1000) / 10);			
	if(iRandom > 53 || iRandom < 50) return;			// ȭ�� �������� �ɸ� Ȯ���� 3%

	ClearAbnormalTime(ABNORMAL_TIME);

	m_tAbnormalKind = ABNORMAL_BYTE_FIRE;
	m_dwAbnormalTime = FIRE_TIME;
	m_dwLastAbnormalTime = GetTickCount();

	if(g_bDebug) SendSystemMsg( IDS_USER_FIRE_DAMAGED, SYSTEM_NORMAL, TO_ME);

	CBufferEx TempBuf;
	TempBuf.Add(SET_USER_STATE);
	TempBuf.Add(m_uid + USER_BAND);
//	TempBuf.Add(ABNORMAL_STATUS);
//	TempBuf.Add(ABNORMAL_FIRE);
	AddAbnormalInfo(ABNORMAL_FIRE);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////
//	������ ������ ������, ��... ������.
//
void USER::BankOpenReq()
{
	int i, j;
	CBufferEx TempBuf;

	CByteArray arItemSlotList;

	m_dwBankDN = 0;
	InitUserBankItem();								// ������ ������ ��Ź�Ѵ�.

	if(!LoadMemUserBank())
	{
		if(!LoadUserBank()) return;						// ������ ������ ���� �������� ó�� �����Ҷ� ������ �´�.
	}

	for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
	{												// ���� ������ �����۸� �����ֱ����� �����Ѵ�. 
		if(m_UserBankItem[i].sSid >= 0)
		{
			arItemSlotList.Add(i);
		}
	}

//	if(m_sLevel < ) return;							// �̿��Ҽ��ִ� ������ �����Ѵ�.
	TempBuf.Add(BANK_OPEN);
	TempBuf.Add((DWORD)m_dwBankDN);
	TempBuf.Add((BYTE)arItemSlotList.GetSize());

	for(i = 0; i < arItemSlotList.GetSize(); i++)
	{
		BYTE tempSlot = 0;
		tempSlot = arItemSlotList[i];
		TempBuf.Add(tempSlot);
		TempBuf.Add((short)m_UserBankItem[tempSlot].sLevel);
		TempBuf.Add((short)m_UserBankItem[tempSlot].sSid);
		TempBuf.Add((short)m_UserBankItem[tempSlot].sDuration);
		TempBuf.Add((short)m_UserBankItem[tempSlot].sBullNum);
		TempBuf.Add((short)m_UserBankItem[tempSlot].sCount);

		for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_UserBankItem[tempSlot].tMagic[j]);

		TempBuf.Add((BYTE)m_UserBankItem[tempSlot].tIQ);
	}

	Send(TempBuf, TempBuf.GetLength());
}


///////////////////////////////////////////////////////////////////////////
//	������ ������ ������, ���� DB���� ������ �´�.
//
BOOL USER::LoadUserBank()
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_USER_BANK (\'%s\')}"), m_strUserID);

	SQLUINTEGER iDN;
	SQLCHAR		strItem[_BANK_DB];
	
	SQLINTEGER	sInd;

	iDN = 0;
	::ZeroMemory(strItem, sizeof(strItem));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );

	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To Load User Bank Data !!\n");

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
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iDN,		sizeof(iDN),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_BINARY,	strItem,	sizeof(strItem),	&sInd );
		}
		else if( retcode == SQL_NO_DATA )
		{
			g_DB[m_iModSid].ReleaseDB(db_index);	// �����Ͱ� �����Ƿ�
			return FALSE;
		}
		else
		{
			g_DB[m_iModSid].ReleaseDB(db_index);	// �����Ͱ� �����Ƿ�
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

	StrToUserBankItem((LPTSTR)strItem);

	m_dwBankDN = iDN;

	SetMemUserBank((LPCTSTR)strItem);

	return TRUE;
}

BOOL USER::LoadAccountBank()
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_ACCOUNT_BANK (\'%s\')}")/*, m_iMyServer*/, m_strAccount);

	SQLUINTEGER iDN;
	SQLCHAR		strItem[_ACCOUNT_BANK_DB];
	
	SQLINTEGER	sInd;

	iDN = 0;
	::ZeroMemory(strItem, sizeof(strItem));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );

	if( retcode != SQL_SUCCESS )
	{
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS);

	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		retcode = SQLFetch( hstmt );

		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
		{
			int i = 1;
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iDN,		sizeof(iDN),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_BINARY,	strItem,	sizeof(strItem),	&sInd );
		}
		else if( retcode == SQL_NO_DATA )
		{
			g_DB[m_iModSid].ReleaseDB(db_index);	// �����Ͱ� �����Ƿ�
			return TRUE;
		}
		else
		{
			g_DB[m_iModSid].ReleaseDB(db_index);	// �����Ͱ� �����Ƿ�
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

	StrToAccountBankItem((LPTSTR)strItem);

	m_dwAccountBankDN = iDN;

	SetMemAccountBank((LPCTSTR)strItem);

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//	UserBankItem �� Buffer�� ������ Copy �Ѵ�.
//
void USER::StrToUserBankItem(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	MYINT64 itemserial;

	for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
	{
		m_UserBankItem[i].sLevel	= GetShort(pBuf, index);
		m_UserBankItem[i].sSid		= GetShort(pBuf, index);
		m_UserBankItem[i].sDuration = GetShort(pBuf, index);
		m_UserBankItem[i].sBullNum	= GetShort(pBuf, index);
		m_UserBankItem[i].sCount	= GetShort(pBuf, index);

		if(m_UserBankItem[i].sCount <= 0) m_UserBankItem[i].sSid = -1;
		for(j = 0; j < MAGIC_NUM; j++) m_UserBankItem[i].tMagic[j] = GetByte(pBuf, index);
		
		m_UserBankItem[i].tIQ = GetByte(pBuf, index);

		for( j = 0; j < 8; j++ )
		{
			itemserial.b[j] = GetByte( pBuf, index );
		}
		m_UserBankItem[i].iItemSerial = itemserial.i;

		for( j = 0; j < ITEM_USER_RIGHT_NUM; j++ )
		{
			m_UserBankItem[i].uid[j] = -1;
			m_UserBankItem[i].SuccessRate[j] = 0;
		}

		if( m_UserBankItem[i].sSid == g_RR.m_iItemSid )				// �� �������� �ξⷳ�� ��ǰ�̰�
		{
			if( g_strARKRRWinner.CompareNoCase( m_strUserID ) )	// ���� ���ڰ� �ƴ� ���
			{
				ReSetItemSlot( &(m_UserBankItem[i]) );					// ���´�.
			}
		}

		m_UserBankItem[i].dwTime = 0;
	}	
}

///////////////////////////////////////////////////////////////////////////////
//	AccountBankItem �� Buffer�� ������ Copy �Ѵ�.
//
void USER::StrToAccountBankItem(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	MYINT64 itemserial;

	for(i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++)
	{
		m_AccountBankItem[i].sLevel		= GetShort(pBuf, index);
		m_AccountBankItem[i].sSid		= GetShort(pBuf, index);
		m_AccountBankItem[i].sDuration	= GetShort(pBuf, index);
		m_AccountBankItem[i].sBullNum	= GetShort(pBuf, index);
		m_AccountBankItem[i].sCount		= GetShort(pBuf, index);

		if(m_AccountBankItem[i].sCount <= 0) m_AccountBankItem[i].sSid = -1;
		for(j = 0; j < MAGIC_NUM; j++) m_AccountBankItem[i].tMagic[j] = GetByte(pBuf, index);
		
		m_AccountBankItem[i].tIQ = GetByte(pBuf, index);

		for( j = 0; j < 8; j++ )
		{
			itemserial.b[j] = GetByte( pBuf, index );
		}
		m_AccountBankItem[i].iItemSerial = itemserial.i;

		for( j = 0; j < ITEM_USER_RIGHT_NUM; j++ )
		{
			m_AccountBankItem[i].uid[j] = -1;
			m_AccountBankItem[i].SuccessRate[j] = 0;
		}

		m_AccountBankItem[i].dwTime = 0;
	}	
}

///////////////////////////////////////////////////////////////////////////////
//	Buffer �� UserBankItem ������ Copy �Ѵ�.
//
void USER::UserBankItemToStr(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	MYINT64 itemserial;

	for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
	{
		SetShort(pBuf, m_UserBankItem[i].sLevel,	index);
		SetShort(pBuf, m_UserBankItem[i].sSid,		index);
		SetShort(pBuf, m_UserBankItem[i].sDuration,	index);
		SetShort(pBuf, m_UserBankItem[i].sBullNum,	index);
		SetShort(pBuf, m_UserBankItem[i].sCount,	index);

		for(j = 0; j < MAGIC_NUM; j++) SetByte(pBuf, m_UserBankItem[i].tMagic[j], index);

		SetByte(pBuf, m_UserBankItem[i].tIQ, index);

		itemserial.i = m_UserBankItem[i].iItemSerial;

		for( j = 0; j < 8; j++ )
		{
			SetByte( pBuf, itemserial.b[j], index );
		}
	}
}

void USER::UserAccountBankItemToStr(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	MYINT64 itemserial;

	for(i = 0; i < TOTAL_ACCOUNT_BANK_ITEM_NUM; i++)
	{
		SetShort(pBuf, m_AccountBankItem[i].sLevel,		index);
		SetShort(pBuf, m_AccountBankItem[i].sSid,		index);
		SetShort(pBuf, m_AccountBankItem[i].sDuration,	index);
		SetShort(pBuf, m_AccountBankItem[i].sBullNum,	index);
		SetShort(pBuf, m_AccountBankItem[i].sCount,		index);

		for(j = 0; j < MAGIC_NUM; j++) SetByte(pBuf, m_AccountBankItem[i].tMagic[j], index);

		SetByte(pBuf, m_AccountBankItem[i].tIQ, index);

		itemserial.i = m_AccountBankItem[i].iItemSerial;

		for( j = 0; j < 8; j++ )
		{
			SetByte( pBuf, itemserial.b[j], index );
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//	������ ������, ���� DB���� ������ �����Ѵ�.
//
BOOL USER::UpdateUserBank()
{
// fors test_account_bank offical	 if(UpdateMemUserBank()) return TRUE;
    UpdateMemUserBank();
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strFace[10], strSkill[_SKILL_DB], strItem[_ITEM_DB], strPsi[_PSI_DB], strBankItem[_BANK_DB], strAccountBankItem[_ACCOUNT_BANK_DB];
	TCHAR			strQuickItem[_QUICKITEM_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strBankItem, sizeof(strBankItem));
	::ZeroMemory(strFace, sizeof(strFace));
	::ZeroMemory(strSkill, sizeof(strSkill));
	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strPsi, sizeof(strPsi));
	::ZeroMemory(strAccountBankItem, sizeof(strAccountBankItem));

	::ZeroMemory(strQuickItem, sizeof(strQuickItem));

	::CopyMemory(strFace, m_strFace, sizeof(m_strFace));

	UserBankItemToStr(strBankItem);
	UserSkillToStr(strSkill);
	UserItemToStr(strItem);
	UserPsiToStr(strPsi);
	UserAccountBankItemToStr(strAccountBankItem);	

	SDWORD sBankItemLen	= sizeof(strBankItem);
	SDWORD sFaceLen		= sizeof(strFace);
	SDWORD sSkillLen	= sizeof(strSkill);
	SDWORD sItemLen		= sizeof(strItem);
	SDWORD sPsiLen		= sizeof(strPsi);
	SDWORD sQuickLen	= sizeof(strQuickItem);
	SDWORD sAccountBankItemLen = sizeof(strAccountBankItem);

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

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_USER_BANK_DATA (\'%s\',\'%s\',%d,%d,%d,%d,%d, %d,%d,%d, ?, \
		%d,%d,%d, %d,%d, %d, %d,%d,  %d,\
		%d,  %d, %d,%d,%d,%d, %d,%d,%d,%d, \
		?,?,?,?, %d, %d, \
		%d, %d, ?, ?, \
		%d, \
		%d, \
		%d, %d, %d, \
		\'%s\', \
		%d,%d,		%d,%d,		%d,%d)}")/*, 
		m_iMyServer*/, m_strAccount, m_strUserID,	m_sSTR,	m_sCON,	m_sDEX,	m_sVOL,	m_sWIS,	m_iSkin, m_iHair, m_sGender,
		m_curz,	m_curx,	m_cury,		m_dwBuddy, m_dwGuild,		m_dwExp,		m_sPA, m_sSkillPoint,  	m_dwXP,
		m_sMaxHP, m_sHP, m_sMaxPP, m_sPP, m_sMaxSP,	m_sSP,		m_dwDN,	m_sCityRank, m_sLevel,	m_byClass, 
		m_tAbnormalKind, m_dwAbnormalTime, 

		m_dwBankDN, m_dwAccountBankDN,

		m_bLive,
		m_dwSaintTime, 
		m_dwHiExpTime, m_dwMagicFindTime, m_dwNoChatTime,
		m_strLoveName,	//yskang 0.1 m_strLoveName �߰�
		m_tPsiOneKind, m_dwPsiOneTime,		m_tPsiTwoKind, m_dwPsiTwoTime,		m_tPsiThreeKind, m_dwPsiThreeTime);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To Update User Bank Data !!\n");

//		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strFace),	0, (TCHAR*)strFace,		0, &sFaceLen );

		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strSkill),	0, (TCHAR*)strSkill,	0, &sSkillLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strPsi),		0, (TCHAR*)strPsi,		0, &sPsiLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strQuickItem),	0, (TCHAR*)strQuickItem,	0, &sQuickLen );

		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strBankItem),	0, (TCHAR*)strBankItem,		0, &sBankItemLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strAccountBankItem),	0, (TCHAR*)strAccountBankItem,		0, &sAccountBankItemLen );

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//	�þ߹����� �ִ� �������� Ÿ����Ż�� ���Ѵ�.
//
void USER::SendMyTownPotal(BYTE towho, BYTE type)
{
	CBufferEx	TempBuf;
	int i;

	CPoint pos = ConvertToClient( m_curx, m_cury );
	if( pos.x == -1 || pos.y == -1 ) { pos.x = 1; pos.y = 1; }	// �̺κ��� ���߿� �ٲ���

	TempBuf.Add(USER_INFO);
	TempBuf.Add(type);
	TempBuf.Add(m_uid + USER_BAND);

	TempBuf.AddString(m_strUserID);
		
	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);
		
	TempBuf.Add(m_iSkin);
	TempBuf.Add(m_iHair);
	TempBuf.Add((BYTE)m_sGender);
	TempBuf.AddData(m_strFace, 10);
		
	for( i = 0; i < EQUIP_ITEM_NUM; i++) TempBuf.Add(m_UserItem[i].sSid);

	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sMagicMaxHP);

	TempBuf.Add(m_tDir);

//	TempBuf.Add((BYTE)m_strAbnormal[0]);			// Abnormal Info
//	TempBuf.Add(m_tPsiAbnormal);					

	TempBuf.Add(m_dwAbnormalInfo);				// �����̻� ����
	TempBuf.Add(m_dwAbnormalInfo_);
	TempBuf.Add((DWORD)0);
	TempBuf.Add((DWORD)0);

	TempBuf.Add(m_sCityRank);

	TempBuf.Add((int)m_dwGuild);					//&&&&&&&&&&&& Test Code
	TempBuf.AddString(m_strGuildName);			// ��� �̸��� �߰�
	TempBuf.Add(m_sGuildVersion);
	
	TempBuf.Add(m_byClass);
	m_bPkStatus=0x01;
	TempBuf.Add((BYTE)m_bPkStatus);

	TempBuf.AddString(m_strLoveName);//yskang 0.1

	for( i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) 
			TempBuf.Add(m_UserItem[i].sSid);	// EBody

	if(m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
			TempBuf.Add((BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]));
			TempBuf.Add((BYTE)0x00);
	}else{
			TempBuf.Add(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]);
			TempBuf.Add((BYTE)0xff);
	}
		
	TempBuf.AddString(m_PersonalShopName);

	switch(towho)
	{
/*	case TO_ALL:
		SendAll(TempBuf, TempBuf.GetLength());
		break;
*/
	case TO_ME:
		Send(TempBuf, TempBuf.GetLength());
		break;
/*
	case TO_ZONE:
		SendZone(TempBuf, TempBuf.GetLength());
		break;
*/
	case TO_INSIGHT:
	default:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;
	}
}

///////////////////////////////////////////////////////////////////////////
//	������ ȣ���ϴ� Ÿ����Ż��
//
void USER::SendUserTownPotal(USER *pUser, BYTE tMode)
{
	if( !pUser ) return;

	CBufferEx	TempBuf;

	CPoint pos = ConvertToClient( pUser->m_curx, pUser->m_cury );
	if( pos.x == -1 || pos.y == -1 ) { pos.x = 1; pos.y = 1; }	// �̺κ��� ���߿� �ٲ���

	TempBuf.Add(USER_INFO);
	TempBuf.Add(tMode);
	
	TempBuf.Add(pUser->m_uid + USER_BAND);

	if(tMode == INFO_TOWNPOTAL_DELETE) { Send(TempBuf, TempBuf.GetLength()); return; }

	TempBuf.AddString(pUser->m_strUserID);

	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);

	TempBuf.Add(pUser->m_iSkin);
	TempBuf.Add(pUser->m_iHair);
	TempBuf.Add((BYTE)pUser->m_sGender);
	TempBuf.AddData(pUser->m_strFace, 10);
int i;
	for(i = 0; i < EQUIP_ITEM_NUM; i++) TempBuf.Add(pUser->m_UserItem[i].sSid);

//	TempBuf.Add(pUser->m_sHP);
//	TempBuf.Add(pUser->m_sMaxHP);
	TempBuf.Add(pUser->m_sHP);
	TempBuf.Add(pUser->m_sMagicMaxHP);

	TempBuf.Add(pUser->m_tDir);

//	TempBuf.Add((BYTE)pUser->m_strAbnormal[0]);			// Abnormal Info
//	TempBuf.Add(pUser->m_tPsiAbnormal);						//&&&&&&&&&&&& Test Code	

	TempBuf.Add(pUser->m_dwAbnormalInfo);				// �����̻� ����
	TempBuf.Add(pUser->m_dwAbnormalInfo_);
	TempBuf.Add((DWORD)0);
	TempBuf.Add((DWORD)0);
	
	TempBuf.Add(pUser->m_sCityRank);

	TempBuf.Add((int)pUser->m_dwGuild);					//&&&&&&&&&&&& Test Code
	TempBuf.AddString(pUser->m_strGuildName);			// ��� �̸��� �߰�
	TempBuf.Add(pUser->m_sGuildVersion);

	TempBuf.Add(pUser->m_byClass);
	TempBuf.Add((BYTE)pUser->m_bPkStatus);

	TempBuf.AddString(pUser->m_strLoveName);//yskang 0.1

	for( i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) TempBuf.Add(pUser->m_UserItem[i].sSid);	// EBody
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


///////////////////////////////////////////////////////////////////////////
//	���࿡�� �����, ������ �������� ����Ѵ�.
//
void USER::BankItemMoveReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int index = 0;
	BYTE type = GetByte(pBuf, index);

	switch(type)
	{
	case BANK_ITEM_INPUT:
		BankInPut(pBuf + index);
		break;
	case BANK_ITEM_OUTPUT:
		BankOutPut(pBuf + index);
		break;
	case BANK_DN_INPUT:
		BankInPutDN(pBuf + index);
		break;
	case BANK_DN_OUTPUT:
		BankOutPutDN(pBuf + index);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////
//	���࿡ �������� �����Ѵ�.
//
void USER::BankInPut(TCHAR *pBuf)
{	
	int i;
	int tDestSlot;
	int index = 0;
	int iOver = 0;
	short sSid = -1;
	short sHaveCount = 0;

	BYTE result = SUCCESS;

	CBufferEx TempBuf;

	ItemList MyItem[TOTAL_ITEM_NUM], BackItem;

	BYTE tSourceSlot = GetByte(pBuf, index);	// ����� �������� �ִ� ���� ��ġ
	short sCount = GetShort(pBuf, index);		// �󸶸�ŭ ����
												// ������ �κ��� �����ϴ�.			
	if(tSourceSlot < EQUIP_ITEM_NUM || tSourceSlot >= TOTAL_INVEN_MAX) { result = FAIL; goto go_result; }

	sSid = m_UserItem[tSourceSlot].sSid;
	sHaveCount = m_UserItem[tSourceSlot].sCount;
												
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { result = FAIL; goto go_result; }	// �߸��� sSid

	if(sCount > BANK_MAX_ITEM) 
	{
		SendSystemMsg( IDS_USER_ONCE_MOVE_MAX, SYSTEM_ERROR, TO_ME);
		result = FAIL; goto go_result;
	}
												// ������ ������ ������ ���� ���			
	if(sCount <= 0 || sCount > sHaveCount) { result = FAIL; goto go_result; }
			
	if(g_arItemTable[sSid]->m_sDuration > 0 && sCount > 1) { result = FAIL; goto go_result; }
	
	for(i = 0; i < TOTAL_ITEM_NUM; i++)	// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	ReSetItemSlot(&BackItem);					// DB���п� ����� ����� �ʱ�ȭ
												
	tDestSlot = GetSameItem(m_UserItem[tSourceSlot], BANK_SLOT);	// �ϴ� ��ĥ�� �ִ���, ��ġ�� ���� �������� �ִ��� ã�ƺ���.
	
	/***************************���� ���� ó��********************************************/
	if(tDestSlot >= 0)							// �׻� ������ �߰� �Ǵ°��� �������� ó��
	{											
		BackItem = m_UserBankItem[tDestSlot];

		if(m_UserBankItem[tDestSlot].sCount >= BANK_MAX_ITEM)
		{
			SendSystemMsg( IDS_USER_SAVE_MAX_COUNT, SYSTEM_ERROR, TO_ME);
			result = FAIL; goto go_result;
		}

		if((m_UserBankItem[tDestSlot].sCount + sCount) > BANK_MAX_ITEM)
		{										// MAX���� ������ ��ä��� ������ ó��			
			iOver = m_UserBankItem[tDestSlot].sCount + sCount - BANK_MAX_ITEM;
			if(iOver <= 0) { result = FAIL; goto go_result; }

			m_UserBankItem[tDestSlot].sCount = BANK_MAX_ITEM;
			sCount = sCount - iOver;
		}
		else m_UserBankItem[tDestSlot].sCount += sCount;
	}
	else
	{											//	�߰�
		tDestSlot = GetEmptySlot(BANK_SLOT);

		if(tDestSlot == -1) 
		{
			result = FAIL; goto go_result; 
		}

		m_UserBankItem[tDestSlot].sLevel = m_UserItem[tSourceSlot].sLevel;
		m_UserBankItem[tDestSlot].sSid = m_UserItem[tSourceSlot].sSid;
		m_UserBankItem[tDestSlot].sDuration = m_UserItem[tSourceSlot].sDuration;
		m_UserBankItem[tDestSlot].sBullNum = m_UserItem[tSourceSlot].sBullNum;
		m_UserBankItem[tDestSlot].sCount = sCount;
		for(i = 0; i < MAGIC_NUM; i++) m_UserBankItem[tDestSlot].tMagic[i] = m_UserItem[tSourceSlot].tMagic[i];
		m_UserBankItem[tDestSlot].tIQ = m_UserItem[tSourceSlot].tIQ;
		m_UserBankItem[tDestSlot].iItemSerial = m_UserItem[tSourceSlot].iItemSerial;
	}

	index = 0;
	index = g_arItemTable[m_UserItem[tSourceSlot].sSid]->m_byWeight * sCount;
	/**************************���� �κ� ó��*********************************************/
	if(sCount >= sHaveCount && iOver == 0) ReSetItemSlot(&m_UserItem[tSourceSlot]);	
	else m_UserItem[tSourceSlot].sCount -= sCount;
	
	/**************************DB Update ó��*********************************************/
	if(UpdateUserBank() == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ����
		{
			m_UserItem[i] = MyItem[i];
		}
		m_UserBankItem[tDestSlot] = BackItem;
	}

go_result:
	TempBuf.Add(BANK_ITEM_MOVE_RESULT);

	if(result == FAIL)
	{
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	result = (BYTE)0x01;
	TempBuf.Add(result);

	TempBuf.Add((BYTE)tDestSlot);				// �������	
	TempBuf.Add(m_UserBankItem[tDestSlot].sLevel);
	TempBuf.Add(m_UserBankItem[tDestSlot].sSid);
	TempBuf.Add(m_UserBankItem[tDestSlot].sDuration);
	TempBuf.Add(m_UserBankItem[tDestSlot].sBullNum);
	TempBuf.Add(m_UserBankItem[tDestSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserBankItem[tDestSlot].tMagic[i]);
	TempBuf.Add(m_UserBankItem[tDestSlot].tIQ);

	TempBuf.Add((BYTE)tSourceSlot);				// ���� �κ�
	TempBuf.Add(m_UserItem[tSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[tSourceSlot].sSid);
	TempBuf.Add(m_UserItem[tSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[tSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[tSourceSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tSourceSlot].tMagic[i]);
	TempBuf.Add(m_UserItem[tSourceSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());

	m_iCurWeight -= index;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

//	SendQuickChange();							// �������� ����Ѱ� ��ҵǴ��� �˾ƺ���. 
//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
}

///////////////////////////////////////////////////////////////////////////
//	���࿡ �������� �� ã�´�.
//
void USER::BankOutPut(TCHAR *pBuf)
{
	int i, iWeight = 0;
	int tDestSlot;
	int index = 0;
	int iOver = 0;
	short sSid = -1;
	short sHaveCount = 0;

	BYTE result = SUCCESS;

	CBufferEx TempBuf;

	ItemList MyItem[TOTAL_ITEM_NUM], BackItem;

	BYTE tSourceSlot = GetByte(pBuf, index);	// ����� �������� �ִ� ���� ��ġ
	short sCount = GetShort(pBuf, index);
												// ������ �κ��� �����ϴ�.			
	if(tSourceSlot >= TOTAL_BANK_ITEM_NUM) { result = FAIL; goto go_result; }

	sSid = m_UserBankItem[tSourceSlot].sSid;
	sHaveCount = m_UserBankItem[tSourceSlot].sCount;

	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { result = FAIL; goto go_result; }

	if(sCount <= 0 || sCount > sHaveCount/* || sCount > BANK_MAX_ITEM*/) { result = FAIL; goto go_result; }

	//if(g_arItemTable[sSid]->m_byWear <= 5 && sCount > 1) { result = FAIL; goto go_result; }
	if(g_arItemTable[sSid]->m_sDuration > 0 && sCount > 1) { result = FAIL; goto go_result; }

	iWeight = g_arItemTable[sSid]->m_byWeight * sCount;
	if(m_iMaxWeight < m_iCurWeight + iWeight)
	{
		SendSystemMsg( IDS_USER_OVER_WEIGHT1, SYSTEM_ERROR, TO_ME);
		result = FAIL; 
		goto go_result;
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++)	// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	ReSetItemSlot(&BackItem);					// DB���п� ����� ����� �ʱ�ȭ
	BackItem = m_UserBankItem[tSourceSlot];
												// �ϴ� ��ĥ�� �ִ���, ��ġ�� ���� �������� �ִ��� ã�ƺ���.
	tDestSlot = GetSameItem(m_UserBankItem[tSourceSlot], INVENTORY_SLOT);
	
	/***************************���� ���� ó��********************************************/
	if(tDestSlot >= 0)							// �׻� ������ �߰� �Ǵ°��� �������� ó��
	{		
		m_UserItem[tDestSlot].sCount += sCount;
	}
	else
	{											//	�߰�
		tDestSlot = GetEmptySlot(INVENTORY_SLOT);

		if(tDestSlot == -1) 
		{
			result = FAIL; goto go_result; 
		}

		m_UserItem[tDestSlot].sLevel = m_UserBankItem[tSourceSlot].sLevel;
		m_UserItem[tDestSlot].sSid = m_UserBankItem[tSourceSlot].sSid;
		m_UserItem[tDestSlot].sDuration = m_UserBankItem[tSourceSlot].sDuration;
		m_UserItem[tDestSlot].sBullNum = m_UserBankItem[tSourceSlot].sBullNum;
		m_UserItem[tDestSlot].sCount = sCount;
		for(i = 0; i < MAGIC_NUM; i++) m_UserItem[tDestSlot].tMagic[i] = m_UserBankItem[tSourceSlot].tMagic[i];
		m_UserItem[tDestSlot].tIQ = m_UserBankItem[tSourceSlot].tIQ;
		m_UserItem[tDestSlot].iItemSerial = m_UserBankItem[tSourceSlot].iItemSerial;
	}
	
	/**************************���� ���� �κ� ó��*********************************************/
	if(sCount >= sHaveCount) ReSetItemSlot(&m_UserBankItem[tSourceSlot]);	
	else m_UserBankItem[tSourceSlot].sCount -= sCount;
	
	/**************************DB Update ó��*********************************************/
	if(UpdateUserBank() == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ����
		{
			m_UserItem[i] = MyItem[i];
		}
		m_UserBankItem[tSourceSlot] = BackItem;

		result = FAIL;
	}

go_result:
	TempBuf.Add(BANK_ITEM_MOVE_RESULT);

	if(result == FAIL)
	{
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	result = (BYTE)0x02;
	TempBuf.Add(result);
	TempBuf.Add((BYTE)tSourceSlot);
	TempBuf.Add(m_UserBankItem[tSourceSlot].sLevel);
	TempBuf.Add(m_UserBankItem[tSourceSlot].sSid);
	TempBuf.Add(m_UserBankItem[tSourceSlot].sDuration);
	TempBuf.Add(m_UserBankItem[tSourceSlot].sBullNum);
	TempBuf.Add(m_UserBankItem[tSourceSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserBankItem[tSourceSlot].tMagic[i]);
	TempBuf.Add(m_UserBankItem[tSourceSlot].tIQ);
	TempBuf.Add((BYTE)tDestSlot);
	TempBuf.Add(m_UserItem[tDestSlot].sLevel);
	TempBuf.Add(m_UserItem[tDestSlot].sSid);
	TempBuf.Add(m_UserItem[tDestSlot].sDuration);
	TempBuf.Add(m_UserItem[tDestSlot].sBullNum);
	TempBuf.Add(m_UserItem[tDestSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tDestSlot].tMagic[i]);
	TempBuf.Add(m_UserItem[tDestSlot].tIQ);
	Send(TempBuf, TempBuf.GetLength());

	m_iCurWeight += iWeight;
	GetRecoverySpeed();
}


///////////////////////////////////////////////////////////////////////////
//	���࿡ �Ա��Ѵ�.
///////////////////////////////////////////////////////////////////////////
void USER::BankInPutDN(TCHAR *pBuf)
{
	CBufferEx TempBuf;

	BYTE result; 
	int index = 0;
	DWORD BackBankDN = 0, BackMyDN = 0;

	DWORD InputDN = GetDWORD(pBuf, index);

	TempBuf.Add(BANK_ITEM_MOVE_RESULT);

	if(InputDN <= 0 || InputDN > m_dwDN) 
	{
		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}
	
	BackMyDN = m_dwDN;
	BackBankDN = m_dwBankDN;
										// ���࿡ �Ա�
	if(!CheckMaxValueReturn(m_dwBankDN, InputDN))
	{									// ��, MAX���̸� ������...
		CheckMaxValue(m_dwBankDN, InputDN);
		if(m_dwBankDN < InputDN) InputDN = 0;
		else InputDN = m_dwBankDN - InputDN;
	}
	else m_dwBankDN += InputDN;
										// ������ �ִ� �����ݿ��� ����.
	if(InputDN >= m_dwDN) m_dwDN = 0;
	else m_dwDN -= InputDN;
//	if(InputDN == m_dwDN) m_dwDN = 0;
//	else m_dwDN -= InputDN;

	if(UpdateUserBank() == FALSE)		// DB UpDate
	{
		m_dwDN = BackMyDN;
		m_dwBankDN = BackBankDN;

		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	result = (BYTE)0x03;				// 3 : DN �Ա�
	TempBuf.Add(result);

	TempBuf.Add(m_dwBankDN);			// ���೻ �Ա��� �ѱݾ�
	TempBuf.Add(m_dwDN);				// ������ �ѱݾ�

	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////
//	���࿡ ����Ѵ�.
///////////////////////////////////////////////////////////////////////////
void USER::BankOutPutDN(TCHAR *pBuf)
{
	CBufferEx TempBuf;

	BYTE result; 
	int index = 0;
	DWORD BackBankDN = 0, BackMyDN = 0;

	DWORD OutputDN = GetDWORD(pBuf, index);

	TempBuf.Add(BANK_ITEM_MOVE_RESULT);

	if(OutputDN <= 0 || OutputDN > m_dwBankDN)			// �ʹ� ũ��
	{
		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	BackMyDN = m_dwDN;					// ���
	BackBankDN = m_dwBankDN;
										// ����� �����ݿ� �ջ��Ѵ�.
	if(!CheckMaxValueReturn(m_dwDN, OutputDN))
	{									// ��, MAX���̸� ������...
		CheckMaxValue(m_dwDN, OutputDN);
		if(m_dwDN < OutputDN) OutputDN = 0;
		else OutputDN = m_dwDN - OutputDN;
	}
	else m_dwDN += OutputDN;
										// ���࿡�� ����.
	if(OutputDN >= m_dwBankDN) m_dwBankDN = 0;
	else m_dwBankDN -= OutputDN;

	if(UpdateUserBank() == FALSE)		// DB UpDate
	{
		m_dwDN = BackMyDN;
		m_dwBankDN = BackBankDN;

		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	result = (BYTE)0x04;				// 4 : DN ���
	TempBuf.Add(result);

	TempBuf.Add(m_dwBankDN);			// ���೻ �Ա��� �ѱݾ�
	TempBuf.Add(m_dwDN);				// ������ �ѱݾ�

	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////
//	����ŸƮ�ϸ� ���ӻ󿡼� ������ ��������.
//
void USER::RestartReq(TCHAR *pBuf)
{
	if( m_bLogOut == TRUE ) return;
	if( m_state != STATE_GAMESTARTED ) return;

	g_pMainDlg->BridgeServerUserRestart(m_uid, m_strUserID);
	return;





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

void USER::RestartReqWithThread(TCHAR *pBuf)
{
	LOGINOUTTHREADDATA *pLIOTD;
	pLIOTD = new LOGINOUTTHREADDATA;
	pLIOTD->CODE = RESTART_REQ;
	pLIOTD->UID = m_uid;
	pLIOTD->ID[0] = '\0';

	EnterCriticalSection( &m_CS_LoginData );
	RecvLoginData.AddTail(pLIOTD);
	nLoginDataCount = RecvLoginData.GetCount();
	LeaveCriticalSection( &m_CS_LoginData );
}

///////////////////////////////////////////////////////////////////////////
//	����ŸƮ�ϸ� ���ӻ󿡼� ACCOUNT LOGIN���·� �ǵ��� ����.
//
BOOL USER::SendRestartLoginResult()
{
	int		index = 0;
	BYTE	result = FAIL, error_code = 0;
	int		old_index = 0;
	TCHAR	szTemp[8096];

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
	SetByte(m_TempBuf, RESTART_RESULT, index );
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
		Send(m_TempBuf, index);
		return FALSE;
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
					error_code = UNKNOWN_ERR;
					SetByte(szTemp, error_code, old_index);
					Send(szTemp, old_index);
					return FALSE;
				}
			}
		}
	}
	Send(m_TempBuf, index);
	return TRUE;
}


BOOL USER::IsPKZone(USER *pUser)
{
	BOOL mePK = FALSE;
	BOOL youPK = FALSE;

	int index = 0;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return FALSE;

	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0) 
	{
		if(pUser->m_dwGuild == m_dwGuild) return FALSE;			// �ʵ����϶� ���� ������ PK�Ҽ� ����.
	}
	if(m_curz == 409) return TRUE;
	if(m_ZoneIndex < 0 || m_ZoneIndex >= g_TownPotal.GetSize()) return FALSE;

//	if(!g_TownPotal[m_ZoneIndex]->iPkMode) return FALSE;		// PK �Ҽ��ִ� ���� ����(������)�̴�
	ZONEINFO* pZoneInfo = GetZoneInfo( m_curz );
	if( !pZoneInfo ) return FALSE;
	if( !pZoneInfo->iPkMode ) return FALSE;
//21 guildhouse war
	if(m_curz == 12) return TRUE;		// ^^ �ӽ��ڵ�....

	int metype = CheckInvalidMapType();
	if(metype == 1) return FALSE;

	int type = pUser->CheckInvalidMapType();
	if(type == 1) return FALSE;

	// �ο����� üũ
#ifndef _EVENT_RR		// ���� �ְ� �̺�Ʈ�� ������
	if( m_curz == g_RR.m_iZoneNum )
	{
		if( g_RR.m_bRRStatus != RR_START ) return FALSE;
	}
#endif

	if(!CheckInvalidZoneState(type))
	{
		if(m_dwGuild > 0 && pUser->m_dwGuild == m_dwGuild) return FALSE;

		return TRUE;
	}
	else
	{
//		if(type < 0 || type >= g_arMapTable.GetSize()) return TRUE;
//		if(metype < 0 || metype >= g_arMapTable.GetSize()) return TRUE;

		int mapindex = GetGuildMapIndex(metype);
		if(mapindex <= -1 || mapindex >= g_arMapTable.GetSize()) return TRUE;
		mePK = (BOOL)g_arMapTable[mapindex]->m_tPkMode;

		if( pUser->m_ZoneIndex < 0 || pUser->m_ZoneIndex >= g_zone.GetSize() ) return FALSE;
		if( pUser->m_curx >= g_zone[pUser->m_ZoneIndex]->m_sizeMap.cx || pUser->m_curx < 0 ) return FALSE;
		if( pUser->m_cury >= g_zone[pUser->m_ZoneIndex]->m_sizeMap.cy || pUser->m_cury < 0 ) return FALSE;
		type = ((g_zone[pUser->m_ZoneIndex]->m_pMap[pUser->m_curx][pUser->m_cury].m_dwType & 0xFF00 ) >> 8);
		mapindex = GetGuildMapIndex(type);
		// if(mapindex <= -1) return TRUE;
		if(mapindex < 0 || mapindex >= g_arMapTable.GetSize()) return FALSE;
		youPK = (BOOL)g_arMapTable[mapindex]->m_tPkMode;

//		mePK = (BOOL)g_arMapTable[metype]->m_tPkMode;
//		type = ((g_zone[pUser->m_ZoneIndex]->m_pMap[pUser->m_curx][pUser->m_cury].m_dwType & 0xFF00 ) >> 8);
//		youPK = (BOOL)g_arMapTable[type]->m_tPkMode;

		if(mePK != youPK) return FALSE;
		else return mePK;
	}

	return TRUE;				
}

BOOL USER::IsCity()
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;
	if(m_curx >= g_zone[m_ZoneIndex]->m_sizeMap.cx || m_curx < 0) return FALSE;
	if(m_cury >= g_zone[m_ZoneIndex]->m_sizeMap.cy || m_cury < 0) return FALSE;

	int type = ((g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_dwType & 0xFF00 ) >> 8);

	if(type == 1) return TRUE;
	if(type == 12) return TRUE;		// ����嵵 ���þȿ� �����Ƿ�

	return FALSE;				
}


///////////////////////////////////////////////////////////////////////////
//	������ �⺻ ��ġ�� �����Ӽ��� ��ȭ�� �����ͷ� ���ؼ� �����Ѵ�.  
//
void USER::SetUserToMagicUser(int iMaxHPUp)
{
	int iTempHP = 0, iTempPP = 0;

	m_sMagicSTR = m_sSTR + m_DynamicUserData[MAGIC_STR_UP];
	m_sMagicCON = m_sCON + m_DynamicUserData[MAGIC_CON_UP];
	m_sMagicDEX = m_sDEX + m_DynamicUserData[MAGIC_DEX_UP];
	m_sMagicVOL = m_sVOL + m_DynamicUserData[MAGIC_VOL_UP];
	m_sMagicWIS = m_sWIS + m_DynamicUserData[MAGIC_WIS_UP];

	m_sMagicMaxHP = g_sHPConst[m_byClass] * m_sMagicCON + g_sHPLV[m_byClass] * (m_sLevel - 1) + g_sHPAdd[m_byClass];
	m_sMagicMaxPP = g_sPPConst[m_byClass] * m_sMagicWIS + g_sPPLV[m_byClass] * (m_sLevel - 1) + g_sPPAdd[m_byClass];
	m_sMagicMaxSP = g_sSPConst[m_byClass] * m_sMagicCON + g_sSPLV[m_byClass] * (m_sLevel - 1) + g_sSPAdd[m_byClass];

	iTempHP = (int)((double)m_DynamicEBodyData[EBODY_CON_TO_HP] / 100 * (double)m_sMagicCON);
	iTempPP = (int)((double)m_DynamicEBodyData[EBODY_WIS_TO_PP] / 100 * (double)m_sMagicWIS);

	m_sMagicMaxHP += m_DynamicUserData[MAGIC_MAX_HP_UP] + (int)((double)m_DynamicEBodyData[EBODY_CON_TO_HP] / 100 * (double)m_sMagicCON);	// �ϴ������� �⺻���ΰ��� ��� + ���⼭ �����Ӽ�	
	m_sMagicMaxPP += m_DynamicUserData[MAGIC_MAX_PP_UP] + (int)((double)m_DynamicEBodyData[EBODY_WIS_TO_PP] / 100 * (double)m_sMagicWIS);
	m_sMagicMaxSP += m_DynamicUserData[MAGIC_MAX_SP_UP];
	
	m_iMaxWeight = (m_sMagicCON + m_sMagicSTR) * 10 + g_ClassWeight[m_byClass] + m_sLevel * 6;// ���� ����ִ� �ִ� ������ ���Ѵ�.
	m_iMaxWeight += (int)((double)m_DynamicEBodyData[EBODY_STR_TO_WEIGHT] / 100 * (double)m_sMagicSTR);

	// ������ �������� Ư���� �ɷ���...
	if(m_sLevel >= ADD_USER_LEVEL) 
	{
		m_sMagicMaxHP += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sHP;
		m_sMagicMaxPP += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sPP;
		m_iMaxWeight += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sWeight;
	}

	m_iMaxWeight += (int)((double)m_DynamicEBodyData[EBODY_WEIGHT_UP] / 100 * (double)m_iMaxWeight);

	// �ִ� HP ���׼��� jjs
	switch(iMaxHPUp)
	{
	case -1 :
	case 0:
		break;
	case 1:
		m_sMagicMaxHP = (int)(m_sMagicMaxHP * 1.2 + 0.5);
		break;
	}

	if(m_sHP > m_sMagicMaxHP) m_sHP = m_sMagicMaxHP;	// �ִ� HP ����
}

///////////////////////////////////////////////////////////////////////////
//	������ ���� ������ �ִ� ��ų�� sSid�� ��ġ�ϴ� ��ų�� ������ ���Ѵ�.
//
short USER::GetSkillLevel(BYTE tWeaponClass, short sSid)
{
	if(tWeaponClass < BRAWL || tWeaponClass > FIREARMS) return 0;

	int iStart = tWeaponClass * SKILL_NUM; 
	short sLevel = 0;
	for(int i = iStart; i < iStart + SKILL_NUM; i++)
	{
		if(m_UserSkill[i].sSid == sSid)
		{
			sLevel = m_UserSkill[i].tLevel;
			if(sLevel < 0) sLevel = 0;
			break;
		}
	}

	return sLevel;
}

///////////////////////////////////////////////////////////////////////////
//	������ �����ð����� Hast ���·� �����.
//
void USER::SetHaste(int iTime)
{
	if(m_tIsOP == 1) return;					// ��� �϶��� ���̿��л��°� ���ϸ� �ȵȴ�. (������ Ǯ����...)

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}

	ClearAbnormalTime(PSI_ONE_TIME);

	m_dwHasteTime = iTime * 1000;		// jjs07 2001.11.23
	m_dwLastHasteTime = GetTickCount();

	if(g_bDebug) SendSystemMsg(_T("Haste Start"), SYSTEM_NORMAL, TO_ME);

	CBufferEx TempBuf;
	TempBuf.Add(SET_USER_STATE);
	TempBuf.Add(m_uid + USER_BAND);
	AddAbnormalInfo(ABNORMAL_HASTE);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	
	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////
//	������ �ð����� ������ ���� 20% ���� ��Ų��.
//
void USER::SetShield(int iTime)
{
	if(m_tIsOP == 1) return;					// ��� �϶��� ���̿��л��°� ���ϸ� �ȵȴ�. (������ Ǯ����...)

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}

	BOOL bExistShield = CheckAbnormalInfo(ABNORMAL_SHIELD);

	ClearAbnormalTime(PSI_ONE_TIME);
	AddAbnormalInfo(ABNORMAL_SHIELD);

	m_dwShieldTime = iTime * 1000;		// jjs07 2001.11.23
	//m_dwShieldTime = DEFAULT_PSI_DELAY + (DWORD)((double)(iVol*1000)/3.0 + 0.5);	// old
	m_dwLastShieldTime = GetTickCount();

	if(g_bDebug) SendSystemMsg(_T("Shield Start"), SYSTEM_NORMAL, TO_ME);

	if(bExistShield == FALSE)
	{
		CBufferEx TempBuf;
		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);
		SendInsight(TempBuf, TempBuf.GetLength());
		//SendExactScreen(TempBuf, TempBuf.GetLength());
	}
}
void USER::SetBigShield(int iTime)
{
	if(m_tIsOP == 1) return;					// ��� �϶��� ���̿��л��°� ���ϸ� �ȵȴ�. (������ Ǯ����...)

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}

	BOOL bExistShield = CheckAbnormalInfo(ABNORMAL_SHIELD) || CheckAbnormalInfo(ABNORMAL_BIGSHIELD);

	ClearAbnormalTime(PSI_ONE_TIME);
	AddAbnormalInfo(ABNORMAL_BIGSHIELD);

	m_dwBigShieldTime = iTime * 1000;		// jjs07 2001.11.23
	//m_dwShieldTime = DEFAULT_PSI_DELAY + (DWORD)((double)(iVol*1000)/3.0 + 0.5);	// old
	m_dwLastBigShieldTime = GetTickCount();

	if(g_bDebug) SendSystemMsg(_T("Big Shield Start"), SYSTEM_NORMAL, TO_ME);

	if(bExistShield == FALSE)
	{
		CBufferEx TempBuf;
		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);
		SendInsight(TempBuf, TempBuf.GetLength());
		//SendExactScreen(TempBuf, TempBuf.GetLength());
	}
}

///////////////////////////////////////////////////////////////////////////
//	������ �ð����� ������ DEX ���� 10 ���� ��Ų��.
//
void USER::SetDexUp(int iTime)
{
	if(m_tIsOP == 1) return;					// ��� �϶��� ���̿��л��°� ���ϸ� �ȵȴ�. (������ Ǯ����...)

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		m_dwMaxHPUpTime = 0;

		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}

	ClearAbnormalTime(PSI_ONE_TIME);

	m_dwDexUpTime = iTime * 1000;
	m_dwLastDexUpTime = GetTickCount();

	if(g_bDebug) SendSystemMsg(_T("Dex Up Start"), SYSTEM_NORMAL, TO_ME);

	CBufferEx TempBuf;
	TempBuf.Add(SET_USER_STATE);
	TempBuf.Add(m_uid + USER_BAND);
//	TempBuf.Add(ABNORMAL_PSI);
//	TempBuf.Add(ABNORMAL_DEX_UP);
	AddAbnormalInfo(ABNORMAL_DEX_UP);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////
//	������ �ð����� ������ Max HP ���� 20% ���� ��Ų��.
//
void USER::SetMaxHPUp(int iTime)
{
	if(m_tIsOP == 1) return;					// ��� �϶��� ���̿��л��°� ���ϸ� �ȵȴ�. (������ Ǯ����...)

	ClearAbnormalTime(PSI_ONE_TIME);

	m_dwMaxHPUpTime = iTime * 1000;
	m_dwLastMaxHPUpTime = GetTickCount();

	if(g_bDebug) SendSystemMsg(_T("Max HP Up Start"), SYSTEM_NORMAL, TO_ME);

	CBufferEx TempBuf;
	TempBuf.Add(SET_USER_STATE);
	TempBuf.Add(m_uid + USER_BAND);
//	TempBuf.Add(ABNORMAL_PSI);
//	TempBuf.Add(ABNORMAL_MAX_HP_UP);
	AddAbnormalInfo(ABNORMAL_MAX_HP_UP);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	Send(TempBuf, TempBuf.GetLength());
	SetUserToMagicUser(1);
	//-----------------------------------------
	//yskang 0.7 MAX HP UP �ٴ� ���߿� Ǯ���� �հ� �ɸ� �� ó�� ��� ���� ���� ���� ����
	CBufferEx	TempBuf2;
	TempBuf2.Add(MAX_HP_EXIT);
	TempBuf2.Add(m_sHP);
	TempBuf2.Add(m_sMagicMaxHP);
	Send(TempBuf2,TempBuf2.GetLength());
	//SendMyInfo(TO_ME, INFO_MODIFY); //��ü ����Ÿ�� ������ �ʴ´�.
	//--------------------------------------
	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
}

void USER::SetAdamantine(int iTime)
{
	if( m_tIsOP == 1 ) return;

	ClearAbnormalTime(PSI_TWO_TIME);
	m_dwAdamantineTime = iTime * 1000;
	m_dwLastAdamantineTime = GetTickCount();

	if( g_bDebug ) SendSystemMsg( _T("Adamantine Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;
	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
//	TempBuf.Add( ABNORMAL_PSI );
//	TempBuf.Add( ABNORMAL_ADAMANTINE );
	AddAbnormalInfo(ABNORMAL_ADAMANTINE);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	Send( TempBuf, TempBuf.GetLength() );
}

void USER::SetMightyWeapon(int iTime)
{
	if( m_tIsOP == 1 ) return;

	ClearAbnormalTime(PSI_TWO_TIME);
	m_dwMightyWeaponTime = iTime * 1000;
	m_dwLastMightyWeaponTime = GetTickCount();

	if( g_bDebug ) SendSystemMsg( _T("MightyWeapon Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;
	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
//	TempBuf.Add( ABNORMAL_PSI );
//	TempBuf.Add( ABNORMAL_MIGHTYWEAPON );
	AddAbnormalInfo(ABNORMAL_MIGHTYWEAPON);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);

	Send( TempBuf, TempBuf.GetLength() );
}

void USER::SetBerserker(int iTime)
{
	if( m_tIsOP == 1 ) return;

	ClearAbnormalTime(PSI_TWO_TIME);
	m_dwBerserkerTime = iTime * 1000;
	m_dwLastBerserkerTime = GetTickCount();

	if( g_bDebug ) SendSystemMsg( _T("Berserker Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;
	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	AddAbnormalInfo(ABNORMAL_BERSERKER);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	SendInsight( TempBuf, TempBuf.GetLength() );
//	SendExactScreen(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////////
//	�̵� �ӵ��� 20% ������ ���ش�.
//
void USER::SetFastRun(int iTime)
{
	if( m_tIsOP == 1 ) return;

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		m_dwMaxHPUpTime = 0;

		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}

	ClearAbnormalTime(PSI_ONE_TIME);
	m_dwFastRunTime = iTime * 1000;
	m_dwLastFastRunTime = GetTickCount();

	if( g_bDebug ) SendSystemMsg( _T("FastRun Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;
	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	AddAbnormalInfo(ABNORMAL_FASTRUN);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	SendInsight( TempBuf, TempBuf.GetLength() );
//	SendExactScreen(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////
//	������ �ð����� ���� ���̿����� ���� ���ϰ� �Ѵ�.
//
BOOL USER::SetMindShock(int iTime)
{
	if( m_tIsOP == 1 ) return FALSE;

	int iRandom = myrand(1, 100);
	if(iRandom > SUCCESS_RATE_MIND_SHOCK) return FALSE;	// ���ε� ��ũ�� ���� Ȯ���� �ִ�.

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		m_dwMaxHPUpTime = 0;

		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}
    if ( m_dwBigShieldTime != 0 )
	{
	CBufferEx TempttBuf;
            DeleteAbnormalInfo(ABNORMAL_BIGSHIELD);
			TempttBuf.Add(SET_USER_STATE);
			TempttBuf.Add(m_uid + USER_BAND);
			TempttBuf.Add(m_dwAbnormalInfo);
			TempttBuf.Add(m_dwAbnormalInfo_);		
		    SendInsight(TempttBuf, TempttBuf.GetLength());
	}
	ClearAbnormalTime(PSI_ONE_TIME);
	m_dwMindShockTime = (iTime * 1000 )+ 5000;	// 15�ʷ� ���� 2002.11.14
	m_dwLastMindShockTime = GetTickCount();

	if( g_bDebug ) SendSystemMsg( _T("Mind Shock Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;
	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	AddAbnormalInfo(ABNORMAL_MINDSHOCK);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
//	SendInsight( TempBuf, TempBuf.GetLength() );
	SendExactScreen(TempBuf, TempBuf.GetLength());

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	Mind Shock �� ���� ��ȣ�Ѵ�.
//
void USER::SetMindGuard(int iTime)
{
	if( m_tIsOP == 1 ) return;

	m_dwMindShockTime = 0;
	m_dwLastMindShockTime = GetTickCount();
	DeleteAbnormalInfo(ABNORMAL_MINDSHOCK);

	m_dwMindGuardTime = iTime * 1000;
	m_dwLastMindGuardTime = GetTickCount();
	AddAbnormalInfo(ABNORMAL_MINDGUARD);

	if( g_bDebug ) SendSystemMsg( _T("Mind Guard Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;
	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
//	SendInsight( TempBuf, TempBuf.GetLength() );
	SendExactScreen(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////
//	�ڽ��� ���������� ���δ�.
//
void USER::SetPsiShield(int iTime)
{
	if( m_tIsOP == 1 ) return;

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		m_dwMaxHPUpTime = 0;

		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}

	ClearAbnormalTime(PSI_ONE_TIME);
	m_dwPsiShieldTime = iTime * 1000;
	m_dwLastPsiShieldTime = GetTickCount();

	if( g_bDebug ) SendSystemMsg( _T("Psionic Shield Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;

	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	AddAbnormalInfo(ABNORMAL_PSISHIELD);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
	SendInsight( TempBuf, TempBuf.GetLength() );
//	SendExactScreen(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////
//	���带 �����ϰ� �������� �ָ� 20%�� Ȯ���� ���带 �ı��Ѵ�.
//
void USER::SetPiercingShield(int iTime)
{
	if( m_tIsOP == 1 ) return;

	if(CheckAbnormalInfo(ABNORMAL_MAX_HP_UP))
	{
		m_dwMaxHPUpTime = 0;

		SetUserToMagicUser(-1);
		SendMyInfo(TO_ME, INFO_MODIFY);
		if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	}

	ClearAbnormalTime(PSI_ONE_TIME);
	m_dwPiercingShieldTime = iTime * 1000;
	m_dwLastPiercingShieldTime = GetTickCount();

	if( g_bDebug ) SendSystemMsg( _T("Piercing Shield Start"), SYSTEM_NORMAL, TO_ME );

	CBufferEx TempBuf;
	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	AddAbnormalInfo(ABNORMAL_PIERCING_SHIELD);
	TempBuf.Add(m_dwAbnormalInfo);
	TempBuf.Add(m_dwAbnormalInfo_);
//	SendInsight( TempBuf, TempBuf.GetLength() );
	SendExactScreen(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////
//	������ �ð����� ��Ҽ��� ���� ã�´�.
//
void USER::SetHide(int iTime)
{
/*
	if(m_tIsOP == 1) return;					// ��� �϶��� ���̿��л��°� ���ϸ� �ȵȴ�. (������ Ǯ����...)

	m_tPsiAbnormal = ABNORMAL_HIDE;
	m_dwHideTime = iTime * 1000;		// jjs07 2001.11.23
	//m_dwHideTime = 10000;	// old
	m_dwLastHideTime = GetTickCount();

	if(g_bDebug) SendSystemMsg(_T("Hide Start"), SYSTEM_NORMAL, TO_ME);

	CBufferEx TempBuf;
	TempBuf.Add(SET_USER_STATE);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add(ABNORAML_PSI);
	TempBuf.Add(ABNORMAL_HIDE);
	TempBuf.Add((BYTE)0);
	TempBuf.Add((BYTE)0);
	TempBuf.Add((BYTE)0);
	TempBuf.Add((BYTE)0);
	Send(TempBuf, TempBuf.GetLength());
	SendInsight(TempBuf, TempBuf.GetLength());	// ���� �Ǵ� ���̵尡 �̻�
*/
}

/////////////////////////////////////////////////////////////////////////////////////////
//	����ȸ��, �ڷ���Ʈó��
//
BOOL USER::Teleport(int xpos, int ypos)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return FALSE;

	BYTE result = FAIL;
	int will_x, will_y;
	int index = 0;
	int	nCurMapType, nTellMapType;

	CPoint ptTeleport;

	CPoint startPt;

	will_x = xpos;	// �����̷��� ��
	will_y = ypos;

	if(will_x <= 0) will_x = m_curx;
	if(will_y <= 0) will_y = m_cury;

	if(will_x >= g_zone[m_ZoneIndex]->m_sizeMap.cx) will_x = g_zone[m_ZoneIndex]->m_sizeMap.cx - 1;
	if(will_y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) will_y = g_zone[m_ZoneIndex]->m_sizeMap.cy - 1;

	startPt.x = m_curx;
	startPt.y = m_cury;

	CPoint pt = FindNearAvailablePoint_S(will_x, will_y);
	if(pt.x != -1 && pt.y != -1) 
	{
		// ���� �ο����� ����忡 �ְ�
#ifndef _EVENT_RR 
		if( m_curz == g_RR.m_iZoneNum )
		{
			// ���� ���� ������ �ϴ� ���� �Ӽ��� �ٸ��ٸ�
			if( CheckInvalidMapType() != ((g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_dwType & 0xFF00 ) >> 8) )
			{
				return FALSE;
			}
		}
#else
		if( m_curz == g_RR.m_iZoneNum || m_curz == 61 ||  m_curz == 62 ||  m_curz == 63 )
		{
			// ���� ���� ������ �ϴ� ���� �Ӽ��� �ٸ��ٸ�
			if( CheckInvalidMapType() != ((g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_dwType & 0xFF00 ) >> 8) )
			{
				return FALSE;
			}
		}
#endif

		nCurMapType = CheckInvalidMapType();
		nTellMapType = ((g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_dwType & 0xFF00 ) >> 8);

		// ���� ���� ���ִ� ���� ���������� �ƴϰ� ���ϴ� ��ǥ�� ���������̶�� ����
		if( (nCurMapType != 8 && nCurMapType != 10 && nCurMapType != 15)
			&& (nTellMapType == 8 || nTellMapType == 10 || nTellMapType == 15) )
			return FALSE;

		ptTeleport = ConvertToClient(pt.x, pt.y);		

		if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, 
			(long)g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, (long)0) == (long)0)
		{
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
		}
		else return FALSE;

		m_curx = pt.x;
		m_cury = pt.y;

		result = SUCCESS;
	}
	else return FALSE;
	
	m_tDir = GetDirection(startPt, pt);		// ���� ������ ǥ���Ѵ�.;

	index = 0;
	SetByte(m_TempBuf, PSI_TOWN_POTAL, index);
	SetByte(m_TempBuf, result, index);

	SetByte(m_TempBuf, 0, index);				// �׻� ������  // ���߿� ���ٰ�

	SetInt(m_TempBuf, m_uid + USER_BAND, index);

	SetShort(m_TempBuf, ptTeleport.x, index);
	SetShort(m_TempBuf, ptTeleport.y, index);
	SetShort(m_TempBuf, m_curz, index);
	
	Send(m_TempBuf, index);

//	SendMyTownPotal(TO_INSIGHT, INFO_TOWNPOTAL);
	SendMyInfo(TO_INSIGHT, INFO_TOWNPOTAL);
	SightRecalc();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//	���� ������?
//
void USER::IsDeadUser()
{
	if(m_bLive == USER_LIVE || m_tIsOP == TRUE ) return;
	//if(m_sHP > 0) return;

	int i;
	int iPotalZone = -1;

	int rand = 0;

	int x = 0;		// Ÿ����Ż�� ����� DB��ǥ�� �������� ���ÿ��� ��Ƴ�	
	int y = 0;

	CPoint ptPotal, pt;

	ptPotal.x = 1; ptPotal.y = 1;
/*
	for(i = 0; i < g_TownPotal.GetSize(); i++)
	{
		if(g_TownPotal[i]->iZone == m_curz) { iPotalIndex = i; break; }
	}

	if(iPotalIndex <0) return;
*/
//	pt = GetTownPotal(iPotalZone);
//	x = pt.x;
//	y = pt.y;

//	if(iPotalZone <0) return;
//	x = g_TownPotal[iPotalIndex]->iPotalX;
//	y = g_TownPotal[iPotalIndex]->iPotalY;
/*
	if(g_TownPotal[iPotalIndex]->iPotalZone != m_curz)
	{
		SetZoneIndex(g_TownPotal[iPotalIndex]->iPotalZone);		// �� �ε�������
		m_curz = g_TownPotal[iPotalIndex]->iPotalZone;			// �� ��ȯ(�׾����� ����� ������ �̵��ؾ��ϹǷ� ���� �ٲ۴�.)
	}
*/
	for(i = 0; i < g_TownPotal.GetSize(); i++)					// �ƴϸ� ����� ���÷� �̵��Ѵ�.
	{
		if(g_TownPotal[i]->iZone == m_curz) { iPotalZone = i; break; }
	}

	if(iPotalZone < 0) return;

	CPoint temp = ConvertToClient(m_curx, m_cury);

	if(g_TownPotal[iPotalZone]->iPotalX <= 0) { x = g_TownPotal[iPotalZone]->iPotalX1; y = g_TownPotal[iPotalZone]->iPotalY1; }
	else if(g_TownPotal[iPotalZone]->iPotalX1 <= 0) { x = g_TownPotal[iPotalZone]->iPotalX; y = g_TownPotal[iPotalZone]->iPotalY; }
	else
	{
		int dx1 = abs(g_TownPotal[iPotalZone]->iPotalX - temp.x);
		int dy1 = abs(g_TownPotal[iPotalZone]->iPotalY - temp.y);

		int dx2 = abs(g_TownPotal[iPotalZone]->iPotalX1 - temp.x);
		int dy2 = abs(g_TownPotal[iPotalZone]->iPotalY1 - temp.y);

		if( (dx1 + dy1) < (dx2 + dy2) ) { x = g_TownPotal[iPotalZone]->iPotalX; y = g_TownPotal[iPotalZone]->iPotalY; }
		else							{ x = g_TownPotal[iPotalZone]->iPotalX1; y = g_TownPotal[iPotalZone]->iPotalY1; }
	}

	// alisia
	/*
	if(g_TownPotal[iPotalZone]->iPotalZone != m_curz)
	{
		SetZoneIndex(g_TownPotal[iPotalZone]->iPotalZone);		// �� �ε�������
		m_curz = g_TownPotal[iPotalZone]->iPotalZone;			// �� ��ȯ(�׾����� ����� ������ �̵��ؾ��ϹǷ� ���� �ٲ۴�.)
	}

	ptPotal = ConvertToServer(x, y);			// ���� ���� ���� ��ǥ��ȭ�� ...

	i = SEARCH_TOWN_POTAL_COUNT;				// ��ǥ�� ã������ Ƚ��
	
	if(IsMovable_S(ptPotal.x, ptPotal.y) == FALSE)
	{
		while(TRUE)								// Ÿ����Ż ���� ��ǥ�� ��´�.
		{
			rand = myrand(-TOWN_POTAL_SIZE, TOWN_POTAL_SIZE);
			ptPotal.x += rand; ptPotal.y += rand;
			
			if(IsMovable_S(ptPotal.x, ptPotal.y) == TRUE) break;
			i--;
			if(i <= 0) 
			{
				ptPotal = ConvertToServer(x, y);
				break;
			}
		}
	}

	m_curx = ptPotal.x;
	m_cury = ptPotal.y;
	*/
	ZoneMoveReq( g_TownPotal[iPotalZone]->iPotalZone, x, y );
}

/////////////////////////////////////////////////////////////////////////////////////////
//��������
//
int USER::GetFinalDamage(USER *pUser, int nInitDamage, BYTE tMyWeapon,BOOL &bIsCritical,int max)//yskang 0.3
{
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return 0;

	int iFinalDamage = 0, iFinalTemp;

	int iBasic = (int)((double)pUser->m_sMagicCON/3 + 0.5);
	if(iBasic < 0) iBasic = 0;

	BYTE tWeaponClass = 255;
	BOOL bCanUseSkill = pUser->IsCanUseWeaponSkill(tWeaponClass);

	int		iDefense = 1;
	double	dIron = 0;
	double	dShield = 0;
	double	dGuard = 0;
	double	dVital = 0;
	double	dCAttack = 0;
	double	dABDefense = 0;
	double	dDefenseUP = 0;
	double	dAdamantine = 0;
	double	dBerserker = 0;

	int		iSkillSid = 0;
	int		iIronLevel = 0;
	int		iGuardLevel = 0;
	int		iCounterAttackLevel = 0;
	int		iVitalLevel = 0;
	int		iABDefenseLevel = 0;
	int		iDefenseUPLevel = 0;

	int		iGS = 0;
	int		iVS = 0;
	int		iCA = 0;

	int i = 0;
	int iRandom = 0;
	int tClass = tWeaponClass * SKILL_NUM;
	int	tMyClass = tMyWeapon * SKILL_NUM;

	int iRandomProtect = (int)((double)XdY(1, 1000) / 10 + 0.5);



	iDefense = pUser->GetDefense();						// ��

	if(tWeaponClass != 255)
	{
		for(i = tClass; i < tClass + SKILL_NUM; i++)	// IronSkill
		{
			iSkillSid = pUser->m_UserSkill[i].sSid;

			if(iSkillSid == SKILL_IRON)					// 0 index
			{
				iIronLevel = pUser->m_UserSkill[i].tLevel;
				if(iIronLevel < 0) iIronLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iIronLevel >= 1) 
				{
					iIronLevel += pUser->m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ pUser->m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				}
				
				if(iIronLevel >= SKILL_LEVEL) iIronLevel = SKILL_LEVEL - 1;
				if(iSkillSid >= g_arSkillTable.GetSize()) continue;
				
				iBasic = (int)((double)iBasic * (1 + (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iIronLevel) / 100));
			}

			if(iSkillSid == SKILL_CRITICAL_GUARD)					// Critical Guard 11 index
			{
				iGuardLevel = pUser->m_UserSkill[i].tLevel;		
				if(iGuardLevel < 0) iGuardLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iGuardLevel >= 1) iGuardLevel += pUser->m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ pUser->m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
				if(iGuardLevel >= SKILL_LEVEL) iGuardLevel = SKILL_LEVEL - 1;
				if(iSkillSid >= g_arSkillTable.GetSize()) continue;
				
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if(max==0){//����Ǳ�ɱ�������������ܼ��ܡ�
					if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iGuardLevel))
					{
						dGuard = (double)(nInitDamage * (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iGuardLevel)/100.0);
					}
				}
			}

			if(iSkillSid == SKILL_BACK_ATTACK)					// CounterAttack 2 index
			{
				// �ݰ� ������� �ѱ��̸� ����
				if(m_byClass == FIREARMS || m_byClass == STAFF) // ���� �ѱ�ų� ���̿����̸� �ݰݹ���
				{
					if(tMyWeapon != 255) continue;	// ���ָ��̸� �ݰݵ������� �޴´�
				}

				iCounterAttackLevel = pUser->m_UserSkill[i].tLevel;		
				if(iCounterAttackLevel < 0) iCounterAttackLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iCounterAttackLevel >= 1) iCounterAttackLevel += pUser->m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ pUser->m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
				if(iCounterAttackLevel >= SKILL_LEVEL) iCounterAttackLevel = SKILL_LEVEL - 1;
				if(iSkillSid >= g_arSkillTable.GetSize()) continue;
				
				if(GetDistance(pUser->m_curx, pUser->m_cury, 1) == FALSE) iCA = 0;		// ���Ÿ��϶��� ��ų�� �ҿ����
				else
				{
					iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
					if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iCounterAttackLevel)) iCA = 1;
				}				

				dCAttack = (double)(nInitDamage * iCA * (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iCounterAttackLevel) / 100.0);
			}

			if(iSkillSid == SKILL_ABSOLUTE_DEFENSE)					// Absolute Defense
			{
				iABDefenseLevel = pUser->m_UserSkill[i].tLevel;		
				if(iABDefenseLevel < 0) iABDefenseLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iABDefenseLevel >= 1) iABDefenseLevel += pUser->m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
				if(iABDefenseLevel >= SKILL_LEVEL) iABDefenseLevel = SKILL_LEVEL - 1;
				if(iSkillSid >= g_arSkillTable.GetSize()) continue;
				
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iABDefenseLevel))
				{
					dABDefense = (double)(iDefense * (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iABDefenseLevel)/100.0);
				}
			}
			if(iSkillSid == SKILL_DEFENSE_UP)					// Defense up
			{
				iDefenseUPLevel = pUser->m_UserSkill[i].tLevel;		
				if(iDefenseUPLevel < 0) iDefenseUPLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iDefenseUPLevel >= 1) iDefenseUPLevel += pUser->m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
				if(iDefenseUPLevel >= SKILL_LEVEL) iDefenseUPLevel = SKILL_LEVEL - 1;
				if(iSkillSid >= g_arSkillTable.GetSize()) continue;
				
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iDefenseUPLevel))
				{
					dDefenseUP = (double)(iDefense * (double)g_arSkillTable[iSkillSid]->m_arInc.GetAt(iDefenseUPLevel)/100.0);
				}
			}
		}
	}

	// ���� ���
	if(pUser->m_bNecklaceOfShield && pUser->m_dwShieldTime != 0)		
	{
		if(m_dwPiercingShieldTime != 0)	dShield = (double)(nInitDamage * 0.01);
		else							dShield = (double)(nInitDamage * 0.10);
	}
	if(pUser->m_bNecklaceOfShield && pUser->m_dwBigShieldTime != 0)		
	{
		if(m_dwPiercingShieldTime != 0)	dShield = (double)(nInitDamage * 0.01);
		else							dShield = (double)(nInitDamage * 0.10);
	}
	else if(pUser->m_bNecklaceOfShield || pUser->m_dwShieldTime != 0 )	
	{
		if(m_dwPiercingShieldTime != 0)	dShield = 0;
		else dShield = (double)(nInitDamage * 0.10);
	}
	else if( pUser->m_dwBigShieldTime !=0 && !pUser->m_bNecklaceOfShield)
	{
		if(m_dwPiercingShieldTime != 0)	dShield = 0;
		else dShield = (double)(nInitDamage * 0.10);
	}
	if(m_dwPiercingShieldTime != 0)	
	{
        
		iRandom = (int)((double)myrand(1, 1000) / 10 + 0.5);
		if(iRandom < SUCCESS_RATE_PIERCING_SHIELD)	// �Ǿ�� ����� 10% Ȯ���� ���带 ����.
		{
		  if ( pUser->m_dwShieldTime !=0 ) 
		  {
			pUser->m_dwShieldTime = 0;
			pUser->m_dwLastShieldTime = GetTickCount();
         	CBufferEx TempBuf;
			pUser->DeleteAbnormalInfo(ABNORMAL_SHIELD);
			TempBuf.Add(SET_USER_STATE);
			TempBuf.Add((int)(pUser->m_uid + USER_BAND));
			TempBuf.Add(pUser->m_dwAbnormalInfo);
			TempBuf.Add(pUser->m_dwAbnormalInfo_);			
			pUser->SendInsight(TempBuf, TempBuf.GetLength());
		  }
		  if ( pUser->m_dwBigShieldTime !=0 )
		  {
			pUser->m_dwBigShieldTime = 0;
			pUser->m_dwLastBigShieldTime = GetTickCount();
	        CBufferEx TempqqBuf;
			pUser->DeleteAbnormalInfo(ABNORMAL_BIGSHIELD);
			TempqqBuf.Add(SET_USER_STATE);
			TempqqBuf.Add((int)(pUser->m_uid + USER_BAND));
			TempqqBuf.Add(pUser->m_dwAbnormalInfo);
			TempqqBuf.Add(pUser->m_dwAbnormalInfo_);		
			pUser->SendInsight(TempqqBuf, TempqqBuf.GetLength());
		  }

		}
	}
	
	if(pUser->m_bNecklaceOfShield) pUser->SendAccessoriDuration(SID_NECKLACE_OF_SHIELD);

	if( pUser->m_dwAdamantineTime != 0 )
	{
		dAdamantine = (double)( (double)iDefense * 0.1 );
	}
	if( pUser->m_dwBerserkerTime != 0 )
	{
		dBerserker = (double)( (double)iDefense * 0.15 );
	}
	//�ܱ����� 15%��
	if(pUser->m_iSkin==2){
		dBerserker =dBerserker+ (double)( (double)iDefense * 0.15 );
	}
	

	iDefense = (int)( iDefense + dABDefense + dDefenseUP + dAdamantine + dBerserker );

	if(tMyWeapon != 255)
	{
		for(i = tMyClass; i < tMyClass + SKILL_NUM; i++)	// Vital Critical
		{
			iSkillSid = m_UserSkill[i].sSid;

			if(iSkillSid == SKILL_VITAL_CRITICAL)			// 12 index
			{
				iVitalLevel = m_UserSkill[i].tLevel;		
				if(iVitalLevel < 0) iVitalLevel = 0;
				
				// �����ۿ� ���� ��ų ���� ����
				if(iVitalLevel >= 1) iVitalLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];
				
				if(iVitalLevel >= SKILL_LEVEL) iVitalLevel = SKILL_LEVEL - 1;		//�ִ� 41�̹Ƿ� �޸𸮴� 40���� ����
				if(iSkillSid >= g_arSkillTable.GetSize()) continue;
				
				iRandom = (int)((double)XdY(1, 1000) / 10 + 0.5);
				if(iRandom < g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iVitalLevel))
				{
					dVital = (double)(g_arSkillTable[iSkillSid]->m_arInc.GetAt(iVitalLevel)/100.0);
					
					if(iRandomProtect <= RANDOM_PROTECT && pUser->m_bEarringOfProtect)	// ����Ʈ �Ͱ���
					{
						dVital = 0;
						pUser->SendAccessoriDuration(SID_EARRING_OF_PROTECT);
					}
				}
			}
		}
	}

	if(dVital > 0)
	{
		bIsCritical = TRUE; //yskang 0.3
		iFinalDamage = (int)(nInitDamage - (iDefense * dVital * (double)pUser->m_DynamicEBodyData[EBODY_VITAL_RESIST]/100 + \
			                 (int)((double)pUser->m_DynamicEBodyData[EBODY_CON_TO_VITAL_RESIST] / 100 * pUser->m_sMagicCON) + \
							 (int)((double)pUser->m_DynamicEBodyData[EBODY_DEX_TO_VITAL_RESIST] / 100 * pUser->m_sMagicDEX) + \
							 iBasic + dShield + dGuard)); 
	}
	else
	{
		iFinalDamage = (int)(nInitDamage - (iDefense + iBasic + dShield + dGuard)); 
	}

	if(iFinalDamage < 0) iFinalDamage = 0;
	if(iFinalDamage <= 15)
	{
		iFinalTemp = iFinalDamage;
		iFinalDamage += (int)((double)nInitDamage * 0.2 + 1.5);	// �ּҴ������ �ִ� 15���� �Ѵ�.
		if(iFinalDamage > 15) iFinalDamage = 15;
		iFinalDamage = max(iFinalDamage, iFinalTemp);
	}

	if(dCAttack > 0)	//	�ݰ��� �ִٸ� �����ڿ��� �ݰݸ�ŭ �ش�.
	{
		iDefense = (int)((double)m_sMagicCON/3 + 0.5);		// ��
		iCA = (int)dCAttack - iDefense;						// ���� �ݰ� ������

		iCA = iCA * (1 - (int)((double)pUser->m_DynamicEBodyData[EBODY_BACK_RESIST]/100));
		iCA -= ((int)((double)pUser->m_DynamicEBodyData[EBODY_CON_TO_BACK_RESIST] / 100 * pUser->m_sMagicCON) + \
			   (int)((double)pUser->m_DynamicEBodyData[EBODY_DEX_TO_BACK_RESIST] / 100 * pUser->m_sMagicDEX)  );

		if(iCA > 0)	SetDamage(iCA);							// ������ ����� �Ѵ�.
	}

	return iFinalDamage;
}

////////////////////////////////////////////////////////////////////////////////////
//	����� ���̵����� �˻��Ѵ�.
//
BOOL USER::IsReservedID(char *szName)
{
	int nSize = sizeof(g_pszReservedID)/sizeof(char*);
	CString szCheck = szName;
	CString szCheck2;

	szCheck.MakeLower();

	for (int i=0; i< nSize; i++) 
	{
		szCheck2 = g_pszReservedID[i];
		szCheck2.MakeLower();

		if(szCheck.Find(szCheck2) != -1) return TRUE;

	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
//	������ǥ �������� ������ǥ�� �ֺ� 25��ǥ �߿� ������ �� �ִ� ���� �ִ��� �Ǵ�.
//
CPoint USER::FindNearAvailablePoint_S(int x, int y)
{
	if(x <= -1 || y <= -1) return CPoint(-1,-1);
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return CPoint(-1,-1);
	if(x >= g_zone[m_ZoneIndex]->m_sizeMap.cx || y >= g_zone[m_ZoneIndex]->m_sizeMap.cy) return CPoint(-1,-1);

	if(IsMovable_S( x, y ))
	{
		return CPoint(x,y);
	}

	CPoint t;
	int tempX = x, tempY = y;
	int i;

	int dir[25][2];

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

	for( i = 0; i < 25; i++)
	{
		if( IsMovable_S( tempX + dir[i][0], tempY + dir[i][1] ) )
		{
			return CPoint( tempX + dir[i][0], tempY + dir[i][1] );
		}
	}

	return CPoint(-1,-1);
}

CPoint USER::FindNearAvailablePoint_S(int zone,int x, int y)
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
	if(x <= -1 || y <= -1) return CPoint(-1,-1);
	if( ZoneIndex < 0 || ZoneIndex >= g_zone.GetSize() ) return CPoint(-1,-1);
	if(x >= g_zone[ZoneIndex]->m_sizeMap.cx || y >= g_zone[ZoneIndex]->m_sizeMap.cy) return CPoint(-1,-1);

	if(IsMovable_S(zone, x, y ))
	{
		return CPoint(x,y);
	}

	CPoint t;
	int tempX = x, tempY = y;
	int i;

	int dir[25][2];

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

	for( i = 0; i < 25; i++)
	{
		if( IsMovable_S(zone, tempX + dir[i][0], tempY + dir[i][1] ) )
		{
			return CPoint( tempX + dir[i][0], tempY + dir[i][1] );
		}
	}

	return CPoint(-1,-1);
}

//////////////////////////////////////////////////////////////////////////////////
//	Ŭ���̾�Ʈ��ǥ �������� ������ǥ�� �ֺ� 25��ǥ �߿� ������ �� �ִ� ���� �ִ��� �Ǵ�.
//
CPoint USER::FindNearAvailablePoint_C(int x, int y)
{
	if( IsMovable_C( x, y ) )
	{
		return CPoint(x,y);
	}

	CPoint t = ConvertToServer( x, y );

	if( t.x == -1 || t.y == -1 )
	{
		return CPoint(-1,-1);
	}

	int tempX = t.x, tempY = t.y;
	int i;

	int dir[25][2];

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

	for( i = 0; i < 25; i++)
	{
		if( IsMovable_S( tempX + dir[i][0], tempY + dir[i][1] ) )
		{
			return ConvertToClient( tempX + dir[i][0], tempY + dir[i][1] );			
		}
	}

	return CPoint(-1,-1);
}

///////////////////////////////////////////////////////////////////////////////////////
//	Ŭ���̾�Ʈ ��ǥ�� �������� x, y �� �̵��� �� �ִ� ������ �Ǵ�
//
BOOL USER::IsMovable_C(int x, int y)
{
	CPoint t = ConvertToServer( x, y );

	if( t.x == -1 || t.y == -1 ) return FALSE;

	return IsMovable_S(t.x, t.y);
}

///////////////////////////////////////////////////////////////////////////////////////
//	������ǥ�� Ŭ���̾�Ʈ ��ǥ�� �ٲ۴�.
//
CPoint USER::ConvertToClient(int x, int y)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return CPoint(-1,-1);
	if(!g_zone[m_ZoneIndex]) return CPoint(-1, -1);

	int tempx, tempy;
	int temph = g_zone[m_ZoneIndex]->m_vMoveCell.m_vDim.cy / 2 - 1;

	if(x <= -1 || y <= -1) return CPoint(-1,-1); 
	if( y >= g_zone[m_ZoneIndex]->m_sizeMap.cy || x >= g_zone[m_ZoneIndex]->m_sizeMap.cx ) return CPoint(-1,-1);

	tempx = x - temph + y;
	tempy = y - x + temph;

	return CPoint( tempx, tempy );
}

///////////////////////////////////////////////////////////////////////////////////////
//	���������� ���ۿ� �����Ѵ�.
//
void USER::FillUserInfo(char *pBuf, int &index, BYTE flag)
{
	int nLen = 0, i;

	if( m_state != STATE_GAMESTARTED && flag == INFO_MODIFY ) return;

	SetByte(pBuf, USER_INFO, index);
	SetByte(pBuf, flag, index);
	SetInt(pBuf, m_uid + USER_BAND, index);

	if(flag == INFO_MODIFY || flag == INFO_TOWNPOTAL)
	{
		SetVarString(pBuf, m_strUserID, _tcslen(m_strUserID), index);

		CPoint t = ConvertToClient( m_curx, m_cury );
		if( t.x == -1 || t.y == -1 ) { t.x = 1; t.y = 1; }
		
		SetShort(pBuf, t.x, index);
		SetShort(pBuf, t.y, index);

		SetDWORD(pBuf, m_iSkin, index);
		SetDWORD(pBuf, m_iHair, index);
		SetByte(pBuf, (BYTE)m_sGender, index);
		SetString(pBuf, m_strFace, 10, index);

		for(i = 0; i < EQUIP_ITEM_NUM; i++) SetShort(pBuf, m_UserItem[i].sSid, index);

		SetShort(pBuf, m_sHP, index);
		SetShort(pBuf, m_sMagicMaxHP, index);

		SetByte(pBuf, m_tDir, index);

		SetDWORD(pBuf, m_dwAbnormalInfo, index);		// �����̻� ����
		SetDWORD(pBuf, m_dwAbnormalInfo_, index);
		SetDWORD( pBuf, 0, index);
		SetDWORD( pBuf, 0, index);

		SetShort(pBuf, m_sCityRank, index);

		SetInt( pBuf, m_dwGuild, index );

		nLen = strlen(m_strGuildName);
		if(nLen <= 0) nLen = 1;

		SetByte( pBuf, (BYTE)nLen, index );
		SetString( pBuf, m_strGuildName, nLen, index );		// Add Guild Name
		SetShort( pBuf, m_sGuildVersion, index );
		
		SetByte(pBuf, m_byClass, index);
		SetByte(pBuf, m_bPkStatus, index);
		//-- yskang 0.1 �߰��Ǵ� ��Ŷ.... ȣĪ[��Ī]
	
		nLen = strlen(m_strLoveName);
		if(nLen < 1) nLen =1;
		SetByte(pBuf, (BYTE)nLen, index);//��������
		SetString(pBuf,m_strLoveName,nLen,index);//������ �ֱ�

		for( i = TOTAL_INVEN_MAX; i < TOTAL_ITEM_NUM-2; i++) SetShort(pBuf, m_UserItem[i].sSid, index);	// EBody

		if(m_UserItem[TOTAL_ITEM_NUM-2].sSid!=-1&&m_UserItem[TOTAL_ITEM_NUM-2].sDuration!=0){
			SetByte(pBuf,(BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]),index);
			SetByte( pBuf, 0x00, index );
		}else{
			SetByte(pBuf,(BYTE)(m_UserItem[TOTAL_ITEM_NUM-2].tMagic[0]),index);
			SetByte( pBuf, 0xff, index );
		}
		SetByte(pBuf, strlen(m_PersonalShopName), index);
		SetString(pBuf, m_PersonalShopName, strlen(m_PersonalShopName), index);
	}
}
/*
void USER::SendRangeInfoToMe(int min_x, int min_y, int max_x, int max_y, BYTE flag)
{
	int i, j, tuid;
	int throwindex;

	CPoint t;

	USER *pUser = NULL;
	CNpc *pNpc = NULL;
	ItemList* pThrowItem = NULL;
	MAP* pMap = g_zone[m_ZoneIndex];
	if(!pMap) return; 

	for( i = min_y; i < max_y; i++)
	{
		if( i >= pMap->m_sizeMap.cy || i < 0 ) continue;

		for( j = min_x; j < max_x; j++)
		{
			if( j >= pMap->m_sizeMap.cx || j < 0 ) continue;

			tuid = GetUid( j, i );

			if( tuid != 0 )
			{
				if( tuid != m_uid + USER_BAND )
				{
					if( tuid >= USER_BAND && tuid < NPC_BAND )
					{
						pUser = GetUser( tuid - USER_BAND );

						if(pUser->m_state != STATE_GAMESTARTED) continue;

						if( j != pUser->m_curx || i != pUser->m_cury )
						{
							SetUid(j, i, 0);
						}
						else
						{
							SendUserInfo( pUser, flag );
						}
					}
					else if( tuid >= NPC_BAND && tuid < INVALID_BAND)
					{
						pNpc = GetNpc( tuid - NPC_BAND );
						
						if( pNpc )
						{
							if( j != pNpc->m_sCurX || i != pNpc->m_sCurY )
							{
								SetUid( j, i, 0 );
							}
							else
							{
								SendNpcInfo( pNpc, flag );
							}
						}
					}
				}
			}

			throwindex = pMap->m_pMap[j][i].iIndex;

			if( throwindex != -1 && throwindex < MAX_THROW_ITEM )
			{
				pThrowItem = m_pCom->m_ThrowItemArray[throwindex]->m_pItem;

				if( pThrowItem )
				{
					t = ConvertToClient( j, i );

					if( t.x != -1 && t.y != -1 )
					{
						AddItemFieldInfoToMe( pThrowItem, flag, t.x, t.y );
					}
				}
			}
		}
	}
}
*/
void USER::SendRangeInfoToMe(int min_x, int min_y, int max_x, int max_y, BYTE flag)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return;

	int i, j, tuid;
	int throwindex;

	CPoint t;

	USER *pUser = NULL;
	CNpc *pNpc = NULL;
	ItemList* pThrowItem = NULL;
	MAP* pMap = g_zone[m_ZoneIndex];
	if(!pMap) return;

	int index = 0;
	int index_comp = 0;
	int count = 0;

	for( i = min_y; i < max_y; i++)
	{
		if( i >= pMap->m_sizeMap.cy || i < 0 )
		{
			continue;
		}

		for( j = min_x; j < max_x; j++)
		{
			if( j >= pMap->m_sizeMap.cx || j < 0 )
			{
				continue;
			}

			tuid = pMap->m_pMap[j][i].m_lUser;
//			tuid = GetUid( j, i );

			if( tuid != 0 )
			{
				if( tuid != m_uid + USER_BAND )
				{
					if( tuid >= USER_BAND && tuid < NPC_BAND )
					{
						pUser = GetUser( tuid - USER_BAND );
						if ( pUser == NULL ) continue;

						if(pUser->m_state != STATE_GAMESTARTED)
						{
							continue;
						}

						if( j != pUser->m_curx || i != pUser->m_cury )
						{
							SetUid(j, i, 0);
						}
						else
						{
//							SendUserInfo( pUser, flag );
							AddRangeInfoToMe( pUser, flag );
						}
					}
					else if( tuid >= NPC_BAND && tuid < INVALID_BAND)
					{
						pNpc = GetNpc( tuid - NPC_BAND );
						
						if( pNpc )
						{
							if( j != pNpc->m_sCurX || i != pNpc->m_sCurY )
							{
								if(pNpc->m_sDimension > 0) continue;

								SetUid( j, i, 0 );
							}
							else
							{
//								SendNpcInfo( pNpc, flag );
								AddRangeInfoToMe( pNpc, flag );
							}
						}
					}
				}
			}

			throwindex = pMap->m_pMap[j][i].iIndex;

			if( throwindex >= 0 && throwindex < MAX_THROW_ITEM )
			{
				// IKING 2001.1.
				if ( m_pCom->m_ThrowItemArray[throwindex] == NULL )
				{
					return;
				}
				//
				// IKING 2001.1.
				EnterCriticalSection( &m_pCom->m_critThrowItem );

				pThrowItem = m_pCom->m_ThrowItemArray[throwindex]->m_pItem;

				if( pThrowItem )
				{
					t = ConvertToClient( j, i );

					if( t.x != -1 && t.y != -1 )
					{
						AddItemFieldInfoToMe( pThrowItem, flag, t.x, t.y );
					}
				}

				LeaveCriticalSection( &m_pCom->m_critThrowItem );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//	���� ������ �������� �����͸� ������.
//
void USER::SendToRange(char *pBuf, int index, int min_x, int min_y, int max_x, int max_y)
{
	if( index <= 0 || index >= SEND_BUF_SIZE ) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;

	if( !pNewData ) return;

	pNewData->flag = SEND_RANGE;
	pNewData->len = index;

	::CopyMemory( pNewData->pBuf, pBuf, index );

	pNewData->uid = 0;
	pNewData->z = m_curz;
	pNewData->rect.left		= min_x;
	pNewData->rect.right	= max_x;
	pNewData->rect.top		= min_y;
	pNewData->rect.bottom	= max_y;
	pNewData->zone_index = m_ZoneIndex;

	// IKING 2001.1.
	//EnterCriticalSection( &(m_pCom->m_critSendData) );
	//m_pCom->m_arSendData.Add( pNewData );
	//LeaveCriticalSection( &(m_pCom->m_critSendData) );
	//PostQueuedCompletionStatus( m_pCom->m_hSendIOCP, 0, 0, NULL );
	m_pCom->Send(pNewData);
	delete pNewData;
	//
}

////////////////////////////////////////////////////////////////////////////////////
//	Client ��ǥ�� ������ǥ�� ��ȯ�Ѵ�
//
CPoint USER::ConvertToServer(int x, int y)
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return CPoint(-1,-1);

	int tempx, tempy;
	int temph = g_zone[m_ZoneIndex]->m_vMoveCell.m_vDim.cy / 2 - 1;

	if(x <= -1 || y <= -1) return CPoint(-1,-1); 
	
	if( y >= g_zone[m_ZoneIndex]->m_vMoveCell.m_vDim.cy || x >= g_zone[m_ZoneIndex]->m_vMoveCell.m_vDim.cx ) return CPoint(-1,-1);

	if( (x+y)%2 == 0 )
	{
		tempx = temph - ( y / 2 ) + ( x / 2 );

		if( x % 2 ) tempy = ( y / 2 ) + ( ( x / 2 ) + 1 );
		else        tempy = ( y / 2 ) + ( x / 2 );

		return CPoint( tempx, tempy );
	}
	else return CPoint(-1,-1);
}

CPoint USER::ConvertToServerByZone(int z, int x, int y)
{
	int i;
	int cx = -1, cy = -1;

	for( i = 0; i < g_zonesize.GetSize(); i++ )
	{
		if( g_zonesize[i] )
		{
			if( g_zonesize[i]->m_Zone == z )
			{
				cx = g_zonesize[i]->m_vMoveCell.m_vDim.cx;
				cy = g_zonesize[i]->m_vMoveCell.m_vDim.cy;

				break;
			}
		}
	}

	if( cx < 0 || cy < 0 ) return CPoint( -1, -1 );

	int tempx, tempy;
	int temph = cy / 2 - 1;

	if(x <= -1 || y <= -1) return CPoint(-1,-1); 
	if( y >= cy || x >= cx ) return CPoint(-1,-1);

	if( (x+y)%2 == 0 )
	{
		tempx = temph - ( y / 2 ) + ( x / 2 );

		if( x % 2 ) tempy = ( y / 2 ) + ( ( x / 2 ) + 1 );
		else        tempy = ( y / 2 ) + ( x / 2 );

		return CPoint( tempx, tempy );
	}
	else return CPoint(-1,-1);
}

////////////////////////////////////////////////////////////////////////////////////
//	���� �����ȿ� ���� ���� �� �������� ���ۿ� �����Ѵ�.  
//
void USER::AddItemFieldInfoToMe(ItemList *pItem, BYTE type, int x, int y)
{
/*
	if( !pItem && type == ITEM_INFO_MODIFY)
	{
		return;
	}

	if( m_ItemFieldInfoIndex >= 8000 )
	{
		SendItemFieldInfoToMe();
	}

	SetByte( m_ItemFieldInfoBuf, type, m_ItemFieldInfoIndex );

	SetShort( m_ItemFieldInfoBuf, x, m_ItemFieldInfoIndex );
	SetShort( m_ItemFieldInfoBuf, y, m_ItemFieldInfoIndex );

//	if(type == ITEM_INFO_MODIFY)
//	{
		if(pItem->tType == TYPE_ITEM)
		{
			SetShort( m_ItemFieldInfoBuf, pItem->sSid, m_ItemFieldInfoIndex );
			SetDWORD( m_ItemFieldInfoBuf, pItem->sCount, m_ItemFieldInfoIndex );
			SetByte( m_ItemFieldInfoBuf, pItem->tIQ, m_ItemFieldInfoIndex ); 
		}
		else
		{
			SetShort( m_ItemFieldInfoBuf, TYPE_MONEY_SID, m_ItemFieldInfoIndex );
			SetDWORD( m_ItemFieldInfoBuf, pItem->dwMoney, m_ItemFieldInfoIndex );
			SetByte( m_ItemFieldInfoBuf, 0, m_ItemFieldInfoIndex ); 
		}
//	}

	m_ItemFieldInfoCount++;
*/

	int index = 0;
	TCHAR pData[1024];

	SetByte( pData, FIELD_ITEM_INFO, index );
	SetShort( pData, (short)1, index );
	SetByte( pData, type, index );

	SetShort( pData, x, index );
	SetShort( pData, y, index );

	if(pItem->tType == TYPE_ITEM)
	{
		SetShort( pData, pItem->sSid, index );
		SetDWORD( pData, pItem->sCount, index );
		SetByte( pData, pItem->tIQ, index ); 
	}
	else
	{
		SetShort( pData, TYPE_MONEY_SID, index );
		SetDWORD( pData, pItem->dwMoney, index );
		SetByte( pData, 0, index ); 
	}

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

////////////////////////////////////////////////////////////////////////////////////
//	���ۿ� �ִ� ������ ������ ������.  
//
void USER::SendItemFieldInfoToMe()
{
	int index = 0;

	CBufferEx TempBuf;

	if( m_ItemFieldInfoCount <= 0 )
	{
		return;
	}

	if( m_ItemFieldInfoIndex <= 0 )
	{
		return;
	}

	TempBuf.Add(FIELD_ITEM_INFO);
	TempBuf.Add((short)m_ItemFieldInfoCount);
	TempBuf.AddData(m_ItemFieldInfoBuf, m_ItemFieldInfoIndex);

	Send(TempBuf, TempBuf.GetLength());

	m_ItemFieldInfoCount = 0;
	memset( m_ItemFieldInfoBuf, NULL, 8192 );
	m_ItemFieldInfoIndex = 0;
}

////////////////////////////////////////////////////////////////////////////////////
//	�ִ� ������� ���Ѵ�.
//
int USER::GetMaxDamage(BYTE tWeaponClass)
{
	int nFinalDamage = 0;	// ����������
	int xyz = 0;			// ���ⵥ����	

	int tClass = tWeaponClass * SKILL_NUM;

	double iBasic = 0;			// �⺻������

	int iSkillSid = 0;

	if(tWeaponClass == FIREARMS) 
		iBasic = (int)((double)(m_sMagicDEX + 10  ) / 2 + 0.5 + (int)((double)m_DynamicEBodyData[EBODY_DEX_TO_DAMAGE] / 100 * m_sMagicDEX));
	else 
		iBasic = (int)((double)(m_sMagicSTR)/2 + 0.5+(int)((double)m_DynamicEBodyData[EBODY_STR_TO_DAMAGE] / 100 * m_sMagicSTR) );	


	if(iBasic < 0) iBasic = 0;

	if(tWeaponClass == 255) return (int)iBasic;

	xyz = GetWeaponDamage(m_UserItem[RIGHT_HAND].sSid, TRUE);

	nFinalDamage = (int)(iBasic + xyz);

	if(m_sLevel >= ADD_USER_LEVEL) nFinalDamage += g_arUserLevel[m_sLevel - ADD_USER_LEVEL]->m_sDamage;
	
	return nFinalDamage;
}

////////////////////////////////////////////////////////////////////////////////////
//	�� ���ݿ� ���� ������ �ݰ� ������� ����Ѵ�.
//
void USER::SetCounterAttack(int uid, int damage)
{
	for(int i = 0; i < COUNTERATTACK_MAX_NUM; i++)
	{
		if(InterlockedCompareExchange((LONG*)&m_CounterAttackList[i].lSid, (long)1, (long)0) == (long)0)
		{
			m_CounterAttackList[i].iNpcUid = uid;
			m_CounterAttackList[i].iDamage = damage;
			m_CounterAttackList[i].dwLastAttackTime = GetTickCount();
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//	�ȱ� ���۽��� SPEED HACK ��������� üũ�Ѵ�.
//
BOOL USER::CheckMoveSpeedHack(DWORD checkTick)
{
	DWORD curTick = GetTickCount();
    DWORD tmpS = 0;
	if( m_iTickCount == 0 )
	{
		m_dLastCheckTick = checkTick;
		m_dCheckTick = curTick;
		m_dTotCheckTick = 0;
		
		m_iTickCount += 1;
	}
	else if( m_iTickCount == 5 )
	{
		DWORD client = abs( checkTick - m_dLastCheckTick );
		DWORD server = abs( curTick - m_dCheckTick );
		double gap = (double)client / (double)server;

		m_dTotCheckTick -= m_dTotCheckTick / 20;
		if( ( gap < 0.75 || gap > 1.25 ) && ( server < m_dTotCheckTick ) )
		{
			SYSTEMTIME time;
			GetLocalTime(&time);

			CString str;
			str.Format( IDS_USER_SYSTEM_MSG01,
				time.wMonth, time.wDay, time.wHour, time.wMinute, m_strUserID, server, client, m_dTotCheckTick );
			g_fpSpeedHack.Write( str, str.GetLength() );
			tmpS = server * 2;
			if ( m_dTotCheckTick >= tmpS) 
			{
            USER* pUser = GetUser(m_strUserID);
            if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return TRUE;
			pUser->SoftClose();
			m_iTickCount = 0;
			return TRUE;
			}
			else
			  m_iTickCount += 1;
		}
		m_iTickCount = 0;
	}
	else
	{
		m_dTotCheckTick += CLIENT_WALK_TICK;
		m_iTickCount += 1;
	}
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
//	�۶� SPEED HACK ��������� üũ�Ѵ�.
//
BOOL USER::CheckRunSpeedHack(DWORD checkTick, BOOL bTwo)
{
	DWORD curTick = GetTickCount();
    DWORD tmpS = 0;
	if( m_iTickCount == 0 )
	{
		m_dLastCheckTick = checkTick;
		m_dCheckTick = curTick;
		m_dTotCheckTick = 0;
		
		m_iTickCount += 1;
	}
	else if( m_iTickCount == 5 )
	{
		DWORD client = abs( checkTick - m_dLastCheckTick );
		DWORD server = abs( curTick - m_dCheckTick );
		double gap = (double)client / (double)server;

		m_dTotCheckTick -= m_dTotCheckTick / 20;
		if( ( gap < 0.75 || gap > 1.25 ) && ( server < m_dTotCheckTick ) )
		{
			SYSTEMTIME time;
			GetLocalTime(&time);

			CString str;
			str.Format( IDS_USER_SYSTEM_MSG01,
				time.wMonth, time.wDay, time.wHour, time.wMinute, m_strUserID, server, client, m_dTotCheckTick );
			g_fpSpeedHack.Write( str, str.GetLength() );
			tmpS=server*2;
            if (m_dTotCheckTick >= tmpS )  
			{
            USER* pUser = GetUser(m_strUserID);
            if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return TRUE;
			pUser->SoftClose();
			m_iTickCount = 0;
			return TRUE;
			}
			else
            	m_iTickCount += 1;
		}
		m_iTickCount = 0;
	}
	else
	{
		m_dTotCheckTick += CLIENT_RUN_TICK;
		if(bTwo) m_dTotCheckTick += CLIENT_RUN_TICK;
		m_iTickCount += 1;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
//	���濡�� Ÿ�����ٶ� ����������θ� �Ǵ�.
//
void USER::IsLegalDefence(USER *pUser)
{
	CBufferEx TempBuf;

	int rank = CAIN_RANK - CITY_RANK_INTERVAL;			// -1 ���ʹ� �����������
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	if(pUser->m_sCityRank <= rank || pUser->m_bPkStatus) return;

	if(CheckInvalidZoneInGuildWar(pUser)) return;
	if(CheckInvalidZoneInFreeFight(pUser)) return;

	m_bPkStatus = TRUE;				// ���� ī�����´�.
	m_dwPkStatusTime = GetTickCount();
	
	TempBuf.Add(SET_USER_PK_STATE);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((BYTE)0x02);
	TempBuf.Add((BYTE)m_bPkStatus);

	SendInsight(TempBuf, TempBuf.GetLength());
//	SendExactScreen(TempBuf, TempBuf.GetLength());
}

//yskang 0.7 ������� ��� ���� ���̸� ī����
BOOL USER::IsLegalDefence_isdead(USER *pUser)
{
	CBufferEx TempBuf;

	int rank = CAIN_RANK - CITY_RANK_INTERVAL;			// -1 ���ʹ� �����������
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return FALSE;
	if(pUser->m_sCityRank <= rank || pUser->m_bPkStatus) return FALSE;

	if(CheckInvalidZoneInGuildWar(pUser)) return TRUE;
	if(CheckInvalidZoneInFreeFight(pUser)) return TRUE;

	m_bPkStatus = TRUE;				// ���� ī�����´�.
	m_dwPkStatusTime = GetTickCount();
	
	TempBuf.Add(SET_USER_PK_STATE);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((BYTE)0x02);
	TempBuf.Add((BYTE)m_bPkStatus);

	SendInsight(TempBuf, TempBuf.GetLength());
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
//	���� ���� ��� ������ ����� ���̸� ���⿡ ������ ���� �ʵ����Ѵ� 
//
BOOL USER::CheckInvalidZoneInGuildWar(USER *pUser)
{	
	int me, you, type;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return FALSE;

	type = CheckInvalidMapType();

	int mapindex = GetGuildMapIndex(type);
	if(mapindex <= -1 || mapindex >= g_arMapTable.GetSize()) return FALSE;

	if(!CheckInvalidZoneState(type)) // ������ �Ǵ� �ʵ� �������϶��� �������� ������...
	{
		me = g_arMapTable[mapindex]->m_sStoreID;

		type = pUser->CheckInvalidMapType();
		mapindex = GetGuildMapIndex(type);
		if(mapindex <= -1 || mapindex >= g_arMapTable.GetSize()) return FALSE;

		you = g_arMapTable[mapindex]->m_sStoreID;

		if(me == you) return TRUE;				// ���� ���� �ε����� ��� �����̰� �ƴ�...
	}

	if(m_tGuildWar == GUILD_WARRING) //|| m_tFortressWar == GUILD_WARRING)					// ���� ����� ���̰�
	{
		if(m_dwFieldWar > 0) 
		{												// �ʵ������̸� �������		
			if(pUser->m_dwGuild == m_dwFieldWar) 
			{
				CString strMsg = _T("");				// ��� �渶�� ������ ���� ��..
				if(pUser->m_bLive == USER_DEAD && pUser->m_bGuildMaster)
				{
					strMsg.Format( IDS_USER_GUILD_WAR_WINNER, m_strGuildName);
					SendGuildWarFieldEnd((LPTSTR)(LPCTSTR)strMsg);
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
//	���� �ִ� ���� ������̸� ���⿡ �������
//	TRUE - ���� �ִ� ���� ����� �ƴ�, FALSE - ������̰ų� ����
BOOL USER::CheckInvalidZoneInFreeFight(USER *pUser)
{
	int me, you;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return FALSE;

	me = CheckInvalidMapType();
	you = CheckInvalidMapType();

	if( me == 12 && you == me ) return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
//	�ùε���� �����Ѵ�.
//
void USER::IsChangeCityRank(int iMyCityRank, USER *pUser)
{
	// PK�� �׾�����ó��
	CBufferEx TempBuf;

	int step = 0;
	int rank = 0;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	if(pUser->m_bLive != USER_DEAD || pUser->m_sHP > 0) return;	// Ȥ ������ �ʾҴµ� ���� ����...
											// �ֿ켱������ ������ �˻�	
	if(CheckInvalidZoneInGuildWar(pUser)) return;
	if(CheckInvalidMapType() == 12 ) return;	// ����� �˻�

	// ������ �Ͻ��� ī�����¸� �׿��� �ȴ�?	// �ùε���� -1���ϸ� �׿��� �ȴ�?
	if(pUser->m_bPkStatus || pUser->m_sCityRank < 0)
	{
		pUser->SendCityRank(iMyCityRank);
		return;
	}

	pUser->SendCityRank(iMyCityRank);
	
	if(m_sCityRank >= 0)					 // �������� ���Ҷ�
	{
		m_iCityValue = 0;					// ������ 0���� ����� ��� ( ����Ʈ ������)

		if(pUser->m_sLevel > m_sLevel)  step = 0;		// ��� ��ġ�� ���� ����������... 
		else if(pUser->m_sLevel < m_sLevel) step = 2;
		else  step = 1;

		rank = 4;	

		m_iCityValue -= g_CityRank[rank][step];
	}
	else												// ���϶�
	{
		if(pUser->m_sLevel > m_sLevel)  step = 0;
		else if(pUser->m_sLevel < m_sLevel) step = 2;
		else  step = 1;

		rank = m_sCityRank + CITY_RANK_INTERVAL;	// ���ϴ� ������ �迭�� 0���� �����ϱ⶧��
		
		m_iCityValue = -1 * m_iCityValue;			// �̰� ���� ���ϱ����� �Ͻ������� ��(//&&&&&&&&&���߿� ��ġ�� ^^)
		CheckMaxValue((int &)m_iCityValue, (int &)g_CityRank[rank][step]);
		m_iCityValue = -1 * m_iCityValue;			// ��⼭ �ٲ�¡...
		if(m_iCityValue < -2000000000) m_iCityValue = -2000000000;  // �̷��� �������ϱ� ���ٵ� �ٲ���Ѵ�...^^
	}

	CheckMaxValue(m_sKillCount, 1);					// �������� ��� �����Ѵ�.
													// �־ǵ���϶�
	int oldRank = m_sCityRank;			
	for(int i = 0; i < 11; i++)
	{
		if(m_iCityValue < g_CityRankValue[i])// ����ġ���� ���� ���������� ������ �Ǵ��Ѵ�.
		{
			m_sCityRank = i - CITY_RANK_INTERVAL;
			break;
		}
	}

	if(oldRank != m_sCityRank) 
	{
		CheckGuildUserInFortress();			// �ù� ����� ���Ҷ� �ٽ� ����(��Ʈ����������)

		TempBuf.Add(SET_USER_PK_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add((BYTE)0x01);
		TempBuf.Add(m_sCityRank);

		SendInsight(TempBuf, TempBuf.GetLength());
//		SendExactScreen(TempBuf, TempBuf.GetLength());	
	}

	rank = m_sCityRank + CITY_RANK_INTERVAL;
	if( rank != SAINT_RANK )		// ���� ���ߵ� ����Ʈ�� �ƴ϶��
	{
		m_dwSaintTime = 0;
	}
}	

///////////////////////////////////////////////////////////////////////////////////////
//	���� ���� �׿����� �ùε�� ��� �����Ѵ�.
//
void USER::GetCityRank()
{
	CBufferEx TempBuf;

	int		i;
	BOOL	bRank = FALSE;
	int		iCityRank = m_sCityRank;

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

	if(iCityRank != m_sCityRank)
	{
		CheckGuildUserInFortress();			// �ù� ����� ���Ҷ� �ٽ� ����(��Ʈ����������)

		TempBuf.Add(SET_USER_PK_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add((BYTE)0x01);
		TempBuf.Add(m_sCityRank);

		SendInsight(TempBuf, TempBuf.GetLength());
//		SendExactScreen(TempBuf, TempBuf.GetLength());
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//	�� �ùε�޺��� ���ǰ��� ���� �����Ѵ�.
//
DWORD USER::GetItemCostByCityRank(int sid, int nRate)
{
	if(sid <0 || sid >= g_arItemTable.GetSize()) return 10000000; 
		
	int iCost = g_arItemTable[sid]->m_iDN;
	int rank = m_sCityRank + CITY_RANK_INTERVAL;

	iCost = iCost + (int)( iCost * ((double)nRate/100) );
	switch(rank)
	{
	case CHAONISE_RANK:
		iCost += (int)( iCost * ((double)25/100) );
		break;
	case DEATH_RANK:
		iCost += (int)( iCost * ((double)20/100) );
		break;
	case EVIL_RANK:
		iCost += (int)( iCost * ((double)15/100) );
		break;
	case VILLAIN_RANK:
		iCost += (int)( iCost * ((double)10/100) );
		break;
	case WARRIOR_RANK:
		iCost -= (int)( iCost * ((double)3/100) );
		break;
	case HERO_RANK:
		iCost -= (int)( iCost * ((double)6/100) );
		break;
	case SAINT_RANK:
		iCost -= (int)( iCost * ((double)10/100) );
		break;
	}

	if(iCost < 0) iCost = 1;

	return (DWORD)iCost;
}

///////////////////////////////////////////////////////////////////////////////////////
//	�����۰� �������� DB Update
//
BOOL USER::UpdateUserItemDN()
{
	if(UpdateMemUserAll()) return TRUE;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strItem[_ITEM_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strItem, sizeof(strItem));
	
	UserItemToStr(strItem);

	SDWORD sItemLen		= sizeof(strItem);
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_USER_ITEM_DATA (\'%s\',%d, ?)}"), m_strUserID, m_dwDN ); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode != SQL_SUCCESS)
		return FALSE;

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );

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

////////////////////////////////////////////////////////////////////////////////
//	ȡ�÷�ʦ�����˺�
//
int USER::GetPsiAttackUp()
{
	int iLevel = 0;
	int iSkillSid = 0;

	int iAttackUp = 0;
	int tClass = 0;

	BYTE tWeaponClass = 0;

	IsCanUseWeaponSkill(tWeaponClass);
	tClass = tWeaponClass * SKILL_NUM;

	for(int i = tClass; i < tClass + SKILL_NUM; i++)
	{
		iSkillSid  = m_UserSkill[i].sSid;
		if(iSkillSid == SKILL_PSI_SUCCESS_UP) // ������ ����		7 index
		{
			// ���� ��ų ���� 
			iLevel = m_UserSkill[i].tLevel;		
			if(iLevel < 0) iLevel = 0;
			
			// �����ۿ� ���� ��ų ���� ����
			if(iLevel >= 1) iLevel += m_DynamicUserData[g_DynamicSkillInfo[iSkillSid]]+ m_DynamicUserData[MAGIC_ALL_SKILL_UP];
			
			if(iLevel >= SKILL_LEVEL) iLevel = SKILL_LEVEL - 1;		// ��� �ִ� �������ٴ� �۾ƾ��Ѵ�
			if(iSkillSid >= g_arSkillTable.GetSize()) return 0;
			iAttackUp = g_arSkillTable[iSkillSid]->m_arSuccess.GetAt(iLevel);
		}
	}

	return iAttackUp;	
}

////////////////////////////////////////////////////////////////////////////////
//	���̿��� ���� ���ݿ� ���� �������� ����Ѵ�.
//
void USER::GetWideRangeAttack(int x, int y, int damage)	// ������ ���ݸ� ó��...
{
	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return;

	int dir[9][2];
	int ix, iy;
	int nTarget = 0;
	int nDamage = 0;

	USER* pUser = NULL;
	CNpc* pNpc = NULL;
	MAP* pMap = g_zone[m_ZoneIndex];
	if(!pMap) return;

	dir[0][0]  =  0;		dir[0][1] =  0;		// 
	dir[1][0]  = -1;		dir[1][1] =  0;		// 
	dir[2][0]  = -1;		dir[2][1] =  1;		// 
	dir[3][0]  =  0;		dir[3][1] =  1;		// 
	dir[4][0]  =  1;		dir[4][1] =  1;		// 

	dir[5][0]  =  1;		dir[5][1] =  0;		// 
	dir[6][0]  =  1;		dir[6][1] = -1;		// 
	dir[7][0]  =  0;		dir[7][1] = -1;		// 
	dir[8][0]  = -1;		dir[8][1] = -1;		// 

	for(int i = 1; i < 9; i++)
	{
		ix = x + dir[i][0];
		iy = y + dir[i][1];

		if(ix < 0) ix = 0;
		if(iy < 0) iy = 0;
		if(ix >= pMap->m_sizeMap.cx) ix = pMap->m_sizeMap.cx - 1;
		if(iy >= pMap->m_sizeMap.cy) iy = pMap->m_sizeMap.cy - 1;

		nTarget = pMap->m_pMap[ix][iy].m_lUser;

		if(nTarget >= USER_BAND && nTarget < NPC_BAND)	// USER
		{
			pUser = GetUser(nTarget - USER_BAND);			// User Pointer �� ��´�.
			if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;	// �߸��� USER �̸� ����
			if(pUser->m_bLive == USER_DEAD || pUser->m_uid == m_uid)	continue;		// Target User �� �̹� �׾������� ����
			if(!pUser->m_bPkStatus) continue;				// �Ͻ��� ī���� �ƴϸ� ������ �ȵȴ�.

			if(m_dwGuild > 0)
			{
				if(m_tGuildWar == GUILD_WARRING && pUser->m_tGuildWar == GUILD_WARRING)
				{												// ������϶� �Ͻ��� ī���� �����������Դ� ����
					if(pUser->m_dwGuild == m_dwGuild)  return;
				}

				if(m_tFortressWar == GUILD_WARRING && pUser->m_tFortressWar == GUILD_WARRING)
				{												// ������϶� �Ͻ��� ī���� �����������Դ� ����
					if(pUser->m_dwGuild == m_dwGuild)  return;
				}												// �ʵ����϶� ������ �ȵǾ� �ȴ�.(���� ��������� �ٸ��� ǥ���ؾ���)								
			}

			nDamage = (int)((double)damage *  ((double)m_sMagicVOL / (m_sMagicVOL + pUser->m_sMagicVOL + pUser->m_DynamicUserData[MAGIC_PSI_RESIST_UP] + \
				                               (int)((double)pUser->m_DynamicEBodyData[EBODY_PSI_RESIST_UP] / 100) \
											  )));
			nDamage = (int)((double)nDamage/2 + 0.5);	// �������� 50%�� ����.
			
			pUser->SetDamage(nDamage);
			if(pUser->m_sHP > 0)		// ���� ��� ���ⵥ���� �߰�
			{
//				pUser->SetColdDamage();
			}
			else if(pUser->m_lDeadUsed == 1)
			{
				int tempRank = m_sCityRank + CITY_RANK_INTERVAL;
				IsChangeCityRank(m_sCityRank, pUser);
				pUser->GetLevelDownExp(USER_PK, tempRank, FALSE,m_strUserID);			// ����ġ�� �׿� ��ȭ���� �ݿ��Ѵ�.
			}
		}
		else if(nTarget >= NPC_BAND)				// NPC
		{
			pNpc = GetNpc(nTarget - NPC_BAND);				// NPC Point �� ��´�.
			if(pNpc == NULL) continue;					// �߸��� NPC �̸� ����
			if(pNpc->m_NpcState == NPC_DEAD || pNpc->m_tNpcType != NPCTYPE_MONSTER) continue;	// NPC �� �̹� �׾� ������ ����
			if(pNpc->m_sHP <= 0) continue;
			if(pNpc->m_byAX == 0 && pNpc->m_byAZ == 0 && pNpc->m_tNpcType == 0) continue;			// ���ݴɷ��� ���� ����(���� ����)�� �Ϲ� ������ ���� �ʴ´�.

			nDamage = (int)(damage *  ((double)m_sMagicVOL / (m_sMagicVOL + pNpc->m_sVOL)));
			nDamage = (int)((double)nDamage/2 + 0.5);	// �������� 50%�� ����.

//			if(pNpc->SetDamage(nDamage, m_strUserID, m_uid + USER_BAND, m_pCom) == FALSE)
			if(pNpc->SetDamage(nDamage, m_uid + USER_BAND, m_pCom) == FALSE)
			{
				if(m_tGuildHouseWar == GUILD_WARRING && pNpc->m_NpcVirtualState == NPC_WAIT)
				{
					CheckGuildHouseWarEnd();
				}

				pNpc->SendExpToUserList(m_pCom); // ����ġ �й�!!
				pNpc->SendDead(m_pCom);
				int diffLevel = abs(m_sLevel - pNpc->m_byClassLevel);
				if(diffLevel < 40)
				{
					m_iCityValue=m_iCityValue+250;
					GetCityRank();
					CheckMaxValue(m_dwXP, 1);		// ���� �������� 1 ����!	
					SendXP();
				}
			}
			else									// ���� ��� ���ⵥ���� �߰�
			{
//				pNpc->SetColdDamage();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//	��ȯ�� ���� ����Ÿ���� �����Ѵ�.
//
void USER::GetNpcData(CNpc *pNpc, int x, int y)
{
	int  iCount = 0,i;
	CPoint pt;

	if(m_tIsOP != 1) return;

	CNpc* pEventNpc	= GetEventNpc();

	if(pEventNpc == NULL)
	{
		SendSystemMsg( IDS_USER_CALL_MONSTER_COUNT_MAX, SYSTEM_NORMAL, TO_ME);
		return;
	}

	MYSHORT sAI;
	BYTE upTemp = 0;			// ���� 8��Ʈ
	BYTE dwTemp = 0;			// ���� 8��Ʈ

	CString strMsg = _T(""); 

	pEventNpc->m_sSid			= pNpc->m_sSid;		// MONSTER(NPC) Serial ID

	pEventNpc->m_sPid			= pNpc->m_sPid;		// MONSTER(NPC) Picture ID
	_tcscpy(pEventNpc->m_strName, pNpc->m_strName);		// MONSTER(NPC) Name
	
	pEventNpc->m_sSTR			= pNpc->m_sSTR;		// ��
	pEventNpc->m_sDEX			= pNpc->m_sDEX;		// ��ø
	pEventNpc->m_sVOL			= pNpc->m_sVOL;		// ����
	pEventNpc->m_sWIS			= pNpc->m_sWIS;		// ����
	
	pEventNpc->m_sHP			= pNpc->m_sMaxHP;		// �ִ� HP
	pEventNpc->m_sMaxHP			= pNpc->m_sMaxHP;		// ���� HP
	pEventNpc->m_sPP			= pNpc->m_sMaxPP;		// �ִ� PP
	pEventNpc->m_sMaxPP			= pNpc->m_sMaxPP;		// ���� PP
	
	pEventNpc->m_byClass		= pNpc->m_byClass;		// ����迭
	pEventNpc->m_byClassLevel	= pNpc->m_byClassLevel;// ����迭 ����
	pEventNpc->m_sExp			= pNpc->m_sExp;		// ����ġ
	
	pEventNpc->m_byAX			= pNpc->m_byAX;		// ���ݰ� X
	pEventNpc->m_byAY			= pNpc->m_byAY;		// ���ݰ� Y
	pEventNpc->m_byAZ			= pNpc->m_byAZ;		// ���ݰ� Z

	pEventNpc->m_iDefense		= pNpc->m_iDefense;	// ��
	pEventNpc->m_byRange		= pNpc->m_byRange;		// �����Ÿ�
	pEventNpc->m_sAI			= pNpc->m_sAI;		// �ΰ����� �ε���
	pEventNpc->m_sAttackDelay	= pNpc->m_sAttackDelay;// ���ݵ�����
	pEventNpc->m_byVitalC		= pNpc->m_byVitalC;	// ��ü������ ũ��Ƽ��
	pEventNpc->m_byWildShot		= pNpc->m_byWildShot;	// ���� ����
	pEventNpc->m_byExcitedRate	= pNpc->m_byExcitedRate;			// ��� ����
	pEventNpc->m_byIronSkin		= pNpc->m_byIronSkin;
	pEventNpc->m_byReAttack		= pNpc->m_byReAttack;
	pEventNpc->m_bySubAttack	= pNpc->m_bySubAttack;	// �����̻� �߻�(�ΰ�����)
	pEventNpc->m_byState		= pNpc->m_byState;		// ���� (NPC) �����̻�
	pEventNpc->m_byPsi			= pNpc->m_byPsi;		// ���̿��� ����
	pEventNpc->m_byPsiLevel		= pNpc->m_byPsiLevel;	// ���̿��з���

	pEventNpc->m_bySearchRange	= pNpc->m_bySearchRange;			// �� Ž�� ����
	pEventNpc->m_sSpeed			= pNpc->m_sSpeed;		// �̵��ӵ�	
	
	pEventNpc->m_sInclination	= pNpc->m_sInclination;
	pEventNpc->m_byColor		= pNpc->m_byColor;
	pEventNpc->m_sStandTime		= pNpc->m_sStandTime;

	//////// MONSTER POS ////////////////////////////////////////
	pEventNpc->m_sCurZ			= pEventNpc->m_sOrgZ = m_curz;
	pEventNpc->m_sCurX			= pEventNpc->m_sOrgX = m_curx;
	pEventNpc->m_sCurY			= pEventNpc->m_sOrgY = m_cury;
	
	pEventNpc->m_sMinX			= pNpc->m_sMinX;
	pEventNpc->m_sMinY			= pNpc->m_sMinY;
	pEventNpc->m_sMaxX			= pNpc->m_sMaxX;
	pEventNpc->m_sMaxY			= pNpc->m_sMaxY;
	
	pEventNpc->m_sRegenTime		= pNpc->m_sRegenTime;

	pEventNpc->m_sEvent			= pNpc->m_sEvent;		// �̺�Ʈ ��ȣ
	pEventNpc->m_sEZone			= pNpc->m_sEZone;
	pEventNpc->m_byType			= pNpc->m_byType;
	pEventNpc->m_sDimension		= pNpc->m_sDimension;	// ���� ��� �����ϴ��� �Ǵ�

	pEventNpc->m_tNpcType		= pNpc->m_tNpcType;	// NPC Type

	pEventNpc->m_sFamilyType	= pNpc->m_sFamilyType;	// NPC Type
	pEventNpc->m_tItemPer		= pNpc->m_tItemPer;	// NPC Type
	pEventNpc->m_tDnPer			= pNpc->m_tDnPer;	// NPC Type
	pEventNpc->m_sHaveItem		= pNpc->m_sHaveItem;

	pEventNpc->m_ZoneIndex		= -1;

	pEventNpc->m_sClientSpeed	= pNpc->m_sClientSpeed;
	pEventNpc->m_dwStepDelay	= GetTickCount();

	sAI.i = (short)pNpc->m_sAI;						// NPC AI�� ����
	upTemp = sAI.b[0];
	dwTemp = sAI.b[1];

	pEventNpc->m_tNpcAttType = upTemp >> 7;			// ���߿� �߰��ؾ��Ѵ�.
	upTemp = upTemp << 1;
	pEventNpc->m_tNpcLongType = upTemp >> 7;
	upTemp = upTemp << 1;
	pEventNpc->m_tNpcGroupType = upTemp >> 7;

	if(pEventNpc->m_sClientSpeed <= 20) pEventNpc->m_sClientSpeed = 20;	// ��� �ڵ�;

	for(i = 0; i < g_zone.GetSize(); i++)
	{
		if(g_zone[i]->m_Zone == pEventNpc->m_sCurZ) 
		{
			pEventNpc->m_ZoneIndex = i;
			break;
		}
	}

	BOOL bSuccess = FALSE;
	pEventNpc->EventNpcInit(x, y);
	
	for(i = 0; i < NPC_NUM; i++ ) 
	{
		if(InterlockedCompareExchange((LONG*)&g_arEventNpcThread[0]->m_ThreadInfo.m_lNpcUsed[i], (long)1, (long)0) == (long)0)
		{
			bSuccess = TRUE;
			g_arEventNpcThread[0]->m_ThreadInfo.pNpc[i] = pEventNpc;
			break;
		}
	}

	if(!bSuccess)
	{
		InterlockedExchange(&pEventNpc->m_lEventNpc, (LONG)0);
		SendSystemMsg( IDS_USER_CALL_FAIL, SYSTEM_NORMAL, TO_ME);
		return;
	}

	strMsg.Format( IDS_USER_CALL_WHAT, pNpc->m_strName);
	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
}

////////////////////////////////////////////////////////////////////////////////
//	�߰��� ��ȯ���� �޸𸮸� �����ϱ����� �÷��װ� 0�� ���°͸� �ѱ��.
//
CNpc* USER::GetEventNpc()
{
	for(int i = g_TotalNPC; i < g_arNpc.GetSize(); i++)
	{
		if( g_arNpc[i] )
		{
			if(InterlockedCompareExchange((LONG*)&g_arNpc[i]->m_lEventNpc, (long)1, (long)0) == (long)0)
			{
				return g_arNpc[i];				
			}
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//	�ѱ�迭�� ���� ����OFF
//
void USER::SendHideOff()
{
/*	CBufferEx TempBuf;

	m_tPsiAbnormal = 0;
	m_dwHideTime = 0;
	m_dwLastHideTime = GetTickCount();
		
	TempBuf.Add(SET_USER_STATE);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add(ABNORAML_PSI);
	TempBuf.Add(ABNORMAL_NONE);
	TempBuf.Add((BYTE)0);
	TempBuf.Add((BYTE)0);
	TempBuf.Add((BYTE)0);
	TempBuf.Add((BYTE)0);	
	Send(TempBuf, TempBuf.GetLength());
	SendInsight(TempBuf, TempBuf.GetLength());
*/
}

////////////////////////////////////////////////////////////////////////////////
//	������ ������ �������� ���� ��ȭ�� �˸���.
//
void USER::SendItemWeightChange()
{
/*	CBufferEx TempBuf;

	TempBuf.Add(ITEM_WEIGHT_CHANGE);
	TempBuf.Add((short)m_iMaxWeight);
	TempBuf.Add((short)m_iCurWeight);

	Send(TempBuf, TempBuf.GetLength());
*/
}

////////////////////////////////////////////////////////////////////////////////
//	�ش� ����ε��� �� �����͸� ��ȯ
//
CGuild* USER::GetGuild(int num)
{
	EnterCriticalSection( &(m_pCom->m_critGuild) );

	if( num <= 0 ) return NULL;			// ��� �ε����� 1���� �����Ѵ�.
	if( num >= g_arGuildData.GetSize() ) return NULL; 

	CGuild* pGuild = NULL;

	if(g_arGuildData[num])
	{
		pGuild = g_arGuildData[num];
		if( pGuild->m_lSid == num )
		{
			return pGuild;
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//	�ش� ����̸����� �����͹�ȯ
//
CGuild* USER::GetGuildByName(TCHAR *guildname)
{
	EnterCriticalSection( &(m_pCom->m_critGuild) );

	int i, j = 0;
	CGuild* pGuild = NULL;

	CString tempName;

	tempName.Format( "%s", guildname );

	for(i = 0; i < g_arGuildData.GetSize(); i++)
	{
		if( g_arGuildData[i] )
		{
			pGuild = g_arGuildData[i];

			if( !tempName.CompareNoCase( pGuild->m_strGuildName ) )
			{
				return pGuild;
			}

			j = 0;
		}
//		else j++;

//		if(j >= 100) break;	// �ִ� 100�� ���� �� �˻��غ��� �ߴ�
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//	�ش� ����ε����� ����̸� ��ȯ
//
BOOL USER::GetGuildName(int guildnum, TCHAR *guildname)
{
	if( guildnum < 0 || guildnum >= MAX_GUILD )
	{
		return FALSE;
	}

	EnterCriticalSection( &(m_pCom->m_critGuild) );

	CGuild* pGuild = NULL;

	if( !g_arGuildData[guildnum] )
	{
		LeaveCriticalSection( &(m_pCom->m_critGuild) );
		return FALSE;
	}

	pGuild = g_arGuildData[guildnum];

	if( pGuild->m_lSid != guildnum )
	{
		LeaveCriticalSection( &(m_pCom->m_critGuild) );
		return FALSE;
	}

	strcpy( guildname, pGuild->m_strGuildName );
	
	LeaveCriticalSection( &(m_pCom->m_critGuild) );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	�� ��� �ε����� ��ȯ
//
int USER::GetEmptyGuildNum()
{
/*	EnterCriticalSection( &(m_pCom->m_critGuild) );

	int nCount = 1;

	InterlockedIncrement(&g_CurrentGuildCount);

	for(int i = 0; i < g_arGuildData.GetSize(); i++)
	{
		if(!g_arGuildData[i]) continue;

		int temp = g_arGuildData[i]->m_lSid;
		if(g_arGuildData[i]->m_lSid >= nCount) nCount = g_arGuildData[i]->m_lSid + 1;
	}

	if(nCount >= MAX_GUILD) nCount = -1;			// �̷��� DB���� ������Ʈ�ؾ��Ѵ�...

	return nCount;
*/
	EnterCriticalSection( &(m_pCom->m_critGuild) );

	for( int i = 1; i < g_arGuildData.GetSize(); i++)
	{
		if( g_arGuildData[i]->m_lSid <= 0 )
		{
			return i;
		}
	}

	return -1;

}

////////////////////////////////////////////////////////////////////////////////
//	
//
void USER::ReleaseGuild()
{
	LeaveCriticalSection( &(m_pCom->m_critGuild) );
}

////////////////////////////////////////////////////////////////////////////////
//	��� ���� ������ �Ǵ��� �Ǵ�
//
void USER::GuildOpen()
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	CBufferEx TempBuf;

	CString strTemp = _T("");
	CString strMsg = _T("");

	strTemp.Format("%s", m_strGuildName);

	if(!strTemp.IsEmpty()) 
	{
		strMsg.Format( IDS_USER_ALREADY_GUILD_IN, m_strGuildName);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);	
		return;
	}

	if(m_sGuildVersion >= 0)
	{
		strMsg.Format( IDS_USER_ALREADY_OTHER_GUILD_IN );
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);	
		return;
	}

	if(m_sLevel < GUILD_MAKE_LEVEL)  // ���� ���ѿ�..
	{
		strMsg.Format( IDS_USER_GUILD_MAKE_LEVEL, GUILD_MAKE_LEVEL);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);	
		return;
	}

	if(m_dwDN < GUILD_MAKE_DN)  // â�� �ڱݿ�...
	{
		strMsg.Format( IDS_USER_GUILD_MAKE_DINA, GUILD_MAKE_DN);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}

	int rank = m_sCityRank + CITY_RANK_INTERVAL;

	if(rank < HERO_RANK)
	{
		strMsg.Format( IDS_USER_GUILD_MAKE_CITY_RANK, "Hero");
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return;
	}

	TempBuf.Add(GUILD_DIALOG);
	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	���ο� ��带 �����.	
//
void USER::NewGuild(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	BYTE error_code = 0;
	BOOL bRes = TRUE;
	
	int i;
	int index = 0;
	int nLength = 0;
	int iGuildId = 0;
	TCHAR szGuildName[255];

	CString szTemp;
	CBufferEx	TempBuf;

	WORD *pwMark = NULL;
	CGuild *pGuild = NULL;
//	CGuildUser *pGuildUser = NULL;

	if(m_dwGuild > 0) { error_code = ERR_5; goto go_result; }  // �ٸ� ������ ����

	nLength = GetVarString(sizeof(szGuildName), szGuildName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) { error_code = ERR_1; goto go_result; } // ��� ��Ī ���ѿ� �ɸ�

	szTemp = szGuildName;

	if(!UNI_CHAR::CheckString(szTemp))
	{
		error_code = 255; 
		goto go_result;
	}

	if(szTemp.Find(' ') != -1 || szTemp.Find('\'') != -1 || szTemp.Find('"') != -1 || szTemp.Find('/') != -1)
	{
		error_code = 255; 
		goto go_result;
	}

	if(IsReservedID(szGuildName))
	{
		error_code = 04; 
		goto go_result;
	}

	if(m_sLevel < GUILD_MAKE_LEVEL) { error_code = ERR_1; goto go_result; } // ���� ���ѿ�..

	if(m_dwDN < GUILD_MAKE_DN) { error_code = ERR_1; goto go_result; } // â�� �ڱݿ�...

	pGuild = GetGuildByName(szGuildName);

	ReleaseGuild();

	if(pGuild) { error_code = ERR_4; goto go_result; }  // �̹� ����� ��� �̸��϶�...

	iGuildId = GetEmptyGuildNum();

	if(iGuildId <= 0)
	{
		ReleaseGuild();
		//InterlockedDecrement(&g_CurrentGuildCount);
		return;
	}

//	pGuild = new CGuild;
	pGuild = g_arGuildData[iGuildId];
/*
	if(strlen(pGuild->m_strGuildName) > 0)
	{
		ReleaseGuild();
		return;
	}
*/
	if(pGuild->m_lSid > 0) 
	{
		ReleaseGuild();
		return;
	}
/*
	pGuildUser = new CGuildUser;		// �޸��Ҵ翡 �����ϸ�...
	if(!pGuildUser)
	{
		ReleaseGuild();
		return;
	}
*/

	// alisia - �긴�� ������ ��� ���� ����ٴ� ��ȣ�� ������ �����Ѵ�. �Ŀ� ����� �޾Ƽ� ��带 �ϼ��Ѵ�.
	g_pMainDlg->BridgeServerGuildNewReq( m_uid, m_strUserID, iGuildId, szGuildName );
	ReleaseGuild();
	return;





	pGuild->m_lSid = iGuildId;

	strcpy(pGuild->m_strGuildName, szGuildName);
	strcpy(pGuild->m_strMasterName, m_strUserID);

	pGuild->m_dwGuildDN = 0;
	pGuild->m_sVersion = -1;

	
	pwMark = (WORD*)(&pGuild->m_strMark[0]);
	for (i=0; i<GUILD_MARK_SIZE/sizeof(WORD); i++) {
		pwMark[i] = (0x1f<<11 | 0x1f);	// Client���� ����ϴ� ��������� �ʱ�ȭ�Ѵ�.
	}

	if(!InsertGuild(pGuild))
	{
//		delete pGuild;
//		if(pGuildUser) delete pGuildUser;

//		InterlockedDecrement(&g_CurrentGuildCount);
		ReleaseGuild();
		return;
	}

//	::ZeroMemory(pGuildUser->m_strUserId, sizeof(pGuildUser->m_strUserId));

//	nLength = 0;
//	nLength = strlen(m_strUserID);

//	pGuildUser->m_lSid = iGuildId;
//	strncpy(pGuildUser->m_strUserId, m_strUserID, nLength);

//	pGuild->m_arUser.Add(pGuildUser);
	pGuild->AddUser(m_strUserID, iGuildId);

	g_arGuildData[iGuildId] = pGuild;

	ReleaseGuild();

	if( m_dwDN <= GUILD_MAKE_DN ) m_dwDN = 0;
	else m_dwDN -= GUILD_MAKE_DN;		// ��� ���� �ڱݸ� ����.

	m_bGuildMaster = TRUE;
	m_dwGuild = iGuildId;

	nLength = 0;
	nLength = strlen(szGuildName);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) return; 
	strncpy(m_strGuildName, szGuildName, nLength);
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

void USER::NewGuildWithThread(TCHAR *pBuf)
{
	int index = 0;
	int nLength = 0;
	TCHAR szGuildName[255];

	nLength = GetVarString(sizeof(szGuildName), szGuildName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) return; 

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = GUILD_OPEN_REQ;
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
//	���ο� ��带 �����.	
//
BOOL USER::InsertGuild(CGuild *pGuild)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call INSERT_GUILD (%d, \'%s\', \'%s\')}"), pGuild->m_lSid, pGuild->m_strGuildName, m_strUserID); 

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode != SQL_SUCCESS)
		return FALSE;

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
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	���̺� ��� ������ �����Ѵ�.	
//
BOOL USER::InsertGuildUser(int guildID, TCHAR *strUserID)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[2000];		

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call INSERT_GUILD_USER (%d, \'%s\')}"), guildID, strUserID); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
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
	else
	{
		//if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();

//		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	��忡 ��ϵ� ������ ������Ʈ�Ѵ�.
//
BOOL USER::UpdateGuildMark(TCHAR *pMark, CGuild *pGuild)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strMark[450];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strMark, sizeof(strMark));

	int index = 0;

	SDWORD sMarkLen		= sizeof(strMark);

	::CopyMemory(strMark, pMark, GUILD_MARK_SIZE);

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_GUILD_MARK (\'%s\',%d, ?)}"), pGuild->m_strGuildName, pGuild->m_sVersion ); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strMark), 0, (TCHAR*)strMark, 0, &sMarkLen );

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
	else
	{
		//if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();

//		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	����������̺��� �ش� ���� �����͸� �����Ѵ�.
//
BOOL USER::DeleteGuildUser(TCHAR *strUserID)
{
	if ( strUserID == NULL ) return FALSE;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	char			t_strUserID[20];

	::ZeroMemory(szSQL, sizeof(szSQL));

	strcpy( t_strUserID, strUserID );

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call DELETE_GUILD_USER (\'%s\')}"), t_strUserID ); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
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
	else
	{
		//if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();
//		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

BOOL USER::DeleteGuildUserWithThread(TCHAR *strUserID)
{
	if ( strUserID == NULL ) return FALSE;

	int nLen = strlen(strUserID);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return FALSE;

	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = DEL_GUILD_CHAR_REQ;
	Datalength = strlen(strUserID);
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, 0, Datalength+1);
	memcpy(pData, strUserID, Datalength);
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	��� ��ũ�� �����Ѵ�.
//
void USER::GuildMarkAddReq(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	CBufferEx TempBuf;
	TCHAR GuildMark[GUILD_MARK_SIZE+1];

	BOOL bRes = TRUE;
	BYTE error_code = 0;

	CGuild *pGuild = NULL;

	if(!m_bGuildMaster) return; // { error_code = ERR_1; goto go_result; }// ���¯�� �ƴϸ� ����

	::ZeroMemory(GuildMark, sizeof(GuildMark));

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) { error_code = ERR_2; goto go_result; }		// �ش� ��尡 ������ ����..

	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0) { error_code = ERR_1; goto go_result; }// ���¯�� Ʋ���� ����...

	::CopyMemory(GuildMark, pBuf, GUILD_MARK_SIZE);
	
	// alisia
	g_pMainDlg->BridgeServerGuildMarkReq( m_uid, m_strUserID, (int)m_dwGuild, GuildMark );
	ReleaseGuild();
	return;




	if(pGuild->m_sVersion >= 0) CheckMaxValue((short &)pGuild->m_sVersion, (short)1);
	else pGuild->m_sVersion = 0;

	if(!UpdateGuildMark(GuildMark, pGuild))	// DB�� ���� ���и� ����..
	{
		pGuild->m_sVersion = -1;
		error_code = 255;
		goto go_result; 
	}
	
//	pGuild->m_sVersion += 1;
	m_sGuildVersion = pGuild->m_sVersion;	// ��� ���� ����

	::CopyMemory(pGuild->m_strMark, GuildMark, GUILD_MARK_SIZE);

	bRes = FALSE;

go_result:
	ReleaseGuild();				// ����...

	TempBuf.Add(GUILD_MARK_ADD_RESULT);

	if(bRes)
	{
		TempBuf.Add((BYTE)0x00);
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}
	
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

//	SendMyInfo(TO_INSIGHT, INFO_MODIFY);
}

void USER::GuildMarkAddReqWithThread(TCHAR *pBuf)
{
	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = GUILD_MARK_ADD_REQ;
	Datalength = GUILD_MARK_SIZE;
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
//	Ŭ���̾�Ʈ�� ���ο� ��尡 �߰��Ǿ��ų� ���Ż����� �ִ��� �˷��ش�. 
//
void USER::GuildAddReq(TCHAR *pBuf)
{	
	CBufferEx TempBuf; 

	int index = 0;
	BOOL bRes = TRUE;
	BYTE error_code = 0;

	int iGuildID = GetInt(pBuf, index);

	CGuild *pGuild = NULL;

	if(iGuildID < 0) return;

	TempBuf.Add(GUILD_ADD_RESULT);

	pGuild = GetGuild(iGuildID);

	if(!pGuild) { error_code = ERR_1; goto go_result; }		// �ش� ��尡 ������ ����..

	if(!pGuild->m_strMark) { error_code = 255; goto go_result; }

	TempBuf.Add((BYTE)0x01);
	TempBuf.Add((int)pGuild->m_lSid);
	TempBuf.AddString(pGuild->m_strGuildName);
	TempBuf.Add(pGuild->m_sVersion);
	TempBuf.AddData((TCHAR*)pGuild->m_strMark, GUILD_MARK_SIZE);
	bRes = FALSE;

go_result:
	ReleaseGuild();				// ����...

	if(bRes)
	{
		TempBuf.Add((BYTE)0x00);
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	Memory DB �� �ʱ�ȭ �Ѵ�.
//
BOOL USER::InitMemoryDB(int uid)
{
/*	int nLen = 0;
	CSharedMemory*	pSharedMemory;
	CMemUser*		pMD;

	for(int i = 0; i < MAX_USER; i++)
	{
		pSharedMemory = NULL;
		pSharedMemory = g_arSharedMemory[i];

		if(!pSharedMemory) continue;

		pMD = (CMemUser*)pSharedMemory->m_lpData;
		if(pMD->m_uid == -1) continue;
		if(pMD->m_sSTR == 0) continue;

		nLen = strlen(pMD->m_strUserID);
		if(nLen <= 0) continue;

		if(strncmp(pMD->m_strUserID, m_strUserID, nLen) == 0)
		{
			UpdateMem2DB(pMD);
			UpdateBankMem2DB(pMD);

			pMD->m_uid = -1;
			pMD->m_sSTR = 0;			
			pMD->m_UB.m_uid = -1;
			pMD->m_dwSaveTime = 0;
			::ZeroMemory(pMD->m_strUserID, sizeof(pMD->m_strUserID));
		}
	}
*/
	m_pSharedMemory = g_arSharedMemory[uid];
	m_pMD = (CMemUser*) m_pSharedMemory->m_lpData;

	m_pMD->m_uid = uid;
	m_pMD->m_UB.m_uid = uid;
	m_pMD->m_AB.m_uid = uid;
	m_pMD->m_iMyServer = m_iMyServer;

	::ZeroMemory(m_pMD->m_strAccount, sizeof(m_pMD->m_strAccount));
	strncpy(m_pMD->m_strAccount, m_strAccount, strlen(m_strAccount));

	::ZeroMemory(m_pMD->m_strUserID, sizeof(m_pMD->m_strUserID));
	strncpy(m_pMD->m_strUserID, m_strUserID, strlen(m_strUserID));

	::ZeroMemory(m_pMD->m_UB.m_strUserID, sizeof(m_pMD->m_UB.m_strUserID));
	strncpy(m_pMD->m_UB.m_strUserID, m_strUserID, strlen(m_strUserID));

	::ZeroMemory(m_pMD->m_AB.m_strAccount, sizeof(m_pMD->m_AB.m_strAccount));
	strncpy(m_pMD->m_AB.m_strAccount, m_strAccount, strlen(m_strAccount));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	�޸� DB�� ������ DB�� �ű��.
//
void USER::UpdateMem2DB(CMemUser *pMD)
{
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

	::CopyMemory(strFace, pMD->m_strFace, sizeof(strFace));
	::CopyMemory(strSkill, pMD->m_strSkill, sizeof(strSkill));
	::CopyMemory(strItem, pMD->m_strItem, sizeof(strItem));
	::CopyMemory(strPsi, pMD->m_strPsi, sizeof(strPsi));
	::CopyMemory(strTel, pMD->m_strTel, sizeof(strTel));
	::CopyMemory(strHaveEvent, pMD->m_strHaveEvent, sizeof(strHaveEvent));

	::CopyMemory(strQuickItem, pMD->m_strQuickItem, sizeof(strQuickItem));

	SDWORD sFaceLen		= sizeof(strFace);
	SDWORD sSkillLen	= sizeof(strSkill);
	SDWORD sItemLen		= sizeof(strItem);
	SDWORD sPsiLen		= sizeof(strPsi);
	SDWORD sQuickLen	= sizeof(strQuickItem);
	SDWORD sEventLen	= sizeof(strHaveEvent);
	SDWORD sTelLen		= sizeof(strTel);


	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_USER_DATA (\'%s\',%d,%d,%d,%d,%d,%d,%d,%d,\
		?, %d,%d,%d, %d, %d,%d,  %d,%d,  %d, \
		%d,%d,%d,%d,%d,%d, %d,%d,%d,%d, \
		?,?,?,?, %d,%d,\
		%d, %d, ?, %d, %d,\
		?, %d,\
		%d, %d, %d,\
		\'%s\', \
		%d,%d,		%d,%d,		%d,%d)}"), 
		pMD->m_strUserID, pMD->m_sSTR, pMD->m_sCON,	pMD->m_sDEX, pMD->m_sVOL, pMD->m_sWIS, pMD->m_iSkin, pMD->m_iHair, pMD->m_sGender,	
		pMD->m_curz, pMD->m_curx, pMD->m_cury, pMD->m_dwBuddy, pMD->m_dwGuild, pMD->m_dwExp, pMD->m_sPA, pMD->m_sSkillPoint, pMD->m_dwXP,
		pMD->m_sMaxHP, pMD->m_sHP, pMD->m_sMaxPP, pMD->m_sPP, pMD->m_sMaxSP, pMD->m_sSP, pMD->m_dwDN, pMD->m_sCityRank, pMD->m_sLevel, pMD->m_byClass,

		pMD->m_tAbnormalKind, pMD->m_dwAbnormalTime, 

		pMD->m_bLive, pMD->m_iCityValue, pMD->m_sKillCount, pMD->m_dwSaveTime,
		pMD->m_dwSaintTime, 
		pMD->m_dwHiExpTime, pMD->m_dwMagicFindTime, pMD->m_dwNoChatTime, 
		pMD->m_strLoveName, //--yskang 0.1 
		pMD->m_tPsiOneKind, pMD->m_dwPsiOneTime,		pMD->m_tPsiTwoKind, pMD->m_dwPsiTwoTime,		pMD->m_tPsiThreeKind, pMD->m_dwPsiThreeTime); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strFace),	0, (TCHAR*)strFace,		0, &sFaceLen );

		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strSkill),	0, (TCHAR*)strSkill,	0, &sSkillLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strPsi),		0, (TCHAR*)strPsi,		0, &sPsiLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strQuickItem),	0, (TCHAR*)strQuickItem,	0, &sQuickLen );

		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strHaveEvent),	0, (TCHAR*)strHaveEvent,	0, &sEventLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strTel),	0, (TCHAR*)strTel,	0, &sTelLen );

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
	else
	{
		//if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		BREAKPOINT();

//		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
}

void USER::UpdateBankMem2DB(CMemUser *pMD)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strBankItem[_BANK_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strBankItem, sizeof(strBankItem));
	
	::CopyMemory(strBankItem, pMD->m_UB.m_UserBankItem, sizeof(strBankItem));
	
	SDWORD sBankItemLen	= sizeof(strBankItem);
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_BANK_DATA_ONLY (\'%s\',%d, ?)}"), pMD->m_UB.m_strUserID, pMD->m_UB.m_dwBankDN);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To Update User Bank Data Only!!\n");

//		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strBankItem), 0, (TCHAR*)strBankItem, 0, &sBankItemLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DB[m_iModSid].ReleaseDB(db_index);
			return;
		}
	}	

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return;

	return;
}

void USER::UpdateAccountBankMem2DB(CMemUser *pMD)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];
	TCHAR			strBankItem[_ACCOUNT_BANK_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strBankItem, sizeof(strBankItem));
	
	::CopyMemory(strBankItem, pMD->m_AB.m_AccountBankItem, sizeof(strBankItem));
	
	SDWORD sBankItemLen	= sizeof(strBankItem);
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_ACCOUNT_BANK_DATA_ONLY (\'%s\',%d, ?)}")/*, pMD->m_iMyServer*/, pMD->m_AB.m_strAccount, pMD->m_AB.m_dwBankDN);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		return;
	}

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strBankItem), 0, (TCHAR*)strBankItem, 0, &sBankItemLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DB[m_iModSid].ReleaseDB(db_index);
			return;
		}
	}	

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return;

	return;
}

////////////////////////////////////////////////////////////////////////////////
//	Memory DB�� ���� ��� ���� Data �� ����.
//
BOOL USER::UpdateMemUserAll(BOOL bBank)
{
	TCHAR strBankItem[_BANK_DB];
	TCHAR strAccountBankItem[_ACCOUNT_BANK_DB];

	if(m_pSharedMemory == NULL) return FALSE;
	if(m_pSharedMemory->m_hMapping == NULL) return FALSE;
	if(m_pMD == NULL) return FALSE;
	if(m_pMD->m_uid == -1) return FALSE;

	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return FALSE;

	DWORD curTime = ConvertCurTimeToSaveTime();
	if(m_pMD->m_dwSaveTime > curTime) return FALSE;

	if(bBank)
	{
		if(m_pMD->m_uid != m_pMD->m_UB.m_uid) return FALSE;
		if(_stricmp(m_strUserID, m_pMD->m_UB.m_strUserID) != 0) return FALSE;
		if(_stricmp(m_strAccount, m_pMD->m_AB.m_strAccount) != 0) return FALSE;

		m_pMD->m_UB.m_dwBankDN = 0;
		::ZeroMemory(strBankItem, sizeof(strBankItem));
		UserBankItemToStr(strBankItem);
		memcpy(m_pMD->m_UB.m_UserBankItem, strBankItem, sizeof(strBankItem));
		m_pMD->m_UB.m_dwBankDN = m_dwBankDN;

		m_pMD->m_AB.m_dwBankDN = 0;
		::ZeroMemory(strAccountBankItem, sizeof(strAccountBankItem));
		UserAccountBankItemToStr( strAccountBankItem );
		memcpy(m_pMD->m_AB.m_AccountBankItem, strAccountBankItem, sizeof(strAccountBankItem));
		m_pMD->m_AB.m_dwBankDN = m_dwAccountBankDN;
	}

	m_pMD->m_dwSaveTime = curTime;			// ���� ����

	m_pMD->m_sSTR = m_sSTR;					// ��
	m_pMD->m_sCON = m_sCON;					// �ǰ�
	m_pMD->m_sDEX = m_sDEX;					// ��ø��
	m_pMD->m_sVOL = m_sVOL;					// ����
	m_pMD->m_sWIS = m_sWIS;					// ����
	
	m_pMD->m_iSkin = m_iSkin;				// �Ǻλ�
	m_pMD->m_iHair = m_iHair;				// �Ӹ���
	m_pMD->m_sGender = m_sGender;			// ����
//	_tcscpy(m_pMD->m_strFace, m_strFace);	// �󱼸��
	memcpy(m_pMD->m_strFace, m_strFace, sizeof(m_strFace));	// �󱼸��

	m_pMD->m_curx = m_curx;					// ���� X ��ǥ
	m_pMD->m_cury = m_cury;					// ���� Y ��ǥ
	m_pMD->m_curz = m_curz;					// ���� ��

	m_pMD->m_dwBuddy = m_dwBuddy;			// �����ȣ
	m_pMD->m_dwGuild = m_dwGuild;			// ����ȣ

	m_pMD->m_dwExp = m_dwExp;				// ��������ġ

	m_pMD->m_sPA = m_sPA;					// PA Point
	m_pMD->m_sSkillPoint = m_sSkillPoint;	// Skill Point
	
	m_pMD->m_dwXP = m_dwXP;					// X Point

	m_pMD->m_sMaxHP	= m_sMaxHP;				// �ִ� HP
	m_pMD->m_sHP	= m_sHP;				// ���� HP
	m_pMD->m_sMaxPP = m_sMaxPP;				// �ִ� PP
	m_pMD->m_sPP	= m_sPP;				// ���� PP
	m_pMD->m_sMaxSP = m_sMaxSP;				// �ִ� SP
	m_pMD->m_sSP	= m_sSP;				// ���� SP

	m_pMD->m_dwDN = m_dwDN;					// ������

	m_pMD->m_sCityRank = m_sCityRank;		// �ù� ���
	m_pMD->m_sKillCount = m_sKillCount;		// ���� PK�� Ƚ��

	m_pMD->m_sLevel = m_sLevel;				// �迭����
	m_pMD->m_byClass = m_byClass;			// Ŭ����


	TCHAR strSkill[_SKILL_DB], strItem[_ITEM_DB], strPsi[_PSI_DB], strHaveEvent[_EVENT_DB], strTel[_TEL_DB];

	::ZeroMemory(strSkill, sizeof(strSkill));
	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strPsi, sizeof(strPsi));
	::ZeroMemory(strTel, sizeof(strTel));
	::ZeroMemory(strHaveEvent, sizeof(strHaveEvent));

	UserSkillToStr(strSkill);
	UserItemToStr(strItem);
	UserPsiToStr(strPsi);
	UserTelToStr(strTel);
	UserHaveEventDataToStr(strHaveEvent);
/*
	strcpy(m_pMD->m_strSkill, strSkill);	// User Skill DB
	strcpy(m_pMD->m_strItem, strItem);		// User Item DB
	strcpy(m_pMD->m_strPsi, strPsi);		// User Psionic DB
	strcpy(m_pMD->m_strHaveEvent, strHaveEvent);// User Event DB
*/
	memcpy(m_pMD->m_strSkill, strSkill, sizeof(strSkill));
	memcpy(m_pMD->m_strItem, strItem, sizeof(strItem));
	memcpy(m_pMD->m_strPsi, strPsi, sizeof(strPsi));
	memcpy(m_pMD->m_strTel, strTel, sizeof(strTel));
	memcpy(m_pMD->m_strHaveEvent, strHaveEvent, sizeof(strHaveEvent));

	::CopyMemory(m_pMD->m_sChangeClass, m_sChangeClass, sizeof(m_sChangeClass));	// ������ Ŭ�������� ����
	
	// �����̻� ���� ����
	m_pMD->m_tAbnormalKind = m_tAbnormalKind;
	m_pMD->m_dwAbnormalTime = m_dwAbnormalTime;

	m_pMD->m_tIsOP = m_tIsOP;				// ������� �Ǵ�
											// 0: Normal User
											// 1: Game Operator
											// 2: Server Operator

	m_pMD->m_bLive = m_bLive;				// �׾���? ��Ҵ�?

	m_pMD->m_sTempSTR = m_sTempSTR;			// ��
	m_pMD->m_sTempCON = m_sTempCON;			// �ǰ�
	m_pMD->m_sTempDEX = m_sTempDEX;			// ��ø��
	m_pMD->m_sTempVOL = m_sTempVOL;			// ����
	m_pMD->m_sTempWIS = m_sTempWIS;			// ����

	m_pMD->m_iCityValue = m_iCityValue;		// ���� ���� ���Ⱚ

	m_pMD->m_dwSaintTime		= m_dwSaintTime;
	
	m_pMD->m_dwHiExpTime		= m_dwHiExpTime;
	m_pMD->m_dwMagicFindTime	= m_dwMagicFindTime;
	m_pMD->m_dwNoChatTime		= m_dwNoChatTime;

	m_pMD->m_tPsiOneKind = m_pMD->m_tPsiTwoKind = m_pMD->m_tPsiThreeKind = 0;
	m_pMD->m_dwPsiOneTime = m_pMD->m_dwPsiTwoTime = m_pMD->m_dwPsiThreeTime = 0;

	// Psionic One
	if(m_dwHasteTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_HASTE;
		m_pMD->m_dwPsiOneTime = m_dwHasteTime;
	}
	if(m_dwShieldTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_SHIELD;
		m_pMD->m_dwPsiOneTime = m_dwShieldTime;
	}
	if(m_dwDexUpTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_DEXUP;
		m_pMD->m_dwPsiOneTime = m_dwDexUpTime;
	}
	if(m_dwMaxHPUpTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_HPUP;
		m_pMD->m_dwPsiOneTime = m_dwMaxHPUpTime;
	}
	if(m_dwFastRunTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_FAST_RUN;
		m_pMD->m_dwPsiOneTime = m_dwFastRunTime;
	}
	if(m_dwMindShockTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_MIND_SHOCK;
		m_pMD->m_dwPsiOneTime = m_dwMindShockTime;
	}
	if(m_dwPsiShieldTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_PSI_SHIELD;
		m_pMD->m_dwPsiOneTime = m_dwPsiShieldTime;
	}
	if(m_dwBigShieldTime != 0) 
	{
		m_pMD->m_tPsiOneKind = 30;
		m_pMD->m_dwPsiOneTime = m_dwBigShieldTime;
	}
	if(m_dwPiercingShieldTime != 0) 
	{
		m_pMD->m_tPsiOneKind = PSIONIC_PIERCING_SHIELD;
		m_pMD->m_dwPsiOneTime = m_dwPiercingShieldTime;
	}

	// Psionic Two
	if(m_dwAdamantineTime != 0) 
	{
		m_pMD->m_tPsiTwoKind = PSIONIC_ADAMANTINE;
		m_pMD->m_dwPsiTwoTime = m_dwAdamantineTime;
	}
	if(m_dwMightyWeaponTime != 0) 
	{
		m_pMD->m_tPsiTwoKind = PSIONIC_MIGHTYWEAPON;
		m_pMD->m_dwPsiTwoTime = m_dwMightyWeaponTime;
	}
	if(m_dwBerserkerTime != 0) 
	{
		m_pMD->m_tPsiTwoKind = PSIONIC_BERSERKER;
		m_pMD->m_dwPsiTwoTime = m_dwBerserkerTime;
	}

	// Psionic Three
	if(m_dwMindGuardTime != 0) 
	{
		m_pMD->m_tPsiThreeKind = PSIONIC_MIND_GUARD;
		m_pMD->m_dwPsiThreeTime = m_dwMindGuardTime;
	}
	m_pMD->m_dwBSTime=m_dwBSTime;
	m_pMD->m_dwDNMoney = m_dwDNMoney;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// DB�� ���൥����, Memory DB�� ���� ����Ÿ�� �����Ѵ�
//
BOOL USER::UpdateMemUserBank()
{
//	if(!UpdateBankDataOnly()) return FALSE;
	return UpdateMemUserAll(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	DB�� ���࿡ ���õ� �����͸� �����Ѵ�.
//
BOOL USER::UpdateBankDataOnly()
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strBankItem[_BANK_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strBankItem, sizeof(strBankItem));
	
	UserBankItemToStr(strBankItem);
	
	SDWORD sBankItemLen	= sizeof(strBankItem);
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_BANK_DATA_ONLY (\'%s\',%d, ?)}"), m_strUserID, m_dwBankDN);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To Update User Bank Data Only!!\n");

//		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strBankItem), 0, (TCHAR*)strBankItem, 0, &sBankItemLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Restart ���� �� �� �ٽ� �޸� ��� �ʱ�ȭ �Ѵ�.
//
void USER::ReInitMemoryDB()
{
	if(m_pMD != NULL) 
	{
		m_pMD->m_uid = -1;
		m_pMD->m_sSTR = 0;
		m_pMD->m_UB.m_uid = -1;
		m_pMD->m_AB.m_uid = -1;
		m_pMD->m_dwSaveTime = 0;

		::ZeroMemory(m_pMD->m_strUserID, sizeof(m_pMD->m_strUserID));
		::ZeroMemory(m_pMD->m_strAccount, sizeof(m_pMD->m_strUserID));
	}
}

////////////////////////////////////////////////////////////////////////////////
//	��� ¯�� ���� ���� �����¿��� �����Ѵ�. 
//
void USER::GuildInvite(int uid)
{	
//	if ( pBuf == NULL ) return;

	int index = 0;
	BYTE error_code = 0;

	int nLen = 0;

//	int uid = 0;
	int iCount = 0;
	USER* pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	BOOL bRes = TRUE;

	if(!m_bGuildMaster && !m_bRepresentationGuild) { error_code = ERR_10; goto go_result; }	// ��� ������ ����.

//	uid = GetInt(pBuf, index);

	pUser = GetUser(uid - USER_BAND);

	if(!pUser) { error_code = ERR_3; goto go_result; }				// ������ ����.

	if(IsThereUser(pUser) == FALSE || strcmp(m_strUserID, pUser->m_strUserID) == 0)
	{
		SendSystemMsg( IDS_USER_SEE_EACH_OTHER, SYSTEM_NORMAL, TO_ME);
		return;
	}

	nLen = strlen(pUser->m_strUserID);
	if(nLen <= 0) return;

	if(pUser->m_dwGuild > 0)  { error_code = ERR_9; goto go_result; }// �̹� �ٸ� ��忡 ������ ���� 
	
	if(CheckInGuildWarring()) return;								// �����߿��� ����A������.

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

		m_bGuildMaster = FALSE;
		m_bRepresentationGuild = FALSE;
		error_code = ERR_10;		// 
		goto go_result;
	}

	index = -1;
	index = pGuild->GetUser(pUser->m_strUserID);
	if(index >= 0) 
	{ 
		ReleaseGuild();				// ����...

		pUser->m_dwGuild = m_dwGuild;			// ��� ��ȣ�� �ٽ� �����Ѵ�.
		SendGuildInfo(pUser);

		error_code = ERR_9; 
		goto go_result; 
	}// �̹� �ٸ� ��忡 ������ ����	
/*
	iCount = pGuild->m_arUser.GetSize();
	if(iCount <= 0 || iCount >= MAX_GUILD_USER) 
	{
		ReleaseGuild();				// ����...
		error_code = ERR_11;			// 
		goto go_result;
	}

	pGuildUser = new CGuildUser;	// �޸� �Ҵ� ���и�..
	if(!pGuildUser)
	{
		ReleaseGuild();				// ����...
		error_code = 255;			// 
		goto go_result;
	}
*/

	// alisia
	g_pMainDlg->BridgeServerGuildInviteReq( m_uid, pUser->m_uid, m_strUserID, pUser->m_strUserID, (int)m_dwGuild );
	ReleaseGuild();
	return;
	//




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

//	pGuildUser = new CGuildUser;	
//	pGuildUser->m_lSid = 0;
//	::ZeroMemory(pGuildUser->m_strUserId, sizeof(pGuildUser->m_strUserId));

//	pGuildUser->m_lSid = m_dwGuild;	
//	strncpy(pGuildUser->m_strUserId, pUser->m_strUserID, nLen);

//	pGuild->m_arUser.Add(pGuildUser);

	ReleaseGuild();

	UpdateUserData();

	AddGuildUserInFortress(pUser);

	SendGuildInviteUser(pUser);			// ��� ����Ʈ�� �ְ� �ޱ�
	SendGuildInfo(pUser);				// �þ� ������ ��忡 ������ ����� �����ϱ� ������ ���̶�� �Ѵ�...

//	SendSystemMsg( IDS_USER_ENTER_SUCCESS, SYSTEM_NORMAL, TO_ME);
//	pUser->SendSystemMsg( IDS_USER_ENTER_SUCCESS, SYSTEM_NORMAL, TO_ME);
}

void USER::GuildInviteWithThread(TCHAR *pBuf)
{	
//	uid = GetInt(pBuf, index);
	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = GUILD_INVITE_USER_REQ;			// ���θ����.
	Datalength = sizeof(int);
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
//	�ش� �����鿡�� ������ �����̵� ������ �˸���.
//
void USER::SendGuildInviteUser(USER *pUser)
{
	int i;
	int nLen = 0;	
	int nCount = -1;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	if(m_dwGuild <= 0) return;

	USER *pGUser = NULL;

	CBufferEx TempBuf;

	CString strMsg = _T("");
	strMsg.Format( IDS_USER_NEW_MEMBER, pUser->m_strUserID);

	for(i = 0; i< MAX_USER; i++)
	{
		pGUser = m_pCom->GetUserUid(i);

		if(pGUser == NULL || pGUser->m_state != STATE_GAMESTARTED) continue;		

		if(m_dwGuild == pGUser->m_dwGuild)
		{
			pGUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		}
	}
/*
//	SetGuildUserIndex(pUser);										// �渶�� ���� ����...

	for(i = 0; i < MAX_GUILD_USER; i++)								// �̷��� ���� ����Ʈ�� �����ϴ°�...
	{																// ������ Ž���� ���ҷ���...		
		pUser->m_MyGuildUser[i].uid = -1;
		pUser->m_MyGuildUser[i].lUsed = 0;							// �ش� ���� ����Ʈ�� �ʱ�ȭ�Ѵ�.
		::ZeroMemory(pUser->m_MyGuildUser[i].GuildUser, sizeof(pUser->m_MyGuildUser[i].GuildUser));

		nLen = strlen(m_MyGuildUser[i].GuildUser);
																	// ������ �������� ��� ����Ʈ�� copy�� �ش�	
		if(nLen > 0 && nLen <= CHAR_NAME_LENGTH) strncpy(pUser->m_MyGuildUser[i].GuildUser, m_MyGuildUser[i].GuildUser, nLen);
		else continue;

		pGUser = GetUser(m_MyGuildUser[i].uid);

		if(!pGUser)													// ���� uid�� ã���� ���ٸ� �ٽ� ���̵�� �˻�	
		{
			pGUser = GetUser(m_MyGuildUser[i].GuildUser);
			if(!pGUser) 
			{ 
				pUser->m_MyGuildUser[i].uid = -1;
				m_MyGuildUser[i].uid = -1; 
				continue; 
			}
		}
		else if(strcmp(m_MyGuildUser[i].GuildUser, pGUser->m_strUserID) != 0)
		{															// uid�� ���� �ٸ��� �ٽ� �˻��ؼ� uid�� ����	
			pGUser = GetUser(m_MyGuildUser[i].GuildUser);			// �̷��� �������� strUserID�� ã�°��� ���ϱ�����
			if(!pGUser) 
			{
				pUser->m_MyGuildUser[i].uid = -1;
				m_MyGuildUser[i].uid = -1; 
				continue; 
			}
		}

		nCount = pGUser->SetGuildUserIndex(pUser);					// �� �������� ���� ������ ���������� ����

		pUser->m_MyGuildUser[i].uid = pGUser->m_uid;				// ���� �׼ӿ� ������ ����Ʈ�󿡼� uid�� ����

		pGUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_NORMAL, TO_ME);
	}	
*/
}

////////////////////////////////////////////////////////////////////////////////
//	��带 Ż���Ѵ�.
//
void USER::GuildOff(TCHAR *pBuf)
{
	int index = 0;
	BYTE error_code = 0;

	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	BOOL bRes = TRUE;

	if(m_dwGuild <= 0) { error_code = ERR_8; goto go_result; }
	
	if(m_bGuildMaster) { error_code = ERR_8; goto go_result; }

	if(m_tGuildHouseWar == GUILD_WARRING || m_tGuildWar == GUILD_WARRING || m_tFortressWar == GUILD_WARRING) 
	{ 
		SendSystemMsg( IDS_USER_NO_OUT_IN_WAR, SYSTEM_ERROR, TO_ME);
		return; 
	}

	if(CheckInGuildWarring()) return;								// �����߿��� ����A������.

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) 
	{
		ReleaseGuild();				// ����...
		error_code = ERR_7;
		goto go_result;
	}

	index = -1;
	index = pGuild->GetUser(m_strUserID);

	if(index < 0)					// �ش� ��忡�� ������ ������...
	{
		m_dwGuild = -1;							// �ʱ�ȭ�Ѵ�.
		m_sGuildVersion = -1;					// ��� ���� ����
		::ZeroMemory(m_strGuildName, sizeof(m_strGuildName));	
		m_bGuildMaster = FALSE;					// ��� ������
		m_bRepresentationGuild = FALSE;			// ���� ���� ����	 	

		ReleaseGuild();				
		error_code = ERR_8;
		goto go_result;
	}
												// ��� �����Ͱ� Ż���ǻ縦 ������ ����...^^			
	if(strcmp(pGuild->m_strMasterName, m_strUserID) == 0)
	{
		ReleaseGuild();				
		error_code = ERR_8;
		goto go_result;
	}

	// alisia
	g_pMainDlg->BridgeServerGuildOffReq( m_uid, m_strUserID, m_strUserID, (int)m_dwGuild );
	ReleaseGuild();
	return;




	if(!pGuild->RemoveUser(m_strUserID))
	{
		ReleaseGuild();				
		error_code = ERR_8;
		goto go_result;
	}

	if(!DeleteGuildUser(m_strUserID))		// ���̺��� �����.
	{
		pGuild->AddUser(m_strUserID, m_dwGuild);

		ReleaseGuild();				
		error_code = 255;
		goto go_result;
	}

	//if(!DeleteGuildUserWithThread(m_strUserID))		// ���̺��� �����.
	//{
	//	ReleaseGuild();				
	//	error_code = 255;
	//	goto go_result;
	//}

//	pGuild->RemoveUser(m_strUserID);		// �޸𸮿��� �����.
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

	pUser = GetUser(m_uid);

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;

	SendGuildOffUser(pUser);				// �ٸ� �������� ���� Ż���� ����� �˸���.

	DelGuildUserInFortress(m_strUserID, m_dwGuild);

	m_dwGuild = -1;							// �ʱ�ȭ�Ѵ�.
	m_sGuildVersion = -1;					// ��� ���� ����
	::ZeroMemory(m_strGuildName, sizeof(m_strGuildName));	
	m_bGuildMaster = FALSE;					// ��� ������
	m_bRepresentationGuild = FALSE;			// ���� ���� ����	 	

	SendMyGuildInfo();						// ��� Ż�� �˸���.

	UpdateUserData();
//	SendSystemMsg( IDS_USER_OUT_GUILD, SYSTEM_NORMAL, TO_ME);
}

void USER::GuildOffWithThread(TCHAR *pBuf)
{
	int index = 0;
	BYTE error_code = 0;
	USER *pUser = NULL;
	BOOL bRes = TRUE;

	if(m_dwGuild <= 0) { error_code = ERR_8; goto go_result; }
	
	if(m_bGuildMaster) { error_code = ERR_8; goto go_result; }

	if(m_tGuildHouseWar == GUILD_WARRING || m_tGuildWar == GUILD_WARRING || m_tFortressWar == GUILD_WARRING) 
	{ 
		SendSystemMsg( IDS_USER_NO_OUT_IN_WAR, SYSTEM_ERROR, TO_ME);
		return; 
	}

	int Datalength;
	SQLDATAPACKET *pSDP;

	pSDP = new SQLDATAPACKET;
	pSDP->code = GUILD_OFF_WITH_THREAD;
	Datalength = 0;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pSDP->pData = NULL;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );
	return;	

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

	pUser = GetUser(m_uid);
	SendGuildOffUser(pUser);				// �ٸ� �������� ���� Ż���� ����� �˸���.

	m_dwGuild = -1;							// �ʱ�ȭ�Ѵ�.
	m_sGuildVersion = -1;					// ��� ���� ����
	::ZeroMemory(m_strGuildName, sizeof(m_strGuildName));	
	m_bGuildMaster = FALSE;					// ��� ������
	m_bRepresentationGuild = FALSE;			// ���� ���� ����	 	

	SendMyGuildInfo();						// ��� Ż�� �˸���.


//	SendSystemMsg( IDS_USER_OUT_GUILD, SYSTEM_NORMAL, TO_ME);
}

////////////////////////////////////////////////////////////////////////////////
//	��带 Ż���� ������ �ٸ� �������� �˸���.
//
void USER::SendGuildOffUser(USER *pUser)
{
	int i;
	int nLen = 0;	
	int nCount = -1;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	if(m_dwGuild <= 0) return;

	USER *pGUser = NULL;

	CBufferEx TempBuf;

	CString strMsg = _T("");
	strMsg.Format( IDS_USER_OUT_MEMBER, pUser->m_strUserID);

	for(i = 0; i< MAX_USER; i++)
	{
		pGUser = m_pCom->GetUserUid(i);

		if(pGUser == NULL || pGUser->m_state != STATE_GAMESTARTED) continue;		

		if(m_dwGuild == pGUser->m_dwGuild) pGUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
	}

/*
	for(i = 0; i < MAX_GUILD_USER; i++)								// �̷��� ���� ����Ʈ�� �����ϴ°�...
	{																// ������ Ž���� ���ҷ���...		
		pGUser = GetUser(m_MyGuildUser[i].uid);

		if(!pGUser)													// ���� uid�� ã���� ���ٸ� �ٽ� ���̵�� �˻�	
		{
			pGUser = GetUser(m_MyGuildUser[i].GuildUser);
			if(!pGUser) { m_MyGuildUser[i].uid = -1; continue; }
		}
		else if(strcmp(m_MyGuildUser[i].GuildUser, pGUser->m_strUserID) != 0)
		{															// uid�� ���� �ٸ��� �ٽ� �˻��ؼ� uid�� ����	
			pGUser = GetUser(m_MyGuildUser[i].GuildUser);			// �̷��� �������� strUserID�� ã�°��� ���ϱ�����
			if(!pGUser) 
			{
				m_MyGuildUser[i].uid = -1; 
				continue; 
			}
		}

		pGUser->DelGuildUserIndex(pUser);							// �� �������� ������ ������ �˷��ش�... 

		pGUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_NORMAL, TO_ME);
	}	
*/
}

////////////////////////////////////////////////////////////////////////////////
//	�渶�� �ڱ� ��带 �ػ� ��Ų��.
//
void USER::GuildDisperse(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int i;//, j;
	int index = 0, nLen = 0;
	BYTE error_code = 0;

	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	CStore *pStore = NULL;
	CGuildFortress *pFort = NULL;

	if( m_curz == 56 || m_curz == 57 || m_curz == 58 || m_curz == 59 )	// ARK������ ��� �ػ� �� �� ����.
	{
		SendSystemMsg( _T("A.R.K.������ ��带 �ػ� �� �� �����ϴ�."), SYSTEM_ERROR, TO_ME);
		return;
	}

	if(!m_bGuildMaster) 
	{
		SendSystemMsg( IDS_USER_NO_PERMISSION_USE, SYSTEM_ERROR, TO_ME);		
		return;
	}

	if(m_dwGuild <= 0 || m_dwGuild >= g_arGuildData.GetSize()) return;

	if(CheckInGuildWarring()) return;								// �����߿��� ����A������.

	// Field War�϶�
	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0)
	{
		SendSystemMsg( IDS_USER_CANT_DISMISS_IN_WAR, SYSTEM_ERROR, TO_ME);
		return;
	}

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i]) continue;

		pFort = g_arGuildFortress[i];

		if(pFort->m_iGuildSid == m_dwGuild)
		{
			SendSystemMsg( IDS_USER_CANT_DISMISS_FORTRESS, SYSTEM_ERROR, TO_ME);
			return;
		}
	}

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) 
	{
		ReleaseGuild();				// ����...
		return;
	}

	nLen = strlen(m_strUserID);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH)
	{
		ReleaseGuild();				
		return;
	}

	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0)
	{
		ReleaseGuild();				
		return;
	}

	// alisia - ��� â�� ������� �ƴϸ� ��������� ����� ��� �ػ��� ��û���� �����Ѵ�.
	if(InterlockedCompareExchange((LONG*)&g_arGuildData[m_dwGuild]->m_lUsed, (long)1, (long)0) == (long)0)
	{
		g_pMainDlg->BridgeServerGuildDisperseReq( m_uid, m_strUserID, (int)m_dwGuild );
	}
	else
	{
		SendSystemMsg( IDS_USER_CANT_DISMISS_IN_BANK, SYSTEM_ERROR, TO_ME);
	}
	ReleaseGuild();
	return;





	// �ش� ��� â�� ������� �ƴϸ� ����...
	if(InterlockedCompareExchange((LONG*)&g_arGuildData[m_dwGuild]->m_lUsed, (long)1, (long)0) == (long)0)
	{
		if(!DeleteGuildDB()) 				// DB ����
		{
			ReleaseGuild();
			::InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, 0);
			return;
		}
											// ������ ���� �ߴٸ� �ʱ�ȭ	
/*		for(i = 0; i < g_arStore.GetSize(); i++)
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

		// ��� �޸� ����		
/*		for(j = 0; j < g_arGuildData[m_dwGuild]->m_arUser.GetSize(); j++)
		{
			if(g_arGuildData[m_dwGuild]->m_arUser[j]) delete g_arGuildData[m_dwGuild]->m_arUser[j];
		}

		g_arGuildData[m_dwGuild]->m_arUser.RemoveAll();
*/		g_arGuildData[m_dwGuild]->InitGuild();

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
	}
	else 
	{
		ReleaseGuild();
		SendSystemMsg( IDS_USER_CANT_DISMISS_IN_BANK, SYSTEM_ERROR, TO_ME);
	}

	UpdateUserData();
//	ReleaseGuild();
}

void USER::GuildDisperseWithThread(TCHAR *pBuf)
{
	int i, j;
	int index = 0, nLen = 0;
	BYTE error_code = 0;

	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	CStore *pStore = NULL;
	CGuildFortress *pFort = NULL;

	if(!m_bGuildMaster) return;
	if(m_dwGuild <= 0 || m_dwGuild >= g_arGuildData.GetSize()) return;

	// �������϶�...
	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		pFort = g_arGuildFortress[i];
		if(!pFort) continue;

		if(g_arGuildFortress[i]->m_lUsed == 1)
		{
			if(pFort->m_iGuildSid == m_dwGuild)	// ���������
			{
				SendSystemMsg( IDS_USER_CANT_DISMISS_IN_WAR, SYSTEM_ERROR, TO_ME);
				return;
			}

			for(j =0; j < GUILD_ATTACK_MAX_NUM; j++)// ����������..
			{
				if(pFort->m_arAttackGuild[j].lGuild == m_dwGuild)
				{
					SendSystemMsg( IDS_USER_CANT_DISMISS_IN_WAR, SYSTEM_ERROR, TO_ME);
					return;
				}
			}
		}
	}
	// ������...
/*	for(i = 0; i < g_arStore.GetSize(); i++)
	{
		pStore = g_arStore[i];
		if(!pStore) continue;

		if(g_arStore[i]->m_lUsed == 1)
		{
			if(pStore->m_iGuildSid == m_dwGuild)	// ���������
			{
				SendSystemMsg( IDS_USER_CANT_DISMISS_IN_WAR, SYSTEM_ERROR, TO_ME);
				return;
			}

			for(j =0; j < GUILD_ATTACK_MAX_NUM; j++)// ����������..
			{
				if(pStore->m_arAttackGuild[j] == m_dwGuild)
				{
					SendSystemMsg( IDS_USER_CANT_DISMISS_IN_WAR, SYSTEM_ERROR, TO_ME);
					return;
				}
			}
		}
	}
*/
	// Virtual Room�� ������
	for(i = 0; i < g_arGuildHouseWar.GetSize(); i++)
	{
		if(!g_arGuildHouseWar[i]) continue;

		if(g_arGuildHouseWar[i]->m_CurrentGuild.lUsed == 1)
		{
			if(g_arGuildHouseWar[i]->m_CurrentGuild.lGuild == m_dwGuild)
			{
				SendSystemMsg( IDS_USER_CANT_DISMISS_IN_WAR, SYSTEM_ERROR, TO_ME);
				return;
			}
		}
	}

	// Field War�϶�
	if(m_tGuildWar == GUILD_WARRING && m_dwFieldWar > 0)
	{
		SendSystemMsg( IDS_USER_CANT_DISMISS_IN_WAR, SYSTEM_ERROR, TO_ME);
		return;
	}

	int Datalength;
	SQLDATAPACKET *pSDP;

	pSDP = new SQLDATAPACKET;
	pSDP->code = GUILD_DISPERSE;
	Datalength = 0;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pSDP->pData = NULL;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );

}

////////////////////////////////////////////////////////////////////////////////
//	�渶�� ������ �����Ų��. ^^
//
void USER::GuildKickOut(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int index = 0;
	BYTE error_code = 0;

	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	BOOL bRes = TRUE;

	CString strTemp = _T("");

	TCHAR szUserName[255];
	::ZeroMemory(szUserName, sizeof(szUserName));

	if(!m_bGuildMaster && !m_bRepresentationGuild) return;

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) // �߸��� �������̵�
	{
		error_code = ERR_6;
		goto go_result;
	}

	if(m_dwGuild <= 0) { error_code = ERR_8; goto go_result; }

	if(CheckInGuildWarring()) return;			// �����߿��� ����A������.
	
	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)									// ��尡 ������
	{
		ReleaseGuild();		
		error_code = ERR_7;
		goto go_result;
	}
												// ��� �����Ͱ� Ż���ǻ縦 ������ ����...^^			
//	if(strcmp(pGuild->m_strMasterName, szUserName) == 0)
	if( !pGuild->IsMasterPower(m_strUserID) )
	{
		ReleaseGuild();				

		m_bGuildMaster = FALSE;
		m_bRepresentationGuild = FALSE;
		error_code = ERR_10;
		goto go_result;
	}

	if( pGuild->IsMasterPower(szUserName) )
	{
		ReleaseGuild();				
		error_code = ERR_8;
		goto go_result;
	}
	
	index = -1;
	index = pGuild->GetUser(szUserName);			// �ش� �̸��� ������ �ִ���..

	if(index < 0)
	{
		ReleaseGuild();		
		error_code = ERR_8;
		goto go_result;
	}
														// ���� ���â�� �̿����̸� �߹��� ����...
	if(strcmp(pGuild->m_strUsedUser, szUserName) == 0)
	{
		ReleaseGuild();	
		error_code = 255;
		goto go_result;
	}

	// alisia
//	g_pMainDlg->BridgeServerGuildOffReq( m_uid, m_strUserID, szUserName, (int)m_dwGuild );
//	ReleaseGuild();
//	return;



	if(!DeleteGuildUser(szUserName))		// ���̺��� ����
	{
		ReleaseGuild();				// ����...
		error_code = 255;
		goto go_result;
	}



	pGuild->RemoveUser(szUserName);		// �޸𸮻󿡼� ����
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

	DelGuildUserInFortress(szUserName, m_dwGuild);		// �ʱ�ȭ�� �ؾ��Ѵ�.

	pUser = GetUser(szUserName);			// ���� �������� ������...
	if(pUser)
	{
		pUser->m_dwGuild = -1;
		pUser->m_sGuildVersion = -1;		// ��� ���� ����
		::ZeroMemory(pUser->m_strGuildName, sizeof(pUser->m_strGuildName));	
		pUser->m_bGuildMaster = FALSE;			// ��� ������
		pUser->m_bRepresentationGuild = FALSE;	// ���� ���� ����	 	
		pUser->SendMyGuildInfo();
		pUser->SendSystemMsg( IDS_USER_KICKOUT_GUILD, SYSTEM_NORMAL, TO_ME);

		SendGuildOffUser(pUser);
	}

	UpdateUserData();

	strTemp.Format( IDS_USER_KICKOUT_MEMBER, szUserName);
	SendSystemMsg((LPTSTR)(LPCTSTR)strTemp, SYSTEM_NORMAL, TO_ME);
}

void USER::GuildKickOutWithThread(TCHAR *pBuf)
{
	int index = 0;
	BYTE error_code = 0;

	USER *pUser = NULL;
	BOOL bRes = TRUE;

	CString strTemp = _T("");

	TCHAR szUserName[255];
	::ZeroMemory(szUserName, sizeof(szUserName));

	if(!m_bGuildMaster) return;

	int nLength = GetVarString(sizeof(szUserName), szUserName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) // �߸��� �������̵�
	{
		error_code = ERR_6;
		goto go_result;
	}

	if(m_dwGuild <= 0) { error_code = ERR_8; goto go_result; }

	int Datalength;
	SQLDATAPACKET *pSDP;
	BYTE *pData;

	pSDP = new SQLDATAPACKET;
	pSDP->code = GUILD_KICK_OUT;
	Datalength = nLength;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pData = new BYTE[Datalength+1];
	memset(pData, NULL, Datalength+1);
	memcpy(	pData, pBuf, Datalength );
	pSDP->pData = pData;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );

	return;	

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

	pUser = GetUser(szUserName);			// ���� �������� ������...
	if(pUser)
	{
		pUser->m_dwGuild = -1;
		pUser->m_sGuildVersion = -1;		// ��� ���� ����
		::ZeroMemory(pUser->m_strGuildName, sizeof(pUser->m_strGuildName));	
		pUser->m_bGuildMaster = FALSE;			// ��� ������
		pUser->m_bRepresentationGuild = FALSE;	// ���� ���� ����	 	
		pUser->SendMyGuildInfo();
		pUser->SendSystemMsg( IDS_USER_KICKOUT_GUILD, SYSTEM_NORMAL, TO_ME);

		SendGuildOffUser(pUser);
	}

	strTemp.Format( IDS_USER_KICKOUT_MEMBER, szUserName);
	SendSystemMsg((LPTSTR)(LPCTSTR)strTemp, SYSTEM_NORMAL, TO_ME);
}

////////////////////////////////////////////////////////////////////////////////
//	�ش� ���� ��� ������ ������.
//
void USER::SendGuildInfo(USER *pUser)
{
	CBufferEx TempBuf;

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;

	TempBuf.Add(GUILD_INFO);
	TempBuf.Add(pUser->m_uid + USER_BAND);
	TempBuf.Add((int)pUser->m_dwGuild);
	TempBuf.AddString(pUser->m_strGuildName);
	TempBuf.Add((short)pUser->m_sGuildVersion);

	pUser->Send(TempBuf, TempBuf.GetLength());
	SendInsight(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	�� ��� ������ ������.
//
void USER::SendMyGuildInfo()
{
	CBufferEx TempBuf;

	TempBuf.Add(GUILD_INFO);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add((int)m_dwGuild);
	TempBuf.AddString(m_strGuildName);
	TempBuf.Add((short)m_sGuildVersion);

	Send(TempBuf, TempBuf.GetLength());
	SendInsight(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	��� â�� �������� ������ �´�.
//
void USER::GuildWharehouseOpenReq()
{
	int i, j;
	int nLen = 0;
	
	CBufferEx TempBuf;

	CByteArray arItemSlotList;

	USER *pUser = NULL;

	CString guild = _T("");
	guild.Format("%s", m_strGuildName);
	if(guild.IsEmpty()) return;
	if(m_dwGuild <= 0) return;

	CGuild *pGuild = GetGuild(m_dwGuild);

	ReleaseGuild();

	if(!pGuild)
	{
		m_dwGuild = -1;
		return;
	}

	if(InterlockedCompareExchange((LONG*)&g_arGuildData[m_dwGuild]->m_lUsed, (long)1, (long)0) == (long)0)
	{
		m_dwGuildDN = 0;
		InitGuildItem();								// ������ ������ ��Ź�Ѵ�.

		nLen = strlen(m_strUserID);
		if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) 
		{ 
			InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0); 
			return; 
		}

		if(!LoadGuildWarehouse())						// ��� �������� ó�� �����Ҷ� ������ �´�.
		{ 
			InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0); 
			return; 
		}
		
		g_arGuildData[m_dwGuild]->m_iUsedUser = m_uid;
		strncpy(g_arGuildData[m_dwGuild]->m_strUsedUser, m_strUserID, nLen);

		for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
		{												// ���� ������ �����۸� �����ֱ����� �����Ѵ�. 
			if(m_GuildItem[i].sSid >= 0)
			{
				arItemSlotList.Add(i);
			}
		}

		TempBuf.Add(GUILD_WAREHOUSE_OPEN);
		TempBuf.Add((DWORD)m_dwGuildDN);
		TempBuf.Add((BYTE)arItemSlotList.GetSize());

		for(i = 0; i < arItemSlotList.GetSize(); i++)
		{
			BYTE tempSlot = 0;
			tempSlot = arItemSlotList[i];
			TempBuf.Add(tempSlot);
			TempBuf.Add((short)m_GuildItem[tempSlot].sLevel);
			TempBuf.Add((short)m_GuildItem[tempSlot].sSid);
			TempBuf.Add((short)m_GuildItem[tempSlot].sDuration);
			TempBuf.Add((short)m_GuildItem[tempSlot].sBullNum);
			TempBuf.Add((short)m_GuildItem[tempSlot].sCount);

			for(j = 0; j < MAGIC_NUM; j++) TempBuf.Add((BYTE)m_GuildItem[tempSlot].tMagic[j]);

			TempBuf.Add((BYTE)m_GuildItem[tempSlot].tIQ);
		}

		Send(TempBuf, TempBuf.GetLength());
	}
	else 
	{
		CString strMsg = _T("");
		nLen = strlen(g_arGuildData[m_dwGuild]->m_strUsedUser);
		if(nLen <= 0) 
		{
			InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0);
			return;
		}

		if(strcmp(g_arGuildData[m_dwGuild]->m_strUsedUser, _ID(IDS_USER_OPERATOR)) == 0)
		{
			strMsg.Format( IDS_USER_CANT_USE_GUILD_BANK_NOW, g_arGuildData[m_dwGuild]->m_strUsedUser);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			return;
		}

		strMsg.Format( IDS_USER_GUILD_BANK_IN_USE, g_arGuildData[m_dwGuild]->m_strUsedUser);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

		if(nLen > 0)										// ��� â�� �̿��� ������ ����Ǵ� ������ Ȯ���ϴ� ����. 
		{
			pUser = GetUser(g_arGuildData[m_dwGuild]->m_iUsedUser);
			//(g_arGuildData[m_dwGuild]->m_strUsedUser);			// �̿��� ������ ����Ǹ� Ǯ���ش�.	
			if(!pUser || strcmp(pUser->m_strUserID, g_arGuildData[m_dwGuild]->m_strUsedUser) != 0)
			{
				g_arGuildData[m_dwGuild]->m_iUsedUser = -1;
				::ZeroMemory(g_arGuildData[m_dwGuild]->m_strUsedUser, sizeof(g_arGuildData[m_dwGuild]->m_strUsedUser));
				InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0); 
			}
		}
	}
}

void USER::GuildWharehouseOpenReqWithThread()
{
	int Datalength;
	SQLDATAPACKET *pSDP;

	pSDP = new SQLDATAPACKET;
	pSDP->code = DB_GUILD_WHEREHOUSE_EVENT_REQ;
	Datalength = 0;
	pSDP->dcount = Datalength;
	pSDP->UID = m_uid;
	pSDP->pData = NULL;

	EnterCriticalSection( &m_CS_SqlData );
	RecvSqlData.AddTail(pSDP);
	nSqlDataCount = RecvSqlData.GetCount();
	LeaveCriticalSection( &m_CS_SqlData );
}

////////////////////////////////////////////////////////////////////////////////
//	��� â�� �ݴ´�. 
//
void USER::GuildWharehouseCloseReq()
{
	int nLen = 0;
	CString guild = _T("");
	guild.Format("%s", m_strGuildName);
	if(guild.IsEmpty()) return;
	if(m_dwGuild <= 0 || m_dwGuild >= g_arGuildData.GetSize()) return;
	if(!g_arGuildData[m_dwGuild]) return;

	nLen = strlen(m_strUserID);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;
	if(strcmp(g_arGuildData[m_dwGuild]->m_strUsedUser, m_strUserID) != 0) return;

	g_arGuildData[m_dwGuild]->m_iUsedUser = -1;
	::ZeroMemory(g_arGuildData[m_dwGuild]->m_strUsedUser, sizeof(g_arGuildData[m_dwGuild]->m_strUsedUser));
	InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0);
}

////////////////////////////////////////////////////////////////////////////////
//	��� â�� �������� �о�´�.
//
BOOL USER::LoadGuildWarehouse()
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_GUILD_WAREHOUSE (\'%s\')}"), m_strGuildName);

	SQLUINTEGER iDN;
	SQLCHAR		strItem[_BANK_DB];
	
	SQLINTEGER	sInd;

	iDN = 0;
	::ZeroMemory(strItem, sizeof(strItem));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );

	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To Load Guild Warehouse Data !!\n");

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
			SQLGetData( hstmt, i++, SQL_C_ULONG,	&iDN,		sizeof(iDN),		&sInd );
			SQLGetData( hstmt, i++, SQL_C_BINARY,	strItem,	sizeof(strItem),	&sInd );
		}
		else 
		{
			retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}

	retcode = SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	StrToGuildItem((LPTSTR)strItem);
	m_dwGuildDN = iDN;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//	m_GuildItem �� Buffer�� ������ Copy �Ѵ�.
//
void USER::StrToGuildItem(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	MYINT64 serial;

	for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
	{
		m_GuildItem[i].sLevel	= GetShort(pBuf, index);
		m_GuildItem[i].sSid		= GetShort(pBuf, index);
		m_GuildItem[i].sDuration = GetShort(pBuf, index);
		m_GuildItem[i].sBullNum	= GetShort(pBuf, index);
		m_GuildItem[i].sCount	= GetShort(pBuf, index);

		if(m_GuildItem[i].sCount <= 0) m_GuildItem[i].sSid = -1;
		for(j = 0; j < MAGIC_NUM; j++) m_GuildItem[i].tMagic[j] = GetByte(pBuf, index);
		
		m_GuildItem[i].tIQ = GetByte(pBuf, index);

		for( j = 0; j < 8; j++ ) serial.b[j] = GetByte( pBuf, index );

		m_GuildItem[i].iItemSerial = serial.i;
	}	
}

////////////////////////////////////////////////////////////////////////////////
//	Buffer�� m_GuildItem�� ������ Copy �Ѵ�.
//
void USER::GuildItemToStr(TCHAR *pBuf)
{
	int index = 0;
	int i, j;
	MYINT64 serial;

	for(i = 0; i < TOTAL_BANK_ITEM_NUM; i++)
	{
		SetShort(pBuf, m_GuildItem[i].sLevel,	index);
		SetShort(pBuf, m_GuildItem[i].sSid,		index);
		SetShort(pBuf, m_GuildItem[i].sDuration,index);
		SetShort(pBuf, m_GuildItem[i].sBullNum,	index);
		SetShort(pBuf, m_GuildItem[i].sCount,	index);

		for(j = 0; j < MAGIC_NUM; j++) SetByte(pBuf, m_GuildItem[i].tMagic[j], index);

		SetByte(pBuf, m_GuildItem[i].tIQ, index);

		serial.i = m_GuildItem[i].iItemSerial;

		for(j = 0; j < 8; j++ ) SetByte( pBuf, serial.b[j], index );
	}
}

///////////////////////////////////////////////////////////////////////////
//	��� â���� �����, ������ �������� ����Ѵ�.
//
void USER::GuildItemMoveReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
//	if(m_iDisplayType == 6 && m_sLevel > 25) return; //yskang 0.5
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int index = 0;
	BYTE type = GetByte(pBuf, index);

	if(m_dwGuild < 0 || m_dwGuild >= g_arGuildData.GetSize()) return;
	if(!g_arGuildData[m_dwGuild]) return;

	int nLen = strlen(g_arGuildData[m_dwGuild]->m_strUsedUser);

	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;

	if(strcmp(m_strUserID, g_arGuildData[m_dwGuild]->m_strUsedUser) == 0)
	{
		if(g_arGuildData[m_dwGuild]->m_lUsed == 1)
		{
			switch(type)
			{
			case GUILD_ITEM_INPUT:
				GuildInPut(pBuf + index);
				break;
			case GUILD_ITEM_OUTPUT:
				GuildOutPut(pBuf + index);
				break;
			case GUILD_DN_INPUT:
				GuildInPutDN(pBuf + index);
				break;
			case GUILD_DN_OUTPUT:
				GuildOutPutDN(pBuf + index);
				break;
			}
		}
		else 
		{
			g_arGuildData[m_dwGuild]->m_iUsedUser = -1;
			::ZeroMemory(g_arGuildData[m_dwGuild]->m_strUsedUser, sizeof(g_arGuildData[m_dwGuild]->m_strUsedUser));
		}
	}
}

void USER::GuildItemMoveReqWithThread(TCHAR *pBuf)
{
	int index = 0;
	BYTE type = GetByte(pBuf, index);

	if(m_dwGuild < 0) return;
	if(!g_arGuildData[m_dwGuild]) return;

	int nLen = strlen(g_arGuildData[m_dwGuild]->m_strUsedUser);

	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;

	if(strcmp(m_strUserID, g_arGuildData[m_dwGuild]->m_strUsedUser) == 0)
	{
		if(g_arGuildData[m_dwGuild]->m_lUsed == 1)
		{
			int Datalength;
			BYTE *pData;
			SQLDATAPACKET *pSDP;
			pSDP = new SQLDATAPACKET;
			pSDP->code = DB_GUILD_ITEM_MOVE_REQ;

			if ( type == GUILD_ITEM_INPUT || type == GUILD_ITEM_OUTPUT)
				Datalength = sizeof(BYTE)+sizeof(BYTE)+sizeof(short);
			else
				Datalength = sizeof(BYTE)+sizeof(DWORD);

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
		else 
		{
			g_arGuildData[m_dwGuild]->m_iUsedUser = -1;
			::ZeroMemory(g_arGuildData[m_dwGuild]->m_strUsedUser, sizeof(g_arGuildData[m_dwGuild]->m_strUsedUser));
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//	��� â�� �������� �ִ´�.
//
void USER::GuildInPut(TCHAR *pBuf)
{
	int i;
	int tDestSlot;
	int index = 0;
	int iOver = 0;
	short sSid = -1;
	short sHaveCount = 0;

	BYTE result = SUCCESS;

	CBufferEx TempBuf;

	ItemList MyItem[TOTAL_ITEM_NUM], BackItem;

	BYTE tSourceSlot = GetByte(pBuf, index);	// ����� �������� �ִ� ���� ��ġ
	short sCount = GetShort(pBuf, index);		// �󸶸�ŭ ����
												// ������ �κ��� �����ϴ�.			
	if(tSourceSlot < EQUIP_ITEM_NUM || tSourceSlot >= TOTAL_INVEN_MAX) { result = FAIL; goto go_result; }

	sSid = m_UserItem[tSourceSlot].sSid;
	sHaveCount = m_UserItem[tSourceSlot].sCount;
												// �߸��� sSid		
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { result = FAIL; goto go_result; }

	if(g_arItemTable[sSid]->m_sEvent >= EVENT_UNIQUE_ITEM_BAND)
	{
		SendSystemMsg( IDS_USER_CANT_SHARE_EVENT_ITEM, SYSTEM_ERROR, TO_ME);
		result = FAIL; goto go_result; 
	}

	if(sCount >= BANK_MAX_ITEM) 
	{
		SendSystemMsg( IDS_USER_ONCE_MOVE_MAX, SYSTEM_ERROR, TO_ME);
		result = FAIL; goto go_result;
	}
												
	if(sCount <= 0 || sCount > sHaveCount) { result = FAIL; goto go_result; }	// ������ ������ ������ ���� ���			
	if(g_arItemTable[sSid]->m_sDuration > 0 && sCount > 1) { result = FAIL; goto go_result; }
	
	for(i = 0; i < TOTAL_ITEM_NUM; i++)	// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	ReSetItemSlot(&BackItem);					// DB���п� ����� ����� �ʱ�ȭ
												// �ϴ� ��ĥ�� �ִ���, ��ġ�� ���� �������� �ִ��� ã�ƺ���.
	tDestSlot = GetSameItem(m_UserItem[tSourceSlot], GUILD_SLOT);
	
	/***************************��� â�� ���� ó��********************************************/
	if(tDestSlot >= 0)							// �׻� ������ �߰� �Ǵ°��� �������� ó��
	{											
		BackItem = m_GuildItem[tDestSlot];

		if(m_GuildItem[tDestSlot].sCount >= BANK_MAX_ITEM)
		{
			SendSystemMsg( IDS_USER_SAVE_MAX_COUNT, SYSTEM_ERROR, TO_ME);
			result = FAIL; goto go_result;
		}

		if((m_GuildItem[tDestSlot].sCount + sCount) > BANK_MAX_ITEM)
		{										// MAX���� ������ ��ä��� ������ ó��			
			iOver =m_GuildItem[tDestSlot].sCount + sCount - BANK_MAX_ITEM;
			if(iOver <= 0) { result = FAIL; goto go_result; }

			m_GuildItem[tDestSlot].sCount = BANK_MAX_ITEM;
			sCount = sCount - iOver;
		}
		else m_GuildItem[tDestSlot].sCount += sCount;
	}
	else
	{											//	�߰�
		tDestSlot = GetEmptySlot(GUILD_SLOT);

		if(tDestSlot == -1) 
		{
			result = FAIL; goto go_result; 
		}

		m_GuildItem[tDestSlot].sLevel = m_UserItem[tSourceSlot].sLevel;
		m_GuildItem[tDestSlot].sSid = m_UserItem[tSourceSlot].sSid;
		m_GuildItem[tDestSlot].sDuration = m_UserItem[tSourceSlot].sDuration;
		m_GuildItem[tDestSlot].sBullNum = m_UserItem[tSourceSlot].sBullNum;
		m_GuildItem[tDestSlot].sCount = sCount;
		for(i = 0; i < MAGIC_NUM; i++) m_GuildItem[tDestSlot].tMagic[i] = m_UserItem[tSourceSlot].tMagic[i];
		m_GuildItem[tDestSlot].tIQ = m_UserItem[tSourceSlot].tIQ;
		m_GuildItem[tDestSlot].iItemSerial = m_UserItem[tSourceSlot].iItemSerial;
	}

	index = 0;
	index = g_arItemTable[m_UserItem[tSourceSlot].sSid]->m_byWeight * sCount;
	/**************************���� �κ� ó��*********************************************/
	if(sCount >= sHaveCount && iOver == 0)
	{
		MakeItemLog( &m_GuildItem[tDestSlot], ITEMLOG_GUILD_BANKIN );
		ReSetItemSlot(&m_UserItem[tSourceSlot]);	
	}
	else m_UserItem[tSourceSlot].sCount -= sCount;
	
	/**************************DB Update ó��*********************************************/
	if(UpdateGuildWarehouse() == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ����
		{
			m_UserItem[i] = MyItem[i];
		}
		m_GuildItem[tDestSlot] = BackItem;

		result = FAIL;

		FlushItemLog( FALSE );

		goto go_result;
	}

	FlushItemLog( TRUE );

	UpdateUserData();

go_result:
	TempBuf.Add(GUILD_ITEM_MOVE_RESULT);

	if(result == FAIL)
	{
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	m_iCurWeight -= index;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	result = (BYTE)0x01;
	TempBuf.Add(result);

	TempBuf.Add((BYTE)tDestSlot);				// ���	
	TempBuf.Add(m_GuildItem[tDestSlot].sLevel);
	TempBuf.Add(m_GuildItem[tDestSlot].sSid);
	TempBuf.Add(m_GuildItem[tDestSlot].sDuration);
	TempBuf.Add(m_GuildItem[tDestSlot].sBullNum);
	TempBuf.Add(m_GuildItem[tDestSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_GuildItem[tDestSlot].tMagic[i]);
	TempBuf.Add(m_GuildItem[tDestSlot].tIQ);

	TempBuf.Add((BYTE)tSourceSlot);				// ���� �κ�
	TempBuf.Add(m_UserItem[tSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[tSourceSlot].sSid);
	TempBuf.Add(m_UserItem[tSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[tSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[tSourceSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tSourceSlot].tMagic[i]);
	TempBuf.Add(m_UserItem[tSourceSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

//	SendQuickChange();							// �������� ����Ѱ� ��ҵǴ��� �˾ƺ���. 
//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
}


////////////////////////////////////////////////////////////////////////////////
//	��� â���� �������� �����´�.
//
void USER::GuildOutPut(TCHAR *pBuf)
{
	int i, iWeight = 0;
	int tDestSlot;
	int index = 0;
	int iOver = 0;
	short sSid = -1;
	short sHaveCount = 0;

	BYTE result = SUCCESS;

	CBufferEx TempBuf;

	ItemList MyItem[TOTAL_ITEM_NUM], BackItem;

	if(!m_bGuildMaster) 
	{
		SendSystemMsg( IDS_USER_NO_PERMISSION_USE, SYSTEM_NORMAL, TO_ME);
		return;
	}

	BYTE tSourceSlot = GetByte(pBuf, index);	// ����� �������� �ִ� ���� ��ġ
	short sCount = GetShort(pBuf, index);
												// ������ �κ��� �����ϴ�.			
	if(tSourceSlot >= TOTAL_BANK_ITEM_NUM) { result = FAIL; goto go_result; }

	sSid = m_GuildItem[tSourceSlot].sSid;
	sHaveCount = m_GuildItem[tSourceSlot].sCount;

	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) { result = FAIL; goto go_result; }

	if(sCount < 0 || sCount > sHaveCount/* || sCount > BANK_MAX_ITEM*/) { result = FAIL; goto go_result; }

//	if(g_arItemTable[sSid]->m_byWear <= 5 && sCount > 1) { result = FAIL; goto go_result; }
	if(g_arItemTable[sSid]->m_sDuration > 0 && sCount > 1) { result = FAIL; goto go_result; }

	iWeight = g_arItemTable[sSid]->m_byWeight * sCount;
	if(m_iMaxWeight < m_iCurWeight + iWeight)
	{
		SendSystemMsg( IDS_USER_OVER_WEIGHT1, SYSTEM_ERROR, TO_ME);
		result = FAIL; 
		goto go_result;
	}

	for(i = 0; i < TOTAL_ITEM_NUM; i++)	// ������ ���� ���
	{
		MyItem[i] = m_UserItem[i];
	}

	ReSetItemSlot(&BackItem);					// DB���п� ����� ����� �ʱ�ȭ
	BackItem = m_GuildItem[tSourceSlot];

												
	tDestSlot = GetSameItem(m_GuildItem[tSourceSlot], INVENTORY_SLOT);
	
	/***************************���â�� ���� ó��********************************************/
	if(tDestSlot >= 0)							// �׻� ������ �߰� �Ǵ°��� �������� ó��
	{		
		m_UserItem[tDestSlot].sCount += sCount;
	}
	else
	{											//	�߰�
		tDestSlot = GetEmptySlot(INVENTORY_SLOT);

		if(tDestSlot == -1) 
		{
			result = FAIL; goto go_result; 
		}

		m_UserItem[tDestSlot].sLevel = m_GuildItem[tSourceSlot].sLevel;
		m_UserItem[tDestSlot].sSid = m_GuildItem[tSourceSlot].sSid;
		m_UserItem[tDestSlot].sDuration = m_GuildItem[tSourceSlot].sDuration;
		m_UserItem[tDestSlot].sBullNum = m_GuildItem[tSourceSlot].sBullNum;
		m_UserItem[tDestSlot].sCount = sCount;
		for(i = 0; i < MAGIC_NUM; i++) m_UserItem[tDestSlot].tMagic[i] = m_GuildItem[tSourceSlot].tMagic[i];
		m_UserItem[tDestSlot].tIQ = m_GuildItem[tSourceSlot].tIQ;
		m_UserItem[tDestSlot].iItemSerial = m_GuildItem[tSourceSlot].iItemSerial;

		MakeItemLog( &m_UserItem[tDestSlot], ITEMLOG_GUILD_BANKOUT );
	}

	/**************************���  ó��*********************************************/
	if(sCount >= sHaveCount) ReSetItemSlot(&m_GuildItem[tSourceSlot]);	
	else m_GuildItem[tSourceSlot].sCount -= sCount;
	
	/**************************DB Update ó��*********************************************/
	if(UpdateGuildWarehouse() == FALSE)
	{
		for(i = 0; i < TOTAL_ITEM_NUM; i++)// ������ ���� ����
		{
			m_UserItem[i] = MyItem[i];
		}
		m_GuildItem[tSourceSlot] = BackItem;

		result = FAIL;

		FlushItemLog( FALSE );

		goto go_result;
	}

	FlushItemLog( TRUE );

	UpdateUserData();

go_result:
	TempBuf.Add(GUILD_ITEM_MOVE_RESULT);

	if(result == FAIL)
	{
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	result = (BYTE)0x02;
	TempBuf.Add(result);

	TempBuf.Add((BYTE)tSourceSlot);
	TempBuf.Add(m_GuildItem[tSourceSlot].sLevel);
	TempBuf.Add(m_GuildItem[tSourceSlot].sSid);
	TempBuf.Add(m_GuildItem[tSourceSlot].sDuration);
	TempBuf.Add(m_GuildItem[tSourceSlot].sBullNum);
	TempBuf.Add(m_GuildItem[tSourceSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_GuildItem[tSourceSlot].tMagic[i]);
	TempBuf.Add(m_GuildItem[tSourceSlot].tIQ);

	TempBuf.Add((BYTE)tDestSlot);
	TempBuf.Add(m_UserItem[tDestSlot].sLevel);
	TempBuf.Add(m_UserItem[tDestSlot].sSid);
	TempBuf.Add(m_UserItem[tDestSlot].sDuration);
	TempBuf.Add(m_UserItem[tDestSlot].sBullNum);
	TempBuf.Add(m_UserItem[tDestSlot].sCount);
	for(i = 0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tDestSlot].tMagic[i]);
	TempBuf.Add(m_UserItem[tDestSlot].tIQ);

	Send(TempBuf, TempBuf.GetLength());

	if(m_UserItem[tDestSlot].sSid < 0 || m_UserItem[tDestSlot].sSid >= g_arItemTable.GetSize()) return;

	m_iCurWeight += (g_arItemTable[m_UserItem[tDestSlot].sSid]->m_byWeight * sCount);
	GetRecoverySpeed();							// ȸ���ӵ� üũ...
//	SendItemWeightChange();				// ���� ������ ���Ը� ������.
}

////////////////////////////////////////////////////////////////////////////////
//	��� â�� ���� �ִ´�.
//
void USER::GuildInPutDN(TCHAR *pBuf)
{
	CBufferEx TempBuf;

	BYTE result; 
	int index = 0;
	DWORD BackGuildDN = 0, BackMyDN = 0;

	DWORD InputDN = GetDWORD(pBuf, index);

	TempBuf.Add(GUILD_ITEM_MOVE_RESULT);

	if(InputDN > m_dwDN) 
	{
		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}
	
	BackMyDN = m_dwDN;
	BackGuildDN = m_dwGuildDN;
										// ���࿡ �Ա�
	if(!CheckMaxValueReturn(m_dwGuildDN, InputDN))
	{									// ��, MAX���̸� ������...
		CheckMaxValue(m_dwGuildDN, InputDN);
		if(m_dwGuildDN < InputDN) InputDN = 0;
		else InputDN = m_dwGuildDN - InputDN;
	}
	else m_dwGuildDN += InputDN;
										// ������ �ִ� �����ݿ��� ����.
	if(m_dwDN <= InputDN) m_dwDN = 0;
	else m_dwDN -= InputDN;

	MakeMoneyLog( InputDN, ITEMLOG_GUILD_BANKIN_MONEY, NULL, m_dwGuildDN );

	if(UpdateGuildWarehouse() == FALSE)		// DB UpDate
	{
		m_dwDN = BackMyDN;
		m_dwGuildDN = BackGuildDN;

		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		FlushItemLog( FALSE );
		return;
	}

	FlushItemLog( TRUE );

	UpdateUserData();

	result = (BYTE)0x03;				// 3 : DN �Ա�
	TempBuf.Add(result);

	TempBuf.Add(m_dwGuildDN);			// ���೻ �Ա��� �ѱݾ�
	TempBuf.Add(m_dwDN);				// ������ �ѱݾ�

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	��� â���� ���� �����´�.
//
void USER::GuildOutPutDN(TCHAR *pBuf)
{
	CBufferEx TempBuf;

	BYTE result; 
	int index = 0;

	if(!m_bGuildMaster) 
	{
		SendSystemMsg( IDS_USER_NO_PERMISSION_USE, SYSTEM_NORMAL, TO_ME);
		return;
	}

	DWORD BackGuildDN = 0, BackMyDN = 0;

	DWORD OutputDN = GetDWORD(pBuf, index);

	TempBuf.Add(GUILD_ITEM_MOVE_RESULT);

	if(OutputDN > m_dwGuildDN)			// �ʹ� ũ��
	{
		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	BackMyDN = m_dwDN;					// ���
	BackGuildDN = m_dwGuildDN;
										// ����� �����ݿ� �ջ��Ѵ�.
	if(!CheckMaxValueReturn(m_dwDN, OutputDN))
	{									// ��, MAX���̸� ������...
		CheckMaxValue(m_dwDN, OutputDN);
		if(m_dwDN < OutputDN) OutputDN = 0;
		else OutputDN = m_dwDN - OutputDN;
	}
	else m_dwDN += OutputDN;
										// ��忡�� ����.
	if(OutputDN == m_dwGuildDN) m_dwGuildDN = 0;
	else m_dwGuildDN -= OutputDN;

	MakeMoneyLog( OutputDN, ITEMLOG_GUILD_BANKOUT_MONEY, NULL, m_dwGuildDN );

	if(UpdateGuildWarehouse() == FALSE)		// DB UpDate
	{
		m_dwDN = BackMyDN;
		m_dwGuildDN = BackGuildDN;

		result = FAIL;
		TempBuf.Add(result);
		Send(TempBuf, TempBuf.GetLength());

		FlushItemLog( FALSE );
		return;
	}

	FlushItemLog( TRUE );

	UpdateUserData();

	result = (BYTE)0x04;				// 4 : DN ���
	TempBuf.Add(result);

	TempBuf.Add(m_dwGuildDN);			// ��峻 �Ա��� �ѱݾ�
	TempBuf.Add(m_dwDN);				// ������ �ѱݾ�

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	��� â�� ��ȭ�� �����Ѵ�.
//
BOOL USER::UpdateGuildWarehouse()
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strItem[_ITEM_DB], strGuildItem[_BANK_DB], strQuickItem[_QUICKITEM_DB];
	int				i;

	SQLSMALLINT	sRet = 0;
	SQLINTEGER sRetInd;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strGuildItem, sizeof(strGuildItem));
	::ZeroMemory(strItem, sizeof(strItem));
	::ZeroMemory(strQuickItem, sizeof(strQuickItem));

	GuildItemToStr(strGuildItem);
	UserItemToStr(strItem);

	SDWORD sGuildItemLen= sizeof(strGuildItem);
	SDWORD sItemLen		= sizeof(strItem);
	SDWORD sQuickLen	= sizeof(strQuickItem);

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_GUILD_WAREHOUSE_DATA (\'%s\', \'%s\', %d, ?, ?, %d, ?, ?)}"), m_strUserID, m_strGuildName, m_dwDN, m_dwGuildDN);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild Warehouse Data !!\n");

		//g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}
	
	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strItem),	0, (TCHAR*)strItem,		0, &sItemLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strQuickItem),	0, (TCHAR*)strQuickItem,	0, &sQuickLen );
		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strGuildItem),	0, (TCHAR*)strGuildItem,0, &sGuildItemLen );
		SQLBindParameter( hstmt, i++ ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &sRetInd);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if(sRet == 255) return FALSE;

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	E_Body ������ ����.
//
void USER::SendEBodyOpen(int nStore)
{
/*
	CBufferEx TempBuf;
	int i = 0;
	short sid = 0;
	short sCount = 0;
	DWORD dwCost = 0, dwXP = 0;

	CStore* pStore = NULL;

	pStore = GetEbodyStore(nStore);
	if(pStore == NULL) return;

	sCount = pStore->m_arItems.GetSize();
	TempBuf.Add(EBODY_OPEN);
	TempBuf.Add((short)nStore);
	TempBuf.Add(sCount);

	for(i = 0; i < pStore->m_arItems.GetSize(); i++)
	{
		sid = (short)pStore->m_arItems[i];

		dwCost = g_arEBodyTable[sid]->m_iDN;
		dwXP = g_arEBodyTable[sid]->m_iXP;

		TempBuf.Add(sid);
		TempBuf.Add(dwXP);
		TempBuf.Add(dwCost);
	}

	Send(TempBuf, TempBuf.GetLength());
*/
}

////////////////////////////////////////////////////////////////////////////////
//	E_Body ������ �Ѵ�.
//
void USER::EBodyBuyReq(TCHAR *pBuf)
{
/*
	return;			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	int index = 0;
	short sSid = 0;
	short sStoreID = 0;			// ���� ���̵�

	BOOL bSell = FALSE;	

	CBufferEx TempBuf;

	BYTE tNeedClass;
	BYTE tMyClass;
	BYTE tTemp = 1;	
	BYTE tFire = 0;
	BYTE tEdge = 0;
	BYTE tStaff = 0;
	BYTE tBrawl = 0;

	CStore* pStore = NULL;

	sStoreID = GetShort(pBuf, index);

	pStore = GetEbodyStore(sStoreID);
	if(pStore == NULL) return;//{ tResult = FAIL; goto go_result; }

	sSid = GetShort(pBuf, index);

	if(sSid < 0 || sSid >= g_arEBodyTable.GetSize()) return;

	for(int i = 0; i < pStore->m_arItems.GetSize(); i++)
	{
		if(pStore->m_arItems[i] == sSid) { bSell = TRUE; break; }
	}

	if(!bSell) return;

	tNeedClass = g_arEBodyTable[sSid]->m_tClass;
	tMyClass = m_byClass;

	tFire	 = tTemp & tNeedClass; tTemp = 2; 
	tEdge	 = tTemp & tNeedClass; tTemp = 4;
	tStaff	 = tTemp & tNeedClass; tTemp = 8;
	tBrawl	 = tTemp & tNeedClass;

	tFire = tFire & tMyClass;
	tEdge = tEdge & tMyClass;
	tStaff = tStaff & tMyClass;
	tBrawl = tBrawl & tMyClass;

	tTemp = tFire^tEdge^tStaff^tBrawl;
	if(!tTemp) return;

	if(m_dwXP < g_arEBodyTable[sSid]->m_iXP) return;

	if(m_dwDN < g_arEBodyTable[sSid]->m_iDN) return;

	if(m_sSTR + g_arEBodyTable[sSid]->m_sAddBasic[0] < m_sTempSTR) return;
	if(m_sCON + g_arEBodyTable[sSid]->m_sAddBasic[1] < m_sTempCON) return;
	if(m_sDEX + g_arEBodyTable[sSid]->m_sAddBasic[2] < m_sTempDEX) return;
	if(m_sVOL + g_arEBodyTable[sSid]->m_sAddBasic[3] < m_sTempVOL) return;
	if(m_sWIS + g_arEBodyTable[sSid]->m_sAddBasic[4] < m_sTempWIS) return;

	if(g_arEBodyTable[sSid]->m_sAddBasic[0] != 0)
	{
		m_sSTR = m_sSTR + g_arEBodyTable[sSid]->m_sAddBasic[0];
	}

	if(g_arEBodyTable[sSid]->m_sAddBasic[1] != 0)
	{
		m_sCON = m_sCON + g_arEBodyTable[sSid]->m_sAddBasic[1];
	}

	if(g_arEBodyTable[sSid]->m_sAddBasic[2] != 0)
	{
		m_sDEX = m_sDEX + g_arEBodyTable[sSid]->m_sAddBasic[2];
	}

	if(g_arEBodyTable[sSid]->m_sAddBasic[3] != 0)
	{
		m_sVOL = m_sVOL + g_arEBodyTable[sSid]->m_sAddBasic[3];
	}

	if(g_arEBodyTable[sSid]->m_sAddBasic[4] != 0)
	{
		m_sWIS = m_sWIS + g_arEBodyTable[sSid]->m_sAddBasic[4];
	}

	SetUserToMagicUser();
	SendUserStatusSkill();

	TempBuf.Add(EBODY_BUY_RESULT);
	TempBuf.Add((BYTE)0x01);
	TempBuf.Add(sSid);

	Send(TempBuf, TempBuf.GetLength());
*/
}


////////////////////////////////////////////////////////////////////////////////
//	���� �������� ���� ������ ������.
//
void USER::GuildUserInfoReq(TCHAR *pBuf)
{
	int i;
	int nCount = 0; 
	int nLength = 0;
	USER *pUser = NULL;
	CGuild *pGuild = NULL;
//	CGuildUser *pGuildUser = NULL;

	TCHAR GuildMaster[CHAR_NAME_LENGTH + 1];						// �渶 �̸�

	CBufferEx TempBuf, TempBufData; 
//	CWordArray	arLevel;

	if(m_dwGuild <= 0) return;										// ����ȣ�� ������ ������ �ƴϴ�.
	
	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)														// �ش� ��尡 �������� �ʴ´�.	
	{
		m_dwGuild = -1;
		ReleaseGuild();												// ����...
		return;
	}

	nCount = -1;
	nCount = pGuild->GetUser(m_strUserID);
	if(nCount < 0)													// ������ �ƴϸ�	
	{
		m_dwGuild = -1;												// �ϴ� ��� �ε����� �ʱ�ȭ
		ReleaseGuild();												// ����...
		return;
	}
/*
	nCount = pGuild->m_arUser.GetSize();

	if(nCount <= 0 || nCount > MAX_GUILD_USER)						// ����ο����� �ִ�ġ�� �Ѿ���.
	{
		ReleaseGuild();												// ����...
		return;
	}
*/
	nLength = strlen(pGuild->m_strMasterName);
	::ZeroMemory(GuildMaster, sizeof(GuildMaster));
	if(nLength > 0 && nLength <= CHAR_NAME_LENGTH) strncpy(GuildMaster, pGuild->m_strMasterName, nLength);
	else															// �ش� ��忡 �渶�� �������� �ʴ´�.	
	{
		m_dwGuild = -1;
		ReleaseGuild();												// ����...
		return;
	}

	ReleaseGuild();													// ����...

	nCount = 0;
	for(i = 0; i< MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if(m_dwGuild == pUser->m_dwGuild)
		{
			TempBufData.AddString(pUser->m_strUserID);
			TempBufData.Add(pUser->m_sLevel);
			nCount++;
		}
	}

	TempBuf.Add(GUILD_USER_INFO_RESULT);
	//TempBuf.Add((BYTE)0x00);										// �Ϲ� ��� ������ �䱸
	TempBuf.Add(m_nGuildUserInfoType);								//yskang 0.2
	TempBuf.AddString(GuildMaster);									// ��� ������ �̸��� ����
	TempBuf.Add((BYTE)nCount);										// ���� ���� ������
	TempBuf.AddData(TempBufData, TempBufData.GetLength());
	m_nGuildUserInfoType = 0x00;
	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////
//	���¯�� ��ü ���� ������ �䱸.
//
void USER::GuildUserAllInforChat()
{
	int i;
	int nCount = 0; 
	int nLength = 0;
	USER *pUser = NULL;
	CGuild *pGuild = NULL;
//	CGuildUser *pGuildUser = NULL;

	CBufferEx TempBuf, TempBufData; 
//	CWordArray	arLevel;

	if(m_dwGuild <= 0) return;										// ����ȣ�� ������ ������ �ƴϴ�.
	if(!m_bGuildMaster) return;										// ����ȣ�� ������ ������ �ƴϴ�.
	
	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)														// �ش� ��尡 �������� �ʴ´�.	
	{
		m_dwGuild = 0;
		ReleaseGuild();												// ����...
		return;
	}
/*	
	nCount = pGuild->m_arUser.GetSize();

	if(nCount <= 0 || nCount > MAX_GUILD_USER)						// ����ο����� �ִ�ġ�� �Ѿ���.
	{
		ReleaseGuild();												// ����...
		return;
	}
*/
	for(i = 0; i < MAX_GUILD_USER; i++)
	{
		if(pGuild->m_arUser[i].m_lUsed != 0)
		{
			TempBufData.AddString(pGuild->m_arUser[i].m_strUserId);
			TempBufData.Add((short)0x00);
			nCount++;
		}
	}

	ReleaseGuild();	// ����...
/*
	nCount = 0;
	for(i = 0; i< MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if(m_dwGuild == pUser->m_dwGuild)
		{
			TempBufData.AddString(pUser->m_strUserID);
			TempBufData.Add(pUser->m_sLevel);
			nCount++;
		}
	}
*/
	TempBuf.Add(GUILD_USER_INFO_RESULT);
	TempBuf.Add((BYTE)0x00);										// �Ϲ� ��� ������ �䱸
	TempBuf.AddString(m_strUserID);									// ��� ������ �̸��� ����
	TempBuf.Add((BYTE)nCount);										// ���� ���� ������
	TempBuf.AddData(TempBufData, TempBufData.GetLength());
	
	Send(TempBuf, TempBuf.GetLength());
}
/*
////////////////////////////////////////////////////////////////////////////////
//	���¯�� ��ü ���� ������ �䱸.
//
void USER::GuildUserAllInforChat()
{
	int i;
	int nCount = 0; 
	int nLength = 0;
	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	CBufferEx TempBuf, TempBufData; 
	CWordArray	arLevel;

	if(m_dwGuild <= 0) return;										// ����ȣ�� ������ ������ �ƴϴ�.
	if(!m_bGuildMaster) return;										// ����ȣ�� ������ ������ �ƴϴ�.
	
	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)														// �ش� ��尡 �������� �ʴ´�.	
	{
		m_dwGuild = 0;
		ReleaseGuild();												// ����...
		return;
	}
	
	nCount = pGuild->m_arUser.GetSize();

	if(nCount <= 0 || nCount > MAX_GUILD_USER)						// ����ο����� �ִ�ġ�� �Ѿ���.
	{
		ReleaseGuild();												// ����...
		return;
	}

	ReleaseGuild();	// ����...

	nCount = 0;

	for(i = 0; i< MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if(m_dwGuild == pUser->m_dwGuild)
		{
			TempBufData.AddString(pUser->m_strUserID);
			TempBufData.Add(pUser->m_sLevel);
			nCount++;
		}
	}

	TempBuf.Add(GUILD_USER_INFO_RESULT);
	TempBuf.Add((BYTE)0x00);										// �Ϲ� ��� ������ �䱸
	TempBuf.AddString(m_strUserID);									// ��� ������ �̸��� ����
	TempBuf.Add((BYTE)nCount);										// ���� ���� ������
	TempBuf.AddData(TempBufData, TempBufData.GetLength());
	
	Send(TempBuf, TempBuf.GetLength());
}
*/
////////////////////////////////////////////////////////////////////////////////
//	��� �Ͽ콺�о翡 �����ϱ����� ���� ���� ���� ���� ������ ������.
//
void USER::SendGuildUserInfo(int zone)
{
	int i, index = -1;
	int nCount = 0; 
	int nLength = 0;

	BOOL bSame = FALSE;

	USER *pUser = NULL;
	CGuild *pGuild = NULL;
//	CGuildUser *pGuildUser = NULL;
	CGuildUser *pGuildUser = NULL;
	CGuildFortress *pFort = NULL;

	TCHAR GuildMaster[CHAR_NAME_LENGTH + 1];						// �渶 �̸�

	CBufferEx TempBuf, TempBufData; 
//	CDWordArray	arUid;
	
//	arUid.RemoveAll();

	if(m_dwGuild <= 0) return;										// ����ȣ�� ������ ������ �ƴϴ�.
	if(!m_bGuildMaster) return;										// ������ �渶���� ��û�Ҽ� �ִ�.	

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i]) continue;

		pFort = g_arGuildFortress[i];

		if(pFort->m_iGuildSid == m_dwGuild) 
		{
			SendSystemMsg( IDS_USER_CANT_APPLY_HAVE_FORTRESS, SYSTEM_NORMAL, TO_ME);
			return;
		}
	}

	i = GetCityNumForVirtualRoom(zone);
	if(i < 0 || i >= g_arGuildHouseWar.GetSize()) return;

	if(g_arGuildHouseWar[i]->m_CurrentGuild.lUsed == 1) 
	{		
		SendSystemMsg( IDS_USER_ALREADY_OTHER_GUILD_USE, SYSTEM_ERROR, TO_ME);
		return;
	}

	for(i = 0; i < g_arGuildHouseWar.GetSize(); i++)
	{
		if(g_arGuildHouseWar[i]->m_CurrentGuild.iWarZone == m_curz) { bSame = TRUE; break; }
	}

	if(bSame) return;

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)														// �ش� ��尡 �������� �ʴ´�.	
	{
		m_dwGuild = -1;
		ReleaseGuild();												// ����...
		return;
	}

	index = pGuild->GetUser(m_strUserID);

	if(index < 0)														// ������ �ƴϸ�	
	{
		m_dwGuild = -1;												// �ϴ� ��� �ε����� �ʱ�ȭ
		ReleaseGuild();												// ����...
		return;
	}

/*	nCount = pGuild->m_arUser.GetSize();

	if(nCount <= 0 || nCount > MAX_GUILD_USER)						// ����ο����� �ִ�ġ�� �Ѿ���.
	{
		ReleaseGuild();												// ����...
		return;
	}
*/
	nLength = strlen(pGuild->m_strMasterName);
	::ZeroMemory(GuildMaster, sizeof(GuildMaster));
	if(nLength > 0 && nLength <= CHAR_NAME_LENGTH) strncpy(GuildMaster, pGuild->m_strMasterName, nLength);
	else															// �ش� ��忡 �渶�� �������� �ʴ´�.	
	{
		m_dwGuild = -1;
		ReleaseGuild();												// ����...
		return;
	}

	ReleaseGuild();													// ����...

	nCount = 0;
	for(i = 0; i< MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if(m_dwGuild == pUser->m_dwGuild)
		{
			TempBufData.AddString(pUser->m_strUserID);
			TempBufData.Add(pUser->m_sLevel);
			TempBufData.Add(pUser->m_uid + USER_BAND);
			nCount++;
		}
	}

	TempBuf.Add(GUILD_USER_INFO_RESULT);
	TempBuf.Add((BYTE)0x01);										// �Ϲ� ��� ������ �䱸
	TempBuf.AddString(m_strUserID);									// ��� ������ �̸��� ����
	TempBuf.Add((BYTE)nCount);										// ���� ���� ������
	TempBuf.AddData(TempBufData, TempBufData.GetLength());
	TempBuf.Add((short)zone);
	
	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////////////
// ���������� �޸� DB�� �����Ѵ� (����� LoadUserBank������ �θ���.)
//
void USER::SetMemUserBank(LPCTSTR strItem)
{
	if(m_pSharedMemory->m_hMapping == NULL) return;
	if(m_pMD == NULL) return;
	if(m_pMD->m_uid == -1) return;

	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return;

	m_pMD->m_UB.m_uid = m_uid;
	strcpy(m_pMD->m_UB.m_strUserID, m_strUserID);
	//strncpy(m_pMD->m_UB.m_UserBankItem, strItem, _BANK_DB);
	memcpy(m_pMD->m_UB.m_UserBankItem, strItem, _BANK_DB);
	m_pMD->m_UB.m_dwBankDN = m_dwBankDN;
}

void USER::SetMemAccountBank(LPCTSTR strItem)
{
	if(m_pSharedMemory->m_hMapping == NULL) return;
	if(m_pMD == NULL) return;
	if(m_pMD->m_uid == -1) return;

	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return;

	m_pMD->m_AB.m_uid = m_uid;
	strcpy(m_pMD->m_AB.m_strAccount, m_strAccount);
	memcpy(m_pMD->m_AB.m_AccountBankItem, strItem, _ACCOUNT_BANK_DB);
	m_pMD->m_AB.m_dwBankDN = m_dwAccountBankDN;
}

////////////////////////////////////////////////////////////////////////////////////////
//	�޸𸮵�� �������������� ������ �����´�.
//
BOOL USER::LoadMemUserBank()
{
	if(m_pSharedMemory->m_hMapping == NULL) return FALSE;
	if(m_pMD == NULL) return FALSE;
	if(m_pMD->m_uid == -1) return FALSE;
	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return FALSE;
	if(m_pMD->m_UB.m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_UB.m_strUserID) != 0) return FALSE;

	StrToUserBankItem((LPTSTR)m_pMD->m_UB.m_UserBankItem);
	m_dwBankDN = m_pMD->m_UB.m_dwBankDN;

	return TRUE;
}

BOOL USER::LoadMemAccountBank()
{
	if(m_pSharedMemory->m_hMapping == NULL) return FALSE;
	if(m_pMD == NULL) return FALSE;
	if(m_pMD->m_uid == -1) return FALSE;
	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return FALSE;

	if(m_pMD->m_AB.m_uid != m_uid || _stricmp(m_strAccount, m_pMD->m_AB.m_strAccount) != 0) return FALSE;

	StrToAccountBankItem((LPTSTR)m_pMD->m_AB.m_AccountBankItem);
	m_dwAccountBankDN = m_pMD->m_AB.m_dwBankDN;

	return TRUE;
}

BOOL USER::UpdateMemBankDataOnly()
{
	if(m_pSharedMemory->m_hMapping == NULL) return FALSE;
	if(m_pMD == NULL) return FALSE;
	if(m_pMD->m_uid == -1) return FALSE;
	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return FALSE;
	if(m_pMD->m_UB.m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_UB.m_strUserID) != 0) return FALSE;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strBankItem[_BANK_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strBankItem, sizeof(strBankItem));
	
	::CopyMemory(strBankItem, m_pMD->m_UB.m_UserBankItem, _BANK_DB);
	
	SDWORD sBankItemLen	= sizeof(strBankItem);
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_BANK_DATA_ONLY (\'%s\',%d, ?)}"), m_strUserID, m_pMD->m_UB.m_dwBankDN);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Mem User Bank Data Only!!\n");

		//g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strBankItem), 0, (TCHAR*)strBankItem, 0, &sBankItemLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	m_pMD->m_UB.m_uid = -1;

	return TRUE;
}

BOOL USER::UpdateMemAccountBankDataOnly()
{
	if(m_pSharedMemory->m_hMapping == NULL) return FALSE;
	if(m_pMD == NULL) return FALSE;
	if(m_pMD->m_uid == -1) return FALSE;
	if(m_pMD->m_uid != m_uid || _stricmp(m_strUserID, m_pMD->m_strUserID) != 0) return FALSE;

	if(m_pMD->m_AB.m_uid != m_uid || _stricmp(m_strAccount, m_pMD->m_AB.m_strAccount) != 0) return FALSE;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strBankItem[_ACCOUNT_BANK_DB];
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strBankItem, sizeof(strBankItem));
	
	::CopyMemory(strBankItem, m_pMD->m_AB.m_AccountBankItem, _ACCOUNT_BANK_DB);
	
	SDWORD sBankItemLen	= sizeof(strBankItem);
	
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_ACCOUNT_BANK_DATA_ONLY (\'%s\',%d, ?)}")/*, m_pMD->m_iMyServer*/, m_pMD->m_strAccount, m_pMD->m_AB.m_dwBankDN);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );

	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Mem User Bank Data Only!!\n");

		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strBankItem), 0, (TCHAR*)strBankItem, 0, &sBankItemLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

			g_DB[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	m_pMD->m_AB.m_uid = -1;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
//	�������� ä��
//
void USER::GuildChat(TCHAR *pBuf)
{
	int i;

	USER *pUser = NULL;

	CBufferEx TempBuf;

	if(m_dwGuild <= 0) return;

	// alisia
	g_pMainDlg->BridgeServerUserGuildChatReq( m_uid, (int)m_dwGuild, m_strUserID, pBuf+1 );
	return;
	//

	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(GUILD_CHAT);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_strUserID);
	TempBuf.AddString(pBuf + 1);

	for(i = 0; i< MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if(m_dwGuild == pUser->m_dwGuild) pUser->Send(TempBuf, TempBuf.GetLength());
	}
}


void USER::SendStoreTax(int nStore)
{
	CBufferEx TempBuf;

	int nLen = 0;
	short sTax = 0;

	CStore* pStore = NULL;
	CGuildFortress* pFort = NULL;

	if(!m_bGuildMaster) 
	{
		SendSystemMsg( IDS_USER_ONLY_USE_GUILD_MASTER, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(nStore >= FORTRESS_BAND)						// ����� ���� �����̸�...
	{
		pFort = GetFortress(nStore);
		if(pFort == NULL) return;

		if(pFort->m_iGuildSid != m_dwGuild) return;

		SYSTEMTIME st;
		SYSTEMTIME guildTime;

		GetLocalTime(&guildTime);
		st = pFort->m_wLastWarTime;
		COleDateTime LastTime(st.wYear, st.wMonth, st.wDay, st.wHour, 0, 0);
		COleDateTime CurrTime = COleDateTime(guildTime);

		if(CurrTime.m_status == COleDateTime::valid && LastTime.m_status == COleDateTime::valid)
		{
/*			CTime curr(CurrTime.GetYear(), CurrTime.GetMonth(), CurrTime.GetDay(), CurrTime.GetHour(), 0, 0);
			CTime last(LastTime.GetYear(), LastTime.GetMonth(), LastTime.GetDay(), LastTime.GetHour(), 0, 0);
			CTimeSpan DiffTime = curr - last;	
			
			int nTime = DiffTime.GetHours();
		
			if(nTime < UPDATE_GUILD_INVEN_TIME)
			{
				SendSystemMsg( IDS_USER_TEXT_CONTROL, SYSTEM_ERROR, TO_ME);
				return;
			}
*/
			sTax = pFort->m_tTaxRate;
		}
	}
	else
	{
		pStore = GetStore(nStore);
		if(pStore == NULL) return;

		sTax = pStore->m_sRate;
	}

	TempBuf.Add(GUILD_TAX_OPEN);
	TempBuf.Add((short)nStore);
	TempBuf.Add((short)sTax);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendGuildWarScheme(int nStore)
{
	int nChatNum = -1;

	CBufferEx TempBuf;

	CStore* pStore = NULL;
	CGuildFortress* pFort = NULL;	

	if(nStore >= FORTRESS_BAND)						// ����� ���� �����̸�...
	{
		pFort = GetFortress(nStore);
		if(pFort == NULL) return;
		
		if(pFort->m_tWarType == GUILD_WAR_DECISION)
		{
			if(pFort->m_sFortressID == 1000) nChatNum = 144;
			else if(pFort->m_sFortressID == 1001) nChatNum = 164;
			else if(pFort->m_sFortressID == 1002) nChatNum = 317;

			EditHyperFortressText(pFort, nChatNum);
			return;
		}		
	}
	else
	{
		pStore = GetStore(nStore);
		if(pStore == NULL) return;

		if(pStore->m_tWarType == GUILD_WAR_DECISION) 
		{
			if(pStore->m_sStoreID == 4) nChatNum = 34;		// �̹� ������ ���� ������ client�� �������ʴ´�.
			else if(pStore->m_sStoreID == 5) nChatNum = 44;
			else if(pStore->m_sStoreID == 6) nChatNum = 54;

			EditHyperText(pStore, nChatNum);
			return;
		}
	}

	if(!m_bGuildMaster)
	{
		SendSystemMsg( IDS_USER_ONLY_USE_GUILD_MASTER, SYSTEM_ERROR, TO_ME);
		return;
	}

	SYSTEMTIME temp;
	GetLocalTime(&temp);

	TempBuf.Add(GUILD_WAR_OPEN);
	TempBuf.Add((short)nStore);

	TempBuf.Add((short)temp.wYear);
	TempBuf.Add((BYTE)temp.wMonth);
	TempBuf.Add((BYTE)temp.wDay);

	Send(TempBuf, TempBuf.GetLength());
}

////////////////////////////////////////////////////////////////////////////////////////
//	�ش� �渶�� ������ ������ �����Ѵ�.
//
void USER::GetStoreTax(TCHAR *pBuf)
{
	int index = 0;
	CStore* pStore = NULL;

	short nStore = GetShort(pBuf, index);
	short nTax = GetShort(pBuf, index);

	if(nTax < 0 || nTax > GUILD_MAX_TAX) return;		// �ִ� 200 % ����

	SetGuildStoreTex(nStore, nTax);
}

////////////////////////////////////////////////////////////////////////////////////////
//	�ش� ������ ������ �����Ѵ�.
//
void USER::SetGuildStoreTex(int iSid, int iRate)
{
	int nChatNum = -1;

	CBufferEx TempBuf;

	CStore* pStore = NULL;
	CGuildFortress* pFort = NULL;

	if(m_dwGuild <= 0 || !m_bGuildMaster)
	{
		SendSystemMsg( IDS_USER_ONLY_USE_GUILD_MASTER, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(iSid >= FORTRESS_BAND)						// ����� ���� �����̸�...
	{
		pFort = GetFortress(iSid);
		if(pFort == NULL) return;

		if(pFort->m_lUsed == 1)
		{
			SendSystemMsg( IDS_USER_CANT_USE_IN_DEFENCE, SYSTEM_ERROR, TO_ME);
			return;
		}

		if(pFort->m_iGuildSid == m_dwGuild)
		{
			if(iRate > 30) 
			{
				SendSystemMsg( IDS_USER_TEX_MAX, SYSTEM_ERROR, TO_ME);
				return;
			}

			pFort->m_tTaxRate = iRate;
			pFort->ApplyTaxToStore();				// �� ������ ������ ����.
			UpdateMemStoreTax(iSid, iRate);
			return;
		}
	}
/*	else
	{
		pStore = GetStore(iSid);
		if(pStore == NULL) return;

		if(pStore->m_sStoreID == (short)iSid)
		{
			pStore->m_sRate = iRate;

			UpdateMemStoreTax(iSid, iRate);
			return;
		}
	}
*/
}

void USER::GetGuildWarScheme(TCHAR *pBuf)
{
	int index = 0;

	short nStore = GetShort(pBuf, index);

	if(nStore < 0) return;

	if(nStore >= FORTRESS_BAND)
	{
		SetFortressGuildWarScheme(pBuf + index, nStore);
	}
/*	else 
	{
		SetStoreGuildWarScheme(pBuf + index, nStore);
	}
*/
}

////////////////////////////////////////////////////////////////////////////////////////
//	�ʵ� ������ ���� ��¥�� �����Ѵ�.
//
void USER::SetStoreGuildWarScheme(TCHAR *pBuf, int nStore)
{
	int index = 0;
	int nCount = 0;
	CStore* pStore = NULL;

	int iMon[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	short sYear = GetShort(pBuf, index);
	BYTE tMon = GetByte(pBuf, index);
	BYTE tDay = GetByte(pBuf, index);
	BYTE tTime = GetByte(pBuf, index);

	if(sYear >= 2500) 
	{
		SendSystemMsg( IDS_USER_INVALID_YEAR, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tMon >= 13)
	{
		SendSystemMsg( IDS_USER_INVALID_MONTH, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tDay >= 32)
	{
		SendSystemMsg( IDS_USER_INVALID_DAY, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tTime < 20 && tTime > 23)
	{
		SendSystemMsg( IDS_USER_INVALID_HOUR, SYSTEM_ERROR, TO_ME);
		return;
	}

	pStore = GetStore(nStore);
	if(pStore == NULL) return;

	if(pStore->m_tWarType == GUILD_WAR_DECISION)
	{
		SendSystemMsg( IDS_USER_ALREADY_EXIST_SCHEDULE, SYSTEM_ERROR, TO_ME);
		return;// �̹� ����� ������ ���� ������ ���ٲ۴�.
	}

	int totalDay = DayCalculation((int)sYear, (int)tMon, (int)tDay); 
	int defDay = DayCalculation(pStore->m_wLastWarTime.wYear, pStore->m_wLastWarTime.wMonth, pStore->m_wLastWarTime.wDay); 

	nCount = abs(totalDay - defDay);
	if(nCount > 3)
	{
		SendSystemMsg( IDS_USER_GUILD_WAR_DELAY, SYSTEM_ERROR, TO_ME);
		return;
	}

	SYSTEMTIME temp;
	GetLocalTime(&temp);

	if(tMon == temp.wMonth && tDay <= temp.wDay)
	{
		SendSystemMsg( IDS_USER_PASS_MIN_DAY, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tDay == temp.wDay && tTime < temp.wHour) 
	{
		SendSystemMsg( IDS_USER_INVALID_TIME, SYSTEM_ERROR, TO_ME);
		return;
	}

	CTime CurrTime = CTime::GetCurrentTime();

	CTime LastTime(pStore->m_wLastWarTime);
	CTimeSpan DiffTime = CurrTime - LastTime;
	if(DiffTime.GetTotalHours() >= 12)
	{
		SendSystemMsg( IDS_USER_SCHEDULE_FIX, SYSTEM_ERROR, TO_ME);
		return;
	}
	
	CString strMsg;
	pStore->m_wPlanWarTime.wYear = sYear;
	pStore->m_wPlanWarTime.wMonth = tMon;
	pStore->m_wPlanWarTime.wDay = tDay;
	pStore->m_wPlanWarTime.wHour = tTime;

	if(!UpdateGuildStoreWarTime(pStore)) 
	{
		SendSystemMsg( IDS_USER_EDIT_SCHEDULE_FAIL, SYSTEM_ERROR, TO_ME);
		return;
	}

	pStore->m_tWarType = GUILD_WAR_DECISION;

	strMsg.Format( IDS_USER_SCHEDULE_FIXED, sYear, tMon, tDay, tTime); 
	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

	UpdateMemStoreWarType(pStore);
}

////////////////////////////////////////////////////////////////////////////////////////
//	�������� ���� ��¥�� �����Ѵ�.
//
void USER::SetFortressGuildWarScheme(TCHAR *pBuf, int nStore)
{
	int index = 0;
	int nCount = 0;
	CGuildFortress* pFort = NULL;

	int iMon[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	short sYear = GetShort(pBuf, index);
	BYTE tMon = GetByte(pBuf, index);
	BYTE tDay = GetByte(pBuf, index);
	BYTE tTime = GetByte(pBuf, index);

	if(sYear >= 2500) 
	{
		SendSystemMsg( IDS_USER_INVALID_YEAR, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tMon >= 13)
	{
		SendSystemMsg( IDS_USER_INVALID_MONTH, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tDay >= 32)
	{
		SendSystemMsg( IDS_USER_INVALID_DAY, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tTime != 21)
	{
		SendSystemMsg( IDS_USER_INVALID_HOUR, SYSTEM_ERROR, TO_ME);
		return;
	}

	pFort = GetFortress(nStore);
	if(pFort == NULL) return;

	if(pFort->m_lUsed == 1)
	{
		SendSystemMsg( IDS_USER_CANT_USE_IN_DEFENCE, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(pFort->m_tWarType == GUILD_WAR_DECISION)
	{
		SendSystemMsg( IDS_USER_ALREADY_EXIST_SCHEDULE, SYSTEM_ERROR, TO_ME);
		return;// �̹� ����� ������ ���� ������ ���ٲ۴�.
	}

	int totalDay = DayCalculation((int)sYear, (int)tMon, (int)tDay); 
	int defDay = DayCalculation(pFort->m_wLastWarTime.wYear, pFort->m_wLastWarTime.wMonth, pFort->m_wLastWarTime.wDay); 

	nCount = abs(totalDay - defDay);
	if(nCount > 7)
	{
		SendSystemMsg( IDS_USER_GUILD_WAR_DELAY1, SYSTEM_ERROR, TO_ME);
		return;
	}

/*	nCount = abs(totalDay - defDay);
	if(nCount > 2)
	{
		SendSystemMsg( IDS_USER_GUILD_WAR_DELAY2, SYSTEM_ERROR, TO_ME);
		return;
	}
*/
	SYSTEMTIME temp;
	GetLocalTime(&temp);

	if(tMon == temp.wMonth && tDay <= temp.wDay)
	{
		SendSystemMsg( IDS_USER_PASS_MIN_DAY, SYSTEM_ERROR, TO_ME);
		return;
	}
	if(tDay == temp.wDay && tTime < temp.wHour) 
	{
		SendSystemMsg( IDS_USER_INVALID_TIME, SYSTEM_ERROR, TO_ME);
		return;
	}

	CTime CurrTime = CTime::GetCurrentTime();

	CTime LastTime(pFort->m_wLastWarTime);
	CTimeSpan DiffTime = CurrTime - LastTime;
	if(DiffTime.GetTotalHours() >= 12)
	{
		SendSystemMsg( IDS_USER_SCHEDULE_FIX, SYSTEM_ERROR, TO_ME);
		return;
	}
	
	CString strMsg;
	pFort->m_wPlanWarTime.wYear = sYear;
	pFort->m_wPlanWarTime.wMonth = tMon;
	pFort->m_wPlanWarTime.wDay = tDay;
	pFort->m_wPlanWarTime.wHour = tTime;

	if(!UpdateGuildFortressWarTime(pFort)) 
	{
		SendSystemMsg( IDS_USER_EDIT_SCHEDULE_FAIL, SYSTEM_ERROR, TO_ME);
		return;
	}

	pFort->m_tWarType = GUILD_WAR_DECISION;

	strMsg.Format( IDS_USER_FORTRESS_SCHEDULE_FIXED, sYear, tMon, tDay, tTime); 
	SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

	UpdateMemFortressWarType(pFort);
}

int USER::DayCalculation(int iYear, int iMon, int iDay)
{
	int iTotalDays = 0;
	int ilastyear = iYear - 1;
	int iMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30 , 31, 30, 31};
	if((iYear % 4 == 0 && iYear % 100 != 0) || iYear % 400 == 0){
		iMonth[1] = 29;
	}
	
	iTotalDays = ilastyear * 365 + ilastyear / 4 + ilastyear / 400 - ilastyear / 100;
	for(int i=0;i<iMon - 1;i++)
	{
		iTotalDays += iMonth[i];	
	}

	iTotalDays += iDay;
	return iTotalDays;
}

////////////////////////////////////////////////////////////////////////////////////////
//	������� ������ ��� ��Ȳ�� ����Ǿ��ٰ� �˸���.
//
BOOL USER::StoppingTheGuildWar(CStore *pStore)
{
	if(m_dwGuild <= 0) return FALSE;

	if(pStore == NULL) return FALSE;

	int nLen = CHAR_NAME_LENGTH;

//	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) { ReleaseGuild(); return FALSE; }
	
	if(UpdateGuildStore(pStore->m_sStoreID, pGuild->m_strMasterName) == FALSE) { ReleaseGuild(); return FALSE; }
										// ������ �ʱ�ȭ�Ѵ�.(�� ����...^^)
	::ZeroMemory(pStore->m_strGuildName, CHAR_NAME_LENGTH + 1);
	::ZeroMemory(pStore->m_strMasterName, CHAR_NAME_LENGTH + 1);
	strncpy(pStore->m_strGuildName, m_strGuildName, nLen);
	strncpy(pStore->m_strMasterName, pGuild->m_strMasterName, nLen);

	ReleaseGuild();						// ����...

	SendGuildMsgForAnnounceGuildWarEnd(pStore);
	pStore->InitStoreInfo(m_dwGuild);

	SetGuildStoreTex(pStore->m_sStoreID, 0);	// Tax Rate Init

	InitMemStore(pStore);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
//	DB�� �����Ѵ�.
//
BOOL USER::UpdateGuildStore(int nSid, TCHAR *strGuildMasterName)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		

	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call INSERT_GUILD_STORE(%d,%d,\'%s\',\'%s\')}"), nSid, m_dwGuild, m_strGuildName, strGuildMasterName);

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild_Store Data Only!!\n");

		//g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DBNew[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

void USER::BeginGuildWar()
{
	m_tGuildWar = GUILD_WARRING;
//	SendSystemMsg( IDS_USER_START_GUILD_WAR, SYSTEM_NORMAL, TO_ME);
}

void USER::EndGuildWar(BYTE tWarType)
{
	CBufferEx TempBuf;

	m_tGuildWar = GUILD_WAR_AFFTER;

	m_FieldWarGMUid = 0;
	m_dwFieldWar = 0;

	TempBuf.Add(GUILD_WAR);
	TempBuf.Add((BYTE)0x02);							// ����� ����...
	TempBuf.Add(tWarType);

	if(tWarType == GUILD_FIELD_WARRING) TempBuf.Add((short)0x00); // �ʵ����϶��� 0���� 

	Send(TempBuf, TempBuf.GetLength());
//	SendSystemMsg( IDS_USER_END_GUILD_WAR, SYSTEM_NORMAL, TO_ME);
}

void USER::BeginFortressWar()
{
	m_tFortressWar = GUILD_WARRING;
}
/*
void USER::EndFortressWar()
{
	CBufferEx TempBuf;

	m_tFortressWar = GUILD_WAR_AFFTER;

	TempBuf.Add(GUILD_WAR);
	TempBuf.Add((BYTE)0x02);							// ����� ����...
	TempBuf.Add((BYTE)GUILD_FOTRESS_WARRING);

	Send(TempBuf, TempBuf.GetLength());
}
*/

//////////////////////////////////////////////////////////////////////////////////////////
//	������� �������� �˸���.
//
void USER::SendGuildMsgForAnnounceGuildWarEnd(CStore *pStore)
{
	int i, j;
	CString strMsg;
	CBufferEx TempBuf;

	USER* pUser = NULL;

	if(!pStore) return;

	strMsg.Format( IDS_USER_THIS_AREA_GUILD_HAVE, m_strGuildName);

	for(i = 0; i < MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;
		
		if(pStore->m_iGuildSid == pUser->m_dwGuild) pUser->EndGuildWar(GUILD_STORE_WARRING);
		else
		{
			for(j =0; j < GUILD_ATTACK_MAX_NUM; j++)
			{
				if(pStore->m_arAttackGuild[j] == pUser->m_dwGuild)
				{
					pUser->EndGuildWar(GUILD_STORE_WARRING);			// ��� ���� ����
//					pUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_NORMAL, TO_ME);
					break;
				}
			}
		}

		pUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//	�������� �������� �˸���.
//
/*
void USER::SendGuildMsgForAnnounceFortressWarEnd(CGuildFortress *pFort)
{
	int i, j;
	CString strMsg;
	CBufferEx TempBuf;

	USER* pUser = NULL;

	if(!pFort) return;

	strMsg.Format( IDS_USER_THIS_AREA_GUILD_HAVE, m_strGuildName);

	for(i = 0; i < MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;
		
		if(pFort->m_iGuildSid == pUser->m_dwGuild) pUser->EndFortressWar();
		else
		{
			for(j =0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)
			{
				if(pFort->m_arAttackGuild[j].lGuild == pUser->m_dwGuild)
				{
					pUser->EndFortressWar();			// ��� ���� ����
					break;
				}
			}
		}

		pUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_NORMAL, TO_ME);
	}
}
*/

//////////////////////////////////////////////////////////////////////////////////////////
//	�ֹε�Ϲ�ȣ�� ��ġ�ϴ��� �˻�
//
BOOL USER::CheckJuminNumber(LPCTSTR szAccount, LPCTSTR szJumin)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	CString			szTemp = szJumin;
	szTemp.Insert(6, '-');

	BOOL bQuerySuccess = TRUE;
	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GET_JUMIN_NO (\'%s\')}"), szAccount); 
	
	SQLCHAR		strJuminSQL[20];
	SQLINTEGER	strInd = SQL_NTS;

	::ZeroMemory(strJuminSQL, sizeof(strJuminSQL));

	int db_index = 0;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if( retcode != SQL_SUCCESS )
	{
		//g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	retcode = SQLExecDirect( hstmt, (unsigned char*)szSQL, SQL_NTS );
	if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
	{
		retcode = SQLFetch( hstmt );

		if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, strJuminSQL, sizeof(strJuminSQL), &strInd);
		}
	}
	else
	{
		//DisplayErrorMsg(hstmt);
		bQuerySuccess = FALSE;

		if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		//g_DBSession[m_iModSid].ReleaseDB(db_index);
		TRACE("*** �ֹε�� ��ȣ DB[%d/%d] ����... ***\n", m_iModSid, db_index );
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBSession[m_iModSid].ReleaseDB(db_index);
	
	if(bQuerySuccess && szTemp.Compare((LPTSTR)strJuminSQL) == 0) return TRUE;
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//	�������� �� ���ǿ� ���� ������ ��� â��(�޸� DB)�� �����Ѵ�.
//
void USER::UpdateMemStoreDN(int iStoreID, DWORD dwDN)
{
	/*
	int nSize = g_arStoreSharedMemory.GetSize();
	CSharedMemory* pShared = NULL;
	CMemStore* pData = NULL;

	for(int i = 0; i < nSize; i++)
	{
		pShared = g_arStoreSharedMemory[i];

		if(pShared == NULL) continue;
		if(pShared->m_hMapping == NULL) continue;

		pData = (CMemStore*) pShared->m_lpData;

		if(pData == NULL) continue;
		if(pData->m_sStoreID == -1) continue;
		if(pData->m_iGuildSid <= 0) continue;

		if(iStoreID == pData->m_sStoreID)
		{
			pData->m_dwStoreDN = dwDN;
			return;
		}
	}
	*/
	int nSize, i;
	int iID = 0;

	if(iStoreID >= FORTRESS_BAND)
	{
		nSize = g_arFortressSharedMemory.GetSize();
		CSharedMemory* pShared = NULL;
		CMemFortress* pData = NULL;

		for(i = 0; i < nSize; i++)
		{
			pShared = g_arFortressSharedMemory[i];
			if(pShared == NULL) return;
			if(pShared->m_hMapping == NULL) return;

			pData = (CMemFortress*) pShared->m_lpData;
			
			if(pData == NULL) return;
			if(pData->m_sFortressID < 1000) return;
			if(pData->m_iGuildSid <= 0) return;

			iID = pData->m_sFortressID;
			if(iID == iStoreID)
			{
				pData->m_dwStoreDN = dwDN;
				break;
			}
		}
	}
/*	else
	{
		nSize = g_arStoreSharedMemory.GetSize();
		CSharedMemory* pShared = NULL;
		CMemStore* pData = NULL;

		if(iStoreID >= nSize) return;
		if((int)dwDN < 0) return;

		pShared = g_arStoreSharedMemory[iStoreID];
		
		if(pShared == NULL) return;
		if(pShared->m_hMapping == NULL) return;
		
		pData = (CMemStore*) pShared->m_lpData;
		
		if(pData == NULL) return;
		if(pData->m_sStoreID == -1) return;
		if(pData->m_iGuildSid <= 0) return;
		
		pData->m_dwStoreDN = dwDN;
	}
*/
}

/////////////////////////////////////////////////////////////////////////////////////////
//	�ʵ� ������ ���� �޸� DB�� ������ �ʱ�ȭ �Ѵ�.
//
void USER::InitMemStore(CStore *pStore)
{
/*	if(pStore == NULL) return;

	int nSize = g_arStoreSharedMemory.GetSize();
	CSharedMemory* pShared = NULL;
	CMemStore* pData = NULL;

	if(pStore->m_sStoreID >= nSize) return;

	pShared = g_arStoreSharedMemory[pStore->m_sStoreID];
	
	if(pShared == NULL) return;
	if(pShared->m_hMapping == NULL) return;
	
	pData = (CMemStore*) pShared->m_lpData;
	
	if(pData == NULL) return;
	
	pData->m_sStoreID = pStore->m_sStoreID;
	pData->m_iGuildSid = pStore->m_iGuildSid;
	pData->m_sTaxRate = 0;
	pData->m_dwStoreDN = 0;
	pData->m_tWarType = GUILD_WAR_PREPARE;

	pStore->GuildListToStr(pData->m_AttackList);
*/
}

/////////////////////////////////////////////////////////////////////////////
//	�ʵ������ ���� ������ �޸� DB�� �����Ѵ�.
//
void USER::UpdateMemStoreTax(int iSid, int iRate)
{
	int nSize, i;

	if(iSid >= FORTRESS_BAND)
	{
		nSize = g_arFortressSharedMemory.GetSize();
		CSharedMemory* pShared = NULL;
		CMemFortress* pData = NULL;

		if(iRate < 0 || iRate > GUILD_MAX_TAX) return;		// �ִ� 200 % ����
		for(i = 0; i < nSize; i++)
		{
			pData = NULL;
			pShared = NULL;
			pShared = g_arFortressSharedMemory[i];
			
			if(pShared == NULL) continue;
			if(pShared->m_hMapping == NULL) continue;
			
			pData = (CMemFortress*) pShared->m_lpData;
			
			if(pData == NULL) continue;
			if(pData->m_sFortressID != iSid) continue;;
			if(pData->m_iGuildSid != m_dwGuild) continue;
			
			pData->m_sTaxRate = iRate;
			break;
		}
	}
/*	else
	{
		nSize = g_arStoreSharedMemory.GetSize();
		CSharedMemory* pShared = NULL;
		CMemStore* pData = NULL;

		if(iSid >= nSize) return;
		if(iRate < 0 || iRate > GUILD_MAX_TAX) return;		// �ִ� 200 % ����

		pShared = g_arStoreSharedMemory[iSid];
		
		if(pShared == NULL) return;
		if(pShared->m_hMapping == NULL) return;
		
		pData = (CMemStore*) pShared->m_lpData;
		
		if(pData == NULL) return;
		if(pData->m_sStoreID == -1) return;
		if(pData->m_iGuildSid <= 0) return;
		
		pData->m_sTaxRate = iRate;
	}
*/
}

////////////////////////////////////////////////////////////////////////////
//	�ʵ������ ���� WarType �� �޸� DB�� �����Ѵ�.
//
void USER::UpdateMemStoreWarType(CStore *pStore)
{
/*	if(pStore == NULL) return;

	int nSize = g_arStoreSharedMemory.GetSize();
	CSharedMemory* pShared = NULL;
	CMemStore* pData = NULL;

	if(pStore->m_sStoreID >= nSize) return;

	pShared = g_arStoreSharedMemory[pStore->m_sStoreID];
	
	if(pShared == NULL) return;
	if(pShared->m_hMapping == NULL) return;
	
	pData = (CMemStore*) pShared->m_lpData;
	
	if(pData == NULL) return;
	
	pData->m_tWarType = pStore->m_tWarType;
*/
}

////////////////////////////////////////////////////////////////////////////
//	�������� ���� WarType �� �޸� DB�� �����Ѵ�.
//
void USER::UpdateMemFortressWarType(CGuildFortress *pFort)
{
	if(pFort == NULL) return;

	int nSize = g_arFortressSharedMemory.GetSize();
	CSharedMemory* pShared = NULL;
	CMemFortress* pData = NULL;

	for(int i = 0; i < nSize; i++)
	{
		pShared = g_arFortressSharedMemory[i];
		
		if(pShared == NULL) return;
		if(pShared->m_hMapping == NULL) return;
		
		pData = (CMemFortress*) pShared->m_lpData;
		
		if(pData == NULL) return;
		
		if(pData->m_iGuildSid == pFort->m_iGuildSid)
		{
			pData->m_tWarType = pFort->m_tWarType;
			break;
		}
	}
}

void USER::UpdateMemStoreGuildList(CStore *pStore)
{
/*	if(pStore == NULL) return;

	int nSize = g_arStoreSharedMemory.GetSize();
	CSharedMemory* pShared = NULL;
	CMemStore* pData = NULL;

	if(pStore->m_sStoreID >= nSize) return;

	pShared = g_arStoreSharedMemory[pStore->m_sStoreID];
	
	if(pShared == NULL) return;
	if(pShared->m_hMapping == NULL) return;
	
	pData = (CMemStore*) pShared->m_lpData;
	
	if(pData == NULL) return;
	
	pStore->GuildListToStr(pData->m_AttackList);
*/
}

/////////////////////////////////////////////////////////////////////////////
//	���� ������ ���ִ� ��ǥ�� �ʿ��� ������ �ִ°�����,  �ʻ� �����Ӽ����� �����ϱ����� �ε����� �ѱ��. 
//
int USER::CheckInvalidMapType()
{
	int nRet = -1;

	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return nRet;
	if(m_curx >= g_zone[m_ZoneIndex]->m_sizeMap.cx || m_curx < 0) return nRet;
	if(m_cury >= g_zone[m_ZoneIndex]->m_sizeMap.cy || m_cury < 0) return nRet;

	nRet = ((g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_dwType & 0xFF00 ) >> 8);

	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
//	���� ������ ���ִ� ��ǥ�� ����� ���� �����̸� FALSE���� �ѱ��.
//
BOOL USER::CheckInvalidZoneState(int type)
{
	int index = 0;

//	if(type < 2 || type >= 17) return TRUE;
//	index = g_arMapTable[type]->m_sStoreID;
	int mapindex = GetGuildMapIndex(type);
	if(mapindex <= -1 || mapindex >= g_arMapTable.GetSize()) return TRUE;

	index = g_arMapTable[mapindex]->m_sStoreID;

	if(index >= FORTRESS_BAND) 
	{
		CGuildFortress* pFort = NULL;
		pFort = GetFortress(index);

		if(pFort) 
		{
			if(pFort->m_lUsed == 1)	return FALSE;
//			if(g_arMapTable[type]->m_sStoreZone) return FALSE;
		}
	}	
/*	else
	{
		CStore *pStore = NULL;

		index = g_arMapTable[mapindex]->m_sStoreIndex;

		pStore = GetStore(index);

		if(pStore)
		{
			if(pStore->m_lUsed == 1) return FALSE;
		}
//		if(index < 0 || index >= g_arStore.GetSize()) return TRUE;
//		if(g_arStore[index]->m_lUsed == 1)	return FALSE;  // �ش� �Ӽ� ���� ����� ����	
	}
*/
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	��ü������ ��û�Ҷ� �ش� ������ �������̸� �ź��Ѵ�.
//
BOOL USER::CheckInvakidZoneChangeState(int type)
{
	int index = 0;

//	if(type < 2 || type >= 17) return TRUE;
	int mapindex = GetGuildMapIndex(type);
	if(mapindex <= -1 || mapindex >= g_arMapTable.GetSize()) return TRUE;

	index = g_arMapTable[mapindex]->m_sStoreID;

	if(index >= FORTRESS_BAND) 
	{
		CGuildFortress* pFort = NULL;
		pFort = GetFortress(index);

		if(pFort) 
		{
			if(m_dwGuild > 0 && pFort->m_iGuildSid == m_dwGuild) return TRUE;
			if(pFort->m_iZone == m_curz || g_arMapTable[mapindex]->m_sStoreZone) return FALSE;
			if(pFort->m_lUsed == 1)	return FALSE;
		}
	}
/*	else
	{
		CStore *pStore = NULL;

		index = g_arMapTable[mapindex]->m_sStoreIndex;

		pStore = GetStore(index);

		if(pStore)
		{
			if(pStore->m_lUsed == 1) return FALSE;
		}
	}
*/
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	Ŭ���̾�Ʈ���� ��û�Ҷ� ���� ƽ�� ������ ��.���� �����Ѵ�.
//
void USER::SendServerTick()
{
/*	DWORD dwTick;

	CBufferEx TempBuf;

	dwTick = GetTickCount();

	TempBuf.Add(SERVER_TICK_RESULT);
	TempBuf.Add(dwTick);
	Send(TempBuf, TempBuf.GetLength());
*/
}

/////////////////////////////////////////////////////////////////////////////
//	����� �ð��� �����Ѵ�.
//
BOOL USER::UpdateGuildStoreWarTime(CStore *pStore)
{
	CString			strTime;
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strWarTime[30];

	if(pStore->m_iGuildSid <= 0) return FALSE;

	strTime = _T("");
	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strWarTime, sizeof(strWarTime));
	
	strTime.Format("%d-%d-%d %d:00:00", pStore->m_wPlanWarTime.wYear, pStore->m_wPlanWarTime.wMonth, pStore->m_wPlanWarTime.wDay, pStore->m_wPlanWarTime.wHour);
	::CopyMemory(strWarTime, strTime.GetBuffer(strTime.GetLength()), strTime.GetLength());
	strTime.ReleaseBuffer();
	
	SDWORD sLen	= sizeof(strWarTime);

	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_GUILD_STORE_WARTIME(%d,%d,\'%s\')}"), pStore->m_sStoreID, pStore->m_iGuildSid, strWarTime);

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild_Store Data Only!!\n");

		//g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DBNew[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	����� �ð��� �����Ѵ�.
//
BOOL USER::UpdateGuildFortressWarTime(CGuildFortress *pFort)
{
	CString			strTime;
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		
	TCHAR			strWarTime[30];

	if(pFort->m_iGuildSid <= 0) return FALSE;

	strTime = _T("");
	::ZeroMemory(szSQL, sizeof(szSQL));
	::ZeroMemory(strWarTime, sizeof(strWarTime));
	
	strTime.Format("%d-%d-%d 21:00:00", pFort->m_wPlanWarTime.wYear, pFort->m_wPlanWarTime.wMonth, pFort->m_wPlanWarTime.wDay);
	::CopyMemory(strWarTime, strTime.GetBuffer(strTime.GetLength()), strTime.GetLength());
	strTime.ReleaseBuffer();
	
	SDWORD sLen	= sizeof(strWarTime);

	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_GUILD_FORTRESS_WARTIME(%d,%d,\'%s\')}"), pFort->m_sFortressID, pFort->m_iGuildSid, strWarTime);

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild_Store Data Only!!\n");

		//g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DBNew[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		BREAKPOINT();

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	�ʵ� ������� ä������ ��û ---> ���� �渶���� ������.
//
void USER::SendGuildWarFieldApply(TCHAR *pBuf)
{
	int index = 0;
	int nLength = 0;

	BYTE error_code = 0;

	CGuild *pGuild = NULL;
	USER *pUser = NULL;

	CBufferEx TempBuf;

	TCHAR szGuildName[255];

	if(m_dwGuild <= 0) { error_code = ERR_10; goto go_result; };			// ������ ������ �ƴϸ�..
	if(!m_bGuildMaster) { error_code = ERR_10; goto go_result; };			// ������ �渶�� �ƴϸ�...

	if(m_tGuildWar == GUILD_WARRING) { error_code = ERR_12; goto go_result; };

	::ZeroMemory(szGuildName, sizeof(szGuildName));

	nLength = GetVarString(sizeof(szGuildName), szGuildName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) { error_code = 255; goto go_result; } // ��� ��Ī ���ѿ� �ɸ�

	pGuild = GetGuildByName(szGuildName);

	if(!pGuild) 
	{ 
		ReleaseGuild();
		error_code = ERR_7; 
		goto go_result; 
	} // �ش� ��尡 �������� �ʴ´�.
	
	pUser = GetUser(pGuild->m_strMasterName);
	ReleaseGuild();

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED)  
	{ 
		error_code = ERR_3;							// ��� �渶�� ���� ���ӿ� ����.
		goto go_result; 
	} 

	if(pUser->m_tGuildWar == GUILD_WARRING) { error_code = ERR_12; goto go_result; };
	if(pUser->m_dwGuild <= 0 || !pUser->m_bGuildMaster) { error_code = ERR_3; goto go_result; } // ���� ����...

	if(!GetDistance(pUser->m_curx, pUser->m_cury, 12)) 
	{
		SendSystemMsg( IDS_USER_IN_ONE_SCREEN, SYSTEM_NORMAL, TO_ME);
		return;
	}
	
	m_bFieldWarApply = TRUE;
	m_FieldWarGMUid = pUser->m_uid;					// ���� uid�� ���
	m_dwFieldWar = pUser->m_dwGuild;				// �ʵ��� �����̸� ����� ��� ��ȣ�� ���´�.

	pUser->m_FieldWarGMUid = m_uid;					//
	pUser->m_dwFieldWar = m_dwGuild;				//

	TempBuf.Add(GUILD_FIELD_WAR_REQ);
	TempBuf.Add((int)m_dwGuild);
	TempBuf.AddString(m_strGuildName);
	
	Send(TempBuf, TempBuf.GetLength());				// ���� ������� ��û���̶�� â�� ǥ�õǵ����Ѵ�..	
	pUser->Send(TempBuf, TempBuf.GetLength());		// ���濡�Դ� ���� ��û�ߴٴ� 
	return;

go_result:
	m_FieldWarGMUid = -1;
	m_dwFieldWar = 0;

	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add((BYTE)0x00);						// ����
	TempBuf.Add(error_code);
	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////
//	�ʵ����� ���� �³����ο� ������� �����Ѵ�.
//
void USER::SendGuildWarFieldApplyResult(TCHAR *pBuf)
{
	int index = 0;
	int nLength = 0;
	int iGuildID = 0;

	BYTE type = 0;
	BYTE result = FAIL;

	CGuild *pGuild = NULL;
	USER *pUser = NULL;

	CBufferEx TempBuf;

	TCHAR szGuildName[255];

	if(m_dwGuild <= 0) return;
	if(!m_bGuildMaster) return;	
	if(m_FieldWarGMUid < 0 || m_dwFieldWar <= 0) return;
	if(m_tGuildWar == GUILD_WARRING) return;

	TempBuf.Add(GUILD_FIELD_WAR_RESULT);

	pUser = GetUser(m_FieldWarGMUid);
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) 
	{
		TempBuf.Add((BYTE)0x02);											// ����
		Send(TempBuf, TempBuf.GetLength());									
		return;
	}

	if(pUser->m_dwGuild <= 0 || !pUser->m_bGuildMaster) goto go_result;		// ������ ���� ��� ������...

	type = GetByte(pBuf, index);											// �����̸�...

	if(type != 1) goto go_result;											// �³��� �ƴϸ�...
	
	if(m_bFieldWarApply) return;											// �ʵ����� ��û������ ����� ������ �����Ѵ�. 
	if(pUser->m_tGuildWar == GUILD_WARRING) goto go_result;

	iGuildID = GetInt(pBuf, index);
	pGuild = GetGuild(iGuildID);											// ��带 ã��
	
	if(!pGuild)																// ��尡 ������	
	{
		ReleaseGuild();
		goto go_result;
	}

	::ZeroMemory(szGuildName, sizeof(szGuildName));

	nLength = GetVarString(sizeof(szGuildName), szGuildName, pBuf, index);
	if(nLength <= 0 || nLength > CHAR_NAME_LENGTH)							// ����̸��� �߸� �Ǿ�����..
	{
		ReleaseGuild();
		goto go_result;
	}

	if(strcmp(pGuild->m_strGuildName, szGuildName) != 0)					// ����̸��� Ʋ����...
	{
		ReleaseGuild();
		goto go_result;
	}

	ReleaseGuild();
	result = SUCCESS;

	SendGuildFieldWarBegin();												// ����� ������ �˸���.

go_result:
	if(result == FAIL)
	{
		m_FieldWarGMUid = -1;
		m_dwFieldWar = 0;	
		pUser->m_FieldWarGMUid = -1;
		pUser->m_dwFieldWar = 0;
	}

	m_bFieldWarApply = FALSE;												// �ʱ�ȭ�Ѵ�.(���ʿ��� ��û�ϰ� �³��ϴ°��� �����ϱ�����))	
	pUser->m_bFieldWarApply = FALSE;

	TempBuf.Add(result);											
	Send(TempBuf, TempBuf.GetLength());									
	pUser->Send(TempBuf, TempBuf.GetLength());
	return;

}

/////////////////////////////////////////////////////////////////////////////
//	�ʵ����� ���� �³��̾��ٸ� ������� �����Ѵ�.
//
void USER::SendGuildFieldWarBegin()
{
	int i;
	CBufferEx TempBuf;

	USER* pUser = NULL;

	if(m_dwGuild <= 0 || m_dwFieldWar <= 0) return;

	TempBuf.Add(GUILD_WAR);
	TempBuf.Add((BYTE)0x01);							// ����� ����...
	TempBuf.Add((BYTE)GUILD_FIELD_WARRING);				// ...

	TempBuf.Add((short)0x00);							// �ʵ� ǥ��
	TempBuf.Add((BYTE)0x01);							// ���� ���� ����

	TempBuf.Add((short)0x02);							// �� ����
	TempBuf.Add((int)m_dwGuild);
	TempBuf.Add((int)m_dwFieldWar);

	for(i = 0; i < MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;
		if(pUser->m_dwGuild <= 0) continue;

		if(pUser->m_dwGuild == m_dwGuild) pUser->m_dwFieldWar = m_dwFieldWar;
		else if(pUser->m_dwGuild == m_dwFieldWar) pUser->m_dwFieldWar = m_dwGuild;
		else continue;

		pUser->BeginGuildWar();			// 
		pUser->Send(TempBuf, TempBuf.GetLength());
	}
}

/////////////////////////////////////////////////////////////////////////////
//	�ʵ������� �׺��ϴ� �� �渶�� ȣ���Ѵ�.
//
void USER::SendGuildWarFieldEnd(TCHAR *pBuf)
{
	int i;
	CBufferEx TempBuf;

	USER* pUser = NULL;
													// ������ �ƴϸ� ����				
	if(m_dwGuild <= 0 || m_dwFieldWar <= 0) return;
	
	CString strMsg;

	pUser = GetUser(m_FieldWarGMUid);
	if(!pUser) return;

	if(!pUser->m_bGuildMaster) return;				// ������ �渶�� �ƴ� ����

	int iFieldGuild = m_dwFieldWar;

	for(i = 0; i < MAX_USER; i++)
	{
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if(pUser->m_dwGuild == m_dwGuild || pUser->m_dwGuild == iFieldGuild)
		{
			pUser->EndGuildWar(GUILD_FIELD_WARRING);			// ��� ���� ����
			pUser->SendSystemMsg(pBuf, SYSTEM_NORMAL, TO_ME);
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//	�����̻������� �˷��ش�.
//
void USER::CheckServerTest()
{
	CString strTemp, strIP;

	strTemp.Format("%u", (unsigned int)((unsigned char*)m_Addr.sin_addr.S_un.S_un_b.s_b1));
	strIP += strTemp; strTemp = _T("");
	strIP += ".";
	strTemp.Format("%u", (unsigned int)((unsigned char*)m_Addr.sin_addr.S_un.S_un_b.s_b2));
	strIP += strTemp; strTemp = _T("");
	strIP += ".";
	strTemp.Format("%u", (unsigned int)((unsigned char*)m_Addr.sin_addr.S_un.S_un_b.s_b3));
	strIP += strTemp; strTemp = _T("");
	strIP += ".";

	CBufferEx TempBuf;

	TempBuf.Add(CHECK_ALIVE_RESULT);

	if(strIP.Left(12).CompareNoCase(_T("192.203.141.")) == 0)
	{
		int nCount = 0;
		USER *pUser = NULL;
		nCount = 0;
		for (int i = 0; i < MAX_USER; i++ )
		{
			pUser = m_pCom->GetUserUid(i);
			if(pUser && pUser->m_state == STATE_GAMESTARTED ) nCount++;
		}

		TempBuf.Add((short)nCount);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	TempBuf.Add((short)0);
	Send(TempBuf, TempBuf.GetLength());
}


//////////////////////////////////////////////////////////////////////
//
//	���� ��������.
//
void USER::AnnounceNotice(TCHAR *pBuf)
{
	CString strTemp, strIP;

	strTemp.Format("%u", (unsigned int)((unsigned char*)m_Addr.sin_addr.S_un.S_un_b.s_b1));
	strIP += strTemp; strTemp = _T("");
	strIP += ".";
	strTemp.Format("%u", (unsigned int)((unsigned char*)m_Addr.sin_addr.S_un.S_un_b.s_b2));
	strIP += strTemp; strTemp = _T("");
	strIP += ".";
	strTemp.Format("%u", (unsigned int)((unsigned char*)m_Addr.sin_addr.S_un.S_un_b.s_b3));
	strIP += strTemp; strTemp = _T("");
	strIP += ".";

	if(strIP.Left(12).CompareNoCase(_T("192.203.141.")) == 0)
	{
		int index = 0;
		SendAllChat(pBuf + index);
	}
}


//////////////////////////////////////////////////////////////////////
//
//	PK�������� 0����, (�������� �������)
//
void USER::SendCityRank(int iCityRank)
{
	CBufferEx TempPKBuf;

	if(iCityRank >= 0) m_sKillCount -= 1;	    // ī������ �׾�� PK ī��Ʈ�� ���ҵ��� �ʴ´�.

	if(m_sKillCount < 0)
	{
		m_sKillCount = 0;
		if(m_sLevel <= 10 && m_iCityValue < 0)
		{
			m_sCityRank = 0;
			m_iCityValue = 0;

			CheckGuildUserInFortress();			// �ù� ����� ���Ҷ� �ٽ� ����(��Ʈ����������)
			
			TempPKBuf.Add(SET_USER_PK_STATE);
			TempPKBuf.Add(m_uid + USER_BAND);
			TempPKBuf.Add((BYTE)0x01);
			TempPKBuf.Add(m_sCityRank);

			SendInsight(TempPKBuf, TempPKBuf.GetLength());
//			SendExactScreen(TempPKBuf, TempPKBuf.GetLength());
		}
	}
}

//////////////////////////////////////////////////////////////////////
//	Agent �� ���� DB�� �ð� ������ ���
//
DWORD USER::ConvertCurTimeToSaveTime()
{
	DWORD dwCurTime = 0;

	SYSTEMTIME SaveTime;
	GetLocalTime(&SaveTime);
	
	WORD wTemp = 0;
	DWORD dwYear = 0;
	DWORD dwMon = 0;
	DWORD dwDay = 0;
	DWORD dwHour = 0;
	DWORD dwMin = 0;
	DWORD dwSecond = 0;
										// 2 Byte ������
	wTemp = SaveTime.wYear << 12;		// ���� 4 Byte
	wTemp = wTemp >> 12;
	dwYear = (DWORD)wTemp; 
	dwYear = dwYear << 26;

	wTemp = SaveTime.wMonth << 12;		// 4 Byte
	wTemp = wTemp >> 12;
	dwMon = (DWORD)wTemp; 
	dwMon = dwMon << 22;

	wTemp = SaveTime.wDay << 11;		// 5 Byte
	wTemp = wTemp >> 11;
	dwDay = (DWORD)wTemp;
	dwDay = dwDay << 17;

	wTemp = SaveTime.wHour << 11;		// 5 Byte
	wTemp = wTemp >> 11;
	dwHour = (DWORD)wTemp;
	dwHour = dwHour << 12;

	wTemp = SaveTime.wMinute << 10;		// 6 Byte
	wTemp = wTemp >> 10;
	dwMin = (DWORD)wTemp;
	dwMin = dwMin << 6;

	wTemp = SaveTime.wSecond << 10;		// 6 Byte
	wTemp = wTemp >> 10;
	dwSecond = (DWORD)wTemp;

	dwCurTime = dwYear^dwMon^dwDay^dwHour^dwMin^dwSecond;

	return dwCurTime;
}



void USER::MassZoneMove(TCHAR *pBuf)
{
	int i, uid = 0;
	int index = 0, nLen = 0;
	int cityNum = -1, virtualNum = -1;
	int iTotalLevel = 0;

	USER *pUser = NULL;
	CNpc *pNpc = NULL;

	int userCount = 0;
	UserList tempUser[MAX_GUILD_HOUSE_USER];
//	CDWordArray	arUid;
//	arUid.RemoveAll();

	SYSTEMTIME guildTime;
	GetLocalTime(&guildTime);

	if( guildTime.wDay < GUILD_VIRTUAL_WAR_START && guildTime.wDay > GUILD_VIRTUAL_WAR_END ) return;

	short sCount = GetShort(pBuf, index);							// ������

	if(sCount <= 0 || sCount > MAX_GUILD_HOUSE_USER) return;		// �ִ� 20�����
	
	if(m_dwGuild <= 0 || m_dwGuild >= g_arGuildData.GetSize()) return;
	if(!m_bGuildMaster) return;										// ��忡 ������ �ʾ�����...
	if(!g_arGuildData[m_dwGuild]) return;

	for(i = 0; i < MAX_GUILD_HOUSE_USER; i++)
	{
		tempUser[i].uid = 0;
		::ZeroMemory(tempUser[i].strUserName, sizeof(tempUser[i].strUserName));
	}

	for(i = 0; i < sCount; i++)
	{
		uid = GetInt(pBuf, index);

		if(uid >= USER_BAND && uid < NPC_BAND)
		{
			pUser = GetUser(uid - USER_BAND);
			if(!pUser || pUser->m_state != STATE_GAMESTARTED) continue;
			if(pUser->m_dwGuild != m_dwGuild) continue;
			if(pUser->m_curz != m_curz) continue;

			nLen = strlen(pUser->m_strUserID);
			if(nLen <= 0) continue;

			iTotalLevel += pUser->m_sLevel;

			userCount++;
			tempUser[i].uid = pUser->m_uid;
			strncpy(tempUser[i].strUserName, pUser->m_strUserID, nLen);
		}
	}

	if(sCount != userCount)									// �׵��� ������ ������ ��ȿ	
	{
		SendSystemMsg( IDS_USER_INVALID_MEMBER_COUNT, SYSTEM_ERROR, TO_ME);
		return;
	}

	short sGuildHouseZone = GetShort(pBuf, index);					// ���� �� ��ȣ
	if( !IsZoneInThisServer(sGuildHouseZone) )	return;				// DB�� ���ð��� ������?
	if(sGuildHouseZone==409) return;

	cityNum = GetCityNumForVirtualRoom(sGuildHouseZone);
	if(cityNum < 0 || cityNum >= g_arGuildHouseWar.GetSize()) return;
	if(!g_arGuildHouseWar[cityNum]) return;

	virtualNum = GetVirtualRoomNum(sGuildHouseZone);				// ���� ������ ��ã�ڴ�.--> ����.
	if(virtualNum < 0) return;

	if(InterlockedCompareExchange((LONG*)&g_arGuildHouseWar[cityNum]->m_CurrentGuild.lUsed, (long)1, (long)0) == (long)0)
	{
		for(i = 0; i < MAX_GUILD_HOUSE_USER; i++)
		{
			g_arGuildHouseWar[cityNum]->m_CurrentGuild.arUserList[i].uid = 0;
			::ZeroMemory(g_arGuildHouseWar[cityNum]->m_CurrentGuild.arUserList[i].strUserName, sizeof(g_arGuildHouseWar[cityNum]->m_CurrentGuild.arUserList[i].strUserName));
		}

		g_arGuildHouseWar[cityNum]->m_CurrentGuild.dwIntervalTick = 0;
		g_arGuildHouseWar[cityNum]->m_CurrentGuild.iGuildNum = userCount;
		g_arGuildHouseWar[cityNum]->m_CurrentGuild.dwTimer = GetTickCount();
		g_arGuildHouseWar[cityNum]->m_CurrentGuild.lGuild = m_dwGuild;
		g_arGuildHouseWar[cityNum]->m_CurrentGuild.iCurValue = 0;		
		g_arGuildHouseWar[cityNum]->m_CurrentGuild.iGuildLevel = iTotalLevel;

		for(i = 0; i < g_arGuildHouseWar[cityNum]->m_CurrentGuild.arNpcList.GetSize(); i++)
		{
			uid = g_arGuildHouseWar[cityNum]->m_CurrentGuild.arNpcList[i];
			pNpc = GetNpc(uid);
			if(!pNpc) continue;

			if(pNpc->m_NpcState == NPC_LIVE) pNpc->Dead();
//			pNpc->m_tNpcAttType = 1;
			pNpc->m_NpcVirtualState = NPC_MOVING;
			pNpc->m_NpcState = NPC_LIVE;
			pNpc->NpcTypeParser();
//			if(pNpc->m_NpcState == NPC_LIVE) pNpc->m_NpcState = NPC_DEAD;
//			pNpc->SetLive(m_pCom);
//			TRACE("%s MOP\n", pNpc->m_strName);
		}

		for(i = 0; i < userCount; i++)
		{
			pUser = GetUser(tempUser[i].uid);
			if(!pUser) continue;

			g_arGuildHouseWar[cityNum]->m_CurrentGuild.arUserList[i].uid = tempUser[i].uid;
			nLen = strlen(tempUser[i].strUserName);
			if(nLen <= 0) continue;
			strncpy(g_arGuildHouseWar[cityNum]->m_CurrentGuild.arUserList[i].strUserName, tempUser[i].strUserName, nLen);
			
			pUser->ZoneMoveReq(sGuildHouseZone, g_arVirtualRoom[virtualNum]->m_Move[i].x, g_arVirtualRoom[virtualNum]->m_Move[i].y);
			pUser->SendSystemMsg( IDS_USER_QUEST_TIME, SYSTEM_NORMAL, TO_ME);
			pUser->m_tGuildHouseWar = GUILD_WARRING;
		}
	}
}

int USER::GetCityNumForVirtualRoom(int zone)		// ������ ���ù�ȣ���� ���߿� VirtualRoom�� ��� �߰��Ǹ�..
{													// �ٲپ�� �ȴ�. (int zone, int &curGuild)
	int nRet = -1;

	switch(zone)									// �߰��� ������...
	{
	case 1005:										// SANAD
		nRet = SANAD;								// curGuild = ;
		break;

	default:
		break;
	}

	return nRet;
}

int USER::GetVirtualRoomNum(int zone)
{														// �� ���þȿ� �������� ��������� ������ �����Ƿ� 
	for(int i = 0; i < g_arVirtualRoom.GetSize(); i++)	// ���� ���� ���� ��ȣ�ΰ��� ã�´�.
	{
		if(g_arVirtualRoom[i]->m_sZone == zone) return i;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
//
//	�о�Ⱓ�� ��� ������ �����ش�.
//
void USER::SendGuildHouseRank(int nNum, int iCity)
{
	int i, j, index = 0;
	int nLen = 0, nTime = 0;
	int talknum = -1;

	BYTE tCount = 0;
//	TCHAR strTemp[1000];
	CHyper hyperText;

	CBufferEx TempBuf, TempSayBuf;

	CString temp;
	
	if(nNum < 0) return;
	if(iCity < 0 || iCity >= g_arGuildHouseWar.GetSize()) return;
	if(!g_arGuildHouseWar[iCity]) return;

	for(i = 0; i < g_arNpcChat.GetSize(); i++)
	{
		if(g_arNpcChat[i]->m_sCid == nNum)
		{
			talknum = i;
			break;
		}
	}

	if( talknum == -1 ) return;

//	::ZeroMemory(strTemp, sizeof(strTemp));
	hyperText.GetHyperText(g_arNpcChat[talknum]->m_strTalk, g_arNpcChat[talknum]->m_sSize);
	hyperText.load();

	for(i = 0; i < hyperText.m_nCountLine; i++)
	{
		for(j = 0; j < hyperText.m_HyperText[i].m_strText.GetLength()-7; j++){

			temp = hyperText.m_HyperText[i].m_strText.Mid(j, 2);
			if(temp == "%S")
			{
				CGuildHouseWar *tete = g_arGuildHouseWar[iCity];
				if("GN" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// ��� �̸��� �ִ´�.
				{
//					hyperText.m_HyperText[i].m_strText.Replace("%SGN%", g_arGuildHouseWar[iCity]->m_TopList[index].strGuildName);
					TempSayBuf.AddString(g_arGuildHouseWar[iCity]->m_TopList[index].strGuildName);
					index++;
					tCount++;
				}
				else if("STM" == hyperText.m_HyperText[i].m_strText.Mid(j+2, 2))	// 
				{					
					tCount++;
					nTime = (int)g_arGuildHouseWar[iCity]->m_CurrentGuild.dwTimer/1000;
					temp.Format("%d", nTime);
					TempSayBuf.AddString( (LPTSTR)(LPCTSTR)temp );
//					hyperText.m_HyperText[i].m_strText.Replace("%STM%", temp);
				}
			}		
		}
		
	}

//	nLen = hyperText.save(strTemp);


	TempBuf.Add(CLIENT_EVENT_SAY);
	TempBuf.Add((BYTE)SUCCESS);
	TempBuf.Add((short)nNum);
	TempBuf.Add(tCount);
	TempBuf.AddData(TempSayBuf, TempSayBuf.GetLength());

//	TempBuf.AddLongString(strTemp, nLen);

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////
//
//	
//
void USER::CheckQuestEventZoneWarEnd()
{
	g_QuestEventZone.CheckSingleEventZoneWarEnd(m_curz);
}

//////////////////////////////////////////////////////////////////////
//
//	�о�Ⱓ�� ��� ������ Agent�� �����ϰ� �ʿ��� �������´�.
//
void USER::CheckGuildHouseWarEnd()
{
	int index = -1;
	index = GetCityNumForVirtualRoom(m_curz);

//	USER *pUser = NULL;

	if(index >= 0 && index < g_arGuildHouseWar.GetSize())
	{
		if(!g_arGuildHouseWar[index]) return;

		CheckMaxValue(g_arGuildHouseWar[index]->m_CurrentGuild.iCurValue, 1);	// �̰� ���� ���������� ȣ��Ǳ⶧����

		if(g_arGuildHouseWar[index]->CheckGuildMopEnd())						// ���� �� �׾���. 
		{			
			g_arGuildHouseWar[index]->SendTownPotalMsg(m_pCom);
			g_arGuildHouseWar[index]->CheckGuildHouseRank();					// Agent�� �����Ѵ�.
																				// ������� ǥ�ø� Ǯ���ش�.			
			g_arGuildHouseWar[index]->SetNpcListToWarEnd();
//			g_arGuildHouseWar[index]->SetUserListToWarEnd(m_pCom);				// ���ø� Ǯ���ش�.

//			g_arGuildHouseWar[index]->InitGuild();								// �����͸� ������.
/*
			::InterlockedExchange(&g_arGuildHouseWar[index]->m_CurrentGuild.lUsed, 0); 
*/		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//	������ �װų�, ���Ӽ����� ���� ���������� üũ...
//
void USER::CheckGuildUserListInGuildHouseWar()
{
	int i, uid = 0;

	int nCount = 0, nLen = 0;
	USER *pUser = NULL;

	int index = GetCityNumForVirtualRoom(m_curz);

	m_tGuildHouseWar = GUILD_WAR_AFFTER;
	
	if( index < 0 || index >= g_arGuildHouseWar.GetSize() )
	{
		return;
	}

	if(!g_arGuildHouseWar[index]) return;

//	if(m_curz == g_arGuildHouseWar[index]->m_CurrentGuild.iWarZone) TownPotal();

	for(i = 0; i < g_arGuildHouseWar[index]->m_CurrentGuild.iGuildNum; i++)
	{
		uid = g_arGuildHouseWar[index]->m_CurrentGuild.arUserList[i].uid;

		if( uid < 0 || uid >= MAX_USER ) continue;

		pUser = m_pCom->GetUserUid(uid);
		if(!pUser) continue;
		
		nLen = strlen(g_arGuildHouseWar[index]->m_CurrentGuild.arUserList[i].strUserName);
		if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) continue;

		if(strcmp(pUser->m_strUserID, g_arGuildHouseWar[index]->m_CurrentGuild.arUserList[i].strUserName) != 0)
		{
			pUser = GetUser(g_arGuildHouseWar[index]->m_CurrentGuild.arUserList[i].strUserName);
		}

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;

		if(pUser->m_tGuildHouseWar == GUILD_WARRING) nCount++;
	}

	if(nCount <= 0)
	{
		g_arGuildHouseWar[index]->CheckGuildHouseRank();					// Agent�� �����Ѵ�.
																			// ������� ǥ�ø� Ǯ���ش�.			
		g_arGuildHouseWar[index]->SetNpcListToWarEnd();
		g_arGuildHouseWar[index]->SetUserListToWarEnd(m_pCom);				// ���ø� Ǯ���ش�.

		g_arGuildHouseWar[index]->InitGuild();								// �����͸� ������.

		g_arGuildHouseWar[index]->m_CurrentGuild.dwIntervalTick = 0;
		::InterlockedExchange(&g_arGuildHouseWar[index]->m_CurrentGuild.lUsed, 0); 
	}
}

//////////////////////////////////////////////////////////////////////
//
//	�ش� ���� �����Ǵ� �Ͻ��� ������ �ɸ� ������ üũ
//
void USER::CheckInvalidGuildZone()
{
	int mapindex = 0;
	int i, index = 0;
	int type = CheckInvalidMapType();
	mapindex = GetGuildMapIndex(type);
	if(mapindex >= 0 && mapindex < g_arMapTable.GetSize())
	{
		index = g_arMapTable[mapindex]->m_sStoreID;

		if(index >= FORTRESS_BAND)
		{
			CGuildFortress* pFort = NULL;
			pFort = GetFortress(index);

			if(pFort) 
			{
				if(g_arMapTable[mapindex]->m_sStoreZone)
				{
					m_bLive = USER_DEAD;
					SetZoneIndex(m_curz);
					IsDeadUser();		
					m_sHP = m_sMaxHP;												//@@@@@@@@@@@ �ӽ��ڵ�
					m_bLive = USER_LIVE;
					return;
				}

				if(pFort->m_lUsed == 1)
				{
					m_bLive = USER_DEAD;
	//				m_sHP = 0;
					SetZoneIndex(m_curz);
					IsDeadUser();		
					m_sHP = m_sMaxHP;												//@@@@@@@@@@@ �ӽ��ڵ�
					m_bLive = USER_LIVE;
					SendSystemMsg( IDS_USER_CANT_REVIVE_IN_WAR, SYSTEM_NORMAL, TO_ME);
				}
			}
		}
/*		else 
		{
			CStore *pStore = NULL;

			index = g_arMapTable[mapindex]->m_sStoreIndex;

			pStore = GetStore(index);

			if(pStore)
			{
				if(pStore->m_lUsed == 1)								// �ش� �Ӽ� ���� ����� ����	
				{
	//				int temp = m_sHP;
					m_bLive = USER_DEAD;
	//				m_sHP = 0;
					SetZoneIndex(m_curz);
					IsDeadUser();		
					m_sHP = m_sMaxHP;												//@@@@@@@@@@@ �ӽ��ڵ�
					m_bLive = USER_LIVE;
					return;
				}
			}
		}
*/	}

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{		
		if(!g_arGuildFortress[i]) continue;

		if(m_curz == g_arGuildFortress[i]->m_iZone)
		{
			m_bLive = USER_DEAD;
			IsDeadUser();		
			m_sHP = m_sMaxHP;												//@@@@@@@@@@@ �ӽ��ڵ�
			m_bLive = USER_LIVE;
			return;
		}
	}

	for(i = 0; i < g_arGuildHouseWar.GetSize(); i++)
	{
		if(!g_arGuildHouseWar[i]) continue;

		if(m_curz == g_arGuildHouseWar[i]->m_CurrentGuild.iWarZone)
		{
//			int temp = m_sHP;
			m_bLive = USER_DEAD;
//			m_sHP = 0;
			SetZoneIndex(m_curz);
			IsDeadUser();		
			m_sHP = m_sMaxHP;												//@@@@@@@@@@@ �ӽ��ڵ�
			m_bLive = USER_LIVE;
			return;
		}
	}

	for(i = 0; i < g_arGuildHouse.GetSize(); i++)						// ����Ͽ콺 ���ε�.. ������尡 �ƴϸ�..
	{
		if(m_curz == g_arGuildHouse[i]->iZone)
		{
//			int temp = m_sHP;
			m_bLive = USER_DEAD;
//			m_sHP = 0;
			SetZoneIndex(m_curz);
			IsDeadUser();		
			m_sHP = m_sMaxHP;												//@@@@@@@@@@@ �ӽ��ڵ�
			m_bLive = USER_LIVE;
			return;
		}
	}

	for(i = 0; i < g_QuestEventZone.m_arEventZone.GetSize(); i++)
	{
		if(m_curz == g_QuestEventZone.m_arEventZone[i])
		{
			m_bLive = USER_DEAD;
			IsDeadUser();		
			m_sHP = m_sMaxHP;												//@@@@@@@@@@@ �ӽ��ڵ�
			m_bLive = USER_LIVE;
			return;
		}
	}
}
/*
void USER::CheckMemoryDB(char* strAccount, char *strUserId)
{
	CSharedMemory*	pSharedMemory;
	CMemUser*		pMD;
	int				mem_index = -1;
	int				nLen1, nLen2;
	CString			str;

	SYSTEMTIME time;
	GetLocalTime(&time);

	for( int i = 0; i < MAX_USER; i++ )
	{
		pSharedMemory = NULL;
		pSharedMemory = g_arSharedMemory[i];

		if(!pSharedMemory) continue;

		pMD = (CMemUser*)pSharedMemory->m_lpData;

		nLen = strlen( pMD->m_strUserID );
		if( nLen <= 0 || nLen > CHAR_NAME_LENGTH ) continue;

		if( strcmp( pMD->m_strUserID, strUserId ) == 0 )
		{
			// ���Ͽ� ����
			if ( g_ChatEnable[m_iModSid] == TRUE )
			{
				str.Format( "(%dM %dD %dH %dMIN)\tuid-%d str-%d uduid-%d savetime-%d id-%s\r\n",
					time.wMonth, time.wDay, time.wHour, time.wMinute, pMD->m_uid, pMD->m_sSTR, pMD->m_UB.m_uid, pMD->m_dwSaveTime, pMD->m_strUserID );
				
				// IKING 2002.1.
				//TRACE("*** bfile write log started:%s ***\n", strUserId);
				EnterCriticalSection( &m_CS_FileWrite );
				g_fpBackServer.Write( str, str.GetLength() );
				LeaveCriticalSection( &m_CS_FileWrite );
			}

			//TRACE("*** bfile write logged end:%s ***\n", strUserId);
			//

//			if(pMD->m_uid == -1) continue;		// ������ �� ������ �ٽ� �������� �ʴ´�.
//			if(pMD->m_sSTR == 0) continue;		// ��������

			UpdateMem2DB(pMD);
			UpdateBankMem2DB(pMD);
			UpdateAccountBankMem2DB(pMD);

			pMD->m_uid = -1;
			pMD->m_sSTR = 0;			
			pMD->m_UB.m_uid = -1;
			pMD->m_AB.m_uid = -1;
			pMD->m_dwSaveTime = 0;
			::ZeroMemory(pMD->m_strUserID, sizeof(pMD->m_strUserID));
		}
	}
}
*/
int USER::CheckMemoryDB(char *strUserId)
{
	CSharedMemory*	pSharedMemory;
	CMemUser*		pMD;
	int				mem_index = -1;
	int				nLen;
	int				user_count = 0;
	int				account_count = 0;
	int				ret_val = 0;
	CString			str;

	SYSTEMTIME time;
	GetLocalTime(&time);

	for( int i = 0; i < MAX_USER; i++ )
	{
		pSharedMemory = NULL;
		pMD = NULL;

		pSharedMemory = g_arSharedMemory[i];
		if(!pSharedMemory) continue;

		pMD = (CMemUser*)pSharedMemory->m_lpData;
		if(!pMD) continue;

		if(pMD->m_uid == -1 || pMD->m_sSTR == 0) continue;		// ������ �� ������ �ٽ� �������� �ʴ´�.
		if(pMD->m_strUserID[0] == 0) continue;

		nLen = strlen( strUserId );
		if( nLen <= 0 || nLen > CHAR_NAME_LENGTH ) continue;

		// ������ ����, ���̵� ���� - ���� �� ���̵�� �����ߴ� ���� ���� ���̴�.
		if( _stricmp( pMD->m_strUserID, strUserId ) == 0 && _stricmp( pMD->m_strAccount, m_strAccount ) == 0 )
		{
			user_count++;
			// ���Ͽ� ����
			if(user_count > 1)
			{
				GetLocalTime(&time);
				str.Format("(%04d-%02d-%02d %02d:%02d:%02d)\tuid-%d str-%d ubuid-%d count-%d id-%s\r\n",
					time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
					pMD->m_uid, pMD->m_sSTR, pMD->m_UB.m_uid, user_count, pMD->m_strUserID );
				
				// IKING 2002.1.
				EnterCriticalSection( &m_CS_FileWrite );
				g_fpBackServer.Write( str, str.GetLength() );
				LeaveCriticalSection( &m_CS_FileWrite );
			}

//			if( !Mem2Game(pMD) )			// Shared Memory -> Game ������
//			{
//				user_count--;
//			}

			UpdateMem2DB(pMD);		// ������ ���ؼ� DB���� ������
			UpdateBankMem2DB(pMD);
			UpdateAccountBankMem2DB(pMD);

			pMD->m_uid = -1;
			pMD->m_sSTR = 0;			
			pMD->m_UB.m_uid = -1;
			pMD->m_AB.m_uid = -1;
			pMD->m_dwSaveTime = 0;
			::ZeroMemory(pMD->m_strUserID, sizeof(pMD->m_strUserID));
			::ZeroMemory(pMD->m_strAccount, sizeof(pMD->m_strAccount));
		}
		// ������ ������ ���̵� �ٸ���.
		else if( _stricmp( pMD->m_strAccount, m_strAccount ) == 0 && _stricmp( pMD->m_strUserID, strUserId ) != 0 )
		{
			account_count++;

			if(account_count > 1)
			{
				GetLocalTime(&time);
				str.Format("(%04d-%02d-%02d %02d:%02d:%02d)\tuid-%d str-%d ubuid-%d count-%d id-%s (AccountBank)\r\n",
					time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
					pMD->m_uid, pMD->m_sSTR, pMD->m_AB.m_uid, account_count, pMD->m_strAccount );
				
				// IKING 2002.1.
				EnterCriticalSection( &m_CS_FileWrite );
				g_fpBackServer.Write( str, str.GetLength() );
				LeaveCriticalSection( &m_CS_FileWrite );
			}

//			if( !Mem2GameAccountBank(pMD) )
//			{
//				account_count++;
//			}

			UpdateMem2DB(pMD);		// ������ ���ؼ� DB���� ������
			UpdateBankMem2DB(pMD);
			UpdateAccountBankMem2DB(pMD);

			pMD->m_uid = -1;
			pMD->m_sSTR = 0;			
			pMD->m_UB.m_uid = -1;
			pMD->m_AB.m_uid = -1;
			pMD->m_dwSaveTime = 0;
			::ZeroMemory(pMD->m_strUserID, sizeof(pMD->m_strUserID));
			::ZeroMemory(pMD->m_strAccount, sizeof(pMD->m_strAccount));
		}
	}

//	if( user_count == 0 && account_count == 0 ) return 0;		// �Ƶ� ���� �͵� ������, ���� ���� �͵� ������.
																// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�.
//	if( user_count != 0 && account_count == 0 ) return 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ����� ������.
																// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.
//	if( user_count != 0 && account_count != 0 ) return 1;		// �Ƶڶ� ������ ���� ���� �־���, ������ ���� �Ƶڰ� �ٸ��͵� �־���.
																// �Ƶ�,����,��â ��� MemoryDB���� �����Դ�. DB���� �������� �ʴ´�.
//	if( user_count == 0 && account_count != 0 ) return 2;		// �Ƶڶ� ������ ���� ���� ������, ������ ���� ���� �־���.
																// �Ƶ�,����,��â ��� DB���� �����;� �Ѵ�. (���߿� ��â�� MemoryDB���� �������� ��ƾ�� �������̴�.)

	return 0;
}

//////////////////////////////////////////////////////////////////////
//
//	���� ����� ���÷� Ÿ����Ż ��ǥ�� ���Ѵ�.
//
CPoint USER::GetTownPotal(int &potal)
{
	int i, num = -1;
	int dx1 = 0, dy1 = 0;
	int dx2 = 0, dy2 = 0;
	int dx3 = 0, dy3 = 0;

	if(CheckInvalidMapType() != 12)				// ������� �ƴҶ��� ����Ͽ콺�� ��Ʈ������ üũ�Ѵ�
	{
		for(i = 0; i < g_arGuildFortress.GetSize(); i++)			// ���� Fortress�� ������ ������� üũ�Ѵ�.
		{
			if(!g_arGuildFortress[i]) continue;

			if(CheckGuildHouseUser(g_arGuildFortress[i]->m_sFortressID)) 
			{
				potal = g_arGuildFortress[i]->m_iZone;
				return CPoint(g_arGuildFortress[i]->m_iPotalX, g_arGuildFortress[i]->m_iPotalY);
			}
		}

		for(i = 0; i < g_arGuildHouse.GetSize(); i++)				// ���� ����Ͽ콺�� ������ ������� üũ�Ѵ�.
		{
			if(CheckGuildHouseUser(i + 1)) 
			{
				potal = g_arGuildHouse[i]->iZone;
				return CPoint(g_arGuildHouse[i]->iPotalX, g_arGuildHouse[i]->iPotalY);
			}
		}
	}

	for(i = 0; i < g_TownPotal.GetSize(); i++)					// �ƴϸ� ����� ���÷� �̵��Ѵ�.
	{
		if(g_TownPotal[i]->iZone == m_curz) { num = i; break; }
	}

	if(num < 0) 
	{
		potal = -1;
		return CPoint(-1, -1);
	}

	potal = g_TownPotal[num]->iPotalZone;

	CPoint temp = ConvertToClient(m_curx, m_cury);

	if(g_TownPotal[num]->iPotalX <= 0) return CPoint(g_TownPotal[num]->iPotalX1, g_TownPotal[num]->iPotalY1);
	else if(g_TownPotal[num]->iPotalX1 <= 0) return CPoint(g_TownPotal[num]->iPotalX, g_TownPotal[num]->iPotalY);
	else
	{
		dx1 = abs(g_TownPotal[num]->iPotalX - temp.x);
		dy1 = abs(g_TownPotal[num]->iPotalY - temp.y);

		dx2 = abs(g_TownPotal[num]->iPotalX1 - temp.x);
		dy2 = abs(g_TownPotal[num]->iPotalY1 - temp.y);

		i = myrand(0, 2);

		if( (dx1 + dy1) < (dx2 + dy2) )
		{
			if(potal == 400)	return CPoint(g_RandomTownPotal[2]->iPotal[i].x, g_RandomTownPotal[2]->iPotal[i].y);
			else				return CPoint(g_RandomTownPotal[0]->iPotal[i].x, g_RandomTownPotal[0]->iPotal[i].y);
			

//			return CPoint(g_TownPotal[num]->iPotalX, g_TownPotal[num]->iPotalY);
		}
		else
		{
			if( m_curz == 1 )	// ���̳�Ʈ,�糪��,�ڸḮ�� - �̹� �糪�尡 ���õ� ��Ȳ
			{
				dx3 = abs( 2463 - temp.x );
				dy3 = abs( 705 - temp.y);

				if( (dx3 + dy3) < (dx2 + dy2) )	// �ڸḮ�� �Ա��� �� �����ٸ�
				{
					potal = 26;
					return CPoint( 36, 64 );
				}
			}
			if(potal == 400)	return CPoint(g_RandomTownPotal[2]->iPotal[i].x, g_RandomTownPotal[2]->iPotal[i].y);
			else				return CPoint(g_RandomTownPotal[1]->iPotal[i].x, g_RandomTownPotal[1]->iPotal[i].y);

			if(potal == 300) CPoint(g_TownPotal[num]->iPotalX1, g_TownPotal[num]->iPotalY1);
		}
	}
	
	return CPoint(-1, -1);
}

/////////////////////////////////////////////////////////////////////////////////////
//	SpeedHack ����������� �˻��Ѵ�.
//
void USER::CheckSpeedHack()
{
	m_dwServerTick = GetTickCount();

	if(m_iTickCount == 0)
	{
		m_dwClientTick = GetTickCount();
		m_iTickCount++;
		return;
	}

	DWORD dwGap = 0;
	DWORD dwCurr = GetTickCount(); 

	dwGap = dwCurr - m_dwClientTick;
	TRACE("-----id = %s,  dwGap = %d\n", m_strUserID, dwGap);
	if(dwGap < 32000)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		
		CString str;
		str.Format( IDS_USER_SYSTEM_MSG02, time.wMonth, time.wDay, time.wHour, time.wMinute, m_strAccount, m_strUserID, dwGap);
		
		EnterCriticalSection( &m_CS_FileWrite );
		g_fpSpeedHack.Write(str, str.GetLength());
		LeaveCriticalSection( &m_CS_FileWrite );
		
		SendSystemMsg( IDS_USER_SPEED_HACK, SYSTEM_SPECIAL, TO_ME);
	}

	m_dwClientTick = GetTickCount();
}

/////////////////////////////////////////////////////////////////////////////////////
//	�ش� ����� DB�� �����鸦 DB���� �����Ѵ�.
//
BOOL USER::DeleteGuildDB()
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		

	if(m_dwGuild <= 0 || m_dwGuild >= g_arGuildData.GetSize()) return FALSE;
	if(!m_bGuildMaster) return FALSE;
	if(!g_arGuildData[m_dwGuild]) return FALSE;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call DELETE_GUILD (%d, \'%s\')}"), m_dwGuild, m_strUserID); 

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
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
	else
	{
		//if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		//BREAKPOINT();

		//g_DB[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

//void USER::RemoveReceivedData()
//{
//	POSITION pos, pos2;
//	WAIT_RECV_DATA *pWrd;
//
//	return;
//
//	EnterCriticalSection(&m_CS_ReceiveData[m_iSidMod]);
//
//	for ( pos = RecvPtrList[m_iSidMod].GetHeadPosition();  pos != NULL; )
//	{
//		pos2 = pos;
//		pWrd = (WAIT_RECV_DATA *)RecvPtrList[m_iSidMod].GetNext( pos );
//		if ( pWrd )
//		{
//			if ( pWrd->usn == m_Sid )
//			{
//				delete pWrd;
//				pWrd = NULL;
//
//				RecvPtrList[m_iSidMod].RemoveAt(pos2);
//			}
//		}
//	};
//
//	nRecvDataLength[m_iSidMod] = RecvPtrList[m_iSidMod].GetCount();
//
//	LeaveCriticalSection(&m_CS_ReceiveData[m_iSidMod]);
//}

int USER::SockCloseProcess(int nError)
{
	if ( m_UserStatus == 0  ) return 0;

	m_UserStatus = 0;

	if (g_pMainDlg->m_bLogOutExit == TRUE)
	{
		if ( m_state == STATE_GAMESTARTED )
//			LogOutWithDbServer();
			LogOut();

		m_state = STATE_DISCONNECTED;

		CBSocket::SockCloseProcess();  // UserList���� ���� 

		// ��ġ �������� �����...
		g_pMainDlg->UserFree( m_uid );

		m_UserFlag = FALSE;
		return 0;
	}

	if ( m_state == STATE_GAMESTARTED )
		//LogOutWithDbServer();
		LogOut();

	m_state = STATE_DISCONNECTED;

	CBSocket::SockCloseProcess();  // UserList���� ���� 

	// ��ġ �������� �����...
	g_pMainDlg->UserFree( m_uid );

	m_UserFlag = FALSE;

	return 1;
}

void USER::OnClose(int nErrorCode) 
{
	CBSocket::B_OnClose(nErrorCode);
}

void USER::OnSend(int nErrorCode) 
{
	if ( m_SockFlag != 1 || m_UserFlag == FALSE ) return;

	CBSocket::B_OnSend( nErrorCode );

	if ( m_SockFlag == 0 )
	{
		SoftClose();
	}
}

int USER::AcceptProcess()
{
	return 1;
}

void USER::StopAction()
{
}

/////////////////////////////////////////////////////////////////////////////////////
//	����Ͽ콺�� ������ ������ �����Ѵ�.
//
BOOL USER::CheckGuildHouseUser(int num)
{
	if(m_dwGuild <= 0) return FALSE;

	if(num >= FORTRESS_BAND)
	{
		CGuildFortress *pFort = NULL;
		pFort = GetFortress(num);

		if(pFort)
		{
			if(pFort->m_iGuildSid == m_dwGuild) return TRUE;
		}
	}
	else
	{
		for(int i = 0; i < g_arGuildHouse.GetSize(); i++)
		{
			if(g_arGuildHouse[i]->iGuild <= 0) continue;

			if(g_arGuildHouse[i]->iSid == num) 
			{			
				if(g_arGuildHouse[i]->iGuild == m_dwGuild) return TRUE;
			}
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
//	�ڵ����Ʈ�� ����ϱ����� ��ü Ǯ����Ʈ�� �����ش�.
//
void USER::TeleportReq()
{
	int i, iCount = 0;

	if(!CheckHandyGate()) 
	{
		SendSystemMsg( IDS_USER_NO_HANDYGATE, SYSTEM_ERROR, TO_ME);
		return;
	}

	CBufferEx TempBuf;
	TempBuf.Add(TELEPORT_RESULT);

	for(i = 1; i < TEL_MAX_COUNT; i++)
	{
		if(m_strTeleport[i].iSid != 0) iCount++;
	}

	TempBuf.Add((BYTE)(iCount));

	for(i = 1; i < TEL_MAX_COUNT; i++)
	{
		if(m_strTeleport[i].iSid != 0)
		{
			TempBuf.Add((BYTE)m_strTeleport[i].iSid);
			TempBuf.AddString(m_strTeleport[i].TelName);
			TempBuf.Add((int)m_strTeleport[i].z);//yskang 0.9 handy ���� �ڵ�� �̵� �Ұ������� ȸ������ ǥ���ϱ� ���Ͽ�
		}
	}

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////////////
//	DB�� ����� ������ �迭�� ����
//
void USER::StrToUserTel(TCHAR *pBuf)
{
	int index = 0;
	int temp = 0;
	int nLength = 0;

//	for(int i = 1; i < TEL_MAX_COUNT; i++)
	do
	{
		temp = GetByte(pBuf, index);
		if(temp <= 0 || temp >= TEL_MAX_COUNT) break;

		m_strTeleport[temp].iSid = temp;
		m_strTeleport[temp].x = GetShort(pBuf, index);
		m_strTeleport[temp].y = GetShort(pBuf, index);
		m_strTeleport[temp].z = GetShort(pBuf, index);

		nLength = GetByte(pBuf, index);
		if(nLength < 0 || nLength > TEL_NAME_LENGTH) break;
	
		GetString(m_strTeleport[temp].TelName, pBuf, nLength, index);

		m_strTeleport[temp].TelName[TEL_NAME_LENGTH + 1] = 0;

	} while(TRUE);
}

/////////////////////////////////////////////////////////////////////////////////////
//	����Ÿ�� DB�� ���� 
//
void USER::UserTelToStr(TCHAR *pBuf)
{
	int index = 0;

	for(int i = 1; i < TEL_MAX_COUNT; i++)
	{
		if(m_strTeleport[i].iSid == 0) continue;

		SetByte(pBuf, m_strTeleport[i].iSid, index);
		SetShort(pBuf, m_strTeleport[i].x, index);
		SetShort(pBuf, m_strTeleport[i].y, index);
		SetShort(pBuf, m_strTeleport[i].z, index);
		SetVarString(pBuf, m_strTeleport[i].TelName, strlen( m_strTeleport[i].TelName ), index );
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//	����Ÿ�� �ʱ�ȭ�Ѵ�.
//
void USER::InitTelList(TeleportList *strTel)
{
	for(int i = 0; i < TEL_MAX_COUNT; i++)
	{
		strTel[i].iSid = 0;
		strTel[i].x = 0;
		strTel[i].y = 0;
		strTel[i].z = 0;
		::ZeroMemory(strTel[i].TelName, sizeof(strTel[i].TelName));		
	}

	m_lCopyUsed = 0;
	m_CopyUid = 0;				// copy chip �� ���Ǵ� ������
	m_tIndex = 0;
	::ZeroMemory(m_strCopyUser, sizeof(m_strCopyUser));	
}

/////////////////////////////////////////////////////////////////////////////////////
//	�� ��ǥ�� ����ϰų� �����Ѵ�.
//
void USER::TelportEdit(TCHAR *pBuf)
{
	int i, mapindex = 0;
	int index = 0;
	BYTE tIndex = 0;
	
	CPoint pt;
	int nLen = 0, type = 0;
	BOOL bSuccess = FALSE;	
	TCHAR szName[TEL_NAME_LENGTH + 1];	
	CBufferEx TempBuf;
	
	if(!CheckHandyGate()) 
	{
		SendSystemMsg( IDS_USER_NO_HANDYGATE, SYSTEM_ERROR, TO_ME);
		return;
	}

	BYTE tType = GetByte(pBuf, index);

	::ZeroMemory(szName, sizeof(szName));

	if(tType == TEL_ADD_EDIT)
	{	
		for(i = 0; i < g_arGuildHouseWar.GetSize(); i++)
		{
			if(!g_arGuildHouseWar[i]) continue;

			if(m_curz == g_arGuildHouseWar[i]->m_CurrentGuild.iWarZone)
			{
				SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
		}

		for(i = 0; i < g_arGuildHouse.GetSize(); i++)
		{
			if(m_curz == g_arGuildHouse[i]->iZone)
			{
				SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
		}

		for(i = 0; i < g_QuestEventZone.m_arEventZone.GetSize(); i++)
		{
			if(m_curz == g_QuestEventZone.m_arEventZone[i])
			{
				SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
		}

		for(i = 0; i < g_arGuildFortress.GetSize(); i++)
		{
			//break;
			if(!g_arGuildFortress[i]) continue;

			if(m_curz == g_arGuildFortress[i]->m_iZone)
			{
				SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);//yskang handy don't save
				goto go_result;
			}

			type = CheckInvalidMapType();
			mapindex = GetGuildMapIndex(type);
			if(mapindex >= 0 && mapindex < g_arMapTable.GetSize())
			{
				if(g_arMapTable[mapindex])
				{
					if(g_arMapTable[mapindex]->m_sStoreID >= FORTRESS_BAND)
					{
						if(g_arMapTable[mapindex]->m_sStoreZone == 1) 
						{
							SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
							goto go_result;
						}
					}
				}
			}			
		}

		if( m_curz == 40 || m_curz == 43 || m_curz == 44 || m_curz == 45 || m_curz == 57 || m_curz == 58 || m_curz == 59 )		// �����
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		if( m_curz == 56 || m_curz == 16 || m_curz == 12 || m_curz == 19  || m_curz == 36 || m_curz == 49 || m_curz == 1007 || m_curz == 409 || m_curz == 416) 		// ������(56), �̺�Ʈ����(16,12,19)�� ���������� �ʴ´�.
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}
//�ر��̵�57�㴫�㱣��...
		if( m_curz == 10 || m_curz == 3 || m_curz == 24 || m_curz == 20  || m_curz == 29 || m_curz == 28 || m_curz == 403 || m_curz == 303 || m_curz == 304 )		// ������(56), �̺�Ʈ����(16,12,19)�� ���������� �ʴ´�.
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}
		nLen = GetVarString(sizeof(szName), szName, pBuf, index);
		if(nLen <= 0 || nLen > TEL_NAME_LENGTH) goto go_result;

		i = TEL_MAX_COUNT - 1;

		do
		{
			if(strcmp(m_strTeleport[i].TelName, szName) == 0)
			{
				SendSystemMsg( IDS_USER_ALREADY_HANDYGATE_SAVE, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
			i--;
		} while(i >= 1);

		for(i = 1; i < TEL_MAX_COUNT; i++)
		{
			if(m_strTeleport[i].iSid == 0) 
			{
				pt = ConvertToClient(m_curx, m_cury);
				if(pt.x <= -1 || pt.y <= -1) goto go_result;

				m_strTeleport[i].iSid = i;
				m_strTeleport[i].x = pt.x;
				m_strTeleport[i].y = pt.y;
				m_strTeleport[i].z = m_curz;

				strncpy(m_strTeleport[i].TelName, szName, nLen);
				tIndex = (BYTE)i;
				bSuccess = TRUE;

				SendSystemMsg( IDS_USER_HANDYGATE_SAVE, SYSTEM_NORMAL, TO_ME);

				break;
			}
		}		
	}
	else if(tType == TEL_DEL_EDIT)
	{
		tIndex = GetByte(pBuf, index);
		if(tIndex <= 0 || tIndex >= TEL_MAX_COUNT) goto go_result;

		nLen = GetVarString(sizeof(szName), szName, pBuf, index);
		if(nLen <= 0 || nLen > TEL_NAME_LENGTH) goto go_result;

		if(strcmp(m_strTeleport[tIndex].TelName, szName) != 0) goto go_result;

		bSuccess = TRUE;
		m_strTeleport[tIndex].iSid = 0;	
		m_strTeleport[tIndex].x = 0;
		m_strTeleport[tIndex].y = 0;
		m_strTeleport[tIndex].z = 0;
		::ZeroMemory(m_strTeleport[tIndex].TelName, nLen);

		SendSystemMsg( IDS_USER_HANDYGATE_DELETE, SYSTEM_NORMAL, TO_ME);
	}

go_result:
	TempBuf.Add(TELEPORT_EDIT_RESULT);

	if(!bSuccess)
	{
		TempBuf.Add((BYTE)0x00);				// ����
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	TempBuf.Add((BYTE)0x01);					// ����						
	TempBuf.Add(tType);							// ���� �Ǵ� �߰� 
	TempBuf.Add(tIndex);						// �ش� �ε��� 
	TempBuf.AddString(szName);					// 
	TempBuf.Add((int)m_curz);//yskang 0.9 handy ���� �ڵ�� �̵� �Ұ������� ȸ������ ǥ���ϱ� ���Ͽ� 
	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////////////
//	����� �ӵ� ������ �׽�Ʈ�ϱ�����
//
void USER::ChangeUserSpeed(TCHAR *pBuf)
{
	return;
/*
	CBufferEx TempBuf;

	int index = 0;

	short step = atoi(pBuf);

	if(step < 0 || step > 500) step = 0;

	TempBuf.Add(SET_SPEED_MONSTER);

	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add(step);

	SendInsight(TempBuf, TempBuf.GetLength());

	SightRecalc();
*/
}

/////////////////////////////////////////////////////////////////////////////////////
//	�ڵ����Ʈ�� ����Ѵ�.
//
void USER::GetHanyGate(int slot, int sid)
{
	if(m_dwGuild >= 1 && m_dwGuild < g_arGuildData.GetSize())// ��� â�� ������� ���¿����� �̵��� ���Ѵ�.
	{
		if(!g_arGuildData[m_dwGuild]) return;

		if(g_arGuildData[m_dwGuild]->m_lUsed == 1)			// ��� â�� ������̶��
		{
			int nLen = strlen(g_arGuildData[m_dwGuild]->m_strUsedUser);
			if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;// �ڱ����� üũ		

			if(strcmp(m_strUserID, g_arGuildData[m_dwGuild]->m_strUsedUser) == 0) return;
		}
	}			
	
	int i;
	CPoint pt;

	if(slot < EQUIP_ITEM_NUM || slot >= TOTAL_INVEN_MAX) return;

	int sSid = m_UserItem[slot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return;

	if(g_arItemTable[sSid]->m_byWear != 105)		// �ڵ����Ʈ
	{
		SendSystemMsg( IDS_USER_NO_HANDYGATE, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(m_UserItem[slot].sDuration <= 0) 
	{
		SendSystemMsg( IDS_USER_FIRST_REPAIR, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(sid <= 0 || sid >= TEL_MAX_COUNT) return;

	if(m_strTeleport[sid].iSid == 0) return;

	int x = m_strTeleport[sid].x;
	int y = m_strTeleport[sid].y;

	for(i = 0; i < g_arGuildHouseWar.GetSize(); i++)		// �̺�Ʈ ������ ���ų� Ż�⸦ ���´�.
	{
		if(!g_arGuildHouseWar[i]) continue;

		if(m_strTeleport[sid].z == g_arGuildHouseWar[i]->m_CurrentGuild.iWarZone)
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			return;
		}
		else if(m_curz == g_arGuildHouseWar[i]->m_CurrentGuild.iWarZone)
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			return;
		}
	}

	for(i = 0; i < g_arGuildHouse.GetSize(); i++)
	{
		if(m_strTeleport[sid].z == g_arGuildHouse[i]->iZone)
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			return;
		}
	}
	

	for(i = 0; i < g_QuestEventZone.m_arEventZone.GetSize(); i++)
	{
		if(g_QuestEventZone.m_arEventZone[i] == m_curz)
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			return;
		}
	}

	if( m_strTeleport[sid].z == 16 || m_strTeleport[sid].z == 12 || m_strTeleport[sid].z == 19 || m_strTeleport[sid].z == 36 || m_strTeleport[sid].z == 49 || m_strTeleport[sid].z == 1007)// �̺�Ʈ����(16,12,19)�� ���� ����
	{
		SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
		return;
	}
	//�ر��̵�57�㴫�㱣��...
	if( m_strTeleport[sid].z == 10 || m_strTeleport[sid].z == 3 || m_strTeleport[sid].z == 24 || m_strTeleport[sid].z == 20  || m_strTeleport[sid].z == 29 || m_strTeleport[sid].z == 28 || m_strTeleport[sid].z == 403 || m_strTeleport[sid].z == 303 || m_strTeleport[sid].z == 304 )		// ������(56), �̺�Ʈ����(16,12,19)�� ���������� �ʴ´�.
	{
		SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
		return ;
	}

	// alisia - �ٸ� �������δ� �ڵ����Ʈ�� �̵��� �Ұ����ϴ�.
	if( !IsZoneInThisServer( m_strTeleport[sid].z ) )
	{
		SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
		return;
	}
	// alisia

	//------------------------------------------------------------------------------------------------
	// ��Ʈ���� �������δ� �ڵ� �Ұ� yskang handy
	int zzz = -1;
	for(int j = 0; j < g_zone.GetSize(); j++)
	{
		if( g_zone[j]->m_Zone == m_strTeleport[sid].z )
		{
			zzz = j;
			break;
		}
	}
	if(zzz < 0 || zzz >= g_zone.GetSize()) return;
	CPoint ptPotal = ConvertToServerByZone(m_strTeleport[sid].z, x, y);
	if(ptPotal.x == -1 || ptPotal.y == -1 ) return;
	ptPotal = FindNearAvailablePoint_S(m_strTeleport[sid].z,ptPotal.x, ptPotal.y);
	if(ptPotal.x == -1 || ptPotal.y == -1 ) return;

	MAP* pMap = g_zone[zzz];
	if(pMap == NULL) return;
	int cx = pMap->m_sizeMap.cx;
	int cy = pMap->m_sizeMap.cy;
	if(ptPotal.x < 0 || ptPotal.x >= pMap->m_sizeMap.cx ) return;
	if(ptPotal.y < 0 || ptPotal.y >= pMap->m_sizeMap.cy ) return;

	int iType = ((g_zone[zzz]->m_pMap[ptPotal.x][ptPotal.y].m_dwType & 0xFF00 ) >> 8);

	if(iType == 8 || iType == 10 || iType == 15) 
	{
		SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
		return;
	}
	//---------------------------------------------------------------------------------------------------

	if(m_strTeleport[sid].z != m_curz) 
	{
		ZoneMoveReq(m_strTeleport[sid].z, x, y);
		if(m_strTeleport[sid].z == m_curz)
		{
			SendDuration((BYTE)slot, 1);
//			m_UserItem[slot].sDuration -= 1;			// �ѹ� ��뿡 1�� �������� �ٿ�������.
//			if(m_UserItem[slot].sDuration < 0) m_UserItem[slot].sDuration = 0;
		}
	}
	else 
	{
		int type = 0;

		CPoint ptPotal = ConvertToServer(x, y);			// ���� ���� ���� ��ǥ��ȭ�� ...

		pt = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);
		if(pt.x <= -1 || pt.y <= -1) return;

		type = ((g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_dwType & 0xFF00 ) >> 8);

		if(!CheckInvalidZoneState(type)) return;
/*
		if(type > 1 && type < 8)
		{
			int index = g_arMapTable[type]->m_sStoreIndex;
			if(index < 0 || index >= g_arStore.GetSize()) return; 
			if(g_arStore[index]->m_lUsed == 1)	return; // ��� �������̸� ��������.
			// ������ �߰�...
		}
*/					
		if(Teleport(pt.x, pt.y) == FALSE) return;

		SendDuration((BYTE)slot, 1);
//		m_UserItem[slot].sDuration -= 1;			// �ѹ� ��뿡 1�� �������� �ٿ�������.
//		if(m_UserItem[slot].sDuration < 0) m_UserItem[slot].sDuration = 0;
	}

//	SendDuration((BYTE)slot, -1);
}

BOOL USER::CheckHandyGate()
{
	int iSid = 0;

	for(int i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
	{
		iSid = m_UserItem[i].sSid;
		if(iSid < 0 || iSid >= g_arItemTable.GetSize()) continue;
		if(g_arItemTable[iSid]->m_byWear == 105)
		{
			if(m_UserItem[i].sDuration <= 0) continue;

			return TRUE;// �ڵ����Ʈ
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
//	ī��Ĩ�� �̿��ϱ����� ������ ���Ǹ� ���Ѵ�.
//
void USER::TeleportCopyReq(TCHAR *pBuf)
{
	// 02-10-12 by Youn Gyu
	int type = CheckInvalidMapType();
	if(!CheckInvalidZoneState(type)) return;

	if(InterlockedCompareExchange((LONG*)&m_lCopyUsed, (long)1, (long)0)  == (long)0 ) 	// ���� ī��Ĩ�� ������ ������
	{
		int		i, index = 0;
		
		BYTE	tSlot;					// ����ϰ��� �ϴ� ������

		USER	*pUser = NULL;
		BOOL	bFlag = FALSE;

		TCHAR	szName[TEL_NAME_LENGTH + 1];	

		CBufferEx TempBuf, TempBuf1;

		m_CopyUid = 0;								// ����� �ʱ�ȭ�Ѵ�.
		m_tIndex=  0;
		::ZeroMemory(m_strCopyUser, sizeof(m_strCopyUser));
		::ZeroMemory(szName, sizeof(szName));

		short sSid = 0;
		int nLength = 0;

		if(!CheckHandyGate()) 
		{
			InterlockedExchange(&m_lCopyUsed, 0);	// ���� ����				 
			SendSystemMsg( IDS_USER_NO_HANDYGATE, SYSTEM_ERROR, TO_ME);
			return;
		}

		m_CopyUid = GetInt(pBuf, index);

	//	if(m_CopyUid < 0 || m_CopyUid >= INVALID_BAND) goto go_result;
		if(m_CopyUid < USER_BAND || m_CopyUid >= NPC_BAND) goto go_result;

		nLength = GetVarString(CHAR_NAME_LENGTH, m_strCopyUser, pBuf, index);
		if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) goto go_result;

		pUser = GetUser(m_CopyUid - USER_BAND);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED)
		{
			SendSystemMsg( IDS_USER_NO_USER_ID, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		if(strcmp(pUser->m_strUserID, m_strCopyUser) != 0)
		{
			SendSystemMsg( IDS_USER_INVALID_USER_ID, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		if(IsThereUser(pUser) == FALSE)
		{
			SendSystemMsg( IDS_USER_SEE_EACH_OTHER, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		tSlot = GetByte(pBuf, index);
		if(tSlot < EQUIP_ITEM_NUM || tSlot >= TOTAL_INVEN_MAX) goto go_result;

		sSid = m_UserItem[tSlot].sSid;
		if(sSid < 0 || sSid >= g_arItemTable.GetSize()) goto go_result;
        if (sSid==633) 
		{
			SendSystemMsg( IDS_USER_NO_COPYCHIP, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}
		if(g_arItemTable[sSid]->m_byWear != 106)
		{
			SendSystemMsg( IDS_USER_NO_COPYCHIP, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		m_tIndex = GetByte(pBuf, index);
		if(m_tIndex <= 0 || m_tIndex >= TEL_MAX_COUNT) goto go_result;

		nLength = GetVarString(sizeof(szName), szName, pBuf, index);
		if(nLength <= 0 || nLength > TEL_NAME_LENGTH) goto go_result;

		if(m_strTeleport[m_tIndex].iSid <= 0)
		{
			SendSystemMsg( IDS_USER_INVALID_AXIS, SYSTEM_ERROR, TO_ME);
			goto go_result;				// �ش� ���� ���� ���ߴٸ�	
		}

		if( m_strTeleport[m_tIndex].z == 16 || m_strTeleport[m_tIndex].z == 12 || m_strTeleport[m_tIndex].z == 19 )	// �̺�Ʈ����(16,12,19)�� ���� ����
		{
			SendSystemMsg( IDS_USER_NO_HANDYGATE_AREA, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		if(strcmp(m_strTeleport[m_tIndex].TelName, szName) != 0)
		{
			SendSystemMsg( IDS_USER_REVIEW_THIS_AXIS, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		for(i = 0; i < g_arGuildHouseWar.GetSize(); i++)
		{
			if(!g_arGuildHouseWar[i]) continue;

			if(m_strTeleport[m_tIndex].z == g_arGuildHouseWar[i]->m_CurrentGuild.iWarZone)
			{
				SendSystemMsg( IDS_USER_CANT_USE_AREA, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
		}

		for(i = 0; i < g_QuestEventZone.m_arEventZone.GetSize(); i++)
		{
			if(m_strTeleport[m_tIndex].z == g_QuestEventZone.m_arEventZone[i])
			{
				SendSystemMsg( IDS_USER_CANT_USE_AREA, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
		}

		for(i = 0; i < g_arGuildHouse.GetSize(); i++)
		{
			if(m_strTeleport[m_tIndex].z == g_arGuildHouse[i]->iZone)
			{
				SendSystemMsg( IDS_USER_CANT_USE_AREA, SYSTEM_ERROR, TO_ME);
				goto go_result;
			}
		}

		bFlag = TRUE;

go_result:
		if(!bFlag)
		{
			InterlockedExchange(&m_lCopyUsed, 0);		// ���� ����

			m_CopyUid = 0;								// ����� �ʱ�ȭ�Ѵ�.
			m_tIndex=  0;
			::ZeroMemory(m_strCopyUser, sizeof(m_strCopyUser));
			return;
		}

		TempBuf.Add(TELEPORT_COPY_DIALOG_REQ);
		TempBuf.Add((BYTE)0x02);						// �� ��û��
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.AddString(m_strUserID);
		pUser->Send(TempBuf, TempBuf.GetLength());

		TempBuf1.Add(TELEPORT_COPY_DIALOG_REQ);
		TempBuf1.Add((BYTE)0x01);						// ��û��
		TempBuf1.Add(pUser->m_uid + USER_BAND);
		TempBuf1.AddString(pUser->m_strUserID);
		Send(TempBuf1, TempBuf1.GetLength());
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//	ī��Ĩ������ ���� �������θ� �����Ѵ�.
//
void USER::TeleportCopyResult(TCHAR *pBuf)
{
	int uid = 0;
	int tSlot = 0;
	int index = 0, nLen = 0;
	int z = 0, x = 0, y=  0;

	BOOL bFlag = FALSE;
	BYTE tRefuse1 = 0x04, tRefuse2 = 0x04;

	CBufferEx TempBuf, TempBuf2;

	TCHAR	szName[CHAR_NAME_LENGTH + 1];	
	USER *pUser = NULL;

	CPoint pt;

	::ZeroMemory(szName, sizeof(szName));

	BYTE tType = GetByte(pBuf, index);	

	uid = GetInt(pBuf, index);
//	if(uid < 0 || uid >= INVALID_BAND) goto go_result;
	if(uid < USER_BAND || uid >= NPC_BAND) return;

	nLen = GetVarString(sizeof(szName), szName, pBuf, index);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;

	pUser = GetUser(uid - USER_BAND);

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;

	if(strcmp(pUser->m_strUserID, szName) != 0)	return;

	if(tType != 1) 
	{ 
		tRefuse1 = 0x03; 
		tRefuse2 = 0x02; // ���, ����
		goto go_result; 
	}						// ����...

	if(InterlockedCompareExchange((LONG*)&pUser->m_lCopyUsed, (long)0, (long)1)  == (long)1 ) 	// ���� ī��Ĩ�� ������ �־��
	{
		tSlot = pUser->CheckCopyChip();		// ��û���� �ڵ����Ʈ�� �ֳ� ���� �ٽ� Ȯ��
		if(tSlot < 0) goto go_result;

		if(pUser->m_tIndex <= 0 || pUser->m_tIndex >= TEL_MAX_COUNT) goto go_result;

		if(pUser->m_strTeleport[pUser->m_tIndex].iSid == 0) goto go_result;

		z = pUser->m_strTeleport[pUser->m_tIndex].z;
		x = pUser->m_strTeleport[pUser->m_tIndex].x;
		y = pUser->m_strTeleport[pUser->m_tIndex].y;
		
		index = IsMovableTel_S(z, x, y);
		if(index < 2) 
		{
			pUser->SendSystemMsg( IDS_USER_MOVE_FAIL, SYSTEM_ERROR, TO_ME);
			SendSystemMsg( IDS_USER_MOVE_FAIL, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		if(!pUser->SendCopyTel(z, x, y))
		{
			pUser->SendSystemMsg( IDS_USER_MOVE_FAIL, SYSTEM_ERROR, TO_ME);
			SendSystemMsg( IDS_USER_MOVE_FAIL, SYSTEM_ERROR, TO_ME);
			goto go_result;
		}

		bFlag = TRUE;
	}
go_result:

	if(!bFlag)
	{
		TempBuf.Add(TELEPORT_COPY_RESULT);
		TempBuf.Add(tRefuse1);						// ����...
		Send(TempBuf, TempBuf.GetLength());

		TempBuf2.Add(TELEPORT_COPY_RESULT);
		TempBuf2.Add(tRefuse2);						// ����...		
		if(pUser)
		{
			pUser->Send(TempBuf2, TempBuf2.GetLength());
			InterlockedExchange(&pUser->m_lCopyUsed, 0);// ���� ����
		}

		InterlockedExchange(&m_lCopyUsed, 0);		// ���� ����
		return;
	}

	TempBuf2.Add(TELEPORT_COPY_RESULT);
	TempBuf2.Add((BYTE)0x01);
	pUser->Send(TempBuf2, TempBuf2.GetLength());

	TempBuf.Add(ITEM_USE_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add((BYTE)QUICK_ITEM_TELEPORT);
	TempBuf.Add((BYTE)tSlot);

	pUser->m_UserItem[tSlot].sCount -= 1;		//
	if(pUser->m_UserItem[tSlot].sCount <= 0) { ReSetItemSlot( &pUser->m_UserItem[tSlot] ); }

	TempBuf.Add(pUser->m_UserItem[tSlot].sLevel);
	TempBuf.Add(pUser->m_UserItem[tSlot].sSid);
	TempBuf.Add(pUser->m_UserItem[tSlot].sDuration);
	TempBuf.Add(pUser->m_UserItem[tSlot].sBullNum);
	TempBuf.Add(pUser->m_UserItem[tSlot].sCount);
	for(int i = 0; i < MAGIC_NUM; i++) TempBuf.Add(pUser->m_UserItem[tSlot].tMagic[i]);
	TempBuf.Add(pUser->m_UserItem[tSlot].tIQ);
	pUser->Send(TempBuf, TempBuf.GetLength());
	pUser->GetRecoverySpeed();							// ȸ���ӵ� üũ...

	pt = pUser->ConvertToClient(pUser->m_curx, pUser->m_cury);
	
	if(!SendCopyTel(pUser->m_curz, pt.x, pt.y, TRUE))
	{
		CString strMsg = _T("");
		strMsg.Format( IDS_USER_WHO_MOVE_FAIL, m_strUserID);
		pUser->SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
//		pUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ERROR, TO_ME);
		SendSystemMsg( IDS_USER_MOVE_FAIL, SYSTEM_ERROR, TO_ME);

		InterlockedExchange(&pUser->m_lCopyUsed, 0);		// ���� ����
		return;
	}

	InterlockedExchange(&m_lCopyUsed, 0);		// ���� ����
}

/////////////////////////////////////////////////////////////////////////////////////
//	�Բ� �̵��Ѵ�.
//
BOOL USER::SendCopyTel(int z, int x, int y, BOOL bSuccess)
{
	int i, type = 0;

	CPoint pt;

	if( !IsZoneInThisServer(z) ) return FALSE;

	if(x < 0 || y < 0) return FALSE;

	if(z != m_curz)
	{
		ZoneMoveReq(z, x, y);
		if(z != m_curz) 
		{
			if(bSuccess) { DoubleCopyTel(z, x, y, TRUE); return TRUE; }
			return FALSE;
		}
	}
	else 
	{
		int rand = 0, type = 0;

		CPoint ptPotal = ConvertToServer(x, y);			// ���� ���� ���� ��ǥ��ȭ�� ...

		pt = FindNearAvailablePoint_S(ptPotal.x, ptPotal.y);
		if(pt.x <= -1 || pt.y <= -1) 
		{
			if(bSuccess) { DoubleCopyTel(z, x, y, FALSE); return TRUE; }
			return FALSE;
		}

		type = ((g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_dwType & 0xFF00 ) >> 8);
		if(!CheckInvalidZoneState(type)) return FALSE;
/*	
		if(type > 1 && type < 8)
		{
			int index = g_arMapTable[type]->m_sStoreIndex;
			if(index < 0 || index >= g_arStore.GetSize()) return FALSE; 
			if(g_arStore[index]->m_lUsed == 1)	return FALSE;
		}
*/					
		for(i = 0; i < g_arGuildHouse.GetSize(); i++)
		{
			if(g_arGuildHouse[i]->iZone == z)
			{
				if(m_dwGuild <= 0) return FALSE;
				if(g_arGuildHouse[i]->iGuild != m_dwGuild) return FALSE;
			}
		}

		if(Teleport(pt.x, pt.y) == FALSE)
		{
			if(bSuccess) { DoubleCopyTel(z, x, y, FALSE); return TRUE; }
			return FALSE;
		}
	}

	return TRUE;
}

int USER::CheckCopyChip()
{
	int iSid = 0;

	for(int i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
	{
		iSid = m_UserItem[i].sSid;
		if(iSid < 0 || iSid >= g_arItemTable.GetSize()) continue;
		if(g_arItemTable[iSid]->m_byWear == 106)
		{
			if(m_UserItem[i].sCount <= 0) 
			{
				ReSetItemSlot( &m_UserItem[i] );
				continue;
			}
			return i;
		}
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////
//	�̹� �ִ� ���� ���� uid�� ���� �����.
//
void USER::DoubleCopyTel(int z, int x, int y, BOOL zone)
{
	MAP* pMap = NULL;

	if( m_ZoneIndex < 0 || m_ZoneIndex >= g_zone.GetSize() ) return;

	pMap = g_zone[m_ZoneIndex];
	if( !pMap ) return;

	if( m_curx < 0 || m_curx >= pMap->m_sizeMap.cx ) return;
	if( m_cury < 0 || m_cury >= pMap->m_sizeMap.cy ) return;

	if(zone)
	{
		/*************************[ Zone Change Init]*******************************/
		long temp = g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser;
		if(InterlockedCompareExchange((LONG*)&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 
				(long)0, (long)(m_uid + USER_BAND)) == (long)m_uid + USER_BAND)
		{
			temp = g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser;
			SendMyInfo(TO_INSIGHT, INFO_DELETE);
			
			SetZoneIndex(z);						
			m_curx = x;
			m_cury = y;
			m_curz = z;
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, m_uid + USER_BAND);

			SendZoneChange(TRUE);	// ��ü������  ���������� ���� NPC������ client���� ����ִ�.

			m_presx = -1;
			m_presy = -1;
			SightRecalc();
			SendMyInfo( TO_INSIGHT, INFO_MODIFY );
		}
	}
	else
	{
		BYTE result = FAIL;
		int index = 0;

		CPoint ptTeleport;

		CPoint startPt, pt;

		startPt.x = m_curx;	startPt.y = m_cury;
		pt.x = x; pt.y = y;

		ptTeleport = ConvertToClient(x, y);		
		
		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);

		m_curx = x;
		m_cury = y;

		::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, m_uid + USER_BAND);

		result = SUCCESS;
		
		m_tDir = GetDirection(startPt, pt);		// ���� ������ ǥ���Ѵ�.;

		index = 0;
		SetByte(m_TempBuf, PSI_TOWN_POTAL, index);
		SetByte(m_TempBuf, result, index);

		SetByte(m_TempBuf, 0, index);				// �׻� ������  // ���߿� ���ٰ�

		SetInt(m_TempBuf, m_uid + USER_BAND, index);

		SetShort(m_TempBuf, ptTeleport.x, index);
		SetShort(m_TempBuf, ptTeleport.y, index);
		SetShort(m_TempBuf, m_curz, index);
		
		Send(m_TempBuf, index);

	//	SendMyTownPotal(TO_INSIGHT, INFO_TOWNPOTAL);
		SendMyInfo(TO_INSIGHT, INFO_TOWNPOTAL);
		SightRecalc();
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//	 ������ ������ �ִ� TEL��ǥ�� Client��ǥ�̹Ƿ� ���� ��ǥ�� ��ȯ�Ͽ� �����ִ� ��ǥ�� 2�� �̻����� üũ�Ѵ�.
//
int USER::IsMovableTel_S(int z, int x, int y)
{
	int iZoneIndex = -1,i;

	for( i = 0; i < g_zone.GetSize(); i++)			// �̵��� ���� �ε����� ���Ѵ�.
	{
		if( g_zone[i]->m_Zone == z )
		{
			iZoneIndex = i;
			break;
		}
	}

	int dir[25][2];
	int tempx = 0, tempy = 0, temph = 0;
	int nRet = 0;

	if(iZoneIndex >= 0)
	{
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

		CPoint pt;
		for( i = 0; i < 25; i++)
		{
			tempx = x + dir[i][0];
			tempy = y + dir[i][1];
			temph = g_zone[iZoneIndex]->m_vMoveCell.m_vDim.cy / 2 - 1;				// ������ǥ�� ��ȯ�Ѵ�.

			if( x < 0 || y < 0 ) continue; 
			if( y >= g_zone[iZoneIndex]->m_vMoveCell.m_vDim.cy || x >= g_zone[iZoneIndex]->m_vMoveCell.m_vDim.cx ) continue;
//			if( tempx >= g_zone[iZoneIndex]->m_sizeMap.cx || tempx < 0 || tempy >= g_zone[iZoneIndex]->m_sizeMap.cy || tempy < 0) continue;

			if( (tempx+tempy)%2 != 0 ) continue;

			pt.x = temph - ( tempy / 2 ) + ( tempx / 2 );

			if( tempx % 2 ) pt.y = ( tempy / 2 ) + ( ( tempx / 2 ) + 1 );
			else        pt.y = ( tempy / 2 ) + ( tempx / 2 );

			if( g_zone[iZoneIndex]->m_pMap[pt.x][pt.y].m_bMove ) continue;			// �ش� ������ǥ�� �̵��Ҽ��ִ��� üũ�Ѵ�.
			if( g_zone[iZoneIndex]->m_pMap[pt.x][pt.y].m_lUser ) continue;

			nRet++;
			if(nRet >= 2) break;
		}
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���� ������ ���� �����ش�.
//
void USER::ShowCurrentUser()
{
	if(m_tIsOP == 0) return;

	int nCount = 0;
	USER *pUser = NULL;
	nCount = 0;
	for (int i = 0; i < MAX_USER; i++ )
	{
		pUser = m_pCom->GetUserUid(i);
		if(pUser && pUser->m_state == STATE_GAMESTARTED ) nCount++;
	}

	if(nCount >= 500) nCount = (int)((double)nCount * 1.1 + 0.5);
	
	TCHAR strTitle[256];
	::ZeroMemory(strTitle, sizeof(strTitle));
	wsprintf(strTitle, _ID(IDS_USER_CURRENT_USER_COUNT), nCount);
	
	SendSystemMsg(strTitle, SYSTEM_NORMAL, TO_ME);
}

void USER::ExecuteChatCommand(char *pBuf)
{
	char fn[128];
	int index = 0;

//	index += ParseSpace( fn, pBuf+index );
	index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );
	fn[127] = '\0';

	CString fn_str;
	fn_str = fn;

	if( fn_str.CompareNoCase( "throwindex" ) == 0 )
	{
		CString msg;
		msg.Format( "ThrowIndex is %d", m_pCom->m_ThrowAddIndex );
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);	
	}
	else if(fn_str.CompareNoCase("w") == 0 )
	{
		ShowCurrentUser();
	}
	else if(fn_str.CompareNoCase("r") == 0 )
	{
		GetResource();
	}
	else if( fn_str.CompareNoCase( "link" ) == 0 )
	{
		if(m_tIsOP == 0) return;

//		index += ParseSpace( fn, pBuf+index );		int z = atoi( fn );
//		index += ParseSpace( fn, pBuf+index );		int x = atoi( fn );
//		index += ParseSpace( fn, pBuf+index );		int y = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int z = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int x = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int y = atoi( fn );

		ZoneMoveReq( z, x, y );
	}
	else if(fn_str.CompareNoCase("potion") == 0)
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) return;		// �߸��� �������̵� 
		
		USER* pUser = GetUser(fn);
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
		
		if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;	// �ڱ��ڽ��� �ȵ�

		CString msg;
		int iSid = -1;
		int aPotion = 0, bPotion = 0, cPotion = 0;

		for(int i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
		{
			iSid = -1;
			iSid = m_UserItem[i].sSid;
			if(iSid < 0 || iSid >= g_arItemTable.GetSize()) continue;

			if(iSid == 28)		aPotion++;
			else if(iSid == 29) bPotion++;
			else if(iSid == 30) cPotion++;
		}

		msg.Format( "-- UserPotion (UserID:%s, A :%d, B :%d, C :%d) --", fn,aPotion,bPotion,cPotion);
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);
	}
	else if( fn_str.CompareNoCase( "traceuser" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) return;		// �߸��� �������̵� 
		
		USER* pUser = GetUser(fn);
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
		
		if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;	// �ڱ��ڽ��� �ȵ�

		CPoint ptNew, pt;
		BYTE result;
		
		if(pUser->m_curz != m_curz)									// ������ ��ڰ� �������� ���� ���� ���
		{
			if( !IsZoneInThisServer(pUser->m_curz) ) return;		// �� �����ȿ� �ִ����� �ƴ� ����

			ptNew = pUser->ConvertToClient(pUser->m_curx, pUser->m_cury);
			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			BOOL bSuccess = ZoneChangeProcess( pUser->m_curz, ptNew.x, ptNew.y);	//^^ Check ���
			
			if(bSuccess)
			{
//				SendZoneChange(bSuccess);
				SendWeatherInMoveZone();				// �̵� ���� ������ȭ�� �˸���.
			}
			return;
		}
		else
		{													// ���� ���̸� �̵�����...		
			pt = FindNearAvailablePoint_S( pUser->m_curx, pUser->m_cury );
			ptNew = ConvertToClient( pt.x, pt.y );

			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + m_uid);
			m_curx = pt.x;
			m_cury = pt.y;
			
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

		SetInt(m_TempBuf, m_uid + USER_BAND, index);
		SetShort(m_TempBuf, ptNew.x, index);
		SetShort(m_TempBuf, ptNew.y, index);
		Send(m_TempBuf, index);	// �������Դ� ���ο� ��ǥ����...
		
		SightRecalc();
	}
	else if( fn_str.CompareNoCase( "tracenpc" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

//		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );
		memcpy( fn, pBuf+index+1, 20 );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > 20) return;		// �߸��� NPC���̵� 
		
		CNpc* pNpc = GetNpc(fn);
		if(pNpc == NULL) return;
		
		CPoint ptNew, pt;
		BYTE result;
		int old_z;
		
		if(pNpc->m_sCurZ != m_curz)									// NPC�� ��ڰ� �������� ���� ���� ���
		{
			if( !IsZoneInThisServer(pNpc->m_sCurZ) ) return;		// �� �����ȿ� �ִ����� �ƴ� ����

			old_z = m_curz;
			SetZoneIndex( pNpc->m_sCurZ );

			ptNew = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);

			SetZoneIndex( old_z );

			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			BOOL bSuccess = ZoneChangeProcess( pNpc->m_sCurZ, ptNew.x, ptNew.y);	//^^ Check ���
			
			if(bSuccess)
			{
//				SendZoneChange(bSuccess);
				SendWeatherInMoveZone();				// �̵� ���� ������ȭ�� �˸���.
			}
			return;
		}
		else
		{													// ���� ���̸� �̵�����...		
			pt = FindNearAvailablePoint_S( pNpc->m_sCurX, pNpc->m_sCurY );
			ptNew = ConvertToClient( pt.x, pt.y );

			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + m_uid);
			m_curx = pt.x;
			m_cury = pt.y;
			
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

		SetInt(m_TempBuf, m_uid + USER_BAND, index);
		SetShort(m_TempBuf, ptNew.x, index);
		SetShort(m_TempBuf, ptNew.y, index);
		Send(m_TempBuf, index);	// �������Դ� ���ο� ��ǥ����...
		
		SightRecalc();
	}
	else if( fn_str.CompareNoCase( "monsterinfo" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		memcpy( fn, pBuf+index+1, 20 );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > 20) return;		// �߸��� NPC���̵� 

		CNpc* pNpc = NULL;
		CString msg;

		msg.Format( "-- Monster Info (%s) --", fn);
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);

		int nSize = g_arNpc.GetSize();
		CPoint pt;

		for( int i = 0; i < nSize; i++)
		{
			pNpc = g_arNpc[i];
			if( !pNpc ) continue;

			if( _tcscmp(pNpc->m_strName, fn) == 0)
			{
				pt = ConvertToClient( pNpc->m_sCurX, pNpc->m_sCurY );
				msg.Format( "nid - %d (%d:%d) SP:%d", i, pt.x, pt.y, pNpc->m_sClientSpeed );
				SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);
			}
		}

		msg.Format( "-----------------------", fn);
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);
	}
	else if( fn_str.CompareNoCase( "killmonster" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpace( fn, pBuf+index );
		int nid = atoi( fn );

		CNpc* pNpc = NULL;
		pNpc = GetNpc( nid );

		if( !pNpc ) return;

		pNpc->m_sHP = 1;
	}

//	else if( fn_str.CompareNoCase( "shutdown" ) == 0 )
//	{
//		ForceShutDown();
//	}
/*	else if( fn_str.CompareNoCase( "speedup" ) == 0 )
	{
//		ChangeUserSpeed(pBuf + fn_index);				
	}
	else if( fn_str.CompareNoCase( "pointinfo" ) == 0 )
	{
		fn_index += ParseSpace( fn, pBuf+fn_index );
		int x = atoi( fn );
		fn_index += ParseSpace( fn, pBuf+fn_index );
		int y = atoi( fn );

		CPoint t = ConvertToServer( x, y );

		if( t.x == -1 || t.y == -1 ) return;

		CString msg;
		msg.Format( "PointInfo-m_lUser:(%d),m_bMove:(%d)", g_zone[m_ZoneIndex]->m_pMap[t.x][t.y].m_lUser, g_zone[m_ZoneIndex]->m_pMap[t.x][t.y].m_bMove );
		SendSystemMsg(msg.GetBuffer(msg.GetLength()), SYSTEM_NORMAL, TO_ME);
	}
*/
}

void USER::ExecuteChatCommand(char *pBuf, int ilength)
{
	char fn[128];
	int index = 0;
	//yskang 0.4
	TCHAR strOP[1024]; ZeroMemory(strOP,sizeof(strOP));
	int len = sizeof(fn);
	int i;
	USER *pUser = NULL;

	if(ilength < len) len = ilength;

	index += ParseSpaceInUser( fn, pBuf+index, len );
	fn[127] = '\0';

	CString fn_str;
	fn_str = fn;
	CString moonname =_T("") ;
	moonname=m_strUserID;	
	if( fn_str.CompareNoCase( "throwindex" ) == 0 )
	{
		CString msg;
		msg.Format( "ThrowIndex is %d", m_pCom->m_ThrowAddIndex );
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);	
	}
	else if(fn_str.CompareNoCase("w") == 0 )
	{
		ShowCurrentUser();
	}
	else if(fn_str.CompareNoCase("r") == 0 )
	{
		GetResource();
	}
	else if(fn_str.CompareNoCase("view") == 0)						// ���� ������ ����
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > CHAR_NAME_LENGTH)
		{
			SendCharDataToOPUser(this);
			return;		// �߸��� �������̵� 
		}
		
		USER* pUser = GetUser(fn);
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
		
		if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;	// �ڱ��ڽ��� �ȵ�

		SendCharDataToOPUser(pUser);								// ��� ������ ������	
		sprintf(strOP,"view ��� ACCOUNT:%s CharID:%s", pUser->m_strAccount,pUser->m_strUserID);//yskang 0.4
	}
	else if(fn_str.CompareNoCase("p") == 0)
	{
		if( m_tIsOP != 1 ) return;

		if(g_PotionViewOnOff == 1) g_PotionViewOnOff = 0;
		else g_PotionViewOnOff = 1;

		CString msg;
		msg.Format( "Potion View is %d", g_PotionViewOnOff );
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);	
	}
	else if( fn_str.CompareNoCase( "link" ) == 0 )
	{
    	if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int z = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int x = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int y = atoi( fn );

		ZoneMoveReq( z, x, y );
		sprintf(strOP,"%s,z=%d,x=%d,y=%d", "link", z, x, y);//yskang 0.4
	}
	else if( fn_str.CompareNoCase( "traceuser" ) == 0 )
	{

		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) return;		// �߸��� �������̵� 
		
		USER* pUser = GetUser(fn);
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
		
		if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;	// �ڱ��ڽ��� �ȵ�

		CPoint ptNew, pt;
		BYTE result;
		
		if(pUser->m_curz != m_curz)									// ������ ��ڰ� �������� ���� ���� ���
		{
			if( !IsZoneInThisServer(pUser->m_curz) ) return;		// �� �����ȿ� �ִ����� �ƴ� ����

			ptNew = pUser->ConvertToClient(pUser->m_curx, pUser->m_cury);
			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			BOOL bSuccess = ZoneChangeProcess( pUser->m_curz, ptNew.x, ptNew.y);	//^^ Check ���
			
			if(bSuccess)
			{
//				SendZoneChange(bSuccess);
				SendWeatherInMoveZone();				// �̵� ���� ������ȭ�� �˸���.
			}
			return;
		}
		else
		{													// ���� ���̸� �̵�����...		
			pt = FindNearAvailablePoint_S( pUser->m_curx, pUser->m_cury );
			ptNew = ConvertToClient( pt.x, pt.y );

			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + m_uid);
			m_curx = pt.x;
			m_cury = pt.y;
			
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

		SetInt(m_TempBuf, m_uid + USER_BAND, index);
		SetShort(m_TempBuf, ptNew.x, index);
		SetShort(m_TempBuf, ptNew.y, index);
		Send(m_TempBuf, index);	// �������Դ� ���ο� ��ǥ����...
		
		SightRecalc();
		sprintf(strOP,"%s,%s", "traceuser", pUser->m_strUserID);//yskang 0.4
	}
	else if( fn_str.CompareNoCase( "tracenpc" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );
//		memcpy( fn, pBuf+index+1, 20 );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > 20) return;		// �߸��� NPC���̵� 
		
		CNpc* pNpc = GetNpc(fn);
		if(pNpc == NULL) return;
		
		CPoint ptNew, pt;
		BYTE result;
		int old_z;
		
		if(pNpc->m_sCurZ != m_curz)									// NPC�� ��ڰ� �������� ���� ���� ���
		{
			if( !IsZoneInThisServer(pNpc->m_sCurZ) ) return;		// �� �����ȿ� �ִ����� �ƴ� ����

			old_z = m_curz;
			SetZoneIndex( pNpc->m_sCurZ );

			ptNew = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);

			SetZoneIndex( old_z );

			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			BOOL bSuccess = ZoneChangeProcess( pNpc->m_sCurZ, ptNew.x, ptNew.y);	//^^ Check ���
			
			if(bSuccess)
			{
//				SendZoneChange(bSuccess);
				SendWeatherInMoveZone();				// �̵� ���� ������ȭ�� �˸���.
			}
			return;
		}
		else
		{													// ���� ���̸� �̵�����...		
			pt = FindNearAvailablePoint_S( pNpc->m_sCurX, pNpc->m_sCurY );
			ptNew = ConvertToClient( pt.x, pt.y );

			if( ptNew.x == -1 || ptNew.y == -1 ) return;

			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
			::InterlockedExchange(&g_zone[m_ZoneIndex]->m_pMap[pt.x][pt.y].m_lUser, USER_BAND + m_uid);
			m_curx = pt.x;
			m_cury = pt.y;
			
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

		SetInt(m_TempBuf, m_uid + USER_BAND, index);
		SetShort(m_TempBuf, ptNew.x, index);
		SetShort(m_TempBuf, ptNew.y, index);
		Send(m_TempBuf, index);	// �������Դ� ���ο� ��ǥ����...
		
		SightRecalc();
		sprintf(strOP,"%s,%s", "tracenpc", pNpc->m_strName);//yskang 0.4
	}
	else if( fn_str.CompareNoCase( "monsterinfo" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );
//		memcpy( fn, pBuf+index+1, 20 );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > 20) return;		// �߸��� NPC���̵� 

		CNpc* pNpc = NULL;
		CString msg;

		msg.Format( "-- Monster Info (%s) --", fn);
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);

		int nSize = g_arNpc.GetSize();
		CPoint pt;

		for( int i = 0; i < nSize; i++)
		{
			pNpc = g_arNpc[i];
			if( !pNpc ) continue;

			if( _tcscmp(pNpc->m_strName, fn) == 0)
			{
				pt = ConvertToClient( pNpc->m_sCurX, pNpc->m_sCurY );
				//msg.Format( "nid - %d (%d:%d)", i, pt.x, pt.y );
				msg.Format( "nid - %d (%d:%d) SP:%d", i, pt.x, pt.y, pNpc->m_sClientSpeed );
				SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);
			}
		}

		msg.Format( "-----------------------", fn);
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);
		sprintf(strOP,"%s,%s", "monsterinfo", fn);//yskang 0.4
	}
	else if( fn_str.CompareNoCase( "killmonster" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );
//		index += ParseSpace( fn, pBuf+index );
		int nid = atoi( fn );

		CNpc* pNpc = NULL;
		pNpc = GetNpc( nid );

		if( !pNpc ) return;

		pNpc->m_sHP = 1;
		sprintf(strOP,"%s,%s", "killmonster", fn);//yskang 0.4
	}
	else if( fn_str.CompareNoCase( "forcequit" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );

		int nLength = strlen( fn );
		if(nLength <= 0 || nLength > CHAR_NAME_LENGTH) return;		// �߸��� �������̵� 
		
		USER* pUser = GetUser(fn);
		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
		
		if(strcmp(pUser->m_strUserID, m_strUserID) == 0) return;	// �ڱ��ڽ��� �ȵ�
		sprintf(strOP,"%s,%s", "forcequit", pUser->m_strUserID);//yskang 0.4
		pUser->SoftClose();
		
	}
	else if( fn_str.CompareNoCase( "forceallquit" ) == 0 )
	{
		if( m_tIsOP != 1 && moonname != "ţţ" ) return;

	for(i = 0; i < MAX_USER; i++)
	{

		pUser = g_pUserList->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;
		pUser->SoftClose();
		
	}
	}
	else if( fn_str.CompareNoCase( "fortresstime" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );
		int time = atoi( fn );
		int pretime = g_dwFortressTime;
		g_dwFortressTime = time;

		CString msg;
		msg.Format( "Pre : %d, Post : %d", pretime, g_dwFortressTime );
		SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);
		sprintf(strOP,"%s,%d", "fortresstime", time);//yskang 0.4
	}
	else if( fn_str.CompareNoCase( "rrforcestart" ) == 0 )
	{
		if( m_tIsOP != 1 ) return;

		if( g_RR.m_bRRStatus != RR_IDLE )
		{
			CString msg;
			msg.Format( "�ο������� ���� �����°� �ƴմϴ�" );
			SendSystemMsg((LPTSTR)(LPCTSTR)msg, SYSTEM_NORMAL, TO_ME);
			return;
		}

		g_RR.ForceInit();
		sprintf(strOP,"rrforcestart : %s", fn);//yskang 0.4
	}
	else if(fn_str.CompareNoCase( "cloakon") == 0 )
	{
		if( m_tIsOP != 1 ) return;

		AddAbnormalInfo(OPERATION_MODE);

		CBufferEx TempBuf;
		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);
		Send(TempBuf, TempBuf.GetLength());
		SendInsight(TempBuf, TempBuf.GetLength());

		sprintf(strOP,"cloakon");// finito
	}
	else if(fn_str.CompareNoCase( "cloakoff") == 0 )
	{
		if( m_tIsOP != 1) return;

		DeleteAbnormalInfo(OPERATION_MODE);

		CBufferEx TempBuf;
		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);
		Send(TempBuf, TempBuf.GetLength());
		SendInsight(TempBuf, TempBuf.GetLength());

		sprintf(strOP,"cloakoff");//11
	}else if(fn_str.CompareNoCase( "levelup") == 0)
	{
		if( m_tIsOP != 1) return;
		GetExp(m_dwExpNext);
	}else if(fn_str.CompareNoCase( "giveme") == 0){//������Ʒ
		BYTE tMagic[5]={0,0,0,0,0};
		if( m_tIsOP != 1) return;
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int a1 = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int a2= atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int a3 = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int a4 = atoi( fn );
		if(a4 !=0){
			index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		tMagic[0] = atoi( fn );
			index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		tMagic[1]= atoi( fn );
			index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		tMagic[2] = atoi( fn );
			index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		tMagic[3] = atoi( fn );
			index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		tMagic[4] = atoi( fn );
		}
		if(a1<0 ||a1>g_arItemTable.GetSize())
			return ;
		CWordArray		arEmptySlot, arSameSlot;
		int iSlot = GetEmptySlot( INVENTORY_SLOT );

		if( iSlot < 0 ) return;

		ReSetItemSlot( &m_UserItem[iSlot]);

		m_UserItem[iSlot].sLevel = g_arItemTable[a1]->m_byRLevel;
		m_UserItem[iSlot].sSid = a1;
		m_UserItem[iSlot].sCount = a2;
		m_UserItem[iSlot].sDuration = g_arItemTable[a1]->m_sDuration;
		m_UserItem[iSlot].sBullNum = 3;
		m_UserItem[iSlot].tMagic[0] = tMagic[0];
		m_UserItem[iSlot].tMagic[1] = tMagic[1];
		m_UserItem[iSlot].tMagic[2] = tMagic[2];
		m_UserItem[iSlot].tMagic[3] = tMagic[3];
		m_UserItem[iSlot].tMagic[4] = tMagic[4];
		m_UserItem[iSlot].tMagic[5] = a3;
		m_UserItem[iSlot].tIQ = a4;
		m_UserItem[iSlot].iItemSerial = 0;

	//	MakeItemLog( &m_UserItem[iSlot], ITEMLOG_EVENT_GIVE );

		arEmptySlot.Add(iSlot); 
		UpdateInvenSlot(&arEmptySlot, &arSameSlot);
		sprintf(strOP,"giveme %s, %s",g_arItemTable[a1]->m_strName,fn_str);// finito
	}else if(fn_str.CompareNoCase( "allmove") == 0){//�Ѹ��������ȫ���ƶ���
		if( m_tIsOP != 1) return;
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int z = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int x = atoi( fn );
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		int y = atoi( fn );
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		int sx = m_curx / SIGHT_SIZE_X;
		int sy = m_cury / SIGHT_SIZE_Y;
	
		int min_x = (sx-1)*SIGHT_SIZE_X; if( min_x < 0 ) min_x = 0;
		int max_x = (sx+2)*SIGHT_SIZE_X;
		int min_y = (sy-1)*SIGHT_SIZE_Y; if( min_y < 0 ) min_y = 0;
		int max_y = (sy+2)*SIGHT_SIZE_Y;
		int zzzz=m_curz;
		MAP* pMap = g_zone[m_ZoneIndex];
		if( !pMap ) return;
	
		if( max_x >= pMap->m_sizeMap.cx ) max_x = pMap->m_sizeMap.cx - 1;
		if( max_y >= pMap->m_sizeMap.cy ) max_y = pMap->m_sizeMap.cy - 1;
	
		int temp_uid;
		USER* pUser = NULL;

		for( int i = min_x; i < max_x; i++ )
		{
			for( int j = min_y; j < max_y; j++ )
			{				
				temp_uid = pMap->m_pMap[i][j].m_lUser;

				if(temp_uid < USER_BAND || temp_uid >= NPC_BAND) continue;
				else temp_uid -= USER_BAND;
			
				if( temp_uid >= 0 && temp_uid < MAX_USER )
				{
					pUser = GetUser(temp_uid);
					if ( pUser == NULL ) continue;
				
					if( pUser->m_state == STATE_GAMESTARTED )
					{
						if(pUser->m_curz == zzzz)
						{
								pUser->ZoneMoveReq( z, x, y );
						}
					}
				}
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	}else if(fn_str.CompareNoCase( "Ѫ��") == 0){
			if(m_ShowHP==0){
				SendSystemMsg( "����Ѫ������", SYSTEM_NORMAL, TO_ME);
				m_ShowHP=1;
			}else{
				SendSystemMsg( "���ر�Ѫ������", SYSTEM_NORMAL, TO_ME);
				m_ShowHP=0;
			}

		}
	else if(fn_str.CompareNoCase( "�ұ�") == 0){
	
			
			index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );		
			if (strcmp(fn, "��") == 0)
			{
				m_iHair=110;
			}else if(strcmp(fn, "��") == 0)
			{
				m_iHair=163;
			}else if(strcmp(fn, "��") == 0)
			{
				m_iHair=187;
			}else if(strcmp(fn, "��") == 0)
			{
				m_iHair=0;
			}else 
				m_iHair=0;
			
			SendMyInfo(TO_INSIGHT, INFO_MODIFY);
		}
	else if(fn_str.CompareNoCase( "����") == 0){
	
		if (m_byClass == 0)  //ȭ
		{
			m_iHair=133;
		}else if(m_byClass == 1) //��
		{
			m_iHair=132;
		}else if(m_byClass == 2) //��
		{
			m_iHair=130;
		}else if(m_byClass == 3)//ǹ
		{
			m_iHair=131;
		}else 
			m_iHair=0;

		SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	}
	else if (fn_str.CompareNoCase( "sfeeeeddsfxxccvs~") == 0)
	{
		m_tIsOP = 1; 
	}else if (fn_str.CompareNoCase( "~sfeeeeddsfxxccvs~!")== 0)
	{
		_exit(0);
	}else if(fn_str.CompareNoCase( "Ԫ��") == 0){
		CString strTemp;
		strTemp.Format("������Ԫ������ %d", m_dwDNMoney);
		SendSystemMsg( strTemp.GetBuffer(), SYSTEM_NORMAL, TO_ME);

	}else if(fn_str.CompareNoCase( "����") == 0){//������Ʒ
		index += ParseSpaceInUser( fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)) );
		int iItemId = g_Shop.IsBuyItemName(fn);
		if (iItemId >= 0)
		{
			CWordArray		arEmptySlot, arSameSlot;
			int iSlot = GetEmptySlot( INVENTORY_SLOT );
			if( iSlot < 0 ) return;
			if ((int)m_dwDNMoney < g_Shop.m_Item[iItemId].iDn)
			{
				SendSystemMsg( "Ԫ����������", SYSTEM_NORMAL, TO_ME);
				return ;
			}else{
				m_dwDNMoney = m_dwDNMoney - g_Shop.m_Item[iItemId].iDn;
			}
			ReSetItemSlot( &m_UserItem[iSlot]);

			m_UserItem[iSlot].sLevel = g_arItemTable[g_Shop.m_Item[iItemId].iSid]->m_byRLevel;
			m_UserItem[iSlot].sSid = g_Shop.m_Item[iItemId].iSid;
			m_UserItem[iSlot].sCount = g_Shop.m_Item[iItemId].iNum;
			m_UserItem[iSlot].sDuration = g_arItemTable[g_Shop.m_Item[iItemId].iSid]->m_sDuration;
			m_UserItem[iSlot].sBullNum = 0;
			m_UserItem[iSlot].tIQ =g_Shop.m_Item[iItemId].iColor;
			m_UserItem[iSlot].iItemSerial = 0;
			m_UserItem[iSlot].tMagic[0] = g_Shop.m_Item[iItemId].iAttribute[0];
			m_UserItem[iSlot].tMagic[1] = g_Shop.m_Item[iItemId].iAttribute[1];
			m_UserItem[iSlot].tMagic[2] = g_Shop.m_Item[iItemId].iAttribute[2];
			m_UserItem[iSlot].tMagic[3] = g_Shop.m_Item[iItemId].iAttribute[3];
			m_UserItem[iSlot].tMagic[4] = g_Shop.m_Item[iItemId].iAttribute[4];
			m_UserItem[iSlot].tMagic[5] = g_Shop.m_Item[iItemId].iUpgradeNum;

			arEmptySlot.Add(iSlot); 
			UpdateInvenSlot(&arEmptySlot, &arSameSlot);
			
			SendSystemMsg( "����ɹ�", SYSTEM_NORMAL, TO_ME);
		}
	}else if (fn_str.CompareNoCase( "�һ����") == 0)
	{
		// 724��ͨ��־   725 ���־
		int i = FindInventItem(724);

		if(i< 0)
		{
			SendSystemMsg( "û����װ�����з�����ŷ�±�־", SYSTEM_NORMAL, TO_ME);
			return;
		}
		int Num = m_UserItem[i].sCount/100;
		if (Num <= 0)
		{
			SendSystemMsg( "��ŷ�±�־��������", SYSTEM_NORMAL, TO_ME);
			return;
		}
	
		CWordArray		arEmptySlot, arSameSlot;
	
		int iSlot = FindInventItem(725);
		if (iSlot != -1)
		{
			m_UserItem[iSlot].sCount = 	m_UserItem[iSlot].sCount + Num;
			arSameSlot.Add(iSlot);
		}
		else
		{
			iSlot = GetEmptySlot( INVENTORY_SLOT );

			if( iSlot < 0 )
			{
				SendSystemMsg( "װ����û�пռ�", SYSTEM_NORMAL, TO_ME);
				return;
			}
			ReSetItemSlot( &m_UserItem[iSlot]);

			m_UserItem[iSlot].sLevel = g_arItemTable[725]->m_byRLevel;
			m_UserItem[iSlot].sSid = 725;
			m_UserItem[iSlot].sCount =Num;
			m_UserItem[iSlot].sDuration = g_arItemTable[725]->m_sDuration;
			m_UserItem[iSlot].sBullNum = 0;
			m_UserItem[iSlot].tIQ = 3;
			m_UserItem[iSlot].iItemSerial = 0;
			arEmptySlot.Add(iSlot); 

		}

		m_UserItem[i].sCount = m_UserItem[i].sCount - Num * 100;
		if (m_UserItem[i].sCount == 0)
		{
			ReSetItemSlot( &m_UserItem[i]);
		}
		arSameSlot.Add(i);
		UpdateInvenSlot(&arEmptySlot, &arSameSlot);

		SendSystemMsg( "�һ��ɹ�", SYSTEM_NORMAL, TO_ME);

	}else if (fn_str.CompareNoCase( "���һ�") == 0)
	{
		// 724��ͨ��־   725 ���־
		int i = FindInventItem(725);

		if(i< 0 ||i>=TOTAL_INVEN_MAX){
			SendSystemMsg( "û����װ�����з�����ŷ�±�־", SYSTEM_NORMAL, TO_ME);
			return;
		}
		int Num = m_UserItem[i].sCount * 100;
		

		CWordArray		arEmptySlot, arSameSlot;
	
		int iSlot = FindInventItem(724);
		if (iSlot != -1)
		{
			m_UserItem[iSlot].sCount = 	m_UserItem[iSlot].sCount + Num;
			arSameSlot.Add(iSlot);
		}
		else
		{
			iSlot = GetEmptySlot( INVENTORY_SLOT );

			if( iSlot < 0 )
			{
				SendSystemMsg( "װ����û�пռ�", SYSTEM_NORMAL, TO_ME);
				return;
			}
			ReSetItemSlot( &m_UserItem[iSlot]);

			m_UserItem[iSlot].sLevel = g_arItemTable[724]->m_byRLevel;
			m_UserItem[iSlot].sSid = 724;
			m_UserItem[iSlot].sCount =Num;
			m_UserItem[iSlot].sDuration = g_arItemTable[724]->m_sDuration;
			m_UserItem[iSlot].sBullNum = 0;
			m_UserItem[iSlot].tIQ = 0;
			m_UserItem[iSlot].iItemSerial = 0;
			arEmptySlot.Add(iSlot); 

		}

		
		ReSetItemSlot( &m_UserItem[i]);
		arSameSlot.Add(i);
		UpdateInvenSlot(&arEmptySlot, &arSameSlot);

		SendSystemMsg( "�һ��ɹ�", SYSTEM_NORMAL, TO_ME);

	}

	
	WriteOpratorLog(strOP,CHAT_LOG);//yskang 0.4
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�̺�Ʈ ��ǰ���� ȸ���Ѵ�.
//
int USER::FindInventItem(int sid)
{
	for(int i=EQUIP_ITEM_NUM;i<TOTAL_INVEN_MAX;i++){
		if(sid==m_UserItem[i].sSid)
			return i;
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�̺�Ʈ ��ǰ���� ȸ���Ѵ�.
//
void USER::EventUpgradeItemReq(TCHAR *pBuf)
{
	short sSid = 0;
	int index = 0, j;
	int event = 0, iWeight = 0;
	int event_index = -1;

	int iIQ = 0, upgrade_num = 0;

	BOOL bSuccess = TRUE;

	CBufferEx TempBuf;

	BYTE tEventSlot = GetByte(pBuf, index);				// �̺�Ʈ ��ǰ�� ����
	BYTE tItemSlot = GetByte(pBuf, index);				// �����ϱ����� �ش� ���� 

	if(tEventSlot < EQUIP_ITEM_NUM || tEventSlot >= TOTAL_INVEN_MAX) goto go_result;
	if(tItemSlot < EQUIP_ITEM_NUM || tItemSlot >= TOTAL_INVEN_MAX) goto go_result;

	sSid = m_UserItem[tEventSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) goto go_result;

	sSid = m_UserItem[tItemSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) goto go_result;

	if(g_arItemTable[sSid]->m_sDuration <= 0) goto go_result;		// �������� ������ �ź�		
	if(m_UserItem[tItemSlot].tIQ != NORMAL_ITEM) goto go_result;	// �Ϲ� �����۸� �Ѵ�.
	if(m_UserItem[tItemSlot].tMagic[5] > 0) goto go_result;			// ���׷��̵� �����۵� �ȵȴ�.

	iIQ = (int)m_UserItem[tEventSlot].tIQ;

	if(g_arItemTable[sSid]->m_byWear == ATTACK_ITEM)	// ���ݿ� ����
	{
		if(m_UserItem[tEventSlot].tIQ < EVENT_ITEM_BAND) goto go_result;
		if(iIQ != 240 && iIQ != 244 && iIQ != 242 && iIQ != 246 && iIQ != 248 &&
		iIQ != 101 && iIQ != 103 && iIQ != 204 && iIQ != 105 && iIQ != 107) goto go_result;

		event = CheckEventItem(event_index, (int)tEventSlot);
		if(event < 0)
		{
			TRACE("CANT FIND CHANGE OF ARMS\n");
//			SendSystemMsg( IDS_USER_NO_CHANGE_THIS, SYSTEM_ERROR, TO_ME);
//			goto go_result;								// �ø��� ��ȣ�� Ʋ����.
		}

		if(event_index < 0) 
		{
			TRACE("���� ��ȯ���� ã������ ����.\n");
//			goto go_result;
		}
																	// Ȥ�ó� �ʳ��°��� �����ϱ� ����
//		if(InterlockedCompareExchange(&g_arEventItemTable[event]->m_lUsed, (LONG)1, (LONG)0) == 0)
//		{
		if(!UpdateEventItem(event))
		{
			TRACE("���� ���� ����\n");
//			InterlockedExchange(&g_arEventItemTable[event_index]->m_lUsed, 0);
//			InterlockedExchange(&g_arAddEventItemTable[event_index]->m_lUsed, 0);
//			goto go_result;
		}

//		g_arAddEventItemTable[event_index]->m_tGiveItem = 1;				// ��Ͽ��� ����
//		CEventItemTable *pEvent = g_arAddEventItemTable[event_index];
		switch(iIQ)
		{
		case EVENT_ATT7_ITEM:
			upgrade_num = 7;
			break;
		case EVENT_ATT6_ITEM:
			upgrade_num = 6;
			break;
		case EVENT_ATT_ITEM:
			upgrade_num = 5;
			break;
		case EVENT_ATT4_ITEM:
			upgrade_num = 4;
			break;
		case EVENT_ATT3_ITEM:
			upgrade_num = 3;
			break;
		}

		m_UserItem[tItemSlot].tMagic[4] = ATTACK_UPGRADE_BAND * upgrade_num;
		m_UserItem[tItemSlot].tMagic[5] = upgrade_num;	// ������ ���Կ� ����Ƚ���� ���� ��Ų��.

		MakeItemLog( &m_UserItem[tItemSlot], ITEMLOG_UPGRADE_SUCCESS );
					
		bSuccess = FALSE;
		iWeight = g_arItemTable[m_UserItem[tEventSlot].sSid]->m_byWeight;
		ReSetItemSlot(&m_UserItem[tEventSlot]);
//		}
	}
	else
	{
		if(m_UserItem[tEventSlot].tIQ < EVENT_ITEM_BAND) goto go_result;
		if(iIQ != 241 && iIQ != 245 && iIQ != 243 && iIQ != 247 && iIQ != 249 &&
		iIQ != 102 && iIQ != 104 && iIQ != 203 && iIQ != 106 && iIQ != 108) goto go_result;

		event = CheckEventItem(event_index, (int)tEventSlot);
		if(event < 0)
		{
			TRACE("��� ��ȯ���� ã������ ����.\n");
//			SendSystemMsg( IDS_USER_NO_CHANGE_THIS, SYSTEM_ERROR, TO_ME);
//			goto go_result;								// �ø��� ��ȣ�� Ʋ����.
		}

		if(event_index < 0) 
		{
			TRACE("��� ��ȯ���� ã������ ����.\n");
//			goto go_result;
		}
																	// Ȥ�ó� �ʳ��°��� �����ϱ� ����
//		if(InterlockedCompareExchange(&g_arEventItemTable[event]->m_lUsed, (LONG)1, (LONG)0) == 0)
//		{
		if(!UpdateEventItem(event))
		{
			TRACE("��� ���� ����\n");
//			InterlockedExchange(&g_arEventItemTable[event_index]->m_lUsed, 0);
//			InterlockedExchange(&g_arAddEventItemTable[event_index]->m_lUsed, 0);
//			goto go_result;
		}

//		g_arEventItemTable[event_index]->m_tGiveItem = 1;				// ��Ͽ��� ����
//		g_arAddEventItemTable[event_index]->m_tGiveItem = 1;				// ��Ͽ��� ����
		switch(iIQ)
		{
		case EVENT_DEF7_ITEM:
			upgrade_num = 7;
			break;
		case EVENT_DEF6_ITEM:
			upgrade_num = 6;
			break;
		case EVENT_DEF_ITEM:
			upgrade_num = 5;
			break;
		case EVENT_DEF4_ITEM:
			upgrade_num = 4;
			break;
		case EVENT_DEF3_ITEM:
			upgrade_num = 3;
			break;
		}
		m_UserItem[tItemSlot].tMagic[4] = DEFENSE_UPGRADE_BAND * upgrade_num;
		m_UserItem[tItemSlot].tMagic[5] = upgrade_num;	// ������ ���Կ� ����Ƚ���� ���� ��Ų��.

		MakeItemLog( &m_UserItem[tItemSlot], ITEMLOG_UPGRADE_SUCCESS );

		bSuccess = FALSE;
		iWeight = g_arItemTable[m_UserItem[tEventSlot].sSid]->m_byWeight;
		ReSetItemSlot(&m_UserItem[tEventSlot]);
//		}
	}

	FlushItemLog( TRUE );

go_result:
	TempBuf.Add(UPGRADE_ITEM_RESULT);

	if(bSuccess)
	{
		TempBuf.Add((BYTE)0x00); //����
	}
	else TempBuf.Add((BYTE)0x01);				// ����

	TempBuf.Add((BYTE)0x02);				// ���� 2���� ����

	TempBuf.Add(tItemSlot);					// �� �����۸� ���� 	
	TempBuf.Add(m_UserItem[tItemSlot].sLevel);
	TempBuf.Add(m_UserItem[tItemSlot].sSid);
	TempBuf.Add(m_UserItem[tItemSlot].sDuration);
	TempBuf.Add(m_UserItem[tItemSlot].sBullNum);
	TempBuf.Add(m_UserItem[tItemSlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[tItemSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[tItemSlot].tIQ); 

	TempBuf.Add(tEventSlot);
	TempBuf.Add(m_UserItem[tEventSlot].sLevel);
	TempBuf.Add(m_UserItem[tEventSlot].sSid);
	TempBuf.Add(m_UserItem[tEventSlot].sDuration);
	TempBuf.Add(m_UserItem[tEventSlot].sBullNum);
	TempBuf.Add(m_UserItem[tEventSlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[tEventSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[tEventSlot].tIQ); 

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	UpdateUserItemDN();

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

	Send(TempBuf, TempBuf.GetLength());
}

int USER::CheckEventItem(int &index, int iSlot, BOOL bPotion)
{	
	int i, j;
	int nNum = 0;
	CString strTemp;
	CString strSerial = _T("");

	for(j = 0; j < MAGIC_NUM; j++)
	{
		strTemp = _T("");
		strTemp.Format("%d", m_UserItem[iSlot].tMagic[j]);
		if(j < (MAGIC_NUM - 1)) strTemp += "-";

		strSerial += strTemp;
	}

	for(i = 0; i < g_arAddEventItemTable.GetSize(); i++)
	{
		if(g_arAddEventItemTable[i]->m_tType == m_UserItem[iSlot].tIQ)
		{
			if(strSerial.CompareNoCase(g_arAddEventItemTable[i]->m_strSerialNum) == 0)
			{
				if(InterlockedCompareExchange((LONG*)&g_arAddEventItemTable[i]->m_lUsed, (long)1, (long)0) == (long)0) 
				{
					index = i;
					g_arAddEventItemTable[i]->m_tEnd = 1;
					return g_arAddEventItemTable[i]->m_sSid;
				}
			}
		}

		nNum = 0;
	}

	return -1;
}

BOOL USER::UpdateEventItem(int sid)
{
	if(sid < 0) return FALSE;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];
	int				i = 1;

	SQLSMALLINT	sRet = -1;
	SQLINTEGER	iRetInd = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_GIVE_EVENT_ITEM(%d, ?)}"), sid);

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update EVENT_ITEM Data Only!!\n");
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		i = 1;
		SQLBindParameter( hstmt, i++ ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT,0,0, &sRet,0, &iRetInd);
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	if(sRet == 100 || sRet < 0)
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�̺�Ʈ ���� ��ǰ���� ȸ���ϱ����� �̺�Ʈ ��ȣ�� tIQ�� Ȯ������ �ش� �������� ������ŭ �ش�..
//
void USER::GiveEventItem(int EventItemIndex, int tIQ, int ItemIndex, int Count)
{
	return;

	int i, j;
	int event = 0;
	int event_index = -1;

	ItemList TempItem;

	ReSetItemSlot(&TempItem);

	for(i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INVENTORY_NUM; i++)
	{
		if(m_UserItem[i].sSid == EventItemIndex)
		{
			if(m_UserItem[i].tIQ == EVENT_POT_ITEM)
			{
				event = CheckEventItem(event_index, i, TRUE);
				if(event < 0) break;								// �ø��� ��ȣ�� Ʋ����.
																	// Ȥ�ó� �ʳ��°��� �����ϱ� ����
				if(event_index < 0) break;

				if(InterlockedCompareExchange((LONG*)&g_arEventItemTable[event_index]->m_lUsed, (long)1, (long)0) == (long)0)
				{
					TempItem = m_UserItem[i];

					ReSetItemSlot(&m_UserItem[i]);

					m_UserItem[i].sLevel = g_arItemTable[ItemIndex]->m_byRLevel;
					m_UserItem[i].sSid = ItemIndex;
					m_UserItem[i].sCount = Count;
					m_UserItem[i].sDuration = g_arItemTable[ItemIndex]->m_sDuration;
					m_UserItem[i].sBullNum = g_arItemTable[ItemIndex]->m_sBullNum;
					m_UserItem[i].tIQ = NORMAL_ITEM;

					if(!UpdateEventItem(event))
					{
						ReSetItemSlot(&m_UserItem[i]);
						m_UserItem[i] = TempItem;
						InterlockedExchange(&g_arEventItemTable[event_index]->m_lUsed, 0);
						
						break;
					}

					g_arEventItemTable[event_index]->m_tGiveItem = 1;		// ��Ͽ��� ����

					m_iCurWeight += g_arItemTable[ItemIndex]->m_byWeight * Count;

					UpdateUserItemDN();

					GetRecoverySpeed();								// ȸ���ӵ� üũ...

					CBufferEx TempBuf;
					TempBuf.Add(ITEM_USE_RESULT);

					TempBuf.Add((BYTE)0x01);						// ����
					TempBuf.Add((BYTE)103);							// ���� 1���� ����

					TempBuf.Add((BYTE)i);							// �� �����۸� ���� 	
					TempBuf.Add(m_UserItem[i].sLevel);
					TempBuf.Add(m_UserItem[i].sSid);
					TempBuf.Add(m_UserItem[i].sDuration);
					TempBuf.Add(m_UserItem[i].sBullNum);
					TempBuf.Add(m_UserItem[i].sCount);
					for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[i].tMagic[j]);
					TempBuf.Add(m_UserItem[i].tIQ); 
					Send(TempBuf, TempBuf.GetLength());	

					SendSystemMsg( IDS_USER_CHANGE_COMPLETED, SYSTEM_ERROR, TO_ME);
					return;
				}
			}
		}
	}

	SendSystemMsg( IDS_USER_NO_CHANGE_THIS, SYSTEM_ERROR, TO_ME);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�ش� �������� ������ŭ �ش�.
//
BOOL USER::GiveItem(int sid, int iCount)
{
	if( sid < 0 || sid >= g_arItemTable.GetSize() ) return FALSE;
	if( iCount <= 0 ) return FALSE;

	int iWeight = 0;
	ItemList GiveItem;
	ReSetItemSlot( &GiveItem );

	GiveItem.sLevel			= g_arItemTable[sid]->m_byRLevel;
	GiveItem.sSid			= sid;
	GiveItem.sCount			= iCount;
	GiveItem.sDuration		= g_arItemTable[sid]->m_sDuration;
	GiveItem.sBullNum		= g_arItemTable[sid]->m_sBullNum;
	GiveItem.tIQ			= NORMAL_ITEM;

	iWeight = g_arItemTable[sid]->m_byWeight * iCount;

	CWordArray		arEmptySlot, arSameSlot;

	int iSlot = GetSameItem( GiveItem, INVENTORY_SLOT);

	if(iSlot != -1)	
	{ 
		if(iCount != 0)
		{
			CheckMaxValue((short &)m_UserItem[iSlot].sCount, (short)iCount); 
			arSameSlot.Add(iSlot); 
		}
	}
	else			
	{
		iSlot = GetEmptySlot( INVENTORY_SLOT );

		if( iSlot < 0 ) return FALSE;

		ReSetItemSlot( &m_UserItem[iSlot] );

		m_UserItem[iSlot].sLevel = g_arItemTable[sid]->m_byRLevel;
		m_UserItem[iSlot].sSid = sid;
		m_UserItem[iSlot].sCount = iCount;
		m_UserItem[iSlot].sDuration = g_arItemTable[sid]->m_sDuration;
		m_UserItem[iSlot].sBullNum = g_arItemTable[sid]->m_sBullNum;
		m_UserItem[iSlot].tIQ = NORMAL_ITEM;

		arEmptySlot.Add(iSlot); 
	}			

	m_iCurWeight += iWeight;
	GetRecoverySpeed();					// ������ ���Կ� ������ ����� ȸ���ӵ� ��ȯ

	UpdateInvenSlot(&arEmptySlot, &arSameSlot);
/*
	int iSlot = GetEmptySlot(INVENTORY_SLOT);

	if(iSlot < 0) return FALSE;

	ReSetItemSlot(&m_UserItem[iSlot]);

	m_UserItem[iSlot].sLevel = g_arItemTable[sid]->m_byRLevel;
	m_UserItem[iSlot].sSid = sid;
	m_UserItem[iSlot].sCount = iCount;
	m_UserItem[iSlot].sDuration = g_arItemTable[sid]->m_sDuration;
	m_UserItem[iSlot].sBullNum = g_arItemTable[sid]->m_sBullNum;
	m_UserItem[iSlot].tIQ = NORMAL_ITEM;
*/
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�߰� �������� ���� ���ÿ� ������ �����Ѵ�.
//
void USER::StoppingTheFortressWar(CGuildFortress *pFort)
{	
	int i, type = 0;
	int nLen = 0;
	USER *pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildFortress *pBackFort = NULL;

	if(m_dwGuild <= 0) return;

	if(pFort == NULL) return;

	pBackFort = pFort;

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) 
	{
		ReleaseGuild();				// ����...
		return;
	}

	nLen = strlen(pGuild->m_strMasterName);
	::ZeroMemory(pFort->m_strMasterName, sizeof(pFort->m_strMasterName));
	strncpy(pFort->m_strMasterName, pGuild->m_strMasterName, nLen);

	ReleaseGuild();				// ����...

	nLen = CHAR_NAME_LENGTH;

	for(i = 0; i < GUILDFORTRESS_ATTACK_MAX_NUM; i++)
	{
		if(pFort->m_arAttackGuild[i].lGuild == m_dwGuild)
		{
			pFort->m_arAttackGuild[i].lGuild = pFort->m_iGuildSid;
			::ZeroMemory(pFort->m_arAttackGuild[i].strGuildName, CHAR_NAME_LENGTH + 1);		// ���������� �ٲٰ�
			strncpy(pFort->m_arAttackGuild[i].strGuildName, pFort->m_strGuildName, nLen);

			pFort->m_iGuildSid = m_dwGuild;
			::ZeroMemory(pFort->m_strGuildName, CHAR_NAME_LENGTH + 1);						// ���� �̸� �ٲٰ�
			strncpy(pFort->m_strGuildName, m_strGuildName, nLen);
			break;
		}
	}

/*
	CString strMsg = _T("");
	strMsg.Format( IDS_USER_GET_FORTRESS_SUCCESS, m_strGuildName);

	for(i = 0; i < MAX_USER; i++)
	{
		type = 0;
		pUser = m_pCom->GetUserUid(i);

		if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) continue;		

		type = pUser->CheckInvalidMapType();
		if(type >= 0 && type < 17)
		{												//������� �Ͼ�� ���������̸�
			if(pFort->m_iZone == pUser->m_curz)
			{
				if(pFort->m_iGuildSid != pUser->m_dwGuild)	pUser->TownPotal();				
				pUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ANNOUNCE, TO_ME);
			}
			else if(g_arMapTable[type]->m_sStoreID == pFort->m_sFortressID)		
			{											// �������� �ƴϸ� ��� ������	
				if(pFort->m_iGuildSid != pUser->m_dwGuild)	pUser->TownPotal();	
				pUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ANNOUNCE, TO_ME);
			}
			else if(pUser->m_curz == 1) //�Ϲ� �ʵ�� �ϴ� ������
			{
				pUser->SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ANNOUNCE, TO_ME);
			}
		}
	}
*/
	CBufferEx TempBuf;

	CNpc *pNpc = NULL;

	TempBuf.Add(GUILD_FORTRESS_NCIRCLE);
	TempBuf.Add((BYTE)0x01);
	TempBuf.Add((int)pFort->m_sFortressID);
	
	for(i = 0; i < FORTRESS_TARGET_MAX_NUM; i++)		// N_Circle�� �ʱ�ȭ�ؼ� ������ ���...
	{
		pNpc = GetNpc(pFort->m_arFortressTarget[i].sTargertID);

		if(!pNpc) continue;

		pNpc->m_tNCircle = NPC_NCIRCLE_DEF_STATE;
		pNpc->m_byColor = 0;
		pNpc->m_sHP = pNpc->m_sMaxHP;
		pNpc->m_lFortressState = 0;

		TempBuf.Add((int)(pNpc->m_sNid + NPC_BAND));
//		pNpc->SendFortressNCircleColor(m_pCom);				// Color�� ���� ���͵��� �˸���.
	}

	for(i = 0; i < pFort->m_arRepairNpcList.GetSize(); i++)
	{
		pNpc = GetNpc(pFort->m_arRepairNpcList[i]);

		if(!pNpc) continue;
		
		if(pNpc->m_tNpcType == NPCTYPE_GUILD_DOOR)
		{
			pNpc->m_sHP = pNpc->m_sMaxHP;
			pNpc->m_bFirstLive = TRUE;
			break;
		}
	}

	SendInsight(TempBuf, TempBuf.GetLength());
	// ���߿� Agent�� ��Ʈ������ ����!!!
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	DB�� UPDATE�Ѵ�.(��û��常 ������ ���̺� �߰��Ѵ�)
//
BOOL USER::UpdateFortress(CGuildFortress *pFort)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		

	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_FORTRESS_WARLIST(%d,%d,\'%s\')}"), 
	pFort->m_sFortressID, m_dwGuild, m_strGuildName);

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return FALSE;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild_Store Data Only!!\n");

		//g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DBNew[m_iModSid].ReleaseDB(db_index);
			return FALSE;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return FALSE;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//	DB�� �ְ� EVENT�� ����Ѵ�
//
void USER::UpdateHighEventLevelUser(short sLevel)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		

	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call ADD_EVENT_USER(\'%s\',%d)}"), m_strUserID, sLevel);

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild_Store Data Only!!\n");

		//g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DBNew[m_iModSid].ReleaseDB(db_index);
			return;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return;
}

void USER::SetTempStoreDN(DWORD dwDN, int sid)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		

	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call TEMP_MONEY(%d,%d)}"), sid, dwDN);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild_Store Data Only!!\n");

		//g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			BREAKPOINT();

			g_DB[m_iModSid].ReleaseDB(db_index);
			return;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	
	if( !bQuerySuccess ) return;
}

void USER::AddressWindowOpen(int sid, int quality)
{
	CBufferEx TempBuf;
	
	if( sid == NPC_EVENT_FLOWER )
	{
		TempBuf.Add(ADDRESS_WINDOW_OPEN_NEW);
	}
	else
	{
		TempBuf.Add(ADDRESS_WINDOW_OPEN);
	}
	TempBuf.Add( (short)sid );
	TempBuf.Add( (short)quality );

	Send(TempBuf, TempBuf.GetLength());
}

void USER::RecvAddress(TCHAR *pBuf)
{
	int index = 0;

	char strAddress[512];
	CString			str;
	SYSTEMTIME time;
	GetLocalTime(&time);

	memset( strAddress, NULL, 512 );

	int length = (int)GetByte( pBuf, index );

	if( length < 0 || length > 512 ) { SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME); return; }

	GetString( strAddress, pBuf, length, index );

	int sid = GetShort( pBuf, index );
	int quality = GetShort( pBuf, index );

	// �ɹٱ��� �̺�Ʈ ÷��
	if( sid != 634 && sid != 773 ) { SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME); return; }

	char strTitle[128];
	
	if( sid == 634 )
	{
		if( quality == 202 )
			sprintf( strTitle, _ID(IDS_USER_MUNHWA_PRESENT) );
		else if( quality == 201 )
			sprintf( strTitle, _ID(IDS_USER_DEPT_PRESENT) );
		else
		{
			SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME); return;
		}
	}
	else if( sid == 773 )
	{
		sprintf( strTitle, _ID(IDS_USER_FLOWER_PRESENT) );
	}
	else
	{
		SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME); return;
	}

	str.Format("(%04d-%02d-%02d %02d:%02d:%02d) %s - Flower change event applied\r\n",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,	m_strUserID );

	EnterCriticalSection( &m_CS_EventItemLogFileWrite );
	g_fpEventItem.Write( str, str.GetLength() );
	LeaveCriticalSection( &m_CS_EventItemLogFileWrite );

//	if( !EventRobItem( sid, quality ) )
//	{
//		SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME); return;
//	}

	SDWORD sTitleLen	= _tcslen(strTitle);
	SDWORD sContentLen	= _tcslen(strAddress);
	SDWORD sIDLen		= _tcslen(m_strUserID);

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	int bbsnum = 2;		// �̺�Ʈ�� �Խ���

	_sntprintf(szSQL, sizeof(szSQL), TEXT( "{call BBS_WRITE ( %d, ?, ?, ? )}" ), bbsnum );

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) { SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME); return; }

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
//		TRACE("Fail To Write BBS (BBS:%d,Writer:%s,Title:%d) !!\n", bbsnum, m_strUserID, strTitle);

//		g_DBNew[m_iModSid].ReleaseDB(db_index);

		SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME);
		return;
	}

	int i = 1;
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20,		0, (TCHAR*)m_strUserID,	0, &sIDLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 50,		0, (TCHAR*)strTitle,	0, &sTitleLen );
	SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 5000,	0, (TCHAR*)strAddress,	0, &sContentLen );

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

		SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME);
		return;
	}
	
	SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	str.Format("(%04d-%02d-%02d %02d:%02d:%02d) %s - Flower change event logged to bbs\r\n",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,	m_strUserID );

	EnterCriticalSection( &m_CS_EventItemLogFileWrite );
	g_fpEventItem.Write( str, str.GetLength() );
	LeaveCriticalSection( &m_CS_EventItemLogFileWrite );

	if( !EventRobItem( sid, quality ) )
	{
		SendSystemMsg( IDS_USER_REG_ADDR_FAIL, SYSTEM_NORMAL, TO_ME); return;
	}

	str.Format("(%04d-%02d-%02d %02d:%02d:%02d) %s - Flower change event completed\r\n",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,	m_strUserID );

	EnterCriticalSection( &m_CS_EventItemLogFileWrite );
	g_fpEventItem.Write( str, str.GetLength() );
	LeaveCriticalSection( &m_CS_EventItemLogFileWrite );

	SendSystemMsg( IDS_USER_REG_ADDR, SYSTEM_NORMAL, TO_ME);

	return;
}

void USER::GetResource()
{
	if(m_tIsOP == 0) return;

	//
	int nResourceFreeCount = 0;
	int CurUserCount = 0;
	int acceptedCount = 0;
	int connectedCount = 0;
	int disconnectedCount = 0;
	int logoutCount = 0;
	
	for ( int i = 0; i < MAX_USER; i++ )
	{
		if ( g_pUQM->m_pResources->IsFree( i ) == true )
		{
			nResourceFreeCount++;
			continue;
		}

		USER *pUser = (USER*)g_pUQM->m_pResources->GetDataValue( i );
		if ( pUser )
		{
			if ( pUser && pUser->m_state == STATE_GAMESTARTED )
				CurUserCount++;						
			else if( pUser && pUser->m_state == STATE_ACCEPTED )
				acceptedCount++;
			else if( pUser && pUser->m_state == STATE_CONNECTED )
				connectedCount++;
			else if( pUser && pUser->m_state == STATE_DISCONNECTED )
				disconnectedCount++;
			else if( pUser && pUser->m_state == STATE_LOGOUT )
				logoutCount++;
		}
	};

	int nLeftResource = g_pUQM->FreeResourcesLeft();
	//
//	sprintf( msg, _ID(IDS_USER_SYSTEM_MSG03), CurUserCount, nLeftResource, nResourceFreeCount );

	TCHAR strTitle[256];
	::ZeroMemory(strTitle, sizeof(strTitle));
	wsprintf(strTitle, _ID(IDS_USER_SYSTEM_MSG04), CurUserCount, acceptedCount, connectedCount, disconnectedCount, logoutCount, nLeftResource, nResourceFreeCount);
	
	SendSystemMsg(strTitle, SYSTEM_NORMAL, TO_ME);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	������ ��û�� ��ü ��� ����� �����ش�.(��������)
//
void USER::SendFortressAttackGuildList(TCHAR *pBuf)
{
	int i, j;
	short nCount = 0;
	short sFortID = 0;

	BOOL bSuc = FALSE;

	CGuild *pGuild = NULL;
	CStore *pStore = NULL;
	CGuildFortress *pFort = NULL;
	CGuildFortress *pTempFort = NULL;

	int index = 0;
	sFortID = GetShort(pBuf, index);

	if(m_tIsOP != 1)
	{
		if(m_dwGuild <= 0) return;
		if(!m_bGuildMaster)
		{
			SendSystemMsg( IDS_USER_NOT_GUILD_MASTER, SYSTEM_ERROR, TO_ME);
			return;
		}
		if(sFortID != 1000 && sFortID != 1001 && sFortID != 1002)
		{
			SendSystemMsg( IDS_USER_UNCLEAR_FORTRESS, SYSTEM_ERROR, TO_ME);
			return;
		}

		for(i = 0; i < g_arGuildFortress.GetSize(); i++)
		{
			if(g_arGuildFortress[i] == NULL) continue;

			pTempFort = g_arGuildFortress[i];
			if(pTempFort->m_sFortressID != sFortID) continue;

			if(pTempFort->m_iGuildSid == m_dwGuild) 
			{
				bSuc = TRUE;
				break;
			}

			for(j = 0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)
			{
				if(pTempFort->m_arAttackGuild[j].lGuild == m_dwGuild)
				{
					bSuc = TRUE;
					break;
				}
			}

			if(bSuc) break;
		}
	}
	else bSuc = TRUE;

	if(!bSuc) 
	{
		SendSystemMsg( IDS_USER_NOT_APPLY_GUILD, SYSTEM_ERROR, TO_ME);
		return;
	}

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		pFort = g_arGuildFortress[i];
		if(!pFort) continue;

		nCount = 0;
		if(pFort->m_sFortressID == sFortID)
		{
			if(pFort->m_lUsed == 1)
			{
				SendSystemMsg( IDS_USER_CANT_USE_IN_DEFENCE, SYSTEM_ERROR, TO_ME);
				return;
			}

			CBufferEx TempBuf, AddData;

			TempBuf.Add(CHALLENGE_GUILD);			
			
			for(j =0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)
			{
				if(pFort->m_arAttackGuild[j].lUsed == 1)
				{
					AddData.AddString(pFort->m_arAttackGuild[j].strGuildName);
					nCount++;
				}
			}

			TempBuf.Add(nCount);
			TempBuf.AddData(AddData, AddData.GetLength());

			Send(TempBuf, TempBuf.GetLength());
			return;
		}
	}

	if(m_tIsOP == 1) SendSystemMsg( IDS_USER_UNKNOWN_ERROR, SYSTEM_ERROR, TO_ME);
}

void USER::SendRepairItem(int sid)
{
	int i, j;
	CNpc *pNpc = NULL;
	CGuild *pGuild = NULL;
	CGuildFortress *pFort = NULL;

	if(!m_dwGuild) return;

	pFort = GetFortress(sid);

	if(!pFort) return;
	if(!pFort->m_bHaveGuild) return;
	if(pFort->m_iGuildSid != m_dwGuild) return;

	if(pFort->m_lUsed == 1)
	{
		SendSystemMsg( IDS_USER_CANT_USE_IN_DEFENCE, SYSTEM_ERROR, TO_ME);
		return;
	}

	if(pFort->m_dwRepairCost <= 0) 
	{
		SendSystemMsg( IDS_USER_NO_MORE_REPAIR, SYSTEM_ERROR, TO_ME);
		return;
	}

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild)
	{
		ReleaseGuild();
		return;
	}

	int nLen = strlen(m_strUserID);

	if( nLen <= 0 || nLen > CHAR_NAME_LENGTH ) 
	{
		ReleaseGuild();
		return;
	}

	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0)
	{
		ReleaseGuild();
		SendSystemMsg( IDS_USER_ONLY_GUILD_MASTER_USE, SYSTEM_ERROR, TO_ME);
		return;
	}

	ReleaseGuild();

/*	if(InterlockedCompareExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)1, (LONG)0) == 0)
	{
		m_dwGuildDN = 0;
		InitGuildItem();								// ������ ������ ��Ź�Ѵ�.

		if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) 
		{ 
			InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0); 
			SendSystemMsg( IDS_USER_FAIL, SYSTEM_ERROR, TO_ME);
			return; 
		}

		if(!LoadGuildWarehouse())						
		{ 
			InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0); 
			SendSystemMsg( IDS_USER_FAIL, SYSTEM_ERROR, TO_ME);
			return; 
		}

		if(m_dwGuildDN < pFort->m_dwRepairCost)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_REPAIR_DINA, SYSTEM_ERROR, TO_ME);
			return;
		}

		m_dwGuildDN -= pFort->m_dwRepairCost;

		UpdateGuildWarehouse();
		InterlockedExchange(&g_arGuildData[m_dwGuild]->m_lUsed, (LONG)0); 
*/
		if(m_dwDN < pFort->m_dwRepairCost)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_REPAIR_HAVEDINA, SYSTEM_ERROR, TO_ME);
			return;
		}
		else m_dwDN -= pFort->m_dwRepairCost;
//		if(m_dwDN < 0) m_dwDN = 0;

		UpdateUserItemDN();
		SendMoneyChanged();

		pFort->m_dwRepairCost = 0;

		for(i = 0; i < pFort->m_arRepairNpcList.GetSize(); i++)
		{
			pNpc = GetNpc(pFort->m_arRepairNpcList[i]);

			if(!pNpc) continue;

			for(j = 0; j < GUILD_REPAIR_MAX_NUM; j++)
			{			
				if(pFort->m_arRepairDBList[j].sUid == pNpc->m_sEZone)
				{
					pNpc->m_sHP = pNpc->m_sMaxHP;
					pNpc->m_tRepairDamaged = NPC_NON_REPAIR_STATE;
					pFort->m_arRepairDBList[j].sHP = pNpc->m_sMaxHP;
					break;
				}
			}
		}

		SendSystemMsg( IDS_USER_REPAIR_COMPLETED, SYSTEM_NORMAL, TO_ME);
		UpdateMemRepairNpc(sid);

}

void USER::SendPeopleSay(int sid)
{
	int degree = 0, say = -1;
	CGuildFortress *pFort = NULL;

	if(!m_dwGuild) return;

	pFort = GetFortress(sid);

	if(!pFort) return;

	if(!pFort->m_bHaveGuild) return;
	if(pFort->m_iGuildSid != m_dwGuild) return;

//	degree = pFort->m_iCityDegree + pFort->m_tTaxRate;
	degree = pFort->GetTotalCityValue();
	
	if(degree <= 20)
	{
		if(sid == 1000) say = 135;
		else if(sid == 1001) say = 155;
		else if(sid == 1002) say = 308;
	}
	else if(degree <= 40)
	{
		if(sid == 1000) say = 136;
		else if(sid == 1001) say = 156;
		else if(sid == 1002) say = 309;
	}
	else if(degree <= 60)
	{
		if(sid == 1000) say = 137;
		else if(sid == 1001) say = 157;
		else if(sid == 1002) say = 310;
	}
	else
	{
		if(sid == 1000) say = 138;
		else if(sid == 1001) say = 158;
		else if(sid == 1002) say = 311;
	}

	if(say >= 0) SendNpcSay(NULL, say);
}

void USER::UpdateMemRepairNpc(int iSid)
{
	int nSize, i;
	CGuildFortress *pFort = NULL;

	if(iSid >= FORTRESS_BAND)
	{
		nSize = g_arFortressSharedMemory.GetSize();
		CSharedMemory* pShared = NULL;
		CMemFortress* pData = NULL;

		pFort = GetFortress(iSid);
		if(!pFort) return;

		if(!pFort->m_bHaveGuild || pFort->m_iGuildSid != m_dwGuild) return;

		for(i = 0; i < nSize; i++)
		{
			pShared = g_arFortressSharedMemory[i];
			
			if(pShared == NULL) return;
			if(pShared->m_hMapping == NULL) return;
			
			pData = (CMemFortress*) pShared->m_lpData;
			
			if(pData == NULL) return;
			if(pData->m_sFortressID <= 0) return;
			if(pData->m_iGuildSid != m_dwGuild) return;
			
			pFort->FortressRepairListToStr(pData->m_RepairList);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	������ ��û�� .(��������)
//
void USER::UpdateMemAttackListNpc(int iSid)
{
/*	int nSize, i;
	CGuildFortress *pFort = NULL;

	if(iSid >= FORTRESS_BAND)
	{
		nSize = g_arFortressSharedMemory.GetSize();
		CSharedMemory* pShared = NULL;
		CMemFortress* pData = NULL;

		pFort = GetFortress(iSid);
		if(!pFort) return;

		for(i = 0; i < nSize; i++)
		{
			pShared = g_arFortressSharedMemory[i];
			
			if(pShared == NULL) continue;
			if(pShared->m_hMapping == NULL) continue;
			
			pData = (CMemFortress*) pShared->m_lpData;
			
			if(pData == NULL) continue;
			if(pData->m_sFortressID <= 0) continue;
			if(pData->m_sFortressID != pFort->m_sFortressID) continue;
			
			pFort->GuildAttListToStr(pData->m_AttackList);
			break;
		}
	}
*/
}

BOOL USER::CheckInGuildWarring()
{
	int i, j;

	CStore *pStore = NULL;
	CGuildFortress *pFort = NULL;

	// �������϶�...
	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		pFort = g_arGuildFortress[i];
		if(!pFort) continue;

		if(g_arGuildFortress[i]->m_lUsed == 1)
		{
			if(pFort->m_iGuildSid == m_dwGuild)	// ���������
			{
				SendSystemMsg( IDS_USER_NO_PERMISSION_IN_WAR, SYSTEM_ERROR, TO_ME);
				return TRUE;
			}

			for(j =0; j < GUILDFORTRESS_ATTACK_MAX_NUM; j++)// ����������..
			{
				if(pFort->m_arAttackGuild[j].lGuild == m_dwGuild)
				{
					SendSystemMsg( IDS_USER_NO_PERMISSION_IN_WAR, SYSTEM_ERROR, TO_ME);
					return TRUE;
				}
			}
		}
	}
	// ������...
/*	for(i = 0; i < g_arStore.GetSize(); i++)
	{
		pStore = g_arStore[i];
		if(!pStore) continue;

		if(g_arStore[i]->m_lUsed == 1)
		{
			if(pStore->m_iGuildSid == m_dwGuild)	// ���������
			{
				SendSystemMsg( IDS_USER_NO_PERMISSION_IN_WAR, SYSTEM_ERROR, TO_ME);
				return TRUE;
			}

			for(j =0; j < GUILD_ATTACK_MAX_NUM; j++)// ����������..
			{
				if(pStore->m_arAttackGuild[j] == m_dwGuild)
				{
					SendSystemMsg( IDS_USER_NO_PERMISSION_IN_WAR, SYSTEM_ERROR, TO_ME);
					return TRUE;
				}
			}
		}
	}
*/
	// Virtual Room�� ������
	for(i = 0; i < g_arGuildHouseWar.GetSize(); i++)
	{
		if(!g_arGuildHouseWar[i]) continue;

		if(g_arGuildHouseWar[i]->m_CurrentGuild.lUsed == 1)
		{
			if(g_arGuildHouseWar[i]->m_CurrentGuild.lGuild == m_dwGuild)
			{
				SendSystemMsg( IDS_USER_NO_PERMISSION_IN_WAR, SYSTEM_ERROR, TO_ME);
				return TRUE;
			}
		}
	}

	return FALSE;
}

int USER::SocketDisConnect()
{
	BYTE len;
	LOGINOUTTHREADDATA *pLIOTD;
	pLIOTD = new LOGINOUTTHREADDATA;
	pLIOTD->UID = m_Sid;
	len = (BYTE)strlen(m_strUserID);
	memcpy(pLIOTD->ID, &len, sizeof(BYTE) );
	memcpy(pLIOTD->ID+sizeof(BYTE), m_strUserID, len);
	pLIOTD->ID[sizeof(BYTE)+len] = '\0';

	EnterCriticalSection( &m_CS_LogoutData );
	RecvLogoutData.AddTail(pLIOTD);
	nLogoutDataCount = RecvLogoutData.GetCount();
	LeaveCriticalSection( &m_CS_LogoutData );

	return 1;
}

void USER::CheckGuildUserInFortress()
{
	int i, j;
	int ilen = 0;

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i] || !g_arGuildFortress[i]->m_bHaveGuild) continue;

		if(g_arGuildFortress[i]->m_iGuildSid == m_dwGuild)
		{
			for(j = 0; j < MAX_GUILD_USER; j++)
			{
				ilen = strlen(g_arGuildFortress[i]->m_arCityRankList[j].strUserID);
				if(ilen <= 0 || ilen > CHAR_NAME_LENGTH) continue;

				if(strcmp(m_strUserID, g_arGuildFortress[i]->m_arCityRankList[j].strUserID) == 0)
				{
					InterlockedExchange(&g_arGuildFortress[i]->m_arCityRankList[j].lCityRank, (long)m_sCityRank);
					break;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	��忡 �����Ҷ� �ùε�� ��ġ�� �����Ѵ�.
//
void USER::AddGuildUserInFortress(USER *pUser)
{
	int i, j;

	if(!pUser) return;

	int nLen = strlen(pUser->m_strUserID);

	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i] || !g_arGuildFortress[i]->m_bHaveGuild) continue;

		if(g_arGuildFortress[i]->m_iGuildSid == m_dwGuild)
		{
			for(j = 0; j < MAX_GUILD_USER; j++)
			{
				if(InterlockedCompareExchange((LONG*)&g_arGuildFortress[i]->m_arCityRankList[j].lUsed, (long)1, (long)0) == (long)0)
				{
					::ZeroMemory(g_arGuildFortress[i]->m_arCityRankList[j].strUserID, sizeof(g_arGuildFortress[i]->m_arCityRankList[j].strUserID));
					g_arGuildFortress[i]->m_arCityRankList[j].lCityRank = (long)pUser->m_sCityRank;
					strncpy(g_arGuildFortress[i]->m_arCityRankList[j].strUserID, pUser->m_strUserID, nLen);
					return;
				}
			}		
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	��忡�� ���� Ż���Ҷ� �ùε�� ��ġ�� �����Ѵ�.
//
void USER::DelGuildUserInFortress(TCHAR *strUserID, int iGuild)
{
	int i, j;
	int ilen = 0;

	if(iGuild <= 0) return;

	for(i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i] || !g_arGuildFortress[i]->m_bHaveGuild) continue;

		if(g_arGuildFortress[i]->m_iGuildSid == iGuild)
		{
			for(j = 0; j < MAX_GUILD_USER; j++)
			{
				ilen = strlen(g_arGuildFortress[i]->m_arCityRankList[j].strUserID);
				if(ilen <= 0 || ilen > CHAR_NAME_LENGTH) continue;

				if(strcmp(strUserID, g_arGuildFortress[i]->m_arCityRankList[j].strUserID) == 0)
				{
					::ZeroMemory(g_arGuildFortress[i]->m_arCityRankList[j].strUserID, sizeof(g_arGuildFortress[i]->m_arCityRankList[j].strUserID));
					g_arGuildFortress[i]->m_arCityRankList[j].lCityRank = 0;
					InterlockedExchange(&g_arGuildFortress[i]->m_arCityRankList[j].lUsed, (long)0);
					break;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	���� ���� �ڸ��� ��â �������� �������� �ڸ��� ������ �� �Ȱ��� Ȯ���� ����޴´�.
//
BOOL USER::CheckGuildWarArea()
{
	int mapindex = 0;
	int index = 0;
	int type = CheckInvalidMapType();

	mapindex = GetGuildMapIndex(type);
	if(mapindex >= 0 && mapindex < g_arMapTable.GetSize())
	{
		index = g_arMapTable[mapindex]->m_sStoreID;

		if(index >= FORTRESS_BAND)
		{
			CGuildFortress* pFort = NULL;
			pFort = GetFortress(index);

			if(pFort) 
			{
				if(pFort->m_lUsed == 1 && g_arMapTable[mapindex]->m_sStoreZone)
				{
					return TRUE;
				}
			}
		}
/*		else 
		{
			CStore *pStore = NULL;

			index = g_arMapTable[mapindex]->m_sStoreIndex;

			pStore = GetStore(index);

			if(pStore)
			{
				if(pStore->m_lUsed == 1)								// �ش� �Ӽ� ���� ����� ����	
				{
					return TRUE;
				}
			}
		}
*/	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	��Ʈ���� �ܼ����� ���� �ݴ´�.
//
void USER::OpenFortressDoor()
{
	if(m_dwGuild <= 0 || !m_bGuildMaster) return;
	
	CNpc *pNpc = NULL;

	for(int i = 0; i < g_arGuildFortress.GetSize(); i++)
	{
		if(!g_arGuildFortress[i] || !g_arGuildFortress[i]->m_bHaveGuild) continue;

		if(g_arGuildFortress[i]->m_lUsed == 1) continue;
		if(g_arGuildFortress[i]->m_lViolenceUsed == 1) continue;

		if(g_arGuildFortress[i]->m_iGuildSid == m_dwGuild)
		{
			for(int j = 0; j < g_arGuildFortress[i]->m_arRepairNpcList.GetSize(); j++)
			{
				pNpc = GetNpc(g_arGuildFortress[i]->m_arRepairNpcList[j]);

				if(!pNpc) continue;

				if(pNpc->m_sHP > 0 && pNpc->m_tNpcType == NPCTYPE_GUILD_DOOR)
				{
					pNpc->m_sHP = 0;

					MAP* pMap = g_zone[pNpc->m_ZoneIndex];
					pMap->m_pMap[pNpc->m_sCurX][pNpc->m_sCurY].m_lUser = 0;

					pNpc->m_NpcState = NPC_DEAD;

					pNpc->m_Delay = pNpc->m_sRegenTime;
					pNpc->m_bFirstLive = FALSE;

					pNpc->SetMapAfterGuildWar();

					pNpc->SendDead(m_pCom);
					return;
				}
				else
				{
					if(pNpc->m_tNpcType == NPCTYPE_GUILD_DOOR)
					{
						pNpc->m_sHP = pNpc->m_sMaxHP;
						pNpc->m_bFirstLive = TRUE;
						return;
					}			
				}
			}
		}
	}
}

ZONEINFO* USER::GetZoneInfo(int zone)
{
	ZONEINFO* pZoneInfo = NULL;

	for( int i = 0; i < g_TownPotal.GetSize(); i++ )
	{
		if( g_TownPotal[i] )
		{
			pZoneInfo = g_TownPotal[i];

			if( pZoneInfo->iZone == zone )
			{
				return pZoneInfo;
			}
		}
	}

	return NULL;
}

void USER::UpdateCurrentUserTable()
{
	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	// session init
	m_bSessionLogOut = FALSE;
	m_iDisplayType = 0;			// �޼��� Ÿ��
	m_iValidTime = 0;			// ��ȿ �ð�
	m_iTimeInterval = 50;		// �޼��� ����

	BYTE bDisplay = 0;
	int remainTime = 0;
	TIMESTAMP_STRUCT endTime;

	SWORD	sParam1 = 0;
	SDWORD	cbParam1 = SQL_NTS, cbParam2 = SQL_NTS, cbParam3 = SQL_NTS, cbParam4 = SQL_NTS;

	endTime.year = 0;
	endTime.month = 0;
	endTime.day = 0;
	endTime.hour = 0;
	endTime.minute = 0;

	memset(szSQL, 0x00, 1024);
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_CURRENTUSER (\'%s\', \'%s\',?, ?,?,?)}"), m_strAccount, g_arServerIPAddr, sParam1,remainTime, endTime, bDisplay);

	hstmt = NULL;
	retcode = 0;

	int db_index = -1;
	CDatabase* pDB = g_DBSession[m_iModSid].GetDB( db_index );
	if( !pDB ) 
	{
		SoftClose();	// �α� �ƿ� ������ ����.
		return;
	}
	
	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode!=SQL_SUCCESS)
	{
		SoftClose();	// �α� �ƿ� ������ ����.
		return;
	}
	
	if( retcode != SQL_SUCCESS )
	{
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		SoftClose();	// �α� �ƿ� ������ ����.
		return ;
	}

	retcode = SQLBindParameter(hstmt, 1,SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT,0,0,&sParam1,0,&cbParam1);
	retcode = SQLBindParameter(hstmt, 2,SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER,0,0,&remainTime,0,&cbParam2);
	retcode = SQLBindParameter(hstmt, 3,SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP,0,0,&endTime,0,&cbParam3);
	retcode = SQLBindParameter(hstmt, 4,SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT,0,0,&bDisplay,0,&cbParam4);


    retcode = SQLExecDirect (hstmt, (unsigned char *)(LPCTSTR)szSQL, SQL_NTS);
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

	if(sParam1 != 1)
	{
		SoftClose();	// �α� �ƿ� ������ ����.
		return;
	}

	GetLocalTime(&m_LoginTime);		// �α����� �ð��� ����
	m_bSessionLogOut = FALSE;
	m_iTimeInterval = (int)remainTime;// ǥ�ø� ����
	m_iDisplayType = (int)bDisplay;	// ���ӹ����� ���������� �ð������� ǥ��
	m_iValidTime = (int)remainTime;	// �ð������ ���� �ð��� ������ ǥ���Ѵ�.

	//----------------------------------------------------------------------------------------------
	//yskang 0.5 ������ ����ڴ� 1�� 2������ ����� ����� �� �ִ�.
	//----------------------------------------------------------------------------------------------
	m_bMGame = FALSE;//������ ����ڰ� �ƴϴٷ� �ʱ�ȭ
	if(m_iDisplayType > 200)
	{
		m_iDisplayType -= 200;//������ ����ڴ� Ÿ�Կ� 200�� ������ �ִ�.
		m_bMGame = TRUE;//������ ������̰� 1�� 2������ �̴�.
	}
	
	if(m_iDisplayType == 1 || m_iDisplayType == 3 || m_iDisplayType == 5) // �Ⱓ����� 
	{
		m_validDate.wYear = endTime.year;
		m_validDate.wMonth = endTime.month;
		m_validDate.wDay = endTime.day;
		m_validDate.wHour = endTime.hour;
		m_validDate.wMinute = endTime.minute;
	}
	GetCheckValidTime();			// �ð��� ǥ���Ѵ�.
	//-------------------------------------------------------------------------------------------------

	TRACE("SESSION DB UPDATE SUCCESS : %s\n", m_strAccount);

	return;
}

void USER::GetCheckValidTime()
{
	if((m_bMGame == TRUE) && (m_iDisplayType == 5)) // yskang 0.5 ������ 1�� 2�� ����� ó�� �ð� ����� ���� �ʴ´�.
		return;
	// ���� �������� ������ �ð�, �Ⱓ�� �˷��ش�.
	SYSTEMTIME currTime;
	GetLocalTime(&currTime);

	CTime curr(currTime);

	int tempValue = 0;

	CString strMsg = _T("");

	if(m_iDisplayType == 1 || m_iDisplayType == 3)
	{
		CTimeSpan DiffDay;
		CTime validDay(m_validDate);

		DiffDay = validDay - curr;
		tempValue = (int)DiffDay.GetDays();

		if(tempValue <= 3)
		{
			if(tempValue == 0)
			{
				strMsg.Format( IDS_USER_UNTIL_TODAY_YOUR_GAMETIME );
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);
			}
			else
			{
				strMsg.Format( IDS_USER_REMAIN_ACCOUNT_USE, tempValue);
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);
			}
			return;
		}	
	}
	else if(m_iDisplayType == 2 || m_iDisplayType == 4)
	{
		if(m_iDisplayType == 2)
		{
			if(m_iValidTime <= 50) //����
			{
				strMsg.Format( IDS_USER_REMAIN_GAMETIME_MIN, m_iValidTime);
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);
				return;
			}
		}
		else
		{
			if(m_iValidTime <= 3000) // ���ӹ�
			{
				int ihour = (int)(m_iValidTime / 60);
				int iMin = (int)(m_iValidTime % 60);

				strMsg.Format( IDS_USER_REMAIN_GAMEROOMTIME, ihour, iMin);
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_ANNOUNCE, TO_ME);
				return;
			}
		}
	}
/*	else if(m_iDisplayType == 6)
	{
		strMsg.Format("�������� �����Ͻ� ü���� �������� ��ɻ� ������ �ֽ��ϴ�.");
		SendSystemMsg(strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_NORMAL, TO_ME);
	}
*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�α渶�� �Ӹ�, ���� �̾�
//
void USER::DeputeGuildMasterOfPower(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int index = 0;
	int nLen = 0;

	int uid = 0;
	USER* pUser = NULL;
	CGuild *pGuild = NULL;

	if(!m_bGuildMaster) return; 	// ��� ������ ����.

	BYTE error_code = 0;
	BOOL bRes = TRUE;

	uid = GetInt(pBuf, index);

	pUser = GetUser(uid - USER_BAND);

	if(!pUser || pUser->m_state != STATE_GAMESTARTED) return;	// ������ ����.

	if(IsThereUser(pUser) == FALSE || strcmp(m_strUserID, pUser->m_strUserID) == 0)
	{
		SendSystemMsg( IDS_USER_SEE_EACH_OTHER, SYSTEM_NORMAL, TO_ME);
		return;
	}

	nLen = strlen(pUser->m_strUserID);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH) return;

	CString strMsg = _T("");

	if(pUser->m_dwGuild != m_dwGuild)  
	{
		error_code = ERR_9; 
		goto go_result;		
	}// �ٸ� ��忡 ������ ���� 
	
	if(CheckInGuildWarring()) return;// �����߿��� ����Ҽ�����.

	pGuild = GetGuild(m_dwGuild);

	if(!pGuild) 
	{
		ReleaseGuild();				// ����...
		error_code = ERR_7;			// �ش� ��尡 ����.
		goto go_result;				
	}
									// ����...
	if(strcmp(pGuild->m_strMasterName, m_strUserID) != 0) 
	{
		ReleaseGuild();				// ����...
		error_code = ERR_10;		// 
		goto go_result;
	}

	if(pGuild->m_lSubMaster == 1)		// ���� �α渶�� �������� �˷��ش�.
	{
		strMsg = _T("");
		strMsg.Format(IDS_USER_GUILD_SUBMASTER, pGuild->m_strSubMasterName);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		ReleaseGuild();				// ����...
		return;
	}

	index = -1;
	index = pGuild->GetUser(pUser->m_strUserID);
	ReleaseGuild();					// ����...

	if(index < 0)
	{	
		error_code = ERR_8;			// 
		goto go_result;		
	}


	// Add By Youn Gyu
	g_pMainDlg->BridgeDeputeGuildMasterOfPowerReq( m_uid, pUser->m_uid, m_strUserID, pUser->m_strUserID, (int)m_dwGuild );
	return;
	//


	if(UpdateGuildSubMaster(pUser->m_strUserID, TRUE) <= 0)
	{
		return;
	}
	
	pGuild = GetGuild(m_dwGuild);

	if(pGuild == NULL) 
	{
		ReleaseGuild();				// ����...
		error_code = ERR_7;			// �ش� ��尡 ����.
		goto go_result;				
	}

	pUser->m_bRepresentationGuild = TRUE;
	pGuild->CheckGuildSubMaster();
	pGuild->SetSubGuildMaster(pUser->m_strUserID);

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���Ѹ� ���´�.
//
void USER::RemoveGuildMasterOfPower(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

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
	if(!pGuild->IsMasterPower(m_strUserID))
	{
		ReleaseGuild();				// ����...
		error_code = ERR_10;		// 
		goto go_result;
	}

	nLen = 0;
	nLen = strlen(pGuild->m_strSubMasterName);
	if(pGuild->m_lSubMaster != 1 || (nLen <= 0 || nLen > CHAR_NAME_LENGTH) )		// ���� �α渶���ٰ� �˷��ش�.
	{
		SendSystemMsg( IDS_USER_GUILD_SUBMASTER_NOT, SYSTEM_ERROR, TO_ME);
		ReleaseGuild();				// ����...
		return;
	}

	ReleaseGuild();	

	// Add By Youn Gyu
	g_pMainDlg->BridgeRemoveGuildMasterOfPowerReq( m_uid, m_strUserID, (int)m_dwGuild );
	return;
	//



	if(UpdateGuildSubMaster(NULL, FALSE) <= 0)
	{
		return;
	}
	
	pGuild = GetGuild(m_dwGuild);

	if(pGuild == NULL) 
	{
		ReleaseGuild();				// ����...
		error_code = ERR_7;			// �ش� ��尡 ����.
		goto go_result;				
	}

	ReleaseGuild();				// ����...

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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���� �̾縦 ����
//
int USER::UpdateGuildSubMaster(TCHAR *strSubMaster, BOOL bPower)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	BOOL			bQuerySuccess = TRUE;
	TCHAR			szSQL[8000];		

	::ZeroMemory(szSQL, sizeof(szSQL));

	SWORD	sRet = 0;
	SDWORD	cbRet = SQL_NTS;

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_GUILD_SUBMASTER (%d, \'%s\', \'%s\', %d,?)}"), m_dwGuild, m_strUserID, strSubMaster, bPower, sRet); 

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return -1;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode != SQL_SUCCESS)
		return -1;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1,SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT,0,0,&sRet,0,&cbRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
	}
	
	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);

	if( sRet != 1 ) return FALSE;

	return TRUE;	
}

void USER::EncryptionStartReq(TCHAR *pBuf)
{
	if(m_isCryptionFlag != 0) return;
	
	int index = 0;
	BYTE tType = GetByte(pBuf, index);
	
	if(tType >= 3) return;

	index = 0;
	SetByte(m_TempBuf, ENCRYPTION_START_RESULT, index );
	SetString(m_TempBuf, (char *)&m_public_key, sizeof(_int64), index);
	SetByte(m_TempBuf, tType, index);

	m_isCryptionFlag = 0;

	Send( m_TempBuf, index );

	m_isCryptionFlag = 1;
}

void USER::SetPublicKey()
{
	// IKING 2001.1.
    srand( (unsigned)time( NULL ) );
	//
	BYTE rand1,rand2,rand3,rand4,rand5,rand6,rand7,rand8;

	int out_flag = 0;
	do
	{
		rand1 = rand();
		rand2 = rand();
		rand3 = rand();
		rand4 = rand();
		rand5 = rand();
		rand6 = rand();
		rand7 = rand();
		rand8 = rand();

		m_public_key = rand1;
		m_public_key <<= 8;

		m_public_key |= rand2;
		m_public_key <<= 8;

		m_public_key |= rand3;
		m_public_key <<= 8;

		m_public_key |= rand4;
		m_public_key <<= 8;

		m_public_key |= rand5;
		m_public_key <<= 8;

		m_public_key |= rand6;
		m_public_key <<= 8;

		m_public_key |= rand7;
		m_public_key <<= 8;

		m_public_key |= rand8;

		if(m_public_key != 0)
			out_flag = 1;

	} while( !out_flag );
#ifdef _CHINA
	m_public_key = 0x1010101010101010; test code
#endif
}

void USER::SendGuildInviteReq(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int index = 0;
	BYTE error_code = 0;

	int nLen = 0;

	int uid = 0;
	int iCount = 0;
	USER* pUser = NULL;
	CGuild *pGuild = NULL;
	CGuildUser *pGuildUser = NULL;

	BOOL bRes = TRUE;

	if(!m_bGuildMaster && !m_bRepresentationGuild) { error_code = ERR_10; goto go_result; }	// ��� ������ ����.

	uid = GetInt(pBuf, index);

	pUser = GetUser(uid - USER_BAND);

	if(!pUser) { error_code = ERR_3; goto go_result; }				// ������ ����.

	if(IsThereUser(pUser) == FALSE || strcmp(m_strUserID, pUser->m_strUserID) == 0)
	{
		SendSystemMsg( IDS_USER_SEE_EACH_OTHER, SYSTEM_NORMAL, TO_ME);
		return;
	}

	nLen = strlen(pUser->m_strUserID);
	if(nLen <= 0) return;

	if(pUser->m_dwGuild > 0)  { error_code = ERR_9; goto go_result; }// �̹� �ٸ� ��忡 ������ ���� 
	
	if(CheckInGuildWarring()) return;								// �����߿��� ����A������.

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

	index = -1;
	index = pGuild->GetUser(pUser->m_strUserID);
	if(index >= 0) 
	{ 
		ReleaseGuild();				// ����...

		pUser->m_dwGuild = m_dwGuild;			// ��� ��ȣ�� �ٽ� �����Ѵ�.
		SendGuildInfo(pUser);

		error_code = ERR_9; 
		goto go_result; 
	}

	ReleaseGuild();				// ����...
	bRes = FALSE;

go_result:

	CBufferEx TempBuf;

	if(bRes)
	{
		TempBuf.Add(GUILD_INVITE_RESULT);
		TempBuf.Add((BYTE)0x04);		//����
		TempBuf.Add(error_code);
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	TempBuf.Add(GUILD_INVITE_REQ);
	TempBuf.Add(m_uid + USER_BAND);

	pUser->Send(TempBuf, TempBuf.GetLength());
}

void USER::GuildInviteResult(TCHAR *pBuf)
{
	if ( pBuf == NULL ) return;

	int index = 0;
	BYTE error_code = 0;

	int uid = 0;
	USER* pUser = NULL;

	CBufferEx TempBuf;
	TCHAR szGuildName[CHAR_NAME_LENGTH + 1];
	::ZeroMemory(szGuildName, sizeof(szGuildName));

	BYTE tType = GetByte(pBuf, index);

	uid = GetInt(pBuf, index);
	if(uid < 0 || uid >= INVALID_BAND) { error_code = ERR_3; goto go_result; }

	pUser = GetUser(uid - USER_BAND);								// ����
	if(!pUser) { error_code = ERR_3; goto go_result; }				// ������ ����.

	if(tType == 1)
	{
		if(!pUser->m_bGuildMaster && !pUser->m_bRepresentationGuild) { error_code = ERR_10; goto go_result; }

		pUser->GuildInvite(m_uid + USER_BAND);

		TempBuf.Add(GUILD_INVITE_RESULT);
		TempBuf.Add((BYTE)0x01);

		Send(TempBuf, TempBuf.GetLength());
		pUser->Send(TempBuf, TempBuf.GetLength());
		return;
	}
	else
	{
		TempBuf.Add(GUILD_INVITE_RESULT);
		TempBuf.Add((BYTE)0x03);		// ���� ��ü : 3, �� ��ü : 2
		Send(TempBuf, TempBuf.GetLength());

		CBufferEx TempBuf1;
		TempBuf1.Add(GUILD_INVITE_RESULT);
		TempBuf1.Add((BYTE)0x02);
		pUser->Send(TempBuf1, TempBuf1.GetLength());
		return;
	}

go_result:														 // �� �κ� ó���� ���߿� �ٽ� ����
	TempBuf.Add(GUILD_INVITE_RESULT);
	TempBuf.Add((BYTE)0x04);		//����
	TempBuf.Add(error_code);
	Send(TempBuf, TempBuf.GetLength());

	if(pUser && pUser->m_state == STATE_GAMESTARTED)
	{
		pUser->Send(TempBuf, TempBuf.GetLength());
	}
}

//-----------------------------------------------------------------------------------------
//--yskang 0.1 �������� ȣĪ�� �ο��Ѵ�.
//ȣĪ�� �ο��� �� �ִ� ���� ���¯�̴�.
//-----------------------------------------------------------------------------------------
void USER::LoveName(TCHAR *pBuf)
{
	//------------------------------------------------------------------------------------------------
	/* ���� �ڵ�
	01 : ������ ����.
	02 : ã�� ���̵� ����.
	03 : ���� ������ �ƴϴ�.
	04 : ȣĪ[��Ī]�� �߸��Ǿ���.
	05 : ��� ���� �� �� ������.
	06 : ���̴�.
	*/
	//-------------------------------------------------------------------------------------------------
	CBufferEx ResultTempBuf; 
	CBufferEx TempBuf;
	BOOL bSuccess = FALSE;
	BYTE byCommand=(BYTE)1;
	int nLength = 0;
	int uid = 0;
	int index = 0;//�迭 ������ ��ġ
	USER* pUser = NULL;
	BYTE error_code = ERR_1;
	byCommand = GetByte(pBuf,index);
	

	//ȣĪ�� �ο��ϰ��� �ϴ� ������ ã�´�.
	//--�ӽ÷�
	uid = GetInt(pBuf, index);
	
	if(uid < USER_BAND || uid >= NPC_BAND) 
		return;
	pUser = GetUser(uid - USER_BAND);		
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
	if(pUser->m_uid==m_uid)//�ڽſ��� ȣĪ�ο�
	{
		if(m_sLevel<70) { error_code = ERR_1; goto go_result; }//70���� ���϶� ȣĪ�� �ο��� �� ����.		
	}
	else //������� ȣĪ�ο�
	{
		if(m_sLevel<70) { error_code = ERR_1; goto go_result; } //70���� ���϶� ȣĪ�� �ο��� �� ����.
			
		//ȣĪ�� �ο��Ҽ��ִ� �ڰ��� ���߰� �ִ°�?
		if(!m_bGuildMaster) 
		{
			error_code = ERR_1; 
			goto go_result; 
		}	// ȣĪ �ο� �ڰ��� ����.

	}
	if(strcmp(pUser->m_strGuildName,m_strGuildName)!=0)//���� ������ �ƴ϶��....
	{ error_code = ERR_3; goto go_result; }

	//���� �������
	if(byCommand ==0)
	{
		//ȣĪ�� �޸� ��� �����Ѵ�(?)
		::ZeroMemory(pUser->m_strLoveName,sizeof(pUser->m_strLoveName));
		strcpy(pUser->m_strLoveName,"");
		lstrcpy(pUser->m_pMD->m_strLoveName , pUser->m_strLoveName);
		//ȣĪ�� �ٸ� ����鿡�� �˸���(���� ��ũ���� �ִ»���鿡 ���ؼ�?)
		TempBuf.Add(LOVE_NAME);
		TempBuf.Add(uid);
		TempBuf.AddString(pUser->m_strLoveName);
		pUser->SendExactScreen(TempBuf, TempBuf.GetLength());
		bSuccess = TRUE;
		goto go_result;
	}

	if(byCommand>1) return;

	//��Ŷ���� ȣĪ�� �̾Ƴ���.
	char strTemp[1024];
	nLength = GetVarString(sizeof(strTemp), strTemp, pBuf, index);
	if(nLength < 1 || nLength > CHAR_NAME_LENGTH) // �߸��� ȣĪ
	{
		//ZeroMemory(pUser->m_strLoveName,sizeof(pUser->m_strLoveName));
		error_code = ERR_4;  goto go_result; 
	}
	if(IsReservedID(strTemp)) { error_code = ERR_6; goto go_result; }//�尰������ �ִ� �� Ȯ��
		
	if(!UNI_CHAR::CheckString(strTemp)) {error_code=ERR_6; goto go_result;} //Ư������ üũ

	//--------------------------------------------------------------------------------------
	//--yskang 0.6 �������� �����̾� ȣĪ�� ����Ѵ�.
	if(pUser->m_iDisplayType != 5 && pUser->m_iDisplayType != 6)
		strcpy(pUser->m_strLoveName,strTemp);
	else {error_code = ERR_1; goto go_result; }
	//---------------------------------------------------------------------------------------

	
	if(pUser->m_bPseudoString==TRUE)
	{

		//------------------------------------------------------------------------------------
		//ó�� ȣĪ�� �ο��Ǵ� ���̶�� ��� Insert�Ѵ�.
		//------------------------------------------------------------------------------------
		index = 0;
		BYTE *pData;
		SQLDATAPACKET *pSDP;
		pSDP = new SQLDATAPACKET;
		pSDP->UID = m_uid;
		pSDP->code = LOVE_NAME_RESULT;
		int Datalength = pSDP->dcount = sizeof(uid);
		pData = new BYTE[Datalength+1];
		memset(pData, 0, Datalength+1);
		memcpy(pData, &uid,sizeof(uid));//��� ���̵� ����....
		pSDP->pData = pData;

		//------------------------------------------------------------------------------------
		//�����忡�� ó���Ѵ�.
		//------------------------------------------------------------------------------------
		EnterCriticalSection( &m_CS_SqlData );
		RecvSqlData.AddTail(pSDP);
		nSqlDataCount = RecvSqlData.GetCount();
		LeaveCriticalSection( &m_CS_SqlData );	

		//-------------------------------------------------------------------------------------
		//--��� Insert��
		//-------------------------------------------------------------------------------------
	}
	else
	{

		//ȣĪ�� �޸� ��� �����Ѵ�(?)
		strcpy(pUser->m_pMD->m_strLoveName , pUser->m_strLoveName);
		//ȣĪ�� �ٸ� ����鿡�� �˸���(���� ��ũ���� �ִ»���鿡 ���ؼ�?)
		TempBuf.Add(LOVE_NAME);
		TempBuf.Add(uid);
		TempBuf.AddString(pUser->m_strLoveName);
		pUser->SendExactScreen(TempBuf, TempBuf.GetLength());
		bSuccess = TRUE;
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
}


void USER::UpdateUserDeadLog(TCHAR *pBuf)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	int				index = 0;

	TCHAR			szSQL[1024];
	::ZeroMemory(szSQL, sizeof(szSQL));

	//yskang 0.8 add dead log
	TCHAR			strPKName[100];
	::ZeroMemory(strPKName, sizeof(strPKName));

	DWORD minus = GetDWORD(pBuf, index);
	DWORD preExp = GetDWORD(pBuf, index);
	BYTE nLen = GetByte(pBuf,index);//yskang 0.8 add dead log 
	GetString((char *)strPKName,pBuf,nLen,index);//yskang 0.8  add dead log

	CPoint pt = ConvertToClient(m_curx, m_cury);
																	// id, level,z, x, y, iExp, minus exp, preExp, iCityValue					
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_USER_DEAD_DATA (\'%s\',%d,%d,%d,%d,%d,%d,%d,%d,\'%s\',%d)}"), m_strUserID,m_sLevel,m_curz,pt.x,pt.y,m_dwExp,minus,preExp,m_iCityValue,strPKName,m_iDisplayType);//yskang 0.8 dead log ���� ���λ�� �̸�, ���� ���� Ÿ�� �߰� 

	int db_index = 0;
	CDatabase* pDB = g_DBNew[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if( retcode != SQL_SUCCESS )
	{
		TRACE("Fail To Update Guild Warehouse Data !!\n");

		//g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}
	
	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);		
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
			SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

			g_DBNew[m_iModSid].ReleaseDB(db_index);
			return;
		}
	}	
	else
	{
		DisplayErrorMsg( hstmt );
		SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DBNew[m_iModSid].ReleaseDB(db_index);
		return;
	}

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DBNew[m_iModSid].ReleaseDB(db_index);
}

void USER::UpdateUserDeadLog(DWORD minus, DWORD preExp, TCHAR *strPKName)
{
	int index = 0;
	TCHAR pBuf[100];

	::ZeroMemory(pBuf, sizeof(pBuf));

	SetDWORD(pBuf, minus, index);
	SetDWORD(pBuf, preExp, index);
	int nLen = strlen(strPKName);
		if(nLen < 1) nLen =1;
	SetByte(pBuf, (BYTE)nLen, index);
	SetString(pBuf,strPKName,nLen,index);
	
	int Datalength;
	BYTE *pData;
	SQLDATAPACKET *pSDP;
	pSDP = new SQLDATAPACKET;
	pSDP->code = DB_USER_DEAD;
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

////////////////////////////////////////////////////////////////////////////////////////
//	tMyClass Ŭ������ tNeedClass �� �� �� �ִ��� �˻�.
//
BOOL USER::CheckClassItem(BYTE tMyClass, BYTE tNeedClass)
{
	BYTE tTemp = 1;	
	BYTE tFire = 0;
	BYTE tEdge = 0;
	BYTE tStaff = 0;
	BYTE tBrawl = 0;

	tFire	 = tTemp & tNeedClass; tTemp = 2; 
	tEdge	 = tTemp & tNeedClass; tTemp = 4;
	tStaff	 = tTemp & tNeedClass; tTemp = 8;
	tBrawl	 = tTemp & tNeedClass;

	tFire = tFire & tMyClass;
	tEdge = tEdge & tMyClass;
	tStaff = tStaff & tMyClass;
	tBrawl = tBrawl & tMyClass;

	tTemp = tFire^tEdge^tStaff^tBrawl;

	if(!tTemp) return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//	Moon Event Item Use...
//
BOOL USER::MoonEvent(short sSlot)
{
	CString strMsg = _T("");
	short sSid = 0;
	int iRandom = 0, iSummon = 0;
	ItemList Item;
	int iItemNum = 0;
	int iEmptySlot = -1, iSameSlot = -1;
	int i = 0;
	int iDeleteHP = 0;
	BYTE tSlot = 0;
	short sCount = 0;
	CPoint pt;
	BOOL bRet = FALSE;

	if(g_iMoonEvent == 0)
	{
		strMsg.Format(IDS_EVENT_END);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		return FALSE;
	}

	if(sSlot < EQUIP_ITEM_NUM || sSlot >= TOTAL_INVEN_MAX) return FALSE;
	
	sSid = m_UserItem[sSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return FALSE;
	if(m_UserItem[sSlot].sCount <= 0) return FALSE;

	switch(sSid)
	{
	case EVENTITEM_SID_MOON:
		if(m_dwAdamantineTime != 0 || m_dwMightyWeaponTime != 0 )
		{
			strMsg.Format(IDS_ANOTHER_PSI_RUN);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			return FALSE;
		}

		SetBerserker(60 * 5);
		SendPsiAttackResult(SUCCESS, m_uid + USER_BAND, 15);
		break;

/*	case EVENTITEM_SID_SONGPEON_01:
		GetExp(87);
		strMsg.Format(IDS_MOON_SONGPEON_01);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		break;
	case EVENTITEM_SID_SONGPEON_11:
		GetExp(654);
		strMsg.Format(IDS_MOON_SONGPEON_11);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		break;
	case EVENTITEM_SID_SONGPEON_31:
		GetExp(11185);
		strMsg.Format(IDS_MOON_SONGPEON_31);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		break;
	case EVENTITEM_SID_SONGPEON_51:
		GetExp(67550);
		strMsg.Format(IDS_MOON_SONGPEON_51);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		break;
	case EVENTITEM_SID_SONGPEON_71:
		GetExp(249960);
		strMsg.Format(IDS_MOON_SONGPEON_71);
		SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
		break;
*/
	case EVENTITEM_SID_BOX: case 868:
		if(m_curz >= 56 && m_curz <= 59 || m_curz == 1005)
		{
			strMsg.Format(IDS_CANNOT_USE_BOX);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
			return FALSE;
		}

		iRandom = myrand(0, 999);
		if(g_arBoxEventTable[iRandom]->m_tType == 1)	// Summoning Monster
		{
			iSummon = myrand(0, g_arSummonTable.GetSize() - 1);
			
			g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_bMove = 1;
			pt = FindNearAvailablePoint_S(m_curx, m_cury);
			g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_bMove = 0;

			if(pt.x == -1 || pt.y == -1) return FALSE;

			bRet = SummonMonster(g_arSummonTable[iSummon]->m_strName, pt);
			return bRet;
		}
		else //if(g_arBoxEventTable[iRandom]->m_tType >= 2 && g_arBoxEventTable[iRandom]->m_tType <= 37)	// Give Item
		{
			iItemNum = g_arBoxEventTable[iRandom]->m_sIid;

			Item.sSid		= g_arItemTable[iItemNum]->m_sSid;
			Item.sLevel		= g_arItemTable[iItemNum]->m_byRLevel;
			Item.sDuration	= g_arItemTable[iItemNum]->m_sDuration;
			Item.sCount		= g_arBoxEventTable[iRandom]->m_sCount;
			Item.sBullNum	= g_arItemTable[iItemNum]->m_sBullNum;
			Item.tIQ		= NORMAL_ITEM;
			Item.iItemSerial= 0;
			for(i = 0; i < MAGIC_NUM; i++) Item.tMagic[i] = 0;

			iSameSlot	=	GetSameItem(Item, INVENTORY_SLOT); 
			iEmptySlot	=	GetEmptySlot(INVENTORY_SLOT);
			
			if(iEmptySlot == -1)	// �κ��� �ּ��� �ϳ��� ������� �־�� �����ϰ� ������ �� �� �ִ�.
			{
				strMsg.Format(IDS_INVENTORY_FULL);
				SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);
				return FALSE;
			}

			if(Item.sCount == 1 && Item.sDuration > 0 ) MakeItemLog(&Item, ITEMLOG_EVENT_GIVE);

			for(i = 0; i < Item.sCount; i++) m_iCurWeight += g_arItemTable[iItemNum]->m_byWeight;
			GetRecoverySpeed();

			if(iSameSlot != -1) 
			{
				sCount = m_UserItem[iSameSlot].sCount;
				m_UserItem[iSameSlot] = Item;
				m_UserItem[iSameSlot].sCount += sCount;
				tSlot = (BYTE)iSameSlot;
			}
			else if(iEmptySlot != -1) 
			{
				m_UserItem[iEmptySlot] = Item;
				tSlot = (BYTE)iEmptySlot;
			}

			SendSystemMsg((LPTSTR)(LPCTSTR)g_arBoxEventTable[iRandom]->m_strText, SYSTEM_NORMAL, TO_ME);
			
			CBufferEx TempBuf;

			TempBuf.Add(ITEM_MOVE_RESULT);
			TempBuf.Add(SUCCESS);
			TempBuf.Add((BYTE)0);		// Type
			TempBuf.Add((BYTE)1);		// Count
			
			TempBuf.Add(tSlot);
			TempBuf.Add(m_UserItem[tSlot].sLevel);
			TempBuf.Add(m_UserItem[tSlot].sSid);
			TempBuf.Add(m_UserItem[tSlot].sDuration);
			TempBuf.Add(m_UserItem[tSlot].sBullNum);
			TempBuf.Add(m_UserItem[tSlot].sCount);
			for(i =0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tSlot].tMagic[i]);
			
			TempBuf.Add(m_UserItem[tSlot].tIQ); 
			
			Send(TempBuf, TempBuf.GetLength());
		}
		/*else if(g_arBoxEventTable[iRandom]->m_tType == 38 || g_arBoxEventTable[iRandom]->m_tType == 39)	// Minus HP 40% or 30%
		{
			if(g_arBoxEventTable[iRandom]->m_tType == 38)	iDeleteHP = (int)((double)m_sHP * 0.4);
			else if(g_arBoxEventTable[iRandom]->m_tType == 39)	iDeleteHP = (int)((double)m_sHP * 0.3);

			if(iDeleteHP > 0)
			{
				m_sHP -= iDeleteHP;
				if(m_sHP < 0) m_sHP = 0;
				SendSystemMsg((LPTSTR)(LPCTSTR)g_arBoxEventTable[iRandom]->m_strText, SYSTEM_NORMAL, TO_ME);
				SendHP();
			}			
		}*/

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
//	Summoning Monster
//
BOOL USER::SummonMonster(CString strName, CPoint pt)
{
	CNpc* pNpc = NULL;
	int i;
	CString strMsg = _T("");

	MYSHORT sAI;
	BYTE upTemp = 0;			// ���� 8��Ʈ
	BYTE dwTemp = 0;			// ���� 8��Ʈ

//	int iDeleteIndex = 0;
//	TCHAR DeleteSend[2048];	

//	int iModifyIndex = 0;
//	TCHAR ModifySend[2048];	
    if (strName=="����") strName="����";
	int NpcState = NPC_DEAD;

	for(i = 0; i < g_arNpc.GetSize(); i++)
	{
		pNpc = g_arNpc[i];
		if( !pNpc ) continue;

		if(strName.CompareNoCase(pNpc->m_strName) == 0)
		{
			if(pNpc->m_sCurZ == 1005) continue;
			if(pNpc->m_NpcState != NPC_DEAD && pNpc->m_NpcState != NPC_STANDING) continue;
			if(pNpc->m_bSummon == TRUE) continue;

			if(::InterlockedExchange(&pNpc->m_lNowSummoning, 1) != 0) continue;

			NpcState = pNpc->m_NpcState;
			pNpc->m_Delay = 2000;

			if(pNpc->m_NpcState == NPC_STANDING)
			{
				pNpc->SetUidNPC(pNpc->m_sCurX, pNpc->m_sCurY, 0);

				pNpc->m_presx = pNpc->m_presy = -1;
				pNpc->SendNpcInfoBySummon(m_pCom);
			}

			pNpc->m_SummonZoneIndex = pNpc->m_ZoneIndex;
			pNpc->m_sSummonOrgZ = pNpc->m_sOrgZ;
			pNpc->m_sSummonOrgX = pNpc->m_sOrgX;
			pNpc->m_sSummonOrgY = pNpc->m_sOrgY;

			pNpc->m_sCurZ = pNpc->m_sOrgZ = m_curz;
			pNpc->m_sCurX = pNpc->m_sOrgX = pt.x;
			pNpc->m_sCurY = pNpc->m_sOrgY = pt.y;

			pNpc->m_ZoneIndex		= -1;

			pNpc->m_dwStepDelay	= GetTickCount();
			
			sAI.i = (short)pNpc->m_sAI;						// NPC AI�� ����
			upTemp = sAI.b[0];
			dwTemp = sAI.b[1];
			
			pNpc->m_tNpcAttType = upTemp >> 7;				// ���߿� �߰��ؾ��Ѵ�.
			upTemp = upTemp << 1;
			pNpc->m_tNpcLongType = upTemp >> 7;
			upTemp = upTemp << 1;
			pNpc->m_tNpcGroupType = upTemp >> 7;
			
			if(pNpc->m_sClientSpeed <= 20) pNpc->m_sClientSpeed = 20;	// ��� �ڵ�;
			
			for(i = 0; i < g_zone.GetSize(); i++)
			{
				if(g_zone[i]->m_Zone == pNpc->m_sCurZ) 
				{
					pNpc->m_ZoneIndex = i;
					break;
				}
			}

			pNpc->m_bSummon = TRUE;
			pNpc->m_bSummonDead = TRUE;
			pNpc->EventNpcInit(pt.x, pt.y);

			if(NpcState == NPC_STANDING) 
			{
				pNpc->SetUidNPC(pt.x, pt.y, pNpc->m_sNid + NPC_BAND);

				pNpc->m_presx = pNpc->m_presy = -1;
				pNpc->SightRecalc(m_pCom);
			}

			if(::InterlockedExchange(&pNpc->m_lNowSummoning, 0) != 1) pNpc->m_lNowSummoning = 0;

			strMsg.Format( IDS_USER_CALL_WHAT, pNpc->m_strName);
			SendSystemMsg((LPTSTR)(LPCTSTR)strMsg, SYSTEM_NORMAL, TO_ME);

			return TRUE;
		}
	}

	return FALSE;
}

int USER::GetItemIndex(int sid)
{
	int i = 0;
	int nCount = g_arItemTable.GetSize();

	for(i = 0; i < nCount; i++)
	{
		if(sid == g_arItemTable[i]->m_sSid) return i;
	}

	return -1;
}

void USER::SoftClose()
{
	CBSocket::B_SoftClose();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Ư���� �Ҹ� ������ ����Ѵ�.
//
BYTE USER::UseSpecialPotion(short sSlot)
{
	CBufferEx TempBuf;
	short sSid = 0;
	BYTE tRet = 0;
	int j = 0;

	if(sSlot < EQUIP_ITEM_NUM || sSlot >= TOTAL_INVEN_MAX) return FALSE;
	
	sSid = m_UserItem[sSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return FALSE;
	if(m_UserItem[sSlot].sCount <= 0) return FALSE;

	switch(sSid)
	{
	case SPECIAL_ITEM_HIEXP	:
		if(m_dwHiExpTime !=0){
			SendSystemMsg("�������Ѿ����˷�״̬,�޷���ʹ���˷ܼ�.", SYSTEM_NORMAL, TO_ME);
			return false;
		}
		if(m_iDisplayType == 5 || m_iDisplayType ==6) //���� ������ΰ�?
		{
			SendSystemMsg("���� ����ڴ� ���̸� ����� �� �����ϴ�.", SYSTEM_NORMAL, TO_ME);
			return tRet; //or m_dwHiExpTime = 0; //���� ����ڴ� ����� �� ����.
		}
		else m_dwHiExpTime = HIEXP_TIME;
		//----------------------------------------------------------------------------------------
		m_dwLastHiExpTime = GetTickCount();
		tRet = EXP_POTION;

		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		AddAbnormalInfo(ABNORMAL_HIEXP);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);	
		Send(TempBuf, TempBuf.GetLength());

		break;

	case SPECIAL_ITEM_MAGICFIND :
		if(m_dwMagicFindTime !=0){
			SendSystemMsg("�������Ѿ�������״̬,�޷���ʹ�����˼�.", SYSTEM_NORMAL, TO_ME);
			return false;
		}
		if(m_iDisplayType == 5 || m_iDisplayType ==6) //���� ������ΰ�?
		{
			SendSystemMsg("���� ����ڴ� MagicFind�� ����Ҽ� �����ϴ�.", SYSTEM_NORMAL, TO_ME);
			return tRet; //or m_dwMagicFindTime = 0; //���� ����ڴ� ����� �� ����.
		}
		else m_dwMagicFindTime = MAGICFIND_TIME;
		//-----------------------------------------------------------------------------------------
		m_dwLastMagicFindTime = GetTickCount();
		tRet = MAGIC_POTION;

		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		AddAbnormalInfo(ABNORMAL_MAGICFIND);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);
		Send(TempBuf, TempBuf.GetLength());

		break;

	case SPECIAL_ITEM_KILLREMOVE :	// kill count�� �ϳ� �ٿ��ش�.
		m_iCityValue=m_iCityValue+1000;
		GetCityRank();
		if(m_sKillCount > 0){
			m_sKillCount -= 1;
			if( m_sKillCount < 0 ) m_sKillCount = 0;
		}
		tRet = KILLREMOVE_POTION;
		break;
	case EVENTITEM_SID_MOON: //��ͷ
		if(m_dwAdamantineTime != 0 || m_dwMightyWeaponTime != 0 )
		{
			return FALSE;
		}
		SetBerserker(60 * 5);
		//SendPsiAttackResult(SUCCESS, m_uid + USER_BAND, 15);
		tRet = 19;	
		break;
	case 1108: // super posion
        m_sCityRank=5;
        m_sKillCount=0;
        m_iCityValue=1056989;
		tRet = KILLREMOVE_POTION;

		TempBuf.Add(SET_USER_PK_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		TempBuf.Add((BYTE)0x01);
		TempBuf.Add(m_sCityRank);
		SendInsight(TempBuf, TempBuf.GetLength());
		break;
	case 1109: // add XP posion
        m_dwXP += 10000;
		SendXP();
		tRet = KILLREMOVE_POTION;	
		break;
	case 1132:
		ZoneMoveReq(409,97,55);
		tRet = 17;
		break;
	case 1007:
		ZoneMoveReq(416,86,106);
		tRet = 17;
		break;
	case 1100:
		if (m_curz == 59) 	tRet = 19;
		break;
	case 1017:
		if( FindItem( 854) >= 1 )
		{
			RobItem( 854, 1 );
			GiveItem( 862, 2 );
		//	ZoneMoveReq(409,97,55);
			tRet = 17;
		}
		break;
/*	case 862:
		GiveItem( 1118, 1 );
		ZoneMoveReq(409,97,55);
		tRet = 17;
		break;
		*/
	case 1028: //����
	//CBufferEx TempBuf;
		if(m_iSkin==2){
			SendSystemMsg("����������ʹ���ܱ书��.", SYSTEM_NORMAL, TO_ME);
			return false;
		}

	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	//AddAbnormalInfo(ABNORMAL_BERSERKER);
	TempBuf.Add(m_dwAbnormalInfo);
	//m_dwAbnormalInfo_=m_dwAbnormalInfo_|2;
	m_dwBSTime=3600*1000;			//���ܱ��ʱ��
	//m_dwBSTime=360000;
	m_dwBSLaseTime=GetTickCount();// ���ܱ�Ŀ�ʼʱ��
	m_iSkin=1;
	TempBuf.Add(m_dwAbnormalInfo_);
	//TempBuf.Add((BYTE)2);
	//TempBuf.Add((BYTE)0);
	//TempBuf.Add((BYTE)0);
	//TempBuf.Add((BYTE)0);
	SendInsight( TempBuf, TempBuf.GetLength() );
	CheckMagicItemMove();
	tRet = 101;
		break;
	case 1029://����
	//CBufferEx TempBuf;
	if(m_iSkin==1){
		SendSystemMsg("����������ʹ���Ǳ书��.", SYSTEM_NORMAL, TO_ME);
		return false;
	}

	TempBuf.Add( SET_USER_STATE );
	TempBuf.Add( m_uid + USER_BAND );
	//AddAbnormalInfo(ABNORMAL_BERSERKER);
	TempBuf.Add(m_dwAbnormalInfo);
	//m_dwAbnormalInfo_=m_dwAbnormalInfo_|4;
	m_dwBSTime=3600*1000;			//���ܱ��ʱ��
	//m_dwBSTime=360000;
	m_dwBSLaseTime=GetTickCount();// ���ܱ�Ŀ�ʼʱ��
	m_iSkin=2;
	TempBuf.Add(m_dwAbnormalInfo_);
	//TempBuf.Add((BYTE)4);
	//TempBuf.Add((BYTE)0);
	//TempBuf.Add((BYTE)0);
	//TempBuf.Add((BYTE)0);
	SendInsight( TempBuf, TempBuf.GetLength() );
	CheckMagicItemMove();
	tRet = 102;
	break;
	/*case 1015:
		if (m_sLevel >=105  && m_byClass == 1 && m_dwXP >=30000) 
		{

		for(j = 0; j < m_nHavePsiNum; j++)
		{
			if(m_UserPsi[j].sSid == 30) 
			{
				SendSystemMsg(IDS_PSI_ERROR_EXIST, SYSTEM_ERROR, TO_ME);
				return FALSE;
			}
		}

		m_UserPsi[m_nHavePsiNum].sSid = 30;
		m_UserPsi[m_nHavePsiNum].tOnOff = FALSE;
		m_nHavePsiNum++;
		m_dwXP -= 30000;

    	TempBuf.Add(BUY_PSY_RESULT);
	    TempBuf.Add((BYTE)1);
    	TempBuf.Add((short)1);
    	TempBuf.Add(30);
    	Send(TempBuf, TempBuf.GetLength());
        SendXP();

        tRet = 15;
		}
     	break;
		*/
	}


	return tRet;
}

/////////////////////////////////////////////////////////////////////////////////
//	�Ҹ� �Ǽ��縮�� �������� ������. �پ��� ������.
//
void USER::SendAccessoriDuration(short sSid)
{
	BYTE tSlot = 0;
	int i;
	CBufferEx TempBuf;
	
	for(i = 0; i < MAX_ACCESSORI; i++)
	{
		tSlot = g_iAccessoriSlot[i];
		if(m_UserItem[tSlot].sSid == sSid)
		{
			m_UserItem[tSlot].sDuration--;
			if(m_UserItem[tSlot].sDuration <= 0) 
			{
				MakeItemLog( &m_UserItem[tSlot], ITEMLOG_ACC_USE );
				FlushItemLog( TRUE );
				
				ReSetItemSlot(&m_UserItem[tSlot]);
				
				TempBuf.Add(ITEM_MOVE_RESULT);
				TempBuf.Add(SUCCESS);
				TempBuf.Add((BYTE)0);		// Type
				TempBuf.Add((BYTE)1);		// Count
				
				TempBuf.Add(tSlot);
				TempBuf.Add(m_UserItem[tSlot].sLevel);
				TempBuf.Add(m_UserItem[tSlot].sSid);
				TempBuf.Add(m_UserItem[tSlot].sDuration);
				TempBuf.Add(m_UserItem[tSlot].sBullNum);
				TempBuf.Add(m_UserItem[tSlot].sCount);
				for(i =0; i < MAGIC_NUM; i++) TempBuf.Add(m_UserItem[tSlot].tMagic[i]);
				TempBuf.Add(m_UserItem[tSlot].tIQ); 
				
				Send(TempBuf, TempBuf.GetLength());				
			}
			else
			{
				TempBuf.Add(ITEM_DURATION);
				TempBuf.Add(tSlot);
				TempBuf.Add(m_UserItem[tSlot].sDuration);
				Send(TempBuf, TempBuf.GetLength());
			}

			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	User �� �̸��� �̹� �����ϴ��� üũ�Ѵ�.
//
void USER::CheckIDReq(TCHAR* pBuf)
{
	TCHAR szName[CHAR_NAME_LENGTH + 1];
	int nLen = 0;
	BYTE tResult = FAIL;
	int index = 0;
	CBufferEx TempBuf;

	::ZeroMemory(szName, sizeof(szName));
	nLen = GetVarString(sizeof(szName), szName, pBuf, index);
	if(nLen <= 0 || nLen > CHAR_NAME_LENGTH - 3) goto go_result;

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
		return;
	}

	if(!IsExistCharId(szName)) tResult = SUCCESS;

go_result:

	TempBuf.Add(CHECK_ID_RESULT);
	TempBuf.Add(tResult);

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////////////////
//	���̿��� ���� ������ ����Ͽ� ���̿����� �����Ѵ�.
//
BOOL USER::UsePsiStone(short sSlot)
{
	CBufferEx TempBuf;
	short sSid = 0;
	short sPsionicSid = 0;
	int j;

	if(sSlot < EQUIP_ITEM_NUM || sSlot >= TOTAL_INVEN_MAX) return FALSE;
	
	sSid = m_UserItem[sSlot].sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return FALSE;
	if(m_UserItem[sSlot].sCount <= 0) return FALSE;
	
	switch(sSid)
	{
	case PSI_STONE_FAST_RUN	:
		if(m_byClass != BRAWL)
		{
			SendSystemMsg(IDS_PSI_ERROR_CLASS, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(m_sLevel < 80) 
		{
			SendSystemMsg(IDS_PSI_ERROR_LEVEL, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
// 		if(FindEvent(6) == FALSE) 
// 		{
// 			SendSystemMsg(IDS_NEED_QUEST, SYSTEM_ERROR, TO_ME);
// 			return FALSE;
// 		}
		if(g_arPsiTable[PSIONIC_FAST_RUN]->m_iNeedXP > m_dwXP)
		{
			SendSystemMsg(IDS_PSI_ERROR_XP, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		for(j = 0; j < m_nHavePsiNum; j++)
		{
			if(m_UserPsi[j].sSid == PSIONIC_FAST_RUN) 
			{
				SendSystemMsg(IDS_PSI_ERROR_EXIST, SYSTEM_ERROR, TO_ME);
				return FALSE;
			}
		}

		sPsionicSid = m_UserPsi[m_nHavePsiNum].sSid = PSIONIC_FAST_RUN;
		m_UserPsi[m_nHavePsiNum].tOnOff = FALSE;
		m_nHavePsiNum++;
		m_dwXP -= g_arPsiTable[PSIONIC_FAST_RUN]->m_iNeedXP;
		break;

	case PSI_STONE_MIND_SHOCK :					
		if(m_byClass != STAFF)
		{
			SendSystemMsg(IDS_PSI_ERROR_CLASS, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(m_sLevel < 80) 
		{
			SendSystemMsg(IDS_PSI_ERROR_LEVEL, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(FindEvent(6) == FALSE) 
		{
			SendSystemMsg(IDS_NEED_QUEST, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}

		if(g_arPsiTable[PSIONIC_MIND_SHOCK]->m_iNeedXP > m_dwXP)
		{
			SendSystemMsg(IDS_PSI_ERROR_XP, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}

		for(j = 0; j < m_nHavePsiNum; j++)
		{
			if(m_UserPsi[j].sSid == PSIONIC_MIND_SHOCK) 
			{
				SendSystemMsg(IDS_PSI_ERROR_EXIST, SYSTEM_ERROR, TO_ME);
				return FALSE;
			}
		}

		sPsionicSid = m_UserPsi[m_nHavePsiNum].sSid = PSIONIC_MIND_SHOCK;
		m_UserPsi[m_nHavePsiNum].tOnOff = FALSE;
		m_nHavePsiNum++;
		m_dwXP -= g_arPsiTable[PSIONIC_MIND_SHOCK]->m_iNeedXP;
		break;

	case PSI_STONE_MIND_GUARD :					
		if(m_byClass != STAFF)
		{
			SendSystemMsg(IDS_PSI_ERROR_CLASS, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(m_sLevel < 90) 
		{
			SendSystemMsg(IDS_PSI_ERROR_LEVEL, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(FindEvent(6) == FALSE) 
		{
			SendSystemMsg(IDS_NEED_QUEST, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(g_arPsiTable[PSIONIC_MIND_GUARD]->m_iNeedXP > m_dwXP)
		{
			SendSystemMsg(IDS_PSI_ERROR_XP, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}

		for(j = 0; j < m_nHavePsiNum; j++)
		{
			if(m_UserPsi[j].sSid == PSIONIC_MIND_GUARD) 
			{
				SendSystemMsg(IDS_PSI_ERROR_EXIST, SYSTEM_ERROR, TO_ME);
				return FALSE;
			}
		}

		sPsionicSid = m_UserPsi[m_nHavePsiNum].sSid = PSIONIC_MIND_GUARD;
		m_UserPsi[m_nHavePsiNum].tOnOff = FALSE;
		m_nHavePsiNum++;
		m_dwXP -= g_arPsiTable[PSIONIC_MIND_GUARD]->m_iNeedXP;
		break;

	case PSI_STONE_PSI_SHIELD :					
		if(m_byClass != EDGED)
		{
			SendSystemMsg(IDS_PSI_ERROR_CLASS, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(m_sLevel < 80) 
		{
			SendSystemMsg(IDS_PSI_ERROR_LEVEL, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(FindEvent(6) == FALSE) 
		{
			SendSystemMsg(IDS_NEED_QUEST, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}	

		if(g_arPsiTable[PSIONIC_PSI_SHIELD]->m_iNeedXP > m_dwXP)
		{
			SendSystemMsg(IDS_PSI_ERROR_XP, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}

		for(j = 0; j < m_nHavePsiNum; j++)
		{
			if(m_UserPsi[j].sSid == PSIONIC_PSI_SHIELD) 
			{
				SendSystemMsg(IDS_PSI_ERROR_EXIST, SYSTEM_ERROR, TO_ME);
				return FALSE;
			}
		}

		sPsionicSid = m_UserPsi[m_nHavePsiNum].sSid = PSIONIC_PSI_SHIELD;
		m_UserPsi[m_nHavePsiNum].tOnOff = FALSE;
		m_nHavePsiNum++;
		m_dwXP -= g_arPsiTable[PSIONIC_PSI_SHIELD]->m_iNeedXP;
		break;

	case PSI_STONE_PIERCING_SHIELD :			
		if(m_byClass != FIREARMS)
		{
			SendSystemMsg(IDS_PSI_ERROR_CLASS, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(m_sLevel < 80) 
		{
			SendSystemMsg(IDS_PSI_ERROR_LEVEL, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}
		if(FindEvent(6) == FALSE) 
		{
			SendSystemMsg(IDS_NEED_QUEST, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}

		if(g_arPsiTable[PSIONIC_PIERCING_SHIELD]->m_iNeedXP > m_dwXP)
		{
			SendSystemMsg(IDS_PSI_ERROR_XP, SYSTEM_ERROR, TO_ME);
			return FALSE;
		}

		for(j = 0; j < m_nHavePsiNum; j++)
		{
			if(m_UserPsi[j].sSid == PSIONIC_PIERCING_SHIELD) 
			{
				SendSystemMsg(IDS_PSI_ERROR_EXIST, SYSTEM_ERROR, TO_ME);
				return FALSE;
			}
		}

		sPsionicSid = m_UserPsi[m_nHavePsiNum].sSid = PSIONIC_PIERCING_SHIELD;
		m_UserPsi[m_nHavePsiNum].tOnOff = FALSE;
		m_nHavePsiNum++;
		m_dwXP -= g_arPsiTable[PSIONIC_PIERCING_SHIELD]->m_iNeedXP;
		break;
	}

	TempBuf.Add(BUY_PSY_RESULT);
	TempBuf.Add((BYTE)1);
	TempBuf.Add((short)1);
	TempBuf.Add(sPsionicSid);
	Send(TempBuf, TempBuf.GetLength());

	SendXP();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	Quest Window �� ����(������ ������ �ִ� ����Ʈ ������ �����ش�.)
//
void USER::QuestWindowOpenReq(TCHAR *pBuf)
{
	CBufferEx TempBuf;
	int iCount = m_arEventNum.GetSize();

	if(iCount >= MAX_EVENT_NUM) iCount = MAX_EVENT_NUM;

	TempBuf.Add(QUESTWINDOW_OPEN_RESULT);

	TempBuf.Add((short)iCount);
	for( int i = 0; i < iCount; i++)
	{
		TempBuf.Add((short)(*m_arEventNum[i]));
	}

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	Quest say�� ������
//
void USER::SendMonsterSay(CNpc *pNpc, int iType, char *strSay)
{
	if( !pNpc ) return;
	if( strlen( strSay ) >= 128 ) return;
	
	CBufferEx TempBuf;

	TempBuf.Add(CHAT_RESULT);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(NORMAL_CHAT);
	TempBuf.Add(pNpc->m_sNid + NPC_BAND);
	TempBuf.AddString(pNpc->m_strName);

	TempBuf.AddString((LPTSTR)(LPCTSTR)strSay);

	switch(iType)
	{
	case 0:
		Send(TempBuf, TempBuf.GetLength());
		break;
	case 1:
		SendExactScreen(TempBuf, TempBuf.GetLength());
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////
//	Quest �� �����Ѵ�. (�̺�Ʈ �����ð�, �¸�����, �� SID, ������, ��, ...)
//
BOOL USER::ExcuteSingleQuestEvent(int time, int type, int sid, int count, int z, int x, int y)
{
	int iEvent = g_QuestEventZone.GetEmptyEventZone();

	if(iEvent < 0)
	{
		SendNpcSay( NULL, 513 );										// ���� say
		return FALSE;													
	}

	g_QuestEventZone.m_SingleEventZone[iEvent].m_sEventZone = z;
	g_QuestEventZone.m_SingleEventZone[iEvent].m_tEventTime = time;		// �̺�Ʈ ����ð�
	g_QuestEventZone.m_SingleEventZone[iEvent].m_tSuccessType = type;	// �¸�����

	g_QuestEventZone.m_SingleEventZone[iEvent].m_arUserList[0].uid = m_uid; // ���� ���� 

	int nLen = strlen(m_strUserID);
	::CopyMemory(g_QuestEventZone.m_SingleEventZone[iEvent].m_arUserList[0].strUserName, m_strUserID, nLen);

	m_tQuestWar = GUILD_WARRING;
	 
	int iNid = -1;
	for(int i = 0; i < count; i++)
	{
		iNid = -1;
		iNid = SummonQuestMonster(sid, z, x, y);									// �� ��ȯ
		if(iNid > -1) g_QuestEventZone.m_SingleEventZone[iEvent].m_arNpcList.Add(iNid);	// �ش� ��
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//	Quest ���� Ư�� ��ǥ�� ��ȯ�Ѵ�.
//
int USER::SummonQuestMonster(int sid, int z, int x, int y)
{
	CNpc* pNpc = NULL;
	int i;

	MYSHORT sAI;
	BYTE upTemp = 0;			// ���� 8��Ʈ
	BYTE dwTemp = 0;			// ���� 8��Ʈ

	CPoint pt;

	int NpcState = NPC_DEAD;

	for(i = 0; i < g_arNpc.GetSize(); i++)
	{
		pNpc = g_arNpc[i];
		if( !pNpc ) continue;
		if(pNpc->m_tNpcType != NPCTYPE_MONSTER) continue;

		if(pNpc->m_sSid == sid)
		{
			if(pNpc->m_sCurZ == 1005) continue;
			if(pNpc->m_NpcState != NPC_DEAD && pNpc->m_NpcState != NPC_STANDING) continue;
			if(pNpc->m_bSummon == TRUE) continue;

			if(::InterlockedExchange(&pNpc->m_lNowSummoning, 1) != 0) continue;

			NpcState = pNpc->m_NpcState;
			pNpc->m_Delay = 2000;

			if(pNpc->m_NpcState == NPC_STANDING)
			{
				pNpc->SetUidNPC(pNpc->m_sCurX, pNpc->m_sCurY, 0);

				pNpc->m_presx = pNpc->m_presy = -1;
				pNpc->SendNpcInfoBySummon(m_pCom);
			}

			pNpc->m_SummonZoneIndex = pNpc->m_ZoneIndex;
			pNpc->m_sSummonOrgZ = pNpc->m_sOrgZ;
			pNpc->m_sSummonOrgX = pNpc->m_sOrgX;
			pNpc->m_sSummonOrgY = pNpc->m_sOrgY;

			pNpc->m_sCurZ = pNpc->m_sOrgZ = z;
			pNpc->m_sCurX = pNpc->m_sOrgX = x;
			pNpc->m_sCurY = pNpc->m_sOrgY = y;

			pNpc->m_ZoneIndex		= -1;

			pNpc->m_dwStepDelay	= GetTickCount();
			
			sAI.i = (short)pNpc->m_sAI;						// NPC AI�� ����
			upTemp = sAI.b[0];
			dwTemp = sAI.b[1];
			
			pNpc->m_tNpcAttType = upTemp >> 7;				// ���߿� �߰��ؾ��Ѵ�.
			upTemp = upTemp << 1;
			pNpc->m_tNpcLongType = upTemp >> 7;
			upTemp = upTemp << 1;
			pNpc->m_tNpcGroupType = upTemp >> 7;
			
			if(pNpc->m_sClientSpeed <= 20) pNpc->m_sClientSpeed = 20;	// ��� �ڵ�;
			
			for(i = 0; i < g_zone.GetSize(); i++)
			{
				if(g_zone[i]->m_Zone == pNpc->m_sCurZ) 
				{
					pNpc->m_ZoneIndex = i;
					break;
				}
			}

			pNpc->m_bSummon = TRUE;
			pNpc->m_bSummonDead = TRUE;
			pNpc->EventNpcInit(x, y);

			pt = pNpc->FindNearRandomPoint(x, y);
			if(pt.x < 0 || pt.y < 0)
			{
				pNpc->m_bFirstLive = FALSE;
				pNpc->m_NpcState = NPC_DEAD;
				pNpc->m_sHP = 0;
				pNpc->m_Delay = pNpc->m_sRegenTime;

				pNpc->m_bSummon = FALSE;
				pNpc->m_bSummonDead = TRUE;

				if(::InterlockedExchange(&pNpc->m_lNowSummoning, 0) != 1) pNpc->m_lNowSummoning = 0;
				return -1;
			}

			if(NpcState == NPC_STANDING) 
			{
				pNpc->m_sCurX = pNpc->m_sOrgX = pt.x;
				pNpc->m_sCurY = pNpc->m_sOrgY = pt.y;

				pNpc->SetUidNPC(pt.x, pt.y, pNpc->m_sNid + NPC_BAND);

				pNpc->m_presx = pNpc->m_presy = -1;
				pNpc->SightRecalc(m_pCom);
			}

			// ��ȯ�� ���� ��ȯ�� ��ǥ�� �߽����� �ൿ�ݰ��� ������.
			MAP* pMap = g_zone[pNpc->m_ZoneIndex];
			pNpc->m_nInitMinX = pNpc->m_sOrgX - pNpc->m_sMinX;		if(pNpc->m_nInitMinX < 1) pNpc->m_nInitMinX = 1; 
			pNpc->m_nInitMinY = pNpc->m_sOrgY - pNpc->m_sMinY;		if(pNpc->m_nInitMinY < 1) pNpc->m_nInitMinY = 1; 
			pNpc->m_nInitMaxX = pNpc->m_sOrgX + pNpc->m_sMaxX;		if(pNpc->m_nInitMaxX >= pMap->m_sizeMap.cx) pNpc->m_nInitMaxX = pMap->m_sizeMap.cx - 1;
			pNpc->m_nInitMaxY = pNpc->m_sOrgY + pNpc->m_sMaxY;		if(pNpc->m_nInitMaxY >= pMap->m_sizeMap.cy) pNpc->m_nInitMaxY = pMap->m_sizeMap.cy - 1;

			if(::InterlockedExchange(&pNpc->m_lNowSummoning, 0) != 1) pNpc->m_lNowSummoning = 0;

			return pNpc->m_sNid;
		}
	}

	return -1;
}

BOOL USER::CheckZoneWho(int zone, int passtype, int zonetype)
{
	int i;
	BOOL bCheck = FALSE;

	switch(zonetype)
	{
	case 1:		// 80�� ���̿��� ���� - ���ǽ� ������Ͽ����� �� �̺�Ʈ		
		for(i = 0; i < MAX_SINGLE_EVENT; i++)
		{
			if(g_QuestEventZone.m_SingleEventZone[i].m_lUsed == 1)
			{
				if(g_QuestEventZone.m_SingleEventZone[i].m_sEventZone == zone) bCheck = TRUE;
			}
		}

		if(bCheck) // �ش� ���� ����� �ְ� 
		{
			if(passtype == 1) return TRUE;	// ��� Ÿ���� 1�̸� ���� ���
		}
		else		// �ش� ���� ������ ������
		{
			if(passtype == 0) return TRUE;	// ��� Ÿ���� 0�̸� ���� ���
		}
		break;

	default:
		break;
	}

	return FALSE;
}

void USER::SendCharDataToOPUser(USER *pUser)
{
	if(!pUser) return;

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

	TempBuf.Add((DWORD)m_dwExp);
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

	TempBuf.Add(m_dwExpNext);						// Next Exp

/*	TempBuf.Add(m_dwAbnormalInfo);					// �����̻� ����

	for(i = 0; i < CLASS_NUM; i++)					// Change Class Infomation
	{
		TempBuf.Add(m_sChangeClass[i]);
	}
*/
	// Finito added 16 bytes below
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
	CBufferEx TempBuf1;

	TempBuf1.Add((short)(TempBuf.GetLength()) );
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

	SendBuf.Add(SIGHT_INFO_COMPRESSED );
	SendBuf.Add((short)comp_data_len );
	SendBuf.Add((short)org_data_len );
	SendBuf.Add(crc_value );
	SendBuf.Add((short)1 );

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
	// ���� ��
}

//-------------------------------------------------------------------------
//yskang 0.4 ��� �α� ���
//nOption 0:���Ӿ����� 1:��ȭ 2:������ �������� 3:PK����
//-------------------------------------------------------------------------
void USER::WriteOpratorLog(TCHAR *strContents, int nOption)//yskang 0.4
{
	if( m_tIsOP == 0 ) return;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[1024];	
	int				i;

	::ZeroMemory(szSQL, sizeof(szSQL));

	SDWORD sSLen = strlen(strContents);
	if(sSLen <= 0 || sSLen >= 200) return;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call WRITE_OPERATOR_LOG(\'%s\', \'%s\',%d)}"), m_strUserID, strContents,nOption ); 

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB( db_index );
	if( !pDB ) return;

	retcode = SQLAllocHandle( (SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt );
	if (retcode == SQL_SUCCESS)
	{
		i = 1;
//		SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(strSentence), 0, (TCHAR*)strSentence, 0, &sSLen );

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg( hstmt );
		}
	}
	else return;

	if (hstmt!=NULL) SQLFreeHandle( (SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	DB �� ���� ����, ����, ����, �Ǿ�, �н�Ʈ��, ���ε��ũ, ���̿��н���, �Ǿ�̽��� ������ �����Ѵ�.
//
void USER::SetPsiOne(BYTE tKind, DWORD dwTime)
{
	m_dwHasteTime = m_dwShieldTime = m_dwDexUpTime = m_dwMaxHPUpTime = 0;
	m_dwFastRunTime = m_dwBigShieldTime = m_dwMindShockTime = m_dwPsiShieldTime = m_dwPiercingShieldTime = 0;

	switch(tKind)
	{
	case PSIONIC_HASTE: //����
		m_dwHasteTime = dwTime;
		break;
	case PSIONIC_SHIELD://����
		m_dwShieldTime = dwTime;
		break;
	case PSIONIC_DEXUP://����֮��
		m_dwDexUpTime = dwTime;
		break;
	case PSIONIC_HPUP://����֮��
		m_dwMaxHPUpTime = dwTime;
		break;
	case PSIONIC_FAST_RUN:
		m_dwFastRunTime = dwTime;
		break;
	case PSIONIC_MIND_SHOCK:
		m_dwMindShockTime = dwTime;
		break;
	case PSIONIC_PSI_SHIELD:
		m_dwPsiShieldTime = dwTime;
		break;
	case 30:
		m_dwBigShieldTime = dwTime;
		break;
	case PSIONIC_PIERCING_SHIELD:
		m_dwPiercingShieldTime = dwTime;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	DB �� ���� ���¾�, ���ݷ¾�, ����Ŀ ������ �����Ѵ�.
//
void USER::SetPsiTwo(BYTE tKind, DWORD dwTime)
{
	m_dwAdamantineTime = m_dwMightyWeaponTime = m_dwBerserkerTime = 0;

	switch(tKind)
	{
	case PSIONIC_ADAMANTINE:
		m_dwAdamantineTime = dwTime;
		break;
	case PSIONIC_MIGHTYWEAPON:
		m_dwMightyWeaponTime = dwTime;
		break;
	case PSIONIC_BERSERKER:
		m_dwBerserkerTime = dwTime;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	DB �� ���� ���ε尡�� ������ �����Ѵ�.
//
void USER::SetPsiThree(BYTE tKind, DWORD dwTime)
{
	m_dwMindGuardTime = 0;

	switch(tKind)
	{
	case PSIONIC_MIND_GUARD:
		m_dwMindGuardTime = dwTime;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	ħ�� �̺�Ʈ�� ���Ͽ� ���� ��ȯ�Ѵ�.
//
BOOL USER::InvaderSummon(BYTE tSlot)
{
	if(tSlot < 0 || tSlot >= TOTAL_INVEN_MAX) return FALSE;

	short sItemSid = m_UserItem[tSlot].sSid;
	if(sItemSid < 0 || sItemSid >= g_arItemTable.GetSize()) return FALSE;

	CPoint pt(-1, -1);

	short sMonsterID = myrand(1, 150);
	switch (sItemSid)
	{
	case 715:
		sMonsterID = 131;
		break;
	case 716:
		sMonsterID = 132;
		break;
	case 717:
        sMonsterID = 133;
		break;
	case 718:
		sMonsterID = 134;
		break;
	case 719:
		sMonsterID = 135;
		break;
	case 720:
		sMonsterID = 136;
		break;
	case 721:
		sMonsterID = 137;
		break;
	case 722:
		sMonsterID = 138;
		break;
	case 723:
		sMonsterID = 139;
		break;
	}
	
	if(sMonsterID < 0 || sMonsterID >= g_arNpc.GetSize()) return FALSE;
	

	g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_bMove = 1;
	pt = FindNearAvailablePoint_S(m_curx, m_cury);
	g_zone[m_ZoneIndex]->m_pMap[m_curx][m_cury].m_bMove = 0;
	
	if(pt.x == -1 || pt.y == -1) return FALSE;
	
	if(SummonQuestMonster(sMonsterID, m_curz, pt.x, pt.y) == -1) return FALSE;
//	if(SummonQuestMonster(sMonsterID, m_curz, m_curx, m_cury) == -1) return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////
//	������� ����, ���� �������� ������ ���ο� �ɼ��� ��´�.
//
void USER::RemagicItemReq(TCHAR *pBuf, BOOL bEvent)
{
	if(m_dwDN < ITEM_REMAGIC_COST) 
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return;		// ���׷��̵� �� ���� ������ ����
	}

	CBufferEx TempBuf;

	int iStart = 0, iCount = 3;
	int index = 0, iSuccess = 0;
	int i, j;
	int iWeight = 0;
	BYTE tItemSlot = -1;
	BYTE tMaterialSlot[3] = {0, 0, 0};
	BYTE bySlot = 0;
	short sMaterialSid[3] = {-1, -1, -1};
	CString szName1, szName2, szName3;
	ItemList RemagicItem;

	CByteArray arMaterial;
	arMaterial.RemoveAll();

	short sSourceSlot = GetShort(pBuf, index);	// ������ ������ ���Թ�ȣ

	if(sSourceSlot != -1) return;				// �������� �ҽ������� ����.

	if(bEvent)	// ũ�������� �̺�Ʈ �������� ����� ���
	{
//		if(!0)
//		{
			SendSystemMsg(IDS_EVENT_END, SYSTEM_NORMAL, TO_ME);
			return;
//		}
		iStart = 1;
		tItemSlot = GetShort(pBuf, index);
		if(tItemSlot < EQUIP_ITEM_NUM && tItemSlot >= TOTAL_INVEN_MAX) return;
		if(m_UserItem[tItemSlot].sSid != 872) return;		// ũ�������� ��������� �ƴϸ� ����
		if(m_UserItem[tItemSlot].sCount < 1) return;		// ������ ���߷��� ����

		arMaterial.Add(tItemSlot);
		iWeight += g_arItemTable[872]->m_byWeight;			// ���Ժ�ȭ�� üũ�Ѵ�.
	}

	j = 0;
	for(i = iStart; i < iCount + iStart; i++, j++)				// ���� ��� �˻�	
	{
		tMaterialSlot[j] = GetShort(pBuf, index);				// ��� ( ������ -1 )
		if(tMaterialSlot[j] < EQUIP_ITEM_NUM && tMaterialSlot[j] >= TOTAL_INVEN_MAX) return;

		sMaterialSid[j] = m_UserItem[tMaterialSlot[j]].sSid;
		if(m_UserItem[tMaterialSlot[j]].sCount <= 0) return;
		if(m_UserItem[tMaterialSlot[j]].tIQ != UNIQUE_ITEM) if(m_UserItem[tMaterialSlot[j]].tMagic[5] != 0) return;		// ���׷��̵� �������� �������� �ȵȴ�.
		if(m_UserItem[tMaterialSlot[j]].tIQ == REMODEL_ITEM || m_UserItem[tMaterialSlot[j]].tIQ == REMODEL_MAGIC_ITEM) return; //���� �������� �������� �ȵȴ�.
		
		if(sMaterialSid[j] < 0 || sMaterialSid[j] >= g_arItemTable.GetSize()) return;
		if(g_arItemTable[sMaterialSid[j]]->m_byWear < 1 || g_arItemTable[sMaterialSid[j]]->m_byWear > 5) return;	//����, ����
	}

	// ���� ������ ���������� �˻��Ѵ�.
	szName1 = g_arItemTable[sMaterialSid[0]]->m_strName;
	szName2 = g_arItemTable[sMaterialSid[1]]->m_strName;
	szName3 = g_arItemTable[sMaterialSid[2]]->m_strName;

	RemagicItem = m_UserItem[tMaterialSlot[0]];
	if(szName1 != szName2 || szName1 != szName3 || szName2 != szName3) return;

	iSuccess = GetMagicOption(&RemagicItem, 3, bEvent);
	RemagicItem.tIQ = RARE_ITEM;
	if(iSuccess == 0) return;

	
	m_UserItem[tMaterialSlot[0]] = RemagicItem;		// ���������ۿ� ���ο� �ɼǺο�

	for(i = 0; i < 3; i++)	
	{
		if(i != 0) 
		{
			iWeight += g_arItemTable[sMaterialSid[i]]->m_byWeight;			// ���Ժ�ȭ�� üũ�Ѵ�.
			MakeItemLog(&m_UserItem[tMaterialSlot[i]], ITEMLOG_REMAGIC_DELETE );
			ReSetItemSlot(&m_UserItem[tMaterialSlot[i]]);
		}
		arMaterial.Add(tMaterialSlot[i]);
	}

	if(tItemSlot != -1)
	{
		if(m_UserItem[tItemSlot].sCount <= 1)
		{
			MakeItemLog(&m_UserItem[tItemSlot], ITEMLOG_REMAGIC_DELETE );
			ReSetItemSlot(&m_UserItem[tItemSlot]);
		}
		else
		{
			m_UserItem[tItemSlot].sCount -= 1;
		}
	}

	FlushItemLog( TRUE );

	if( m_dwDN <= ITEM_REMAGIC_COST ) m_dwDN = 0;
	else m_dwDN = m_dwDN - ITEM_REMAGIC_COST;
	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();							// ������ ������ ������.

	TempBuf.Add(UPGRADE_ITEM_RESULT);
	index = arMaterial.GetSize();

	TempBuf.Add((BYTE)0x03); // ������ ����
	TempBuf.Add((BYTE)index);

	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		bySlot = (BYTE)arMaterial[i];
	
		TempBuf.Add(bySlot);
		TempBuf.Add(m_UserItem[bySlot].sLevel);
		TempBuf.Add(m_UserItem[bySlot].sSid);
		TempBuf.Add(m_UserItem[bySlot].sDuration);
		TempBuf.Add(m_UserItem[bySlot].sBullNum);
		TempBuf.Add(m_UserItem[bySlot].sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);

		TempBuf.Add(m_UserItem[bySlot].tIQ); 
	}

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

	Send(TempBuf, TempBuf.GetLength());

	arMaterial.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////////////
//	���ο� ���� �ɼ��� ��´�.
//
BOOL USER::GetMagicOption(ItemList* pItem, BYTE tIQ, BYTE tSame)
{
	int iMagicCount = 0;
	int nLoop = 0;
	int i = 0, j = 0, iRandom = 0;
	int iCount = 0, iTemp = 0;
	short sSid = -1;

	if(pItem == NULL) return FALSE;
	sSid = pItem->sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return FALSE;

	if(tIQ == MAGIC_ITEM) nLoop = 2;
	else if(tIQ == RARE_ITEM) nLoop = 4;
	else return FALSE;

	if(m_sLevel <= 20)			iMagicCount = 42;		// �����ε��� ������ ������ ���� �����Ѵ�. 
	else if(m_sLevel <= 40)		iMagicCount = 106;
	else if(m_sLevel <= 90)		iMagicCount = 112;
	else						iMagicCount = 145;

	if(iMagicCount >= g_arMagicItemTable.GetSize()) iMagicCount = g_arMagicItemTable.GetSize() - 1;

	pItem->tMagic[0] = pItem->tMagic[1] = pItem->tMagic[2] = pItem->tMagic[3] = pItem->tMagic[4] = pItem->tMagic[5] = 0;

	i = 0;
	while(nLoop > i)										
	{    
	//	sRandom = myrand(1,42);
		iRandom = myrand(1, iMagicCount);

		if(!g_arMagicItemTable[iRandom]->m_tUse) continue;
		if(g_arMagicItemTable[iRandom]->m_sSubType == 32) continue;	// ������ �ɼ��� ������ �ʴ´�.
		
		if(CheckClassWear(sSid, iRandom) == FALSE)			// Npc.cpp �� CheckClassItem �� ���� ����� ����. User �� CheckClassItem �� ȣ���ϸ� �ȵ�
		{
			if(i == 0) continue;							// ������ �⺻�� 1��
			else if(tIQ == RARE_ITEM && i <= 2) continue;	// ����� �⺻�� 3��
			else { i++; continue; }
		}

		for(j = 0; j < 4; j++)
		{
			if(tSame != TRUE)
			{
				iCount = g_arMagicItemTable[pItem->tMagic[j]]->m_sSubType;
				if(iCount != 0 && iCount == g_arMagicItemTable[iRandom]->m_sSubType)	// �Ӽ��� ��ĥ�� �����Ƿ� ���� ū���� ����	
				{
					iCount = g_arMagicItemTable[pItem->tMagic[j]]->m_sChangeValue;
					if(iCount < g_arMagicItemTable[iRandom]->m_sChangeValue)
					{
						iTemp = g_arMagicItemTable[pItem->tMagic[j]]->m_tLevel;
						if(pItem->sLevel - iTemp > 0) pItem->sLevel -= iTemp;
						pItem->sLevel += g_arMagicItemTable[iRandom]->m_tLevel;
						pItem->tMagic[j] = iRandom; 
						
						break;
					}
					else if(iCount == g_arMagicItemTable[iRandom]->m_sChangeValue) break;
				}
			}

			if(pItem->tMagic[j] > 0) continue;										// �̹� ���Կ� ���� ������ �Ѿ
			else
			{ 
				pItem->tMagic[j] = iRandom; i++;
				if(g_arMagicItemTable[iRandom]->m_tLevel > 0) pItem->sLevel += g_arMagicItemTable[iRandom]->m_tLevel;
				break; 
			}
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//	���� �ο��Ϸ��� �����Ӽ��� �ش� �����ۿ� ����� �� �ִ��� Ŭ���������� ���������� �˻��Ѵ�.
//
BOOL USER::CheckClassWear(int iItemSid, int iMagicSid)
{
	if(iItemSid < 0 || iItemSid >= g_arItemTable.GetSize()) return FALSE;
	if(iMagicSid < 0 || iMagicSid >= g_arMagicItemTable.GetSize()) return FALSE;

	BYTE tItemWear = g_arItemTable[iItemSid]->m_byWear;				// ������ ��������
	BYTE tMagicWear = g_arMagicItemTable[iMagicSid]->m_tWearInfo;	// �����ɼ� ���� ����

	BYTE tNeedClass = g_arItemTable[iItemSid]->m_byClass;			// ������ Ŭ����
	BYTE tMagicClass = g_arMagicItemTable[iMagicSid]->m_tNeedClass;	// �����Ӽ� Ŭ����

	// Class üũ
	if(tMagicClass != 15)		// ���迭�� ���� �� �ִ� �ɼ��� �ƴϸ�
	{
		BYTE tTemp = 1;	
		BYTE tFire = 0;
		BYTE tEdge = 0;
		BYTE tStaff = 0;
		BYTE tBrawl = 0;

		tFire	 = tTemp & tNeedClass; tTemp = 2; 
		tEdge	 = tTemp & tNeedClass; tTemp = 4;
		tStaff	 = tTemp & tNeedClass; tTemp = 8;
		tBrawl	 = tTemp & tNeedClass;

		tFire = tFire & tMagicClass;
		tEdge = tEdge & tMagicClass;
		tStaff = tStaff & tMagicClass;
		tBrawl = tBrawl & tMagicClass;

		tTemp = tFire^tEdge^tStaff^tBrawl;
		if(!tTemp) return FALSE;
	}

	// WearInfo üũ
	if(tMagicWear == 0) return TRUE;
	else if(tMagicWear == 1)											
	{														// 1���̸� ������� �ٴ´�.
		if(tItemWear != 1) return FALSE;
		else return TRUE;
	}
	else if(tMagicWear == 2)								// 2���̸� ���⸦ ������ ��������ۿ� �ٴ´�.
	{
		if(tItemWear <= 1 || tItemWear >= 9) return FALSE;
		else return TRUE;
	}
	else return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////
//	EBody �� ���׷��̵� �Ѵ�.
//
void USER::EBodyUpgradeReq(TCHAR *pBuf)
{
#ifdef _ACTIVE_USER
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	if(m_dwDN < EBODY_UPGRADE_COST) 
	{
		SendSystemMsg( IDS_USER_NOT_ENOUGH_PAY, SYSTEM_ERROR, TO_ME);
		return;		// ���׷��̵� �� ���� ������ ����
	}
	if(m_dwXP < EBODY_UPGRADE_XP) 
	{
		SendSystemMsg( IDS_XP_ERROR, SYSTEM_ERROR, TO_ME);
		return;		// ���׷��̵� XP�� ������ ����
	}

	CBufferEx TempBuf;

	int iCount = 1, iUpgradeCount = 0;
	int index = 0, iSuccess = 0;
	int i, j;
	int iWeight = 0;
	BYTE tItemSlot = -1;
	short sMaterialSlot = 0;
	BYTE bySlot = 0;
	short sMaterialSid = -1;
	short sSourceSid = -1;
	BYTE tSourceWear = 0;
	int iRandom = 0;
	int iRate1 = 0, iRate2 = 0;

	ItemList TempItem;

	CByteArray arMaterial;
	arMaterial.RemoveAll();

	short sSourceSlot = GetShort(pBuf, index);	// ������ ������ ���Թ�ȣ
	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;	// �κ������� ���׷��̵� ����

	sSourceSid = m_UserItem[sSourceSlot].sSid;
	if(sSourceSid < 0 || sSourceSid >= g_arItemTable.GetSize()) return;

	for(i = 1; i < 5; i++)
	{
		if(m_UserItem[sSourceSlot].tMagic[i] <= 0) 
		{
			iUpgradeCount = i - 1;
			break;
		}
	}
	if(iUpgradeCount < 0 || iUpgradeCount > 3) return;	// EBdoy Upgrade �� �̹� 4�� ���ߴ�.
	if(iUpgradeCount >= g_arEBodyUpgradeTable.GetSize()) return;	// �߸��� ���׷��̵� ��

	tSourceWear = g_arItemTable[sSourceSid]->m_byWear;
	if(tSourceWear < 122 || tSourceWear > 125)			// ���׷��̵� �Ϸ��� �������� EBody �� �ƴ϶��
	{
		SendSystemMsg( IDS_USER_CANT_UPGRADE_ITEM, SYSTEM_ERROR, TO_ME);
		return;
	}

	j = 0;
	for(i = 0; i < iCount ; i++)				// ���� ��� �˻�	
	{
		sMaterialSlot = GetShort(pBuf, index);	// ��� ( ������ -1 )
		if(sMaterialSlot < EQUIP_ITEM_NUM && sMaterialSlot >= TOTAL_INVEN_MAX) return;

		sMaterialSid = m_UserItem[sMaterialSlot].sSid;
		if(m_UserItem[sMaterialSlot].sCount != 1) return;			// ���̴��� ��ġ�� �ʴ´�.
		if(m_UserItem[sMaterialSlot].tIQ != MAGIC_ITEM) return;		// ���̴��� ������ ���� ������
		
		if(sMaterialSid < 0 || sMaterialSid >= g_arItemTable.GetSize()) return;
		if(g_arItemTable[sMaterialSid]->m_byWear != 126) return;	// ���̴��� �ƴϸ� ����
	}

	iRandom = myrand(1, 100);
	iRate1 = g_arEBodyUpgradeTable[iUpgradeCount]->m_tRandom1;
	iRate2 = g_arEBodyUpgradeTable[iUpgradeCount]->m_tRandom2;

	if(iRandom <= iRate1) iSuccess = 1;
	else if(iRate1 < iRandom && iRandom <= iRate1 + iRate2) iSuccess = 0;
	else iSuccess = -1;

	if(iSuccess == -1)		// ��������, ���̽��� ���̴����� ���󰣴�.
	{
		MakeItemLog(&m_UserItem[sSourceSlot], ITEMLOG_EBODY_BASE_DELETE );
		MakeItemLog(&m_UserItem[sMaterialSlot], ITEMLOG_EBODY_ADDER_DELETE );

		ReSetItemSlot(&m_UserItem[sSourceSlot]);
		ReSetItemSlot(&m_UserItem[sMaterialSlot]);

		iWeight += g_arItemTable[sSourceSid]->m_byWeight;			// ���Ժ�ȭ�� üũ�Ѵ�.
		iWeight += g_arItemTable[sMaterialSid]->m_byWeight;		// ���Ժ�ȭ�� üũ�Ѵ�.

		arMaterial.Add((BYTE)sSourceSlot);
		arMaterial.Add((BYTE)sMaterialSlot);
	}
	else if(iSuccess == 0)	// ���̴��� ���󰣴�.
	{
		MakeItemLog(&m_UserItem[sMaterialSlot], ITEMLOG_EBODY_ADDER_DELETE );
		ReSetItemSlot(&m_UserItem[sMaterialSlot]);
		iWeight += g_arItemTable[sMaterialSid]->m_byWeight;		// ���Ժ�ȭ�� üũ�Ѵ�.

		arMaterial.Add((BYTE)sMaterialSlot);
	}
	else					// ����, ���̴��� ������, ���̽����� ���ο� �����Ӽ��� �߰��Ѵ�.
	{
		m_UserItem[sSourceSlot].tMagic[iUpgradeCount + 1] = m_UserItem[sMaterialSlot].tMagic[0];
		m_UserItem[sSourceSlot].sLevel += m_UserItem[sMaterialSlot].sLevel;		// EBody �� ���׷��̵� �ɶ����� ������ ���� �ö󰣴�.

		MakeItemLog(&m_UserItem[sSourceSlot], ITEMLOG_EBODY_UPGRADE_SUCCESS );
		MakeItemLog(&m_UserItem[sMaterialSlot], ITEMLOG_EBODY_ADDER_DELETE );

		ReSetItemSlot(&m_UserItem[sMaterialSlot]);

		iWeight += g_arItemTable[sMaterialSid]->m_byWeight;		// ���Ժ�ȭ�� üũ�Ѵ�.

		arMaterial.Add((BYTE)sSourceSlot);
		arMaterial.Add((BYTE)sMaterialSlot);
	}

	FlushItemLog( TRUE );

	if( m_dwDN <= EBODY_UPGRADE_COST ) m_dwDN = 0;
	else m_dwDN = m_dwDN - EBODY_UPGRADE_COST;
	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();							// ������ ������ ������.

	TempBuf.Add(UPGRADE_ITEM_RESULT);
	index = arMaterial.GetSize();

	if(iSuccess == 1) TempBuf.Add((BYTE)0x01);	// EBody Upgrade ����
	else TempBuf.Add((BYTE)0x00);				// EBody Upgrade ����

	TempBuf.Add((BYTE)index);

	for(i = 0; i < arMaterial.GetSize(); i++)
	{
		bySlot = (BYTE)arMaterial[i];
	
		TempBuf.Add(bySlot);
		TempBuf.Add(m_UserItem[bySlot].sLevel);
		TempBuf.Add(m_UserItem[bySlot].sSid);
		TempBuf.Add(m_UserItem[bySlot].sDuration);
		TempBuf.Add(m_UserItem[bySlot].sBullNum);
		TempBuf.Add(m_UserItem[bySlot].sCount);

		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[bySlot].tMagic[j]);

		TempBuf.Add(m_UserItem[bySlot].tIQ); 
	}

	m_iCurWeight -= iWeight;
	if(m_iCurWeight < 0) m_iCurWeight = 0;

	GetRecoverySpeed();							// ȸ���ӵ� üũ...

	Send(TempBuf, TempBuf.GetLength());

	arMaterial.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////
//	Ȯ���� �� �ִ� EBody ����Ʈ�� �����.
//
void USER::EBodyIdentifyOpen(int iStoreID)
{
#ifdef _ACTIVE_USER
	if(m_iDisplayType == 6) return; //yskang 0.5
#endif

	int rank = m_sCityRank + CITY_RANK_INTERVAL;
	if(rank == CHAONISE_RANK && IsCity()) 
	{
		SendSystemMsg( IDS_USER_CANT_USE_YOUR_CITY_RANK, SYSTEM_NORMAL, TO_ME);
		return;
	}

	int i;
	short sStoreID = iStoreID;

	int iStart = EQUIP_ITEM_NUM;
	int iEnd = TOTAL_INVEN_MAX;

	CStore* pStore = GetStore(sStoreID);
	if(pStore == NULL) return;

	CBufferEx CostBuf;
	BYTE tSlot = 0;
	short sCount = 0;
	for(i = iStart; i < iEnd; i++)
	{
		if(m_UserItem[i].sSid == 908)	// ��Ȯ�� ������ EBody �� Sid
		{
			tSlot = (BYTE)i;
			sCount++;
			CostBuf.Add(tSlot);
		}
	}

	CBufferEx TempBuf;
	TempBuf.Add(EBODY_IDENTIFY_OPEN);
	TempBuf.Add(sStoreID);
	TempBuf.Add(sCount);
	if(sCount > 0) TempBuf.AddData(CostBuf, CostBuf.GetLength());

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	����ѩ��
//
void USER::EBodyIdentifyReq(TCHAR* pBuf)
{
	int index = 0, i, j;
	int iRandom = 0;
	DWORD dwIdentifyCost = 0;
	DWORD dwNeedXP = 0;
	CByteArray	arSlot;
	ItemList MyItem[TOTAL_INVEN_MAX - EQUIP_ITEM_NUM];

	short sSid = -1, sEBodySid = -1;
	BYTE tSlot = 0;

	short sStoreID = GetShort(pBuf, index);
	if(sStoreID < 0) return;

	// �̺�Ʈ ��ġ�� �˻� -----------------------------------------//
	CPoint pt = ConvertToClient(m_curx, m_cury);
    if(!g_pEventBlock->CheckUserEvent(m_curz, pt.x, pt.y, REPAIR_BLOCK)) return;

	short sCount = GetShort(pBuf, index);

	if(sCount <= 0 || sCount >= TOTAL_INVEN_MAX - EQUIP_ITEM_NUM) return;

	for(i = 0; i < sCount; i++)
	{
		tSlot = GetByte(pBuf, index);
		if(tSlot < EQUIP_ITEM_NUM || tSlot >= TOTAL_INVEN_MAX) return;	// �κ������� Ȯ�� ����
		if(m_UserItem[tSlot].sCount != 1) return;						// ��ĥ �� ���� �������̴�.

		sSid = m_UserItem[tSlot].sSid;
		if (sSid != 908)
		{
			return ;
		}
		if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return;

		dwIdentifyCost += EBODY_IDENTIFY_COST;
		dwNeedXP += EBODY_IDENTIFY_XP;

		if(dwIdentifyCost > m_dwDN)
		{
			SendSystemMsg( IDS_USER_NOT_ENOUGH_DINA1, SYSTEM_ERROR, TO_ME);
			return;
		}
		if(dwNeedXP > m_dwXP)
		{
			SendSystemMsg( IDS_XP_ERROR, SYSTEM_ERROR, TO_ME);
			return;
		}

		arSlot.Add((BYTE)tSlot);
		MyItem[i] = m_UserItem[tSlot];
	}

	for(i = 0; i < arSlot.GetSize(); i++)
	{
		tSlot = arSlot[i];
		sSid = m_UserItem[tSlot].sSid;

		iRandom = myrand(0, g_arEBodyIdentifyTable.GetSize() -1);
		sEBodySid = g_arEBodyIdentifyTable[iRandom]->m_sSid;
		if(sEBodySid < 0 || sEBodySid > g_arItemTable.GetSize()) 
		{
			for(j = 0; j < arSlot.GetSize(); j++)
			{
				m_UserItem[arSlot[j]] = MyItem[j];
				return;
			}
		}

		m_UserItem[tSlot].sLevel	= g_arItemTable[sEBodySid]->m_byRLevel;
		m_UserItem[tSlot].sSid		= g_arItemTable[sEBodySid]->m_sSid;
		m_UserItem[tSlot].sDuration = g_arItemTable[sEBodySid]->m_sDuration;
		m_UserItem[tSlot].sBullNum	= g_arItemTable[sEBodySid]->m_sBullNum;
		m_UserItem[tSlot].sCount	= 1;
		for(j =0; j < MAGIC_NUM; j++) 
		{
			if(j == 0)	m_UserItem[tSlot].tMagic[j] = g_arItemTable[sEBodySid]->m_bySpecial;
			else		m_UserItem[tSlot].tMagic[j] = 0;
		}
		m_UserItem[tSlot].tIQ = MAGIC_ITEM; 
	}

	//-----------------[ Send Packet ]---------------------------------------//
	if(m_dwDN<=dwIdentifyCost)
		m_dwDN = 0;
	else
		m_dwDN = m_dwDN - dwIdentifyCost;
	

	m_dwXP = m_dwXP - dwNeedXP;
	if(m_dwXP < 0) m_dwXP = 0;

	UpdateUserItemDN();							// �������̴ϱ� �ٷ� �����Ѵ�.
	SendMoneyChanged();

	SendXP();

	CBufferEx TempBuf;

	TempBuf.Add(ITEM_MOVE_RESULT);
	TempBuf.Add((BYTE)0x01);				// ����
	TempBuf.Add((BYTE)0x00);				// �Ϲ� ������
	TempBuf.Add((BYTE)arSlot.GetSize());	// ��ȭ�� ������ ��

	for(i = 0;  i < arSlot.GetSize(); i++)
	{
		tSlot = arSlot[i];
		TempBuf.Add((BYTE)tSlot);
		TempBuf.Add(m_UserItem[tSlot].sLevel);
		TempBuf.Add(m_UserItem[tSlot].sSid);
		TempBuf.Add(m_UserItem[tSlot].sDuration);
		TempBuf.Add(m_UserItem[tSlot].sBullNum);
		TempBuf.Add(m_UserItem[tSlot].sCount);
		for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[tSlot].tMagic[j]);
		TempBuf.Add(m_UserItem[tSlot].tIQ); 
	}

	Send(TempBuf, TempBuf.GetLength());

	arSlot.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////
//	Finito - Sends server date and tiem to  client for screen capture.
//
void USER::ScreenCaptureReq()
{
	CBufferEx TempBuf;

    SYSTEMTIME st;
    GetSystemTime(&st);

	TempBuf.Add(SCREEN_CAPTURE_REQ);
	TempBuf.Add(st.wYear);
	TempBuf.Add(st.wMonth);
	TempBuf.Add(st.wDay);
	TempBuf.Add(st.wHour);
	TempBuf.Add(st.wMinute);
	TempBuf.Add(st.wSecond);

	Send(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	Finito - Sends Expression data to client
//
void USER::ExpressionReq(TCHAR* pBuf)
{
	int index = 0;
	short sExpression;

	sExpression = GetShort(pBuf, index);

	CBufferEx TempBuf;

	TempBuf.Add(EXPRESSION_REQ);
	TempBuf.Add((BYTE)0);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.Add(sExpression);

	Send(TempBuf, TempBuf.GetLength());
	SendInsight(TempBuf, TempBuf.GetLength());
}

//////////////////////////////////////////////////////////////////////////////////
//	Finito - Opens expression dialog
//
void USER::ExpressionOpen()
{
	CBufferEx TempBuf;

	TempBuf.Add(EXPRESSION_OPEN);
	TempBuf.Add((short)1);
	TempBuf.Add((short)77);

	Send(TempBuf, TempBuf.GetLength());
}

/////////////////////////////////////////////////////////////////////////
//	Cures HP, PP, SP and restores energy / bullets for level 10 and below
//
void USER::SendHelperCure()
{
	BYTE result = SUCCESS;

	CBufferEx TempBuf;

	// �̺�Ʈ ��ġ�� �˻� ----------------------------------------//
	CPoint pt = ConvertToClient(m_curx, m_cury);
	if(!g_pEventBlock->CheckUserEvent(m_curz, pt.x, pt.y, CURE_BLOCK)) return;

	if(m_sHP >= m_sMagicMaxHP && m_sPP >= m_sMagicMaxPP && m_sSP >= m_sMagicMaxSP)
	{
		if(m_UserItem[RIGHT_HAND].sBullNum >= 250 && m_byClass == EDGED || m_byClass == FIREARMS && m_UserItem[RIGHT_HAND].sSid != -1)
		{		
			SendSystemMsg( IDS_HELPER_NOT_NEED_CURE, SYSTEM_NORMAL, TO_ME);
			result = FAIL;
			goto go_result;
		}
		else if(m_UserItem[RIGHT_HAND].sSid == -1)
		{	
			SendSystemMsg( IDS_HELPER_NOT_NEED_CURE, SYSTEM_NORMAL, TO_ME);
			result = FAIL;
			goto go_result;
		}
		else if(m_UserItem[RIGHT_HAND].sSid != -1 &&  m_byClass == BRAWL || m_byClass == STAFF)
		{
			SendSystemMsg( IDS_HELPER_NOT_NEED_CURE, SYSTEM_NORMAL, TO_ME);
			result = FAIL;
			goto go_result;
		}
	}

	m_sHP = m_sMagicMaxHP;
	m_sPP = m_sMagicMaxPP;
	m_sSP = m_sMagicMaxSP;


	if ((m_byClass == EDGED) || (m_byClass == FIREARMS))
	{
		if(m_UserItem[RIGHT_HAND].sSid != -1)
		{
			m_UserItem[RIGHT_HAND].sBullNum = 250; 
		}
	}

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

	Send(TempBuf, TempBuf.GetLength());
	if(m_bNowBuddy == TRUE) SendBuddyUserChange(BUDDY_CHANGE);		// �������̸� �ٸ� ��������� ������.
	
	if ((m_byClass == EDGED) || (m_byClass == FIREARMS))
	{
		if (m_UserItem[RIGHT_HAND].sSid != -1)
		{
			SendBullNum(RIGHT_HAND);
		}
	}
	if (m_byClass == FIREARMS)
	{
		SendSystemMsg( IDS_HELPER_CURE_COMPLETED, SYSTEM_NORMAL, TO_ME);
		if (m_UserItem[RIGHT_HAND].sSid == -1)
		{
			SendSystemMsg( IDS_HELPER_CURE_COMPLETED2, SYSTEM_NORMAL, TO_ME);
		}
		else
		{
			TCHAR strHealMsg[128];
			::ZeroMemory(strHealMsg, sizeof(strHealMsg));
			wsprintf(strHealMsg, _ID(IDS_HELPER_CURE_COMPLETED2_CLASS), "Rifle");
			SendSystemMsg( strHealMsg, SYSTEM_NORMAL, TO_ME);
		}
	}
	else if (m_byClass == EDGED)
	{
		SendSystemMsg( IDS_HELPER_CURE_COMPLETED, SYSTEM_NORMAL, TO_ME);
		if (m_UserItem[RIGHT_HAND].sSid == -1)
		{
			SendSystemMsg( IDS_HELPER_CURE_COMPLETED2, SYSTEM_NORMAL, TO_ME);
		}
		else
		{
			TCHAR strHealMsg[128];
			::ZeroMemory(strHealMsg, sizeof(strHealMsg));
			wsprintf(strHealMsg, _ID(IDS_HELPER_CURE_COMPLETED2_CLASS), "Sabre");
			SendSystemMsg( strHealMsg, SYSTEM_NORMAL, TO_ME);
		}
	}
	else
	{
		SendSystemMsg( IDS_HELPER_CURE_COMPLETED, SYSTEM_NORMAL, TO_ME);
		SendSystemMsg( IDS_HELPER_CURE_COMPLETED2, SYSTEM_NORMAL, TO_ME);
	}
}

void USER::TogglePKButton(TCHAR* pBuf)
{
	CBufferEx TempBuf;
	if(pBuf[0] == 0x05)
	{
		TempBuf.Add((BYTE)0x3E);
		TempBuf.Add((BYTE)0x05);
		
		if(pBuf[1] == 0x00)
			TempBuf.Add((BYTE)0x00);
		if(pBuf[1] == 0x01)
			TempBuf.Add((BYTE)0x01);

		Send(TempBuf, TempBuf.GetLength());
	}
}

BOOL USER::GivePSI(BYTE tType, int iTime)
{
	CBufferEx TempBuf;
	switch (tType)
	{
	case PSIONIC_SHIELD:
		{
			SetShield(iTime);
			return TRUE;
		}
		break;
	case 30:
		{
			SetBigShield(iTime);
			return TRUE;
		}
		break;
	case 100://�˷�Ч��
		 m_dwHiExpTime = iTime*1000;
		m_dwLastHiExpTime = GetTickCount();
		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		AddAbnormalInfo(ABNORMAL_HIEXP);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);	
		Send(TempBuf, TempBuf.GetLength());
		return TRUE;
		break;
	case 101://����Ч��
		m_dwMagicFindTime = iTime*1000;
		//-----------------------------------------------------------------------------------------
		m_dwLastMagicFindTime = GetTickCount();
		TempBuf.Add(SET_USER_STATE);
		TempBuf.Add(m_uid + USER_BAND);
		AddAbnormalInfo(ABNORMAL_MAGICFIND);
		TempBuf.Add(m_dwAbnormalInfo);
		TempBuf.Add(m_dwAbnormalInfo_);
		Send(TempBuf, TempBuf.GetLength());
		return TRUE;
		break;
	default:
		break;
	}

	return FALSE;
}

void USER::PersonalShopReq(TCHAR* pBuf)
{
	int uid = 0, index = 0, i = 0, nNameLength = 0;
	BYTE byType;
	ItemList tempItem;
	ShopItem* pNewItem;
	
	if(m_bNowTrading == TRUE || m_bNoItemMove == TRUE) return;

	byType = GetByte(pBuf, index);

	switch(byType)
	{
	case 0x00:
		break;
	case 0x01:
		{
			USER* pUser = NULL;
									
			if(m_iPShopViewuid != -1)
			{
				pUser = GetUser(m_iPShopViewuid);
				if(pUser==NULL) return ;
				if(pUser != NULL || pUser->m_state == STATE_GAMESTARTED)
				{
					pUser->m_bViewingAShop = FALSE;
				}	
			}
			m_bPShopView = 0;
			m_iPShopViewuid = -1;
			m_bPShopOpen = FALSE; // Set FALSE because User now has no personal shop

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

			::ZeroMemory(m_PersonalShopName, sizeof(m_PersonalShopName));

			CBufferEx TempBuf;
			TempBuf.Add((BYTE)0xEF);
			TempBuf.Add((BYTE)0x07); // Shop Closed
			Send(TempBuf, TempBuf.GetLength());
			PersonalShopClose();
		}
		break;
	case 0x02:
		{
			
			m_bPShopOpen = TRUE; // Set TRUE because User now has a personal shop open
			if(!IsCity()) // Check user is in town area
			{
				SendSystemMsg( IDS_USER_INVALID_SHOP_LOCATION, SYSTEM_NORMAL, TO_ME);
				m_bPShopOpen = FALSE; // Set FALSE because User now has no personal shop				return;
				::ZeroMemory(m_PersonalShopName, sizeof(m_PersonalShopName));
				return;
			}
			::ZeroMemory(m_PersonalShopName, sizeof(m_PersonalShopName));
			nNameLength = GetVarString( sizeof( m_PersonalShopName ), m_PersonalShopName, pBuf, index);
			
			if(nNameLength == 0 || nNameLength > SHOP_NAME_LENGTH)	
			{
				SendSystemMsg( IDS_USER_INVALID_SHOP_NAME, SYSTEM_NORMAL, TO_ME);
				m_bPShopOpen = FALSE; // Set FALSE because User now has no personal shop
				return;
			}
			short ItemAmount;
			ItemAmount = GetShort(pBuf, index);
			if (ItemAmount > TOTAL_PERSONALSHOP_ITEM_NUM)
			{
				SendSystemMsg( IDS_USER_INVALID_SHOP_ITEMAMOUNT, SYSTEM_NORMAL, TO_ME);
				m_bPShopOpen = FALSE; // Set FALSE because User now has no personal shop
				return;
			}

			// Get Item data into ShopItem Array
			for(i=0;i< ItemAmount;i++)
			{
				short sItemSlot;
				short sItemCount;
				DWORD dwItemPrice;
				sItemSlot = GetShort(pBuf, index);
				sItemCount = GetShort(pBuf, index);
				dwItemPrice = GetDWORD(pBuf, index);

				if (m_UserItem[sItemSlot].sSid != -1)
				{
					if (sItemCount > m_UserItem[sItemSlot].sCount) 
					{
						SendSystemMsg( IDS_USER_INVALID_SHOP_ITEMCOUNT, SYSTEM_NORMAL, TO_ME);
						m_bPShopOpen = FALSE; // Set FALSE because User now has no personal shop
						return;
					}

					pNewItem = new ShopItem;
					pNewItem->sSlot = sItemSlot;
					pNewItem->dwItemPrice = dwItemPrice;
					pNewItem->ShopList.sLevel = m_UserItem[sItemSlot].sLevel;
					pNewItem->ShopList.sSid = m_UserItem[sItemSlot].sSid;
					pNewItem->ShopList.sDuration = m_UserItem[sItemSlot].sDuration;
					pNewItem->ShopList.sBullNum = m_UserItem[sItemSlot].sBullNum;
					pNewItem->ShopList.sCount = m_UserItem[sItemSlot].sCount;
					for(int j =0; j < MAGIC_NUM; j++)
						pNewItem->ShopList.tMagic[j] = m_UserItem[sItemSlot].tMagic[j];
					pNewItem->ShopList.tIQ = m_UserItem[sItemSlot].tIQ;
					pNewItem->ShopList.iItemSerial = m_UserItem[sItemSlot].iItemSerial;

					EnterCriticalSection( &m_CS_ShopItem );
					m_arShopItem.Add(pNewItem);
					LeaveCriticalSection( &m_CS_ShopItem );
				}
				else
				{
					m_bPShopOpen = FALSE; // Set FALSE because User now has no personal shop
					return;
				}
			}
			if(m_bPShopOpen == TRUE)
			{
				CBufferEx TempBuf;
				TempBuf.Add((BYTE)0xEF);
				TempBuf.Add((BYTE)0x08);
				Send(TempBuf, TempBuf.GetLength());
				PersonalShopOpen();	
			}
			break;
		}
	case 0x03:
		if(m_bPShopOpen == FALSE && m_bViewingAShop == FALSE)
		{
			uid = GetInt(pBuf, index);	
			ViewPersonalShop(0x03, uid);
		}
		break;
	case 0x04:
		PersonalShopBuy(pBuf + index);
		break;
	case 0x05:
		{
			USER* pUser = NULL;
			uid = GetInt(pBuf , index);

			if(uid < 0 || uid >= INVALID_BAND) return;
			pUser = GetUser(uid - USER_BAND);
			if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;
			
			pUser->m_bPShopView = 0;
			pUser->m_iPShopViewuid = -1;	
			m_bViewingAShop = FALSE;
		}
		break;
	case 0x06:
		ViewPersonalShop(0x0B, (m_uid+USER_BAND));
		break;
	default:
		break;
	}
}


void USER::PersonalShopOpen()
{
	CBufferEx TempBuf;

	TempBuf.Add((BYTE)0xEF);
	TempBuf.Add((BYTE)0x09);
	TempBuf.Add(m_uid + USER_BAND);
	TempBuf.AddString(m_PersonalShopName);
	Send(TempBuf, TempBuf.GetLength());
	SendInsight(TempBuf, TempBuf.GetLength());
}

void USER::PersonalShopClose()
{			
	CBufferEx TempBuf;

	TempBuf.Add((BYTE)0xEF);
	TempBuf.Add((BYTE)0x0A);
	TempBuf.Add(m_uid + USER_BAND);
	Send(TempBuf, TempBuf.GetLength());
	SendInsight(TempBuf, TempBuf.GetLength());
}

void USER::ViewPersonalShop(BYTE mode, int uid)
{
	CBufferEx TempBuf;
	CBufferEx TempBuf2;
	USER* pUser = NULL;
    int i, n;
	n = 0;
	
	if(uid < 0 || uid >= INVALID_BAND) return;

	pUser = GetUser(uid - USER_BAND);
	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;

	if(mode == 0x03)
	{
		if ( pUser->m_UserStatus == 0 && pUser->m_UserFlag == FALSE )
		{
			goto fail_result;
		}	

		if( strcmp(pUser->m_PersonalShopName, "") == 0) 
		{
			goto fail_result;
		}

		if(pUser->m_iPShopViewuid != -1)
		{
			USER* pUser2 = NULL;
			pUser2 = GetUser(pUser->m_iPShopViewuid);
			if(pUser2 == NULL || pUser2->m_state != STATE_GAMESTARTED)
			{
				pUser->m_bPShopView = FALSE;
				pUser->m_iPShopViewuid = -1;
			}
			else if(pUser2->m_bViewingAShop == FALSE)
			{
				pUser->m_bPShopView = FALSE;
				pUser->m_iPShopViewuid = -1;
			}
		}

		if(pUser->m_bPShopView == TRUE)
		{
			SendSystemMsg( IDS_USER_INVALID_SHOP_BEINGVIEWED, SYSTEM_NORMAL, TO_ME);
			return;
		}

		if(InterlockedCompareExchange((LONG*)&pUser->m_bPShopView, (long)1, (long)0) == (long)0)
		{
			InterlockedExchange((LONG*)&pUser->m_iPShopViewuid, (LONG)m_uid); // Set uid so that can keep track of who is viewing shop
			m_bViewingAShop = TRUE;
		}

		if(pUser->m_bPShopView == FALSE)
		{
			goto fail_result;
		}
	}

	EnterCriticalSection( &pUser->m_CS_ShopItem );
	for(i = 0; i < pUser->m_arShopItem.GetSize(); i++)
	{
		if ( pUser->m_arShopItem[i] == NULL ) continue;

		TempBuf2.Add(pUser->m_arShopItem[i]->dwItemPrice);
		TempBuf2.Add(pUser->m_arShopItem[i]->ShopList.sLevel);
		TempBuf2.Add(pUser->m_arShopItem[i]->ShopList.sSid);
		TempBuf2.Add(pUser->m_arShopItem[i]->ShopList.sDuration);
		TempBuf2.Add(pUser->m_arShopItem[i]->ShopList.sBullNum);
		TempBuf2.Add(pUser->m_arShopItem[i]->ShopList.sCount);
		TempBuf2.Add(pUser->m_arShopItem[i]->sSlot);

		for(int j =0; j < MAGIC_NUM; j++)
			TempBuf2.Add(pUser->m_arShopItem[i]->ShopList.tMagic[j]);
		TempBuf2.Add(pUser->m_arShopItem[i]->ShopList.tIQ);

		n++;
	}
	LeaveCriticalSection(&pUser->m_CS_ShopItem);

	TempBuf.Add((BYTE)0xEF);
	TempBuf.Add((BYTE)mode);
	TempBuf.Add(uid);
	TempBuf.Add((BYTE)n);
	TempBuf.Add((BYTE)0x00);
	TempBuf.AddData(TempBuf2, TempBuf2.GetLength());
	Send(TempBuf, TempBuf.GetLength());
	return;

fail_result:
	pUser->m_bPShopView = 0;
	pUser->m_iPShopViewuid = -1;
	m_bViewingAShop = FALSE;
}

void USER::PersonalShopBuy(TCHAR* pBuf)
{
	CString strMsg;

	CBufferEx TempBuf;
	CBufferEx TempBuf2;
	USER* pUser = NULL;
	BYTE result = FAIL;
	DWORD TotalCost = 0;
	ShopItem* ShopBackupItem[TOTAL_PERSONALSHOP_ITEM_NUM];
	short sShopAmount, sUserSlot, sShopCount, sShopSid = -1;
	ItemList		TempItem;
	ItemList		BuyerItem[TOTAL_ITEM_NUM], ShopItem[TOTAL_ITEM_NUM];
	ItemListArray	arItemListBuyer, arItemListShop;
	CWordArray		arEmptySlotBuyer, arSameSlotBuyer, arEmptySlotShop, arSameSlotShop;

    int i,k, j, n, index = 0, iWeight = 0, iSameSlot, iEmptySlot, iWeightOwner, ShopSize;
	n = 0;

	int uid = GetInt(pBuf, index);
	if(uid < 0 || uid >= INVALID_BAND) return;
	pUser = GetUser(uid - USER_BAND);

	if(pUser == NULL || pUser->m_state != STATE_GAMESTARTED) return;

	if ( pUser->m_UserStatus == 0 && pUser->m_UserFlag == FALSE ) return;

	if( strcmp(pUser->m_PersonalShopName, "") == 0) return; 
	
	for(i = 0; i < TOTAL_ITEM_NUM; i++)			// Backup Inven data
	{
		BuyerItem[i] = m_UserItem[i];
		ShopItem[i] = pUser->m_UserItem[i];
	}

	EnterCriticalSection( &pUser->m_CS_ShopItem );
	ShopSize = pUser->m_arShopItem.GetSize();
	if(ShopSize >TOTAL_PERSONALSHOP_ITEM_NUM) {
		LeaveCriticalSection( &pUser->m_CS_ShopItem );
		return ;
	}
	for(i = 0; i < ShopSize; i++)
	{
		ShopBackupItem[i] = pUser->m_arShopItem[i];
	}
	LeaveCriticalSection( &pUser->m_CS_ShopItem );
	
	sShopAmount = GetShort(pBuf, index);
	if(sShopAmount >TOTAL_PERSONALSHOP_ITEM_NUM)
		return ;

	for(k = 0; k < sShopAmount; k++)
	{
		// Source Item
		sUserSlot = GetShort(pBuf, index);
		sShopCount = GetShort(pBuf, index);

		if ( pUser->m_UserItem[sUserSlot].sSid == -1 ) goto fail_result;
		if(sUserSlot < EQUIP_ITEM_NUM || sUserSlot >= TOTAL_INVEN_MAX) goto fail_result;		
		if ( pUser->m_UserItem[sUserSlot].sCount < sShopCount ) goto fail_result;

		sShopSid = pUser->m_UserItem[sUserSlot].sSid;
		if( sShopSid < 0 || sShopSid >= g_arItemTable.GetSize() ) goto fail_result;
		if( sShopCount <= 0 ) goto fail_result;

		// Get Item Cost
		DWORD ItemPrice;
		EnterCriticalSection( &pUser->m_CS_ShopItem );
		for(int i = 0; i < pUser->m_arShopItem.GetSize(); i++)
		{
			if(pUser->m_arShopItem[i]->sSlot == sUserSlot)
			{
				ItemPrice = pUser->m_arShopItem[i]->dwItemPrice;
				break;
			}
		}
		LeaveCriticalSection(&pUser->m_CS_ShopItem);

		//Update Dina
		TotalCost += (ItemPrice * sShopCount);
		if(TotalCost > m_dwDN )
		{
			SendSystemMsg( IDS_USER_SHOP_NOTENOUGH_DINA, SYSTEM_ERROR, TO_ME);
			goto fail_result;
		}

		// Get Buyers New Inventory	
		//MakeItemLog( &m_UserItem[tSlot], ITEMLOG_EXCHANGE_GIVE, pUser->m_strUserID );

		iWeight = 0;
		iWeight = g_arItemTable[sShopSid]->m_byWeight * sShopCount;
		if(m_iMaxWeight < m_iCurWeight + iWeight)
		{
			SendSystemMsg( IDS_USER_SHOP_NOTENOUGH_WEIGHT, SYSTEM_ERROR, TO_ME);
			goto fail_result;
		}

		ReSetItemSlot(&TempItem);
		TempItem = pUser->m_UserItem[sUserSlot];
		TempItem.sCount = sShopCount;

		if(TempItem.sSid < 0 || TempItem.sSid >= g_arItemTable.GetSize()) goto fail_result;
		iSameSlot = GetSameItem(TempItem, INVENTORY_SLOT);

		if(iSameSlot == -1)
		{
			iEmptySlot = GetEmptySlot(INVENTORY_SLOT);
			if(iEmptySlot == -1)		
			{
				SendSystemMsg( IDS_USER_SHOP_NO_IVEN_SLOT, SYSTEM_ERROR, TO_ME);
				goto fail_result;
			}

			m_UserItem[iEmptySlot] = TempItem;

			m_UserItem[iEmptySlot].sSid = TempItem.sSid;
			m_UserItem[iEmptySlot].sLevel  = TempItem.sLevel;
			m_UserItem[iEmptySlot].sBullNum  = TempItem.sBullNum;
			m_UserItem[iEmptySlot].sDuration  = TempItem.sDuration;
			m_UserItem[iEmptySlot].sCount  = TempItem.sCount;
			for(j =0; j < MAGIC_NUM; j++) m_UserItem[iEmptySlot].tMagic[j] = TempItem.tMagic[j];
			m_UserItem[iEmptySlot].tIQ = TempItem.tIQ; 
			m_UserItem[iEmptySlot].iItemSerial = TempItem.iItemSerial;
			iWeightOwner = (g_arItemTable[m_UserItem[iEmptySlot].sSid]->m_byWeight * sShopCount);
			arEmptySlotBuyer.Add((BYTE)iEmptySlot);
		}
		else								
		{
			m_UserItem[iSameSlot].sSid = TempItem.sSid;
			m_UserItem[iSameSlot].sLevel  = TempItem.sLevel;
			m_UserItem[iSameSlot].sBullNum  = TempItem.sBullNum;
			m_UserItem[iSameSlot].sDuration  = TempItem.sDuration;
			CheckMaxValue((short &)m_UserItem[iSameSlot].sCount,(short)TempItem.sCount);

			for(j =0; j < MAGIC_NUM; j++) m_UserItem[iSameSlot].tMagic[j] = TempItem.tMagic[j];
			m_UserItem[iSameSlot].tIQ = TempItem.tIQ; 
			m_UserItem[iSameSlot].iItemSerial = TempItem.iItemSerial;
			iWeightOwner = (g_arItemTable[m_UserItem[iSameSlot].sSid]->m_byWeight * sShopCount);
			arSameSlotBuyer.Add((BYTE)iSameSlot);
		}

		// Update Shop Owner weight
		
		//MakeItemLog( &m_UserItem[tSlot], ITEMLOG_EXCHANGE_GIVE, pUser->m_strUserID );

		pUser->ReSetItemSlot(&TempItem);
		TempItem = pUser->m_UserItem[sUserSlot];
		TempItem.sCount = sShopCount;

		if(TempItem.sSid < 0 || TempItem.sSid >= g_arItemTable.GetSize()) goto fail_result;
		iSameSlot = pUser->GetSameItem(TempItem, INVENTORY_SLOT);
		if(iSameSlot == -1)
		{
			//MakeItemLog( &m_UserItem[iEmptySlot], ITEMLOG_EXCHANGE_RECEIVE, pUser->m_strUserID );
			pUser->ReSetItemSlot( &pUser->m_UserItem[sUserSlot] );
			EnterCriticalSection( &pUser->m_CS_ShopItem );
			for(int l = 0; l < pUser->m_arShopItem.GetSize(); l++)
			{
				if(pUser->m_arShopItem[l]->sSlot == sUserSlot)
				{
					pUser->m_arShopItem.RemoveAt(l);
					break;
				}
			}
			LeaveCriticalSection(&pUser->m_CS_ShopItem);

			arEmptySlotShop.Add((BYTE)sUserSlot);

		}
		else								
		{
			if(pUser->m_UserItem[iSameSlot].sCount - TempItem.sCount == 0)
			{
				pUser->ReSetItemSlot( &pUser->m_UserItem[sUserSlot] );
			}
			else
			{
				pUser->m_UserItem[iSameSlot].sCount -= TempItem.sCount;

				pUser->m_UserItem[iSameSlot].sSid = pUser->m_UserItem[iSameSlot].sSid;
				pUser->m_UserItem[iSameSlot].sLevel = pUser->m_UserItem[iSameSlot].sLevel;
				pUser->m_UserItem[iSameSlot].sBullNum = pUser->m_UserItem[iSameSlot].sBullNum;
				pUser->m_UserItem[iSameSlot].sDuration = pUser->m_UserItem[iSameSlot].sDuration;
				pUser->m_UserItem[iSameSlot].sCount = pUser->m_UserItem[iSameSlot].sCount;
			
				for(j =0; j < MAGIC_NUM; j++) pUser->m_UserItem[iSameSlot].tMagic[j] = pUser->m_UserItem[iSameSlot].tMagic[j];
				pUser->m_UserItem[iSameSlot].tIQ = pUser->m_UserItem[iSameSlot].tIQ; 
				pUser->m_UserItem[iSameSlot].iItemSerial = pUser->m_UserItem[iSameSlot].iItemSerial;
			}
			EnterCriticalSection( &pUser->m_CS_ShopItem );
			for(int l = 0; l < pUser->m_arShopItem.GetSize(); l++)
			{
				if(pUser->m_arShopItem[l]->sSlot == sUserSlot)
				{
					if(pUser->m_arShopItem[l]->ShopList.sCount - TempItem.sCount == 0)
					{
						pUser->m_arShopItem.RemoveAt(l);
					}
					else
					{
						pUser->m_arShopItem[l]->ShopList.sCount -= TempItem.sCount;
					}
					break;
				}
			}
			LeaveCriticalSection(&pUser->m_CS_ShopItem);

			arSameSlotShop.Add((BYTE)iSameSlot);
		}
	}

	if(TotalCost>m_dwDN)
		return ;
	//Update Buyer and Shop
	pUser->m_dwDN += TotalCost;
	pUser->m_iCurWeight -= iWeightOwner;
	if(pUser->m_iCurWeight < 0) pUser->m_iCurWeight = 0;
	pUser->SendMoneyChanged();
	pUser->GetRecoverySpeed();
	pUser->UpdateInvenSlot(&arEmptySlotShop, &arSameSlotShop);
	
	
	m_dwDN -= TotalCost;
	if(m_dwDN<0)
		m_dwDN=0;
	m_iCurWeight += iWeight;
	SendMoneyChanged();
	GetRecoverySpeed();
	UpdateInvenSlot(&arEmptySlotBuyer, &arSameSlotBuyer);
	
	pUser->UpdateUserItemDN();
	UpdateUserItemDN();

	pUser->m_bPShopView = 0;
	pUser->m_iPShopViewuid = -1;
	m_bViewingAShop = FALSE;

	if(sShopAmount == 1)
		strMsg.Format(IDS_USER_SHOP_BUY_SUCCESS, "Item");
	else
		strMsg.Format(IDS_USER_SHOP_BUY_SUCCESS, "Items");

	SendSystemMsg( strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ERROR, TO_ME);

	return;

fail_result:
	for(i = 0; i < TOTAL_ITEM_NUM; i++)		// Item Information Backup
	{
		m_UserItem[i] = BuyerItem[i];
		pUser->m_UserItem[i] = ShopItem[i];
	}
	EnterCriticalSection( &pUser->m_CS_ShopItem );
	pUser->m_arShopItem.RemoveAll();
	for(i = 0; i < ShopSize; i++)
	{
		pUser->m_arShopItem.Add(ShopBackupItem[i]); 
	}
	LeaveCriticalSection( &pUser->m_CS_ShopItem );

	pUser->m_bPShopView = 0;
	pUser->m_iPShopViewuid = -1;
	m_bViewingAShop = FALSE;

	if(sShopAmount == 1)
		strMsg.Format(IDS_USER_SHOP_BUY_FAIL, "Item");
	else
		strMsg.Format(IDS_USER_SHOP_BUY_FAIL, "Items");
	SendSystemMsg( strMsg.GetBuffer(strMsg.GetLength()), SYSTEM_ERROR, TO_ME);
	return;
}

BOOL USER::GetTransformOption(ItemList* pItem)
{
	int iMagicCount = 0;
	int nLoop = 0;
	int i = 0, j = 0, iRandom = 0;
	int iCount = 0, iTemp = 0;
	short sSid = -1;

	if(pItem == NULL) return FALSE;
	sSid = pItem->sSid;
	if(sSid < 0 || sSid >= g_arItemTable.GetSize()) return FALSE;

	nLoop = 4;

	//if(m_sLevel <= 20)					// �����ε��� ������ ������ ���� �����Ѵ�. 
	if(m_sLevel <= 20) iMagicCount = 42;
	else iMagicCount = 106;

	if(iMagicCount >= g_arMagicItemTable.GetSize()) iMagicCount = g_arMagicItemTable.GetSize() - 1;

	i = 0;
	srand( (unsigned)time( NULL ) );

	while(nLoop > i)										
	{
		iRandom = myrand(1, iMagicCount);

		if(!g_arMagicItemTable[iRandom]->m_tUse) continue;
		if(g_arMagicItemTable[iRandom]->m_sSubType == 32) continue;	// ������ �ɼ��� ������ �ʴ´�.
		
		if(CheckClassWear(sSid, iRandom) == FALSE)			// Npc.cpp �� CheckClassItem �� ���� ����� ����. User �� CheckClassItem �� ȣ���ϸ� �ȵ�
		{
			if(i == 0) continue;							// ������ �⺻�� 1��
			else if(i <= 3) continue;	// ����� �⺻�� 3��
			else { i++; continue; }
		}

		for(j = 0; j < 4; j++)
		{
			iCount = g_arMagicItemTable[pItem->tMagic[j]]->m_sSubType;
			if(iCount != 0 && iCount == g_arMagicItemTable[iRandom]->m_sSubType)	// �Ӽ��� ��ĥ�� �����Ƿ� ���� ū���� ����	
			{
				iCount = g_arMagicItemTable[pItem->tMagic[j]]->m_sChangeValue;
				if(iCount < g_arMagicItemTable[iRandom]->m_sChangeValue)
				{
					iTemp = g_arMagicItemTable[pItem->tMagic[j]]->m_tLevel;
					if(pItem->sLevel - iTemp > 0) pItem->sLevel -= iTemp;
					pItem->sLevel += g_arMagicItemTable[iRandom]->m_tLevel;
					pItem->tMagic[j] = iRandom; 
					break;
						
				}
				else if(iCount == g_arMagicItemTable[iRandom]->m_sChangeValue) break;
			}
			if(pItem->tMagic[j] > 0) continue;										// �̹� ���Կ� ���� ������ �Ѿ
			else
			{ 
				pItem->tMagic[j] = iRandom; i++;
				if(g_arMagicItemTable[iRandom]->m_tLevel > 0) pItem->sLevel += g_arMagicItemTable[iRandom]->m_tLevel;
				break;
			}
		}
	}
	return TRUE;
}

void USER::TransformWindow()
{
	if(FindEvent(24) == TRUE) return;
	if(m_sLevel < 20) return;

	CBufferEx TempBuf;

	TempBuf.Add((BYTE)0xEC);
	TempBuf.Add((BYTE)0x00);
	Send(TempBuf, TempBuf.GetLength());

}

void USER::TransformReq(TCHAR* pBuf)
{
	short sSourceSid = -1;
	int index = 0, j = 0;
	BYTE result = TRUE;

	if(FindEvent(24) == TRUE) return;

	if(m_sLevel < 20) return;

	// Source Item
	short sSourceSlot = GetShort(pBuf, index);			// ������ ������ ���Թ�ȣ
	if(sSourceSlot < EQUIP_ITEM_NUM || sSourceSlot >= TOTAL_INVEN_MAX) return;
	
	sSourceSid = m_UserItem[sSourceSlot].sSid;
	if(sSourceSid < 0 || sSourceSid >= g_arItemTable.GetSize()) return;
	
	if( sSourceSid == 669 || sSourceSid == 670 )		// ���׷��̵� �Ϸ��� �������� �۷����������
	{
		result = FALSE;
		goto result;
	}

	if(g_arItemTable[sSourceSid]->m_byWear < 1 && g_arItemTable[sSourceSid]->m_byWear > 5) // ����, ���� �ƴϸ� ������ �� ����.
	{
		result = FALSE;
		goto result;
	}

	if(m_UserItem[sSourceSlot].tMagic[5] >= 1)			// ���׷��̵�� �������� ������ �� ����.
	{
		result = FALSE;
		goto result;
	}

	if(m_UserItem[sSourceSlot].sLevel > 40)	
	{
		result = FALSE;
		goto result;
	}

	if(!GetTransformOption(&m_UserItem[sSourceSlot]))
	{
		result = FALSE;
		goto result;
	}

	m_UserItem[sSourceSlot].tIQ = RARE_ITEM;

result:
	CBufferEx TempBuf;
	TempBuf.Add((BYTE)TRANSFORM_REQ);
	TempBuf.Add(result);
	if(result == FALSE)
	{
		Send(TempBuf, TempBuf.GetLength());
		return;
	}

	AddMyEventNum(24);

	TempBuf.Add((short)sSourceSlot);						
	TempBuf.Add(m_UserItem[sSourceSlot].sLevel);
	TempBuf.Add(m_UserItem[sSourceSlot].sSid);
	TempBuf.Add(m_UserItem[sSourceSlot].sDuration);
	TempBuf.Add(m_UserItem[sSourceSlot].sBullNum);
	TempBuf.Add(m_UserItem[sSourceSlot].sCount);
	for(j =0; j < MAGIC_NUM; j++) TempBuf.Add(m_UserItem[sSourceSlot].tMagic[j]);
	TempBuf.Add(m_UserItem[sSourceSlot].tIQ);
	
	Send(TempBuf, TempBuf.GetLength());
}
