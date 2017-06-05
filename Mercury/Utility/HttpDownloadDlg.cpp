/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include <afxinet.h>
#include "HttpDownloadDlg.h"
#include "ZqrResourceCompiler.h"

///////////////////////////////// Defines /////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_HTTPDOWNLOAD_THREAD_FINISHED = WM_APP + 1;


#define IDS_HTTPDOWNLOAD_FILESTATUS "%s from %s"
#define IDS_HTTPDOWNLOAD_CONNECTED "Connected to %s"
#define IDS_HTTPDOWNLOAD_RESOLVING_NAME "Resolving name: %s"
#define IDS_HTTPDOWNLOAD_RESOLVED_NAME "Resolved name to %s"
#define IDS_HTTPDOWNLOAD_CONNECTING "Connecting to %s"
#define IDS_HTTPDOWNLOAD_REDIRECTING "Redirecting to %s"
#define IDS_HTTPDOWNLOAD_GETTING_FILE_INFORMATION "Getting file information"
#define IDS_HTTPDOWNLOAD_FAIL_PARSE_ERROR "An error occurred parsing the url: %s"
#define IDS_HTTPDOWNLOAD_GENERIC_ERROR  "An error occurred while attempting to download the file, Error:%s\n"
#define IDS_HTTPDOWNLOAD_FAIL_CONNECT_SERVER "An error occurred connecting to the server, Error:%s"
#define IDS_HTTPDOWNLOAD_BYTESPERSECOND "%s Bytes/Sec"
#define IDS_HTTPDOWNLOAD_KILOBYTESPERSECOND "%s KB/Sec"
#define IDS_HTTPDOWNLOAD_OK_TO_OVERWRITE  "The file '%s' already exists.\nDo you want to replace it?"
#define IDS_HTTPDOWNLOAD_FAIL_FILE_OPEN "An error occured while opening the file to be downloaded, Error:%s"
#define IDS_HTTPDOWNLOAD_ABORTING_TRANSFER "Aborting transfer"
#define IDS_HTTPDOWNLOAD_INVALID_SERVER_RESPONSE "Failed to receive a valid response from the server"
#define IDS_HTTPDOWNLOAD_INVALID_HTTP_RESPONSE "Failed to receive a valid HTTP response from the server"
#define IDS_HTTPDOWNLOAD_ERROR_READFILE "An error occurred while downloading the file, Error:%s"
#define IDS_HTTPDOWNLOAD_PERCENTAGE "%s%% of %s Completed"
#define IDS_HTTPDOWNLOAD_RETREIVEING_FILE "Retrieving the file"
#define IDS_HTTPDOWNLOAD_OF     "%s of %s"
#define IDS_HTTPDOWNLOAD_SECONDS "%s sec"
#define IDS_HTTPDOWNLOAD_MINUTES "%s min"
#define IDS_HTTPDOWNLOAD_MINUTES_AND_SECONDS "%s min %s sec"
#define IDS_HTTPDOWNLOAD_BYTES  "%s Bytes"
#define IDS_HTTPDOWNLOAD_KILOBYTES "%s KB"
#define IDS_HTTPDOWNLOAD_MEGABYTES "%s MB"
#define IDS_HTTPDOWNLOAD_TIMELEFT "%s (%s copied)"
#define IDS_HTTPDOWNLOAD_CANNOT_CONNECT "Program is blocked by firewall or by other program"

/*
#define IDS_HTTPDOWNLOAD_FILESTATUS "%s from %s"
#define IDS_HTTPDOWNLOAD_CONNECTED "Connected to %s"
#define IDS_HTTPDOWNLOAD_RESOLVING_NAME "Resolving name: %s"
#define IDS_HTTPDOWNLOAD_RESOLVED_NAME "Resolved name to %s"
#define IDS_HTTPDOWNLOAD_CONNECTING "Connecting to %s"
#define IDS_HTTPDOWNLOAD_REDIRECTING "Redirecting to %s"
#define IDS_HTTPDOWNLOAD_GETTING_FILE_INFORMATION "Getting file information"
#define IDS_HTTPDOWNLOAD_FAIL_PARSE_ERROR "An error occurred parsing the url: %s"
#define IDS_HTTPDOWNLOAD_GENERIC_ERROR  "An error occurred while attempting to download the file, Error:%s\n"
#define IDS_HTTPDOWNLOAD_FAIL_CONNECT_SERVER "An error occurred connecting to the server, Error:%s"
#define IDS_HTTPDOWNLOAD_BYTESPERSECOND "%s Bytes/öü"
#define IDS_HTTPDOWNLOAD_KILOBYTESPERSECOND "%s KB/öü"
#define IDS_HTTPDOWNLOAD_OK_TO_OVERWRITE  "The file '%s' already exists.\nDo you want to replace it?"
#define IDS_HTTPDOWNLOAD_FAIL_FILE_OPEN "An error occured while opening the file to be downloaded, Error:%s"
#define IDS_HTTPDOWNLOAD_ABORTING_TRANSFER "Aborting transfer"
#define IDS_HTTPDOWNLOAD_INVALID_SERVER_RESPONSE "Failed to receive a valid response from the server"
#define IDS_HTTPDOWNLOAD_INVALID_HTTP_RESPONSE "Failed to receive a valid HTTP response from the server"
#define IDS_HTTPDOWNLOAD_ERROR_READFILE "An error occurred while downloading the file, Error:%s"
#define IDS_HTTPDOWNLOAD_PERCENTAGE "%s%% of %s Completed"
#define IDS_HTTPDOWNLOAD_RETREIVEING_FILE "Retrieving the file"
#define IDS_HTTPDOWNLOAD_OF     "%s of %s"
#define IDS_HTTPDOWNLOAD_SECONDS "%s öü"
#define IDS_HTTPDOWNLOAD_MINUTES "%s öÅ"
#define IDS_HTTPDOWNLOAD_MINUTES_AND_SECONDS "%s öÅ %s öü"
#define IDS_HTTPDOWNLOAD_BYTES  "%s Bytes"
#define IDS_HTTPDOWNLOAD_KILOBYTES "%s KB"
#define IDS_HTTPDOWNLOAD_MEGABYTES "%s MB"
#define IDS_HTTPDOWNLOAD_TIMELEFT "%s (%s ëùçøêóèãäøô)"
#define IDS_HTTPDOWNLOAD_CANNOT_CONNECT "çêùïêôüô øèäùëóèô Firewall_øû üøóê"
*/
#define IDD_HTTPDOWNLOAD	1000
#define IDC_ANIMATE1		1001
#define IDC_STATUS			1002
#define IDC_FILESTATUS		1003
#define IDC_TIMELEFT		1004
#define IDC_TRANSFER_RATE	1005
#define IDC_PROGRESS1		1006
#define IDR_HTTPDOWNLOAD_ANIMATION 170


///////////////////////////////// Implementation //////////////////////////////
IMPLEMENT_DYNAMIC(CHttpDownloadDlg, CDialog);


CHttpDownloadDlg::CHttpDownloadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(1, pParent)
	{
	m_hInternetSession	= NULL;
	m_hHttpConnection	= NULL;
	m_hHttpFile			= NULL;
	m_bAbort			= FALSE;
	m_bSafeToClose		= FALSE;
	m_pThread			= NULL;

	/*
	CString sDlgScript;
	sDlgScript.Format(
	"%d DIALOGEX  0, 0, 228, 143\
	STYLE DS_SETFONT|DS_MODALFRAME|WS_POPUP|WS_CAPTION|WS_SYSMENU\
	CAPTION ~File Download~\
	FONT 9, ~GeoMegasoft~, 400, 0, 0x1\
	BEGIN\
		CONTROL         ~Animate1~,%d,~SysAnimate32~,ACS_TRANSPARENT,7,7,192,32\
		LTEXT           ~~,%d,7,47,214,8\
		LTEXT           ~~,%d,7,60,214,8\
		CONTROL         ~Progress1~,%d,~msctls_progress32~,WS_BORDER|PBS_SMOOTH,7,74,214,10\
		LTEXT           ~Estimated time left:~,%d,7,91,60,8\
		LTEXT           ~~,%d,79,91,142,8\
		LTEXT           ~Transfer rate:~,%d,7,104,43,8\
		LTEXT           ~~,%d,80,104,141,8\
		PUSHBUTTON   ~Cancel~,%d,171,122,50,14\
	END", IDD_HTTPDOWNLOAD, IDC_ANIMATE1, IDC_STATUS, IDC_FILESTATUS,  
	IDC_PROGRESS1, IDC_STATIC, IDC_TIMELEFT, IDC_STATIC, IDC_TRANSFER_RATE, IDCANCEL);
	sDlgScript.Replace('~', '"');
	*/

	CString sDlgScript;
	sDlgScript.Format(
	"%d DIALOGEX  0, 0, 228, 143\
	STYLE DS_SETFONT|DS_MODALFRAME|WS_POPUP|WS_CAPTION|WS_SYSMENU\
	CAPTION ~File Download~\
	FONT 9, ~Arial~, 400, 0, 0x1\
	BEGIN\
		CONTROL         ~Animate1~,%d,~SysAnimate32~,ACS_TRANSPARENT,7,7,192,32\
		LTEXT           ~~,%d,7,47,214,8\
		LTEXT           ~~,%d,7,60,214,8\
		CONTROL         ~Progress1~,%d,~msctls_progress32~,WS_BORDER|PBS_SMOOTH,7,74,214,10\
		LTEXT           ~Estimated time left:~,%d,7,91,60,8\
		LTEXT           ~~,%d,79,91,142,8\
		LTEXT           ~Transfer rate:~,%d,7,104,80,8\
		LTEXT           ~~,%d,80,104,141,8\
		PUSHBUTTON   ~Cancel~,%d,171,122,50,14\
	END", IDD_HTTPDOWNLOAD, IDC_ANIMATE1, IDC_STATUS, IDC_FILESTATUS,  
	IDC_PROGRESS1, IDC_STATIC, IDC_TIMELEFT, IDC_STATIC, IDC_TRANSFER_RATE, IDCANCEL);
	sDlgScript.Replace('~', '"');

	m_lpDialogTemplate	= (LPCDLGTEMPLATE)new BYTE[1024];
	UINT nBuffSize		= 0L;
	BOOL bRet			= ZqrResourceCompiler::GetCompiler()->CompileDialogScript(sDlgScript, (void*&)m_lpDialogTemplate, nBuffSize);
	ASSERT( bRet );
	m_lpszTemplateName	= NULL;
	}

void 
CHttpDownloadDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATUS,		m_ctrlStatus);
	DDX_Control(pDX, IDC_TRANSFER_RATE, m_ctrlTransferRate);
	DDX_Control(pDX, IDC_TIMELEFT,		m_ctrlTimeLeft);
	DDX_Control(pDX, IDC_PROGRESS1,		m_ctrlProgress);
	DDX_Control(pDX, IDC_FILESTATUS,	m_ctrlFileStatus);
	DDX_Control(pDX, IDC_ANIMATE1,		m_ctrlAnimate);
	}

BEGIN_MESSAGE_MAP(CHttpDownloadDlg, CDialog)
	ON_WM_DESTROY	()
	ON_WM_CLOSE		()
	ON_MESSAGE		(WM_HTTPDOWNLOAD_THREAD_FINISHED, OnThreadFinished)
END_MESSAGE_MAP()


LRESULT 
CHttpDownloadDlg::OnThreadFinished(WPARAM wParam, LPARAM /*lParam*/)
	{
	//It's now safe to close since the thread has signaled us
	m_bSafeToClose = TRUE;
	//Stop the animation
	m_ctrlAnimate.Stop();

	//If an error occured display the message box
	if( m_bAbort )
		EndDialog(IDCANCEL);
	else 
	if (wParam)
		{
		AfxMessageBox(m_sError);
		EndDialog(IDCANCEL);
		}
	else
		EndDialog(IDOK);
	return 0L;
	}

BOOL 
CHttpDownloadDlg::OnInitDialog() 
	{
	//Let the parent class do its thing
	CDialog::OnInitDialog();

	//Setup the animation control

	HMODULE hRes = AfxGetResourceHandle();
	AfxSetResourceHandle(LoadLibrary("shell32.dll"));
	m_ctrlAnimate.Open(IDR_HTTPDOWNLOAD_ANIMATION);
	AfxSetResourceHandle(hRes);

	//Validate the URL
	ASSERT(m_sURLToDownload.GetLength()); //Did you forget to specify the file to download
	if(!AfxParseURL(m_sURLToDownload, m_dwServiceType, m_sServer, m_sObject, m_nPort))
		{
		//Try sticking "http://" before it
		m_sURLToDownload = _T("http://") + m_sURLToDownload;
		if (!AfxParseURL(m_sURLToDownload, m_dwServiceType, m_sServer, m_sObject, m_nPort))
			{
			TRACE(_T("Failed to parse the URL: %s\n"), m_sURLToDownload);
			EndDialog(IDCANCEL);
			return TRUE;
			}
		}

	//Check to see if the file we are downloading to exists and if
	//it does, then ask the user if they were it overwritten
	CFileStatus fs;
	ASSERT(m_sFileToDownloadInto.GetLength());
	if (CFile::GetStatus(m_sFileToDownloadInto, fs))
		{
		CString sMsg;
		sMsg.Format(IDS_HTTPDOWNLOAD_OK_TO_OVERWRITE, m_sFileToDownloadInto);
		if (AfxMessageBox(sMsg, MB_YESNO) != IDYES)
			{
			TRACE(_T("Failed to confirm file overwrite, download aborted\n"));
			EndDialog(IDCANCEL);
			return TRUE;
			}
		}

	//Try and open the file we will download into
	if (!m_FileToWrite.Open(m_sFileToDownloadInto, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		{
		TRACE(_T("Failed to open the file to download into, Error:%d\n"), GetLastError());
		CString sError;
		sError.Format(_T("%d"), ::GetLastError());
		CString sMsg;
		sMsg.Format( IDS_HTTPDOWNLOAD_FAIL_FILE_OPEN, sError);
		AfxMessageBox(sMsg);
		EndDialog(IDCANCEL);
		return TRUE;
		}

	//Pull out just the filename component
	int nSlash = m_sObject.ReverseFind(_T('/'));
	if (nSlash == -1)
		nSlash = m_sObject.ReverseFind(_T('\\'));
	if (nSlash != -1 && m_sObject.GetLength() > 1)
		m_sFilename = m_sObject.Right(m_sObject.GetLength() - nSlash - 1);
	else
		m_sFilename = m_sObject;

	//Set the file status text
	CString sFileStatus;
	ASSERT(m_sObject.GetLength());
	ASSERT(m_sServer.GetLength());
	sFileStatus.Format(IDS_HTTPDOWNLOAD_FILESTATUS, m_sFilename, m_sServer);
	m_ctrlFileStatus.SetWindowText(sFileStatus);

	//Spin off the background thread which will do the actual downloading
	m_pThread = AfxBeginThread(_DownloadThread, this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED);
	if (m_pThread == NULL)
		{
		TRACE(_T("Failed to create download thread, dialog is aborting\n"));
		EndDialog(IDCANCEL);
		return TRUE;
		}
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();
	return TRUE;
	}

UINT 
CHttpDownloadDlg::_DownloadThread(LPVOID pParam){
	//Convert from the SDK world to the C++ world
	CHttpDownloadDlg* pDlg = (CHttpDownloadDlg*) pParam;
	ASSERT(pDlg);
	ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CHttpDownloadDlg)));
	pDlg->DownloadThread();
	return 0;
	}

void 
CHttpDownloadDlg::SetPercentage(int nPercentage){
	//Change the progress control
	m_ctrlProgress.SetPos(nPercentage);

	//Change the caption text
	CString sPercentage;
	sPercentage.Format(_T("%d"), nPercentage);
	CString sCaption;
	sCaption.Format(IDS_HTTPDOWNLOAD_PERCENTAGE, sPercentage, m_sFilename);
	SetWindowText(sCaption);
	}

void 
CHttpDownloadDlg::SetProgressRange(DWORD dwFileSize){
	m_ctrlProgress.SetRange(0, (short)((dwFileSize+512)/1024));
	}

void 
CHttpDownloadDlg::SetProgress(DWORD dwBytesRead){
	m_ctrlProgress.SetPos(dwBytesRead/1024);
	}

void 
CHttpDownloadDlg::SetTimeLeft(DWORD dwSecondsLeft, DWORD dwBytesRead, DWORD dwFileSize){
	CString sCopied;
	if (dwBytesRead < 1024){
		CString sBytes;
		sBytes.Format(_T("%d"), dwBytesRead);
		sCopied.Format(IDS_HTTPDOWNLOAD_BYTES, sBytes);
		}
	else if (dwBytesRead < 1048576)
		{
		CString sKiloBytes;
		sKiloBytes.Format(_T("%0.1f"), dwBytesRead/1024.0);
		sCopied.Format( IDS_HTTPDOWNLOAD_KILOBYTES, sKiloBytes);
		}
	else
		{
		CString sMegaBytes;
		sMegaBytes.Format(_T("%0.2f"), dwBytesRead/1048576.0);
		sCopied.Format( IDS_HTTPDOWNLOAD_MEGABYTES, sMegaBytes);
		}

	CString sTotal;
	if (dwFileSize < 1024)
		{
		CString sBytes;
		sBytes.Format(_T("%d"), dwFileSize);
		sTotal.Format( IDS_HTTPDOWNLOAD_BYTES, sBytes);
		}
	else if (dwFileSize < 1048576)
		{
		CString sKiloBytes;
		sKiloBytes.Format(_T("%0.1f"), dwFileSize/1024.0);
		sTotal.Format( IDS_HTTPDOWNLOAD_KILOBYTES, sKiloBytes);
		}
	else
		{
		CString sMegaBytes;
		sMegaBytes.Format(_T("%0.2f"), dwFileSize/1048576.0);
		sTotal.Format( IDS_HTTPDOWNLOAD_MEGABYTES, sMegaBytes);
		}

	CString sOf;
	sOf.Format(IDS_HTTPDOWNLOAD_OF, sCopied, sTotal);

	CString sTime;
	if (dwSecondsLeft < 60)
		{
		CString sSeconds;
		sSeconds.Format(_T("%d"), dwSecondsLeft);
		sTime.Format( IDS_HTTPDOWNLOAD_SECONDS, sSeconds);
		}
	else
		{
		DWORD dwMinutes = dwSecondsLeft / 60;
		DWORD dwSeconds = dwSecondsLeft % 60;
		CString sSeconds;
		sSeconds.Format(_T("%d"), dwSeconds);
		CString sMinutes;
		sMinutes.Format(_T("%d"), dwMinutes);
		if (dwSeconds == 0)
		  sTime.Format( IDS_HTTPDOWNLOAD_MINUTES, sMinutes);
		else
		  sTime.Format( IDS_HTTPDOWNLOAD_MINUTES_AND_SECONDS, sMinutes, sSeconds);
		}

	CString sTimeLeft;
	sTimeLeft.Format( IDS_HTTPDOWNLOAD_TIMELEFT, sTime, sOf);
	m_ctrlTimeLeft.SetWindowText(sTimeLeft);
	}

void 
CHttpDownloadDlg::SetStatus(const CString& sCaption){
	m_ctrlStatus.SetWindowText(sCaption);
	}

void 
CHttpDownloadDlg::SetStatus(UINT nID){
	CString sCaption;
	sCaption.LoadString(nID);
	SetStatus(sCaption);
	}

void 
CHttpDownloadDlg::SetStatus(CString nFmt, const CString& lpsz1){
	CString sStatus;
	sStatus.Format(nFmt, lpsz1);
	SetStatus(sStatus);
	}

void 
CHttpDownloadDlg::SetTransferRate(double KbPerSecond){
	CString sRate;
	if( KbPerSecond < 1 )
		{
		CString sBytesPerSecond;
		sBytesPerSecond.Format(_T("%0.0f"), KbPerSecond*1024);
		sRate.Format(IDS_HTTPDOWNLOAD_BYTESPERSECOND, sBytesPerSecond);
		}
	else 
	if (KbPerSecond < 10)
		{
		CString sKiloBytesPerSecond;
		sKiloBytesPerSecond.Format(_T("%0.2f"), KbPerSecond);
		sRate.Format(IDS_HTTPDOWNLOAD_KILOBYTESPERSECOND, sKiloBytesPerSecond);
		}
	else
		{
		CString sKiloBytesPerSecond;
		sKiloBytesPerSecond.Format(_T("%0.0f"), KbPerSecond);
		sRate.Format(IDS_HTTPDOWNLOAD_KILOBYTESPERSECOND, sKiloBytesPerSecond);
		}
	m_ctrlTransferRate.SetWindowText(sRate);
	}

void 
CHttpDownloadDlg::PlayAnimation(){
	m_ctrlAnimate.Play(0, (UINT)-1, (UINT)-1);
	}

void 
CHttpDownloadDlg::HandleThreadErrorWithLastError(CString sErrorFmt, DWORD dwLastError){
	//Form the error string to report
	CString sError;
	if( dwLastError )
		sError.Format(_T("%d"), dwLastError);
	else
		sError.Format(_T("%d"), ::GetLastError());

	char szError[255];
	sprintf(szError, sErrorFmt.GetBuffer(), sError.GetBuffer());
	m_sError = szError;

	//Delete the file being downloaded to if it is present
	m_FileToWrite.Close();
	::DeleteFile(m_sFileToDownloadInto);

	PostMessage(WM_HTTPDOWNLOAD_THREAD_FINISHED, 1);
	}

void 
CHttpDownloadDlg::HandleThreadError(CString sError){
	m_sError = sError;
	PostMessage(WM_HTTPDOWNLOAD_THREAD_FINISHED, 1);
	}

void 
CHttpDownloadDlg::DownloadThread(){
	Sleep(15000);
	//Create the Internet session handle
	ASSERT(m_hInternetSession == NULL);
	m_hInternetSession = ::InternetOpen(AfxGetAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInternetSession == NULL)
		{
		TRACE(_T("Failed in call to InternetOpen, Error:%d\n"), ::GetLastError());
		HandleThreadErrorWithLastError(IDS_HTTPDOWNLOAD_GENERIC_ERROR);
		return;
		}

	//Should we exit the thread
	if (m_bAbort)
		{
		PostMessage(WM_HTTPDOWNLOAD_THREAD_FINISHED);
		return;
		}  

	//Setup the status callback function
	if (::InternetSetStatusCallback(m_hInternetSession, _OnStatusCallBack) == INTERNET_INVALID_STATUS_CALLBACK)
		{
		TRACE(_T("Failed in call to InternetSetStatusCallback, Error:%d\n"), ::GetLastError());
		HandleThreadErrorWithLastError(IDS_HTTPDOWNLOAD_GENERIC_ERROR);
		return;
		}

	//Should we exit the thread
	if (m_bAbort)
		{
		PostMessage(WM_HTTPDOWNLOAD_THREAD_FINISHED);
		return;
		}  

	//Make the connection to the HTTP server          
	ASSERT(m_hHttpConnection == NULL);
	if (m_sUserName.GetLength())
		m_hHttpConnection = (HINTERNET)::InternetConnect(m_hInternetSession, m_sServer, m_nPort, m_sUserName, 
								  m_sPassword, m_dwServiceType, 0, (DWORD) this);
	else
		m_hHttpConnection = (HINTERNET)::InternetConnect(m_hInternetSession, m_sServer, m_nPort, NULL, 
								  NULL, m_dwServiceType, 0, (DWORD) this);
	if (m_hHttpConnection == NULL)
		{
		TRACE(_T("Failed in call to InternetConnect, Error:%d\n"), ::GetLastError());
		HandleThreadErrorWithLastError(IDS_HTTPDOWNLOAD_FAIL_CONNECT_SERVER);
		return;
		}

	//Should we exit the thread
	if (m_bAbort)
		{
		PostMessage(WM_HTTPDOWNLOAD_THREAD_FINISHED);
		return;
		}  

	//Start the animation to signify that the download is taking place
	PlayAnimation();

	//Issue the request to read the file
	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
	ppszAcceptTypes[1] = NULL;
	ASSERT(m_hHttpFile == NULL);
	m_hHttpFile = HttpOpenRequest(m_hHttpConnection, NULL, m_sObject, NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | 
					INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, (DWORD) this);
	if (m_hHttpFile == NULL)
		{
		TRACE(_T("Failed in call to HttpOpenRequest, Error:%d\n"), ::GetLastError());
		HandleThreadErrorWithLastError(IDS_HTTPDOWNLOAD_FAIL_CONNECT_SERVER);
		return;
		}

	//Should we exit the thread
	if (m_bAbort)
		{
		PostMessage(WM_HTTPDOWNLOAD_THREAD_FINISHED);
		return;
		}  

	//label used to jump to if we need to resend the request
	resend:

	//Issue the request
	BOOL bSend = ::HttpSendRequest(m_hHttpFile, NULL, 0, NULL, 0);
	if (!bSend)
		{ 
		DWORD dwLastError = ::GetLastError();
		TRACE(_T("Failed in call to HttpSendRequest, Error:%d\n"), dwLastError);

		if( dwLastError == ERROR_INTERNET_CANNOT_CONNECT )
			{
			HandleThreadError(IDS_HTTPDOWNLOAD_CANNOT_CONNECT);
			return;
			}

		CString sError = IDS_HTTPDOWNLOAD_FAIL_CONNECT_SERVER;
		HandleThreadErrorWithLastError(sError);
		return;
		}

	//Check the HTTP status code
	TCHAR szStatusCode[32];
	DWORD dwInfoSize = 32;
	if (!HttpQueryInfo(m_hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
		{
		TRACE(_T("Failed in call to HttpQueryInfo for HTTP query status code, Error:%d\n"), ::GetLastError());
		CString sError = IDS_HTTPDOWNLOAD_INVALID_SERVER_RESPONSE;
		HandleThreadErrorWithLastError(sError);
		return;
		}
	else
		{
		long nStatusCode = _ttol(szStatusCode);

		//Handle any authentication errors
		if (nStatusCode == HTTP_STATUS_PROXY_AUTH_REQ || nStatusCode == HTTP_STATUS_DENIED)
			{
			// We have to read all outstanding data on the Internet handle
			// before we can resubmit request. Just discard the data.
			char szData[51];
			DWORD dwSize;
			do
				{
				::InternetReadFile(m_hHttpFile, (LPVOID)szData, 50, &dwSize);
				}
			while (dwSize != 0);

			//Bring up the standard authentication dialog
			if (::InternetErrorDlg(GetSafeHwnd(), m_hHttpFile, ERROR_INTERNET_INCORRECT_PASSWORD, FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
						 FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, NULL) == ERROR_INTERNET_FORCE_RETRY)
			goto resend;
			}
		else if (nStatusCode != HTTP_STATUS_OK)
			{
			TRACE(_T("Failed to retrieve a HTTP 200 status, Status Code:%d\n"), nStatusCode);
			HandleThreadErrorWithLastError(IDS_HTTPDOWNLOAD_INVALID_HTTP_RESPONSE, nStatusCode);
			return;
			}
		}

	//Update the status control to reflect that we are getting the file information
	SetStatus(IDS_HTTPDOWNLOAD_GETTING_FILE_INFORMATION);

	// Get the length of the file.            
	TCHAR szContentLength[32];
	dwInfoSize = 32;
	DWORD dwFileSize = 0;
	BOOL bGotFileSize = FALSE;
	if (::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
		{
		//Set the progress control range
		bGotFileSize = TRUE;
		dwFileSize = (DWORD) _ttol(szContentLength);
		SetProgressRange(dwFileSize);
		}

	//Update the status to say that we are now downloading the file
	SetStatus(IDS_HTTPDOWNLOAD_RETREIVEING_FILE);

	//Now do the actual read of the file
	DWORD dwStartTicks = ::GetTickCount();
	DWORD dwCurrentTicks = dwStartTicks;
	DWORD dwBytesRead = 0;
	char szReadBuf[1024];
	DWORD dwBytesToRead = 1024;
	DWORD dwTotalBytesRead = 0;
	DWORD dwLastTotalBytes = 0;
	DWORD dwLastPercentage = 0;
	do
		{
		if (!::InternetReadFile(m_hHttpFile, szReadBuf, dwBytesToRead, &dwBytesRead))
			{
			TRACE(_T("Failed in call to InternetReadFile, Error:%d\n"), ::GetLastError());
			HandleThreadErrorWithLastError(IDS_HTTPDOWNLOAD_ERROR_READFILE);
			return;
			}
		else if (dwBytesRead && !m_bAbort)
			{
			//Write the data to file
			TRY
			{
			m_FileToWrite.Write(szReadBuf, dwBytesRead);
			}
			CATCH(CFileException, e);                                          
			{
			TRACE(_T("An exception occured while writing to the download file\n"));
			HandleThreadErrorWithLastError(IDS_HTTPDOWNLOAD_ERROR_READFILE, e->m_lOsError);
			e->Delete();
			return;
			}
			END_CATCH

			//Increment the total number of bytes read
			dwTotalBytesRead += dwBytesRead;  

			UpdateControlsDuringTransfer(dwStartTicks, dwCurrentTicks, dwTotalBytesRead, dwLastTotalBytes, 
								 dwLastPercentage, bGotFileSize, dwFileSize);
			}
		} 
	while (dwBytesRead && !m_bAbort);

	//Delete the file being downloaded to if it is present and the download was aborted
	m_FileToWrite.Close();
	if (m_bAbort)
	::DeleteFile(m_sFileToDownloadInto);

	//We're finished
	PostMessage(WM_HTTPDOWNLOAD_THREAD_FINISHED);
	}

void 
CHttpDownloadDlg::UpdateControlsDuringTransfer(DWORD dwStartTicks, DWORD& dwCurrentTicks, DWORD dwTotalBytesRead, DWORD& dwLastTotalBytes, 
                                                    DWORD& dwLastPercentage, BOOL bGotFileSize, DWORD dwFileSize)
	{
	if (bGotFileSize)
		{
		//Update the percentage downloaded in the caption
		DWORD dwPercentage = (DWORD) (dwTotalBytesRead * 100.0 / dwFileSize);
		if (dwPercentage != dwLastPercentage)
			{
			SetPercentage(dwPercentage);
			dwLastPercentage = dwPercentage;
			//Update the progress control bar
			SetProgress(dwTotalBytesRead);
			}
		}

	//Update the transfer rate amd estimated time left every second
	DWORD dwNowTicks = GetTickCount();
	DWORD dwTimeTaken = dwNowTicks - dwCurrentTicks;
	if (dwTimeTaken > 1000)
		{
		double KbPerSecond = ((double)(dwTotalBytesRead) - (double)(dwLastTotalBytes)) / ((double)(dwTimeTaken));
		SetTransferRate(KbPerSecond);

		//Setup for the next time around the loop
		dwCurrentTicks = dwNowTicks;
		dwLastTotalBytes = dwTotalBytesRead;

		if (bGotFileSize)
			{
			//Update the estimated time left
			if (dwTotalBytesRead)
				{
				DWORD dwSecondsLeft = (DWORD) (((double)dwNowTicks - dwStartTicks) / dwTotalBytesRead * 
					   (dwFileSize - dwTotalBytesRead) / 1000);
				SetTimeLeft(dwSecondsLeft, dwTotalBytesRead, dwFileSize);
				}
			}
		}
	}

void CALLBACK 
CHttpDownloadDlg::_OnStatusCallBack(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, 
                                    LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
	{
	//Convert from the SDK C world to the C++ world
	CHttpDownloadDlg* pDlg = (CHttpDownloadDlg*) dwContext;
	ASSERT(pDlg);
	ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CHttpDownloadDlg)));
	pDlg->OnStatusCallBack(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
	}

void 
CHttpDownloadDlg::OnStatusCallBack(HINTERNET /*hInternet*/, DWORD dwInternetStatus, 
                                         LPVOID lpvStatusInformation, DWORD /*dwStatusInformationLength*/)
	{
	switch (dwInternetStatus)
		{
		case INTERNET_STATUS_RESOLVING_NAME:
			{
			SetStatus(IDS_HTTPDOWNLOAD_RESOLVING_NAME, (LPCTSTR) lpvStatusInformation);
			break;
			}
		case INTERNET_STATUS_NAME_RESOLVED:
			{
			SetStatus(IDS_HTTPDOWNLOAD_RESOLVED_NAME, (LPCTSTR) lpvStatusInformation);
			break;
			}
		case INTERNET_STATUS_CONNECTING_TO_SERVER:
			{
			SetStatus(IDS_HTTPDOWNLOAD_CONNECTING, (LPCTSTR) lpvStatusInformation);
			break;
			}
		case INTERNET_STATUS_CONNECTED_TO_SERVER:
			{
			SetStatus(IDS_HTTPDOWNLOAD_CONNECTED, (LPCTSTR) lpvStatusInformation);
			break;
			}
		case INTERNET_STATUS_REDIRECT:
			{
			SetStatus(IDS_HTTPDOWNLOAD_REDIRECTING, (LPCTSTR) lpvStatusInformation);
			break;
			}
		default:
			{
			break;
			}
		}
	}

void 
CHttpDownloadDlg::OnDestroy() 
	{
	//Wait for the worker thread to exit
	if (m_pThread)
		{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		delete m_pThread;
		m_pThread = NULL;
		}

	//Free up the internet handles we may be using
	if (m_hHttpFile)
		{
		::InternetCloseHandle(m_hHttpFile);
		m_hHttpFile = NULL;
		}

	if (m_hHttpConnection)
		{
		::InternetCloseHandle(m_hHttpConnection);
		m_hHttpConnection = NULL;
		}

	if (m_hInternetSession)
		{
		::InternetCloseHandle(m_hInternetSession);
		m_hInternetSession = NULL;
		}

	//Let the parent class do its thing
	CDialog::OnDestroy();

	delete [] m_lpDialogTemplate;
	}

void 
CHttpDownloadDlg::OnCancel() 
	{
	//Just set the abort flag to TRUE and
	//disable the cancel button
	m_bAbort = TRUE;	
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	SetStatus(IDS_HTTPDOWNLOAD_ABORTING_TRANSFER);
	}

void 
CHttpDownloadDlg::OnClose() 
	{
	if( m_bSafeToClose)	
		CDialog::OnClose();
	else
		{
		//Just set the abort flag to TRUE and
		//disable the cancel button
		m_bAbort = TRUE;	
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		SetStatus(IDS_HTTPDOWNLOAD_ABORTING_TRANSFER);
		}
	}
