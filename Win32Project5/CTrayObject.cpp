#include "stdafx.h"
#include "Win32Project5.h"

#include <Windows.h>
#include <Windowsx.h>
#include <Shellapi.h>

#include "CError.hxx"

#include "CTrayObject.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

app::CTrayObject::CImpl app::CTrayObject::impl_ ;

namespace app
{
	sys::EErrorType CTrayObject::init( HWND wnd , HINSTANCE inst ) noexcept
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

	
	sys::EErrorType CTrayObject::free () noexcept
	{

		return sys::NoError ;
	}

	sys::EErrorType CTrayObject::show_tray_icon() noexcept {
		if ( ! Shell_NotifyIcon( NIM_ADD , &impl_.data ) ) 
			return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
		return sys::NoError ; 
	}

	sys::EErrorType CTrayObject::hide_tray_icon() noexcept {
		if ( ! Shell_NotifyIcon( NIM_DELETE, &impl_.data ) )
			return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
		return sys::NoError ; 
	}

}