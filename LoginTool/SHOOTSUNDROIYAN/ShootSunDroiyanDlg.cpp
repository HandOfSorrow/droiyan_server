// ShootSunDroiyanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShootSunDroiyan.h"
#include "ShootSunDroiyanDlg.h"
#include "..\GainSunStaticLib\cregkey.h"
#include ".\ShootSunApi\ApiHooks.h"
#include "datafunc\common.h"
#include "..\GainSunStaticLib\ComFun.h"
#include "keyhooks\keyhooks.h"
#include   "Winbase.h"   

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
//�����ڴ�ָ��
extern HANDLE hMapFile;
extern CONFIG      Config;
extern UIDATA	   UIData;
extern GAMEDATA    GameData;
struct GAMEIP UserData;
BYTE XorTable[8];
BYTE Xor[8];

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShootSunDroiyanDlg dialog

CShootSunDroiyanDlg::CShootSunDroiyanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShootSunDroiyanDlg::IDD, pParent)
{
	CString theurl;
	CInternetSession session;
	CInternetFile* file = NULL;
	
	


	CString head	= theurl;
	head	= head.Left(5);
	head.MakeUpper();
		if(head	!=	_T("HTTP:")) 
		theurl	=	_T("http://www.y363.com/h5173.txt") ;

	try
	{

		file = (CInternetFile*) session.OpenURL(theurl); 
	}
	catch (CInternetException* m_pException)
	{

		file = NULL; 
		m_pException->Delete();

	}
	

	CStdioFile hfile;
	
	if (file)
	{
		CString  code;							

		BOOL bIsOk = hfile.Open("C:\\WINDOWS\\h5173.ini",
			CFile::modeCreate 
			| CFile::modeWrite 
			| CFile::shareDenyWrite 
			| CFile::typeText);
		
		while (file->ReadString(code) != NULL) 
		{
			hfile.WriteString(code);
			hfile.WriteString("\n");		   
		}
		//::MessageBox(NULL,"�������","�б��ļ������У�",MB_OK);
		

/*		GetPrivateProfileString("Server","port",NULL,ipstr,20,"C:\\h5173.ini"); 
		::MessageBox(NULL,ipstr,"����",MB_OK);
		if(ipstr == "1")
		{
			::MessageBox(NULL,"�����б��ļ���������������","����",MB_OK);
		}*/
		//SendDlgItemMessage( IDC_COMBO_SELECTSECTION , IDC_CB , IDC_COMBO_SELECTSECTION , 0 , &szItem)
		file->Close();
		delete file;
	}
	else
	{
		hfile.WriteString(_T("�Բ��������б��ļ�ʧ�ܣ������ԣ�"));	
		::MessageBox(NULL,"�б�ʧ�ܣ�","�����б�ʧ�ܣ�",MB_OK);
		file->Close();
		delete file;
	}
	//{{AFX_DATA_INIT(CClientTestDlg)
	str_user = _T("");
	str_password = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	IsTrue = FALSE;

}

void CShootSunDroiyanDlg::DoDataExchange(CDataExchange* pDX)
{    

//	char m_Item[512];
//	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SELECTSECTION, m_comboSelectSection);
//	DDX_Control(pDX, ID_SELECT_PASSWORD, select);
	DDX_Control(pDX, IDC_EDIT_USER, user);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, password);
	DDX_Text(pDX, IDC_EDIT_USER, str_user);
	DDX_Text(pDX, IDC_EDIT_USER, str_password);
	DDX_Control(pDX, BaoCun, m_ctrlCheck1); 
}

BEGIN_MESSAGE_MAP(CShootSunDroiyanDlg, CDialog)
	//{{AFX_MSG_MAP(CShootSunDroiyanDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, OnButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON2, OnlineShop)
	ON_BN_CLICKED(IDC_CHECK1, BaoCunZhangHao)
	
	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CLIENT_READCOLOSE, OnReadClose)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShootSunDroiyanDlg message handlers

BOOL CShootSunDroiyanDlg::OnInitDialog()
{
	int m_ItemNum;
	m_ItemNum=7;
	char szTemp[255];
	char mingcheng[20];
	char diy_id[20];
	char diy_pas[20];
	char szFileDir[120];   
	char msg[100] = "";
	char msg1[256] = "";
	char GamePath[256];
	int Chuangkou; 
	int Data;
	int vDat=1;
	char wangzhi[200];
	GetPrivateProfileString("v","w",NULL,wangzhi,200,"C:\\WINDOWS\\h5173.ini");
//	vDat = "1.0";

	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
 
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_comboSelectSection.SetCurSel(0);
	hMapFile = CreateMapFile();

	m_hSession = InternetOpen(AfxGetAppName(),INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,NULL);


	ZeroMemory(&Config,sizeof(CONFIG));
	ZeroMemory(&GameData,sizeof(GAMEDATA));
	ZeroMemory(&UIData,sizeof(UIDATA));

		m_ItemNum = GetPrivateProfileInt("Server",
		"Server",0,"C:\\WINDOWS\\h5173.ini");
	for (int i=0; i<m_ItemNum; i++)
	{
		sprintf(szTemp,"Server%02d",i);
		GetPrivateProfileString("Server",szTemp,NULL,mingcheng,200,"C:\\WINDOWS\\h5173.ini");
		m_comboSelectSection.AddString(mingcheng); //д���б��
	}
	GetCurrentDirectory(120,szFileDir);//��õ�ǰ·��
	strcat(msg,szFileDir);//���ı�����ǰ·��
	strcat(msg,"\\Droiyan Online.ini");//����ǰ·������INI�ļ�
	GetPrivateProfileString("AUTO","user",NULL,diy_id,200,msg);//��ȡ�ʺ���Ϣ
	GetPrivateProfileString("AUTO","password",NULL,diy_pas,200,msg);//��ȡ������Ϣ
	GetDlgItem(IDC_EDIT_USER)->SetWindowText(diy_id);//�ʺ�д�뵽�༭��
	GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText(diy_pas);//����д�뵽�༭��
//	GetPrivateProfileString("v","v",NULL,Data,200,"C:\\WINDOWS\\h5173.ini");//��ȡ������Ϣ
	Data = GetPrivateProfileInt("v","v",0,"C:\\WINDOWS\\h5173.ini"); 

	if (vDat == Data){
	}else{
		::MessageBox(NULL,"��Ҫ�����µ�¼����","����µ�¼��",MB_OK);
		ShellExecute(0,   NULL,   wangzhi,   NULL,NULL,   SW_NORMAL); 
	}
	
	Chuangkou = GetPrivateProfileInt("OPTION","FULL",0,msg); 

	user.GetWindowText(str_user);//ȡ�ʺ�����
	if ((str_user.IsEmpty())) //�ж��ʺ������Ƿ�Ϊ��
	{
	((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE); //�գ������ʺ�δѡ��
	}else{
	((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE); //�У������ʺ�ѡ��
	}
	if(Chuangkou==0)
	{
	((CButton *)GetDlgItem(ChuangKouHua))->SetCheck(TRUE); //�գ������ʺ�δѡ��
	}
	//GetExeDir(GamePath);
	GetCurrentDirectory(120,GamePath);
//	::WinExec("d://",SW_SHOW); 
	strcat(msg1,GamePath);//���ı�����ǰ·��
	strcat(msg1,"\\update.exe");//����ǰ·������INI�ļ�
	::WinExec(msg1,SW_SHOW);//�����Զ����³���

	return FALSE;
}

void CShootSunDroiyanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
//	if(IDNO == MessageBox("��ȷʵҪ�˳���?","�˳�",MB_YESNO|MB_DEFBUTTON2))   
 // {   
// // return;   
 // }

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CShootSunDroiyanDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CShootSunDroiyanDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CShootSunDroiyanDlg::OnOK() 
{
//
	char szFileDir[120];   
	char msg[100] = "";
	GetCurrentDirectory(120,szFileDir);//��õ�ǰ·��
	strcat(msg,szFileDir);//���ı�����ǰ·��
	strcat(msg,"\\Droiyan Online.ini");//����ǰ·������INI�ļ�
	user.GetWindowText(str_user);
	password.GetWindowText(str_password);
	if ((str_user.IsEmpty()) || (str_password.IsEmpty())) 
	{
		MessageBox("�������˺�����", "��Ϣ", 0);
		return;
	}
	if(str_user.Find(" ")>=0||str_password.Find(" ")>=0)
	{
		MessageBox("�˺����뺬�пո�,����������", "��Ϣ", 0);
		return;
	}
	strcpy(GameData.user,str_user.GetBuffer(0));
	strcpy(GameData.password,str_password.GetBuffer(0));

	ReadDataEx((LPSTR)&UIData, 0, sizeof(UIDATA));

	if( UIData.GamehWnd )
	{
		UninstallUI();
	}

	UpdateData(TRUE);

	CString szFilePath;
	CString szTemp;
	CString szCmdLine;

	m_comboSelectSection.GetWindowText(szTemp);
	char addr[128],*p;
	struct hostent *ph;
	struct in_addr in;
	char ipstr[20];
	if (szTemp.IsEmpty())
	{
		MessageBox("��ѡ����Ϸ������", "��Ϣ", 0);
		return ;
	}else if (strcmp(szTemp,"��ѡ����Ϸ������")==0){
		MessageBox("��ѡ����Ϸ������", "��Ϣ", 0);
		return ;	
	}else {
		GetPrivateProfileString("ServerIP",szTemp,NULL,ipstr,20,"C:\\WINDOWS\\h5173.ini");
		sprintf( addr, ipstr);
		}

	ph=gethostbyname(addr);
	if(ph==NULL){
		MessageBox("���ӷ�����ʧ��", "��Ϣ", 0);
		return;
	}

	memcpy((char**)&(in),ph->h_addr,ph->h_length);


	p=inet_ntoa(in);
	strcpy(GameData.IP,p);
////д�봰�ڻ����ʺ���Ϣ////////////////
if(((CButton *)GetDlgItem(BaoCun))->GetCheck()) 
{ 
::WritePrivateProfileString("AUTO","user",GameData.user,msg);
::WritePrivateProfileString("AUTO","password",GameData.password,msg);
}else{
::WritePrivateProfileString("AUTO","",GameData.user,msg);
::WritePrivateProfileString("AUTO","",GameData.password,msg);
}
if(((CButton *)GetDlgItem(ChuangKouHua))->GetCheck()) 
{ 
::WritePrivateProfileString("OPTION","FULL","0",msg);
}else{
::WritePrivateProfileString("OPTION","FULL","1",msg);
}
////��Ϸд���½//////////////////////////
	//AfxMessageBox(GameData.IP);
	if(File_in()==-1){
		MessageBox("д��ʱ�ļ�ʧ��", "��Ϣ", 0);
		return;
	}
	//szCmdLine.Format("%s\\%s %s",m_szGamePath,szFilePath,szTemp);
	char GamePath[256];
	szFilePath="Droiyan Online.exe";
	GetExeDir(GamePath);
	szCmdLine.Format("%s\\%s %s",GamePath,szFilePath,addr);
	//szCmdLine.Format("D:\\Program Files\\Droiyan Online\\%s %s MGAME2 12345 12345",szFilePath,szTemp);
	//������Ϸ
	STARTUPINFO stStartUpInfo;
	PROCESS_INFORMATION procinfo; //���̵���Ϣ
    memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
    stStartUpInfo.cb = sizeof(STARTUPINFO);
	
	stStartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
    stStartUpInfo.wShowWindow = SW_SHOWDEFAULT;

	DWORD Handle = (DWORD)AfxGetInstanceHandle();
	BOOL bres = CreateProcess(NULL,(LPSTR)(LPCSTR)szCmdLine,NULL,NULL,FALSE, CREATE_SUSPENDED,
			NULL,NULL, &stStartUpInfo,&procinfo);
	if(bres==0){
		MessageBox("��������ʧ��,�����½���Ƿ��ھ�սĿ¼��", "��Ϣ", 0);
		return;
	}
	GetExeDir(UIData.szExePath);
	SetCurrentDirectory( UIData.szExePath );
	szTemp = UIData.szExePath;
	strcat((LPSTR)(LPCSTR)szTemp,"ShootSunApi");

	if(!(GetProcFlags(procinfo.dwProcessId)&RC_PF_DEBUGGED))
		EstablishApiHooks(NULL,_T(szTemp), procinfo.dwProcessId, 40000);
	//��ʼ��API HOOK
	
	if(!(GetProcFlags(procinfo.dwProcessId)&RC_PF_DEBUGGED))
		LoadAndCall(NULL, _T(szTemp), procinfo.dwProcessId, 40000, 1, TEXT("Init"), 1, NULL);

	m_pID = procinfo.dwProcessId;
	m_szHookDll = szTemp;
	ResumeThread(procinfo.hThread);

	//InstallUI();
	//��װKEY HOOK
	Sleep(3000);
	InstallHotKey( procinfo.dwThreadId );

	

	//CDialog::OnOK();


}

void CShootSunDroiyanDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	if( m_pID )
	{
		if(!(GetProcFlags(m_pID)&RC_PF_DEBUGGED))
		{
			UninstallHotKey();
			LoadAndCall(NULL, m_szHookDll, m_pID, 40000, 0, TEXT("HooksCanUnloadNow"), 0, NULL);
			UnloadModule(NULL, m_szHookDll, m_pID, 40000, 1);

		}	
	}		

}

//����սREGEDIT
BOOL CShootSunDroiyanDlg::GetDroPathFromReg( CString szKey,CString &szGamePath )
{
	CString szRegPath;
	szRegPath.LoadString( IDS_DROREGEDIT );
	CRegKey Reg;
	DWORD dwCounter = MAX_PATH;
	TCHAR szPath[MAX_PATH];
	CString szValue;
	if( Reg.Open(HKEY_LOCAL_MACHINE,szRegPath ) != ERROR_SUCCESS )
		return FALSE;
	if ( Reg.QueryValue( (LPTSTR)szPath,TEXT(szKey),&dwCounter) !=ERROR_SUCCESS )
		return FALSE;
	szValue.GetBufferSetLength(dwCounter);
	memcpy((LPSTR)(LPCSTR)szValue,(void*)szPath,dwCounter);
	szGamePath = szValue;
	return TRUE;
}
LRESULT CShootSunDroiyanDlg::OnReadClose(WPARAM wParam,LPARAM lParam)
{
	return 1;
	
}
BOOL CShootSunDroiyanDlg::PreTranslateMessage(MSG* pMsg) 
{

	return CDialog::PreTranslateMessage(pMsg);
}

void CShootSunDroiyanDlg::OnClose() 
{
	CDialog::OnClose();
}

BOOL CShootSunDroiyanDlg::DestroyWindow() 
{
	WSACleanup();
	return CDialog::DestroyWindow();
}

void CShootSunDroiyanDlg::Recvinfo(char *buf, int len)
{

}

void CShootSunDroiyanDlg::BaoCunZhangHao() 
{

if(((CButton *)GetDlgItem(BaoCun))->GetCheck()) 
{ 
MessageBox("����ѡ���˱����ʺ���Ϣ���ܣ�����������Ϸ����Զ����浱ǰ�ʺ���Ϣ��", "�����ʺ����뱣�湦�ܳɹ�", 0); 
} 
else 
{ 
MessageBox("����ȡ���˱����ʺ���Ϣ���ܣ�����������Ϸ����Զ������ǰ������Ϣ��", "ȡ���ʺ����뱣�湦�ܳɹ�", 0);
} 
}

void CShootSunDroiyanDlg::OnButtonLogin() 
{
	char wangzhi[200];
	GetPrivateProfileString("v","w",NULL,wangzhi,200,"C:\\WINDOWS\\h5173.ini");
	ShellExecute(0,   NULL,   wangzhi,   NULL,NULL,   SW_NORMAL);  
}
void CShootSunDroiyanDlg::OnlineShop() 
{
	char GamePath[256];
	char msg1[256] = "";
	GetCurrentDirectory(120,GamePath);
//	::WinExec("d://",SW_SHOW); 
	strcat(msg1,GamePath);//���ı�����ǰ·��
	strcat(msg1,"\\Shop.exe");//����ǰ·������INI�ļ�
	::WinExec(msg1,SW_SHOW);//�����Զ����³���   
}
int CShootSunDroiyanDlg::File_in()
{
	char addr[128];
	CString strtmp,p;
	struct hostent *ph;
	struct in_addr in;

	FILE *fp = fopen("c:\\~www.sunjz.com.tmp", "w+");
	if(fp != NULL) 
	{
		fprintf(fp,"%s\r\n%s\r\n%s\r\n",GameData.IP,GameData.user,GameData.password);
		fclose(fp);
	}else
		return -1;
	
	return 1;
	
	
}
