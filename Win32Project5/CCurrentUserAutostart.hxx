#pragma once

#include "CError.hxx"



namespace sys
{
	struct CCurrentUserAutostart final
	{
		static EErrorType init ( const char * app_name , const char * exe_path ) noexcept ;
		static EErrorType free() noexcept ;
		static EErrorType set ( bool ) noexcept ;
		static bool state () noexcept ;
	private :
		struct CImpl final
		{
			enum { MAX_VALUE_LENGTH = 260 } ;
			HKEY key ; 
			bool state ;
			std::size_t exe_file_path_sz ;
			char exe_file_path [ MAX_VALUE_LENGTH + 1 ] ;
			char app_name [ MAX_VALUE_LENGTH + 1 ] ;
		} ;
		static CImpl impl_ ;
	} ;
}

