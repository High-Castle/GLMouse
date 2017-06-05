// Win32Project5.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Win32Project5.h"

#include <cassert>
#include <memory>

#include <iostream>
#include <utility>

#include <cstdio>
#include <cstring>
#include <cstdarg>

#include <Windows.h>
#include <Windowsx.h>
#include <lmcons.h>
#include <Shellapi.h>

#include "CError.hxx"
#include "CCursor.hxx"

#include "ICursorProvider.hxx"
#include "CColoredProvider.hxx"

#include "CCurrentUserAutostart.hxx"
#include "CSingleInstanceMutex.hxx"

#define HC_TOOLS_CPP_FILE
#include "hc_action_macro.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

#define TRY_TO( EXPR ) try_to( EXPR , __FILE__ , __LINE__  , #EXPR )
void try_to ( sys::EErrorType err , const char * file_name , std::size_t line , const char * func_name ) ;

struct CLogging final
{
	static sys::EErrorType init ( const char * fname , std::size_t max_byte_count ) noexcept
	{
		impl_.log_file = fopen( fname , "w+" ) ;
		if ( ! impl_.log_file )
			return sys::crash_error( "failed to open log file" , FINFO ) ;
		impl_.byte_counter = 0 ;
		impl_.max_byte_count = max_byte_count ;
		return sys::NoError ;
	}

	static sys::EErrorType free () noexcept
	{
		fclose( impl_.log_file ) ;
		return sys::NoError ;
	}

	static sys::EErrorType log ( const char * fmt , ... ) noexcept
	{
		if ( impl_.byte_counter == impl_.max_byte_count ) 
		{
			std::rewind( impl_.log_file ) ;
			impl_.byte_counter = 0 ;
		}

		va_list args ;
		va_start( args , fmt ) ;
		int written = std::vfprintf( impl_.log_file , fmt , args ) ;
		va_end( args ) ;

		if ( written < 0 ) {
			sys::CError::set_value( sys::BAD_ATTEMPT ) ; 
			return sys::NotifyError ;
		} 

		std::fflush( impl_.log_file ) ;

		impl_.byte_counter += written ;
 
		return sys::NoError ;
	}
	private :
		struct CImpl final 
		{
			FILE * log_file ;
			std::size_t byte_counter ;
			std::size_t max_byte_count ;
		} ;
		static CImpl impl_ ;
} ;

struct CTrayMenu final 
{
	enum { ENABLE_CURSOR_ID = 666 ,
		   EXIT_ID ,
		   AUTOSTART_ID = 6666 } ;

	static sys::EErrorType init ( bool is_on_autostart ) noexcept
	{
		bool error_occured = true ;
		HC_TOOLS_SCOPE {
			HC_TOOLS_ACTION( { return impl_.root = CreatePopupMenu() ; } ,
								{ DestroyMenu( impl_.root ) ; } ) ;
				
			HC_TOOLS_ACTION( { return impl_.settings = CreatePopupMenu() ; } ,
							 { DestroyMenu( impl_.settings ) ; } ) ;

			HC_TOOLS_ACTION( {  
				bool res = AppendMenuA( impl_.root , MF_STRING , ENABLE_CURSOR_ID , "Disable" )
						&& AppendMenuA( impl_.root , MF_POPUP , ( UINT_PTR ) impl_.settings , "Settings" ) 
						&& AppendMenuA( impl_.settings , ( is_on_autostart ? MF_CHECKED : MF_UNCHECKED ) ,
											AUTOSTART_ID , "Enable Autostart" )
						&& AppendMenuA( impl_.root , MF_STRING , EXIT_ID , "Exit" ) ;
				return res ;
			} , {} ) ;

			error_occured = false ;
			HC_TOOLS_LEAVE_SCOPE
		} HC_TOOLS_END_SCOPE
			
		if ( error_occured )
			return sys::crash_error( sys::str_last_error() , FINFO ) ;

		return sys::NoError ;
	}

	static HMENU CTrayMenu::root() noexcept 
	{
		return impl_.root ;
	}

	static sys::EErrorType free () noexcept
	{
		DestroyMenu( impl_.root ) ;
		DestroyMenu( impl_.settings ) ;
		return sys::NoError ;
	}

	static sys::EErrorType update_menu_cursor_state( bool is_active ) noexcept
	{
		BOOL res = ModifyMenuA( impl_.root , ENABLE_CURSOR_ID , 
					MF_BYCOMMAND | MF_STRING , ENABLE_CURSOR_ID  , 
					( is_active ? "Disable" : "Enable" ) ) ;
			
		if ( ! res ) 
			return sys::crash_error( sys::str_last_error() , FINFO ) ;
			
		return sys::NoError ;
	} 

	static sys::EErrorType update_menu_autostart_state( bool new_state )
	{
		if ( -1 == CheckMenuItem( impl_.settings , AUTOSTART_ID , 
					MF_BYCOMMAND | ( new_state ? MF_CHECKED : MF_UNCHECKED ) ) ) 
		{
			return sys::notify_error( "no such menu item" , sys::BAD_ATTEMPT , FINFO ) ;
		}
		return sys::NoError ;
	}

	private :
		struct CImpl final {
			HMENU root ;
			HMENU settings ;
		} ;
		static CImpl impl_ ;
} ;
// 
	
struct CTrayObject final
{
	enum { WM_TRAY_OBJECT = 15090 } ; // application-defined
		
	enum { TRAYOBJ_SINGLE_ID = 3666 } ;

	static sys::EErrorType init( HWND wnd , HINSTANCE inst ) noexcept
	{
		impl_.data = NOTIFYICONDATA {} ;
		impl_.data.cbSize = sizeof impl_.data ;
		impl_.data.hWnd = wnd ;
		impl_.data.uID = TRAYOBJ_SINGLE_ID ;
		impl_.data.uCallbackMessage = CTrayObject::WM_TRAY_OBJECT ;
		impl_.data.uFlags = NIF_MESSAGE | NIF_ICON /* .! */ ;
		impl_.data.hIcon = LoadIcon( inst, MAKEINTRESOURCE( IDI_WIN32PROJECT5 ) ) ;
		if( ! impl_.data.hIcon ) 
			return sys::crash_error( sys::str_last_error() , FINFO ) ;
		return sys::NoError ; 
	}

	static sys::EErrorType free () noexcept
	{

		return sys::NoError ;
	}

	static sys::EErrorType show_tray_icon() noexcept {
			if ( ! Shell_NotifyIcon( NIM_ADD , &impl_.data ) ) 
				return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
			return sys::NoError ; 
	}

	static sys::EErrorType hide_tray_icon() noexcept {
		if ( ! Shell_NotifyIcon( NIM_DELETE, &impl_.data ) )
			return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
		return sys::NoError ; 
	}

	private :
		struct CImpl final {
			NOTIFYICONDATA data ;
		} ;
		static CImpl impl_ ;
} ;

struct CMouse_LL_Hook final
{
	static sys::EErrorType set( HOOKPROC mouse_hook ) noexcept {
		if ( impl_.hook ) 
			return sys::notify_error( "hook is already set" , sys::BAD_ATTEMPT , FINFO ) ;
		
		impl_.hook = SetWindowsHookEx( WH_MOUSE_LL , mouse_hook , nullptr , 0 ) ; // stackoverflow.com/questions/2060345/ 
		
		if ( impl_.hook ) return sys::NoError ;
		return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
	}

	static sys::EErrorType unset() noexcept {
		if ( ! impl_.hook ) 
			return sys::notify_error( "no hook to unset" , sys::BAD_ATTEMPT , FINFO ) ;
		
		if ( UnhookWindowsHookEx( impl_.hook ) ) {
			impl_.hook = nullptr ;
			return sys::NoError ;
		}
		assert ( false ) ;
		return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ; ;
	}

	static bool is_set() noexcept { return impl_.hook ; }
	static HHOOK handle () noexcept { return impl_.hook ; } 
		
	private :
		struct CImpl final {
			HHOOK hook ;
		};
		static CImpl impl_ ;
} ;

struct CMessageOnlyWindow final
{
	static sys::EErrorType init ( HINSTANCE hInstance , WNDPROC WndProc ) noexcept
	{
		impl_.win = CreateWindowExA( 0 , "Static", "MO-Window" , 0 ,
									 0, 0, 0, 0, HWND_MESSAGE, nullptr , 
									 hInstance , nullptr ) ;
		if ( ! impl_.win )
			return sys::crash_error( sys::str_last_error() , FINFO ) ;

		impl_.prev_win_proc = ( WNDPROC ) SetWindowLongPtr( impl_.win , GWL_WNDPROC , ( LONG_PTR ) WndProc ) ;

		return sys::NoError ;
	}

	static sys::EErrorType free () noexcept
	{
		DestroyWindow( impl_.win ) ;
		return sys::NoError ;
	}

	static HWND handle () noexcept { return impl_.win ; }
	static WNDPROC prev_proc () noexcept { return impl_.prev_win_proc ; } // todo : remove
	private :
		struct CImpl final
		{
			HWND win ;
			WNDPROC prev_win_proc ;
		};
		/* thread_local */ static CImpl impl_ ;
} ;

struct CApplication final
{
	static sys::EErrorType init ( HINSTANCE hInstance , WNDPROC win_proc ) noexcept
	{
		sys::EErrorType err_at_now = sys::NoError ;
		impl_.instance = hInstance ;

		HC_TOOLS_SCOPE 
		{
			HC_TOOLS_ACTION( {
				char user_name [ UNLEN + 1 ] ;
				char user_token[ UNLEN + sizeof impl_.app_token ] ;
				{ 
					DWORD token_sz = sizeof user_name ;
					GetUserNameA( user_name , &token_sz ) ;
					sprintf_s( user_token , sizeof user_token , 
							   "%s%s" , user_name , impl_.app_token ) ; 
				}
				err_at_now = sys::CSingleInstanceMutex::acquire( user_token ) ;
				return err_at_now == sys::NoError ;
			} , { sys::CSingleInstanceMutex::release() ; } ) ;
			
			HC_TOOLS_ACTION( {
				char exe_path [ MAX_PATH + 1 ] ;
				GetModuleFileNameA( NULL , exe_path , MAX_PATH + 1 ) ;
				err_at_now = sys::CCurrentUserAutostart::init( impl_.app_name , exe_path ) ;
				return err_at_now == sys::NoError ;
			} , { sys::CCurrentUserAutostart::free() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = CMessageOnlyWindow::init( instance() , win_proc ) ; 
				return err_at_now == sys::NoError ;
			} , { CMessageOnlyWindow::free() ; } ) ;
			
			HC_TOOLS_ACTION( { 
				err_at_now = cursor::CCursor::init( LoadCursor( nullptr , IDC_ARROW ) ,
													cursor::CColoredProvider::create( instance() ) ) ;
				return err_at_now == sys::NoError ;
			} , { cursor::CCursor::free() ; } ) ;
			cursor::CCursor::corrupt() ;
			
			HC_TOOLS_ACTION( { 
				err_at_now = CTrayObject::init( CMessageOnlyWindow::handle() , instance() ) ;
				return err_at_now == sys::NoError ;
			} , { CTrayObject::free() ; } ) ;
			
			HC_TOOLS_ACTION( { 
				err_at_now = CTrayObject::show_tray_icon() ;
				return err_at_now == sys::NoError ;
			} , { } ) ;
			
			HC_TOOLS_ACTION( { 
				assert( sys::CCurrentUserAutostart::state() ) ;
				err_at_now = CTrayMenu::init( sys::CCurrentUserAutostart::state() ) ;
				return err_at_now == sys::NoError ;
			} , { } ) ;

			HC_TOOLS_LEAVE_SCOPE
		}
		HC_TOOLS_END_SCOPE
		
		if ( err_at_now == sys::NoError )
		{
			impl_.inited = true ;
			return sys::NoError ;
		}
		return sys::CrashError ;
	}
	
	static HINSTANCE instance () noexcept {
		return impl_.instance ;
	}

	static bool inited () noexcept {
		return impl_.inited ;
	}

	static sys::EErrorType free () noexcept 
	{
		CTrayMenu::free() ;
		CTrayObject::free() ;
		cursor::CCursor::free() ;
		sys::CCurrentUserAutostart::free() ; 
		CMessageOnlyWindow::free() ;
		sys::CSingleInstanceMutex::release() ;
		return sys::NoError ;
	}

	private :
		struct CImpl final {
			enum { MAX_APP_NAME = 255 , MAX_TOKEN_LEN = 31 } ;
			HINSTANCE instance ;
			char app_name [ MAX_APP_NAME + 1 ] = "HCTrickyMouse" ;
			char app_token [ MAX_TOKEN_LEN + 1 ] = "Mu1o90Tex_LnjIOE" ;
			bool inited = false ;
		} ; 
		static CImpl impl_ ; // as if 
} ;

CTrayMenu::CImpl CTrayMenu::impl_ ;
CTrayObject::CImpl CTrayObject::impl_ ;
CMouse_LL_Hook::CImpl CMouse_LL_Hook::impl_ ;
CMessageOnlyWindow::CImpl CMessageOnlyWindow::impl_ ;
CApplication::CImpl CApplication::impl_ ;
CLogging::CImpl CLogging::impl_ ;

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
	return CallNextHookEx( CMouse_LL_Hook::handle() , code , wParam , lParam ) ; // needed for LL.?
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE , _In_ LPWSTR lpCmdLine, _In_ int )
{
	LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

	CLogging::init( "out.log" , 40000 ) ; // todo interproc race case
    TRY_TO(( CApplication::init( hInstance , WndProc ) )) ;
	TRY_TO(( CMouse_LL_Hook::set( mouse_hook ) )) ;

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
	if ( ! CMouse_LL_Hook::is_set() )
		CMouse_LL_Hook::unset() ;

	if ( CApplication::inited() )
		CApplication::free() ;

	CLogging::free() ;
	PostQuitMessage( 0 ) ;
}

void switch_cursor_state () noexcept
{
	if ( CMouse_LL_Hook::is_set() ) {
		TRY_TO(( CMouse_LL_Hook::unset() )) ;
		TRY_TO(( cursor::CCursor::release() )) ; 
	}
	else { TRY_TO(( CMouse_LL_Hook::set( mouse_hook ) )) ;
	TRY_TO(( cursor::CCursor::corrupt() )) ; }

	TRY_TO(( CTrayMenu::update_menu_cursor_state( CMouse_LL_Hook::is_set() ) )) ;
}

void switch_autostart_state () noexcept
{
	TRY_TO(( sys::CCurrentUserAutostart::set( ! sys::CCurrentUserAutostart::state() ) )) ;
	TRY_TO(( CTrayMenu::update_menu_autostart_state( sys::CCurrentUserAutostart::state() ) )) ;
}

LRESULT CALLBACK WndProc( HWND hWnd , UINT message , WPARAM wParam , LPARAM lParam )
{
	switch( message )
	{
		case CTrayObject::WM_TRAY_OBJECT:
			if( lParam == WM_RBUTTONUP )
			{
				POINT pos ;
				GetCursorPos( &pos ) ; // GET_X_LPARAM( wParam ) does not work for some reason ;
				
				SetForegroundWindow( CMessageOnlyWindow::handle() ) ; // foreground window ( globally )
				
				UINT action = TrackPopupMenu( CTrayMenu::root() ,
					TPM_RETURNCMD | TPM_NONOTIFY, // Do not send notifications while open
					pos.x , pos.y , 0, hWnd, nullptr ) ;
		
				switch( action )
				{
					case CTrayMenu::EXIT_ID:
						quit() ;
						break ;
					case CTrayMenu::ENABLE_CURSOR_ID :
						switch_cursor_state() ;
						break ;
					case CTrayMenu::AUTOSTART_ID :
						switch_autostart_state() ;
						break ;
				}
			}
			break ; //*/
	}
	return CallWindowProc( CMessageOnlyWindow::prev_proc() , hWnd , // todo 
						   message , wParam , lParam ) ; // must.! be called
}

void try_to ( sys::EErrorType err , const char * file_name , std::size_t line , const char * func_name )
{ 
	if ( err == sys::NoError ) {		
		return ;
	}

	CLogging::log( "\n[from %s:%d | %s()] %s | %s:%d ( %s ) | %s" , file_name , line , func_name , 
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