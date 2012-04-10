#include "stdafx.h"
#include "I4C3DLauncher.h"
#include "I4C3DLaunchController.h"
#include "AnalyzeXML.h"
#include "resource.h"

#include <map>
using namespace std;

#if UNICODE || _UNICODE
static LPCTSTR g_FILE = __FILEW__;
#else
static LPCTSTR g_FILE = __FILE__;
#endif

static VOID CreateErrorMessageMap(VOID);
static VOID CloseProcess(LPCTSTR szApplicationName, HANDLE hProcess, DWORD dwProcessId);
static VOID TerminateOtherLaucher();
static VOID GetFileNameWithNoExtension(LPCTSTR szAppTitleWithExtension, LPTSTR szAppTitle, int length);

// アプリケーションの起動に関する関数（ランチャー）
unsigned int __stdcall LaunchApplicationsProc(void *pParam);
unsigned int __stdcall LaunchProgramThreadProc(void *pParam);

namespace {
	static AnalyzeXML* g_pAnalyzer = NULL;

	// プロセス終了を識別するイベント
	static HANDLE g_hStopEvent = INVALID_HANDLE_VALUE;

	// RTTECモードで起動するか
	BOOL g_bRTTECMode = FALSE;
	// コアアプリケーション名
	LPCTSTR g_szCoreAppName = NULL;

	// 残ったプロセスを終了するときの終了待ち時間および次のタスクを起動するまでのインターバル
	static int g_nTimeToWait = 1000;
	// エラーメッセージマップ
	static map<int, tstring> g_ErrorMessageMap;

	const PCTSTR TAG_CORE_APP_NAME	= _T("core_app_name");
	const PCTSTR TAG_RTTEC_MODE		= _T("rttec_mode");
	const PCTSTR TAG_TIME_TO_WAIT	= _T("time_to_wait");
	const PCTSTR TAG_SPLASHWINDOW	= _T("splashwindow");
	const PCTSTR RTTEC_STRING		= _T("rttec");
	const UINT g_uErrorDialogType = MB_OK | MB_ICONERROR | MB_TOPMOST;

	static BOOL g_bXMLFileError = FALSE;
};

extern TCHAR szTitle[];					// タイトル バーのテキスト

extern I4C3DLauncherContext g_context;
extern I4C3DLoadLibrary g_loadLibrary;

extern CRITICAL_SECTION g_lock;
extern CRITICAL_SECTION g_dialogLock;

extern HINSTANCE hInst;
extern HWND g_hWnd;
// ダイアログウィンドウ
extern HWND g_hDlg;

extern HANDLE g_hLauchApplicationThread;

BOOL Initialize(VOID)
{
	// エラーメッセージをStringTableから取得
	CreateErrorMessageMap();

	// stopイベントの作成
	g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hStopEvent == INVALID_HANDLE_VALUE) {
		g_loadLibrary.LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_HANDLE_INVALID), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, g_ErrorMessageMap[EXIT_FAILURE].c_str(), szTitle, g_uErrorDialogType);
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// 設定ファイル解析クラスを初期化
	BOOL bFileExist = TRUE;
	g_pAnalyzer = new AnalyzeXML(&g_loadLibrary);
	if (!g_pAnalyzer) {
		g_loadLibrary.LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_SYSTEM_INIT), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, g_ErrorMessageMap[EXIT_FAILURE].c_str(), szTitle, g_uErrorDialogType);
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	if (!g_pAnalyzer->LoadXML(SHARED_XML_FILE, &bFileExist)) {
		g_bXMLFileError = TRUE;
		if (bFileExist) {
			g_loadLibrary.LPFNLoggingMessage(Log_Error, g_ErrorMessageMap[EXIT_INVALID_FILE_CONFIGURATION].c_str(), GetLastError(), __FILEW__, __LINE__);
			MessageBox(g_hWnd, g_ErrorMessageMap[EXIT_INVALID_FILE_CONFIGURATION].c_str(), szTitle, g_uErrorDialogType);
		} else {
			g_loadLibrary.LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_LOAD), GetLastError(), __FILEW__, __LINE__);
			MessageBox(g_hWnd, g_ErrorMessageMap[EXIT_FILE_NOT_FOUND].c_str(), szTitle, g_uErrorDialogType);
		}
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	g_context.pAnalyzer = g_pAnalyzer;

	// スプラッシュウィンドウプログラムスタート
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb = sizeof(STARTUPINFO);
	TCHAR szSplashWindow[BUFFER_SIZE] = {0};
	_tcscpy_s(szSplashWindow, _countof(szSplashWindow), g_pAnalyzer->GetSoftValue(szTitle, TAG_SPLASHWINDOW));
	if (szSplashWindow) {
		CreateProcess(NULL, szSplashWindow, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	g_nTimeToWait = _tstoi(g_pAnalyzer->GetSoftValue(szTitle, TAG_TIME_TO_WAIT));

	// RTTECモードか確認
	if (!_tcsicmp(g_pAnalyzer->GetGlobalValue(TAG_RTTEC_MODE), _T("on"))) {
		g_bRTTECMode = TRUE;
	}
	// コアアプリケーション名を確認(コアは確実に起動する必要がある)
	g_szCoreAppName = g_pAnalyzer->GetGlobalValue(TAG_CORE_APP_NAME);
	if (!g_szCoreAppName) {
		g_loadLibrary.LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_CFG_COREAPPNAME), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, _T(MESSAGE_ERROR_CFG_COREAPPNAME), szTitle, g_uErrorDialogType);
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// 他のランチャーを終了(この機能はコアランチャーのみに必要です)
	TerminateOtherLaucher();

	// アプリケーションを起動
	g_hLauchApplicationThread = (HANDLE)_beginthreadex(NULL, 0, LaunchApplicationsProc, NULL, 0, NULL);
	
	return TRUE;
}

VOID UnInitialize(VOID)
{
	// Launchしたすべてのプロセスを終了させる
	// Launchを担当したすべてのスレッドの終了を確認する
	ExitLauncher();
	EnterCriticalSection(&g_lock);
	WaitForMultipleObjects(g_context.nThreadCount, g_context.hThread, TRUE, INFINITE);
	for (int i = 0; i < g_context.nThreadCount; i++) {
		CloseHandle(g_context.hThread[i]);
		g_context.hThread[i] = INVALID_HANDLE_VALUE;
	}
	g_context.nThreadCount = 0;
	LeaveCriticalSection(&g_lock);

	// ストップイベントの破棄
	CloseHandle(g_hStopEvent);
	g_hStopEvent = INVALID_HANDLE_VALUE;

	//if (g_hDlg) {
	//	PostMessage(g_hDlg, WM_CLOSE, 0, 0);
	//}

	// 他のランチャーを終了(この機能はコアランチャーのみに必要です)
	TerminateOtherLaucher();
	delete g_pAnalyzer;
	g_pAnalyzer = NULL;
}

VOID TerminateOtherLaucher(VOID)
{
	if (g_bXMLFileError) {
		return;
	}
	TCHAR szKey[32] = {0};
	LPCTSTR szTargetLauncher = NULL;
	HWND hLauncherWnd = NULL;
	for (int i = 1; i <= MAX_TARGET_COUNT; i++) {
		_stprintf_s(szKey, _countof(szKey), _T("target_launcher%d"), i);
		if (g_pAnalyzer) {
			szTargetLauncher = g_pAnalyzer->GetSoftValue(szTitle, szKey);	// 起動ファイル名取得
			if (szTargetLauncher == NULL) {
				//break;
				continue;
			}
			hLauncherWnd = FindWindow(szTargetLauncher, NULL);
			PostMessage(hLauncherWnd, WM_CLOSE, 0, 0);
		}
	}	
}

VOID ExitLauncher(VOID)
{
	if (g_hStopEvent != INVALID_HANDLE_VALUE) {
		SetEvent(g_hStopEvent);
	}
}

VOID CreateErrorMessageMap(VOID)
{
	TCHAR szMessage[BUFFER_SIZE];
	LoadString(hInst, IDS_EXIT_SYSTEM_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_SYSTEM_ERROR] = szMessage;

	LoadString(hInst, IDS_EXIT_BAD_ARGUMENTS, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_BAD_ARGUMENTS] = szMessage;
	LoadString(hInst, IDS_EXIT_NO_ARGUMENTS, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_NO_ARGUMENTS] = szMessage;
	
	LoadString(hInst, IDS_EXIT_SOCKET_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_SOCKET_ERROR] = szMessage;
	LoadString(hInst, IDS_EXIT_SOCKET_CONNECT_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_SOCKET_CONNECT_ERROR] = szMessage;
	LoadString(hInst, IDS_EXIT_SOCKET_BIND_LISTEN_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_SOCKET_BIND_LISTEN_ERROR] = szMessage;

	LoadString(hInst, IDS_EXIT_FILE_NOT_FOUND, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_FILE_NOT_FOUND] = szMessage;
	LoadString(hInst, IDS_EXIT_INVALID_FILE_CONFIGURATION, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_INVALID_FILE_CONFIGURATION] = szMessage;

	LoadString(hInst, IDS_EXIT_DEVICE_NOT_FOUND, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_DEVICE_NOT_FOUND] = szMessage;
	LoadString(hInst, IDS_EXIT_GAMEPAD_NOT_FOUND, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_GAMEPAD_NOT_FOUND] = szMessage;
	LoadString(hInst, IDS_EXIT_GAMEPAD_SETUP_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_GAMEPAD_SETUP_ERROR] = szMessage;

	LoadString(hInst, IDS_EXIT_RTTEC_CONNECT_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_RTTEC_CONNECT_ERROR] = szMessage;

	LoadString(hInst, IDS_EXIT_CORE_CONNECT_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_CORE_CONNECT_ERROR] = szMessage;

	// ライセンス系
	LoadString(hInst, IDS_EXIT_CERT_UNINITIALIZED, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_CERT_UNINITIALIZED] = szMessage;
	LoadString(hInst, IDS_EXIT_CERT_INVALID_MACADDRESS, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_CERT_INVALID_MACADDRESS] = szMessage;
	LoadString(hInst, IDS_EXIT_CERT_INVALID_EXPIRE_DATE, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_CERT_INVALID_EXPIRE_DATE] = szMessage;
	LoadString(hInst, IDS_EXIT_CERT_FILE_NOT_FOUND, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_CERT_FILE_NOT_FOUND] = szMessage;
	LoadString(hInst, IDS_EXIT_CERT_SYSTEM_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_CERT_SYSTEM_ERROR] = szMessage;

	LoadString(hInst, IDS_EXIT_NOT_EXECUTABLE, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_NOT_EXECUTABLE] = szMessage;
	LoadString(hInst, IDS_EXIT_SOME_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_SOME_ERROR] = szMessage;
}

VOID CloseProcess(LPCTSTR szApplicationName, HANDLE hProcess, DWORD dwProcessId)
{
	LPCTSTR szAppTitleWithExtension = PathFindFileName(szApplicationName);
	TCHAR szAppTitle[MAX_PATH] = {0};
	DWORD dwExitCode = EXIT_SUCCESS;
	GetFileNameWithNoExtension(szAppTitleWithExtension, szAppTitle, _countof(szAppTitle));
	// HWNDを取得できるならWM_CLOSEメッセージを送信
	if (szAppTitleWithExtension) {
		HWND hWnd = FindWindow(szAppTitle, NULL);
		if (hWnd) {
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}

	// プロセス情報が与えられていれば、プロセスが終了しているか確認
	if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE) {
		GetExitCodeProcess(hProcess, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE) {
			return;
		}
	}

	// taskkillを試す
	for (int i = 0; i < 5; i++) {
		ShellExecute(NULL, _T("open"), _T("taskkill"), tstring(_T("/IM ")).append(szAppTitleWithExtension).c_str(), NULL, SW_HIDE);
	}

	// プロセス情報が与えられていれば、プロセスが終了しているか確認
	if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE) {
		GetExitCodeProcess(hProcess, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE) {
			return;
		}
	}

	// プロセスIDが与えられているならtaskkill pid xxxx、与えられていなければTerminatePorcess
	if (dwProcessId > 0) {
		TCHAR szPID[8] = {0};
		_stprintf_s(szPID, _countof(szPID), _T("%u"), dwProcessId);
		for (int i = 0; i < 5; i++) {
			ShellExecute(NULL, _T("open"), _T("taskkill"), tstring(_T("/pid ")).append(szPID).c_str(), NULL, SW_HIDE);
		}
	}
	if (hProcess) {
		for (int i = 0; i < 5; i++) {
			TerminateProcess(hProcess, EXIT_SUCCESS);
		}
	}
}

VOID GetFileNameWithNoExtension(LPCTSTR szAppTitleWithExtension, LPTSTR szAppTitle, int length)
{
	LPCTSTR szFileName = PathFindFileName(szAppTitleWithExtension);
	if (szFileName && szAppTitle) {
		_tcscpy_s(szAppTitle, length, szFileName);

		LPTSTR pExtension = _tcschr(szAppTitle, _T('.'));
		if (pExtension) {
			*pExtension = _T('\0');
		}
	}
}

unsigned int __stdcall LaunchApplicationsProc(void *pParam)
{
	// Launchする実行ファイル名と起動時の引数を取得
	// ランチャータグの<key name="targetX">起動ファイル名</key>を読み取る
	// <key name="起動ファイル名">引数</key>
	TCHAR szKey[MAX_PATH] = {0};
	PCTSTR szTargetName = NULL;
	PCTSTR szTargetParams = NULL;
	for (int i = 1; i <= MAX_TARGET_COUNT; i++) {
		_stprintf_s(szKey, _countof(szKey), _T("target%d"), i);
		szTargetName = g_pAnalyzer->GetSoftValue(szTitle, szKey);	// 起動ファイル名取得
		if (szTargetName == NULL) {
			//break;
			continue;
		}
		szTargetParams = g_pAnalyzer->GetSoftValue(szTitle, szTargetName); // 引数取得
		g_context.targetList.push_back(pair<tstring, tstring>(szTargetName, szTargetParams != NULL ? szTargetParams : _T("")));
	}

	// Launch開始
	UINT uThreadId = 0;
	vector<pair<tstring, tstring>>::reverse_iterator rit = g_context.targetList.rbegin();	// 終了処理用の逆イテレータ
	vector<pair<tstring, tstring>>::iterator it = g_context.targetList.begin();
	LPTSTR szCommandLine = NULL;
	LPCTSTR szContainsRTTEC = NULL;
	LPCTSTR szContainsCoreApp = NULL;


	// まずは残っているプロセスをすべて終了させる
	EnterCriticalSection(&g_lock);
	for (; rit != g_context.targetList.rend(); ++rit) {
		CloseProcess(rit->first.c_str(), NULL, 0);
	}
	LeaveCriticalSection(&g_lock);

	EnterCriticalSection(&g_lock);
	for (; it != g_context.targetList.end(); it++) {
		// 該当するプラグイン以外は起動しない
		szContainsRTTEC = _tcsstr(it->first.c_str(), RTTEC_STRING);
		szContainsCoreApp = _tcsstr(it->first.c_str(), g_szCoreAppName);
		if (!szContainsCoreApp) {
			if ((g_bRTTECMode && !szContainsRTTEC) || (!g_bRTTECMode && szContainsRTTEC)) {
				continue;
			}
		}

		if (WaitForSingleObject(g_hStopEvent, g_nTimeToWait) != WAIT_TIMEOUT) {
			LeaveCriticalSection(&g_lock);
			return EXIT_FAILURE;
		}

		szCommandLine = new TCHAR[MAX_PATH];
		if (szCommandLine == NULL) {
			g_loadLibrary.LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_MEMORY_INVALID), GetLastError(), g_FILE, __LINE__);
			MessageBox(g_hWnd, g_ErrorMessageMap[EXIT_FAILURE].c_str(), szTitle, g_uErrorDialogType);
			PostMessage(g_hWnd, WM_CLOSE, 0, 0);
			return FALSE;
		}
		ZeroMemory(szCommandLine, sizeof(TCHAR) * MAX_PATH);
		_stprintf_s(szCommandLine, MAX_PATH, _T("%s %s"), it->first.c_str(), it->second.c_str());
		g_context.hThread[g_context.nThreadCount++] = (HANDLE)_beginthreadex(NULL, 0, LaunchProgramThreadProc, (void*)szCommandLine, 0, &uThreadId);
	}
	LeaveCriticalSection(&g_lock);
	return EXIT_SUCCESS;
}

unsigned int __stdcall LaunchProgramThreadProc(void *pParam)
{
	DWORD dwExitCode = EXIT_SUCCESS;
	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {0};
	HANDLE waitHandle[2] = {0};
	BOOL isCreateProcessSucceeded = NULL;
	tstring szErrorMessage(_T(""));
	tstring szDialogMessage(_T(""));

	LPTSTR szCommandLine = (LPTSTR)pParam;
	TCHAR szApplicationName[MAX_PATH] = {0};
	LPCTSTR space = _tcschr((LPCTSTR)pParam, _T(' '));
	if (space != NULL) {
		_tcsncpy_s(szApplicationName, _countof(szApplicationName), szCommandLine, space - szCommandLine);
	} else {
		_tcscpy_s(szApplicationName, _countof(szApplicationName), szCommandLine);
	}
	szDialogMessage = szApplicationName;
	szDialogMessage.append(_T(": "));

	if (!PathFileExists(szApplicationName)) {
		PostMessage(g_hWnd, MY_LAUNCHFAILED, 0, 0);
		szDialogMessage.append(g_ErrorMessageMap[EXIT_FILE_NOT_FOUND]);
		g_loadLibrary.LPFNLoggingMessage(Log_Error, szDialogMessage.c_str(), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, g_ErrorMessageMap[EXIT_FILE_NOT_FOUND].c_str(), szTitle, g_uErrorDialogType);
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
		delete pParam;
		return EXIT_FAILURE;
	}

	isCreateProcessSucceeded = CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);

	waitHandle[0] = g_hStopEvent;
	waitHandle[1] = pi.hProcess;

	WaitForMultipleObjects(2, waitHandle, FALSE, INFINITE);
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	if (dwExitCode == STILL_ACTIVE) {	// 終了イベントがONになった
		// プロセスを終了させる
		if (pi.hProcess != 0 && pi.hProcess != INVALID_HANDLE_VALUE) {
			CloseProcess(szApplicationName, pi.hProcess, pi.dwProcessId);
		}

	} else if (!isCreateProcessSucceeded) {
		PostMessage(g_hWnd, MY_LAUNCHFAILED, 0, 0);

		// すべてのプログラムの実行を中止
		szDialogMessage.append(_T(MESSAGE_ERROR_SYSTEM_INIT));
		g_loadLibrary.LPFNLoggingMessage(Log_Error, szDialogMessage.c_str(), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, _T(MESSAGE_ERROR_SYSTEM_INIT), szTitle, g_uErrorDialogType);

		PostMessage(g_hWnd, WM_CLOSE, 0, 0);

	} else if (dwExitCode != EXIT_SUCCESS) {
		PostMessage(g_hWnd, MY_LAUNCHFAILED, 0, 0);

		szErrorMessage = g_ErrorMessageMap[dwExitCode];
		if (szErrorMessage.empty() || szErrorMessage == _T("") ) {
			szErrorMessage = g_ErrorMessageMap[EXIT_SOME_ERROR];
		}

		// すべてのプログラムの実行を中止
		szDialogMessage.append(szErrorMessage);
		g_loadLibrary.LPFNLoggingMessage(Log_Error, szDialogMessage.c_str(), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, szErrorMessage.c_str(), szTitle, g_uErrorDialogType);
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
	}

	if (pi.hProcess != 0 && pi.hProcess != INVALID_HANDLE_VALUE) {
		CloseHandle(pi.hProcess);
	}

	EnterCriticalSection(&g_dialogLock);
	if (++g_context.nDiedThreadCount >= g_context.nThreadCount) {
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
	}
	LeaveCriticalSection(&g_dialogLock);

	delete pParam;
	return TRUE;
}