#pragma once

#include <Windows.h>
#include <Shellapi.h>

#include "CError.hxx"

namespace app 
{
	struct CTrayObject final
	{
		enum { WM_TRAY_OBJECT = 15090 } ; // application-defined
		enum { TRAYOBJ_SINGLE_ID = 3666 } ;

		static sys::EErrorType init( HWND wnd , HINSTANCE inst ) noexcept ;

		static sys::EErrorType free () noexcept ; 
		static sys::EErrorType show_tray_icon() noexcept ;

		static sys::EErrorType hide_tray_icon() noexcept ;

	private :
		struct CImpl final {
			NOTIFYICONDATA data ;
		} ;
		static CImpl impl_ ;
	} ;
}