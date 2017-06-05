#pragma once

#include "CError.hxx"
#include "Windows.h"

namespace sys
{
	struct CSingleInstanceMutex final
	{
		static EErrorType acquire ( const char * name ) noexcept ;
		static void release () noexcept ;

	private :
		struct CImpl 
		{
			HANDLE mutex ;
		} ; 
		static CImpl impl_ ;
	} ;
}
