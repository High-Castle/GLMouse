#include "stdafx.h"
#include "Windows.h"

#include "CError.hxx"
#include "CSingleInstanceMutex.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

sys::CSingleInstanceMutex::CImpl sys::CSingleInstanceMutex::impl_ ;

namespace sys
{
	EErrorType CSingleInstanceMutex::acquire( const char * name ) noexcept
	{
		impl_.mutex = CreateMutexA( nullptr /**/ , true /*acquire created mutex*/ , name ) ;  

		if ( ! impl_.mutex ) 
			return crash_error( str_last_error() , FINFO ) ; 
		
		if ( GetLastError() == ERROR_ALREADY_EXISTS ) {
			CloseHandle( impl_.mutex ) ; 
			return crash_error( "There can be only one instance on the application" , FINFO ) ;
		}
		return NoError ;
	}

	void CSingleInstanceMutex::release() noexcept
	{
		CloseHandle( impl_.mutex ) ; // necessaraly for named objects
	}
}