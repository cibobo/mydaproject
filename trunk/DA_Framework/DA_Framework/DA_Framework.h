// DA_Framework.h : Hauptheaderdatei f�r die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "\"stdafx.h\" vor dieser Datei f�r PCH einschlie�en"
#endif

#include "resource.h"		// Hauptsymbole


// CDA_FrameworkApp:
// Siehe DA_Framework.cpp f�r die Implementierung dieser Klasse
//

class CDA_FrameworkApp : public CWinApp
{
public:
	CDA_FrameworkApp();

// �berschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CDA_FrameworkApp theApp;