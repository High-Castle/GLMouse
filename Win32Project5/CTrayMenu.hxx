#pragma once

#include <Windows.h>

#include "CError.hxx"

namespace app
{
	struct CTrayMenu final 
	{
		enum { ENABLE_CURSOR_ID = 666 ,
			EXIT_ID ,
			AUTOSTART_ID = 6666 } ;

		static sys::EErrorType init ( bool is_on_autostart ) noexcept ;

		static HMENU CTrayMenu::root() noexcept ;

		static sys::EErrorType free () noexcept ;

		static sys::EErrorType update_menu_cursor_state( bool is_active ) noexcept ;

		static sys::EErrorType update_menu_autostart_state( bool new_state ) noexcept ;

	private :
		struct CImpl final {
			HMENU root ;
			HMENU settings ;
		} ;
		static CImpl impl_ ;
	} ;
}
