#include "StdAfx.h"
#include "Misc.h"
#include "I4C3DLoadLibrary.h"

namespace {
	static PCTSTR g_szMiscPath = _T("mod\\Misc.dll");
	static PCTSTR g_szXMLParserPath = _T("mod\\XMLParser.dll");
};

I4C3DLoadLibrary::I4C3DLoadLibrary(void):
m_hMiscModule(NULL), m_hXMLParserModule(NULL),
LPFNExecuteOnce(NULL), LPFNCleanupMutex(NULL), LPFNLogFileOpenW(NULL), LPFNLogFileCloseW(NULL),
LPFNLoggingMessageW(NULL), LPFNLogFileOpenA(NULL), LPFNLogFileCloseA(NULL), LPFNLoggingMessageA(NULL),
LPFNInitialize(NULL), LPFNUnInitialize(NULL), LPFNStoreValues(NULL), LPFNCleanupStoredValues(NULL),
LPFNGetDOMTree(NULL), LPFNFreeDOMTree(NULL), LPFNGetChildList(NULL), LPFNFreeChildList(NULL),
LPFNGetListItem(NULL), LPFNFreeListItem(NULL), LPFNGetAttribute(NULL), LPFNGetMapItem(NULL)
{
}


I4C3DLoadLibrary::~I4C3DLoadLibrary(void)
{
}

BOOL I4C3DLoadLibrary::LoadLibraries(void)
{
	if (!LoadMiscFunctions()) {
		return FALSE;
	}
	if (!LoadXMLParserFunctions()) {
		FreeMiscFunctions();
		return FALSE;
	}
	return TRUE;
}

void I4C3DLoadLibrary::FreeLibraries(void)
{
	FreeMiscFunctions();
	FreeXMLParserFunctions();
}

BOOL I4C3DLoadLibrary::LoadMiscFunctions(void)
{
	// Misc系関数のアドレスを取得する
	if (m_hMiscModule) {
		FreeMiscFunctions();
	}
	m_hMiscModule = ::LoadLibrary(g_szMiscPath);
	if (!m_hMiscModule) {
		return FALSE;
	}

	LPFNExecuteOnce = (BOOL (WINAPI *)(LPCTSTR))GetProcAddress(m_hMiscModule, "ExecuteOnce");
	if (!LPFNExecuteOnce) {
		OutputDebugString(_T("Load ExecuteOnce Failed.\n"));
		return FALSE;
	}

	LPFNCleanupMutex = (void (WINAPI *)(void))GetProcAddress(m_hMiscModule, "CleanupMutex");
	if (!LPFNCleanupMutex) {
		OutputDebugString(_T("Load CleanupMutex Failed.\n"));
		return FALSE;
	}

	LPFNLogFileOpenW = (BOOL (WINAPI *)(PCSTR, PCSTR, LOG_LEVEL))GetProcAddress(m_hMiscModule, "LogFileOpenW");
	if (!LPFNLogFileOpenW) {
		OutputDebugString(_T("Load LogFileOpenW Failed.\n"));
		return FALSE;
	}

	LPFNLogFileCloseW = (BOOL (WINAPI *)(void))GetProcAddress(m_hMiscModule, "LogFileCloseW");
	if (!LPFNLogFileCloseW) {
		OutputDebugString(_T("Load LogFileCloseW Failed.\n"));
		return FALSE;
	}

	LPFNLoggingMessageW = (void (WINAPI *)(LOG_LEVEL, LPCWSTR, DWORD, LPCWSTR, int))GetProcAddress(m_hMiscModule, "LoggingMessageW");
	if (!LPFNLoggingMessageW) {
		OutputDebugString(_T("Load LoggingMessageW Failed.\n"));
		return FALSE;
	}

	LPFNLogFileOpenA = (BOOL (WINAPI *)(PCSTR, PCSTR, LOG_LEVEL))GetProcAddress(m_hMiscModule, "LogFileOpenA");
	if (!LPFNLogFileOpenA) {
		OutputDebugString(_T("Load LogFileOpenA Failed.\n"));
		return FALSE;
	}

	LPFNLogFileCloseA = (BOOL (WINAPI *)(void))GetProcAddress(m_hMiscModule, "LogFileCloseA");
	if (!LPFNLogFileCloseA) {
		OutputDebugString(_T("Load LogFileCloseA Failed.\n"));
		return FALSE;
	}

	LPFNLoggingMessageA = (void (WINAPI *)(LOG_LEVEL, LPCSTR, DWORD, LPCSTR, int))GetProcAddress(m_hMiscModule, "LoggingMessageA");
	if (!LPFNLoggingMessageA) {
		OutputDebugString(_T("Load LoggingMessageA Failed.\n"));
		return FALSE;
	}

	return TRUE;
}

BOOL I4C3DLoadLibrary::LoadXMLParserFunctions(void)
{
	// XMLParser系関数のアドレスを取得する
	if (m_hXMLParserModule) {
		FreeXMLParserFunctions();
	}
	m_hXMLParserModule = ::LoadLibrary(g_szXMLParserPath);
	if (!m_hXMLParserModule) {
		return FALSE;
	}

	LPFNInitialize = (BOOL (WINAPI *)(IXMLDOMElement**, PCTSTR))GetProcAddress(m_hXMLParserModule, "Initialize");
	if (!LPFNInitialize) {
		OutputDebugString(_T("Load Initialize Failed.\n"));
		return FALSE;
	}

	LPFNUnInitialize = (void (WINAPI *)(IXMLDOMElement*))GetProcAddress(m_hXMLParserModule, "UnInitialize");
	if (!LPFNUnInitialize) {
		OutputDebugString(_T("Load UnInitialize Failed.\n"));
		return FALSE;
	}

	LPFNStoreValues = (map<PCTSTR, PCTSTR>* (WINAPI *)(IXMLDOMNode*, PCTSTR))GetProcAddress(m_hXMLParserModule, "StoreValues");
	if (!LPFNStoreValues) {
		OutputDebugString(_T("Load StoreValues Failed.\n"));
		return FALSE;
	}

	LPFNCleanupStoredValues = (void (WINAPI *)(map<PCTSTR, PCTSTR>*))GetProcAddress(m_hXMLParserModule, "CleanupStoredValues");
	if (!LPFNCleanupStoredValues) {
		OutputDebugString(_T("Load CleanupStoredValues Failed.\n"));
		return FALSE;
	}

	LPFNGetDOMTree = (BOOL (WINAPI *)(IXMLDOMElement*, PCTSTR, IXMLDOMNode**))GetProcAddress(m_hXMLParserModule, "GetDOMTree");
	if (!LPFNGetDOMTree) {
		OutputDebugString(_T("Load GetDOMTree Failed.\n"));
		return FALSE;
	}

	LPFNFreeDOMTree = (void (WINAPI *)(IXMLDOMNode*))GetProcAddress(m_hXMLParserModule, "FreeDOMTree");
	if (!LPFNFreeDOMTree) {
		OutputDebugString(_T("Load FreeDOMTree Failed.\n"));
		return FALSE;
	}

	LPFNGetChildList = (LONG (WINAPI *)(IXMLDOMNode*, IXMLDOMNodeList**))GetProcAddress(m_hXMLParserModule, "GetChildList");
	if (!LPFNGetChildList) {
		OutputDebugString(_T("Load GetChildList Failed.\n"));
		return FALSE;
	}

	LPFNFreeChildList = (void (WINAPI *)(IXMLDOMNodeList*))GetProcAddress(m_hXMLParserModule, "FreeChildList");
	if (!LPFNFreeChildList) {
		OutputDebugString(_T("Load FreeChildList Failed.\n"));
		return FALSE;
	}

	LPFNGetListItem = (BOOL (WINAPI *)(IXMLDOMNodeList*, LONG, IXMLDOMNode**))GetProcAddress(m_hXMLParserModule, "GetListItem");
	if (!LPFNGetListItem) {
		OutputDebugString(_T("Load GetListItem Failed.\n"));
		return FALSE;
	}

	LPFNFreeListItem = (void (WINAPI *)(IXMLDOMNode*))GetProcAddress(m_hXMLParserModule, "FreeListItem");
	if (!LPFNFreeListItem) {
		OutputDebugString(_T("Load FreeListItem Failed.\n"));
		return FALSE;
	}

	LPFNGetAttribute = (BOOL (WINAPI *)(IXMLDOMNode*, PCTSTR, PTSTR, SIZE_T))GetProcAddress(m_hXMLParserModule, "GetAttribute");
	if (!LPFNGetAttribute) {
		OutputDebugString(_T("Load GetAttribute Failed.\n"));
		return FALSE;
	}

	LPFNGetMapItem = (PCTSTR (WINAPI *)(map<PCTSTR, PCTSTR>*, PCTSTR))GetProcAddress(m_hXMLParserModule, "GetMapItem");
	if (!LPFNGetMapItem) {
		OutputDebugString(_T("Load GetMapItem Failed.\n"));
		return FALSE;
	}

	return TRUE;
}

void I4C3DLoadLibrary::FreeMiscFunctions(void)
{
	::FreeLibrary(m_hMiscModule);
	m_hMiscModule = NULL;
}

void I4C3DLoadLibrary::FreeXMLParserFunctions(void)
{
	::FreeLibrary(m_hXMLParserModule);
	m_hXMLParserModule = NULL;
}
