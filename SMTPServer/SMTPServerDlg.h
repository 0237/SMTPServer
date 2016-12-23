
// SMTPServerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "SMTPsocket.h"

// CSMTPServerDlg 对话框
class CSMTPServerDlg : public CDialogEx
{
// 构造
public:
	SMTPsocket MySock;
	CSMTPServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMTPSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 接收内容
	CEdit m_info;
	// 服务器日志
	CListBox m_log;
	// 附件图片
	CStatic m_bmp;
};
