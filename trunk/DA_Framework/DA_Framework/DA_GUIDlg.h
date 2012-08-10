// DA_GUIDlg.h : Headerdatei
//

#pragma once

#include "MainThread.hpp"


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
	MainThread *pMainThread;

	bool radioOnline;
	bool radioOffline;
	CString offlinePath;
	// whether the real-time Observing Window is visible
	BOOL isObservingWindowVisible;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
