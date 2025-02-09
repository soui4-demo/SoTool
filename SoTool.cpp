// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "MainDlg.h"
#include "com-cfg.h"
#include "SCaptureButton.h"
#include "STabCtrlEx.h"
#include <SouiFactory.h>
#pragma comment(lib,"shlwapi.lib")

//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 1
//#define RES_TYPE 0   //从文件中加载资源
// #define RES_TYPE 1  //从PE资源中加载UI资源

#define INIT_R_DATA
#include "res/resource.h"

#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    HRESULT hRes = OleInitialize(NULL);
    SASSERT(SUCCEEDED(hRes));
	SouiFactory souiFac;
    int nRet = 0;
    
    SComMgr *pComMgr = new SComMgr(_T("imgdecoder-gdip"));

    {
        BOOL bLoaded=FALSE;
        CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
        CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
        bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));
        bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        SApplication *theApp = new SApplication(pRenderFactory, hInstance);
        
        theApp->RegisterWindowClass<SEdit2>();
        theApp->RegisterWindowClass<SImgCanvas>();
        theApp->RegisterWindowClass<SFolderTreeList>();
		theApp->RegisterWindowClass<SCaptureButton>();
		theApp->RegisterWindowClass<STabCtrlEx>();

        HMODULE hSysResource=LoadLibrary(SYS_NAMED_RESOURCE);
        if(hSysResource)
        {
            CAutoRefPtr<IResProvider> sysSesProvider;
            sysSesProvider.Attach(souiFac.CreateResProvider(RES_PE));
            sysSesProvider->Init((WPARAM)hSysResource,0);
            theApp->LoadSystemNamedResource(sysSesProvider);
        }

        CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
        //将程序的运行路径修改到项目所在目录所在的目录
        SStringT strPath = theApp->GetAppDir();
        strPath += _T("\\..\\demos\\SoTool");
        SetCurrentDirectory(strPath);
		
		pResProvider.Attach(souiFac.CreateResProvider(RES_FILE));
        if (!pResProvider->Init((LPARAM)_T("uires"), 0))
        {
            SASSERT(0);
            return 1;
        }
#else 
		pResProvider.Attach(souiFac.CreateResProvider(RES_PE));
        pResProvider->Init((WPARAM)hInstance, 0);
#endif

        theApp->AddResProvider(pResProvider);

        
        // BLOCK: Run application
        {
            CMainDlg dlgMain;
            dlgMain.Create(GetActiveWindow());
            dlgMain.SendMessage(WM_INITDIALOG);
            dlgMain.CenterWindow(dlgMain.m_hWnd);
            dlgMain.ShowWindow(SW_SHOWNORMAL);
            nRet = theApp->Run(dlgMain.m_hWnd);
        }

        delete theApp;
    }
    
    delete pComMgr;
    
    OleUninitialize();
    return nRet;
}
