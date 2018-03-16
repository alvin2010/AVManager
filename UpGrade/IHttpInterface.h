/*****************************************************************
*HTTP�����࣬��Ҫ����HTTP GET/POST�����أ�֧���ض��򣩹���
*Author��	
*Date��		
*Email��	
*/
/*****************************************************************
*/
#pragma once
#include <stdio.h>
#include <tchar.h>
#include <string>
using std::string;
using std::wstring;


enum HttpRequest
{
	Hr_Post,
	Hr_Get,
};
//ö������״̬
enum DownloadState
{
	DS_Loading = 0,
	DS_Fialed,
	DS_Finished,
};

/******************************************************
*���������Ϣ
*
******************************************************/
enum HttpInterfaceError
{
	Hir_Success = 0,		//�ɹ�
	Hir_InitErr,			//��ʼ��ʧ��
	Hir_ConnectErr,			//����HTTP������ʧ��
	Hir_SendErr,			//��������ʧ��
	Hir_QueryErr,			//��ѯHTTP����ͷʧ��
	Hir_404,				//ҳ�治����
	Hir_IllegalUrl,			//��Ч��URL
	Hir_CreateFileErr,		//�����ļ�ʧ��
	Hir_DownloadErr,		//����ʧ��
	Hir_QueryIPErr,			//��ȡ������Ӧ�ĵ�ַʧ��
	Hir_SocketErr,			//�׽��ִ���
	Hir_UserCancel,			//�û�ȡ������
	Hir_BufferErr,			//�ļ�̫�󣬻���������
	Hir_HeaderErr,			//HTTP����ͷ����
	Hir_ParamErr,			//�������󣬿�ָ�룬���ַ�����
	Hir_UnknowErr,

};




//���صĻص�
class IHttpCallback
{
public:
	virtual void	OnDownloadCallback(void* pParam, DownloadState state, double nTotalSize, double nLoadSize)		= 0;
	virtual bool	IsNeedStop()																					= 0;
};

class IHttpBase
{
public:
	virtual void	SetDownloadCallback(IHttpCallback* pCallback, void* pParam)				= 0;
	virtual bool	DownloadFile(LPCWSTR lpUrl, LPCWSTR lpFilePath)							= 0;
	virtual bool	DownloadToMem(LPCWSTR lpUrl, OUT void** ppBuffer, OUT int* nSize)		= 0;
	virtual HttpInterfaceError GetErrorCode()												= 0;
};

////////////////////////////////////////////////////////////////////////////////////
//HTTP����ӿ���
class IWininetHttp
	:public IHttpBase
{
public:
	//HTTP������
	virtual string	Request(LPCSTR lpUrl, HttpRequest type, LPCSTR lpPostData = NULL, LPCSTR lpHeader = NULL)			= 0;
	virtual string	Request(LPCWSTR lpUrl, HttpRequest type, LPCWSTR lpPostData = NULL, LPCWSTR lpHeader = NULL) = 0;
};

