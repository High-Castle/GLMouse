#include "stdafx.h"
#include "Win32Project5.h"


#include <Windows.h>

#include "CError.hxx"

#include "CMouse_LL_Hook.hxx"


#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

sys::CMouse_LL_Hook::CImpl sys::CMouse_LL_Hook::impl_ ;

namespace sys 
{
	sys::EErrorType  CMouse_LL_Hook::set( HOOKPROC mouse_hook ) noexcept {
		if ( impl_.hook ) 
			return sys::notify_error( "hook is already set" , sys::BAD_ATTEMPT , FINFO ) ;

		impl_.hook = SetWindowsHookEx( WH_MOUSE_LL , mouse_hook , nullptr , 0 ) ; // stackoverflow.com/questions/2060345/ 

		if ( impl_.hook ) return sys::NoError ;
		return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
	}

	sys::EErrorType CMouse_LL_Hook::unset() noexcept {
		if ( ! impl_.hook ) 
			return sys::notify_error( "no hook to unset" , sys::BAD_ATTEMPT , FINFO ) ;

		if ( UnhookWindowsHookEx( impl_.hook ) ) {
			impl_.hook = nullptr ;
			return sys::NoError ;
		}
		return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ; ;
	}

	bool CMouse_LL_Hook::is_set() noexcept { return impl_.hook != nullptr ; }
	HHOOK CMouse_LL_Hook::handle () noexcept { return impl_.hook ; } 

}