#include "stdafx.h"
#include "Win32Project5.h"

#include <cassert>

#include <cstdio>
#include <cstdarg>

#include "CError.hxx"

#include "CLogging.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"



sys::CLogging::CImpl sys::CLogging::impl_ ;

namespace sys
{
	sys::EErrorType CLogging::init ( const char * fname , std::size_t max_byte_count ) noexcept
	{
		impl_.log_file = fopen( fname , "w+" ) ;
		if ( ! impl_.log_file )
			return sys::crash_error( "failed to open log file" , FINFO ) ;
		impl_.byte_counter = 0 ;
		impl_.max_byte_count = max_byte_count ;
		return sys::NoError ;
	}

	sys::EErrorType CLogging::free () noexcept
	{
		fclose( impl_.log_file ) ;
		return sys::NoError ;
	}

	sys::EErrorType CLogging::log ( const char * fmt , ... ) noexcept
	{
		if ( impl_.byte_counter == impl_.max_byte_count ) 
		{
			std::rewind( impl_.log_file ) ;
			impl_.byte_counter = 0 ;
		}

		va_list args ;
		va_start( args , fmt ) ;
		int written = std::vfprintf( impl_.log_file , fmt , args ) ;
		va_end( args ) ;

		if ( written < 0 ) {
			sys::CError::set_value( sys::BAD_ATTEMPT ) ; 
			return sys::NotifyError ;
		} 

		std::fflush( impl_.log_file ) ;

		impl_.byte_counter += written ;

		return sys::NoError ;
	}
} 