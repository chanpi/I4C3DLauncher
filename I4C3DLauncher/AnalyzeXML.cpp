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
 * XMLParser.dll�Ŏ擾����XML�̃}�b�v�I�u�W�F�N�g��������܂��B
 * 
 * XMLParser.dll�Ŏ擾����XML�̃}�b�v�I�u�W�F�N�g��������܂��B
 * 
 * @remarks
 * �}�b�v�̎擾�ɐ��������ꍇ�ɂ͕K���{�֐��ŉ�����Ă��������B
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
 * XML�����[�h���I�u�W�F�N�g�ɂ��܂��B
 * 
 * @param szXMLUri
 * ���[�h����XML�t�@�C���B
 * 
 * @returns
 * XML�t�@�C���̃��[�h�ɐ��������ꍇ�ɂ�TRUE�A���s�����ꍇ�ɂ�FALSE���Ԃ�܂��B
 * 
 * XMLParser.dll�𗘗p���AXML�����[�h���I�u�W�F�N�g�ɂ��܂��B
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
 * global�^�O�̓��e��₢���킹�܂��B
 * 
 * @param szKey
 * global/keys/key�^�O�̃L�[�l�B
 * 
 * @returns
 * �}�b�v���ɃL�[�Ŏw�肵���l������Βl���A�Ȃ����NULL��Ԃ��܂��B
 * 
 * global�^�O�̓��e���i�[�����}�b�v�ɃL�[��₢���킹�A�l���擾���܂��B
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
 * softs�^�O�̓��e��₢���킹�܂��B
 * 
 * @param szKey
 * softs/soft/keys/key�^�O�̃L�[�l�B
 * 
 * @returns
 * �}�b�v���ɃL�[�Ŏw�肵���l������Βl���A�Ȃ����NULL��Ԃ��܂��B
 * 
 * softs�^�O�̓��e���i�[�����}�b�v�ɃL�[��₢���킹�A�l���擾���܂��B
 * 
 * @see
 * ReadGlobalTag() | ReadSoftsTag()
 */
PCTSTR AnalyzeXML::GetSoftValue(PCTSTR szSoftName, PCTSTR szKey)
{
	//if (!this->ReadGlobalTag()) {
	//	ReportError(_T("[ERROR] global�^�O�̓ǂݍ��݂Ɏ��s���Ă��܂��B"));
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
 * I4C3D.xml��global�^�O�̉�͂��s���܂��B
 * 
 * @returns
 * I4C3D.xml��global�^�O�̉�͂ɐ��������ꍇ�ɂ�TRUE�A���s�����ꍇ�ɂ�FALSE��Ԃ��܂��B
 * 
 * XMLParser.dll���g�p����I4C3D.xml��global�^�O�̉�͂��s���A�}�b�v�Ɋi�[���܂��B
 * ��: <key name="log">info</key>
 * key�^�Oname�A�g���r���[�g�̒l���}�b�v�̃L�[�ɁA�o�����[���}�b�v�̃o�����[�Ɋi�[���܂��B
 * 
 * @remarks
 * �}�b�v�̓��e���Q�Ƃ���ɂ�GetGlobalValue()���g�p���܂��B
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
 * I4C3D.xml��softs�^�O�̉�͂��s���܂��B
 * 
 * @returns
 * I4C3D.xml��softs�^�O�̉�͂ɐ��������ꍇ�ɂ�TRUE�A���s�����ꍇ�ɂ�FALSE��Ԃ��܂��B
 * 
 * XMLParser.dll���g�p����I4C3D.xml��softs�^�O�̉�͂��s���A�}�b�v�Ɋi�[���܂��B
 * 
 * @remarks
 * �}�b�v�̓��e���Q�Ƃ���ɂ�GetSoftValue()���g�p���܂��B
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

					// global�^�O�̃^�[�Q�b�g���Ɣ�r
					TCHAR* pSoftwareName = new TCHAR[_tcslen(szSoftwareName)+1];	// CleanupRootElement()��delete
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