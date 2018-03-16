#ifndef UTILS_H_
#define UTILS_H_


class CUtils
{
public:
	//	��ȡ��ǰĿ¼
	tstring GetCurDirectory();
	tstring GetModuleName();
	tstring GetFullModuleName();
	tstring GetAppVersionInfo(LPCTSTR lpFileName);

	BOOL	FloderIsExist(LPCTSTR lpPath);
	BOOL	FileIsExist(LPCTSTR lpPath);
	tstring GetFileName(LPCTSTR lpPath);
	tstring GetExtName(LPCTSTR lpPath);
	int64_t	GetFileSize(LPCTSTR lpFileName);

	// �ַ���ת��
	tstring AnsiStringToUnicodeString(const char* pIn);
	string	UnicodeStringToAnsiString(const wchar_t* lpin);
	tstring Utf8StringToUnicodeString(const char* strUtf8);
	string	UnicodeStringToUtf8String(const wchar_t* lpin);
	string	UTF8ToGBK(const string& strUTF8);

	//	���̹���
	DWORD	GetProcessidFromName(LPCTSTR name);
	int		KillProcess(DWORD processID);
	BOOL	StartProcess(tstring strFilePath);
private:
	tstring m_strCurrentDir;
	tstring m_strModuleName;
	tstring m_strFullFileName;
	tstring m_strAppVersion;
};


#endif