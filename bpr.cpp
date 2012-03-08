//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Forms\ObectsLibrary.cpp", frmObjLib);
USEFORM("Forms\main.cpp", frmMain);
USEFORM("Forms\Scope.cpp", frmScope);
USEFORM("Components\Forms\frmProp.cpp", frmProperty);
USEFORM("Components\Forms\EditGrid.cpp", frmEditGrid);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
   	TfrmMain* frmMain;
		Application->Initialize();
		Application->Title = "Power System";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmProperty), &frmProperty);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
