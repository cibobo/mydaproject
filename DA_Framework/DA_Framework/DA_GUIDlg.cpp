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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// create a instance for MainThread
	pMainThread = new MainThread();
}

void CDA_GUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_VISUAL1, pMainThread->isObservingWindowVisible);
	DDX_Check(pDX, IDC_CHECK_VISUAL2, pMainThread->isResultWindowVisible);
	DDX_Check(pDX, IDC_CHECK_VISUAL3, pMainThread->isOpenCVWindowVisible);

	// Radio Button for controlling the On and Offline mode
	DDX_Check(pDX, IDC_RADIO_ONLINE, pMainThread->isOnline);
	DDX_Check(pDX, IDC_RADIO_OFFLINE, pMainThread->isOffline);
	// Radio Button for controlling the Learning and Recognition mode
	DDX_Check(pDX, IDC_RADIO_LEARNING, pMainThread->isLearning);
	DDX_Check(pDX, IDC_RADIO_RECOGNITION, pMainThread->isRecognise);
	
	DDX_Control(pDX, IDC_SLIDER_FRAMERATE, framerateSlider);
	DDX_Control(pDX, IDC_EDIT_FRAMERATE, framerateEditor);
	DDX_Control(pDX, IDC_EDIT_OFFLINEPATH, inputPathEditor);
	DDX_Control(pDX, IDC_EDIT_ONLINEPATH, outputPathEditor);

	// Detection Parameters
	DDX_Text(pDX, IDC_EDIT_MAXAFEATURES, pMainThread->pDetector->MAXFEATURECOUNT);
	DDX_Text(pDX, IDC_EDIT_MINAFEATURES, pMainThread->pDetector->MINFEATURECOUNT);
	DDX_Text(pDX, IDC_EDIT_MAXELOOPS, pMainThread->pDetector->MAXDETECTLOOPS);
	DDX_Text(pDX, IDC_EDIT_BBRIGHTNESS, pMainThread->pDetector->BEGINBRIGHTNESS);
	DDX_Text(pDX, IDC_EDIT_BCONTRAST, pMainThread->pDetector->BEGINCONTRAST);

	// CenSurE Parameters
	DDX_Text(pDX, IDC_EDIT_MAXSIZE, pMainThread->pDetector->MAXSIZE);
	DDX_Text(pDX, IDC_EDIT_RESTHRESHOLD, pMainThread->pDetector->responseThreshold);
	DDX_Text(pDX, IDC_EDIT_LINETHRESHOLDP, pMainThread->pDetector->LINETHRESHOLDPROJECTED);
	DDX_Text(pDX, IDC_EDIT_LINETHRESHOLDB, pMainThread->pDetector->LINETHRESHOLDBINARIZED);
	DDX_Text(pDX, IDC_EDIT_NONMAXSUPSIZE, pMainThread->pDetector->SUPPRESSNONMAXSIZE);

	// Brightness Controll
	DDX_Text(pDX, IDC_EDIT_CBRIGHTNESS, pMainThread->pDetector->balance);
	DDX_Text(pDX, IDC_EDIT_CCONTRAST, pMainThread->pDetector->contrast);

	// Distance Filter Parameters
	DDX_Text(pDX, IDC_EDIT_DFVALUE, pMainThread->pParameters->DISTANCEFILTER_EPS);
	DDX_Text(pDX, IDC_EDIT_DFPROPORTION, pMainThread->pParameters->DISTANCEFILTER_DIFFRATE);
	DDX_Text(pDX, IDC_EDIT_DFFRAMES, pMainThread->pParameters->DISTANCEFILTER_FRAMES);
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
	ON_BN_CLICKED(IDC_CHECK_VISUAL2, &CDA_GUIDlg::OnBnClickedCheckVisual2)
	ON_BN_CLICKED(IDC_CHECK_VISUAL3, &CDA_GUIDlg::OnBnClickedCheckVisual3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_FRAMERATE, &CDA_GUIDlg::OnNMCustomdrawSliderFramerate)
	ON_EN_CHANGE(IDC_EDIT_FRAMERATE, &CDA_GUIDlg::OnEnChangeEditFramerate)
	ON_EN_CHANGE(IDC_EDIT_OFFLINEPATH, &CDA_GUIDlg::OnEnChangeEditOfflinepath)
	ON_BN_CLICKED(IDC_RADIO_OFFLINE, &CDA_GUIDlg::OnBnClickedRadioOffline)
	ON_BN_CLICKED(IDC_RADIO_ONLINE, &CDA_GUIDlg::OnBnClickedRadioOnline)
	ON_BN_CLICKED(IDC_RADIO_LEARNING, &CDA_GUIDlg::OnBnClickedRadioLearning)
	ON_BN_CLICKED(IDC_RADIO_RECOGNITION, &CDA_GUIDlg::OnBnClickedRadioRecognition)
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
	//if(pMainThread->isObservingWindowVisible){
	//	CreateThread(NULL, 0, pMainThread->beginOpenGLSceneThread, (void*)pMainThread, 0, &OpenGLThreadID);
	//}
	DWORD CalculateThreadID;
	//CreateThread(NULL, 0, pMainThread->beginCalculationThread, (void*)pMainThread, 0, &CalculateThreadID);
	pMainThread->run();
	//Update the Dialog data
	UpdateData(true);
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
	pMainThread->isObservingWindowVisible = !(pMainThread->isObservingWindowVisible);
}

void CDA_GUIDlg::OnBnClickedCheckVisual2()
{
	pMainThread->isResultWindowVisible = !(pMainThread->isResultWindowVisible);
}

void CDA_GUIDlg::OnBnClickedCheckVisual3()
{
	pMainThread->isOpenCVWindowVisible = !(pMainThread->isOpenCVWindowVisible);
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




void CDA_GUIDlg::OnBnClickedRadioOffline()
{
	pMainThread->isOffline = true;
	pMainThread->isOnline = false;
}

void CDA_GUIDlg::OnBnClickedRadioOnline()
{
	pMainThread->isOnline = true;
	pMainThread->isOffline = false;
}

void CDA_GUIDlg::OnBnClickedRadioLearning()
{
	pMainThread->isLearning = true;
	pMainThread->isRecognise = false;
}

void CDA_GUIDlg::OnBnClickedRadioRecognition()
{
	pMainThread->isRecognise = true;
	pMainThread->isLearning = false;
}
