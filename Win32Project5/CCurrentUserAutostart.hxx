#pragma once

#include "CError.hxx"

namespace sys
{
	struct CCurrentUserAutostart final
	{
		EErrorType init ( const char * app_name , const char * exe_path ) noexcept ;
		EErrorType set ( bool ) noexcept ;
		bool state () noexcept ;
	private :
		struct CImpl final
		{
			bool set_ ;
		} ;
		static CImpl impl_ ;
	} ;
}

