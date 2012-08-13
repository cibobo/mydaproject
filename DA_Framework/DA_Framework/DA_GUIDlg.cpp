// DA_GUIDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "DA_Framework.h"
#include "DA_GUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDA_GUIDlg-Dialogfeld




CDA_GUIDlg::CDA_GUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDA_GUIDlg::IDD, pParent)
	, isOnline(false)
	, isOffline(true)
	, isObservingWindowVisible(true)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// create a instance for MainThread
	pMainThread = new MainThread();
}

void CDA_GUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_RADIO_OFFLINE, isOffline);
	DDX_Check(pDX, IDC_CHECK_VISUAL1, isObservingWindowVisible);
	DDX_Control(pDX, IDC_SLIDER_FRAMERATE, framerateSlider);
	DDX_Control(pDX, IDC_EDIT_FRAMERATE, framerateEditor);
	DDX_Control(pDX, IDC_EDIT_OFFLINEPATH, inputPathEditor);
	DDX_Control(pDX, IDC_EDIT1, outputPathEditor);
}

BEGIN_MESSAGE_MAP(CDA_GUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDA_GUIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDA_GUIDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CDA_GUIDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_CHECK_VISUAL1, &CDA_GUIDlg::OnBnClickedCheckVisual1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_FRAMERATE, &CDA_GUIDlg::OnNMCustomdrawSliderFramerate)
	ON_EN_CHANGE(IDC_EDIT_FRAMERATE, &CDA_GUIDlg::OnEnChangeEditFramerate)
	ON_EN_CHANGE(IDC_EDIT_OFFLINEPATH, &CDA_GUIDlg::OnEnChangeEditOfflinepath)
	ON_BN_CLICKED(IDC_RADIO_OFFLINE, &CDA_GUIDlg::OnBnClickedRadioOffline)
	ON_BN_CLICKED(IDC_RADIO_ONLINE, &CDA_GUIDlg::OnBnClickedRadioOnline)
END_MESSAGE_MAP()


// CDA_GUIDlg-Meldungshandler

BOOL CDA_GUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
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

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// Init Framerate Slider
	this->framerateSlider.SetRangeMin(FRAMERATE_MIN, false);
	this->framerateSlider.SetRangeMax(FRAMERATE_MAX, false);

	// Init Input Path with the default Data Source
	this->inputPathEditor.SetWindowTextA(CString("Eva2Boxes"));

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CDA_GUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
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

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CDA_GUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CDA_GUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//OnClick listener for the Run button
void CDA_GUIDlg::OnBnClickedOk()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	//OnOK();
	//pMainThread->beginInputThread(0);

	//if(_beginthread (pMainThread->beginInputThread, 0, NULL) == -1){
	//	cout<<"Failed to create input thread"<<endl;
	//}

	DWORD InputThreadID, OpenGLThreadID;
	if(this->isOffline){
		CreateThread(NULL, 0, pMainThread->beginInputThread, (void*)pMainThread, 0, &InputThreadID);
	}

	if(this->isObservingWindowVisible){
		CreateThread(NULL, 0, pMainThread->beginOpenGLSceneThread, (void*)pMainThread, 0, &OpenGLThreadID);
	}
}

//OnClick Listener for the Quit button
void CDA_GUIDlg::OnBnClickedCancel()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	OnCancel();
}

//Taking Pause of the Input Date Stream
void CDA_GUIDlg::OnBnClickedButtonPause()
{
	if(pMainThread->isPause){
		cout<<"frame running!"<<endl;
		LeaveCriticalSection(&(pMainThread->pauseCrs));
		pMainThread->isPause = false;
	} else {
		cout<<"frame pause!"<<endl;
		EnterCriticalSection(&(pMainThread->pauseCrs));
		pMainThread->isPause = true;
	}
}

//Controll the visuable of the Observing Windows
void CDA_GUIDlg::OnBnClickedCheckVisual1()
{
	this->isObservingWindowVisible = !(this->isObservingWindowVisible);
}


void CDA_GUIDlg::OnNMCustomdrawSliderFramerate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	*pResult = 0;

	CString str;
	str.Format("%i", this->framerateSlider.GetPos());
	// Connect the slider and edit text field of the framerate
	this->framerateEditor.SetWindowTextA(str);

	// Update the framerate
	pMainThread->FRAMERATE = this->framerateSlider.GetPos();
	UpdateData(false);
}

void CDA_GUIDlg::OnEnChangeEditFramerate()
{
	// TODO:  Falls dies ein RICHEDIT-Steuerelement ist, wird das Kontrollelement
	// diese Benachrichtigung nicht senden, es sei denn, Sie setzen den CDialog::OnInitDialog() außer Kraft.
	// Funktion und Aufruf CRichEditCtrl().SetEventMask()
	// mit dem ENM_CHANGE-Flag ORed in der Eingabe.

	CString str;
	this->framerateEditor.GetWindowTextA(str);

	int framerate = atoi(str);
	if(framerate < FRAMERATE_MIN){
		framerate = FRAMERATE_MIN;
	} else if (framerate > FRAMERATE_MAX){
		framerate = FRAMERATE_MAX;
	}

	this->framerateSlider.SetPos(framerate);
	UpdateData(false);

	pMainThread->FRAMERATE = framerate;
}

void CDA_GUIDlg::OnEnChangeEditOfflinepath()
{
	// TODO:  Falls dies ein RICHEDIT-Steuerelement ist, wird das Kontrollelement
	// diese Benachrichtigung nicht senden, es sei denn, Sie setzen den CDialog::OnInitDialog() außer Kraft.
	// Funktion und Aufruf CRichEditCtrl().SetEventMask()
	// mit dem ENM_CHANGE-Flag ORed in der Eingabe.

	CString str;
	this->inputPathEditor.GetWindowTextA(str);
	int length = str.GetLength();
	strcpy(pMainThread->INPUTPATH, str);

}

//Control the Online mode is been selected or not
void CDA_GUIDlg::OnBnClickedRadioOffline()
{
	this->isOnline = !(this->isOnline);
}

//Control the Offline mode is been selected or not
void CDA_GUIDlg::OnBnClickedRadioOnline()
{
	this->isOffline = !(this->isOffline);
}
