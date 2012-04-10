#pragma once
#include <map>
class I4C3DLoadLibrary;

class AnalyzeXML
{
public:
	AnalyzeXML(I4C3DLoadLibrary* pLoadLibrary);
	~AnalyzeXML(void);

	// �������R���X�g���N�^���Ă΂Ȃ��ꍇ�́A�ʓr�Ăяo�����Ƃ��ł��܂��B
	BOOL LoadXML(PCTSTR szXMLUri, BOOL* bFileExist);

	PCTSTR GetGlobalValue(PCTSTR szKey);
	PCTSTR GetSoftValue(PCTSTR szSoftName, PCTSTR szKey);

private:
	BOOL ReadGlobalTag(void);
	BOOL ReadSoftsTag(void);
};

