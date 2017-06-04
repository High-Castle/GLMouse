#pragma once

namespace sys
{
	using error_value_t = int ;

	enum EErrorType { NoError , CrashError , NotifyError } ;

	enum EErrorValue : error_value_t
	{ 
		UNSET ,
		BAD_ATTEMPT ,
	} ;

	struct CError final
	{
		enum { MAX_ERROR_STR_LENGTH = 256 } ;
		
		static void set_error_msg ( const char * ) noexcept ;
		static void set_func_name ( const char * ) noexcept ;
		static void set_file_name ( const char * ) noexcept ;
		
		
		static const char * error_msg ( ) noexcept ;
		static const char * func_name ( ) noexcept ;
		static const char * file_name ( ) noexcept ;
		
		static void set_value ( error_value_t value ) noexcept ; 
		static error_value_t value () noexcept ;

		static void set_message ( const char * error_msg ,
								  const char * file_name , 
								  const char * func_name ) noexcept ;
		private :

		struct CLastError final
		{
			int	 value_ ;
			char error_msg_ [ MAX_ERROR_STR_LENGTH + 1 ] ;
			char file_name_ [ MAX_ERROR_STR_LENGTH + 1 ] ;
			char func_name_ [ MAX_ERROR_STR_LENGTH + 1 ] ;
		} ;

		static thread_local CLastError last_error_ ; // inited (zeroed) during static initialization
	} ;

	EErrorType crash_error ( const char * , const char * func_name , 
							 const char * file_name , error_value_t = UNSET ) noexcept ;

	EErrorType notify_error ( const char * msg , error_value_t val , 
							  const char * func_name , const char * file_name ) noexcept ;

	const char * get_last_error() noexcept ; // TODO : replace to another header
}