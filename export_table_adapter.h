#pragma once
#include <Dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")
#include <helper/SAdapterBase.h>
struct ExportTableItemData
{		
	SStringT strName;//用户名
};
class CExportTableTreeViewAdapter :public STreeAdapterBase<ExportTableItemData>
{
public:	
	CExportTableTreeViewAdapter() {}
	~CExportTableTreeViewAdapter() {}
	
	virtual void getView(HSTREEITEM loc, SWindow * pItem, SXmlNode xmlTemplate)
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)loc);
		int itemType = getViewType(loc);
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(&xmlTemplate.child(L"item_data"));
		}		
		pItem->FindChildByName(L"txt_fun_name")->SetWindowText(ii.data.strName);
	}		

	int Updata(LPBYTE lpBaseAddress)
	{
		m_tree.DeleteAllItems();
		notifyBranchChanged(ITEM_ROOT);		
		if (lpBaseAddress == NULL)
		{
			return -1;
		}
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBaseAddress;
		PIMAGE_NT_HEADERS32 pNtHeaders = (PIMAGE_NT_HEADERS32)(lpBaseAddress + pDosHeader->e_lfanew);
		//测试一下是不是一个有效的PE文件
		if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE || IMAGE_NT_SIGNATURE != pNtHeaders->Signature)
		{			
			return -1;
		}
		if (pNtHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
		{
			DWORD Rva_export_table = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

			if (Rva_export_table == 0)
			{
				return -1;
			}

			PIMAGE_EXPORT_DIRECTORY pImportTable = (PIMAGE_EXPORT_DIRECTORY)ImageRvaToVa(
				(PIMAGE_NT_HEADERS)pNtHeaders,
				lpBaseAddress,
				Rva_export_table,
				NULL
			);
			DWORD **ppdwNames = (DWORD **)pImportTable->AddressOfNames;

			ppdwNames = (PDWORD*)ImageRvaToVa((PIMAGE_NT_HEADERS)pNtHeaders,
				pDosHeader, (DWORD)ppdwNames, 0);
			if (!ppdwNames)
			{
				return -1;
			}

			IMAGE_EXPORT_DIRECTORY null_iid;
			memset(&null_iid, 0, sizeof(null_iid));
			//每个元素代表了一个引入的DLL。
			ExportTableItemData data;

			DWORD dwNumOfExports = pImportTable->NumberOfNames;

			for (UINT i = 0; i < dwNumOfExports; i++)
			{
				char *szFunc = (PSTR)ImageRvaToVa((PIMAGE_NT_HEADERS)pNtHeaders, pDosHeader, (DWORD)*ppdwNames, 0);
				data.strName = S_CA2T(szFunc);
				InsertItem(data);
				ppdwNames++;
			}
			notifyBranchChanged(ITEM_ROOT);
		}
		else if (pNtHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 ||
			pNtHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
		{
			DWORD Rva_export_table = ((PIMAGE_NT_HEADERS64)pNtHeaders)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

			if (Rva_export_table == 0)
			{
				return -1;
			}

			PIMAGE_EXPORT_DIRECTORY pImportTable = (PIMAGE_EXPORT_DIRECTORY)ImageRvaToVa(
				(PIMAGE_NT_HEADERS)pNtHeaders,
				lpBaseAddress,
				Rva_export_table,
				NULL
			);
			DWORD **ppdwNames = (DWORD **)pImportTable->AddressOfNames;

			ppdwNames = (PDWORD*)ImageRvaToVa((PIMAGE_NT_HEADERS)pNtHeaders,
				pDosHeader, (DWORD)ppdwNames, 0);
			if (!ppdwNames)
			{
				return -1;
			}

			IMAGE_EXPORT_DIRECTORY null_iid;
			memset(&null_iid, 0, sizeof(null_iid));
			//每个元素代表了一个引入的DLL。
			ExportTableItemData data;

			DWORD dwNumOfExports = pImportTable->NumberOfNames;

			for (UINT i = 0; i < dwNumOfExports; i++)
			{
				char *szFunc = (PSTR)ImageRvaToVa((PIMAGE_NT_HEADERS)pNtHeaders, pDosHeader, (DWORD)*ppdwNames, 0);
				data.strName = S_CA2T(szFunc);
				InsertItem(data);
				ppdwNames++;
			}
			notifyBranchChanged(ITEM_ROOT);
		}
		return 0;
	}
	
	BOOL OnGroupPanleClick(EventArgs *pEvt)
	{
		SItemPanel *pItem = sobj_cast<SItemPanel>(pEvt->Sender());
		SToggle *pSwitch = pItem->FindChildByName2<SToggle>(L"tgl_switch");

		HSTREEITEM loc = (HSTREEITEM)pItem->GetItemIndex();
		ExpandItem(loc, TVC_TOGGLE);
		pSwitch->SetToggle(IsItemExpanded(loc));
		return true;
	}

	BOOL OnSwitchClick(EventArgs *pEvt)
	{
		SToggle *pToggle = sobj_cast<SToggle>(pEvt->Sender());
		SASSERT(pToggle);
		SItemPanel *pItem = sobj_cast<SItemPanel>(pToggle->GetRoot());
		SASSERT(pItem);
		HSTREEITEM loc = (HSTREEITEM)pItem->GetItemIndex();
		ExpandItem(loc, TVC_TOGGLE);
		return true;
	}
	
	virtual int WINAPI getViewType(HSTREEITEM hItem) const
	{
		return 0;
	}

	virtual int WINAPI getViewTypeCount() const
	{
		return 1;
	}

	BOOL OnDeleteItem(EventArgs* pEvt)
	{
		return true;
	}
};