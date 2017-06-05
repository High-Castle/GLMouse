#include "stdafx.h"

#include <cstring>
#include <cstddef>
#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <Windows.h>

#include "CError.hxx"

#include "str_tools.hxx"

thread_local sys::CError::CLastError sys::CError::last_error_ ;

namespace sys 
{
	void CError::set_message ( EErrorType type ,
							   const char * error_msg ,
							   const char * func_name , 
							   const char * file_name ,
							   std::size_t line  ) noexcept
	{
		set_error_type( type ) ;
		set_line( line ) ;
		set_file_name( file_name ) ;
		set_func_name( func_name ) ;
		set_error_msg( error_msg ) ;
	}

	void CError::set_error_msg ( const char * error_msg_str ) noexcept
	{
		tools::set_str( last_error_.error_msg_ , error_msg_str ) ;
	}

	void CError::set_func_name ( const char * error_msg_str ) noexcept
	{
		tools::set_str( last_error_.func_name_ , error_msg_str ) ;
	}
	
	void CError::set_file_name ( const char * error_msg_str ) noexcept
	{
		tools::set_str( last_error_.file_name_ , error_msg_str  ) ;
	}

	void CError::set_error_type( EErrorType type ) noexcept
	{
		last_error_.type_ = type ;
	}

	const char * CError::error_msg ( ) noexcept
	{
		return last_error_.error_msg_ ;
	}

	const char * CError::func_name ( ) noexcept
	{
		return last_error_.func_name_ ;
	}

	const char * CError::file_name ( ) noexcept
	{
		return last_error_.file_name_ ;
	}

	EErrorType CError::error_type() noexcept
	{
		return last_error_.type_ ;
	}

	void CError::set_value( error_value_t val ) noexcept
	{
		last_error_.value_ = val ;
	}

	error_value_t CError::value () noexcept
	{
		return last_error_.value_ ;
	}

	void CError::set_line( std::size_t line ) noexcept
	{
		last_error_.line_ = line ; 
	}

	std::size_t CError::line() noexcept
	{
		return last_error_.line_ ;
	}

	const char * str_error_type( EErrorType err ) noexcept {
		static const char * table [] = { "NoError" , "CrashError" , "NotifyError" } ;
		static_assert( sizeof( table ) / sizeof * table == NOT_FOR_USE_EErrorType_END ,
					   "Probably you missed something from EErrorType" ) ;
		
		if ( err >= NOT_FOR_USE_EErrorType_END ) 
			return "UnknownError" ;

		return table[ err ] ;
	}

	EErrorType crash_error( const char * msg , const char * func_name ,
							const char * file_name , std::size_t line , error_value_t val ) noexcept
	{
		CError::set_value( val ) ;
		CError::set_message( CrashError , msg , func_name , file_name , line ) ;
		return CrashError ;
	}

	EErrorType notify_error( const char * msg , error_value_t val ,
							 const char * func_name , const char * file_name , std::size_t line ) noexcept
	{
		CError::set_value( val ) ;
		CError::set_message( NotifyError , msg , func_name , file_name , line ) ;
		return NotifyError ;
	}

	const char * str_win_error ( LONG err ) noexcept 
	{
		thread_local char str [ CError::MAX_ERROR_STR_LEN + 1 ] ;
		str[ 0 ] = '\0' ;
		DWORD ret = FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM , nullptr , err , 
									MAKELANGID( LANG_NEUTRAL , SUBLANG_DEFAULT ) , 
									str , CError::MAX_ERROR_STR_LEN + 1 , NULL ) ;
		if ( ! ret ) {
			std::strncpy( str , "[str_win_error - error : FormatMessageA failed]" ,  sizeof str ) ;
		}
		return str ;
	}

	const char * str_last_error () noexcept
	{
		return str_win_error( GetLastError() ) ;
	}
	// */
}