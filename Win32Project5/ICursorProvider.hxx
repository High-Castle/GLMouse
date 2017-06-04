#pragma once

#include <Windows.h>

namespace cursor 
{
	struct ICursorProvider
	{
		virtual HCURSOR next_cursor( POINT new_position ) = 0 ; // returns a copy (receiver becomes its owner)
		virtual HCURSOR default_cursor () = 0 ;
		virtual ~ ICursorProvider () = default ;
	} ;
}