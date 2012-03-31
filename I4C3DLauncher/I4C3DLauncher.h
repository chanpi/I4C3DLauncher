#pragma once

#include "resource.h"
#include <vector>
#include "stdafx.h"

#define MAX_LOADSTRING		100
#define BUFFER_SIZE			256
#define MAX_TARGET_COUNT	20

#define MY_LAUNCHFAILED	(WM_APP+2)

class AnalyzeXML;

typedef struct I4C3DLauncherContext {
	AnalyzeXML* pAnalyzer;
	vector<pair<tstring, tstring>> targetList;
	HANDLE hThread[MAX_TARGET_COUNT];
	int nThreadCount;
	int nDiedThreadCount;
} I4C3DLauncherContext;

