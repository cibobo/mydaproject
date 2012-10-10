// DA_GUIDlg.h : Headerdatei
//

#pragma once

#include "Configurator.hpp"
//#include "MainThread.hpp"
#include "afxcmn.h"
#include "afxwin.h"


// CDA_GUIDlg-Dialogfeld
class CDA_GUIDlg : public CDialog
{
// Konstruktion
public:
	CDA_GUIDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_DA_FRAMEWORK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// An Instance of Class MainThread, which include all necessary parameters of visualization
	MainThread *pMainThread;

	// Configurator for the Parameters' value, which have been saved in a Configuration TXT File
	Configurator *pConfigurator;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonPause();
	// Check box for the visibility of OpenGL Observing Window
	afx_msg void OnBnClickedCheckVisual1();
	// Check box for the visibility of OpenGL Result Window
	afx_msg void OnBnClickedCheckVisual2();
	// Check box for the visibility of OpenCV Help Window
	afx_msg void OnBnClickedCheckVisual3();
	// Control Variable for the Framerate Slider
	CSliderCtrl framerateSlider;
	// The Slider for the Framerate
	afx_msg void OnNMCustomdrawSliderFramerate(NMHDR *pNMHDR, LRESULT *pResult);
	// The Edit Control for the Framerate
	CEdit framerateEditor;
	afx_msg void OnEnChangeEditFramerate();
	// The Edit Control for the input data path for the offline mode
	CEdit inputPathEditor;
	afx_msg void OnEnChangeEditOfflinepath();
	// The Edit Control for the outputP path for the online mode
	CEdit outputPathEditor;

	// Radio Button for controlling the On and Offline mode
	afx_msg void OnBnClickedRadioOffline();
	afx_msg void OnBnClickedRadioOnline();
	// Radio Button for controlling the Learning and Recognition mode
	afx_msg void OnBnClickedRadioLearning();
	afx_msg void OnBnClickedRadioRecognition();
	// Button for saving the object into a VTK data
	afx_msg void OnBnClickedButtonSave();
	// Edit Control to set the name of the saving VTK datei of the object
	CEdit SavingNameEdit;
	// Funktion to check whether the saving name is empty
	afx_msg void OnEnChangeEditSavingname();
	// Button to save the result of learning
	CButton SaveButton;
	// Button to pause and continue the programm 
	CButton PauseButton;
	// The names of loading models for the recognition
	CEdit LoadingNameEdit;
	// Radio Buttons to change the different moduls of loading models
	afx_msg void OnBnClickedRadioRmodelTest();
	afx_msg void OnBnClickedRadioRmodelLoad();
	afx_msg void OnBnClickedRadioRmodelCreate();
	// To controll whether the input data should be saved or not
	CButton OutputSavedCheck;
	afx_msg void OnBnClickedCheckOnlinesaved();
	afx_msg void OnEnChangeEditOnlinepath();
	// The Button to run and stop the programm
	CButton RunButton;
	// Check box of the Rotation's Kalman Filter
	CButton RFilterControl;
	// Check box of the Translation's Kalman Filter
	CButton TFilterControl;
	afx_msg void OnBnClickedCheckTfilter();
};
