// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	
#include "../uieditor/Global.h"

CMainDlg::CMainDlg(LPCTSTR pszLayoutId) : SHostWnd(pszLayoutId),m_pHover(NULL)
{
}

CMainDlg::~CMainDlg()
{
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	const WCHAR *pszXml=L"<window float=\"1\" msgTransparent=\"1\" margin=\"1,1,1,1\" colorBorder=\"#ff0000\"/>";
	m_pHover = CreateChildren(pszXml);
	return 0;
}
//TODO:消息映射
void CMainDlg::OnClose()
{
	SNativeWnd::DestroyWindow();
	PostQuitMessage(0);
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

LRESULT CMainDlg::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(GetKeyState(VK_MENU)&0x80)
	{
		SetMsgHandled(FALSE);
		return TRUE;
	}else
	{
		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		if(uMsg == WM_LBUTTONDOWN)
		{//tell editor the clicked window id.
			m_pHover->SetVisible(FALSE);
			SWND swnd = SwndFromPoint(pt);
			m_pHover->SetVisible(TRUE);
			if(swnd)
			{
				SWindow *p = SWindowMgr::GetWindow(swnd);
				SList<int> lstIndex;
				GetSwndIndex(p,lstIndex);
				HWND hEditor = FindWindowEx(NULL,NULL,ksz_editor_cls,ksz_editor_wnd);
				if(hEditor)
				{
					COPYDATASTRUCT cds;
					cds.dwData = kcds_id;
					cds.cbData = sizeof(int)*lstIndex.GetCount();
					int *pData = new int[lstIndex.GetCount()];
					int i=0;
					SPOSITION pos = lstIndex.GetHeadPosition();
					while(pos)
					{
						pData[i++]=lstIndex.GetNext(pos);
					}
					cds.lpData = pData;
					::SendMessage(hEditor,WM_COPYDATA,(WPARAM)m_hWnd,(LPARAM)&cds);
					delete []pData;
				}
			}
		}else if(uMsg == WM_MOUSEMOVE)
		{//highlight the hovered window
			m_pHover->SetVisible(FALSE);
			SWND swnd = SwndFromPoint(pt);
			m_pHover->SetVisible(TRUE);
			if(swnd)
			{
				SWindow *p = SWindowMgr::GetWindow(swnd);
				CRect rc = p->GetWindowRect();
				m_pHover->Move(rc);
			}else
			{
				m_pHover->Move(0,0,0,0);
			}
		}

		return TRUE;
	}
}

void CMainDlg::GetSwndIndex(SWindow *pWnd,SList<int> &lstIndex)
{
	int idx = 0;
	SWindow *pPrev = pWnd->GetWindow(GSW_PREVSIBLING);
	while(pPrev)
	{
		idx++;
		pPrev = pPrev->GetWindow(GSW_PREVSIBLING);
	}
	lstIndex.AddHead(idx);
	SWindow *pParent = pWnd->GetParent();
	if(pParent)
	{
		GetSwndIndex(pParent,lstIndex);
	}
}

