// DA_Framework.h : Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "\"stdafx.h\" vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CDA_FrameworkApp:
// Siehe DA_Framework.cpp für die Implementierung dieser Klasse
//

class CDA_FrameworkApp : public CWinApp
{
public:
	CDA_FrameworkApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CDA_FrameworkApp theApp;