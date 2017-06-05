#pragma once
#include <wininet.h>

class CHttpDownloadDlg : public CDialog
{
public:
//Constructors / Destructors
	CHttpDownloadDlg(CWnd* pParent = NULL);

	DECLARE_DYNAMIC(CHttpDownloadDlg);

//Public Member variables
	CString		m_sURLToDownload;
	CString		m_sFileToDownloadInto;
	CString		m_sUserName;
	CString		m_sPassword;

protected:
	CString       m_sError;
	CString       m_sServer; 
	CString       m_sObject; 
	CString       m_sFilename;
	INTERNET_PORT m_nPort;
	DWORD         m_dwServiceType;
	HINTERNET     m_hInternetSession;
	HINTERNET     m_hHttpConnection;
	HINTERNET     m_hHttpFile;
	BOOL          m_bAbort;
	BOOL          m_bSafeToClose;
	CFile         m_FileToWrite;
	CWinThread*   m_pThread;

protected:
	static void CALLBACK _OnStatusCallBack(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, 
										LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	void OnStatusCallBack(HINTERNET hInternet, DWORD dwInternetStatus, 
						LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	static UINT _DownloadThread(LPVOID pParam);
	void HandleThreadErrorWithLastError(CString sErrorFmt, DWORD dwLastError=0);
	void HandleThreadError(CString sError);
	void DownloadThread();
	void SetPercentage(int nPercentage);
	void SetTimeLeft(DWORD dwSecondsLeft, DWORD dwBytesRead, DWORD dwFileSize);
	void SetProgressRange(DWORD dwFileSize);
	void SetStatus(const CString& sCaption);
	void SetStatus(UINT nID);
	void SetStatus(CString nFmt, const CString& lpsz1);
	void SetTransferRate(double KbPerSecond);
	void PlayAnimation();
	void SetProgress(DWORD dwBytesRead);
	void UpdateControlsDuringTransfer(DWORD dwStartTicks, DWORD& dwCurrentTicks, DWORD dwTotalBytesRead, DWORD& dwLastTotalBytes, 
									DWORD& dwLastPercentage, BOOL bGotFileSize, DWORD dwFileSize);

protected:
	//{{AFX_DATA(CHttpDownloadDlg)
	CStatic			m_ctrlStatus;
	CStatic			m_ctrlTransferRate;
	CStatic			m_ctrlTimeLeft;
	CProgressCtrl	m_ctrlProgress;
	CStatic			m_ctrlFileStatus;
	CAnimateCtrl	m_ctrlAnimate;
	//}}AFX_DATA

	virtual void	DoDataExchange	(CDataExchange* pDX);
	virtual BOOL	OnInitDialog	();
	virtual void	OnCancel		();

	afx_msg void	OnDestroy		();
	afx_msg void	OnClose			();
	afx_msg LRESULT OnThreadFinished(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

