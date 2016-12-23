
// SMTPServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMTPServer.h"
#include "SMTPServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSMTPServerDlg �Ի���

 int Count = 0;

CSMTPServerDlg::CSMTPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SMTPSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSMTPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_info);
	DDX_Control(pDX, IDC_LIST1, m_log);
	DDX_Control(pDX, IDC_BMP, m_bmp);
}

BEGIN_MESSAGE_MAP(CSMTPServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CSMTPServerDlg ��Ϣ�������

BOOL CSMTPServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	int bFlag = MySock.Create(25, SOCK_STREAM, FD_ACCEPT | FD_READ);
	if (bFlag == 0)
	{
		m_log.SetWindowText(L"SMTP�׽ӿڽ���ʧ��\r\n");
		MySock.Close();
	}
	else
	{
		m_log.InsertString(Count++, _T("*** SMTP������׼����"));
		m_log.InsertString(Count++, _T("$$$$$$$$$$$$$$$$$$$$$$$$"));
		int i = MySock.Listen(1);
		if (i == 0)
		{
			m_log.SetWindowText(L"SMTP����ʧ��\r\n");
			MySock.Close();
		}
		else
		{
			m_log.SetWindowText(L"SMTP������׼����\r\n\r\n");
		}
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSMTPServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSMTPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

