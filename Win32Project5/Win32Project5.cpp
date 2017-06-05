// Win32Project5.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Win32Project5.h"

#include <cassert>
#include <memory>

#include <utility>

#include <cstdio>
#include <cstring>
#include <cstdarg>

#include <Windows.h>
#include <Windowsx.h>
#include <lmcons.h>
#include <Shellapi.h>

#include "CError.hxx"
#include "CLogging.hxx"
#include "CCursor.hxx"


#include "ICursorProvider.hxx"
#include "CColoredProvider.hxx"

#include "CCurrentUserAutostart.hxx"
#include "CSingleInstanceMutex.hxx"

#include "CTrayMenu.hxx"
#include "CTrayObject.hxx"
#include "CMouse_LL_Hook.hxx"
#include "CMessageOnlyWindow.hxx"
#include "CApplication.hxx"

#define HC_TOOLS_CPP_FILE
#include "hc_action_macro.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

#define TRY_TO( EXPR ) try_to( EXPR , __FILE__ , __LINE__  , #EXPR )
void try_to ( sys::EErrorType err , const char * file_name , std::size_t line , const char * func_name ) ;
// TODO remove capplication 
LRESULT CALLBACK mouse_hook ( int code , WPARAM wParam , LPARAM lParam )
{
	switch( wParam )
	{
		case WM_MOUSEMOVE :
			POINT pos ;
			GetCursorPos( &pos ) ;
			cursor::CCursor::move_to_pos( pos ) ;
			break ;
	}
	return CallNextHookEx( sys::CMouse_LL_Hook::handle() , code , wParam , lParam ) ; // needed for LL.?
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPWSTR lpCmdLine, _In_ int )
{
	LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

	sys::CLogging::init( "out.log" , 40000 ) ; // todo interproc race case
    TRY_TO(( app::CApplication::init( hInstance , WndProc ) )) ;
	TRY_TO(( sys::CMouse_LL_Hook::set( mouse_hook ) )) ;

	HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_WIN32PROJECT5 ) );
	
    MSG msg ;

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

void quit() noexcept 
{
	if ( ! sys::CMouse_LL_Hook::is_set() )
		sys::CMouse_LL_Hook::unset() ;

	if ( app::CApplication::inited() )
		app::CApplication::free() ;

	sys::CLogging::free() ;
	PostQuitMessage( 0 ) ;
}

void switch_cursor_state () noexcept
{
	if ( sys::CMouse_LL_Hook::is_set() ) {
		TRY_TO(( sys::CMouse_LL_Hook::unset() )) ;
		TRY_TO(( cursor::CCursor::release() )) ; 
	}
	else { TRY_TO(( sys::CMouse_LL_Hook::set( mouse_hook ) )) ;
	TRY_TO(( cursor::CCursor::corrupt() )) ; }

	TRY_TO(( app::CTrayMenu::update_menu_cursor_state( sys::CMouse_LL_Hook::is_set() ) )) ;
}

void switch_autostart_state () noexcept
{
	TRY_TO(( sys::CCurrentUserAutostart::set( ! sys::CCurrentUserAutostart::state() ) )) ;
	TRY_TO(( app::CTrayMenu::update_menu_autostart_state( sys::CCurrentUserAutostart::state() ) )) ;
}

LRESULT CALLBACK WndProc( HWND hWnd , UINT message , WPARAM wParam , LPARAM lParam )
{
	switch( message )
	{
		case app::CTrayObject::WM_TRAY_OBJECT:
			if( lParam == WM_RBUTTONUP )
			{
				POINT pos ;
				GetCursorPos( &pos ) ; // GET_X_LPARAM( wParam ) does not work for some reason ;
				
				SetForegroundWindow( app::CMessageOnlyWindow::handle() ) ; // foreground window ( globally )
				
				UINT action = TrackPopupMenu( app::CTrayMenu::root() ,
					TPM_RETURNCMD | TPM_NONOTIFY, // Do not send notifications while open
					pos.x , pos.y , 0, hWnd, nullptr ) ;
		
				switch( action )
				{
					case app::CTrayMenu::EXIT_ID:
						quit() ;
						break ;
					case app::CTrayMenu::ENABLE_CURSOR_ID :
						switch_cursor_state() ;
						break ;
					case app::CTrayMenu::AUTOSTART_ID :
						switch_autostart_state() ;
						break ;
				}
			}
			break ; //*/
	}
	return CallWindowProc( app::CMessageOnlyWindow::prev_proc() , hWnd , // todo 
						   message , wParam , lParam ) ; // must.! be called
}

void try_to ( sys::EErrorType err , const char * file_name , std::size_t line , const char * func_name )
{ 
	if ( err == sys::NoError ) {		
		return ;
	}

	sys::CLogging::log( "\n[from %s:%d | %s()] %s | %s:%d ( %s ) | %s" , file_name , line , func_name , 
		sys::str_error_type( err ) , 
		sys::CError::file_name() ,
		sys::CError::line() ,
		sys::CError::func_name() ,
		sys::CError::error_msg() ) ;

	if ( err == sys::NotifyError ) {
		return ;
	} 

	if ( err == sys::CrashError ) {
		quit() ;
	}	
}
