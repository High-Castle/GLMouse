#include "stdafx.h"

#include <cassert>
#include <cstring>

#include "Windows.h"

#include "CError.hxx"
#include "CCurrentUserAutostart.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

#include "str_tools.hxx"

sys::CCurrentUserAutostart::CImpl sys::CCurrentUserAutostart::impl_ ;

namespace sys
{
	EErrorType CCurrentUserAutostart::init ( const char * app_name , const char * exe_path ) noexcept 
	{
		static constexpr char subkey_path [] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run" ;

		tools::set_str( impl_.app_name , app_name ) ;
		tools::set_str( impl_.exe_file_path , exe_path ) ;
		impl_.exe_file_path_sz = std::strlen( impl_.exe_file_path ) ;
		
		{ LONG res = RegOpenKeyExA( HKEY_CURRENT_USER , subkey_path , 0 , 
									KEY_WRITE | KEY_READ , &impl_.key ) ;

		  if ( res != ERROR_SUCCESS )  
			  return crash_error( str_win_error( res ) , FINFO ) ;  
		}
		
		{ LONG res = RegQueryValueExA( impl_.key , impl_.app_name , 0 ,
									   nullptr , nullptr , nullptr ) ;
		  if ( res == ERROR_FILE_NOT_FOUND ) { /* not on autostart */
				impl_.state = false ;
				return NoError ;
		  }
		  if ( res != ERROR_SUCCESS ) 
			  return crash_error( str_win_error( res ) , FINFO ) ;								
		}
		
		{ LONG res = RegSetValueExA( impl_.key , impl_.app_name , 0 , REG_SZ , /* null-terminated string */  
									( BYTE * ) impl_.exe_file_path , 
									impl_.exe_file_path_sz ) ;

		 if ( res != ERROR_SUCCESS ) 
			  return notify_error( str_win_error( res ) , BAD_ATTEMPT , FINFO ) ;
		  RegFlushKey( impl_.key ) ;
		  impl_.state = true ;										
		}
		return NoError ;
	}
	
	EErrorType CCurrentUserAutostart::free ( ) noexcept
	{
		// RegFlushKey( impl_.key ) ; 
		LONG res = RegCloseKey( impl_.key ) ; // not necessaraly immediate flush
		if ( res != ERROR_SUCCESS ) 
			return notify_error( str_win_error( res ) , BAD_ATTEMPT , FINFO ) ;
		return NoError ;
	}

	EErrorType CCurrentUserAutostart::set ( bool new_state ) noexcept 
	{
		if ( new_state == false ) 
		{
			LONG res = RegDeleteValueA( impl_.key , impl_.app_name ) ;
			if ( res != ERROR_SUCCESS ) 
				return notify_error( str_win_error( res ) , BAD_ATTEMPT , FINFO ) ;
			
			impl_.state = false ;
			return NoError ;
		}

		LONG res = RegSetValueExA( impl_.key , impl_.app_name , 0 , REG_SZ /* null-terminated string */ , 
								   ( BYTE * ) impl_.exe_file_path , 
									impl_.exe_file_path_sz ) ;
		if ( res != ERROR_SUCCESS ) 
			return notify_error( str_win_error( res ) , BAD_ATTEMPT , FINFO ) ;

		impl_.state = true ;
		return NoError ;
	}

	bool CCurrentUserAutostart::state ( ) noexcept { return impl_.state ; }
}