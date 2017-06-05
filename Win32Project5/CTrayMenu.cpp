#include "stdafx.h"
#include "Win32Project5.h"

#include <Windows.h>
#include <Windowsx.h>
#include <Shellapi.h>

#include "CError.hxx"

#include "CTrayMenu.hxx"

#define HC_TOOLS_CPP_FILE
#include "hc_action_macro.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

app::CTrayMenu::CImpl app::CTrayMenu::impl_ ;

namespace app
{
	sys::EErrorType CTrayMenu::init ( bool is_on_autostart ) noexcept
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

	HMENU CTrayMenu::root() noexcept 
	{
		return impl_.root ;
	}

	sys::EErrorType CTrayMenu::free () noexcept
	{
		DestroyMenu( impl_.root ) ;
		DestroyMenu( impl_.settings ) ;
		return sys::NoError ;
	}

	sys::EErrorType CTrayMenu::update_menu_cursor_state( bool is_active ) noexcept
	{
		BOOL res = ModifyMenuA( impl_.root , ENABLE_CURSOR_ID , 
								MF_BYCOMMAND | MF_STRING , ENABLE_CURSOR_ID  , 
			( is_active ? "Disable" : "Enable" ) ) ;

		if ( ! res ) 
			return sys::crash_error( sys::str_last_error() , FINFO ) ;

		return sys::NoError ;
	} 

	sys::EErrorType CTrayMenu::update_menu_autostart_state( bool new_state ) noexcept
	{
		if ( -1 == CheckMenuItem( impl_.settings , AUTOSTART_ID , 
									MF_BYCOMMAND | ( new_state ? MF_CHECKED : MF_UNCHECKED ) ) ) 
		{
			return sys::notify_error( "no such menu item" , sys::BAD_ATTEMPT , FINFO ) ;
		}
		return sys::NoError ;
	}
}