// DA_GUIDlg.h : Headerdatei
//

#pragma once

#include "MainThread.hpp"
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


	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedCheckVisual1();
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
	// The edit control for the outputP path for the online mode
	CEdit outputPathEditor;
};
