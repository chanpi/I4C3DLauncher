#pragma once
#include <map>
class I4C3DLoadLibrary;

class AnalyzeXML
{
public:
	AnalyzeXML(I4C3DLoadLibrary* pLoadLibrary);
	~AnalyzeXML(void);

	// 引数つきコンストラクタを呼ばない場合は、別途呼び出すことができます。
	BOOL LoadXML(PCTSTR szXMLUri, BOOL* bFileExist);

	PCTSTR GetGlobalValue(PCTSTR szKey);
	PCTSTR GetSoftValue(PCTSTR szSoftName, PCTSTR szKey);

private:
	BOOL ReadGlobalTag(void);
	BOOL ReadSoftsTag(void);
};

