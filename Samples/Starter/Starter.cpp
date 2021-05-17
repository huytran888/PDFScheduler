/*********************************************************************

 Copyright (C) 2010 Foxit Corporation
 All rights reserved.

 NOTICE: Foxit permits you to use, modify, and distribute this file
 in accordance with the terms of the Foxit license agreement
 accompanying it. If you have received this file from a source other
 than Foxit, then your use, modification, or distribution of it
 requires the prior written permission of Foxit.

 ---------------------------------------------------------------------

Starter.cpp

 - Defines the entry point for the DLL application, the entry point is PlugInMain.

 - Skeleton .cpp file for a plug-in. It implements the basic
   handshaking methods required for all plug-ins.

*********************************************************************/


#include "stdafx.h"
#include "Starter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CStarterApp

BEGIN_MESSAGE_MAP(CStarterApp, CWinApp)
	//{{AFX_MSG_MAP(CStarterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStarterApp construction

CStarterApp::CStarterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CStarterApp object

CStarterApp theApp;

/** 
	Starter is a plug-in template that provides a minimal 
	implementation for a plug-in. Developers may use this plug-in a 
	basis for their plug-ins.
*/



/* PIExportHFTs
** ------------------------------------------------------
**/
/** 
** Create and Add the extension HFT's.
**
** @return true to continue loading plug-in,
** false to cause plug-in loading to stop.
*/
FS_BOOL PIExportHFTs(void)
{
	return TRUE;
}

/** 
The application calls this function to allow it to
<ul>
<li> Import HFTs supplied by other plug-ins.
<li> Replace functions in the HFTs you're using (where allowed).
<li> Register to receive notification events.
</ul>
*/
FS_BOOL PIImportReplaceAndRegister(void)
{
	return TRUE;
}


void GetPDFScheduleInfo(void * clientData) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const wchar_t* inputfile = L"";
	FR_Document frDocument;
	CFileDialog dlgFile(TRUE);
	if (dlgFile.DoModal() == IDOK) {
		CString filepath = dlgFile.GetPathName();
		inputfile = filepath.GetString();
		frDocument = FRDocOpenFromFile(inputfile, (FS_LPCSTR)L"", true, true); //shows how to load a PDF and display it.  Do not open the same PDF before this.
		FPD_Document fpdDocument = FRDocGetPDDoc(frDocument);
		int pageCount = FPDDocGetPageCount(fpdDocument);
		for (int i = 0; i != pageCount; ++i)
		{

			FPD_Object pDict = FPDDocGetPage(fpdDocument, i);
			FPD_Page pPage = FPDPageNew();
			FPDPageLoad(pPage, fpdDocument, pDict, TRUE);
			FPDPageParseContent(pPage, NULL);
			FPD_TextPage textPage = FPDTextPageNew(pPage, FPD_TEXT_DISPLAY_ORDER);
			FS_BOOL isTextPageParse = FPDTextPageParseTextPage(textPage);
			if (isTextPageParse) {
				int totalCharCountOnPage = FPDTextPageCountChars(textPage);
				FS_WideString currentPageTextWideString = FSWideStringNew();
				FPDTextPageGetPageText(textPage, 0, totalCharCountOnPage, &currentPageTextWideString);
				wchar_t const* currentPageTextwchart = FSWideStringCastToLPCWSTR(currentPageTextWideString);
				FRSysShowMessageBox(currentPageTextwchart, MB_OK | MB_ICONINFORMATION, NULL, NULL, FRAppGetMainFrameWnd());
			}

		}
	}
	else {
		FRSysShowMessageBox((FS_LPCWSTR)L"No file has been selected for the scan.", MB_OK | MB_ICONINFORMATION, NULL, NULL, FRAppGetMainFrameWnd());
	}

}
////////////////////////////////////////////////////////////////////
/* Plug-ins can use their Initialization procedures to hook into Foxit Reader's 
	 * user interface by adding menu items, toolbar buttons, windows, and so on.
	 * It is also acceptable to modify Foxit Reader's user interface later when the plug-in is running.
	 */

void PILoadRibbonUI(void* pParentWnd)
{

	FR_RibbonBar fr_Bar = FRAppGetRibbonBar(NULL); //The RibbonBar will free by Foixt Reader or Phantom.
	FR_RibbonCategory fr_Category = FRRibbonBarAddCategory(fr_Bar, "Calendar Scan", (FS_LPCWSTR)L"Calendar Scan");
	FR_RibbonPanel fr_Panel = FRRibbonCategoryAddPanel(fr_Category, "Calendar Scan Panel", (FS_LPCWSTR)L"Calendar Scan Panel", NULL);

	//Create a Ribbon button
	FS_INT32 nElementCount = FRRibbonPanelGetElementCount(fr_Panel);
	FR_RibbonButton fr_Button = (FR_RibbonButton)FRRibbonPanelAddElement(fr_Panel, FR_RIBBON_BUTTON,
		"Start Scan", (FS_LPCWSTR)L"Start Scan", nElementCount);

	FR_RibbonElement fr_ElementButton = FRRibbonPanelGetElementByName(fr_Panel, "Start Scan");
	FRRibbonElementSetTooltip(fr_ElementButton, (FS_LPCWSTR)L"Select PDF to scan for Calendar dates.");

	FRRibbonElementSetExecuteProc(fr_ElementButton, GetPDFScheduleInfo);

	//FRSysShowMessageBox((FS_LPCWSTR)L"Hello World", MB_OK | MB_ICONINFORMATION, NULL, NULL, FRAppGetMainFrameWnd());
		/*FR_Document doc = FRDocOpenFromFile((FS_LPCWSTR)L"C:\\Users\\Huy\\Desktop\\PDF\\Demo\\BioSampleTest.pdf",
			(FS_LPCSTR)L"",
			true,
			true
		);*/
}

void PILoadStatusBarUI(void* pParentWnd)
{
	//FRSysShowMessageBox((FS_LPCWSTR)L"PILoadStatusBarUI", MB_OK | MB_ICONINFORMATION, NULL, NULL, FRAppGetMainFrameWnd());
}

/* PIInit
** ------------------------------------------------------
**/
/** 
	The main initialization routine.
	
	@return true to continue loading the plug-in, 
	false to cause plug-in loading to stop.
*/
FS_BOOL PIInit(void)
{
	return TRUE;
}

/* PIUnload
** ------------------------------------------------------
**/
/** 
	The unload routine.
	Called when your plug-in is being unloaded when the application quits.
	Use this routine to release any system resources you may have
	allocated.

	Returning false will cause an alert to display that unloading failed.
	@return true to indicate the plug-in unloaded.
*/
FS_BOOL PIUnload(void)
{
	return TRUE;
}

/** PIHandshake
	function provides the initial interface between your plug-in and the application.
	This function provides the callback functions to the application that allow it to 
	register the plug-in with the application environment.

	Required Plug-in handshaking routine:
	
	@param handshakeVersion the version this plug-in works with. 
	@param handshakeData OUT the data structure used to provide the primary entry points for the plug-in. These
	entry points are used in registering the plug-in with the application and allowing the plug-in to register for 
	other plug-in services and offer its own.
	@return true to indicate success, false otherwise (the plug-in will not load).
*/
FS_BOOL PIHandshake(FS_INT32 handshakeVersion, void *handshakeData)
{
	if(handshakeVersion != HANDSHAKE_V0100)
		return FALSE;
	
	/* Cast handshakeData to the appropriate type */
	PIHandshakeData_V0100* pData = (PIHandshakeData_V0100*)handshakeData;
	
	/* Set the name we want to go by */
	pData->PIHDRegisterPlugin(pData, "Starter", (FS_LPCWSTR)L"Starter");

	/* If you export your own HFT, do so in here */
	pData->PIHDSetExportHFTsCallback(pData, &PIExportHFTs);
		
	/*
	** If you import plug-in HFTs, replace functionality, and/or want to register for notifications before
	** the user has a chance to do anything, do so in here.
	*/
	pData->PIHDSetImportReplaceAndRegisterCallback(pData, &PIImportReplaceAndRegister);

	/* Perform your plug-in's initialization in here */
	pData->PIHDSetInitDataCallback(pData, &PIInit);

	PIInitUIProcs initUIProcs;
	INIT_CALLBACK_STRUCT(&initUIProcs, sizeof(PIInitUIProcs));
	initUIProcs.lStructSize = sizeof(PIInitUIProcs);
	initUIProcs.PILoadRibbonUI = PILoadRibbonUI;
	initUIProcs.PILoadStatusBarUI = PILoadStatusBarUI;
	pData->PIHDSetInitUICallbacks(pData, &initUIProcs);

	/* Perform any memory freeing or state saving on "quit" in here */
	pData->PIHDSetUnloadCallback(pData, &PIUnload);

	return TRUE;
}

