
#pragma once

#include <memory>

#include <Windows.h>
#include "stdafx.h"

#include "CError.hxx"
#include "ICursorProvider.hxx"

namespace cursor
{
	struct CCursor final
	{
		// note that only after init is called any other operation can be performed
	    static sys::EErrorType init( HCURSOR cur , std::shared_ptr< ICursorProvider > provider ) noexcept ,
							   free() noexcept ;
		
		static sys::EErrorType set_cursor_provider ( std::unique_ptr< ICursorProvider > ) noexcept ;
		
		static ICursorProvider& get_cursor_provider () noexcept ;

		static sys::EErrorType corrupt () noexcept ;
		static sys::EErrorType release () noexcept ;
		static sys::EErrorType move_to_pos ( POINT pos ) noexcept ;

		static bool empty () noexcept ;
		static bool corrupted () noexcept ;

	private :
		struct CImpl { 
			std::shared_ptr< ICursorProvider > provider_ ;
			HCURSOR original_cursor_ ;
			bool is_corrupted_ ; 
		} ;
		static CImpl impl_ ;
	} ;
}
