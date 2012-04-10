#pragma once

#include "Misc.h"		// enum Log_Error éQè∆ÇÃÇΩÇﬂ
#include <map>
using namespace std;

class I4C3DLoadLibrary
{
public:
	I4C3DLoadLibrary(void);
	~I4C3DLoadLibrary(void);

	BOOL LoadLibraries(void);
	void FreeLibraries(void);

	// Miscån
	BOOL (WINAPI *LPFNExecuteOnce)(LPCTSTR szAppName);
	void (WINAPI *LPFNCleanupMutex)(void);
	BOOL (WINAPI *LPFNLogFileOpenW)(PCSTR szDirectory, PCSTR szTitle, LOG_LEVEL logLevel);
	BOOL (WINAPI *LPFNLogFileCloseW)(void);
	void (WINAPI *LPFNLoggingMessageW)(LOG_LEVEL logLevel, LPCWSTR srcMessage, DWORD dwErrorCode, LPCWSTR szFILE, int line);
	BOOL (WINAPI *LPFNLogFileOpenA)(PCSTR szDirectory, PCSTR szTitle, LOG_LEVEL logLevel);
	BOOL (WINAPI *LPFNLogFileCloseA)(void);
	void (WINAPI *LPFNLoggingMessageA)(LOG_LEVEL logLevel, LPCSTR srcMessage, DWORD dwErrorCode, LPCSTR szFILE, int line);

	// XMLParserån
	BOOL (WINAPI *LPFNInitialize)(IXMLDOMElement** pRootElement, PCTSTR szXMLuri);
	void (WINAPI *LPFNUnInitialize)(IXMLDOMElement* pRootElement);

	map<PCTSTR, PCTSTR>* (WINAPI *LPFNStoreValues)(IXMLDOMNode* pNode, PCTSTR attrName);
	void (WINAPI *LPFNCleanupStoredValues)(map<PCTSTR, PCTSTR>* keysMap);

	BOOL (WINAPI *LPFNGetDOMTree)(IXMLDOMElement* pRootElement, PCTSTR szNodeName, IXMLDOMNode** pNode);
	void (WINAPI *LPFNFreeDOMTree)(IXMLDOMNode* pNode);

	LONG (WINAPI *LPFNGetChildList)(IXMLDOMNode* pNode, IXMLDOMNodeList** pNodeList);
	void (WINAPI *LPFNFreeChildList)(IXMLDOMNodeList* pNodeList);

	BOOL (WINAPI *LPFNGetListItem)(IXMLDOMNodeList* pNodeList, LONG index, IXMLDOMNode** pNode);
	void (WINAPI *LPFNFreeListItem)(IXMLDOMNode* pNode);

	BOOL (WINAPI *LPFNGetAttribute)(IXMLDOMNode* pNode, PCTSTR attrName, PTSTR attrValue, SIZE_T cchLength);

	PCTSTR (WINAPI *LPFNGetMapItem)(map<PCTSTR, PCTSTR>* pMap, PCTSTR szKey);

#ifdef UNICODE
#define LPFNLoggingMessage	LPFNLoggingMessageW
#define LPFNLogFileClose	LPFNLogFileCloseW
#define LPFNLoggingMessage	LPFNLoggingMessageW
#else
#define LPFNLoggingMessage	LPFNLoggingMessageA
#define LPFNLogFileClose	LPFNLogFileCloseA
#define LPFNLoggingMessage	LPFNLoggingMessageA
#endif

private:
	HMODULE m_hMiscModule;
	HMODULE m_hXMLParserModule;
	BOOL LoadMiscFunctions(void);
	BOOL LoadXMLParserFunctions(void);
	void FreeMiscFunctions(void);
	void FreeXMLParserFunctions(void);

	// Miscån
	//BOOL ExecuteOnce(LPCTSTR szAppName);
	//void CleanupMutex();
	//BOOL LogFileOpenW(PCSTR szTitle, LOG_LEVEL logLevel);
	//BOOL LogFileCloseW();
	//void LoggingMessageW(LOG_LEVEL logLevel, LPCWSTR srcMessage, DWORD dwErrorCode, LPCWSTR szFILE, int line);

	// XMLParserån
	//BOOL WINAPI Initialize(IXMLDOMElement** pRootElement, PCTSTR szXMLuri);
	//void WINAPI UnInitialize(IXMLDOMElement* pRootElement);

	//map<PCTSTR, PCTSTR>* WINAPI StoreValues(IXMLDOMNode* pNode, PCTSTR attrName);
	//void WINAPI CleanupStoredValues(map<PCTSTR, PCTSTR>* keysMap);

	//BOOL WINAPI GetDOMTree(IXMLDOMElement* pRootElement, PCTSTR szNodeName, IXMLDOMNode** pNode);
	//void WINAPI FreeDOMTree(IXMLDOMNode* pNode);

	//LONG WINAPI GetChildList(IXMLDOMNode* pNode, IXMLDOMNodeList** pNodeList);
	//void WINAPI FreeChildList(IXMLDOMNodeList* pNodeList);

	//BOOL WINAPI GetListItem(IXMLDOMNodeList* pNodeList, LONG index, IXMLDOMNode** pNode);
	//void WINAPI FreeListItem(IXMLDOMNode* pNode);

	//BOOL WINAPI GetAttribute(IXMLDOMNode* pNode, PCTSTR attrName, PTSTR attrValue, SIZE_T cchLength);

	//PCTSTR WINAPI GetMapItem(map<PCTSTR, PCTSTR>* pMap, PCTSTR szKey);
};

