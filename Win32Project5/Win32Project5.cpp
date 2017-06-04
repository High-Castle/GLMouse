// Win32Project5.cpp : Defines the entry point for the application.
//




#include "stdafx.h"
#include "Win32Project5.h"

#include <Windowsx.h>
#include <Shellapi.h>
#include <Windows.h>

#include <cassert>
#include <cmath>
#include <memory>

#include <iostream>
#include <utility>
#include <string>



#define TRY_TO( EXPR ) try_to( EXPR , __LINE__  , #EXPR )

namespace
{




	
	struct TrayMenu final
	{
		enum
		{
			POPUP = 0 ,
			ENABLE_CURSOR_ID = 666 ,
			EXIT_ID ,
			AUTOSTART_ID = 666
		};
		HMENU root ;
		HMENU settings ;
		bool inited ;
	} ;

	// 
	bool init_menu ( TrayMenu& menu, bool is_on_autostart ) noexcept
	{
		menu.root = CreatePopupMenu() ;
		if ( ! menu.root ) return false ;
		menu.settings = CreatePopupMenu() ;
		
		if( ! menu.settings ) {
			DestroyMenu( menu.root ) ;
			return false ;
		}
		menu.inited = true ;
		
		AppendMenu( menu.root , MF_STRING  , 
					TrayMenu::ENABLE_CURSOR_ID , L"Disable") ;


		AppendMenu( menu.root , MF_POPUP , ( UINT_PTR ) &menu.settings , L"Settings") ;
		AppendMenu( menu.settings , MF_POPUP | ( is_on_autostart ? MF_CHECKED : MF_UNCHECKED ),
			TrayMenu::AUTOSTART_ID , L"Enable Autostart" ) ;

		AppendMenu( menu.root , MF_STRING , TrayMenu::EXIT_ID , L"Exit" ) ;
		
		return true ;
	}
	
	bool update_menu_cursor_state( TrayMenu& menu , bool is_active )
	{
		return ModifyMenu( menu.root , TrayMenu::ENABLE_CURSOR_ID , 
			MF_BYCOMMAND | MF_STRING , TrayMenu::ENABLE_CURSOR_ID  , 
			( is_active ? L"Disable" : L"Enable" ) ) ;
	} 

	void destroy_menu ( TrayMenu& menu ) noexcept
	{
		if ( ! menu.inited ) return ;
		DestroyMenu( menu.root ) ;
		DestroyMenu( menu.settings ) ;
	}

	struct TrayObject final
	{
		enum { WM_TRAY_OBJECT = 15090 } ; // application-defined
		NOTIFYICONDATA data ;
	} ;

	enum { TRAYOBJ_SINGLE_ID = 3666 } ;

	bool init_tray_icon( TrayObject& obj, HWND wnd ,HINSTANCE inst ) noexcept
	{
		obj.data = NOTIFYICONDATA{} ;
		obj.data.cbSize = sizeof obj.data ;
		obj.data.hWnd = wnd ;
		obj.data.uID = TRAYOBJ_SINGLE_ID ;
		obj.data.uCallbackMessage = TrayObject::WM_TRAY_OBJECT ;
		obj.data.hIcon = LoadIcon( inst, MAKEINTRESOURCE(IDI_WIN32PROJECT5) ) ;
		obj.data.uFlags = NIF_MESSAGE | NIF_ICON /* .! */ ;
		return ( bool ) obj.data.hIcon ;
	}

	bool show_tray_icon(TrayObject& obj)  {
		return Shell_NotifyIcon(NIM_ADD,&obj.data) ;
	}

	bool hide_tray_icon(TrayObject& obj)  {
		return Shell_NotifyIcon(NIM_DELETE,&obj.data) ;
	}


	// autostart
	struct Autostart
	{
		bool is_enabled ;
	};

	void init_autostart( Autostart& astart )
	{

	}

	void enable_autostart_for_current_user( Autostart & )
	{
		// SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
	}

	void disable_autostart_for_current_user( Autostart & )
	{

	}

	bool autostart_state ( Autostart & )
	{
		return true ;
	}

	struct Hook final
	{
		HHOOK hook ;
	} ;

	bool set_low_level_mouse_hook( Hook & hook , HOOKPROC mouse_hook ) {
		hook.hook = SetWindowsHookEx( WH_MOUSE_LL , mouse_hook , nullptr , 0 ) ; // stackoverflow.com/questions/2060345/ 
		return ( bool ) hook.hook ;
	}
	
	bool unset_low_level_mouse_hook( Hook & hook ) {
		if ( ! hook.hook ) return false ;
		if ( UnhookWindowsHookEx( hook.hook ) ) {
			hook.hook = nullptr ;
			return true ;
		}
		return false ;
	}
}

namespace
{
	Hook hook ;
	TrayObject tray_obj ;
	TrayMenu tray_menu ;
	Autostart autostart ;
	CursorContext cursor_ctx ;
}

void quit() noexcept
{
	void system_quit() ;
	destroy_menu( tray_menu ) ;
	unset_low_level_mouse_hook( hook ) ;
	release_cursor( cursor_ctx ) ;
	destroy_cursor_context( cursor_ctx ) ;
	system_quit() ;
}

// Global Variables:
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                
WCHAR szTitle[ MAX_LOADSTRING ] ;              
WCHAR szWindowClass[ MAX_LOADSTRING ] ;                         


LRESULT CALLBACK mouse_hook ( int code , WPARAM wParam , LPARAM lParam )
{
	switch( wParam )
	{
		case WM_MOUSEMOVE :
			POINT pos ;
			GetCursorPos( &pos ) ;
			update_cursor( cursor_ctx , pos ) ;
			break ;
	}
	return CallNextHookEx( hook.hook , code , wParam , lParam ) ; // needed for LL.?
}

bool switch_cursor_state ( bool& state )
{
	if ( hook.hook ) {
		bool result = unset_low_level_mouse_hook( hook ) ;
		release_cursor( cursor_ctx ) ;
		state = ! result ;
		return result ;
	}
	state = set_low_level_mouse_hook( hook , mouse_hook ) ;
	corrupt_cursor( cursor_ctx ) ;
	return state ;
}



void try_to ( bool ok , long line , const char * str )
{
	enum { LARGEST_ERROR_LEN = 256 } ;
	char buff[ LARGEST_ERROR_LEN ] ;
	if ( ok ) return ;
	assert( false ) ;
	// function translate message
	//FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM , nullptr , GetLastError() , 
		//MAKELANGID( LANG_NEUTRAL , SUBLANG_DEFAULT ) , buff , LARGEST_ERROR_LEN , NULL );
	//std::cerr << "\n" << line << "  " << str << " : " << buff ; // todo : message window
	 SystemManager::Error::last_message() ;
	quit() ;
}


BOOL                InitInstance( HINSTANCE, int );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Perform application initialization:
    if ( ! InitInstance ( hInstance , nCmdShow ) )
    {
        return FALSE ;
    }

    HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT5));
	
    MSG msg;

    // Main message loop:

    while ( GetMessage( &msg , nullptr , 0 , 0 ) )
    {
        if ( ! TranslateAccelerator( msg.hwnd , hAccelTable , &msg ) )
        {
            TranslateMessage( &msg ) ;
            DispatchMessage( &msg ) ;
        }
    }

    return ( int ) msg.wParam;
}

namespace 
{
	HWND hWnd = nullptr ;
	WNDPROC prev_win_proc ;
}

BOOL InitInstance( HINSTANCE hInstance , int nCmdShow )
{
   hInst = hInstance ;

   hWnd = CreateWindowExW( 0 , L"Static", szTitle, 0 ,
						   0, 0, 0, 0, HWND_MESSAGE, 
						   nullptr , hInstance , nullptr ) ;

   if ( ! hWnd )
   {
	  assert( false ) ;
      return FALSE;
   }

   prev_win_proc = ( WNDPROC ) SetWindowLongPtr( hWnd , GWL_WNDPROC , 
												( LONG_PTR ) &WndProc ) ;

   TRY_TO(( set_low_level_mouse_hook( hook , mouse_hook ))) ;
   TRY_TO(( init_tray_icon( tray_obj , hWnd , hInstance ))) ;
   TRY_TO(( init_menu( tray_menu , autostart_state( autostart ) ) )) ;
   TRY_TO(( show_tray_icon( tray_obj ) )) ;
   TRY_TO(( init_cursor_context( cursor_ctx , hInst ) )) ;
   TRY_TO(( corrupt_cursor( cursor_ctx ) )) ;
   
   return TRUE ;
}

LRESULT CALLBACK WndProc( HWND hWnd , UINT message , WPARAM wParam , LPARAM lParam )
{
	switch( message )
	{
		case TrayObject::WM_TRAY_OBJECT:
			if( lParam == WM_RBUTTONUP )
			{
				
				POINT pos ;
				GetCursorPos( &pos ) ; // GET_X_LPARAM( wParam ) does not work for some reason ;
				
				TRY_TO(( SetForegroundWindow( hWnd ) )); // foreground window ( globally )
				
				UINT action = TrackPopupMenu(tray_menu.root,
					TPM_RETURNCMD | TPM_NONOTIFY, // Do not send notifications while open
					pos.x , pos.y , 0, hWnd, nullptr ) ;
		
				switch( action )
				{
					case TrayMenu::POPUP :

						break ;
					case TrayMenu::EXIT_ID:
						quit() ;
						break ;
					case TrayMenu::ENABLE_CURSOR_ID :
						bool new_state ;
						TRY_TO(( switch_cursor_state( new_state ) )) ;
						TRY_TO(( update_menu_cursor_state( tray_menu , new_state ) )) ;
						break ;
				}
			}
			break ; 
	}
	return CallWindowProc( prev_win_proc , hWnd , 
						   message , wParam , lParam ) ; // must.! be called
}

void system_quit() noexcept 
{
	if ( hWnd ) DestroyWindow( hWnd ) ;
	PostQuitMessage( 0 ) ;
}
