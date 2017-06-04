#include <cstring>

#include <Windows.h>

#include "CError.hxx"

namespace sys 
{
	namespace 
	{
		void set_str( char * dst , const char * src ) noexcept
		{
			if ( ! src ) {
				std::strncpy( dst , "[Writing error string error : src is nullptr]" ,  CError::MAX_ERROR_STR_LENGTH + 1 ) ;
				return ;
			}
			std::strncpy( dst , src , CError::MAX_ERROR_STR_LENGTH + 1 ) ;
			dst[ CError::MAX_ERROR_STR_LENGTH ] = '\0' ;
		}
	}


	void CError::set_message ( const char * error_msg ,
							   const char * func_name , 
							   const char * file_name ) noexcept
	{
		set_file_name( file_name ) ;
		set_func_name( func_name ) ;
		set_error_msg( error_msg ) ;
	}

	void CError::set_error_msg ( const char * error_msg_str ) noexcept
	{
		set_str( last_error_.error_msg_ , error_msg_str ) ;
	}

	void CError::set_func_name ( const char * error_msg_str ) noexcept
	{
		set_str( last_error_.func_name_ , error_msg_str ) ;
	}
	
	void CError::set_file_name ( const char * error_msg_str ) noexcept
	{
		set_str( last_error_.file_name_ , error_msg_str ) ;
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

	void CError::set_value( error_value_t val ) noexcept
	{
		last_error_.value_ = val ;
	}

	error_value_t CError::value () noexcept
	{
		return last_error_.value_ ;
	}

	EErrorType crash_error( const char * msg , const char * func_name ,
							const char * file_name , error_value_t val ) noexcept
	{
		CError::set_value( val ) ;
		CError::set_message( msg , func_name , file_name ) ;
		return CrashError ;
	}

	EErrorType notify_error( const char * msg , error_value_t val ,
							 const char * func_name , const char * file_name ) noexcept
	{
		CError::set_value( val ) ;
		CError::set_message( msg , func_name , file_name ) ;
		return NotifyError ;
	}

	const char * get_last_error () noexcept
	{
		static char str [ CError::MAX_ERROR_STR_LENGTH + 1 ] ;
		str[ 0 ] = '\0' ;
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM , nullptr , GetLastError() , 
						MAKELANGID( LANG_NEUTRAL , SUBLANG_DEFAULT ) , 
						str , CError::MAX_ERROR_STR_LENGTH , NULL ) ;
		return str ;
	}
}