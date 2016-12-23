// SMTPsocket.cpp : 实现文件
//

#include "stdafx.h"
#include "SMTPServer.h"
#include "SMTPsocket.h"
#include "base.h"
#include "SMTPServerDlg.h"


// SMTPsocket

SMTPsocket::SMTPsocket()
	: IsData(false)
	, Quit(false)
	, IsBmp(false)
	, pic(_T(""))
	, DateFinish(true)
{
}

SMTPsocket::~SMTPsocket()
{
}


// SMTPsocket 成员函数


void SMTPsocket::OnAccept(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSMTPServerDlg * pMMD = (CSMTPServerDlg*)AfxGetMainWnd();
	pMMD->m_log.InsertString(Count++, _T("*** 收到连接请求"));
	SMTPsocket *NewSock = new SMTPsocket();

	int IsConnect = Accept(*NewSock);
	if (IsConnect != 0)
	{
		char* send = "220 SMTP is ready \r\n";
		NewSock->Send(send, strlen(send), 0);
		NewSock->AsyncSelect(FD_READ);//调用接受函数进行操作。
		pMMD->m_log.InsertString(Count++, _T("*** 建立连接"));
		pMMD->m_log.InsertString(Count++, _T("S: 220 Simple Mail Server Ready for mail"));
	}
	else
	{
		pMMD->m_log.InsertString(Count++, _T("*** 建立连接失败"));
	}
	CAsyncSocket::OnAccept(nErrorCode);
}


void SMTPsocket::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnClose(nErrorCode);
}


void SMTPsocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	int length = Receive(data, sizeof(data), 0);//接收到的数据存到data
	CString receive;
	receive += data;
	CString PreLog, Log;

	AfxGetMainWnd()->GetDlgItemText(IDC_LIST1, PreLog);
	CSMTPServerDlg * pMMD = (CSMTPServerDlg*)AfxGetMainWnd();
	if (!IsData)//写接收日志
	{
		pMMD->m_log.InsertString(Count++, _T("C: "+ receive.Left(length)));
	}
	else
	{
		if(DateFinish)
			pMMD->m_log.InsertString(Count++, _T("C: " + receive.Left(4)));
	}

	if (length != 0)//如果接收到数据
	{
		if (!IsData)//如果接收到的是命令
		{
			//分别对不同命令进行应答
			if (receive.Left(4) == "EHLO")
			{
				char* send = "250 OK 127.0.0.1\r\n";//应答

				Send(send, strlen(send), 0);//发送应答
				//Log = Log + L"S:" + L"127.0.0.1\r\n";
				AsyncSelect(FD_READ);//触发接收函数
				pMMD->m_log.InsertString(Count++, _T("S: 250 OK 127.0.0.1"));
				return;
			}
			else if (receive.Left(10) == "MAIL FROM:")
			{
				char *send = "250 Sender OK\r\n";

				Send(send, strlen(send), 0);
				AsyncSelect(FD_READ);
				pMMD->m_log.InsertString(Count++, _T("S: 250 Sender OK"));
				return;
			}
			else if (receive.Left(8) == "RCPT TO:")
			{
				char* send = "250 Receiver OK\r\n";

				Send(send, strlen(send), 0);
				AsyncSelect(FD_READ);
				pMMD->m_log.InsertString(Count++, _T("S: 250 Receiver OK"));
				return;
			}
			else if (receive.Left(4) == "DATA")
			{
				DateFinish = false;//数据未接收完成
				IsData = true;//如果收到DATA命令，说明接下来的接收到的是数据
				char* send = "354 Go ahead. End with <CRLF>.<CRLF>\r\n";

				Send(send, strlen(send), 0);

				AsyncSelect(FD_READ);
				pMMD->m_log.InsertString(Count++, _T("S: 354 Go ahead. End with <CRLF>.<CRLF>"));
				return;
			}
			else if (receive.Left(4) == "QUIT" || receive.Left(4) == "RSET")
			{
				char* send = "221 Quit, Goodbye !\r\n";

				Send(send, strlen(send), 0);
				Quit = true;//客户端退出命令，终止程序
				//AsyncSelect(FD_READ);
				pMMD->m_log.InsertString(Count++, _T("S: 221 Quit, Goodbye !"));
				//return;
			}
			else//命令均不匹配
			{
				char* send = "500 order is wrong\r\n";

				Send(send, strlen(send), 0);
				Quit = true;
				pMMD->m_log.InsertString(Count++, _T("S: 500 order is wrong"));
				return;
			}
		}
		else//接收的是数据，在数据未接收完全时，不发送应答
		{
			CString str1, str2;
			AfxGetMainWnd()->GetDlgItemText(IDC_EDIT1, str1);
			str2 = str1 + receive.Left(length);
			AfxGetMainWnd()->SetDlgItemText(IDC_EDIT1, str2);
			if (receive.Find(_T("\r\n.\r\n")) != -1)//数据接收完成
			{
				DateFinish = true;
				IsData = false;
				char* send;
				send = "250 Message accepted for delivery\r\n";//发送应答

				Send(send, strlen(send), 0);
				pMMD->m_log.InsertString(Count++, _T("S: 250 Message accepted for delivery"));
				AfxGetMainWnd()->GetDlgItemText(IDC_EDIT1, pic);
				if (pic.Find(_T("Content-Type: image/bmp")) != -1)//附件中有bmp图片
				{
					//截取bmp图片的base64编码
					int Attachment_Start = pic.Find(_T("Content-Disposition: attachment"), 0);
					int Bmp_Start = pic.Find(_T("\r\n\r\n"), Attachment_Start);
					CString Start = pic.Mid(Bmp_Start + 4, pic.GetLength() - Bmp_Start - 4);
					int length = Start.Find(_T("\r\n\r\n"), 0);
					pic = Start.Left(length);
					HBITMAP picture;
					//解码
					DeCode(pic, picture);
					//输入到对话框
					pMMD->m_bmp.SetBitmap(picture);
				}
			}
			AsyncSelect(FD_READ);

			return;
		}
	}
	if (Quit)//退出
	{
		Close();//关闭套接字
		Quit = false;
		return;

	}
	CAsyncSocket::OnReceive(nErrorCode);
}
