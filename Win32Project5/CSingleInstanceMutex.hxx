#pragma once

#include "CError.hxx"
#include "Windows.h"

namespace sys
{
	struct CSingleInstanceMutex final
	{
		EErrorType acquire ( const char * ) noexcept ;
		void release () noexcept ;
	private :
		static HANDLE mutex_ ;
	} ;
}
