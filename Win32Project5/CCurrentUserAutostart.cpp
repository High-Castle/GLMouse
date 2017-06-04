#include "Windows.h"

#include "CError.hxx"
#include "CCurrentUserAutostart.hxx"

namespace sys
{
	EErrorType CCurrentUserAutostart::init ( const char * app_name , const char * exe_path ) noexcept 
	{
		HKEY key ;

		RegCreateKeyExA( HKEY_CURRENT_USER ,   , , , , , &key , ) ;

		return NoError ;
	}

	EErrorType CCurrentUserAutostart::set ( bool new_state ) noexcept 
	{

		return NoError ;
	}

	bool CCurrentUserAutostart::state ( ) noexcept 
	{

		return NoError ;
	}

}