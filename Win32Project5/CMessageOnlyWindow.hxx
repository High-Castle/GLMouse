#pragma once

#include <Windows.h>
#include "CError.hxx"

namespace app
{
	struct CMessageOnlyWindow final
	{
		static sys::EErrorType init ( HINSTANCE hInstance , WNDPROC WndProc ) noexcept ;

		static sys::EErrorType free () noexcept ;

		static HWND handle () noexcept ;
		static WNDPROC prev_proc () noexcept ;
	private :
		struct CImpl final
		{
			HWND win ;
			WNDPROC prev_win_proc ;
		};
		/* thread_local */ static CImpl impl_ ;
	} ;
}
