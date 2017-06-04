#pragma once

#include <memory>
#include <exception>

#include <Windows.h>

#include "stdafx.h"
#include "Win32Project5.h"
#include "CError.hxx"
#include "ICursorProvider.hxx"

namespace cursor 
{
	struct CColoredProvider final : ICursorProvider
	{
		CColoredProvider() ;
		HCURSOR default_cursor () override ;
		HCURSOR next_cursor( POINT new_position ) override ;
		private :
			HCURSOR table_ [ DIRECTION_TABLE_SZ ] ;
			POINT last_pos_ ;
	} ;
}
