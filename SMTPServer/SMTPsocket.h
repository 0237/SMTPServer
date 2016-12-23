#if !defined(AFX_NEWSOCKCLASS_H__8EB4225C_953E_4C9D_B82B_735228DD045A__INCLUDED_)
#define AFX_NEWSOCKCLASS_H__8EB4225C_953E_4C9D_B82B_735228DD045A__INCLUDED_
#define BufferLen 4096
#endif

// SMTPsocket ÃüÁîÄ¿±ê
#if _MSC_VER > 1000
#pragma once
#endif

extern int Count;

class SMTPsocket : public CAsyncSocket
{
public:
	SMTPsocket();
	virtual ~SMTPsocket();
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	bool IsData;
	bool DateFinish;
	bool Quit;
	bool IsBmp;
	CString pic;
	char data[BufferLen];
};


