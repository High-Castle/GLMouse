#include "Windows.h"

#include "CError.hxx"
#include "CSingleInstanceMutex.hxx"

#define FINFO __FILE__ , __func__

namespace sys
{
	EErrorType CSingleInstanceMutex::acquire( const char * name ) noexcept
	{
		mutex_ = CreateMutexA( nullptr /*TODO (.?)*/ , true /*acquire created mutex*/ , name ) ;  

		if ( ! mutex_ ) 
			return crash_error( get_last_error() , FINFO ) ; ;
		
		if ( GetLastError() == ERROR_ALREADY_EXISTS ) 
			return crash_error( "There can be only one instance on the application" , FINFO ) ;
	
		return NoError ;
	}

	void CSingleInstanceMutex::release() noexcept
	{
		CloseHandle( mutex_ ) ;
	}
}