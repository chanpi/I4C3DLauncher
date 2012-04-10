#include "StdAfx.h"
#include "I4C3DLauncher.h"
#include "AnalyzeXML.h"
#include <vector>

#if UNICODE || _UNICODE
static LPCTSTR g_FILE = __FILEW__;
#else
static LPCTSTR g_FILE = __FILE__;
#endif

extern TCHAR szTitle[];
static I4C3DLoadLibrary* g_pLoadLibrary				= NULL;

static IXMLDOMElement* g_pRootElement				= NULL;
static BOOL g_bInitialized							= FALSE;
static map<PCTSTR, PCTSTR>* g_pGlobalConfig			= NULL;
typedef pair<PCTSTR, map<PCTSTR, PCTSTR>*> config_pair;
static vector<config_pair>* g_pConfigPairContainer	= NULL;

static void CleanupRootElement(void);

static const PCTSTR TAG_GLOBAL		= _T("global");
static const PCTSTR TAG_SOFTS		= _T("softs");
static const PCTSTR TAG_NAME		= _T("name");

inline void SafeReleaseMap(map<PCTSTR, PCTSTR>* pMap)
{
	if (pMap != NULL) {
		g_pLoadLibrary->LPFNCleanupStoredValues(pMap);
		pMap = NULL;
	}
}

AnalyzeXML::AnalyzeXML(I4C3DLoadLibrary* pLoadLibrary)
{
	g_pLoadLibrary = pLoadLibrary;
	g_pConfigPairContainer = new vector<config_pair>;
}

AnalyzeXML::~AnalyzeXML(void)
{
	CleanupRootElement();
	if (g_pConfigPairContainer) {
		g_pConfigPairContainer->clear();
		delete g_pConfigPairContainer;
		g_pConfigPairContainer = NULL;
	}
}

/**
 * @brief
 * XMLParser.dllで取得したXMLのマップオブジェクトを解放します。
 * 
 * XMLParser.dllで取得したXMLのマップオブジェクトを解放します。
 * 
 * @remarks
 * マップの取得に成功した場合には必ず本関数で解放してください。
 * 
 * @see
 * ReadGlobalTag() | ReadSoftsTag()
 */
void CleanupRootElement(void)
{
	if (g_bInitialized) {
		if (g_pConfigPairContainer) {
			int size = g_pConfigPairContainer->size();
			for (int i = 0; i < size; ++i) {
				delete (*g_pConfigPairContainer)[i].first;
				map<PCTSTR, PCTSTR>* pMap = (*g_pConfigPairContainer)[i].second;
				SafeReleaseMap(pMap);
				pMap = NULL;
			}
		}
		SafeReleaseMap(g_pGlobalConfig);
		g_pGlobalConfig		= NULL;

		g_pLoadLibrary->LPFNUnInitialize(g_pRootElement);
		g_pRootElement = NULL;
		g_bInitialized = FALSE;
	}
}

/**
 * @brief
 * XMLをロードしオブジェクトにします。
 * 
 * @param szXMLUri
 * ロードするXMLファイル。
 * 
 * @returns
 * XMLファイルのロードに成功した場合にはTRUE、失敗した場合にはFALSEが返ります。
 * 
 * XMLParser.dllを利用し、XMLをロードしオブジェクトにします。
 */
BOOL AnalyzeXML::LoadXML(PCTSTR szXMLUri, BOOL* bFileExist)
{
	CleanupRootElement();
	if (!PathFileExists(szXMLUri)) {
		g_pLoadLibrary->LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_LOAD), GetLastError(), g_FILE, __LINE__);
		*bFileExist = FALSE;
		return FALSE;
	}
	*bFileExist = TRUE;
	g_bInitialized = g_pLoadLibrary->LPFNInitialize(&g_pRootElement, szXMLUri);
	return g_bInitialized;
}

/**
 * @brief
 * globalタグの内容を問い合わせます。
 * 
 * @param szKey
 * global/keys/keyタグのキー値。
 * 
 * @returns
 * マップ中にキーで指定した値があれば値を、なければNULLを返します。
 * 
 * globalタグの内容を格納したマップにキーを問い合わせ、値を取得します。
 * 
 * @see
 * ReadGlobalTag()
 */
PCTSTR AnalyzeXML::GetGlobalValue(PCTSTR szKey)
{
	if (!ReadGlobalTag()) {
		g_pLoadLibrary->LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_TAG_GLOBAL), GetLastError(), g_FILE, __LINE__);
		return NULL;
	}

	return g_pLoadLibrary->LPFNGetMapItem(g_pGlobalConfig, szKey);
}

/**
 * @brief
 * softsタグの内容を問い合わせます。
 * 
 * @param szKey
 * softs/soft/keys/keyタグのキー値。
 * 
 * @returns
 * マップ中にキーで指定した値があれば値を、なければNULLを返します。
 * 
 * softsタグの内容を格納したマップにキーを問い合わせ、値を取得します。
 * 
 * @see
 * ReadGlobalTag() | ReadSoftsTag()
 */
PCTSTR AnalyzeXML::GetSoftValue(PCTSTR szSoftName, PCTSTR szKey)
{
	//if (!this->ReadGlobalTag()) {
	//	ReportError(_T("[ERROR] globalタグの読み込みに失敗しています。"));
	//	return NULL;
	//}
	if (!this->ReadSoftsTag()) {
		g_pLoadLibrary->LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_TAG_SOFTS), GetLastError(), g_FILE, __LINE__);
		return NULL;
	}

	if (g_pConfigPairContainer) {
		int size = g_pConfigPairContainer->size();
		for (int i = 0; i < size; ++i) {
			if (_tcsicmp((*g_pConfigPairContainer)[i].first, szSoftName) == 0) {
				return g_pLoadLibrary->LPFNGetMapItem((*g_pConfigPairContainer)[i].second, szKey);
			}
		}
	}
	return NULL;
}

/**
 * @brief
 * I4C3D.xmlのglobalタグの解析を行います。
 * 
 * @returns
 * I4C3D.xmlのglobalタグの解析に成功した場合にはTRUE、失敗した場合にはFALSEを返します。
 * 
 * XMLParser.dllを使用してI4C3D.xmlのglobalタグの解析を行い、マップに格納します。
 * 例: <key name="log">info</key>
 * keyタグnameアトリビュートの値をマップのキーに、バリューをマップのバリューに格納します。
 * 
 * @remarks
 * マップの内容を参照するにはGetGlobalValue()を使用します。
 * 
 * @see
 * GetGlobalValue()
 */
BOOL AnalyzeXML::ReadGlobalTag(void)
{
	if (g_pGlobalConfig == NULL) {
		IXMLDOMNode* pGlobal = NULL;
		if (g_pLoadLibrary->LPFNGetDOMTree(g_pRootElement, TAG_GLOBAL, &pGlobal)) {
			g_pGlobalConfig = g_pLoadLibrary->LPFNStoreValues(pGlobal, TAG_NAME);
			g_pLoadLibrary->LPFNFreeDOMTree(pGlobal);

		} else {
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * @brief
 * I4C3D.xmlのsoftsタグの解析を行います。
 * 
 * @returns
 * I4C3D.xmlのsoftsタグの解析に成功した場合にはTRUE、失敗した場合にはFALSEを返します。
 * 
 * XMLParser.dllを使用してI4C3D.xmlのsoftsタグの解析を行い、マップに格納します。
 * 
 * @remarks
 * マップの内容を参照するにはGetSoftValue()を使用します。
 * 
 * @see
 * GetSoftValue()
 */
BOOL AnalyzeXML::ReadSoftsTag(void)
{
	if (g_pConfigPairContainer->empty()) {

		IXMLDOMNode* pSofts = NULL;
		IXMLDOMNodeList* pSoftsList = NULL;
		if (!g_pLoadLibrary->LPFNGetDOMTree(g_pRootElement, TAG_SOFTS, &pSofts)) {
			g_pLoadLibrary->LPFNLoggingMessage(Log_Error, _T(MESSAGE_ERROR_XML_TAG_SOFTS_DOM), GetLastError(), g_FILE, __LINE__);
			return FALSE;
		}

		LONG childCount = g_pLoadLibrary->LPFNGetChildList(pSofts, &pSoftsList);
		for (int i = 0; i < childCount; i++) {
			IXMLDOMNode* pTempNode = NULL;
			if (g_pLoadLibrary->LPFNGetListItem(pSoftsList, i, &pTempNode)) {
				TCHAR szSoftwareName[BUFFER_SIZE] = {0};
				if (g_pLoadLibrary->LPFNGetAttribute(pTempNode, TAG_NAME, szSoftwareName, _countof(szSoftwareName))) {

					// globalタグのターゲット名と比較
					TCHAR* pSoftwareName = new TCHAR[_tcslen(szSoftwareName)+1];	// CleanupRootElement()でdelete
					ZeroMemory(pSoftwareName, sizeof(pSoftwareName));
					_tcscpy_s(pSoftwareName, _tcslen(szSoftwareName)+1, szSoftwareName);
					g_pConfigPairContainer->push_back(config_pair(pSoftwareName, g_pLoadLibrary->LPFNStoreValues(pTempNode, TAG_NAME)));
					OutputDebugString(szSoftwareName);
					OutputDebugString(_T(" push_back\n"));
				}
				g_pLoadLibrary->LPFNFreeListItem(pTempNode);
			}
		}

		g_pLoadLibrary->LPFNFreeChildList(pSoftsList);
		g_pLoadLibrary->LPFNFreeDOMTree(pSofts);
	}
	return TRUE;
}