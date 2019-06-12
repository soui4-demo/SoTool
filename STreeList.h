#pragma once

#include <control/streectrl.h>

namespace SOUI
{
    class SMCTreeCtrl : public STreeCtrl
    {
        SOUI_CLASS_NAME(SMCTreeCtrl,L"mctreectrl")

        friend class STreeList;
        struct MCITEM
        {
            SArray<SStringT> arrText;
            LPARAM   lParam;
        };
    public:
        SMCTreeCtrl();
        ~SMCTreeCtrl();
        
        int InsertColumn(int iCol,int nWid);
        BOOL SetColWidth(int iCol,int nWid);
        void SetTreeWidth(int nWid);
        BOOL DeleteColumn(int iCol);
        BOOL SetItemText(HSTREEITEM hItem,int iCol,const SStringT strText);

        void SetItemData(HSTREEITEM hItem, LPARAM lParam);
        LPARAM GetItemData(HSTREEITEM hItem);

        HSTREEITEM InsertItem(LPCTSTR pszText,int iImage,int iSelImage,LPARAM lParam,HSTREEITEM hParent=STVI_ROOT,HSTREEITEM hAfter = STVI_LAST,BOOL bEnsureVisible=FALSE);
    protected:
        virtual void DrawItem(IRenderTarget *pRT, const CRect & rc, HSTREEITEM hItem);
        virtual void DrawTreeItem(IRenderTarget *pRT, CRect & rc,HSTREEITEM hItem);
        virtual void DrawListItem(IRenderTarget *pRT, CRect & rc,HSTREEITEM hItem);
        
        virtual void OnNodeFree(LPTVITEM & pItemData);
        virtual void OnInsertItem(LPTVITEM & pItemData);
        virtual int CalcItemWidth(const LPTVITEM pItem);
        
        virtual void OnFinalRelease();
    protected:
        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"treeWidth",m_nTreeWidth,FALSE)
        SOUI_ATTRS_END()

        SArray<int> m_arrColWidth;
        int         m_nTreeWidth;
        int         m_nItemWid;
    };
    
    class STreeList : public SWindow
    {
    SOUI_CLASS_NAME(STreeList,L"treelist")
    public:
        STreeList(void);
        ~STreeList(void);
        
        SMCTreeCtrl * GetMCTreeCtrl(){return m_pTreeCtrl;}
        
    protected:
            /**
        * SListCtrl::OnHeaderClick
        * @brief    列表头单击事件 -- 
        * @param    EventArgs *pEvt
        *
        * Describe  列表头单击事件
        */
        bool            OnHeaderClick(EventArgs *pEvt);
        /**
        * SListCtrl::OnHeaderSizeChanging
        * @brief    列表头大小改变
        * @param    EventArgs *pEvt -- 
        *
        * Describe  列表头大小改变
        */
        bool            OnHeaderSizeChanging(EventArgs *pEvt);
        /**
        * SListCtrl::OnHeaderSwap
        * @brief    列表头交换
        * @param    EventArgs *pEvt -- 
        *
        * Describe  列表头交换
        */
        bool            OnHeaderSwap(EventArgs *pEvt);

        bool            OnScrollEvent(EventArgs *pEvt);
    protected:
        virtual BOOL CreateChildren(pugi::xml_node xmlNode);
        virtual BOOL OnRelayout(const CRect &rcWnd);
        virtual SHeaderCtrl * CreateHeader();
        virtual SMCTreeCtrl * CreateMcTreeCtrl();

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"headerHeight",m_nHeaderHeight,FALSE)
            ATTR_I18NSTRT(L"treeLabel",m_strTreeLabel,FALSE)
        SOUI_ATTRS_END()
        
        SHeaderCtrl *m_pHeader;
        int          m_nHeaderHeight;
        STrText      m_strTreeLabel;
        
        SMCTreeCtrl *m_pTreeCtrl;
    };
}

