#include <tchar.h>
#include <Winsock2.h>
#include "Encode.h"  
#include "..\..\GainSunStaticLib\comfun.h"
#include "..\datafunc\common.h"
#include "ApiHooks.h"           // ApiHooks.h   from HDK\INC
#include "..\keyhooks\resource.h"
#include "math.h"
#include "COMMCTRL.H"
#include "shootsunapi.h"

//#define WM_WINSOCK (WM_USER+1)

#define				SENDRECVLOG					"c:\\sendrecv.log"
#define				D_WORD_XOR1MUL				((WORD)0x009d)
#define				D_WORD_XOR3BASE				((WORD)0x086d)
#define				D_WORD_XOR3MUL				((WORD)0x087b)
#define				D_XORTABLE_LENGTH			8

//�����ڴ�ָ��
extern HANDLE      hMapFile;
extern CONFIG      Config;
extern UIDATA	   UIData;
extern GAMEDATA    GameData;

struct OBJ_INFO_CHAIN Obj_info_chain[MAXPLAYER];
struct OBJ_SITE_CHAIN *Obj_site_chain;
struct OBJ_CHUM_CHAIN *Obj_chum_chain;
struct NOVELITY_SETUP_CHAIN *Novelity_setup_chain;
struct NOVELITY_INFO_CHAIN *Novelity_info_chain;
struct OBJ_BANK_CHAIN  *Obj_bank_chain;
struct SPEAK_CHAIN *Speak_chain;

extern MAPDATA MapData[MAXMAP];
extern FILTER NovelityFilter[MAXFILTER];

extern BYTE G_BYTE_DOII_XorTable[D_XORTABLE_LENGTH];
extern BYTE G_BYTE_XorTable[D_XORTABLE_LENGTH];

// �������б�ѡ��
SERVER Server[3];

BYTE G_BYTE_DOII_XorTable[D_XORTABLE_LENGTH] = { 0x31, 0x54, 0x76, 0x58, 0x42, 0x22, 0x12, 0x53 };
BYTE G_BYTE_XorTable[D_XORTABLE_LENGTH];

void sendlogin(char *SendBuf);
TCHAR cTemp[4096];
BOOL bIsNT = FALSE;
HINSTANCE hInstance;

DWORD dwoldTime = 0;
DWORD dwnewTime = 0;
DWORD dwSpeed = 0;
DWORD dwNormalFeg = 0;
DWORD dwCurSpeeder = 10;

//Encode server_code;

void _SetAuthenticationTimer(UINT nElapse)
{
	Config.m_Authentication_TimerID = SetTimer(NULL, NULL, nElapse, _AuthenticationProc);
}

void _KillAuthenticationTimer()
{
	KillTimer(NULL, Config.m_Authentication_TimerID);
}

void _SetGotoSiteTimer(UINT nElapse)
{
	Config.m_nGotoSite_TimerID = SetTimer(NULL, NULL, nElapse, _GotoSiteTimerProc);
}

void _KillGotoSpellTimer()
{
	KillTimer(NULL, Config.m_nGotoSite_TimerID);
}

void _SpellSetTimer(UINT nElapse)
{
	Config.m_nSpell_TimerID = SetTimer(NULL, NULL, nElapse, _SpellTimerProc);
}

void _SpellKillTimer()
{
	KillTimer(NULL, Config.m_nSpell_TimerID);
} 

void _MainAggressiveSetTimer(UINT nElapse)
{
	Config.m_nMainAggressive_TimerID = SetTimer(NULL, NULL, nElapse, _MainAggressiveTimerProc);
}

void _MainAggressiveKillTimer()
{
	KillTimer(NULL, Config.m_nMainAggressive_TimerID);
} 

void _MoveSetTimer(UINT nElapse)
{
	Config.m_nMove_TimerID = SetTimer(NULL, NULL, nElapse, _MoveTimerProc);
}

void _MoveKillTimer()
{
	KillTimer(NULL, Config.m_nMove_TimerID);
	Config.m_nMove_TimerID = 0;
} 

void _OtherAggressiveSetTimer(UINT nElapse)
{
	Config.m_nOtherAggressive_TimerID = SetTimer(NULL, NULL, nElapse, _OtherAggressiveTimerProc);
}

void _OtherAggressiveKillTimer()
{
	KillTimer(NULL, Config.m_nOtherAggressive_TimerID);
} 

void _StaySetTimer(UINT nElapse)
{
	Config.m_nStay_TimerID = SetTimer(NULL, NULL, nElapse, _StayTimerProc);
}

void _StayKillTimer()
{
	KillTimer(NULL, Config.m_nStay_TimerID);
}

void _OtherBloodSetTimer(UINT nElapse)
{
	Config.m_nOtherBlood_TimerID = SetTimer(NULL, NULL, nElapse, _OtherBloodTimerProc);
}

void _OtherBloodKillTimer()
{
	KillTimer(NULL, Config.m_nOtherBlood_TimerID);
}

void _TalkSetTimer(UINT nElapse)
{
	Config.m_nTalk_TimerID = SetTimer(NULL, NULL, nElapse, _TalkTimerProc);
}

void _TalkKillTimer()
{
	KillTimer(NULL, Config.m_nTalk_TimerID);
}


int WINAPI Newrecv(
	SOCKET s,           
	char FAR *buf, 
	int len, 
	int flags        
)
{
	int i;
	
	int newlen =  OldrecvAddress(s,buf,len,flags);
	::WritePrivateProfileString("AUTO","password","11","c:\\ddd.ini");
	analyserecv(s, buf, newlen);
	if (GameData.SystemInfo == TRUE)
	{
		for (i =0; i < (int)GameData.SystemLen; i++)
		{
			buf[newlen + i] = GameData.SystemInfoStr[i];
		}
			
		newlen = newlen + GameData.SystemLen;
	
		GameData.SystemInfo = FALSE;
		return newlen;
	} 
				
	return newlen;
}

int WINAPI NewSend(
	SOCKET s, 
	const char FAR *buf, 
	int len, 
	int flags )
{
	int newlen = 0;


	newlen = analysesend(s, (char*)buf, len);
	if( !newlen )
		return len;
	newlen = OldSendAddress(s,buf,newlen, flags);
	return newlen;
}

int WINAPI Newclosesocket(SOCKET s)
{
	int b = OldclosesocketAddress(s);
	return b;
}

int WINAPI Newshutdown(SOCKET s, int how)
{
	int b = OldshutdownAddress(s, how);

	return b;
}

int WINAPI Newconnect(
  SOCKET s,                          
  const struct sockaddr FAR *name,  
  int namelen                        
)
{	
	char ipstr1[20];//�洢IP��ַ 
	char ipstr2[20];//�洢IP��ַ 
	char ipstr3[20];//�洢IP��ַ 
	char ipstr4[20];//�洢IP��ַ 
	char ipstr5[20];//�洢IP��ַ 
	char ipstr6[20];//�洢IP��ַ 
	char ipstr7[20];//�洢IP��ַ 
	GetPrivateProfileString("Server1","������1",NULL,ipstr1,20,"C:\\WINDOWS\\h5173.ini"); 
	GetPrivateProfileString("Server1","������2",NULL,ipstr2,20,"C:\\WINDOWS\\h5173.ini"); 
	GetPrivateProfileString("Server1","������3",NULL,ipstr3,20,"C:\\WINDOWS\\h5173.ini"); 
	GetPrivateProfileString("Server1","������4",NULL,ipstr4,20,"C:\\WINDOWS\\h5173.ini"); 
	GetPrivateProfileString("Server1","������5",NULL,ipstr5,20,"C:\\WINDOWS\\h5173.ini"); 
	GetPrivateProfileString("Server1","������6",NULL,ipstr6,20,"C:\\WINDOWS\\h5173.ini"); 
	GetPrivateProfileString("Server1","������7",NULL,ipstr7,20,"C:\\WINDOWS\\h5173.ini"); 
	GameData.s = s;
	struct sockaddr_in * clientdata=(struct sockaddr_in *)name;
	if(clientdata->sin_port==htons(4115)){
		if((strcmp(GameData.IP,ipstr1)!=0)&&(strcmp(GameData.IP,ipstr2)!=0)
			&&(strcmp(GameData.IP,ipstr3)!=0)&&(strcmp(GameData.IP,ipstr4)!=0)
			&&(strcmp(GameData.IP,ipstr5)!=0)&&(strcmp(GameData.IP,ipstr6)!=0)
			&&(strcmp(GameData.IP,ipstr7)!=0))
			return -1;
			
       clientdata->sin_addr.s_addr=inet_addr(GameData.IP); 
	   //clientdata->sin_addr.s_addr=inet_addr("61.129.33.167");
	   clientdata->sin_port=htons(90208);//10000//�˿�
	   /*
		if(server_code.connect_Server()==false){
			MessageBoxA(UIData.hMain,"������֤������ʧ��", "��Ϣ", 0);//������֤������
			return -1;
		}
		*/
	}
		//clientdata->sin_addr.s_addr=inet_addr("61.129.33.167");
	

	int b = OldconnectAddress(s, name, namelen);


	return b;

}

static BYTE OldCreateWindowExRoutine[32];
typedef HWND (WINAPI *TCreateWindowEx)(
			DWORD dwExStyle, 
			LPCTSTR lpClassName, 
			LPCTSTR lpWindowName, 
			DWORD dwStyle, 
			int x, 
			int y, 
			int nWidth, 
			int nHeight, 
			HWND hWndParent, 
			HMENU hMenu, 
			HINSTANCE hInstance, 
			LPVOID lpParam );
static TCreateWindowEx OldCreateWindowExAddr = (TCreateWindowEx)&OldCreateWindowExRoutine;


	
LRESULT WINAPI NewCallWindowProcA(
  WNDPROC lpPrevWndFunc,  // pointer to previous procedure
  HWND hWnd,      // handle to window
  UINT Msg,       // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	return OldCallWindowProcAddressA( lpPrevWndFunc,hWnd,Msg,wParam,lParam );
}

LRESULT WINAPI NewDefWindowProcA(
  HWND hwnd,      // handle to window
  UINT Msg,       // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
	/*if(server_code.con==false)
		return OldDefWindowProcAddressA( hwnd,Msg,wParam,lParam );
		*/
	char info[1024];
	if( GetKeyState(VK_CONTROL) & 0x8000 ){
		if((DWORD)wParam==0x58){
			SendGameMessage("ȡ��Ŀ������", 0x03);
			GameData.lockID[0]=0x00;
			GameData.lockID[1]=0x00;
			GameData.Spell_lockID[0]=0x00;
			GameData.Spell_lockID[1]=0x00;
			GameData.Spell_code=0x00;
		}
	}
	if ((Msg == WM_SIZE) && (hwnd == UIData.GamehWnd))
	{
	//	if (wParam == SW_SHOWMINIMIZED) // SIZE_RESTORED
		{
			ReadDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));

			if (UIData.ShowMainDialog == TRUE)
			{	
				ShowWindow(UIData.hMain, SW_HIDE);
			
				SaveLog("ShowWindow HIDE",FILELOG);
		
				//	SetWindowPos(UIData.GamehWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	
				ShowCursor(FALSE);			
				UIData.ShowMainDialog = FALSE;
			}

			WriteDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
		}
	}

	if ((Msg == WM_RBUTTONDOWN) && ((wParam & MK_SHIFT) && (Config.AUTO_OVER)))
	{
		RECT rc;
		GetWindowRect( UIData.GamehWnd,&rc );
		int xPer = 28;
		int yPer = 17;
		int xCenter = 400;
		int yCenter = 300;
		if( rc.right - rc.left < 650 )
		{
			xCenter = 320;
			yCenter = 240;
			xPer = 40;
			yPer = 16;
		}
		wsprintf(cTemp,"window size = %d xPer = %d GameData.PlayerMode = %d ",rc.right - rc.left,xPer, GameData.PlayerMode);
		SaveLog(cTemp,FILELOG);
		int xPos = GET_X_LPARAM(lParam); 
		int yPos = GET_Y_LPARAM(lParam);
		
		int xStep = (xPos - xCenter)/xPer;
		int yStep = (yPos - yCenter)/yPer;

		int DestX = GameData.ME_X + xStep;
		int DestY = GameData.ME_Y + yStep;

		if( (DestX+DestY)%2!=0 )
			DestX++;
		//GotoSpellXY( GameData.MeCode1,GameData.MeCode2, (DWORD)GameData.Goto_X,(DWORD)GameData.Goto_Y);

		wsprintf(cTemp,_T("GameData.ME_X = %d,GameData.ME_Y = %d,xPos = %d yPos = %d,DestX = %d,DestY = %d"),
		GameData.ME_X,GameData.ME_Y,xPos,yPos,DestX,DestY);
		SaveLog(cTemp,FILELOG);

		if (GameData.PlayerMode == 1)
		{
			GotoSpellXY( GameData.MeCode1, GameData.MeCode2, DestX, DestY);
		}
	}

	if( Msg == WM_POPUPMAIN)
	{
		ReadDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
		SaveLog("PopDialog",FILELOG);
		wsprintf(cTemp,"UIData.hMainConfig[0]=0x%x",UIData.hMainConfig[0]);
		SaveLog(cTemp,FILELOG);
		if (UIData.ShowMainDialog == TRUE)
		{
			//SetWindowPos(UIData.hMain, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			ShowWindow(UIData.hMain, SW_HIDE);

			SaveLog("ShowWindow HIDE",FILELOG);
			SetWindowPos(UIData.GamehWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

			ShowCursor(FALSE);
			UIData.ShowMainDialog = FALSE;
		}
		else if (UIData.ShowMainDialog == FALSE)
		{
			//SetWindowPos(UIData.hMain, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			ShowWindow(UIData.hMain, SW_SHOWNA);
			SaveLog("ShowWindow SHOW",FILELOG);
			SetWindowPos(UIData.GamehWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

			ShowCursor(TRUE);
			UIData.ShowMainDialog = TRUE;
			
		}
		WriteDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
	}

	if( Msg == WM_CLOSEMAIN )
	{
		ReadDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
		ShowWindow(UIData.hMain, SW_HIDE);
		UIData.ShowMainDialog = FALSE;
		WriteDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
		wsprintf(cTemp,"Close Config.AUTO_BLOOD = %d ",Config.AUTO_BLOOD );
		SaveLog(cTemp,FILELOG);
	}

	if( Msg == WM_COMMONDMAIN )
	{
	}

	if( Msg == WM_NOTIFYCONFIG1 )
	{
		int nPos = (int)lParam;
		Config.m_nGameSpeeder = nPos + 1;
		Config.bSpeedChanged = TRUE;
	}

	if( Msg == WM_NOTIFYCONFIG2 )
	{
		int nPos = (int)lParam;

	//	Config.Speed2 = nPos;
	//	_MainAggressiveKillTimer();
	//	_MainAggressiveSetTimer(nPos * 100);
	}

	if( Msg == WM_COMMONDCONFIG1 )
	{
		HWND hWnd = UIData.hMainConfig[0];
		switch(LOWORD(wParam))
		{	 			   
		    case IDC_CHECK_AUTO_OVER:             // ��ʦ��ǽ		
				Config.AUTO_OVER = Button_GetCheck(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_AUTO_OVER));		
				SetToGameInfo("��ʦ��ǽ", Config.AUTO_OVER);
			   	break;

		    case IDC_CHECK_AUTO_SPELL:            // �Զ���ħ��
			   	Config.AUTO_SPELL = Button_GetCheck(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_AUTO_SPELL));
			   	SetToGameInfo("����ħ��", Config.AUTO_SPELL);
			   	break;

		   	case IDC_CHECK_GAME_INFO:            // ��Ϸ��Ϣ
			   	Config.AUTO_GAME_INFO = Button_GetCheck(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_GAME_INFO));
			   	SetToGameInfo("ս����Ϣ", Config.AUTO_GAME_INFO);
			   	break;

//		    	case IDC_CHECK_AUTO_OTHER_AGGRESSIVE: // ���湥��
//			    	Config.AUTO_OTHER_AGGRESSIVE = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_OTHER_AGGRESSIVE));
//			    	break;
 
		   	case IDC_CHECK_LOCK_TARGET:
			   	Config.AUTO_LOCK_TARGET = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_LOCK_TARGET));
			   	GameData.AggressiveStatus = FALSE; // ȡ���ϴι���Ŀ�������
			   	SetToGameInfo("����Ŀ��", Config.AUTO_LOCK_TARGET);
			   	break;

		   	case IDC_RADIO_BLOOD_A:           // ��Ѫģʽ����
			   	Config.BLOOD_MODE = 0;
				SetToGameInfo("�����ָ��� A", 3);
			   	break;

		   	case IDC_RADIO_BLOOD_B:
			   	Config.BLOOD_MODE = 1;
				SetToGameInfo("�����ָ��� B", 3);
			   	break;

		    case IDC_RADIO_BLOOD_C:
			   	Config.BLOOD_MODE = 2;
				SetToGameInfo("�����ָ��� C", 3);
			   	break;
		   	case IDC_RADIO_BLOOD_D:
			   	Config.BLOOD_MODE = 3;
				SetToGameInfo("�����ָ��� D", 3);
			   	break;

		   	case IDC_RADIO_BLOOD_COMMON:
			   	Config.BLOOD_ADD_MODE = 0;
				SetToGameInfo("�������� һ��", 3);
			   	break;

		   	case IDC_RADIO_BLOOD_FULL:
			   	Config.BLOOD_ADD_MODE = 1;
				SetToGameInfo("�������� ����", 3);
			   	break;

		   	case IDC_RADIO_BLOOD_BEST:
			   	Config.BLOOD_ADD_MODE = 2;
				SetToGameInfo("�������� ��ǿ", 3);
			   	break;

		   	case IDC_RADIO_BLOOD_PK:
			   	Config.BLOOD_ADD_MODE = 3;
				SetToGameInfo("�������� PK", 3);
			   	break;

		   	case IDC_CHECK_AUTO_BUY:
			   	Config.AUTO_BUY = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_BUY));
			   	break;
//		   	case IDC_CHECK_AUTO_CONNECT:
//			   	Config.AUTO_CONNECT = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_CONNECT));
//			   	break;
///		   	case IDC_CHECK_RECV_INFO:
//		     	Config.RECV_INFO = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_RECV_INFO));
//			   	break;
 
		    case IDC_CHECK_NO_ATTACK:
			   	Config.NO_ATTACK =  Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_NO_ATTACK));
				SetToGameInfo("��������", Config.NO_ATTACK);
			   	break;

		   	case IDC_CHECK_ENERGY:              // ��������
			   	Config.AUTO_ENERGY = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_ENERGY));
				SetToGameInfo("��������", Config.AUTO_ENERGY);
			   	break;

		    case IDC_CHECK_PHYSICAL:            // �Զ�����
			   	Config.AUTO_PHYSICAL = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_PHYSICAL));
				SetToGameInfo("��������", Config.AUTO_PHYSICAL);
			   	break;

		    case IDC_CHECK_AUTO_MAX_KILL:           // ����װ��
		    	Config.AUTO_MAX_KILL = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_MAX_KILL));
				SetToGameInfo("�Զ�ʹ�ñ�ɱ", Config.AUTO_MAX_KILL);
			    break;

		   	case IDC_CHECK_BACK_CASTLE:         // �سǱ���
			   	Config.BACK_CASTLE = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_BACK_CASTLE));
				SetToGameInfo("�سǱ���",Config.BACK_CASTLE);
				if(Config.BACK_CASTLE==1){
					GameData.Blood_W=(int)(GameData.Blood_Max*0.30);
					sprintf(info,"��ǰѪ������ %d ʹ�ûس�",GameData.Blood_W);
					SendGameMessage(info, 0x03);
				}
			   	break;

		    case IDC_CHECK_BLOOD:               // �Զ���Ѫ
			   	Config.AUTO_BLOOD = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_BLOOD));
				SetToGameInfo("��������", Config.AUTO_BLOOD);
			   	break;
		    case IDC_CHECK_AUTO_MONEY:          // �Զ���Ǯ
		    	Config.AUTO_MONEY = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_MONEY));
			   	SetToGameInfo("�Զ���Ǯ", Config.AUTO_MONEY);
				break;

		    case IDC_CHECK_AUTO_NOVELITY:       // �Զ�����
		    	Config.AUTO_NOVELITY = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_NOVELITY));
			   	SetToGameInfo("�Զ�����",Config.AUTO_NOVELITY);
				break;
		   	case IDC_CHECK_AUTO_COUTERATTACK:   // �Զ�����
			   	Config.AUTO_COUTERATTACK = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_COUTERATTACK));
			   	break;
	
		   	case IDC_CHECK_OTHER_ADD_BLOOD:
		    	Config.OTHER_ADD_BLOOD = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_OTHER_ADD_BLOOD)); 

			   	SetToGameInfo("�����Ѫ", Config.OTHER_ADD_BLOOD);
			   	if (Config.OTHER_ADD_BLOOD == TRUE)
				{
			    	_OtherBloodSetTimer(Config.EDIT_OTHER_3);
				}
			   	else
				{
			    	_OtherBloodKillTimer();
				}
			   	break;

		   	case IDC_WITH_AUTO_AGGRESSIVE:
		    	Config.AUTO_OTHER_AGGRESSIVE = Button_GetCheck(GetDlgItem(hWnd, IDC_WITH_AUTO_AGGRESSIVE));
			   	break;
 
		   	case IDC_CHECK_SPELL_1:             // ����
			   	Config.Player_Mode_1 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_1));
			   	break;

		   	case IDC_CHECK_SPELL_2:  //ȭ���� 
			   	Config.Player_Mode_2 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_2));
			   	break;

			case IDC_CHECK_SPELL_3:             // ����
				Config.Player_Mode_3 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_3));
				break;

			case IDC_CHECK_SPELL_4:             // ����
				Config.Player_Mode_4 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_4));
				break;

			case IDC_CHECK_SPELL_5:             // ��
				Config.Player_Mode_5 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_5));
				break;

			case IDC_CHECK_SPELL_6:             // 
				Config.Player_Mode_6 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_6));
				break;

			case IDC_CHECK_SPELL_7:             // 
				Config.Player_Mode_7 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_7));
				break;

			case IDC_CHECK_SPELL_8:             // ����
				Config.Player_Mode_8 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_8));
				break;

			case IDC_CHECK_SPELL_9:             // �ػ�
				Config.Player_Mode_9 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_9));
				break;

			case IDC_CHECK_SPELL_10:             // 
				Config.Player_Mode_10 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_10));
				break;
					
			case IDC_CHECK_SPELL_11:            // ǿ��
				Config.Player_Mode_11 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_11));
				break;

			case IDC_CHECK_SPELL_12:            // 
				Config.Player_Mode_12 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_12));
				break;

			case IDC_CHECK_SPELL_13:            // 
				Config.Player_Mode_13 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_13));
				break;

			case IDC_CHECK_SPELL_14:            // 
				Config.Player_Mode_14 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_14));
				break;

			case IDC_CHECK_SPELL_15:            // 
				Config.Player_Mode_15 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_15));
				break;

			case IDC_CHECK_SPELL_16:            // 
				Config.Player_Mode_16 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_SPELL_16));
				break;

			case IDC_CHECK_CTRL:
				if( Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_CTRL)) == TRUE)
				{
					keybd_event( VK_CONTROL,
					  0,
					  KEYEVENTF_EXTENDEDKEY | 0,
					  0 );
					Config.AUTO_CTRL = TRUE;

				}
				else
				{
					keybd_event( VK_CONTROL,
					  0,
					  KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
					  0);
					Config.AUTO_CTRL = FALSE;
				}
				break;

			case IDC_COMBO_CONFIG1_1:
				Config.AggressiveMode1 = SendDlgItemMessage(hWnd, IDC_COMBO_CONFIG1_1, CB_GETCURSEL, 0, 0);
				break;

			case IDC_COMBO_CONFIG1_2:
				Config.AggressiveMode2 = SendDlgItemMessage(hWnd, IDC_COMBO_CONFIG1_2, CB_GETCURSEL, 0, 0);
				break;
		}
			
	}
	if( Msg == WM_COMMONDCONFIG2 )
	{
		HWND hWnd = UIData.hMainConfig[1];
		if ((LOWORD(wParam) >= IDC_COMBO1_1) && (LOWORD(wParam) <= IDC_COMBO2_3))
		{	
			if (Config.LOADDOOR == TRUE)
			{
				GetWindowText(GetDlgItem(hWnd, IDC_COMBO1_1), Config.Computer_NPC1[0], 20);						
				GetWindowText(GetDlgItem(hWnd, IDC_COMBO1_2), Config.Computer_NPC1[1], 20);			
				GetWindowText(GetDlgItem(hWnd, IDC_COMBO1_3), Config.Computer_NPC1[2], 20);			
				
				GetWindowText(GetDlgItem(hWnd, IDC_COMBO2_1), Config.Computer_NPC2[0], 20);					
				GetWindowText(GetDlgItem(hWnd, IDC_COMBO2_2), Config.Computer_NPC2[1], 20);	
				GetWindowText(GetDlgItem(hWnd, IDC_COMBO2_3), Config.Computer_NPC2[2], 20);		
//				SaveLog_(Config.Computer_NPC1[1]);
			}
		}

		if( LOWORD(wParam) == IDC_BUTTON_GOTO )
		{
		//	if (Button_GetCheck(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_AUTO_OVER)) == FALSE) 
			{
				GameData.Goto_X = (unsigned short)GetDlgItemInt(hWnd,IDC_EDIT_X,NULL,TRUE);
				GameData.Goto_Y = (unsigned short)GetDlgItemInt(hWnd,IDC_EDIT_Y,NULL,TRUE);
				if( ( GameData.Goto_X + GameData.Goto_Y )%2 != 0)
					GameData.Goto_X++;

				int nTemp1 = abs( GameData.ME_X - GameData.Goto_X ) / 20;
				if( abs( GameData.ME_X - GameData.Goto_X ) % 20 ) 
					nTemp1++;
				GameData.nGotoCounte = abs( GameData.ME_Y - GameData.Goto_Y ) / 20;
				if( abs( GameData.ME_Y - GameData.Goto_Y ) % 20 )
					GameData.nGotoCounte ++;
				if( nTemp1 > GameData.nGotoCounte )
					GameData.nGotoCounte = nTemp1;
				GotoSpellXY( GameData.MeCode1,GameData.MeCode2, (DWORD)GameData.Goto_X,(DWORD)GameData.Goto_Y);
			}
		}

		if ( LOWORD(wParam) == IDC_CHECK_AUTO_SAVE_SITE1 )
		{
			Config.AUTO_SAVE_SITE1 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_SAVE_SITE1));	
		}

		if ( LOWORD(wParam) == IDC_CHECK_AUTO_SAVE_SITE2 )
		{
			Config.AUTO_SAVE_SITE2 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_SAVE_SITE2));			
		}

		switch (LOWORD(wParam))
		{
		    case IDC_BUTTON_GOTO_1:
				// AE 00 00 00 83 01 22 4E 00 00
				break;
		    case IDC_BUTTON_GOTO_2:
				break;
		    case IDC_BUTTON_GOTO_3:
				break;
		    case IDC_BUTTON_GOTO_4:
				break;
		    case IDC_BUTTON_GOTO_5:
				break;
		    case IDC_BUTTON_GOTO_6:			
				break;
		    case IDC_BUTTON_GOTO_7:
				break;
		    case IDC_BUTTON_GOTO_8:
				break;
		}

	}
	if( Msg == WM_COMMONDCONFIG3 )
	{
		HWND hWnd = UIData.hMainConfig[2];
		switch(LOWORD(wParam))
		{
		    case IDC_RADIO_3_1:
			   	ShowWindow(UIData.hMainConfig3_1, SW_SHOW);
			   	ShowWindow(UIData.hMainConfig3_2, SW_HIDE);
			   	break;
		   	case IDC_RADIO_3_2:
			   	ShowWindow(UIData.hMainConfig3_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig3_2, SW_SHOW);
				break;
		}
	}

	if( Msg == WM_COMMONDCONFIG3_1 )
	{
		char tmp[10], tmp1[255];
		HWND hWnd = UIData.hMainConfig3_1;
		switch(LOWORD(wParam))
		{
/*			    case IDC_BUTTON_SPEAK_ADD:
					Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_SPEAK), tmp, 255);
					ListBox_AddString(GetDlgItem(hWnd, IDC_SPEAK_LIST), tmp);
					break;
				case IDC_BUTTON_SPEAK_SUB:

					t1 = ListBox_GetCurSel(GetDlgItem(hWnd, IDC_SPEAK_LIST));

					ListBox_DeleteString(GetDlgItem(hWnd, IDC_SPEAK_LIST), t1);

					break;
*/					
		    case IDC_RADIO_SPEAK_MODE_1:
			   	Config.TalkMode = 0x01;
			   	break;
			
		   	case IDC_RADIO_SPEAK_MODE_2:
			   	Config.TalkMode = 0x03;
			   	break;

		   	case IDC_CHECK_AUTO_SPEAK:	
		
			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_SPEAK), tmp1, 49);
			   	strcpy(Config.TalkBuf, tmp1);

				Config.AUTO_SPEAK = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_SPEAK)); 

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_SPEAK_TIMER), tmp, 10);
			   	Config.SPEAK_TIMER = GetNumber(tmp);
//			    	Button_Enable(GetDlgItem(hWnd, IDC_EDIT_SPEAK_TIMER), !Config.AUTO_SPEAK); 
//			    	Button_Enable(GetDlgItem(hWnd, IDC_EDIT_SPEAK), !Config.AUTO_SPEAK); 

			   	_TalkKillTimer();
			   	_TalkSetTimer(5000);

			   	break;

		   	case IDC_CHECK_AUTO_SPEAK_PK:
			   	Config.AUTO_SPEAK_PK = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_SPEAK_PK));
			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_SPEAK_PK), tmp, 255);
	
	//		    	Button_Enable(GetDlgItem(hWnd, IDC_EDIT_SPEAK_PK), !Config.AUTO_SPEAK_PK); 

			   	strcpy(Config.SPEAK_PK, tmp);
			   
			   	break;
		}
	}

	if( Msg == WM_COMMONDCONFIG3_2 )
	{
		HWND hWnd = UIData.hMainConfig3_2;
//		switch(LOWORD(wParam))
		{
			;
		}
	}

	if( Msg == WM_COMMONDCONFIG4 )
	{
		HWND hWnd = UIData.hMainConfig[3];
		switch(LOWORD(wParam)) 
		{
		    case IDC_RADIO_4_1:
			   	ShowWindow(UIData.hMainConfig4_1, SW_SHOW);
			    ShowWindow(UIData.hMainConfig4_2, SW_HIDE);
			    ShowWindow(UIData.hMainConfig4_3, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_4, SW_HIDE);
			   	break;
		    case IDC_RADIO_4_2:
			   	ShowWindow(UIData.hMainConfig4_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_2, SW_SHOW);
			   	ShowWindow(UIData.hMainConfig4_3, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_4, SW_HIDE);
			    break;
		   	case IDC_RADIO_4_3:
			   	ShowWindow(UIData.hMainConfig4_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_2, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_3, SW_SHOW);
			   	ShowWindow(UIData.hMainConfig4_4, SW_HIDE);
			    break; 
		    case IDC_RADIO_4_4:
			   	ShowWindow(UIData.hMainConfig4_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_2, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_3, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig4_4, SW_SHOW);
			    break; 
		}
	}
	if( Msg == WM_COMMONDCONFIG4_2 )
	{
		char tmp[128]; LONG t1;
		HWND hWnd = UIData.hMainConfig4_2;
		switch(LOWORD(wParam)) 
		{
		    case IDC_BUTTON_4_2_SUB:
			   	if (ListView_GetSelectedCount(GetDlgItem(hWnd, IDC_LIST_4_2_1)) >= 1)
				{
				   	t1 = ListView_GetSelectionMark(GetDlgItem(hWnd, IDC_LIST_4_2_1));
				   	ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_4_2_1), t1, 0, tmp, 128);
				   	ListView_DeleteItem(GetDlgItem(hWnd, IDC_LIST_4_2_1), t1);
				   	Novelity_setup_chain = sub_novelity_setup_obj(tmp);      
				}
				
				break;
	    	case IDC_CHECK_AUTO_SELL_SCORE:
				Config.AUTO_SELL_SCORE = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_SELL_SCORE));
					
				Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_AUTO_SELL_SCORE), tmp, 128);
				Config.SELL_SCORE = GetNumber(tmp);
				break;
		}
	}

	if( Msg == WM_COMMONDCONFIG4_3 )
	{
		HWND hWnd = UIData.hMainConfig4_3;
//		switch(LOWORD(wParam)) 
		{
			;
		}
	}

	if( Msg == WM_COMMONDCONFIG4_4 )
	{
		HWND hWnd = UIData.hMainConfig4_4;
		switch(LOWORD(wParam)) 
		{
		case 0: 
			break;
		}
	}

	if( Msg == WM_COMMONDCONFIG5 )
	{
		HWND hWnd = UIData.hMainConfig[4];
		switch(LOWORD(wParam)) 
		{
		    case IDC_RADIO_5_1:
			   	ShowWindow(UIData.hMainConfig5_1, SW_SHOW);
			   	ShowWindow(UIData.hMainConfig5_2, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig5_3, SW_HIDE);
			   	break;
		   	case IDC_RADIO_5_2:
			   	ShowWindow(UIData.hMainConfig5_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig5_2, SW_SHOW);
			   	ShowWindow(UIData.hMainConfig5_3, SW_HIDE);
			    break;
		   	case IDC_RADIO_5_3:
			   	ShowWindow(UIData.hMainConfig5_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig5_2, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig5_3, SW_SHOW);
				break;
		}
	}

	if( Msg == WM_COMMONDCONFIG5 )
	{
        char tmp[128]; LONG t1;
		HWND hWnd = UIData.hMainConfig5_1;
		switch(LOWORD(wParam)) 
		{
		   	case IDC_BUTTON_5_1_ADD:
			   	AddChum();
			   	break;
		   	case IDC_BUTTON_5_1_SUB:
			   	if (ListView_GetSelectedCount(GetDlgItem(hWnd, IDC_LIST_5_1_1)) >= 1)
				{
				   	t1 = ListView_GetSelectionMark(GetDlgItem(hWnd, IDC_LIST_5_1_1));
				   	ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_5_1_1), t1, 1, tmp, 128);
				   	ListView_DeleteItem(GetDlgItem(hWnd, IDC_LIST_5_1_1), t1);
				   	Obj_chum_chain = sub_chum_obj(tmp);      
				}		
			   	break;
		}

	}

	if( Msg == WM_COMMONDCONFIG5 )
	{
		HWND hWnd = UIData.hMainConfig5_2;
//		switch(LOWORD(wParam)) 
		{
			;
		}

	}

	if( Msg == WM_COMMONDCONFIG5 )
	{
		HWND hWnd = UIData.hMainConfig5_3;
//		switch(LOWORD(wParam)) 
		{
			;
		}

	}

	if( Msg == WM_COMMONDCONFIG6 )
	{
		HWND hWnd = UIData.hMainConfig[5];
		switch(LOWORD(wParam)) 
		{
		    case IDC_RADIO_6_1:
			   	ShowWindow(UIData.hMainConfig6_1, SW_SHOW);
			    ShowWindow(UIData.hMainConfig6_2, SW_HIDE);
			    break;
		    case IDC_RADIO_6_2:
			   	ShowWindow(UIData.hMainConfig6_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig6_2, SW_SHOW);
				break;
		}
	}

	if (Msg == WM_COMMONDCONFIG6_1)
	{
		char tmp[128]; LONG t1;
		HWND hWnd = UIData.hMainConfig6_1;
	  	switch(LOWORD(wParam)) 
		{
		    case IDC_BUTTON_GOTO_SITE:
			  	GetWindowText(GetDlgItem(hWnd, IDC_COMBO_6_1_1), tmp, 20);
			   	break;
		   	case IDC_BUTTON_OPEN_DOOR:
			   	OpenFun(0x9B);
			   	break;
		
		    case IDC_BUTTON_6_1_ADD:
			   	AddAutoSite();
			   	break;
		    case IDC_BUTTON_6_1_SUB:
		     	if (ListView_GetSelectedCount(GetDlgItem(hWnd, IDC_LIST_6_1_1)) >= 1)
				{
			    	t1 = ListView_GetSelectionMark(GetDlgItem(hWnd, IDC_LIST_6_1_1));
			    	ListView_GetItemText(GetDlgItem(hWnd, IDC_LIST_6_1_1), t1, 1, tmp, 128);
				   	ListView_DeleteItem(GetDlgItem(hWnd, IDC_LIST_6_1_1), t1);
				   	Obj_site_chain = sub_site_obj(tmp);      
				}
		 	   	break;
		   	case IDC_CHECK_START_AUTOPLAY:
			   	
			   	break;
		}
	}

	if( Msg == WM_COMMONDCONFIG7 )
	{
		HWND hWnd = UIData.hMainConfig[6];
		switch(LOWORD(wParam)) 
		{
			case IDC_BUTTON_AUTOWEAPON:

				break;
			case IDC_BUTTON_AUTOBANK:

				break;
			case IDC_BUTTON_AUTODRUG:

				break;
			case IDC_BUTTON_AUTOALL:
				break;
			case IDC_BUTTON_7_1:
				ShowWindow(UIData.GamehWnd, SW_MINIMIZE); // SW_SHOWMINIMIZED
				break;
			case IDC_BUTTON_7_2:

				break;
		}
	}

	if( Msg == WM_COMMONDCONFIG8 )
	{
//		char tmp[10];
		HWND hWnd = UIData.hMainConfig[7];
	   	switch(LOWORD(wParam)) 
		{
		   	case IDC_RADIO_8_1:
			   	ShowWindow(UIData.hMainConfig8_1, SW_SHOW);
			   	ShowWindow(UIData.hMainConfig8_2, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig8_3, SW_HIDE);
		      	break;
 
		   	case IDC_RADIO_8_2:
			   	ShowWindow(UIData.hMainConfig8_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig8_2, SW_SHOW);
			   	ShowWindow(UIData.hMainConfig8_3, SW_HIDE);
			   	break;

		   	case IDC_RADIO_8_3:
			   	ShowWindow(UIData.hMainConfig8_1, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig8_2, SW_HIDE);
			   	ShowWindow(UIData.hMainConfig8_3, SW_SHOW);
			   	break;
		}
	}

	if( Msg == WM_COMMONDCONFIG8_1 )
	{
		char tmp[10];
		HWND hWnd = UIData.hMainConfig8_1;
	   	switch(LOWORD(wParam)) 
		{		 
		    case IDC_BUTTON_8_1_1:
			    Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_1), tmp, 10);
			    Config.EDIT_8_1_1 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_2), tmp, 10);
			   	Config.EDIT_8_1_2 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_3), tmp, 10);
			   	Config.EDIT_8_1_3 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_4), tmp, 10);
			   	Config.EDIT_8_1_4 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_5), tmp, 10);
			   	Config.EDIT_8_1_5 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_6), tmp, 10);
			   	Config.EDIT_8_1_6 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_7), tmp, 10);
			   	Config.EDIT_8_1_7 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_8), tmp, 10);
			   	Config.EDIT_8_1_8 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_9), tmp, 10);
			   	Config.EDIT_8_1_9 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_10), tmp, 10);
			   	Config.EDIT_8_1_10 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_11), tmp, 10);
			   	Config.EDIT_8_1_11 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_12), tmp, 10);
			   	Config.EDIT_8_1_12 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_13), tmp, 10);
			   	Config.EDIT_8_1_13 = GetNumber(tmp);

			    Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_14), tmp, 10);
			   	Config.EDIT_8_1_14 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_15), tmp, 10);
			   	Config.EDIT_8_1_15 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_16), tmp, 10);
			   	Config.EDIT_8_1_16 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_17), tmp, 10);
			   	Config.EDIT_8_1_17 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_18), tmp, 10);
			   	Config.EDIT_8_1_18 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_19), tmp, 10);
			    Config.EDIT_8_1_19 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_20), tmp, 10);
			   	Config.EDIT_8_1_20 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_21), tmp, 10);
			   	Config.EDIT_8_1_21 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_8_1_22), tmp, 10);
			   	Config.EDIT_8_1_22 = GetNumber(tmp);
				break;
		}
	}

	if( Msg == WM_COMMONDCONFIG8_2 )
	{
		char tmp[10];
		HWND hWnd = UIData.hMainConfig8_2;
		switch(LOWORD(wParam)) 
		{
		    case IDC_BUTTON_8_2_1:

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_OTHER_1), tmp, 10);
			   	Config.EDIT_OTHER_1 = GetNumber(tmp);	
	
			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_OTHER_2), tmp, 10);
			   	Config.EDIT_OTHER_2 = GetNumber(tmp);
	
			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_OTHER_3), tmp, 10);
			   	Config.EDIT_OTHER_3 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_OTHER_4), tmp, 10);
			   	Config.EDIT_OTHER_4 = GetNumber(tmp);

			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_OTHER_5), tmp, 10);
			   	Config.EDIT_OTHER_5 = GetNumber(tmp);
		
			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_OTHER_6), tmp, 10);	
			   	Config.EDIT_OTHER_6 = GetNumber(tmp);

			   	break;
		    case IDC_COMBO_8_1_1:
			   	Config.BLOOD_USE_MODE = SendDlgItemMessage(hWnd, IDC_COMBO_8_1_1, CB_GETCURSEL, 0, 0);
			   	break;

		   	case IDC_COMBO_8_1_2:
			   	Config.SPELL_USE_MODE = SendDlgItemMessage(hWnd, IDC_COMBO_8_1_2, CB_GETCURSEL, 0, 0);
			   	break;

		   	case IDC_CHECK_8_1:
			   	Config.CHECK_8_1 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_8_1));
			   	break;

		   	case IDC_CHECK_8_2:
			   	Config.CHECK_8_2 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_8_2));
			   	break;
				
		}	
	}

	if( Msg == WM_COMMONDCONFIG8_3 )
	{
		HWND hWnd = UIData.hMainConfig8_3;
	   	switch(LOWORD(wParam)) 
		{
		   	case IDC_NOVELITY_0:	
			   	Config.AUTO_NOVELITY_0 = Button_GetCheck(GetDlgItem(hWnd, IDC_NOVELITY_0));
			   	break; 	
		   	case IDC_NOVELITY_1:
			   	Config.AUTO_NOVELITY_1 = Button_GetCheck(GetDlgItem(hWnd, IDC_NOVELITY_1));
			   	break;
		   	case IDC_NOVELITY_2:
			   	Config.AUTO_NOVELITY_2 = Button_GetCheck(GetDlgItem(hWnd, IDC_NOVELITY_2));
			   	break;	
		   	case IDC_NOVELITY_3:		
			   	Config.AUTO_NOVELITY_3 = Button_GetCheck(GetDlgItem(hWnd, IDC_NOVELITY_3));
			   	break;
		   	case IDC_CHECK_AUTO_DO_1:
			   	Config.AUTO_DO_1 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_DO_1));
			   	break;
		   	case IDC_CHECK_AUTO_DO_2:
			   	Config.AUTO_DO_2 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_DO_2));
			   	break;
		   	case IDC_CHECK_AUTO_DO_3:
		    	Config.AUTO_DO_3 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_DO_3));
			   	break;
		   	case IDC_CHECK_AUTO_DO_4:
			   	Config.AUTO_DO_4 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_DO_4));
			   	break;
			case IDC_CHECK_BANK_0:
				Config.BANK_0 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_BANK_0));	
				break;
			case IDC_CHECK_BANK_1:
				Config.BANK_1 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_BANK_1));	
				break;
			case IDC_CHECK_AUTO_OPEN_DOOR:
				Config.AUTO_OPEN_DOOR = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_AUTO_OPEN_DOOR));
				break;

		}
	}

	if( Msg == WM_COMMONDCONFIG9 )
	{
		HWND hWnd = UIData.hMainConfig[8];
		switch(LOWORD(wParam)) 
		{
			case IDC_RADIO_9_1:
				ShowWindow(UIData.hMainConfig9_1, SW_SHOW);
				ShowWindow(UIData.hMainConfig9_2, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_3, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_4, SW_HIDE);
				break;

			case IDC_RADIO_9_2:
				ShowWindow(UIData.hMainConfig9_1, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_2, SW_SHOW);
				ShowWindow(UIData.hMainConfig9_3, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_4, SW_HIDE);
				break;

			case IDC_RADIO_9_3:
				ShowWindow(UIData.hMainConfig9_1, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_2, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_3, SW_SHOW);
				ShowWindow(UIData.hMainConfig9_4, SW_HIDE);
				break;
			case IDC_RADIO_9_4:
				ShowWindow(UIData.hMainConfig9_1, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_2, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_3, SW_HIDE);
				ShowWindow(UIData.hMainConfig9_4, SW_SHOW);
				break;
		}

	}
	if( Msg == WM_COMMONDCONFIG9_1 )
	{
		char tmp[10];

		HWND hWnd = UIData.hMainConfig9_1;
	    switch(LOWORD(wParam)) 
		{
		   	case IDC_CHECK_9_1_1:
			   	Config.CHECK_9_1_1 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_9_1_1));
			   	break;
		    case IDC_CHECK_9_1_2:
			   	Config.CHECK_9_1_2 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_9_1_2));
			   	break;
		    case IDC_CHECK_9_1_3:
			   	Config.CHECK_9_1_3 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_9_1_3));
			   	break;
		   	case IDC_CHECK_9_1_4:
			   	Config.CHECK_9_1_4 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_9_1_4));
			   	break;
		   	case IDC_CHECK_9_1_5:
			   	Config.CHECK_9_1_5 = Button_GetCheck(GetDlgItem(hWnd, IDC_CHECK_9_1_5));
			   	Edit_GetText(GetDlgItem(hWnd, IDC_EDIT_9_1_1), tmp, 10);
			   	Button_Enable(GetDlgItem(hWnd, IDC_EDIT_9_1_1), !Config.CHECK_9_1_5); 
			   	Config.EDIT_9_1_1 = GetNumber(tmp);
			   	break;

		   	case IDC_COMBO_9_1_1:                     // ��ʾģʽ
			   	Config.COMBO_9_1_1 = SendDlgItemMessage(hWnd, IDC_COMBO_9_1_1, CB_GETCURSEL, 0, 0) + 1;
			   	break;
		}
	}
	return OldDefWindowProcAddressA( hwnd,Msg,wParam,lParam );
}

DWORD WINAPI NewGetLastError(VOID)
{
	DWORD dwError = OldGetLastErrorAddress();
	if( dwError == ERROR_ALREADY_EXISTS )
		dwError = 0;
	return dwError;
}

__EXPORT DWORD HooksCanUnloadNow(VOID);

VOID WINAPI NewExitProcess(UINT uExitCode)
{
	
	wsprintf(cTemp,_T("ExitProcess:  uExitCode = 0x%x"),uExitCode);
	SaveLog(cTemp,FILELOG);
	keybd_event( VK_CONTROL,
		  0,
		  KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
		  0);

	UninstallUI();
	HooksCanUnloadNow();

	WriteDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
	
	SendMessage( UIData.hMain,WM_CLOSE,NULL,NULL );
	
	OldExitProcessAddress(uExitCode);
}



HWND WINAPI NewCreateWindowEx(
			DWORD dwExStyle, 
			LPCTSTR lpClassName, 
			LPCTSTR lpWindowName, 
			DWORD dwStyle, 
			int x, 
			int y, 
			int nWidth, 
			int nHeight, 
			HWND hWndParent, 
			HMENU hMenu, 
			HINSTANCE hInstance, 
			LPVOID lpParam )
{	
	HWND hWnd = OldCreateWindowExAddr
		  (	dwExStyle,
			lpClassName, 
			lpWindowName, 
			dwStyle, 
			x, 
			y, 
			nWidth, 
			nHeight, 
			hWndParent, 
			hMenu, 
			hInstance, 
			lpParam );

//	char szTemp[512], szTemp1[8];

//	BYTE Code[8] = { 0x31, 0x54, 0x76, 0x58, 0x42, 0x22, 0x12, 0x53 };

	wsprintf(cTemp,_T("%s"),lpWindowName);
	
	if( strcmp(cTemp,GameName) == 0 )
	{
//		int i;
		//ReadData();

		ReadDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
		UIData.GamehWnd = hWnd;
		//WriteData();
		WriteDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
		wsprintf(cTemp,_T("GamehWnd :0x%x"),hWnd);
		SaveLog(cTemp,FILELOG);
		wsprintf(cTemp,_T("MainhWnd :0x%x"),UIData.hMain);
		SaveLog(cTemp,FILELOG);
		//dwoldTime = dwnewTime = OldGetTickCountAddress();
		if( IsWinNT() )
			dwNormalFeg = NORMALFREGNT;
		else
			dwNormalFeg = NORMALFREG9X;
		//SetParent(UIData.hMain,UIData.GamehWnd);
	}
	wsprintf(cTemp,_T("CreateWindow WindowText = %s"),lpWindowName);
	SaveLog(cTemp,FILELOG);
	return hWnd;
}

UINT WINAPI NewSetTimer(
    HWND hWnd ,
    UINT nIDEvent,
    UINT uElapse,
    TIMERPROC lpTimerFunc)
{
	if( uElapse == 50 )
		uElapse = 10;
	return OldSetTimerAddress(hWnd,nIDEvent,uElapse,lpTimerFunc);
}

int nGetTickCountStart = 0;
DWORD WINAPI NewGetTickCount( void )
{
	DWORD dwTime = OldGetTickCountAddress();
	//if( IsWinNT() )
	//{
		if( nGetTickCountStart == 0 )
		{
			dwoldTime = dwTime;
			nGetTickCountStart = 1;
			return dwoldTime;
		}
		dwTime = dwCurSpeeder*(dwTime - dwoldTime)/10+dwoldTime;
	//}
	return dwTime;
}

int nQueryPerformanceStart = 0;
BOOL WINAPI NewQueryPerformanceCounter(
  LARGE_INTEGER *lpPerformanceCount   // counter value
)
{
	BOOL bRes = OldQueryPerformanceCounterAddress(lpPerformanceCount);
	//if( IsWinNT() )
	//{
		if( !nQueryPerformanceStart )
		{
			dwnewTime = lpPerformanceCount->QuadPart;
			nQueryPerformanceStart = 1;
			return bRes;
		}
		lpPerformanceCount->QuadPart = dwCurSpeeder*(lpPerformanceCount->QuadPart - dwnewTime)/10+dwnewTime;
	//}
	return bRes;
}

#define NUNHOOKS 20

ADDR_CONTENTS UH1[NUNHOOKS];
ADDR_CONTENTS UH2[NUNHOOKS];
ADDR_CONTENTS UH3[NUNHOOKS];
ADDR_CONTENTS UH4[NUNHOOKS];
ADDR_CONTENTS UH5[NUNHOOKS];
ADDR_CONTENTS UH6[NUNHOOKS];
ADDR_CONTENTS UH7[NUNHOOKS];
ADDR_CONTENTS UH8[NUNHOOKS];
ADDR_CONTENTS UH9[NUNHOOKS];
ADDR_CONTENTS UH10[NUNHOOKS];
ADDR_CONTENTS UH11[NUNHOOKS];
ADDR_CONTENTS UH12[NUNHOOKS];
ADDR_CONTENTS UH13[NUNHOOKS];
ADDR_CONTENTS UH14[NUNHOOKS];
ADDR_CONTENTS UH15[NUNHOOKS];
ADDR_CONTENTS UH16[NUNHOOKS];
ADDR_CONTENTS UH17[NUNHOOKS];
ADDR_CONTENTS UH18[NUNHOOKS];
ADDR_CONTENTS UH19[NUNHOOKS];

API_UNHOOK Unhook1 = {NUNHOOKS, 0, UH1};
API_UNHOOK Unhook2 = {NUNHOOKS, 0, UH2};
API_UNHOOK Unhook3 = {NUNHOOKS, 0, UH3};
API_UNHOOK Unhook4 = {NUNHOOKS, 0, UH4};
API_UNHOOK Unhook5 = {NUNHOOKS, 0, UH5};
API_UNHOOK Unhook6 = {NUNHOOKS, 0, UH6};
API_UNHOOK Unhook7 = {NUNHOOKS, 0, UH7};
API_UNHOOK Unhook8 = {NUNHOOKS, 0, UH8};
API_UNHOOK Unhook9 = {NUNHOOKS, 0, UH9};
API_UNHOOK Unhook10 = {NUNHOOKS, 0, UH10};
API_UNHOOK Unhook11 = {NUNHOOKS, 0, UH11};
API_UNHOOK Unhook12 = {NUNHOOKS, 0, UH12};
API_UNHOOK Unhook13 = {NUNHOOKS, 0, UH13};
API_UNHOOK Unhook14 = {NUNHOOKS, 0, UH14};
API_UNHOOK Unhook15 = {NUNHOOKS, 0, UH15};
API_UNHOOK Unhook16 = {NUNHOOKS, 0, UH16};
API_UNHOOK Unhook17 = {NUNHOOKS, 0, UH17};
API_UNHOOK Unhook18 = {NUNHOOKS, 0, UH18};
API_UNHOOK Unhook19 = {NUNHOOKS, 0, UH19};


__EXPORT API_HOOK ApiHookChain[NUNHOOKS] = {
	{"WS2_32.DLL","closesocket",HOOK_OVERWRITE | HOOK_HARD, &OldclosesocketAddress, &Unhook1, Newclosesocket},
	{"WS2_32.DLL","shutdown",HOOK_OVERWRITE | HOOK_HARD, &OldshutdownAddress, &Unhook2, Newshutdown},
	{"WS2_32.DLL","connect",HOOK_OVERWRITE | HOOK_HARD, &OldconnectAddress, &Unhook3, Newconnect},
	{"WS2_32.DLL","send",HOOK_OVERWRITE | HOOK_HARD, &OldSendAddress, &Unhook4, NewSend},
	{"WS2_32.DLL","recv",HOOK_OVERWRITE | HOOK_HARD, &OldrecvAddress, &Unhook5, Newrecv},
	{"WSOCK32.DLL","closesocket",HOOK_OVERWRITE | HOOK_HARD, &OldclosesocketAddress, &Unhook1, Newclosesocket},
	{"WSOCK32.DLL","shutdown",HOOK_OVERWRITE | HOOK_HARD, &OldshutdownAddress, &Unhook2, Newshutdown},
	{"WSOCK32.DLL","connect",HOOK_OVERWRITE | HOOK_HARD, &OldconnectAddress, &Unhook3, Newconnect},
	{"WSOCK32.DLL","send",HOOK_OVERWRITE | HOOK_HARD, &OldSendAddress, &Unhook4, NewSend},
	{"WSOCK32.DLL","recv",HOOK_OVERWRITE | HOOK_HARD, &OldrecvAddress, &Unhook5, Newrecv},
	{"KERNEL32.DLL","ExitProcess",HOOK_OVERWRITE | HOOK_HARD, &OldExitProcessAddress, &Unhook7, NewExitProcess},
	{"USER32.DLL","DefWindowProcA",HOOK_OVERWRITE | HOOK_HARD , &OldDefWindowProcAddressA, &Unhook8, NewDefWindowProcA},
	{"USER32.DLL","CreateWindowExA",HOOK_OVERWRITE | HOOK_HARD, &OldCreateWindowExAddr, &Unhook10, NewCreateWindowEx},
	{"USER32.DLL","SetTimer",HOOK_OVERWRITE | HOOK_HARD, &OldSetTimerAddress, &Unhook11, NewSetTimer},
	{"KERNEL32.DLL","GetTickCount",HOOK_OVERWRITE | HOOK_HARD, &OldGetTickCountAddress, &Unhook12, NewGetTickCount},
	{"KERNEL32.DLL","QueryPerformanceCounter",HOOK_OVERWRITE | HOOK_HARD, &OldQueryPerformanceCounterAddress, &Unhook13, NewQueryPerformanceCounter},
	{HOOKS_END}
};

__EXPORT DWORD Init( DWORD cExePath )
{
	//SaveLog("Init",FILELOG);
	FILE *fp = fopen("c:\\~www.sunjz.com.tmp", "r");
	if(fp != NULL) 
	{
		fscanf(fp,"%s\r\n",GameData.IP);
		fscanf(fp,"%s\r\n",GameData.user);
		fscanf(fp,"%s\r\n",GameData.password);
		fclose(fp);

	}
	fp = fopen("c:\\~www.sunjz.com.tmp", "w");
	if(fp != NULL ){
		fclose(fp);
	}
	
	return 1;
};

__EXPORT DWORD HooksCanUnloadNow(VOID) 
{
   DWORD i;
   DWORD ErrorUnload = 0;
   HANDLE ModuleExport;

   for(i=0; i<NUNHOOKS-1; i++)
     if((ApiHookChain[i].dwFlags & HOOK_EXPORT) &&
        (ModuleExport = GetModuleHandle(ApiHookChain[i].ModuleExport))) {
       if((bIsNT) ||
          (ApiHookChain[i].dwFlags & HOOK_HARD) ||
          ((int)ModuleExport > 0)) {
         ErrorUnload = 1;
         break;
       }
     }
     else
       if(ApiHookChain[i].dwFlags & (HOOK_BY_ADDRESS | HOOK_BY_ADDRESS))
         ErrorUnload = 1;

   if(!UnhookApis(ApiHookChain, WHOLE_AH_CHAIN))
   {
	 SaveLog("ApiHooks Unload Failed",FILELOG);
     return(1);
   }
   else {
     //free heap memory
     for(i=1; i<NUNHOOKS-1; i++)
       if((ApiHookChain[i].dwFlags & (HOOK_OVERWRITE | HOOK_RAW))
          && *(HLOCAL*)(ApiHookChain[i].ModuleImport))
         *(HLOCAL*)(ApiHookChain[i].ModuleImport) = LocalFree(*(HLOCAL*)ApiHookChain[i].ModuleImport);
	 SaveLog("ApiHooks Unload Success",FILELOG);
     return(ErrorUnload);
   }
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		hMapFile = CreateMapFile();
		hInstance = (HINSTANCE)hModule;
//		InitalizeApplication(hInstance);
		wsprintf(cTemp,"ApiHookDLL load Success,Instance = 0x%x",hInstance);
		SaveLog(cTemp,FILELOG);
		Init(NULL);
		
		if((int)GetVersion() < 0) //is 9x?
			if((int)hModule > 0)    //then I must be >= 2GB
			return FALSE;
	}
	if(ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		CloseHandle( hMapFile );
		SaveLog("ApiHookDLL Unload Success",FILELOG);
	}
	return TRUE;
}

// ������������
int WINAPI analysesend(SOCKET s, char *buf, int len)
{
	int i,index=0;
//#ifdef _DEBUG
	char szTemp[15120];

	char speak[1512],lpTarget[1512],lpSource[1512];
	int nSpeedTemp;
	BYTE code;
	int strlen;
//	SaveSendRecvLog("����ǰ���:",(char*)buf,len,"C:\\send.log");

	if ((BYTE(buf[0]) == 0x00) && (BYTE(buf[1]) == 0x80))
	{
		HMODULE  module = 0;
        module = GetModuleHandle("XTrapVa.dll");
		sendlogin(buf);
		Config.RUNED = FALSE;
		Config.PLAYING = FALSE;
		_StayKillTimer();
		_SpellKillTimer();
		_TalkKillTimer();
	}



	if ((BYTE(buf[0]) == 0xAA) && (BYTE(buf[1]) == 0x55) && (BYTE(buf[4]) == 0xF9))
	{	
		for (i=0; i<len - 7; i++)	
		{
			lpSource[i] = BYTE(buf[5 + i]);
		}
		Encode_Decode((LPBYTE)lpTarget, (LPBYTE)lpSource, len - 7);
		GameData.MYSENDTime1=lpTarget[0];
		GameData.MYSENDTime2=lpTarget[1];
		GameData.MYSENDTime3=lpTarget[2];
		GameData.MYSENDTime4=lpTarget[3];


		switch ((BYTE)lpTarget[4]){
			case 0x85:
				//SaveSendRecvLog("����:",(char*)lpTarget,len-7,"C:\\buy.log");
			//	buf[9+5]=(BYTE)79;
			//	buf[9+6]=(BYTE)3;
			//	SaveSendRecvLog("����:",(char*)buf,len,"C:\\buy.log");
			//	SendGameMessage("������", 0x03);
				break;
			case 0x6f: //������
				if (Config.AUTO_LOCK_TARGET  == TRUE){
					if((GameData.lockID[0]!=lpTarget[5])||(GameData.lockID[1]!=lpTarget[6])){
						GameData.lockID[0]=lpTarget[5];
						GameData.lockID[1]=lpTarget[6];
						sprintf(szTemp,"����Ŀ�� %d",GameData.lockID[0]+GameData.lockID[1]*256);
						SendGameMessage(szTemp, 0x03);
					}
					
				}
				break;
			case 0x6d: //ħ������
				if (Config.AUTO_LOCK_TARGET  == TRUE){
					code=lpTarget[9];
					if(code==2||code==3||code==4||
							code==5||code==8||code==10)
					{
						if((GameData.Spell_lockID[0]!=lpTarget[5])||(GameData.Spell_lockID[1]!=lpTarget[6])){
							GameData.Spell_lockID[0]=lpTarget[5];
							GameData.Spell_lockID[1]=lpTarget[6];
							GameData.Spell_code=lpTarget[9];
							sprintf(szTemp,"����Ŀ�� %d",GameData.Spell_lockID[0]+GameData.Spell_lockID[1]*256);
							SendGameMessage(szTemp, 0x03);
						
						}
					}
				}
				break;
			case 0x1e: //˵��
				for (i=0; i<(int)lpTarget[6]; i++)
				{
					speak[i] = (char)lpTarget[7 + i];
				}
				speak[i]=0;
				if (TFCommand(speak, (int)lpTarget[6]) == 0) 
					return 0;
				break;
			case 0xE9:
				GameData.MYSENDTime1 = lpTarget[0];
				GameData.MYSENDTime2 = lpTarget[1];
				GameData.MYSENDTime3 = lpTarget[2];
				GameData.MYSENDTime4 = lpTarget[3];
				break;
			case 0x0E:
				GameData.MYSENDTime1 = lpTarget[0];
				GameData.MYSENDTime2 = lpTarget[1];
				GameData.MYSENDTime3 = lpTarget[2];
				GameData.MYSENDTime4 = lpTarget[3];
				//index=5;
				//strlen=GetByte((char*)lpTarget,index);
				//_GetString(GameData.PlayerAcount,(char*)lpTarget,strlen,index);
			//	GetShort((char*)lpTarget,index);
				//strlen=GetByte((char*)lpTarget,index);
				////_GetString(GameData.PlayerAcount,(char*)lpTarget,strlen,index);
				//GameData.PlayerAcount[strlen]='\0';
				//for(i=0;i<strlen;i++) GameData.PlayerAcount1[i]=(GameData.PlayerAcount[i])^((char)(0xaa+i));
				//GameData.PlayerAcount_len=strlen;
				//server_code.Send_User(GameData.PlayerAcount1,GameData.PlayerAcount_len);
				//_KillAuthenticationTimer();
				//_SetAuthenticationTimer(10*1000);
				break;
			case 0x2D: //����
				break;
			default:
			
				break;
		}
	}
	

		return len;
}

// ������������
// ������Կ
int WINAPI analyserecv(SOCKET s, char *buf, int len)
{ 
	int i, n;
	int NowLen; 
	n = len;


	if ((BYTE(buf[4]) == 0xD6)) //&& (BYTE(buf[13]) != 0x00))
	{	
		for (i=0; i<8; i++)
		{
			/*if (UIData.RUNED) */G_BYTE_XorTable[i] = BYTE(buf[5 + i]) ^ G_BYTE_DOII_XorTable[i];//UIData.XorTable[i];G_BYTE_DOII_XorTable[i];
		}

		if (Config.RUNED != TRUE) 
		{
			ReadDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));
			InstallPKware(UIData.szExePath);
			Config.RUNED = TRUE;

		}
		return n;

	}
	int plen=0;
	int slen,index;
//	SaveSendRecvLog("00:",(char*)(buf),len,"C:\\niuniu3.log");
	while(plen<len){
		//SaveSendRecvLog("11:",(char*)(buf+plen+2),2,"C:\\niuniu3.log");
		index=2;
		slen=GetShort(buf+plen,index)+6;
	//	SaveSendRecvLog("22:",(char*)(buf+plen),slen,"C:\\niuniu3.log");
		if ((BYTE(buf[plen+0]) == 0xAA) && (BYTE(buf[plen+1]) == 0x55) && (BYTE(buf[plen+4]) == 0xF9))
		{
			NowLen = 2;
			Encode_Recv(buf+plen, NowLen);
			plen=plen+ *(short *)(buf+plen+2)+6;
		}else
			break;
	}
    
	return n;
}


void Encode_Decode(LPBYTE lpTarget, LPBYTE lpSource, WORD nLen)
{
	memcpy(lpTarget,lpSource,nLen);
	return ;
	/*
    //                     8   * 0x009d
	BYTE	l_BYTE_Xor1 = nLen * D_WORD_XOR1MUL, l_BYTE_Xor3;
	//                     0x086d
	WORD	l_WORD_Xor3 = D_WORD_XOR3BASE, nCir;

	for (nCir=0; nCir<nLen; nCir++) 
	{
		l_BYTE_Xor3    = HIBYTE(l_WORD_Xor3);
		lpTarget[nCir] = lpSource[nCir] ^ l_BYTE_Xor1 ^ G_BYTE_XorTable[nCir % D_XORTABLE_LENGTH] ^ l_BYTE_Xor3;
		l_WORD_Xor3   *= D_WORD_XOR3MUL;
	}
	*/
}
void RegisterUser(LPBYTE lpTarget)
{
	int i, n;

	char *RegisterUserInfo;

	if (lpTarget[4] == 0x0E)
	{
		//ReadData();

		for(i =0; i<(int)lpTarget[5]; i++)
		{
			GameData.NamePlayerInfo[i] = (char)lpTarget[6 + i];
		}
		strcat(GameData.PlayerInfoPath, UIData.szExePath);
		strcat(GameData.PlayerInfoPath, GameData.NamePlayerInfo);
		strcat(GameData.PlayerInfoPath, ".ini");


		n = 6 + (int)lpTarget[5] + 3;

	
		RegisterUserInfo = (char*)malloc((int)lpTarget[n-1]);

		for(i = 0; i<(int)lpTarget[n-1];i++)
		{
			RegisterUserInfo[i] = (char)lpTarget[n+i];
		}
	}
		
}

void UseSpell(BYTE Code1, BYTE Code2, BYTE data)
{
    // 00 01 02 03 04 05 06 07 08 09
	// BB 00 00 00 6D 69 6C 00 00 02 ���
	// C8 00 00 00 6D 39 2D 00 00 03 ����
	// F0 00 00 00 6D D8 6B 00 00 04 ����
	// 0C 01 00 00 6D E3 6B 00 00 05 ����
	// 2A 01 00 00 6D 39 2D 00 00 06 ����
	// 51 02 00 00 6D 39 2D 00 00 07 B9 01 00 00 C9 05 00 00 ˲��
	// 8D 01 00 00 6D D8 6B 00 00 08 ����
	// 6B 01 00 00 6D 39 2D 00 00 0D ��ջ���
	// A7 01 00 00 6D 39 2D 00 00 0E ����ǿ��
	// 30 02 00 00 6D 39 2D 00 00 0F �񱩹���
	// 22 02 00 00 6D 39 2D 00 00 11 ����籩
	// 58 01 00 00 6D 39 2D 00 00 12 �����ػ�
	// 7D 16 00 00 8D 02 00 01 00 07 00 ѧϰ˲��ħ�� ����
	// 0A 00 00 00 6D 3F 27 00 00 13 ħ��

	char tmp[100];

	char lpTarget[100];
	char lpSource[100];

//	SendGameMessage("�Զ�ʹ��ħ��", 0x02);

	lpSource[0] = GameData.MYSENDTime1;
    lpSource[1] = GameData.MYSENDTime2;
    lpSource[2] = GameData.MYSENDTime3;
    lpSource[3] = GameData.MYSENDTime4;
    lpSource[4] = 0x6D;
    lpSource[5] = Code1;
    lpSource[6] = Code2;

    lpSource[7] = 0x00;
    lpSource[8] = 0x00;
    lpSource[9] = data;

	Encode_Decode((LPBYTE)lpTarget, (LPBYTE)lpSource, 10);


	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x0B;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����

	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10]  = (char)lpTarget[5];
	tmp[11]  = (char)lpTarget[6];
	tmp[12]  = (char)lpTarget[7];
	tmp[13]  = (char)lpTarget[8];
	tmp[14]  = (char)lpTarget[9];

	tmp[15] = (char)0x55;
	tmp[16] = (char)0xAA;
	
	

	OldSendAddress(GameData.s,tmp,17,0);

}

void Aggressive(BYTE Code1, BYTE Code2)
{
	char *tmp;

	LPBYTE lpTarget;
	LPBYTE lpSource;

	lpTarget =  (LPBYTE)malloc(7);
	lpSource =  (LPBYTE)malloc(7);

	lpSource[0] = GameData.MYSENDTime1;
    lpSource[1] = GameData.MYSENDTime2;
    lpSource[2] = GameData.MYSENDTime3;
    lpSource[3] = GameData.MYSENDTime4;
    lpSource[4] = 0x6F;
    lpSource[5] = Code1;
    lpSource[6] = Code2;

	Encode_Decode(lpTarget, lpSource, 7);

	tmp =  (char*)malloc(14);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x08;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];
	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10] = (char)lpTarget[5];
	tmp[11] = (char)lpTarget[6];

	tmp[12] = (char)0x55;
	tmp[13] = (char)0xAA;


	OldSendAddress(GameData.s,tmp,14,0);
	free(tmp);
}
//˲��
void GotoSpellXY(BYTE Code1,BYTE Code2, int X, int Y)
{
	char *tmp;
	BYTE newx1, newx2, newy1, newy2;

	LPBYTE lpTarget;
	LPBYTE lpSource;

	newx1 = (X % 255) - ((X - (X % 255)) / 255); 

	newx2 = (X - (X % 255)) / 255;

	newy1 = (Y % 255) - ((Y - (Y % 255)) / 255);

	newy2 = (Y - (Y % 255)) / 255;


	lpTarget =  (LPBYTE)malloc(18);
	lpSource =  (LPBYTE)malloc(18);

	//  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17
	//	67 00 00 00 6D DC 28 00 00 07 59 03 00 00 B1 01 00 00
	lpSource[0] = 0x00;
	lpSource[1] = 0x00;
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
	//dwSendIndex++;
	//memcpy(lpSource,&dwSendIndex,4);
    lpSource[4] = 0x6D;
    lpSource[5]  = Code1;    
    lpSource[6]  = Code2;
    lpSource[7]  = 0x00;
    lpSource[8]  = 0x00;
    lpSource[9]  = 0x07;
    lpSource[10] = newx1;
    lpSource[11] = newx2;
    lpSource[12] = 0x00;
    lpSource[13] = 0x00;
    lpSource[14] = newy1;
    lpSource[15] = newy2;
    lpSource[16] = 0x00;
    lpSource[17] = 0x00;


	Encode_Decode(lpTarget, lpSource, 18);

	tmp =  (char*)malloc(25);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x13;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	memcpy(tmp+5,lpTarget,18);
	tmp[23] = (char)0x55;
	tmp[24] = (char)0xAA;

	OldSendAddress(GameData.s,tmp,25,0);

	free(lpTarget);
	free(lpSource);	
	free(tmp);

}
void Encode_Recv(char *buf, int NowLen)
{
	int i =0;
	char lpTarget[60480];
	
	char lpSource[60480];
	
	int t1 = (int)((BYTE)(buf[NowLen]));
	int t2 = (int)((BYTE)buf[NowLen + 1]);
	int t3 = t1+t2*255 +t2 - 1;

	for (i=0; i<t3; i++)	
	{			
		lpSource[i] = buf[NowLen + 3 + i];	
	}
	SaveSendRecvLog("����ǰ���:",(char*)buf,t3+7,"C:\\recv.log");
	::WritePrivateProfileString("AUTO","����ǰ���",(char*)buf,"c:\\ddd.ini");

    //  ��ʼ����
	Encode_Decode((LPBYTE)lpTarget, (LPBYTE)lpSource, t3);
	// �������ܺ������
	SaveSendRecvLog("���ܺ���:",(char*)lpTarget,t3,"C:\\recv.log");
	::WritePrivateProfileString("AUTO","���ܺ���",(char*)lpTarget,"c:\\ddd.ini");
	TF((LPBYTE)lpTarget, t3);
//	free(lpTarget);
//	free(lpSource);
}


int FindNextTarget()
{
	int p;

	p = search_space_obj();

	if (p != MAXPLAYER)
	{
		GameData.AggressiveCode1 = Obj_info_chain[p].Code1;
		GameData.AggressiveCode2 = Obj_info_chain[p].Code2;
		GameData.AggressiveX     = Obj_info_chain[p].X;
		GameData.AggressiveY     = Obj_info_chain[p].Y;

		strcpy(GameData.AggressiveStr, Obj_info_chain[p].Name);

		return 0;
	}

	return 1;
}
void SetNovelityNumber(LPBYTE lpTarget, int nLen)
{   

}

int TFPKOBJData(char *buf, int nLen)
{
	int i,z;

	char info[256];
//	struct SITE_CHAIN *p1, *q1;
		
	SelectPlayerMode();
//	SaveSendRecvLog("Recv\r\n",buf,nLen,"c:\\niuniu1.log");
	int index=6;
	_GetString(GameData.PlayerName,buf,buf[5],index);//����
	SaveLog(GameData.PlayerName,FILELOG);
	GetShort(buf,index); //����
	GetShort(buf,index); //����
	GetShort(buf,index); //����
	GetShort(buf,index); //�ǻ�
	GetShort(buf,index); //����
	GetShort(buf,index); //����λ

	GetDWORD(buf,index);//������ص�
	GetDWORD(buf,index);//������ص�
	GetByte(buf,index);//������ص�
	_GetString(info,buf,10,index);//������ص�
	GetDWORD(buf,index);//���顣
	GetDWORD(buf,index);//ս������

	GetShort(buf,index); //δ�����Ե�
	GetShort(buf,index);//δ��λ
	GetShort(buf,index);//δ��λ

	GetShort(buf,index);//���HP
	GetShort(buf,index);//����HP
	GetShort(buf,index);//���MP
	GetShort(buf,index);//����MP
	GetShort(buf,index);//�������
	GetShort(buf,index);//��������

	GetDWORD(buf,index);//MONEY
	GetShort(buf,index);//����ȼ�

	GameData.PlayerLevel = GetShort(buf,index); //�ȼ�
	GameData.PlayerMode = GetByte(buf,index);//ְҵ
//	sprintf(info,"�ȼ� %d, ְҵ���� %d .\r\n ",GameData.PlayerLevel,GameData.PlayerMode);
//	SaveLog(info,FILELOG);
	_GetString(info,buf,20*3,index);//����1
	_GetString(info,buf,12*3,index);//����2
	int spell=(int)GetByte(buf,index);
//	sprintf(info,"ħ������%d:",spell);
//	SaveLog(info,FILELOG);
	for (z=1; z <= spell; z++)
	{

		int code=(int)GetShort(buf,index);
		GetByte(buf,index);
		sprintf(info,"code: %d",code);
		SaveLog(info,FILELOG);
		switch (code)
		{
			case 0x00: //�����ظ�
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_13), TRUE); 
				break;
			case 0x01://ȭ����
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_2), TRUE); 
				break;
			case 0x02:  // ���
				break;
			case 0x03:  // ����
				break;
			case 0x04:  // ����
				break;
			case 0x05:  // ����
				break;
			case 0x06:  // ����
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_8), TRUE); // ����
				break;
			case 0x07:  // ˲��
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_14), TRUE); // ˲��
				break;
			case 0x08:  // ����
				break;
			case 0x09://����
				break;
			case 0x0A://��֮��
				break;
			case 0x0B://����
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_3), TRUE);
				break;
			case 0x0C://����
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_1), TRUE);
				break;
			case 0x0D:  // ��ջ���
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_4), TRUE);
				break;
			case 0x0E:  // ����ǿ��
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_11), TRUE);
				break;
			case 0x0F:  // �񱩹���
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_5), TRUE);
				break;
			case 0x10:
				break;
			case 0x11:  // ����籩
				break;
			case 0x12:  // �����ػ�
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_9), TRUE);
				break;
			case 0x13:  // ħ��
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_10), TRUE);
				break;
			case 0x14://����
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_6), TRUE);
				break;
			case 0x15:
				break;
			case 0x16:
				break;
			case 0x17:
				break;
			case 0x18:
				break;
			case 0x19:
				break;
			case 0x1A:
				break;
			case 0x1B:
				break;
			case 0x1C:
				break;
			case 0x1D:
				break;
			case 0x1E:  // ��������
				Button_Enable(GetDlgItem(UIData.hMainConfig[0], IDC_CHECK_SPELL_15), TRUE);
				break;
		}
	}

	for(i=0; i<39; i++)
	{
		GameData.Novelity39[i].Code[0] = (BYTE)i;
		_GetString((char *)(GameData.Novelity39[i].Code+1),buf,22,index);
	//	SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[i].Code+1),22,"c:\\send1111.log");

	}


	for (i =0; i< 150; i++)
	{
			GameData.Novelity150[i].Code[3] = 0xFF;
			GameData.Novelity150[i].Code[4] = 0xFF;
	}


	for (i =0; i< 48; i++)
	{
		GameData.Novelity48[i].Code[3] = 0xFF;
		GameData.Novelity48[i].Code[4] = 0xFF;
	}
	SendDlgItemMessage(UIData.hMainConfig4_1, IDC_COMBO_4_1_1, CB_SETCURSEL, 2, 0);

	if (!Config.PLAYING) 
	{
		Config.PLAYING = TRUE;
		Parse_Novelity(0);
		_SpellKillTimer();
		_SpellSetTimer(1000);
		
		_TalkKillTimer();
		if(Config.SPEAK_TIMER!=0)
			_TalkSetTimer(Config.SPEAK_TIMER * 1000);

		_MainAggressiveKillTimer();
		_MainAggressiveSetTimer(2 * 100);

		_OtherAggressiveKillTimer();
		_OtherAggressiveSetTimer(100);
		_KillGotoSpellTimer();//��������
		_SetGotoSiteTimer(200);//1���һ��Ѫ



	}
	return 0;
}
/*************************************

**************************************/
void Parse_Novelity(int a)
{
	int i;
	BYTE code1,code2;
	
	GameData.Energy_Now=*(short *)((GameData.Novelity39[4].Code)+7);
	int n =*(short *)((GameData.Novelity39[4].Code)+9);
	//��������������
	if(a!=0){
		code1=GameData.Novelity39[a].Code[3];
		code2=GameData.Novelity39[a].Code[4];
//		SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[a].Code),23,"C:\\send111.log");
		if(code1==(BYTE)28 && code2==(BYTE)0){//����A
			GameData.Blood_N[0]=a;
		}
		if(code1==(BYTE)29 && code2==(BYTE)0){//����B
			GameData.Blood_N[1]=a;
		}		
		if(code1==(BYTE)30 && code2==(BYTE)0){//����C
			GameData.Blood_N[2]=a;
		}
		if(code1==(BYTE)226 && code2==(BYTE)3){//����C
			GameData.Blood_N[3]=a;
		}

		if(code1==(BYTE)31 && code2==(BYTE)0){//����A
			GameData.Spell_N[0]=a;
		}
		if(code1==(BYTE)32 && code2==(BYTE)0){//����B
			GameData.Spell_N[1]=a;
		}
		if(code1==(BYTE)33 && code2==(BYTE)0){//����C
			GameData.Spell_N[2]=a;
		}
		if(code1==(BYTE)227 && code2==(BYTE)3){//����D
			GameData.Spell_N[3]=a;
		}
		
		if(code1==(BYTE)39 && code2==(BYTE)0){//����A
			GameData.Spell_N[4]=a;

		}
		if(code1==(BYTE)57 && code2==(BYTE)0){//����B
			GameData.Spell_N[5]=a;

		}
		if(code1==(BYTE)34 && code2==(BYTE)0){//ѹ������A
			GameData.Physical_N[0]=a;
		}
		if(code1==(BYTE)56 && code2==(BYTE)0){//ѹ������B
			GameData.Physical_N[1]=a;

		}
		if(code1==(BYTE)67 && code2==(BYTE)0){//ѹ������C
			GameData.Physical_N[2]=a;
		}
		if(code1==(BYTE)41 && code2==(BYTE)0){//����������λ��
			GameData.EnergyWeapon_Now1=a;
		}
		if(code1==(BYTE)42 && code2==(BYTE)0){//ǹ��������λ��
			GameData.EnergyWeapon_Now2=a;
		}
		if(code1==(BYTE)85 && code2==(BYTE)0){
			GameData.Return_Now=a;
		}
		if(code1==(BYTE)0xff && code2==(BYTE)0xff){//һ������û�ˡ����һ���ǲ���ʹ�õĶ���
			for(i=0;i<6;i++){
				if(GameData.Blood_N[i]==a)
					GameData.Blood_N[i]=0;
			}
			for(i=0;i<6;i++){
				if(GameData.Spell_N[i]==a)
					GameData.Spell_N[i]=0;
			}
			for(i=0;i<6;i++){
				if(GameData.Physical_N[i]==a)
				GameData.Physical_N[i]=0;
			}
			if(GameData.EnergyWeapon_Now1==a)
				GameData.EnergyWeapon_Now1=0;

			if(GameData.EnergyWeapon_Now2==a)
				GameData.EnergyWeapon_Now2=0;
			if(GameData.Return_Now==a)
				GameData.Return_Now=0;

		}
		
		char tmp[200];
#ifdef _DEBUG
	sprintf(tmp,"λ�� %d %d %d %d - %d %d %d %d %d %d - %d %d %d - %d %d - %d",GameData.Blood_N[0],
		GameData.Blood_N[1],GameData.Blood_N[2],GameData.Blood_N[3],GameData.Spell_N[0],
		GameData.Spell_N[1],GameData.Spell_N[2],GameData.Spell_N[3],GameData.Spell_N[4],GameData.Spell_N[5],
		GameData.Physical_N[0],GameData.Physical_N[1],GameData.Physical_N[2],GameData.EnergyWeapon_Now1,GameData.EnergyWeapon_Now2,GameData.Return_Now);
	SendGameMessage(tmp, 0x02);
#endif

		return ;
	}
	
	for(i=0;i<6;i++){
		GameData.Blood_N[i]=0;
	}
	for(i=0;i<6;i++){
		GameData.Spell_N[i]=0;
	}
	for(i=0;i<6;i++){
		GameData.Physical_N[i]=0;
	}

	GameData.EnergyWeapon_Now1=0;
	GameData.EnergyWeapon_Now2=0;

	for(i=10; i<34; i++)
	{
		code1=GameData.Novelity39[i].Code[3];
		code2=GameData.Novelity39[i].Code[4];
//		SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[i].Code),23,"C:\\send111.log");
		if(code1==(BYTE)28 && code2==(BYTE)0){//����A
			GameData.Blood_N[0]=i;
		}
		if(code1==(BYTE)29 && code2==(BYTE)0){//����B
			GameData.Blood_N[1]=i;
		}		
		if(code1==(BYTE)30 && code2==(BYTE)0){//����C
			GameData.Blood_N[2]=i;
		}
		if(code1==(BYTE)226 && code2==(BYTE)3){//����C
			GameData.Blood_N[3]=i;
		}

		if(code1==(BYTE)31 && code2==(BYTE)0){//����A
			GameData.Spell_N[0]=i;
		}
		if(code1==(BYTE)32 && code2==(BYTE)0){//����B
			GameData.Spell_N[1]=i;
		}
		if(code1==(BYTE)33 && code2==(BYTE)0){//����C
			GameData.Spell_N[2]=i;
		}
		if(code1==(BYTE)227 && code2==(BYTE)3){//����D
			GameData.Spell_N[3]=i;
		}
		
		if(code1==(BYTE)39 && code2==(BYTE)0){//����A
			GameData.Spell_N[4]=i;

		}
		if(code1==(BYTE)57 && code2==(BYTE)0){//����B
			GameData.Spell_N[5]=i;

		}
		if(code1==(BYTE)34 && code2==(BYTE)0){//ѹ������A
			GameData.Physical_N[0]=i;
		}
		if(code1==(BYTE)56 && code2==(BYTE)0){//ѹ������B
			GameData.Physical_N[1]=i;

		}
		if(code1==(BYTE)67 && code2==(BYTE)0){//ѹ������C
			GameData.Physical_N[2]=i;
		}
		if(code1==(BYTE)41 && code2==(BYTE)0){//����������λ��
			GameData.EnergyWeapon_Now1=i;
		}
		if(code1==(BYTE)42 && code2==(BYTE)0){//ǹ��������λ��
			GameData.EnergyWeapon_Now2=i;
		}
		if(code1==(BYTE)85 && code2==(BYTE)0){
			GameData.Return_Now=i;
		}
	}
#ifdef _DEBUG	
	char tmp[200];
	sprintf(tmp,"λ�� %d %d %d %d - %d %d %d %d %d %d - %d %d %d - %d %d -%d ",GameData.Blood_N[0],
		GameData.Blood_N[1],GameData.Blood_N[2],GameData.Blood_N[3],GameData.Spell_N[0],
		GameData.Spell_N[1],GameData.Spell_N[2],GameData.Spell_N[3],GameData.Spell_N[4],GameData.Spell_N[5],
		GameData.Physical_N[0],GameData.Physical_N[1],GameData.Physical_N[2],GameData.EnergyWeapon_Now1,GameData.EnergyWeapon_Now2,GameData.Return_Now);
	SendGameMessage(tmp, 0x02);
#endif

}
int TFOBJData(char *buf, int nLen)
{

	return 0;
}

// PKware ��ѹ
void TFPKware(LPBYTE lpTarget, int nLen)
{
	int i, t1, t2;
	char lpPKware[20480];

//	char szTemp[128];

	t1 = (int)lpTarget[5] + (int)lpTarget[6] + (int)lpTarget[6] * 255;
	t2 = (int)lpTarget[7] + (int)lpTarget[8] + (int)lpTarget[8] * 255; 

	char info[199];
	sprintf(info,"t1= %d,t2= %d\r\n",t1,t2);
	SaveLog(info,"c:\\niuniu2.log");

	for (i=0; i<t1; i++)
	{	
		lpPKware[i] = (char)lpTarget[i+15]; 
	}


	
	if(t2<1500){
		PKware_Explode(lpPKware, t1, t2);
		TFPkData(GameData.StrPKWARE, GameData.PKLen);
	//	SaveSendRecvLog("Recv\r\n",GameData.StrPKWARE,GameData.PKLen,SENDRECVLOG);
	free(GameData.StrPKWARE);
	}
	//Add_Player_info();
	
}
// ����Pkware ��ѹ��õ����ַ���
int TFPkData(char *buf, int nLen)
{
	int NowLen;


	NowLen = 0;

	for(;;)
	{
		if (NowLen == 0)
		{
			NowLen = 2;
			TFZipData(buf, NowLen);
		}
		else
		{
			if ((NowLen + (int)(BYTE)buf[NowLen - 2] + (int)(BYTE)buf[NowLen - 1] * 255 + 
				(int)(BYTE)buf[NowLen - 1]) + 2 >= nLen)
			{
				return 0;
			}
			else
			{
				NowLen = NowLen + (int)(BYTE)buf[NowLen - 2] + (int)(BYTE)buf[NowLen - 1] * 255 + 
				(int)(BYTE)buf[NowLen - 1] + 2; 
				TFZipData(buf, NowLen);
			}
		}
			
	}

	return 0;
}
void TFZipData(char *buf, int NowLen)
{
	int i, t1;
	char tmp[10000];

	t1 = (int)(BYTE)buf[NowLen - 2] + (int)(BYTE)buf[NowLen - 1] * 255 + (int)(BYTE)buf[NowLen - 1] + 2;

	for (i=0; i<t1; i++)
	{
		tmp[i+2] = buf[NowLen - 2 + i];
	}
	// ��pkware �������ݷ���
	TF((LPBYTE)tmp, t1);

	free(tmp);
}


int TFCommand(char *buf, int len)
{
	char fn[10],info[100];
	int i;

//	SendGameMessage(buf, 0x02);
	if(len<7) return 1;
	if(buf[0]=='/' && buf[1]=='r' && buf[2]=='e' && buf[3]=='t' && buf[4]==' '){

		for(i=0;i<4;i++){
			if(5+i >=len)
				break;
			if(buf[5+i]==' ' ||buf[5+i]=='\0')
				break;
			fn[i]=buf[5+i];
		}
		fn[i]='\0';
		GameData.Blood_W=atoi(fn);
		sprintf(info,"��ǰѪ������ %d ʹ�ûس�",GameData.Blood_W);
		SendGameMessage(info, 0x03);
	}
	return 1;
}
 
BOOL TFGetXYAggressive()
{
	int p;

	p = search_code_obj(GameData.AggressiveCode1, GameData.AggressiveCode2);

	if (p == MAXPLAYER) return FALSE;

	if (GameData.PlayerMode == 0)
	{
		if ((abs((GameData.AggressiveX - GameData.ME_X)) <= 4)  
			&& (abs((GameData.AggressiveY - GameData.ME_Y)) <= 4))
		
			return TRUE;		
		else GameData.AggressiveStatus = FALSE;
	}

	if (GameData.PlayerMode == 1)
	{
		if (Config.AggressiveMode1 == 1)
		{
			if ((abs((GameData.AggressiveX - GameData.ME_X)) <= 4)  
				&& (abs((GameData.AggressiveY - GameData.ME_Y)) <= 4))		
				return TRUE;
			else GameData.AggressiveStatus = FALSE;
		}
		else
		{
			if ((abs((GameData.AggressiveX - GameData.ME_X)) <= 8)  
				&& (abs((GameData.AggressiveY - GameData.ME_Y)) <= 8))	
				return TRUE;
			else GameData.AggressiveStatus = FALSE;
		}
	}
	else if (GameData.PlayerMode == 2)
	{
		if ((abs((GameData.AggressiveX - GameData.ME_X)) <= 4)  
			&& (abs((GameData.AggressiveY - GameData.ME_Y)) <= 4))	
			return TRUE;
		else GameData.AggressiveStatus = FALSE;
	}
	else if (GameData.PlayerMode == 3)
	{
		if ((abs((GameData.AggressiveX - GameData.ME_X)) <= 8)  
			&& (abs((GameData.AggressiveY - GameData.ME_Y)) <= 8))
			return TRUE;
		else GameData.AggressiveStatus = FALSE;
	}

	return FALSE;
}
// �жϽ��ղ���
void TF(LPBYTE lpTarget, int nLen)
{
	int i;
	BYTE code1,code2;
	char tmp[128];
	char info[256];
	int index=0,ServerID,len,n,count,res,x,y,ID;
	int sSid,sCount,tIQ;


	switch(lpTarget[4]){
		case 0x5b:
		case 0x5d:
		case 0x61:
		case 0x63://�ƶ�
	//	SaveSendRecvLog("�ƶ�:",(char*)lpTarget,nLen,"C:\\niuniu1.log");
		index=6;
		ServerID=GetDWORD((char*)lpTarget,index);
		if(ServerID==GameData.ServerID){
			if(lpTarget[4]==0x63 ||lpTarget[4]==0x61){
				GetShort((char*)lpTarget,index);
				GetShort((char*)lpTarget,index);
				x=GetShort((char*)lpTarget,index);
				y=GetShort((char*)lpTarget,index);
			//	sprintf(info,"��·����x=%d,y=%d",x,y);
			//	SendGameMessage(info, 0x02);
			}else{
				x=GetShort((char*)lpTarget,index);
				y=GetShort((char*)lpTarget,index);
			//	sprintf(info,"��·����x=%d,y=%d",x,y);
			//	SendGameMessage(info, 0x02);
			}
			GameData.x=x;
			GameData.y=y;
		}
		break;
		case 0x2:
			TFPKOBJData((char *)lpTarget, nLen);
			break;
		case 0xc1:
			TFPKware(lpTarget, nLen);
			break;
		case  0x6b:// �����ж����ڻ��ж���
			index=5;
			GameData.Blood_Now =  GetShort((char*)lpTarget,index);

//			sprintf(info,"��������ֵ���� %d",GameData.Blood_Now);
//			SendGameMessage(info, 0x02);
			break;
		case 0x66: // �ж��������ж���
			index=5;
			GameData.Physical_Now = GetShort((char*)lpTarget,index);
//			sprintf(info,"��������ֵ���� %d",GameData.Physical_Now);
//			SendGameMessage(info, 0x02);
			break;
		case 0x2b:// �жϽ������ж���
			index=6;
			GameData.Energy_Now  = GetShort((char*)lpTarget,index);

		//	sprintf(info,"��������������%d",GameData.Energy_Now);
		//	SendGameMessage(info, 0x02);
			break;
		case 0x6C://ħ��ֵ�ж����ڻ��ж���
			index=5;
			GameData.Spell_Now = GetShort((char*)lpTarget,index);
//			sprintf(info,"����ħ��ֵ %d",GameData.Spell_Now);
//			SendGameMessage(info, 0x02);
			break;
		case 0x4f://�������ʼ�����������ֵ....
			GameData.Blood_Now=GameData.Blood_Max= *(short*)(lpTarget+23);
			GameData.Spell_Now=GameData.Spell_Max=  *(short*)(lpTarget+25);;
			GameData.Physical_Now=GameData.Physical_Max=  *(short*)(lpTarget+27);
//			sprintf(info,"��������ֵ���� %d",GameData.Physical_Now);
//			SendGameMessage(info, 0x02);
//			sprintf(info,"��������ֵ���� %d",GameData.Blood_Now);
//			SendGameMessage(info, 0x02);
//			sprintf(info,"����ħ��ֵ %d",GameData.Spell_Now);
//			SendGameMessage(info, 0x02);
			break;
		case 0x38:
			if(lpTarget[5]==0)
				return ;
			index=7;
			if(lpTarget[6]==1){//sp
				GameData.Physical_Now= *(short*)(lpTarget+7);
				//sprintf(info,"��������ֵ���� %d",GameData.Physical_Now);
				//SendGameMessage(info, 0x02);
				index=15;
			}
			if(lpTarget[6]==2){//pp 
				GameData.Spell_Now=*(short*)(lpTarget+11);
				//sprintf(info,"����ħ��ֵ %d",GameData.Spell_Now);
				//SendGameMessage(info, 0x02);
				index=15;
			}

			if(lpTarget[6]==4){//hp
				GameData.Blood_Now=*(short*)(lpTarget+9);
			//	sprintf(info,"��������ֵ���� %d",GameData.Blood_Now);
				//SendGameMessage(info, 0x02);
				index=15;
			}
			if(lpTarget[6]==7){
				GameData.Physical_Now=*(short*)(lpTarget+7);
				GameData.Blood_Now=*(short*)(lpTarget+9);
				GameData.Spell_Now=*(short*)(lpTarget+11);
			//	sprintf(info,"��������ֵ���� %d",GameData.Physical_Now);
			//	SendGameMessage(info, 0x02);
			//	sprintf(info,"��������ֵ���� %d",GameData.Blood_Now);
			//	SendGameMessage(info, 0x02);
			///	sprintf(info,"����ħ��ֵ %d",GameData.Spell_Now);
			//	SendGameMessage(info, 0x02);
				index=15;
			}
			len=GetByte((char *)lpTarget,index);
			_GetString((char *)(GameData.Novelity39[len].Code+1),(char *)lpTarget,22,index);
			 Parse_Novelity(len);
//			SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[len].Code+1),22,"C:\\send.log");

			break;
		case 0x36: //�ƶ���Ʒ
			index=5;
			if(GetByte((char *)lpTarget,index)==0)
				break;
			GetByte((char *)lpTarget,index);
			count=GetByte((char *)lpTarget,index);
			for(i=0;i<count;i++){
				n=GetByte((char *)lpTarget,index);
				_GetString((char *)(GameData.Novelity39[n].Code+1),(char *)lpTarget,22,index);
			//	SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[n].Code+1),22,"C:\\11send.log");
				Parse_Novelity(n);
			}
			break;
		case 0x2E://����
			index=5;
			if(GetByte((char *)lpTarget,index)==0)
				break;
			GetByte((char *)lpTarget,index);
			n=GetByte((char *)lpTarget,index);
			_GetString((char *)(GameData.Novelity39[n].Code+1),(char *)lpTarget,22,index);
		//	SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[n].Code+1),22,"C:\\11send.log");
			Parse_Novelity(n);
			break;
		case 0x30://�Ӷ���
			index=5;
			GetByte((char *)lpTarget,index);
			n=GetByte((char *)lpTarget,index);
			_GetString((char *)(GameData.Novelity39[n].Code+1),(char *)lpTarget,22,index);
			//SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[n].Code+1),22,"C:\\11send.log");
			Parse_Novelity(n);
			break;
		case 0x91: //���д�ȡ����
			index=5;
			if(GetByte((char *)lpTarget,index)==0)
				break;
			GetByte((char *)lpTarget,index);
		//	GetDWORD((char *)lpTarget,index);
			_GetString((char *)(info),(char *)lpTarget,22,index);
			n=GetByte((char *)lpTarget,index);
			_GetString((char *)(GameData.Novelity39[n].Code+1),(char *)lpTarget,22,index);
			Parse_Novelity(n);
		//	SaveSendRecvLog("װ������:\r\n",(char *)(GameData.Novelity39[n].Code+1),22,"C:\\11send.log");
			break;
		case 0xcf:
			index=5;
			res=GetByte((char *)lpTarget,index);
			GetDWORD((char *)lpTarget,index);
			if(res==0)
				break;
			else if (res==1){//�涫��
				GetByte((char *)lpTarget,index);
				_GetString((char *)(info),(char *)lpTarget,22,index);
				n=GetByte((char *)lpTarget,index);
				_GetString((char *)(GameData.Novelity39[n].Code+1),(char *)lpTarget,22,index);
				Parse_Novelity(n);	
			}else{//ȡ����
				count=GetShort((char *)lpTarget,index);
				for(i=0;i<count;i++){
					n=GetByte((char *)lpTarget,index);
					_GetString((char *)(GameData.Novelity39[n].Code+1),(char *)lpTarget,22,index);
					Parse_Novelity(n);
				}
			}
			break;
		case 0x3a://���³�һ�����Լ�Ķ���
			index=7;
			res=GetByte((char *)lpTarget,index);
		//	sprintf(info,"res=%d",res);
		//	SendGameMessage(info, 0x02);
			if(res==3) break;
			if(res==1){//��Ʒ����
				x=GetShort((char *)lpTarget,index);
				y=GetShort((char *)lpTarget,index);
				sSid=GetShort((char *)lpTarget,index);
				sCount=GetDWORD((char *)lpTarget,index);
				tIQ=GetByte((char *)lpTarget,index);
				
				if(sSid==10000 && Config.AUTO_MONEY==0)  break;
				if((sSid!=10000) && Config.AUTO_NOVELITY==0)  break;
			//	sprintf(info,"��������Ʒx=%d,y=%d,sSid=%d,����=%d,tIQ=%d,x=%d,y=%d,absx=%d,absy= %d",x,y,sSid,sCount,tIQ,GameData.x,GameData.y,abs(GameData.x-x),abs(GameData.y-y));			
			//	SendGameMessage(info, 0x02);
				if((abs(GameData.x-x)<=10) && (abs(GameData.y-y)<=10))
					GetNovelity(x,y);
			}
			break;
		case 0x70: //������û�й����Լ�����
			index=5;
			res=GetByte((char *)lpTarget,index);
			ID = GetDWORD((char *)lpTarget,index);
			ServerID=GetDWORD((char *)lpTarget,index);
			//sprintf(info,"���ߵ�ID %d ������ID %d",ID,ServerID);
		//	SendGameMessage(info, 0x02);
			if(Config.AUTO_COUTERATTACK==1){
				if(GameData.ServerID==ServerID){
					if(GameData.PlayerMode!=1){
						if(GameData.lockID[0]==0 && GameData.lockID[1]==0){
							GameData.lockID[0]=ID%256;
							GameData.lockID[1]=ID/256;
						}
					}else{
						if(GameData.Spell_lockID[0]==0 && GameData.Spell_lockID[1]==0){
							GameData.Spell_lockID[0]=ID%256;
							GameData.Spell_lockID[1]=ID/256;
						}
					}
				}
			}
			break;
		case 0xf:
			index=5;
			res=GetByte((char *)lpTarget,index);
			if(res!=1) break;
			GameData.z=GetShort((char *)lpTarget,index);
			GameData.ServerID=GetDWORD((char * )lpTarget,index);
			GameData.x=GetShort((char *)lpTarget,index);
			GameData.y=GetShort((char *)lpTarget,index);
		//	sprintf(info,"�Լ���ServerID %d x= %d ,y= %d ,z= %d, �˺�= %s ",GameData.ServerID,GameData.x,GameData.y,GameData.z,GameData.PlayerAcount);
		//	SendGameMessage(info, 0x02);
			break;
		case 0x1:
			index=5;
			//SaveSendRecvLog("������Ϣ:",(char*)lpTarget,nLen,"C:\\niuniu2.log");
			if(GetByte((char *)lpTarget,index)==2)
				break;
			ServerID=GetDWORD((char *)lpTarget,index);
			len=GetByte((char *)lpTarget,index);
			_GetString(tmp,(char *)lpTarget,len,index);
			tmp[len]='\0';
			//sprintf(info,"���յ� %s ��Ϣ���� %d",tmp,len);
		//	SendGameMessage(info, 0x02);

			x=GetShort((char *)lpTarget,index); //����X
			y=GetShort((char *)lpTarget,index); //����Y
			GetDWORD((char *)lpTarget,index);//������ص�
			GetDWORD((char *)lpTarget,index);//������ص�
			GetByte((char *)lpTarget,index);//������ص�
			_GetString(tmp,(char *)lpTarget,10,index);//������ص�
			for(i = 0; i < 10; i++)
				GetShort((char *)lpTarget,index);//װ��Code

			if(ServerID==GameData.ServerID){
				GameData.Blood_Now=GetShort((char *)lpTarget,index);
				GameData.Blood_Max=GetShort((char *)lpTarget,index);
				GameData.x=x;
				GameData.y=y;
		//		sprintf(info,"��������x=%d,y=%d",GameData.x,GameData.y);
			//	SendGameMessage(info, 0x02);
				GetByte((char *)lpTarget,index);
				_GetString(GameData.stats,(char *)lpTarget,8,index);
#ifdef _DEBUG
				sprintf(info,"ȡ�����״̬:%x,%x,%x,%x,%x,%x,%x,%x",GameData.stats[0],
					GameData.stats[1],GameData.stats[2],GameData.stats[3],GameData.stats[4],
					GameData.stats[5],GameData.stats[6],GameData.stats[7]);
				SendGameMessage(info, 0x02);
#endif
			}
			break;
		case 113: //�������NPC ������
			code1=lpTarget[5];
			code2=lpTarget[6];
			if(GameData.lockID[0]==code1 &&GameData.lockID[1]==code2){//������Ŀ���Ѿ�����
				GameData.lockID[0]=0x00;
				GameData.lockID[1]=0x00;
				SendGameMessage("������Ŀ��������", 0x03);
			}
			if(GameData.Spell_lockID[0]==code1 &&GameData.Spell_lockID[1]==code2){
				GameData.Spell_lockID[0]=0x00;
				GameData.Spell_lockID[1]=0x00;
			//	GameData.Spell_code=0;
				SendGameMessage("������Ŀ��������", 0x03);
			}
			break;
			case 117://ȡ�����״̬
				int index=5;
				ServerID=GetDWORD((char *)lpTarget,index);
				if(ServerID==GameData.ServerID){
					GameData.stats[0]=lpTarget[9];
					GameData.stats[1]=lpTarget[10];
					GameData.stats[2]=lpTarget[11];
					GameData.stats[3]=lpTarget[12];
					GameData.stats[4]=lpTarget[13];
					GameData.stats[5]=lpTarget[14];
					GameData.stats[6]=lpTarget[15];
					GameData.stats[7]=lpTarget[16];
#ifdef _DEBUG
					sprintf(info,"ȡ�����״̬:%x,%x,%x,%x,%x,%x,%x,%x",GameData.stats[0],
						GameData.stats[1],GameData.stats[2],GameData.stats[3],GameData.stats[4],
						GameData.stats[5],GameData.stats[6],GameData.stats[7]);
					SendGameMessage(info, 0x01);
#endif
				}
			break;
	}
	
}
void Sell(LPBYTE Se, int Number)
{
}

void BuyWeapon(BYTE NPC, BYTE Code, int Number)
{   // 00 01 02 03 04 05 06 07 08 09 10 11 12
	// 53 00 00 00 85 00 00 01 00 29 00 14 00
	// 9A 02 00 00 85 0F 00 01 00 29 00 03 00
	char *tmp;

	BYTE a1, a2;

	a1 = Number % 255;
	a2 = (Number - a1) / 255;

	LPBYTE lpTarget;
	LPBYTE lpSource;


	lpTarget =  (LPBYTE)malloc(13);
	lpSource =  (LPBYTE)malloc(13);

	lpSource[0] = 0x00;
	lpSource[1] = 0x00;
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
    lpSource[4] = 0x85;
	lpSource[5] = NPC;
    lpSource[6] = 0x00;
	lpSource[7] = 0x01;
    lpSource[8] = 0x00;
	lpSource[9] = Code;
    lpSource[10] = 0x00;
	lpSource[11] = a1;
    lpSource[12] = a2;

	Encode_Decode(lpTarget, lpSource, 13);

	tmp =  (char*)malloc(20);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x0E;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����

	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10]  = (char)lpTarget[5];
	tmp[11]  = (char)lpTarget[6];
	tmp[12]  = (char)lpTarget[7];
	tmp[13]  = (char)lpTarget[8];
	tmp[14]  = (char)lpTarget[9];
	tmp[15]  = (char)lpTarget[10];
	tmp[16]  = (char)lpTarget[11];
	tmp[17]  = (char)lpTarget[12];

	tmp[18] = (char)0x55;
	tmp[19] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, 20);

	OldSendAddress(GameData.s,tmp,20,0);

	free(lpTarget);
	free(lpSource);
	free(tmp);
}

void Remedy(BYTE NPC, BYTE Code)
{   // 00 01 02 03 04 05 06 07 08 09
	// 30 00 00 00 83 02 0A 00 05 00
	// 4B 00 00 00 83 02 93 01 09 00
	// 4C 02 00 00 83 02 1D 00 02 00
	char *tmp;

	LPBYTE lpTarget;
	LPBYTE lpSource;

	lpTarget =  (LPBYTE)malloc(10);
	lpSource =  (LPBYTE)malloc(10);

	lpSource[0] = 0x00;
	lpSource[1] = 0x00;
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
    lpSource[4] = 0x83;
    lpSource[5] = 0x02;
    lpSource[6] = Code;

	if (Code == 0x93)
	{
		lpSource[7] = 0x01;
		lpSource[8] = 0x09; 
		lpSource[9] = 0x00;
	}
	else
	{
		lpSource[7] = 0x00;
		lpSource[8] = NPC; 
		lpSource[9] = 0x00;
	}

	Encode_Decode(lpTarget, lpSource, 10);

	tmp =  (char*)malloc(17);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x0B;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����

	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10]  = (char)lpTarget[5];
	tmp[11]  = (char)lpTarget[6];
	tmp[12]  = (char)lpTarget[7];
	tmp[13]  = (char)lpTarget[8];
	tmp[14]  = (char)lpTarget[9];

	tmp[15] = (char)0x55;
	tmp[16] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, 17);

	OldSendAddress(GameData.s,tmp,17,0);

	free(lpTarget);
	free(lpSource);
	free(tmp);
}

void SetToGameInfo(char tmp1[64], int Status)
{
	char tmp2[6];
	char info[256];

	if (Status == 1)
	{	
		strcpy(tmp2, "��");	
		wsprintf(info, "%s = %s", 
		tmp1, tmp2);

		SendGameMessage(info, 0x02);
	}
	else if (Status == 0)
	{
		strcpy(tmp2, "��");
		wsprintf(info, "%s = %s", 
		tmp1, tmp2);

		SendGameMessage(info, 0x02);
	}
	else if (Status == 3)
	{
		wsprintf(info, "%s", 
		tmp1);

		SendGameMessage(info, 0x02);
	}
}

void SendGameMessage(char *info, BYTE mode)
{

	char lpTarget[1280],lpSource[1280],tmp[128];
	int len=strlen(info);
	
	lpSource[0] = 0x00;	
	lpSource[1] = 0x00;	
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
	lpSource[4] = 0x05; 
	lpSource[5] = mode;    
	lpSource[6] = (BYTE)len;
	for(int i=0; i<(int)len; i++)
	{
		lpSource[7 + i] = (BYTE)info[i];
	}
		
	Encode_Decode((LPBYTE)lpTarget, (LPBYTE)lpSource, strlen(info) + 7);
	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)((BYTE)(len + 7 + 1));
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
			
	for (i=0; i < (int)len + 7; i++)
	{
				tmp[5+i] = (char)lpTarget[i];
	}
		
	tmp[len + 7 + 7 - 2] = (char)0x55;
	tmp[len + 7 + 7 - 1] = (char)0xAA;


	if(GameData.SystemInfo== TRUE){
		memcpy(GameData.SystemInfoStr+GameData.SystemLen,tmp,len + 7 + 7);
		GameData.SystemLen=GameData.SystemLen+len + 7 + 7;

	}else{
		memcpy(GameData.SystemInfoStr,tmp,len + 7 + 7);
		GameData.SystemLen=len + 7 + 7;
		GameData.SystemInfoMode    = mode;
		GameData.SystemInfo        = TRUE;
	}

	
}
void OpenSell(BYTE Code)
{

	char *tmp;
    // 00 01 02 03 04 05 06 07 08 09
    // 29 00 00 00 83 02 14 00 03 00 
	// 1D 00 00 00 83 02 03 00 03 00
	// 35 00 00 00 83 02 93 01 04 00
	LPBYTE lpTarget;
	LPBYTE lpSource;

	lpTarget =  (LPBYTE)malloc(10);
	lpSource =  (LPBYTE)malloc(10);

	lpSource[0] = 0x00;
	lpSource[1] = 0x00;
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
    lpSource[4] = 0x83;
    lpSource[5] = 0x02;
    lpSource[6] = Code;

	if (Code == 0x93)
	{
		lpSource[7] = 0x01;
		lpSource[8] = 0x04;
		lpSource[9] = 0x00;
	}
	else
	{
		lpSource[7] = 0x00;
		lpSource[8] = 0x03;
		lpSource[9] = 0x00;
	}


	Encode_Decode(lpTarget, lpSource, 10);

	tmp =  (char*)malloc(17);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x0B;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����

	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10] = (char)lpTarget[5];
	tmp[11] = (char)lpTarget[6];
	tmp[12] = (char)lpTarget[7];
	tmp[13] = (char)lpTarget[8];
	tmp[14] = (char)lpTarget[9];

	tmp[15] = (char)0x55;
	tmp[16] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, 17);

	OldSendAddress(GameData.s,tmp,17,0);

	free(lpTarget);
	free(lpSource);
	free(tmp);
	
}

void Repair(LPBYTE Mode, int Number)
{
	int i, n;

	char *tmp;
    //                            01    02    03    04    05
	// 76 01 00 00 8B 00 00 05 00 00 00 01 00 04 00 08 00 09 00
	// 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19


	LPBYTE lpTarget;
	LPBYTE lpSource;

	lpTarget =  (LPBYTE)malloc(Number * 2 + 9);
	lpSource =  (LPBYTE)malloc(Number * 2 + 9);

	lpSource[0] = 0x00;
	lpSource[1] = 0x00;
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
    lpSource[4] = 0x8B;
    lpSource[5] = 0x00; 
    lpSource[6] = 0x00;
    lpSource[7] = (BYTE)Number;
	lpSource[8] = 0x00;

	n = 0;

	for(i=0; i<Number; i++)
	{
		lpSource[9 + n]     = Mode[i];
		lpSource[9 + n + 1] = 0x00;

		n = n + 2;
	}

	Encode_Decode(lpTarget, lpSource, Number * 2 + 9);

	tmp =  (char*)malloc(Number * 2 + 9 + 7);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55; 
	tmp[2]  = (char)(BYTE)(Number * 2 + 9 + 7 - 6);
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;

	for(i=0;i<Number * 2 + 11;i++)
	{
		tmp[5 + i] = (char)lpTarget[i];
	}

	tmp[Number * 2 + 9 + 7 - 2] = (char)0x55;
	tmp[Number * 2 + 9 + 7 - 1] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, Number * 2 + 9 + 7);

	OldSendAddress(GameData.s,tmp,Number * 2 + 9 + 7,0);

	free(lpTarget);
	free(lpSource);
	free(tmp);
}

void OpenFun(BYTE Code)
{
	char *tmp;

//	03 00 00 00 9B


	LPBYTE lpTarget;
	LPBYTE lpSource;


	lpTarget =  (LPBYTE)malloc(5);
	lpSource =  (LPBYTE)malloc(5);

	lpSource[0] = 0x00;
	lpSource[1] = 0x00;
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
    lpSource[4] = Code;

	Encode_Decode(lpTarget, lpSource, 5);

	tmp =  (char*)malloc(12);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x06;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����

	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];


	tmp[10] = (char)0x55;
	tmp[11] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, 12);

	OldSendAddress(GameData.s,tmp,12,0);

	free(lpTarget);
	free(lpSource);
	free(tmp);
}



STATUS GetDoorStatus(BYTE Code)
{

	STATUS Status;
	int i;

	for(i=0; i<MAXMAP; i++)
	{
		if (MapData[i].Now == Code)
		{
			Status.Door = MapData[i].Door;
			Status.Mode = MapData[i].Mode; 
			return Status;
		}
	}
	
	Status.Door = TRUE;
	Status.Mode = 0;
	return Status;
}

int GetNovelityStatus(BYTE Code1, BYTE Code2, int Score, BYTE Mode, BYTE Color)
{
	int i;
	NOVELITY_SETUP_CHAIN *c, *d;
	// ����ǸĹ���װ��������ɫ��װ���Ͳ�����
	//{0x29, 0x00,"���⽣������"     ,1,   0,   0},
	//{0x2A, 0x00,"����ǹ������"     ,1,   0,   0},

	for (i=0; i< MAXFILTER; i++)
	{
		if ((Code1 == NovelityFilter[i].Code1) && (Code2 == NovelityFilter[i].Code2) && (NovelityFilter[i].Mode1 == 1))
		{
			return 0;			
		}
	}

	c = NULL;
	c = search_novelity_setup_obj(Novelity_setup_chain, GetNovelitytName(Code1, Code2), &d);

	if (c != NULL)
	{
		if (c->Mode == 1) return 1;
		if (c->Mode == 0) return 0;
	}

	if (GameData.PlayerMode == 2)
	{
		if ((Code1 == 0x29) && (Code2 == 0x00)) return 0;
	}

	if (GameData.PlayerMode == 3)
	{
		if ((Code1 == 0x2A) && (Code2 == 0x00)) return 0;
	}

	if (Mode > 0) return 0;
	if ((int)Color != 0) 
	{
		if (Config.AUTO_SELL_SCORE)
		{
			if (Score <= Config.SELL_SCORE)
			{
				return 1;
			}
		}
		return 0;
	}

	return 1; // ���������δ���� ��ô����������Ʒ
}

char *GetMeNow(BYTE Code)
{
	int i;

	for(i=0; i<MAXMAP; i++)
	{
		if (MapData[i].Now == Code)
		{
			return MapData[i].Name;
		}
	}
	
	return "δ֪�ص�";
}
// ��Ѫ��Ʒ����ַ  �� 0A  ~   22
void UseItem(BYTE Now)
{
	char *tmp;

	LPBYTE lpTarget;
	LPBYTE lpSource;

	lpTarget =  (LPBYTE)malloc(7);
	lpSource =  (LPBYTE)malloc(7);

	lpSource[0] = 0x00;
    lpSource[1] = 0x00;
    lpSource[2] = 0x00;    
    lpSource[3] = 0x00;
    lpSource[4] = 0x37;
    lpSource[5] = 0x01;
    lpSource[6] = Now;

	Encode_Decode(lpTarget, lpSource, 7);

	tmp =  (char*)malloc(14);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x08;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����
	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10] = (char)lpTarget[5];
	tmp[11] = (char)lpTarget[6];

	tmp[12] = (char)0x55;
	tmp[13] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, 14);

	OldSendAddress(GameData.s,tmp,14,0);
	
	free(lpTarget);
	free(lpSource);
	free(tmp);
}



void OpenDrug(BYTE Code)
{
	
}

// �Զ���Ǯ
void GetNovelity(int x, int y)
{
	char tmp[100];


	char lpTarget[100];
	char lpSource[100];


	lpSource[0] = GameData.MYSENDTime1;
    lpSource[1] = GameData.MYSENDTime2;
    lpSource[2] = GameData.MYSENDTime3;
    lpSource[3] = GameData.MYSENDTime4;
    lpSource[4] = 0x2D;
    lpSource[5] = 0x02;
    lpSource[6] = (BYTE)(x%256);
	lpSource[7] = (BYTE)(x/256);

	lpSource[8] = (BYTE)(y%256);
	lpSource[9] = (BYTE)(y/256);

	Encode_Decode((LPBYTE)lpTarget, (LPBYTE)lpSource, 10);


	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x0B;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����
	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10] = (char)lpTarget[5];
	tmp[11] = (char)lpTarget[6];
	tmp[12] = (char)lpTarget[7];
	tmp[13] = (char)lpTarget[8];
	tmp[14] = (char)lpTarget[9];
	tmp[15] = (char)0x55;
	tmp[16] = (char)0xAA;

	OldSendAddress(GameData.s,tmp,17,0);

}
void UserNovelity(BYTE mode,BYTE now)//ʹ����Ʒ
{
	
	char tmp[100];

	char lpTarget[100];
	char lpSource[100];


	lpSource[0] = GameData.MYSENDTime1;
    lpSource[1] = GameData.MYSENDTime2;
    lpSource[2] = GameData.MYSENDTime3;
    lpSource[3] = GameData.MYSENDTime4;
    lpSource[4] = 0x37;
    lpSource[5] = mode;
    lpSource[6] = now;

	Encode_Decode((LPBYTE)lpTarget,(LPBYTE)lpSource, 7);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x08;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����
	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10]  = (char)lpTarget[5];
	tmp[11]  = (char)lpTarget[6];
	tmp[12] = (char)0x55;
	tmp[13] = (char)0xAA;
	

	OldSendAddress(GameData.s,tmp,14,0);
}
// �Զ���������
void Speak(BYTE mode, char *buf)
{
	char *tmp;

	int i;

	LPBYTE lpTarget;
	LPBYTE lpSource;

	lpTarget = (LPBYTE)malloc(strlen(buf) + 7);
	lpSource = (LPBYTE)malloc(strlen(buf) + 7);

	lpSource[0] = GameData.MYSENDTime1;
    lpSource[1] = GameData.MYSENDTime2;
    lpSource[2] = GameData.MYSENDTime3;
    lpSource[3] = GameData.MYSENDTime4;

    lpSource[4] = 0x1E;
    lpSource[5] = mode;    
    lpSource[6] = (BYTE)strlen(buf);

	for(i=0; i<(int)strlen(buf); i++)
	{
		lpSource[7 + i] = (BYTE)buf[i];
	}

	Encode_Decode(lpTarget, lpSource, strlen(buf) + 7);

	tmp = (char*)malloc(strlen(buf) + 7 + 7);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)((BYTE)(strlen(buf) + 7 + 1));
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;

	for (i =0; i < (int)strlen(buf) + 7; i++)
	{
		tmp[5+i] = (char)lpTarget[i];
	}

	tmp[strlen(buf) + 7 + 7 - 2] = (char)0x55;
	tmp[strlen(buf) + 7 + 7 - 1] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, strlen(buf) + 7 + 7);

	OldSendAddress(GameData.s,tmp,strlen(buf) + 7 + 7,0);

	free(lpTarget);
	free(lpSource);
	free(tmp);
	//79 00 00 00 1E 01 1A 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41;
}

void CALLBACK _GotoSiteTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	int i,lost,per,Blood_B,Blood_Add=0,n,now;
	char info[1000];
	
	if(Config.AUTO_SPELL==1 &&GameData.Spell_Now<60)
	{//����ħ��
		for(i=0;i<6;i++){
			if(GameData.Spell_N[i]!=0)
				break;
		}
		if(i>=6){
			SendGameMessage("����δЯ������ָ���,��سǲ���", 0x04);
		}else{
			now=GameData.Spell_N[i];
			if(now<10 || now >=34){
				SendGameMessage("��������λ��", 0x02);
			}
			n=*(short *)((GameData.Novelity39[now].Code)+9);
			if(n<=1){
				GameData.Spell_N[i]=0;
			//	SendGameMessage("1��λ�þ����þ���", 0x02);
			}
			UserNovelity((BYTE)0x01,(BYTE)(now));
			
		}
	}
	if(Config.AUTO_PHYSICAL==1 &&GameData.Physical_Now<10)
	{//��������
		for(i=0;i<3;i++){
			if(GameData.Physical_N[i]!=0)
				break;
		}
		
		if(i>=3){
			SendGameMessage("����δЯ�������ָ���,��سǲ���", 0x04);
		}else{
			now=GameData.Physical_N[i];
			if(now<10 || now >=34){
				SendGameMessage("��������λ��", 0x02);
			}
			n=*(short *)((GameData.Novelity39[now].Code)+9);
			if(n<=1){
				GameData.Spell_N[i]=0;
			//	SendGameMessage("1��λ�������þ���", 0x02);
			}
			UserNovelity((BYTE)0x01,(BYTE)(now));
		}
	}

	
	if(Config.AUTO_ENERGY==1){
		if(GameData.Energy_Now<=1){
			if(GameData.PlayerMode==2)
				now=GameData.EnergyWeapon_Now1;//��
			else
				now=GameData.EnergyWeapon_Now2;//��
			if(now==0){
				SendGameMessage("�������þ�,�뾡��سǲ���", 0x04);
			}else{
				if(now<10 || now >=34){
					SendGameMessage("����������λ��", 0x02);
				}
				UserNovelity((BYTE)0x01,(BYTE)(now));
				GameData.Energy_Now=250;
				GameData.Novelity39[4].Code[7]=0xfa;
				GameData.Novelity39[4].Code[8]=0x00;
				n=*(short *)((GameData.Novelity39[now].Code)+9);
				if(n<1){
				//	SendGameMessage("�������ù�", 0x02);
					if(GameData.PlayerMode==2)
						GameData.EnergyWeapon_Now1=0;
					else
						GameData.EnergyWeapon_Now2=0;
				}
			}
		}

	}

	int ret=0;
	lost=GameData.Blood_Max-GameData.Blood_Now;
	if(Config.BACK_CASTLE==1){
		if(GameData.Return_Now==0){
			SendGameMessage("δЯ��ʱ��ҩˮ,��سǹ���", 0x04);
			GameData.Return_Now=-1;
		}else{
			if(GameData.Return_Now<10 || GameData.Return_Now >=34)
				ret=0;
			else
				ret=1;
		}
		if((GameData.Blood_Now<=GameData.Blood_W) && ret==1){
			SendGameMessage("������ֵ����ʹ�ûسǱ���", 0x04);
			UserNovelity((BYTE)0x01,(BYTE)(GameData.Return_Now));
		}
	}
	if(lost>0 && Config.AUTO_BLOOD==1){
		now=GameData.Blood_N[Config.BLOOD_MODE];
		if(now==0){
			SendGameMessage("�����ָ������þ�,�سǱ���", 0x04);
			//�سǱ���
			if(ret==1)
				UserNovelity((BYTE)0x01,(BYTE)(GameData.Return_Now));
			else{//ʹ�ô����Ż�����
				;
			}
			return ;
		}
		if(now<10 || now >=34){
			SendGameMessage("���������ָ�ҩλ��", 0x02);
		}
		n=*(short *)((GameData.Novelity39[now].Code)+9);
		//sprintf(info,"����ʧ %d ����,���ڻ�������ҩ���� %d",lost,n);
	//	SendGameMessage(info, 0x02);
		
		if(n<=10){
			SendGameMessage("�����ָ�ҩ��������,��سǹ���~!", 0x04);
		}
		if(n<=1){
			SendGameMessage("�����ָ�ҩ�����þ�,�����Ѿ��޷���Ѫ,��سǹ���~!", 0x04);
			return;
		}

		if(GameData.Blood_Max<=60){
			SendGameMessage("������С��60,�����˷����ֶ���Ѫ", 0x02);
			return;
		}
		per=(GameData.Blood_Max*10)/lost;
		if(Config.BLOOD_MODE==0) Blood_B=60;//������ѪA
		if(Config.BLOOD_MODE==1) Blood_B=120;//������Ѫb
		if(Config.BLOOD_MODE==2) Blood_B=250;//������Ѫc
		if(Config.BLOOD_MODE==3) Blood_B=450;//������Ѫd
		if(per<=33){//90/60  ���ֻ��30%Ѫ�������Ϻ�һ��
			UserNovelity((BYTE)0x01,(BYTE)(now));
			Blood_Add=Blood_Add+Blood_B;
		}
		if((lost-Blood_Add)>Blood_B){//���ܺȡ������ź�
			UserNovelity((BYTE)0x01,(BYTE)(now));
			Blood_Add=Blood_Add+Blood_B;
		}
		if(Config.BLOOD_ADD_MODE==3){//PKģʽ
			if(GameData.Blood_Now+Blood_Add <GameData.Blood_Max){//�ٺ�һ��
				UserNovelity((BYTE)0x01,(BYTE)(now));
			}
		}
	}

}

// �Զ���ħ��
void CALLBACK _SpellTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if( (!( GameData.stats[1] & (BYTE)0x20)) && (!(GameData.stats[1] & (BYTE)0x40)) && (!(GameData.stats[1] & (BYTE)0x80)) ){
		if(Config.Player_Mode_5==1){//�񱬹���
			UseSpell((BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x0f);
		}
		if(Config.Player_Mode_4==1){//��ջ���
			UseSpell((BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x0d);
		}
		if(Config.Player_Mode_11==1){//��ջ���
		
			UseSpell((BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x0e);
		}
	}
	
	if( (Config.Player_Mode_3==1) && (!(GameData.stats[1]&(BYTE)0x08)) ){//����
		UseSpell( (BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x0b);
	}
	
	if( (Config.Player_Mode_1==1) && (!(GameData.stats[1]&(BYTE)0x10)) ){//����
		UseSpell( (BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x0c);
	}
	if(Config.Player_Mode_15==1 && (!(GameData.stats[3]&0x40 ))){//��������
		UseSpell( (BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x1E);
		GameData.stats[3]=GameData.stats[3]+0x40;
	}
	if(Config.Player_Mode_9==1 && (!(GameData.stats[2]&0x20 ))){//�����ػ�
		UseSpell( (BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x12);
	}
	if(Config.Player_Mode_10==1 && (!(GameData.stats[2]&0x40 ))){//��ħ
		UseSpell( (BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x13);
	}
	if(Config.Player_Mode_6==1&& (!(GameData.stats[2]&0x80 ))){//����
		UseSpell((BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x14);
	}

	if( (!( GameData.stats[1] & (BYTE)0x02)) && (!(GameData.stats[1]&(BYTE)0x10)) &&
		(!(GameData.stats[1]&(BYTE)0x08)) && (!(GameData.stats[1]&(BYTE)0x01)))
	{
		if(Config.Player_Mode_8==1){//����	
			UseSpell( (BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x06);
		}
		if(Config.Player_Mode_2==1){//����
			UseSpell( (BYTE)(GameData.ServerID%256),(BYTE)(GameData.ServerID/256),0x01);
		}
		
	
	}

}
// ���Խ��ж����Ѫ����
void CALLBACK _OtherBloodTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{  
	if (GameData.MOTION_STOP == TRUE) return;
	// �������ÿ��� 4 ��
	if (Config.OTHER_ADD_BLOOD == TRUE)
	{
		if (GameData.Novelity_39.BloodANumber >= 1) UseItem(GameData.Novelity_39.BloodANow);
		else if (GameData.Novelity_39.BloodBNumber >= 1) UseItem(GameData.Novelity_39.BloodBNow); 
		else if (GameData.Novelity_39.BloodCNumber >= 1) UseItem(GameData.Novelity_39.BloodCNow);
	}
}
// �������� ͣ��ʱ��
void CALLBACK _StayTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
//	
}

// �һ�
void CALLBACK _MoveTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{

}

void CALLBACK _MainAggressiveTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (Config.AUTO_LOCK_TARGET == TRUE)
	{
		if(GameData.lockID[0]!=0||GameData.lockID[1]!=0)
			Aggressive(GameData.lockID[0],GameData.lockID[1]);
	}
	
	if (Config.AUTO_MAX_KILL==TRUE)
	{
		if(GameData.lockID[0]!=0||GameData.lockID[1]!=0 || GameData.Spell_lockID[0]!=0||GameData.Spell_lockID[1]!=0){
			if(Config.MaxKill_time>7){
				Config.MaxKill_time=0;
				if(GameData.lockID[0]!=0||GameData.lockID[1]!=0)
					UseSpell(GameData.lockID[0], GameData.lockID[1],(BYTE)(GameData.PlayerMode+0x17));
				else if(GameData.Spell_lockID[0]!=0||GameData.Spell_lockID[1]!=0)
					UseSpell(GameData.Spell_lockID[0], GameData.Spell_lockID[1],(BYTE)(GameData.PlayerMode+0x17));

			}else
				Config.MaxKill_time++;
		}
	}

}
void CALLBACK _OtherAggressiveTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (Config.AUTO_LOCK_TARGET == TRUE)
	{
		if(GameData.Spell_lockID[0]!=0||GameData.Spell_lockID[1]!=0){
			if(GameData.Spell_code==2||GameData.Spell_code==3||GameData.Spell_code==4||
				  GameData.Spell_code==5||GameData.Spell_code==8||GameData.Spell_code==10)
			{
				UseSpell(GameData.Spell_lockID[0], GameData.Spell_lockID[1],GameData.Spell_code);
			}
		}
	}
}


void CALLBACK _AuthenticationProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	/*
	char data[40];
	server_code.con=false;
	
	if(server_code.S_Encode_recv(server_code.Socket,data)>0){
		if(data[0]==(char)0xee)
			server_code.con=true;
		server_code.Time=*(int *)(data+1);
		SaveSendRecvLog("�˺�:",(char*)data,10,"C:\\niuniu2.log");
	}
	if(data[0]==(char)0x10 || server_code.con==false || server_code.Time==0){
		SendGameMessage("δ��Ȩ���˺Ż���ʹ��ʱ������,10����˳���Ϸ", 0x04);
		Sleep(10);
		_exit(-1);
	}else{
		sprintf(data,"����˺Ż��� %d Сʱʹ�õ�ʱ��",server_code.Time/3600);
		SendGameMessage(data, 0x03);

	}
	_KillAuthenticationTimer();
	*/

}

// ��������
void BloodControl()
{

}

// ����ʱ��
void CALLBACK _TalkTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (Config.AUTO_SPEAK == TRUE)
	{
		Speak(Config.TalkMode, Config.TalkBuf);	
		SaveLog(Config.TalkBuf,"c:\\niuniu2.log");
	}

}

void SubAutoSite(char *name)
{
	;
}

void AddAutoSite()
{
	int t3, t4, t5;
	char tmp1[128], tmp2[128], tmp3[128], tmp4[128];

	int x1, y1;

	GetWindowText(GetDlgItem(UIData.hMainConfig6_1, IDC_COMBO_6_1_1), tmp1, 128);
    Edit_GetText(GetDlgItem(UIData.hMainConfig6_1, IDC_EDIT_AUTO_TIME), tmp2, 128);   
	Edit_GetText(GetDlgItem(UIData.hMainConfig6_1, IDC_SITEXY_EDIT), tmp3, 128);
	GetWindowText(GetDlgItem(UIData.hMainConfig6_1, IDC_COMBO_6_1_2), tmp4, 128);

	if ((strcmp(tmp1, "") == 0)) return; 
	if ((strcmp(tmp2, "") == 0)) return; 
	if ((strcmp(tmp3, "") == 0)) return; 
	if ((strcmp(tmp4, "") == 0)) return; 

	x1 = 100, y1 = 200;
	sscanf(tmp3, "%d - %d", &x1, &y1); 

	AddInfoList6_1(0, tmp2, tmp1, tmp3);
	t3 = 0;	t4 = 0; t5 = 0;
	Obj_site_chain = add_site_obj(tmp1, GetNumber(tmp2), GetNumber(tmp4), x1, y1, t5);
}

void AddChum()
{
	char tmp1[4][64] = { "����", "����", "����", "����" };
	char tmp3[2][10] = { "�ر�", "����" };

	char tmp2[128];
	int t1, t2;

	t1 = SendDlgItemMessage(UIData.hMainConfig5_1, IDC_COMBO_5_1_1, CB_GETCURSEL, 0, 0);
	GetWindowText(GetDlgItem(UIData.hMainConfig5_1, IDC_COMBO_5_1_2), tmp2, 128);
	t2 = Button_GetCheck(GetDlgItem(UIData.hMainConfig5_1, IDC_CHECK_AUTO_TEAM));

	if (strcmp(tmp2, "") == 0) return;

	AddInfoList5_1(0, tmp1[t1], tmp2, tmp3[t2]);

	Obj_chum_chain = add_chum_obj(t1, tmp2, t2);

}

void GotoSite(BYTE Code1, BYTE Code2, BYTE Code3, BYTE Code4, BYTE Code5)
{
	char *tmp;
/*
2D 00 00 00 83 02 01 00 13 02 ɳ-����
34 00 00 00 83 02 1A 00 FB 01 ��-��ɳ
39 00 00 00 83 02 01 00 0A 02 ɳ-���� 
3F 00 00 00 83 02 01 00 FA 01 ��-����ԭ�Ǳ�
47 00 00 00 83 02 01 00 FB 01 ��ԭ�Ǳ�-����
4C 00 00 00 83 02 01 00 09 02 ��-��ɳ
51 00 00 00 83 02 01 00 0B 02 ɳ-��ɳĮ����
41 00 00 00 83 02 1A 00 FD 01 �ϵ���
56 00 00 00 83 02 1A 00 FC 01 ark
E0 00 00 00 83 02 38 00 CA 00 tian  
09 00 00 00 83 02 90 01 0C 00 ��-����
?E 00 00 00 83 02 1A 00 02 02 zha -> tian
`A 00 00 00 83 02 90 01 02 00 tian -> ark
AE 00 00 00 83 01 22 4E 00 00 ark -> luo
*/

	LPBYTE lpTarget;
	LPBYTE lpSource;


	lpTarget =  (LPBYTE)malloc(10);
	lpSource =  (LPBYTE)malloc(10);

	lpSource[0] = 0x00;
	lpSource[1] = 0x00;
	lpSource[2] = 0x00;
	lpSource[3] = 0x00;
    lpSource[4] = 0x83;
	lpSource[5] = Code1;
	lpSource[6] = Code2;
	lpSource[7] = Code3;
	lpSource[8] = Code4;
    lpSource[9] = Code5;

	Encode_Decode(lpTarget, lpSource, 10);

	tmp =  (char*)malloc(17);

	tmp[0]  = (char)0xAA;
	tmp[1]  = (char)0x55;
	tmp[2]  = (char)0x0B;
	tmp[3]  = (char)0x00;
	tmp[4]  = (char)0xF9;
	tmp[5]  = (char)lpTarget[0];     // ʱ��Ч����

	tmp[6]  = (char)lpTarget[1];
	tmp[7]  = (char)lpTarget[2];
	tmp[8]  = (char)lpTarget[3];
	tmp[9]  = (char)lpTarget[4];
	tmp[10] = (char)lpTarget[5];
	tmp[11] = (char)lpTarget[6];
	tmp[12] = (char)lpTarget[7];
	tmp[13] = (char)lpTarget[8];
	tmp[14] = (char)lpTarget[9];

	tmp[15] = (char)0x55;
	tmp[16] = (char)0xAA;

//	int newlen = analysesend(GameData.s, tmp, 17);

	OldSendAddress(GameData.s,tmp,17,0);

	free(lpTarget);
	free(lpSource);
	free(tmp);
}
void sendlogin(char *SendBuf)
{
	int userlen,pwdlen,i,len;

	char tmp[256];
	SaveLog("�û�������û���\r\n","c:\\niuniu.log");
	userlen=strlen(GameData.user);
	pwdlen=strlen(GameData.password);
tmp[0]= SendBuf[0];
	tmp[1]= SendBuf[1];
	tmp[2]= SendBuf[2];
	tmp[3]= 00;
	tmp[4]=(char)userlen;
	tmp[5]=0x00;

	for(i=0;i<userlen;i++){
		tmp[6+i]=GameData.user[i];
	}
	len=6+userlen;

	tmp[len]=(char)pwdlen;
	len++;
	tmp[len]=0x00;
	len++;

	for(i=0;i<pwdlen;i++){
		tmp[len+i]=GameData.password[i];
	}
	len=len+pwdlen;
/*
   tmp[len]='V';
   tmp[len+1]='=';
   tmp[len+2]='1';
   tmp[len+3]='2';
   len=len+4;
   */
  int l1=(unsigned int)SendBuf[2];
   char *p=&SendBuf[4+l1-19];
   tmp[2]=(unsigned int)len+19;
   memcpy(SendBuf,tmp,len);
   memcpy(SendBuf+len,p,19);
	
//	SaveSendRecvLog("�û�:",(char*)SendBuf,len+19,"C:\\send.log");
	return;
}
