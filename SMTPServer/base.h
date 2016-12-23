#include"StdAfx.h"
BOOL SwapCharToByte(BYTE&, char&);//换码
BOOL DeCode(CString &, HBITMAP &);//解码并写成bmp图片
char symbol[65] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', '+', '/' };
BOOL SwapCharToByte(BYTE& a, char& b)
{
	for (int i = 0; i<65; i++)
	{
		if (b == symbol[i])
		{
			a = i & 0x00ff;//截取低字节
			return true;
		}
	}
	return false;
}

BOOL DeCode(CString &attachment, HBITMAP &picture)
{
	BYTE buf[4];
	CFile file;
	if (file.Open(L"1.bmp", CFile::modeWrite | CFile::modeCreate))
	{
		int len = attachment.GetLength();
		BYTE temp[4];
		int i = 0;
		while (i<len)
		{
			for (int k = 0; k<4; k++)
			{
				if (i<len)
				{
					char t = attachment.GetAt(i);//获得字符串里的每一个字符
					if (!SwapCharToByte(temp[k], t))
					{
						temp[k] = 255;
						k--;
					}
					i++;
				}
			}
			//base64解码，四个六位变成三个八位
			buf[0] = (temp[0] << 2) | (temp[1] >> 4);
			buf[1] = (temp[1] << 4) | (temp[2] >> 2);
			buf[2] = (temp[2] << 6) | (temp[3]);
			//写入文件
			file.Write(buf, 3);
			file.SeekToEnd();
		}
	}
	file.Close();
	//文件变成bmp图像
	picture = (HBITMAP)::LoadImage(NULL, _T("1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	return true;
}

