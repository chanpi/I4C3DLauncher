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
static VOID GetFileNameWithNoExtension(LPCTSTR szAppTitleWithExtension, LPTSTR szAppTitle, int length);

// �A�v���P�[�V�����̋N���Ɋւ���֐��i�����`���[�j
unsigned int __stdcall LaunchApplicationsProc(void *pParam);
unsigned int __stdcall LaunchProgramThreadProc(void *pParam);

namespace {
	// �v���Z�X�I�������ʂ���C�x���g
	static HANDLE g_hStopEvent = INVALID_HANDLE_VALUE;

	// RTT4EC���[�h�ŋN�����邩
	BOOL g_bRTT4ECMode = FALSE;
	// �R�A�A�v���P�[�V������
	LPCTSTR g_szCoreAppName = NULL;

	// �c�����v���Z�X���I������Ƃ��̏I���҂����Ԃ���ю��̃^�X�N���N������܂ł̃C���^�[�o��
	static int g_nTimeToWait = 1000;
	// �G���[���b�Z�[�W�}�b�v
	static map<int, tstring> g_ErrorMessageMap;

	const PCTSTR TAG_CORE_APP_NAME	= _T("core_app_name");
	const PCTSTR TAG_RTT4EC_MODE	= _T("rtt4ec_mode");
	const PCTSTR TAG_TIME_TO_WAIT	= _T("time_to_wait");
	const PCTSTR RTT4EC_STRING		= _T("rtt4ec");
	const tstring CRNL = _T("\r\n");
	const UINT g_uErrorDialogType = MB_OK | MB_ICONERROR | MB_TOPMOST;

	static BOOL g_bXMLFileError = FALSE;
};

extern TCHAR szTitle[];					// �^�C�g�� �o�[�̃e�L�X�g

extern I4C3DLauncherContext g_context;
extern AnalyzeXML g_analyzer;

extern CRITICAL_SECTION g_lock;
extern CRITICAL_SECTION g_dialogLock;

extern HINSTANCE hInst;
extern HWND g_hWnd;
// ���O�E�B���h�E
extern HWND g_hDlg;
// ���O�_�C�A���O�ɕ\������e�L�X�g
extern tstring g_statusString;

extern HANDLE g_hLauchApplicationThread;

BOOL Initialize(HWND hWnd)
{
	// �G���[���b�Z�[�W��StringTable����擾
	CreateErrorMessageMap();

	// stop�C�x���g�̍쐬
	g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hStopEvent == INVALID_HANDLE_VALUE) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_HANDLE_INVALID), GetLastError(), g_FILE, __LINE__);
		MessageBox(hWnd, g_ErrorMessageMap[EXIT_FAILURE].c_str(), szTitle, g_uErrorDialogType);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// �ݒ�t�@�C����̓N���X��������
	BOOL bFileExist = TRUE;
	if (!g_analyzer.LoadXML(SHARED_XML_FILE, &bFileExist)) {
		g_bXMLFileError = TRUE;
		if (bFileExist) {
			LoggingMessage(Log_Error, g_ErrorMessageMap[EXIT_INVALID_FILE_CONFIGURATION].c_str(), GetLastError(), __FILEW__, __LINE__);
			MessageBox(hWnd, g_ErrorMessageMap[EXIT_INVALID_FILE_CONFIGURATION].c_str(), szTitle, g_uErrorDialogType);
		} else {
			LoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_LOAD), GetLastError(), __FILEW__, __LINE__);
			MessageBox(hWnd, g_ErrorMessageMap[EXIT_FILE_NOT_FOUND].c_str(), szTitle, g_uErrorDialogType);
		}
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	g_context.pAnalyzer = &g_analyzer;

	g_nTimeToWait = _tstoi(g_analyzer.GetSoftValue(szTitle, TAG_TIME_TO_WAIT));
	g_statusString.append(_T("�N��������........\r\n"));
	SetDlgItemText(g_hDlg, IDC_EDIT1, g_statusString.c_str());
	InvalidateRect(g_hDlg, NULL, FALSE);

	// RTT4EC���[�h���m�F
	if (!_tcsicmp(g_analyzer.GetGlobalValue(TAG_RTT4EC_MODE), _T("on"))) {
		g_bRTT4ECMode = TRUE;
	}
	// �R�A�A�v���P�[�V���������m�F(�R�A�͊m���ɋN������K�v������)
	g_szCoreAppName = g_analyzer.GetGlobalValue(TAG_CORE_APP_NAME);
	if (!g_szCoreAppName) {
		LoggingMessage(Log_Error, _T(MESSAGE_ERROR_CFG_COREAPPNAME), GetLastError(), g_FILE, __LINE__);
		MessageBox(hWnd, _T(MESSAGE_ERROR_CFG_COREAPPNAME), szTitle, g_uErrorDialogType);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// �A�v���P�[�V�������N��
	g_hLauchApplicationThread = (HANDLE)_beginthreadex(NULL, 0, LaunchApplicationsProc, NULL, 0, NULL);
	
	return TRUE;
}

VOID UnInitialize(VOID)
{
	// Launch�������ׂẴv���Z�X���I��������
	// Launch��S���������ׂẴX���b�h�̏I�����m�F����
	ExitLauncher();
	EnterCriticalSection(&g_lock);
	WaitForMultipleObjects(g_context.nThreadCount, g_context.hThread, TRUE, INFINITE);
	for (int i = 0; i < g_context.nThreadCount; i++) {
		CloseHandle(g_context.hThread[i]);
		g_context.hThread[i] = INVALID_HANDLE_VALUE;
	}
	g_context.nThreadCount = 0;
	LeaveCriticalSection(&g_lock);

	// �X�g�b�v�C�x���g�̔j��
	CloseHandle(g_hStopEvent);
	g_hStopEvent = INVALID_HANDLE_VALUE;

	if (g_hDlg) {
		PostMessage(g_hDlg, WM_CLOSE, 0, 0);
	}

	// ���̃����`���[���I��(���̋@�\�̓R�A�����`���[�݂̂ɕK�v�ł�)
	if (g_bXMLFileError) {
		return;
	}
	TCHAR szKey[32] = {0};
	LPCTSTR szTargetLauncher = NULL;
	HWND hLauncherWnd = NULL;
	for (int i = 1; i <= MAX_TARGET_COUNT; i++) {
		_stprintf_s(szKey, _countof(szKey), _T("target_launcher%d"), i);
		szTargetLauncher = g_analyzer.GetSoftValue(szTitle, szKey);	// �N���t�@�C�����擾
		if (szTargetLauncher == NULL) {
			//break;
			continue;
		}
		hLauncherWnd = FindWindow(szTargetLauncher, NULL);
		PostMessage(hLauncherWnd, WM_CLOSE, 0, 0);
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

	LoadString(hInst, IDS_EXIT_RTT4EC_CONNECT_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_RTT4EC_CONNECT_ERROR] = szMessage;

	LoadString(hInst, IDS_EXIT_CORE_CONNECT_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_CORE_CONNECT_ERROR] = szMessage;

	LoadString(hInst, IDS_EXIT_NOT_EXECUTABLE, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_NOT_EXECUTABLE] = szMessage;
	LoadString(hInst, IDS_EXIT_SOME_ERROR, szMessage, _countof(szMessage));
	g_ErrorMessageMap[EXIT_SOME_ERROR] = szMessage;
}

VOID CloseProcess(LPCTSTR szApplicationName, HANDLE hProcess, DWORD dwProcessId)
{
	LPCTSTR szAppTitleWithExtension = PathFindFileName(szApplicationName);
	TCHAR szAppTitle[MAX_PATH] = {0};
	DWORD dwExitCode;
	GetFileNameWithNoExtension(szAppTitleWithExtension, szAppTitle, _countof(szAppTitle));
	// HWND���擾�ł���Ȃ�WM_CLOSE���b�Z�[�W�𑗐M
	if (szAppTitleWithExtension) {
		HWND hWnd = FindWindow(szAppTitle, NULL);
		if (hWnd) {
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}

	// �v���Z�X��񂪗^�����Ă���΁A�v���Z�X���I�����Ă��邩�m�F
	if (hProcess != NULL || hProcess != INVALID_HANDLE_VALUE) {
		GetExitCodeProcess(hProcess, &dwExitCode);
		if (dwExitCode == STILL_ACTIVE) {
			// taskkill������
			for (int i = 0; i < 5; i++) {
				ShellExecute(NULL, _T("open"), _T("taskkill"), tstring(_T("/IM ")).append(szAppTitleWithExtension).c_str(), NULL, SW_HIDE);
			}

			GetExitCodeProcess(hProcess, &dwExitCode);
			if (dwExitCode == STILL_ACTIVE) {
				// �v���Z�XID���^�����Ă���Ȃ�taskkill pid xxxx�A�^�����Ă��Ȃ����TerminatePorcess
				if (dwProcessId > 0) {
					TCHAR szPID[8];
					_stprintf_s(szPID, _countof(szPID), _T("%d"), dwProcessId);
					for (int i = 0; i < 5; i++) {
						ShellExecute(NULL, _T("open"), _T("taskkill"), tstring(_T("/pid ")).append(szPID).c_str(), NULL, SW_HIDE);
					}
				} else {
					for (int i = 0; i < 5; i++) {
						TerminateProcess(hProcess, EXIT_SUCCESS);
					}
				}
			}
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
	// Launch������s�t�@�C�����ƋN�����̈������擾
	// �����`���[�^�O��<key name="targetX">�N���t�@�C����</key>��ǂݎ��
	// <key name="�N���t�@�C����">����</key>
	TCHAR szKey[MAX_PATH] = {0};
	PCTSTR szTargetName = NULL, szTargetParams = NULL;
	for (int i = 1; i <= MAX_TARGET_COUNT; i++) {
		_stprintf_s(szKey, _countof(szKey), _T("target%d"), i);
		szTargetName = g_analyzer.GetSoftValue(szTitle, szKey);	// �N���t�@�C�����擾
		if (szTargetName == NULL) {
			//break;
			continue;
		}
		szTargetParams = g_analyzer.GetSoftValue(szTitle, szTargetName); // �����擾
		g_context.targetList.push_back(pair<tstring, tstring>(szTargetName, szTargetParams != NULL ? szTargetParams : _T("")));
	}

	// Launch�J�n
	UINT uThreadId;
	vector<pair<tstring, tstring>>::iterator it = g_context.targetList.begin();
	LPTSTR szCommandLine;
	LPCTSTR szContainsRTT4EC = NULL;
	LPCTSTR szContainsCoreApp = NULL;

	EnterCriticalSection(&g_lock);
	for (; it != g_context.targetList.end(); it++) {
		// �Y������v���O�C���ȊO�͋N�����Ȃ�
		szContainsRTT4EC = _tcsstr(it->first.c_str(), RTT4EC_STRING);
		szContainsCoreApp = _tcsstr(it->first.c_str(), g_szCoreAppName);
		if (!szContainsCoreApp) {
			if ((g_bRTT4ECMode && !szContainsRTT4EC) || (!g_bRTT4ECMode && szContainsRTT4EC)) {
				continue;
			}
		}

		CloseProcess(it->first.c_str(), NULL, 0);
		if (WaitForSingleObject(g_hStopEvent, g_nTimeToWait) != WAIT_TIMEOUT) {
			LeaveCriticalSection(&g_lock);
			return EXIT_FAILURE;
		}

		szCommandLine = new TCHAR[MAX_PATH];
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
	HANDLE waitHandle[2];
	BOOL isCreateProcessSucceeded;
	tstring szErrorMessage;
	tstring szDialogMessage;

	LPTSTR szCommandLine = (LPTSTR)pParam;
	TCHAR szApplicationName[MAX_PATH] = {0};
	LPCTSTR space = _tcschr((LPCTSTR)pParam, _T(' '));
	if (space != NULL) {
		_tcsncpy_s(szApplicationName, _countof(szApplicationName), szCommandLine, space - szCommandLine);
	} else {
		_tcscpy_s(szApplicationName, _countof(szApplicationName), szCommandLine);
	}
	szDialogMessage = szApplicationName;

	if (!PathFileExists(szApplicationName)) {
		PostMessage(g_hWnd, MY_LAUNCHFAILED, 0, 0);
		szDialogMessage.append(_T("���N���ł��܂���ł����B"));
		szDialogMessage.append(g_ErrorMessageMap[EXIT_FILE_NOT_FOUND]);
		LoggingMessage(Log_Error, szDialogMessage.c_str(), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, szDialogMessage.c_str(), szTitle, g_uErrorDialogType);
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
		delete pParam;
		return EXIT_FAILURE;
	}

	isCreateProcessSucceeded = CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

	EnterCriticalSection(&g_dialogLock);
	g_statusString.append(szApplicationName);
	g_statusString.append(_T("���N�����Ă��܂�........\r\n"));
	LeaveCriticalSection(&g_dialogLock);

	SetDlgItemText(g_hDlg, IDC_EDIT1, g_statusString.c_str());
	InvalidateRect(g_hDlg, NULL, FALSE);

	CloseHandle(pi.hThread);

	waitHandle[0] = g_hStopEvent;
	waitHandle[1] = pi.hProcess;

	WaitForMultipleObjects(2, waitHandle, FALSE, INFINITE);
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	if (dwExitCode == STILL_ACTIVE) {	// �I���C�x���g��ON�ɂȂ���
		// �v���Z�X���I��������
		if (pi.hProcess != 0 && pi.hProcess != INVALID_HANDLE_VALUE) {
			CloseProcess(szApplicationName, pi.hProcess, pi.dwProcessId);
		}

	} else if (!isCreateProcessSucceeded) {
		PostMessage(g_hWnd, MY_LAUNCHFAILED, 0, 0);

		// ���ׂẴv���O�����̎��s�𒆎~
		szDialogMessage.append(_T("���N���ł��܂���ł����B"));
		szDialogMessage.append(_T(MESSAGE_ERROR_SYSTEM_INIT));
		LoggingMessage(Log_Error, szDialogMessage.c_str(), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, szDialogMessage.c_str(), szTitle, g_uErrorDialogType);
		szDialogMessage.append(CRNL);

		EnterCriticalSection(&g_dialogLock);
		g_statusString.append(szDialogMessage);
		SetDlgItemText(g_hDlg, IDC_EDIT1, g_statusString.c_str());
		LeaveCriticalSection(&g_dialogLock);

		InvalidateRect(g_hDlg, NULL, FALSE);

		PostMessage(g_hWnd, WM_CLOSE, 0, 0);

	} else if (dwExitCode != EXIT_SUCCESS) {
		PostMessage(g_hWnd, MY_LAUNCHFAILED, 0, 0);

		szErrorMessage = g_ErrorMessageMap[dwExitCode];
		if (szErrorMessage.empty() || szErrorMessage == _T("") ) {
			szErrorMessage = g_ErrorMessageMap[EXIT_SOME_ERROR];
		}

		// ���ׂẴv���O�����̎��s�𒆎~
		szDialogMessage.append(_T("�ɃG���[���������܂����B"));
		szDialogMessage.append(szErrorMessage);
		LoggingMessage(Log_Error, szDialogMessage.c_str(), GetLastError(), g_FILE, __LINE__);
		MessageBox(g_hWnd, szDialogMessage.c_str(), szTitle, g_uErrorDialogType);
		szDialogMessage.append(CRNL);

		EnterCriticalSection(&g_dialogLock);
		g_statusString.append(szDialogMessage);
		SetDlgItemText(g_hDlg, IDC_EDIT1, g_statusString.c_str());
		LeaveCriticalSection(&g_dialogLock);

		InvalidateRect(g_hDlg, NULL, FALSE);
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
	}

	if (pi.hProcess != 0 && pi.hProcess != INVALID_HANDLE_VALUE) {
		CloseHandle(pi.hProcess);
	}

	EnterCriticalSection(&g_dialogLock);
	if (++g_context.nDiedThreadCount >= g_context.nThreadCount) {
		g_statusString.append(_T("�N���������ׂẴv���Z�X���I���������߁A�v���O�������I�����܂��B"));
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
	}
	LeaveCriticalSection(&g_dialogLock);

	delete pParam;
	return TRUE;
}