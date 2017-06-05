#pragma once

#include <Windows.h>

#include "CError.hxx"


namespace app
{
	struct CApplication final
	{
		static sys::EErrorType init ( HINSTANCE hInstance , WNDPROC win_proc ) noexcept ;

		static HINSTANCE instance () noexcept ;

		static bool inited () noexcept ;

		static sys::EErrorType free () noexcept ;

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
}
