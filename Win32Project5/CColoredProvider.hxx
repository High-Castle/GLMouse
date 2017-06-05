#pragma once

#include <memory>
#include <exception>

#include "stdafx.h"
#include "Win32Project5.h"

#include <memory>

#include <Windows.h>

#include "CError.hxx"
#include "ICursorProvider.hxx"

namespace cursor 
{
	struct CColoredProvider final : ICursorProvider
	{
		static std::unique_ptr< ICursorProvider > create ( HINSTANCE ) noexcept ;
		HCURSOR default_cursor () override ;
		HCURSOR next_cursor( POINT new_position ) override ;
		private :
			enum { DIRECTION_TABLE_SZ = 8 ,
				   DEFAULT_CURSOR_IDX = 4 } ;
			CColoredProvider( HINSTANCE ) ;
			HCURSOR table_ [ DIRECTION_TABLE_SZ ] ;
			POINT last_pos_ ;
	} ;
}
