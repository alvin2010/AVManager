#include "stdafx.h"
#include "VideoPlay.h"
#include "assert.h"

////////////////////////////////////////////////////////////////////////////////
CVideoPlay::CVideoPlay()
{
	m_pGraph        = NULL;
	m_pMediaControl = NULL;
	m_pMediaEvent   = NULL;
	m_pBasicVideo   = NULL;
	m_pBasicAudio   = NULL;
	m_pVideoWindow  = NULL;
	m_pMediaSeeking = NULL;

	mObjectTableEntry = 0;
}

CVideoPlay::~CVideoPlay()
{

}

bool CVideoPlay::Init(HWND hWnd)
{
	m_hParent = hWnd;

	if (!m_pGraph)
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IFilterGraph2, (void **)&m_pGraph)))
		{
			return QueryInterfaces();
		}
		m_pGraph = 0;
	}
	return false;
}

bool CVideoPlay::QueryInterfaces(void)
{
	if (m_pGraph)
	{
		HRESULT hr = NOERROR;
		hr |= m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
		hr |= m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pMediaEvent);
		hr |= m_pGraph->QueryInterface(IID_IBasicVideo, (void **)&m_pBasicVideo);
		hr |= m_pGraph->QueryInterface(IID_IBasicAudio, (void **)&m_pBasicAudio);
		hr |= m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&m_pVideoWindow);
		hr |= m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&m_pMediaSeeking);
		if (m_pMediaSeeking)
		{
			m_pMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
		}
		return SUCCEEDED(hr);
	}
	return false;
}

void CVideoPlay::Release(void)
{
	if (m_pMediaSeeking)
	{
		m_pMediaSeeking->Release();
		m_pMediaSeeking = NULL;
	}
	if (m_pMediaControl)
	{
		m_pMediaControl->Release();
		m_pMediaControl = NULL;
	}
	if (m_pMediaEvent)
	{
		m_pMediaEvent->Release();
		m_pMediaEvent = NULL;
	}
	if (m_pBasicVideo)
	{
		m_pBasicVideo->Release();
		m_pBasicVideo = NULL;
	}
	if (m_pBasicAudio)
	{
		m_pBasicAudio->Release();
		m_pBasicAudio = NULL;
	}
	if (m_pVideoWindow)
	{
		m_pVideoWindow->put_Visible(OAFALSE);
		m_pVideoWindow->put_MessageDrain((OAHWND)NULL);
		m_pVideoWindow->put_Owner(OAHWND(0));
		m_pVideoWindow->Release();
		m_pVideoWindow = NULL;
	}
	if (m_pGraph) 
	{
		m_pGraph->Release(); 
		m_pGraph = NULL;
	}
}

IMediaEventEx * CVideoPlay::GetEventHandle(void)
{
	return m_pMediaEvent;
}

// Connect filter from the upstream output pin to the downstream input pin
bool CVideoPlay::ConnectFilters(IPin * inOutputPin, IPin * inInputPin, 
	const AM_MEDIA_TYPE * inMediaType)
{
	if (m_pGraph && inOutputPin && inInputPin)
	{
		HRESULT hr = m_pGraph->ConnectDirect(inOutputPin, inInputPin, inMediaType);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

void CVideoPlay::DisconnectFilters(IPin * inOutputPin)
{
	if (m_pGraph && inOutputPin)
	{
		HRESULT hr = m_pGraph->Disconnect(inOutputPin);
	}
}

//������ʾ���ڵľ����inWindow
bool CVideoPlay::SetDisplayWindow(HWND inWindow)
{	
	if (m_pVideoWindow)
	{
		// ����������Ƶ����
		m_pVideoWindow->put_Visible(OAFALSE);
		m_pVideoWindow->put_Owner((OAHWND)inWindow);

		//��ȡ���봰�ڵ���ʾ����
		RECT windowRect;
		::GetClientRect(inWindow, &windowRect);
		m_pVideoWindow->put_Left(windowRect.left);
		m_pVideoWindow->put_Top(windowRect.top);
		m_pVideoWindow->put_Width(windowRect.right - windowRect.left);
		m_pVideoWindow->put_Height(windowRect.bottom - windowRect.top);
		m_pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		m_pVideoWindow->put_MessageDrain((OAHWND) inWindow);
		// �ظ���Ƶ����
		if (inWindow != NULL)
		{
			m_pVideoWindow->put_Visible(OATRUE);
		}
		else
		{
			m_pVideoWindow->put_Visible(OAFALSE);
		}
		return true;
	}
	return false;
}

bool CVideoPlay::SetNotifyWindow(HWND inWindow)
{
	if (m_pMediaEvent)
	{
		m_pMediaEvent->SetNotifyWindow((OAHWND)inWindow, WM_GRAPHNOTIFY, 0);
		return true;
	}
	return false;
}

void CVideoPlay::HandleEvent(WPARAM inWParam, LPARAM inLParam)
{
	if (m_pMediaEvent)
	{
		LONG eventCode = 0, eventParam1 = 0, eventParam2 = 0;
		while (SUCCEEDED(m_pMediaEvent->GetEvent(&eventCode, &eventParam1, &eventParam2, 0)))
		{
			m_pMediaEvent->FreeEventParams(eventCode, eventParam1, eventParam2);
			switch (eventCode)
			{
			case EC_COMPLETE:
				break;

			case EC_USERABORT:
			case EC_ERRORABORT:
				break;

			default:
				break;
			}
		}
	}
}

bool CVideoPlay::VideoPlayStart(LPCTSTR lpFile)
{
	
	if (m_pGraph && m_pMediaControl)
	{

		if (FAILED(m_pGraph->RenderFile(lpFile, NULL)))
		{
			return false;
		}

		if (!IsRunning())
		{
			if (SUCCEEDED(m_pMediaControl->Run()))
			{
				SetDisplayWindow(m_hParent);
				SetNotifyWindow(m_hParent);
				return true;
			}
		}
		else
		{
			return true;
		}
		
	}
	return false;
}

bool CVideoPlay::VideoPlayStop(void)
{
	if (m_pGraph && m_pMediaControl)
	{
		if (!IsStopped())
		{	
			if (SUCCEEDED(m_pMediaControl->Stop()))
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CVideoPlay::VideoPlayPause(void)
{
	if (m_pGraph && m_pMediaControl)
	{
		if (!IsPaused())
		{	
			if (SUCCEEDED(m_pMediaControl->Pause()))
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CVideoPlay::IsRunning(void)
{
	if (m_pGraph && m_pMediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(m_pMediaControl->GetState(10, &state)))
		{
			return state == State_Running;
		}
	}
	return false;
}

bool CVideoPlay::IsStopped(void)
{
	if (m_pGraph && m_pMediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(m_pMediaControl->GetState(10, &state)))
		{
			return state == State_Stopped;
		}
	}
	return false;
}

bool CVideoPlay::IsPaused(void)
{
	if (m_pGraph && m_pMediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(m_pMediaControl->GetState(10, &state)))
		{
			return state == State_Paused;
		}
	}
	return false;
}

bool CVideoPlay::SetFullScreen(BOOL inEnabled)
{
	if (m_pVideoWindow)
	{
		HRESULT hr = m_pVideoWindow->put_FullScreenMode(inEnabled ? OATRUE : OAFALSE);
		return SUCCEEDED(hr);
	}
	return false;
}

bool CVideoPlay::GetFullScreen(void)
{
	if (m_pVideoWindow)
	{
		long  fullScreenMode = OAFALSE;
		m_pVideoWindow->get_FullScreenMode(&fullScreenMode);
		return (fullScreenMode == OATRUE);
	}
	return false;
}

// IMediaSeeking features
bool CVideoPlay::GetCurrentPosition(double * outPosition)
{
	if (m_pMediaSeeking)
	{
		__int64 position = 0;
		if (SUCCEEDED(m_pMediaSeeking->GetCurrentPosition(&position)))
		{
			*outPosition = ((double)position) / 10000000.;
			return true;
		}
	}
	return false;
}

bool CVideoPlay::GetStopPosition(double * outPosition)
{
	if (m_pMediaSeeking)
	{
		__int64 position = 0;
		if (SUCCEEDED(m_pMediaSeeking->GetStopPosition(&position)))
		{
			*outPosition = ((double)position) / 10000000.;
			return true;
		}
	}
	return false;
}

bool CVideoPlay::SetCurrentPosition(double inPosition)
{
	if (m_pMediaSeeking)
	{
		__int64 one = 10000000;
		__int64 position = (__int64)(one * inPosition);
		HRESULT hr = m_pMediaSeeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
			0, AM_SEEKING_NoPositioning);
		return SUCCEEDED(hr);
	}
	return false;
}

bool CVideoPlay::SetStartStopPosition(double inStart, double inStop)
{
	if (m_pMediaSeeking)
	{
		__int64 one = 10000000;
		__int64 startPos = (__int64)(one * inStart);
		__int64 stopPos  = (__int64)(one * inStop);
		HRESULT hr = m_pMediaSeeking->SetPositions(&startPos, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
			&stopPos, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame);
		return SUCCEEDED(hr);
	}
	return false;
}

bool CVideoPlay::GetDuration(double * outDuration)
{
	if (m_pMediaSeeking)
	{
		__int64 length = 0;
		if (SUCCEEDED(m_pMediaSeeking->GetDuration(&length)))
		{
			*outDuration = ((double)length) / 10000000.;
			return true;
		}
	}
	return false;
}

bool CVideoPlay::SetPlaybackRate(double inRate)
{
	if (m_pMediaSeeking)
	{
		if (SUCCEEDED(m_pMediaSeeking->SetRate(inRate)))
		{
			return true;
		}
	}
	return false;
}

// Attention: range from -10000 to 0, and 0 is FULL_VOLUME.
bool CVideoPlay::SetAudioVolume(long inVolume)
{
	if (m_pBasicAudio)
	{
		HRESULT hr = m_pBasicAudio->put_Volume(inVolume);
		return SUCCEEDED(hr);
	}
	return false;
}

long CVideoPlay::GetAudioVolume(void)
{
	long volume = 0;
	if (m_pBasicAudio)
	{
		m_pBasicAudio->get_Volume(&volume);
	}
	return volume;
}

void CVideoPlay::ChangeAudioVolume(int nVolume)
{
	assert(nVolume >= 0 && nVolume <= 100);

	m_nVolume = nVolume;

	if (!m_pBasicAudio)
		return;

	long lVolume = (nVolume - 100) * 100;
	m_pBasicAudio->put_Volume(lVolume);
}

void CVideoPlay::Mute()
{
	if (!m_pBasicAudio)
		return;

	m_pBasicAudio->put_Volume(-10000);
}

void CVideoPlay::UnMute()
{
	if (!m_pBasicAudio)
		return;

	long lVolume = (m_nVolume - 100) * 100;
	m_pBasicAudio->put_Volume(lVolume);
}

// Attention: range from -10000(left) to 10000(right), and 0 is both.
bool CVideoPlay::SetAudioBalance(long inBalance)
{
	if (m_pBasicAudio)
	{
		HRESULT hr = m_pBasicAudio->put_Balance(inBalance);
		return SUCCEEDED(hr);
	}
	return false;
}

long CVideoPlay::GetAudioBalance(void)
{
	long balance = 0;
	if (m_pBasicAudio)
	{
		m_pBasicAudio->get_Balance(&balance);
	}
	return balance;
}

bool CVideoPlay::SnapshotBitmap(const TCHAR *outFile)//const char * outFile)
{
	if (m_pBasicVideo)
	{
		long bitmapSize = 0;
		if (SUCCEEDED(m_pBasicVideo->GetCurrentImage(&bitmapSize, 0)))
		{
			bool pass = false;
			unsigned char * buffer = new unsigned char[bitmapSize];
			if (SUCCEEDED(m_pBasicVideo->GetCurrentImage(&bitmapSize, (long *)buffer)))
			{
				BITMAPFILEHEADER	hdr;
				LPBITMAPINFOHEADER	lpbi;

				lpbi = (LPBITMAPINFOHEADER)buffer;
				int nColors = 0;
				if (lpbi->biBitCount < 8)
				{
					nColors = 1 << lpbi->biBitCount;
				}


				hdr.bfType		= ((WORD) ('M' << 8) | 'B');	//always is "BM"
				hdr.bfSize		= bitmapSize + sizeof( hdr );
				hdr.bfReserved1 	= 0;
				hdr.bfReserved2 	= 0;
				hdr.bfOffBits		= (DWORD) (sizeof(BITMAPFILEHEADER) + lpbi->biSize +
					nColors * sizeof(RGBQUAD));

				DWORD  dwWrite; 
				HANDLE hFile = CreateFile(outFile,GENERIC_READ | GENERIC_WRITE ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,NULL,NULL);
				WriteFile(hFile,&hdr,sizeof(BITMAPFILEHEADER),&dwWrite,NULL);
				WriteFile(hFile,buffer,bitmapSize,&dwWrite,NULL);
				CloseHandle(hFile);
				pass = true;
			}
			delete [] buffer;
			return pass;
		}
	}
	return false;
}