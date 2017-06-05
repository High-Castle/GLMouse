#pragma once

#include <cassert>

#include <iostream>
#include <utility>

#include <cstdio>
#include <cstring>
#include <cstdarg>

#include "CError.hxx"
namespace sys 
{
	struct CLogging final
	{
		static sys::EErrorType init ( const char * fname , std::size_t max_byte_count ) noexcept ;

		static sys::EErrorType free () noexcept ;

		static sys::EErrorType log ( const char * fmt , ... ) noexcept ;
	private :
		struct CImpl final 
		{
			FILE * log_file ;
			std::size_t byte_counter ;
			std::size_t max_byte_count ;
		} ;
		static CImpl impl_ ;
	} ;
}