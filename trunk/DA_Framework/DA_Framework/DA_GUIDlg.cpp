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
	// create a instance for Configurator
	pConfigurator = new Configurator(this->pMainThread);

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

	// Check box to control, whether the online data should be saved or not
	DDX_Check(pDX, IDC_CHECK_ONLINESAVED, pMainThread->pPMDCamIO->isDataSaved);

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
	DDX_Text(pDX, IDC_EDIT_MINENERGY, pMainThread->pDetector->MINSTANDARDENERGY);
	DDX_Text(pDX, IDC_EDIT_MAXENERGY, pMainThread->pDetector->MAXSTANDARDENERGY);
	DDX_Text(pDX, IDC_EDIT_MINCONTRAST, pMainThread->pDetector->MINCONTRAST);
	DDX_Text(pDX, IDC_EDIT_MAXCONTRAST, pMainThread->pDetector->MAXCONTRAST);
	DDX_Text(pDX, IDC_EDIT_MINRSPTHRESHOLD, pMainThread->pDetector->MINRESPONSETHRESHOLD);
	DDX_Text(pDX, IDC_EDIT_MAXRSPTHRESHOLD, pMainThread->pDetector->MAXRESPONSETHRESHOLD);

	//Plane DBSCAN Parameters
	DDX_Text(pDX, IDC_EDIT_PEPS, pMainThread->pDetector->planeCombiEps);
	DDX_Text(pDX, IDC_EDIT_PMINPTS, pMainThread->pDetector->planeCombiMinPts);

	// Distance Filter Parameters
	DDX_Text(pDX, IDC_EDIT_DFVALUE, pMainThread->pDFilter->eps);
	DDX_Text(pDX, IDC_EDIT_DFPROPORTION, pMainThread->pDFilter->diffRate);
	DDX_Text(pDX, IDC_EDIT_DFFRAMES, pMainThread->pDFilter->creatingFrames);

	//Spatial DBSCAN Parameters
	DDX_Text(pDX, IDC_EDIT_SEPS, pMainThread->pLearning->spatialCombiEps);
	DDX_Text(pDX, IDC_EDIT_SMINPTS, pMainThread->pLearning->spatialCombiMinPts);

	//Association Parameters
	DDX_Text(pDX, IDC_EDIT_ASSVARIANCE, pMainThread->pLearning->associateVariance);
	DDX_Text(pDX, IDC_EDIT_ASSRATE, pMainThread->pLearning->associateRate);

	//Kalmanfilter's Controller
	DDX_Check(pDX, IDC_CHECK_TFILTER, pMainThread->pLearning->isTKFilter);
	DDX_Check(pDX, IDC_CHECK_RFILTER, pMainThread->pLearning->isQKFilter);

	DDX_Control(pDX, IDC_CHECK_RFILTER, RFilterControl);
	DDX_Control(pDX, IDC_CHECK_TFILTER, TFilterControl);

	//Graph Update Prameters
	DDX_Text(pDX, IDC_EDIT_GUPDATE_DT, pMainThread->pLearning->updateDThreshold);
	DDX_Text(pDX, IDC_EDIT_GUPDATE_TT, pMainThread->pLearning->updateTThreshold);

	//Frame Controll Parameters
	DDX_Text(pDX, IDC_EDIT_FC_DTHRESHOLD, pMainThread->pIterator->qualityCheckThreshold);
	DDX_Text(pDX, IDC_EDIT_FC_ARATE, pMainThread->pIterator->qualityCheckMinAcceptRate);
	DDX_Text(pDX, IDC_EDIT_FC_LDATABUFFER, pMainThread->pIterator->DATABUFFERLENGTH);
	DDX_Text(pDX, IDC_EDIT_FC_MJFEATURES, pMainThread->pIterator->MAXJUMPEDFEATURES);

	//Controllers for the Object Saving 
	DDX_Control(pDX, IDC_EDIT_SAVINGNAME, SavingNameEdit);
	DDX_Control(pDX, IDC_BUTTON_SAVE, SaveButton);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, PauseButton);

	//Graph Isomorphismus Prameters
	DDX_Text(pDX, IDC_EDIT_GISO_DPRO, pMainThread->pRecognition->distanceProportion);
	DDX_Text(pDX, IDC_EDIT_GISO_NPRO, pMainThread->pRecognition->nodesCountProportion);
	DDX_Text(pDX, IDC_EDIT_GISO_DT, pMainThread->pRecognition->distanceThreshold);

	//Model-input's mode
	DDX_Check(pDX, IDC_RADIO_RMODEL_TEST, pMainThread->pRecognition->isTest);
	DDX_Check(pDX, IDC_RADIO_RMODEL_LOAD, pMainThread->pRecognition->isLoad);
	DDX_Check(pDX, IDC_RADIO_RMODEL_CREATE, pMainThread->pRecognition->isCreate);

	DDX_Control(pDX, IDC_EDIT_MODELNAMES, LoadingNameEdit);
	DDX_Control(pDX, IDC_CHECK_ONLINESAVED, OutputSavedCheck);
	DDX_Control(pDX, IDOK, RunButton);

	DDX_Control(pDX, IDC_CHECK_EVA, ResultSavingControl);
	DDX_Control(pDX, IDC_EDIT_EVAFNAME, ResultPathEditor);
	DDX_Control(pDX, IDC_RADIO_EVA_SAVED, UsingDefaultSavePath);
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
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDA_GUIDlg::OnBnClickedButtonSave)
	ON_EN_CHANGE(IDC_EDIT_SAVINGNAME, &CDA_GUIDlg::OnEnChangeEditSavingname)
	ON_BN_CLICKED(IDC_RADIO_RMODEL_TEST, &CDA_GUIDlg::OnBnClickedRadioRmodelTest)
	ON_BN_CLICKED(IDC_RADIO_RMODEL_LOAD, &CDA_GUIDlg::OnBnClickedRadioRmodelLoad)
	ON_BN_CLICKED(IDC_RADIO_RMODEL_CREATE, &CDA_GUIDlg::OnBnClickedRadioRmodelCreate)
	ON_BN_CLICKED(IDC_CHECK_ONLINESAVED, &CDA_GUIDlg::OnBnClickedCheckOnlinesaved)
	ON_EN_CHANGE(IDC_EDIT_ONLINEPATH, &CDA_GUIDlg::OnEnChangeEditOnlinepath)
	ON_BN_CLICKED(IDC_CHECK_TFILTER, &CDA_GUIDlg::OnBnClickedCheckTfilter)
	ON_BN_CLICKED(IDC_CHECK_EVA, &CDA_GUIDlg::OnBnClickedCheckEva)
	ON_BN_CLICKED(IDC_RADIO_EVA_SAVED, &CDA_GUIDlg::OnBnClickedRadioEvaSaved)
	ON_EN_CHANGE(IDC_EDIT_EVAFNAME, &CDA_GUIDlg::OnEnChangeEditEvafname)
	ON_BN_CLICKED(IDC_RADIO_EVA_SAVEN, &CDA_GUIDlg::OnBnClickedRadioEvaSaven)
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

	// Read from Configuration File
	this->pConfigurator->readConfigurationFile();

	// Init Framerate Slider
	this->framerateSlider.SetRangeMin(FRAMERATE_MIN, false);
	this->framerateSlider.SetRangeMax(FRAMERATE_MAX, false);

	// Init Input Path with the default Data Source
	//this->inputPathEditor.SetWindowTextA(CString("Eva2Boxes"));
	this->inputPathEditor.SetWindowTextA(CString(this->pMainThread->INPUTPATH));

	// Init SaveButton. The defaut status is disable
	this->SaveButton.EnableWindow(false);

	// Init LoadingNameEdit. The defaut status is disable, if the load modul is not selected
	//this->LoadingNameEdit.SetWindowTextA("Box_all2,Box_all4");
	this->LoadingNameEdit.SetWindowTextA(CString(this->pConfigurator->modelNames.data()));
	if(!this->pMainThread->pRecognition->isLoad){
		this->LoadingNameEdit.EnableWindow(false);
	}

	// Init the Online input mode
	if(!this->pMainThread->pPMDCamIO->isDataSaved){
		this->OutputSavedCheck.EnableWindow(false);
		this->outputPathEditor.EnableWindow(false);
	} else {
		this->outputPathEditor.SetWindowTextA(CString(this->pMainThread->pPMDCamIO->savedPath));
	}

	//Init the CheckBox of the Kalman Filter
	this->RFilterControl.EnableWindow(false);

	//Init the Editor of the Result Path
	this->ResultPathEditor.EnableWindow(false);

	this->UsingDefaultSavePath.SetCheck(true);

	this->ResultSavingControl.SetCheck(true);

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
	if(pMainThread->bDone){
		pMainThread->bDone = false;

		//If in Recognitions modul
		if(pMainThread->isRecognise){
			//If the test modul has been selected
			if(pMainThread->pRecognition->isTest){
				pMainThread->pRecognition->loadModels();
			}

			//If the load modul has been selected
			if(pMainThread->pRecognition->isLoad){
				vector<string> namesVec;
				//Get the models' names
				CString str;
				this->LoadingNameEdit.GetWindowTextA(str);
				string names((LPCTSTR)str);
				int findPos;
				//find the names, which are separated with ','
				while((findPos=names.find(','))!=string::npos){
					namesVec.push_back(names.substr(0, findPos));
					names.erase(names.begin(), names.begin()+findPos+1);
				}
				if(names.size()>0){
					namesVec.push_back(names);
				}
				//Load models with these names
				pMainThread->pRecognition->loadModels(namesVec);
			}
		}
		pMainThread->run();
		//Set the Button to Stop
		this->RunButton.SetWindowTextA("Stop");
	} else {
		pMainThread->bDone = true;
		this->RunButton.SetWindowTextA("Run");
	}

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
		this->PauseButton.SetWindowTextA(CString("Pause"));
	} else {
		cout<<"frame pause!"<<endl;
		EnterCriticalSection(&(pMainThread->pauseCrs));
		pMainThread->isPause = true;
		this->PauseButton.SetWindowTextA(CString("Continue"));
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
	strcpy(pMainThread->INPUTPATH, str);
}

void CDA_GUIDlg::OnEnChangeEditOnlinepath()
{
	// TODO:  Falls dies ein RICHEDIT-Steuerelement ist, wird das Kontrollelement
	// diese Benachrichtigung nicht senden, es sei denn, Sie setzen den CDialog::OnInitDialog() außer Kraft.
	// Funktion und Aufruf CRichEditCtrl().SetEventMask()
	// mit dem ENM_CHANGE-Flag ORed in der Eingabe.


	CString str;
	this->outputPathEditor.GetWindowTextA(str);
	string path((LPCTSTR)str);
	strcpy(pMainThread->pPMDCamIO->savedPath, str);
		//pMainThread->pPMDCamIO->savedPath = path.data();

}

void CDA_GUIDlg::OnBnClickedRadioOffline()
{
	pMainThread->isOffline = true;
	pMainThread->isOnline = false;

	this->OutputSavedCheck.EnableWindow(false);

	this->inputPathEditor.EnableWindow(true);
}

void CDA_GUIDlg::OnBnClickedRadioOnline()
{
	pMainThread->isOnline = true;
	pMainThread->isOffline = false;

	this->OutputSavedCheck.EnableWindow(true);

	this->inputPathEditor.EnableWindow(false);
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

void CDA_GUIDlg::OnBnClickedButtonSave()
{
	CString str;
	this->SavingNameEdit.GetWindowTextA(str);
	//int length = str.GetLength();
	//const char* name;
	string name((LPCTSTR)str);
	this->pMainThread->pLearning->pObject->saveToVTKFile(name.data());
}

void CDA_GUIDlg::OnEnChangeEditSavingname()
{
	// TODO:  Falls dies ein RICHEDIT-Steuerelement ist, wird das Kontrollelement
	// diese Benachrichtigung nicht senden, es sei denn, Sie setzen den CDialog::OnInitDialog() außer Kraft.
	// Funktion und Aufruf CRichEditCtrl().SetEventMask()
	// mit dem ENM_CHANGE-Flag ORed in der Eingabe.

	CString str;
	this->SavingNameEdit.GetWindowTextA(str);
	if(str.GetLength()<1){
		this->SaveButton.EnableWindow(false);
	} else {
		this->SaveButton.EnableWindow(true);
	}
}

void CDA_GUIDlg::OnBnClickedRadioRmodelTest()
{
	this->pMainThread->pRecognition->isTest = true;
	this->pMainThread->pRecognition->isLoad = false;
	this->pMainThread->pRecognition->isCreate = false;

	this->LoadingNameEdit.EnableWindow(false);
}

void CDA_GUIDlg::OnBnClickedRadioRmodelLoad()
{
	this->pMainThread->pRecognition->isTest = false;
	this->pMainThread->pRecognition->isLoad = true;
	this->pMainThread->pRecognition->isCreate = false;

	this->LoadingNameEdit.EnableWindow(true);
}

void CDA_GUIDlg::OnBnClickedRadioRmodelCreate()
{
	this->pMainThread->pRecognition->isTest = false;
	this->pMainThread->pRecognition->isLoad = false;
	this->pMainThread->pRecognition->isCreate = true;

	this->LoadingNameEdit.EnableWindow(false);
	this->pMainThread->isLearning = true;
	this->pMainThread->isRecognise = false;
}

void CDA_GUIDlg::OnBnClickedCheckOnlinesaved()
{
	pMainThread->pPMDCamIO->isDataSaved = this->OutputSavedCheck.GetCheck();
	this->outputPathEditor.EnableWindow(pMainThread->pPMDCamIO->isDataSaved);
}



void CDA_GUIDlg::OnBnClickedCheckTfilter()
{
	this->pMainThread->pLearning->isTKFilter = this->TFilterControl.GetCheck();
}

void CDA_GUIDlg::OnBnClickedCheckEva()
{
	this->pMainThread->isResultSaved = this->ResultSavingControl.GetCheck();
}

void CDA_GUIDlg::OnBnClickedRadioEvaSaved()
{
	this->pMainThread->resultSavePath = "RecognitionResult/";
	this->ResultPathEditor.EnableWindow(false);
}

void CDA_GUIDlg::OnEnChangeEditEvafname()
{
	// TODO:  Falls dies ein RICHEDIT-Steuerelement ist, wird das Kontrollelement
	// diese Benachrichtigung nicht senden, es sei denn, Sie setzen den CDialog::OnInitDialog() außer Kraft.
	// Funktion und Aufruf CRichEditCtrl().SetEventMask()
	// mit dem ENM_CHANGE-Flag ORed in der Eingabe.

	CString str;
	this->ResultPathEditor.GetWindowTextA(str);
	strcpy(this->pMainThread->resultSavePath, str);
}

void CDA_GUIDlg::OnBnClickedRadioEvaSaven()
{
	this->ResultPathEditor.EnableWindow(true);
}
