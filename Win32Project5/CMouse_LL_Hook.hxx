#pragma once

namespace sys
{
	struct CMouse_LL_Hook final
	{
		static sys::EErrorType set( HOOKPROC mouse_hook ) noexcept ; 

		static sys::EErrorType unset() noexcept ;

		static bool is_set() noexcept ;
		static HHOOK handle () noexcept ;

	private :
		struct CImpl final {
			HHOOK hook ;
		};
		static CImpl impl_ ;
	} ;


}