#include "stdafx.h"
#include "Win32Project5.h"

#include <Windows.h>
#include "CError.hxx"

#include "CMessageOnlyWindow.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

app::CMessageOnlyWindow::CImpl app::CMessageOnlyWindow::impl_ ;

namespace app
{
	sys::EErrorType CMessageOnlyWindow::init ( HINSTANCE hInstance , WNDPROC WndProc ) noexcept
	{
		impl_.win = CreateWindowExA( 0 , "Static", "MO-Window" , 0 ,
									 0, 0, 0, 0, HWND_MESSAGE, nullptr , 
									 hInstance , nullptr ) ;
		if ( ! impl_.win )
			return sys::crash_error( sys::str_last_error() , FINFO ) ;

		impl_.prev_win_proc = ( WNDPROC ) SetWindowLongPtr( impl_.win , GWL_WNDPROC , ( LONG_PTR ) WndProc ) ;

		return sys::NoError ;
	}

	sys::EErrorType CMessageOnlyWindow::free () noexcept
	{
		DestroyWindow( impl_.win ) ;
		return sys::NoError ;
	}

	HWND CMessageOnlyWindow::handle () noexcept { return impl_.win ; }
	WNDPROC CMessageOnlyWindow::prev_proc () noexcept { return impl_.prev_win_proc ; } // todo : remove

}