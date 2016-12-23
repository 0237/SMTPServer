// SMTPsocket.cpp : ʵ���ļ�
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


// SMTPsocket ��Ա����


void SMTPsocket::OnAccept(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSMTPServerDlg * pMMD = (CSMTPServerDlg*)AfxGetMainWnd();
	pMMD->m_log.InsertString(Count++, _T("*** �յ���������"));
	SMTPsocket *NewSock = new SMTPsocket();

	int IsConnect = Accept(*NewSock);
	if (IsConnect != 0)
	{
		char* send = "220 SMTP is ready \r\n";
		NewSock->Send(send, strlen(send), 0);
		NewSock->AsyncSelect(FD_READ);//���ý��ܺ������в�����
		pMMD->m_log.InsertString(Count++, _T("*** ��������"));
		pMMD->m_log.InsertString(Count++, _T("S: 220 Simple Mail Server Ready for mail"));
	}
	else
	{
		pMMD->m_log.InsertString(Count++, _T("*** ��������ʧ��"));
	}
	CAsyncSocket::OnAccept(nErrorCode);
}


void SMTPsocket::OnClose(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnClose(nErrorCode);
}


void SMTPsocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	int length = Receive(data, sizeof(data), 0);//���յ������ݴ浽data
	CString receive;
	receive += data;
	CString PreLog, Log;

	AfxGetMainWnd()->GetDlgItemText(IDC_LIST1, PreLog);
	CSMTPServerDlg * pMMD = (CSMTPServerDlg*)AfxGetMainWnd();
	if (!IsData)//д������־
	{
		pMMD->m_log.InsertString(Count++, _T("C: "+ receive.Left(length)));
	}
	else
	{
		if(DateFinish)
			pMMD->m_log.InsertString(Count++, _T("C: " + receive.Left(4)));
	}

	if (length != 0)//������յ�����
	{
		if (!IsData)//������յ���������
		{
			//�ֱ�Բ�ͬ�������Ӧ��
			if (receive.Left(4) == "EHLO")
			{
				char* send = "250 OK 127.0.0.1\r\n";//Ӧ��

				Send(send, strlen(send), 0);//����Ӧ��
				//Log = Log + L"S:" + L"127.0.0.1\r\n";
				AsyncSelect(FD_READ);//�������պ���
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
				DateFinish = false;//����δ�������
				IsData = true;//����յ�DATA���˵���������Ľ��յ���������
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
				Quit = true;//�ͻ����˳������ֹ����
				//AsyncSelect(FD_READ);
				pMMD->m_log.InsertString(Count++, _T("S: 221 Quit, Goodbye !"));
				//return;
			}
			else//�������ƥ��
			{
				char* send = "500 order is wrong\r\n";

				Send(send, strlen(send), 0);
				Quit = true;
				pMMD->m_log.InsertString(Count++, _T("S: 500 order is wrong"));
				return;
			}
		}
		else//���յ������ݣ�������δ������ȫʱ��������Ӧ��
		{
			CString str1, str2;
			AfxGetMainWnd()->GetDlgItemText(IDC_EDIT1, str1);
			str2 = str1 + receive.Left(length);
			AfxGetMainWnd()->SetDlgItemText(IDC_EDIT1, str2);
			if (receive.Find(_T("\r\n.\r\n")) != -1)//���ݽ������
			{
				DateFinish = true;
				IsData = false;
				char* send;
				send = "250 Message accepted for delivery\r\n";//����Ӧ��

				Send(send, strlen(send), 0);
				pMMD->m_log.InsertString(Count++, _T("S: 250 Message accepted for delivery"));
				AfxGetMainWnd()->GetDlgItemText(IDC_EDIT1, pic);
				if (pic.Find(_T("Content-Type: image/bmp")) != -1)//��������bmpͼƬ
				{
					//��ȡbmpͼƬ��base64����
					int Attachment_Start = pic.Find(_T("Content-Disposition: attachment"), 0);
					int Bmp_Start = pic.Find(_T("\r\n\r\n"), Attachment_Start);
					CString Start = pic.Mid(Bmp_Start + 4, pic.GetLength() - Bmp_Start - 4);
					int length = Start.Find(_T("\r\n\r\n"), 0);
					pic = Start.Left(length);
					HBITMAP picture;
					//����
					DeCode(pic, picture);
					//���뵽�Ի���
					pMMD->m_bmp.SetBitmap(picture);
				}
			}
			AsyncSelect(FD_READ);

			return;
		}
	}
	if (Quit)//�˳�
	{
		Close();//�ر��׽���
		Quit = false;
		return;

	}
	CAsyncSocket::OnReceive(nErrorCode);
}
