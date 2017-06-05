#pragma once

#include <cstddef>

namespace sys
{
	using error_value_t = int ;

	enum EErrorType { 
		NoError , 
		CrashError , 
		NotifyError , 
	NOT_FOR_USE_EErrorType_END 
	} ;

	const char * str_error_type ( EErrorType ) noexcept ;

	enum EErrorValue : error_value_t
	{ 
		UNSET ,
		BAD_ATTEMPT ,
	} ;

	struct CError final
	{
		enum { 
			   MAX_ERROR_STR_LEN = 260 * 2 ,
			   MAX_FILE_NAME_LEN = 260 ,
			   MAX_FUNC_NAME_LEN = 128 ,	
			 }  ;
		
		static void set_error_msg ( const char * ) noexcept ;
		static void set_func_name ( const char * ) noexcept ;
		static void set_file_name ( const char * ) noexcept ;
		
		
		static const char * error_msg ( ) noexcept ;
		static const char * func_name ( ) noexcept ;
		static const char * file_name ( ) noexcept ;
		
		static void set_value ( error_value_t value ) noexcept ; 
		static error_value_t value () noexcept ;

		static void set_line ( std::size_t line ) noexcept ;
		static std::size_t line () noexcept ;

		static void set_message ( const char * error_msg ,
								  const char * func_name , 
								  const char * file_name ,
								  std::size_t line ) noexcept ;
		private :
			struct CLastError final
			{
				std::size_t line_ ;
				int	 value_ ;
				char error_msg_ [ MAX_ERROR_STR_LEN + 1 ] ;
				char file_name_ [ MAX_FILE_NAME_LEN + 1 ] ;
				char func_name_ [ MAX_FUNC_NAME_LEN + 1 ] ;
			} ;

			static thread_local CLastError last_error_ ; // inited (zeroed) during static initialization
	} ;

	EErrorType crash_error ( const char * , const char * func_name , 
							 const char * file_name , std::size_t line , error_value_t = UNSET ) noexcept ;

	EErrorType notify_error ( const char * msg , error_value_t val , 
							  const char * func_name , const char * file_name , std::size_t line ) noexcept ;
	
	const char * str_win_error ( LONG err ) noexcept ;
	const char * str_last_error () noexcept ; // TODO : replace to another header
}