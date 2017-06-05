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

#define HC_TOOLS_CPP_FILE
#include "hc_action_macro.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

#define TRY_TO( EXPR ) try_to( EXPR , __FILE__ , __LINE__  , #EXPR )

static const char * app_name  = "HCTrickyMouse" ;
static const char * app_token = "Mu1o90Tex_LnjIOE" ; 

namespace 
{
	void try_to ( sys::EErrorType err , const char * file_name , std::size_t line , const char * func_name ) noexcept ;
	void log_last_error () noexcept ;
}

namespace 
{
	sys::EErrorType app_init ( HINSTANCE instance , WNDPROC win_proc , HOOKPROC ms_hook ) noexcept
	{
		using namespace app ;
		sys::EErrorType err_at_now = sys::NoError ;

		HC_TOOLS_SCOPE 
		{
			HC_TOOLS_ACTION( { 
				err_at_now = sys::CLogging::init( "out.log" , 40000 ) ; // todo concurency
				return err_at_now == sys::NoError ;
			} , { log_last_error () ;
				  sys::CLogging::free() ; } 
			) ;
			
			HC_TOOLS_ACTION( {
				char user_name [ UNLEN + 1 ] ;
				char user_token[ UNLEN + sizeof app_token ] ;
				{ 
					DWORD token_sz = sizeof user_name ;
					GetUserNameA( user_name , &token_sz ) ;
					sprintf_s( user_token , sizeof user_token , 
							   "%s%s" , user_name , app_token ) ; 
				}
				err_at_now = sys::CSingleInstanceMutex::acquire( user_token ) ;
				return err_at_now == sys::NoError ;
			} , { sys::CSingleInstanceMutex::release() ; } ) ;

			HC_TOOLS_ACTION( {
				char exe_path [ MAX_PATH + 1 ] ;
				GetModuleFileNameA( NULL , exe_path , MAX_PATH + 1 ) ;
				err_at_now = sys::CCurrentUserAutostart::init( app_name , exe_path ) ;
				return err_at_now == sys::NoError ;
			} , { sys::CCurrentUserAutostart::free() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = app::CMessageOnlyWindow::init( instance , win_proc ) ; 
				return err_at_now == sys::NoError ;
			} , { app::CMessageOnlyWindow::free() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = cursor::CCursor::init( LoadCursor( nullptr , IDC_ARROW ) ,
				cursor::CColoredProvider::create( instance ) ) ;
				return err_at_now == sys::NoError ;
			} , { cursor::CCursor::free() ; } ) ;
			
			cursor::CCursor::corrupt() ;

			HC_TOOLS_ACTION( {
				err_at_now = sys::CMouse_LL_Hook::set( ms_hook ) ;
				return err_at_now == sys::NoError ;
			} , { sys::CMouse_LL_Hook::unset() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = CTrayObject::init( CMessageOnlyWindow::handle() , instance ) ;
				return err_at_now == sys::NoError ;
			} , { CTrayObject::free() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = CTrayMenu::init( sys::CCurrentUserAutostart::state() ) ;
				return err_at_now == sys::NoError ;
			} , { CTrayMenu::free() ; } ) ;
			
			HC_TOOLS_LEAVE_SCOPE
		}
		HC_TOOLS_END_SCOPE

		if ( err_at_now != sys::NoError )
			return sys::CrashError ;
	
		CTrayObject::show_tray_icon() ;

		return sys::NoError ;
	}

	static sys::EErrorType app_free () noexcept 
	{
		using namespace app ;
		CTrayMenu::free() ;
		CTrayObject::free() ;
		CMessageOnlyWindow::free() ;
		if ( sys::CMouse_LL_Hook::is_set() )
			sys::CMouse_LL_Hook::unset() ;
		cursor::CCursor::free() ;
		sys::CCurrentUserAutostart::free() ; 
		sys::CSingleInstanceMutex::release() ;
		sys::CLogging::free() ;
		return sys::NoError ;
	}
}

namespace 
{
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

	void quit ( int err ) noexcept 
	{
		app_free() ;
		PostQuitMessage( err ) ;
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
					quit( 0 ) ;
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

	void log_last_error () noexcept 
	{
		sys::CLogging::log( "\n[%s]\nfile : %s\nline : %d\nfunc : %s\nmessage : %s\n[end error]" ,
			sys::str_error_type( sys::CError::error_type() ) ,
			sys::CError::file_name() ,
			sys::CError::line() ,
			sys::CError::func_name() ,
			sys::CError::error_msg() ) ;
	}

	void try_to ( sys::EErrorType err , const char * file_name , std::size_t line , const char * func_name ) noexcept
	{ 
		if ( err == sys::NoError ) {		
			return ;
		}

		log_last_error () ;

		if ( err == sys::NotifyError ) {
			return ;
		} 

		if ( err == sys::CrashError ) {
			quit( - 1 ) ;
		}	
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPWSTR lpCmdLine, _In_ int )
{
	LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

	 // todo interproc race case
    if ( app_init( hInstance , WndProc , mouse_hook ) != sys::NoError )
		PostQuitMessage( - 1 ) ;
		
	HACCEL hAccelTable = LoadAccelerators( hInstance , MAKEINTRESOURCE( IDC_WIN32PROJECT5 ) );
	
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

