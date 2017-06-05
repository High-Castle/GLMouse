#pragma once
#include "stdafx.h"
#include "Win32Project5.h"

#include <memory>

#include <exception>

#include <cassert>

#include <Windows.h>

#include "CError.hxx"
#include "ICursorProvider.hxx"
#include "CCursor.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

cursor::CCursor::CImpl cursor::CCursor::impl_ ;

namespace cursor
{
	enum { CURSOR_NORMAL = 32512 } ;

	namespace
	{
		bool set_cursor_copy( HCURSOR cur ) noexcept
		{
			HCURSOR cursor = CopyCursor( cur ) ;					// SetSystemCursor applies internally DestroyCursor.
			if ( ! cursor ) return false ;
			BOOL res = SetSystemCursor( cursor , CURSOR_NORMAL ) ;	// but DestroyCursor connot be used with shared resourses.
			DestroyCursor( cursor ) ;								// ( for example, obtained via LoadCursor ) 
			return res ;
		}

		bool set_cursor( HCURSOR cur ) noexcept {	
			return SetSystemCursor( cur , CURSOR_NORMAL ) ; ;
		}
	}

	

	sys::EErrorType CCursor::init( HCURSOR cur , std::shared_ptr< ICursorProvider > provider ) noexcept
	{
		if ( ! provider ) 
			return sys::crash_error( "nullptr passed as provider" , FINFO ) ; 
		
		impl_.provider_ = std::move( provider ) ;
		impl_.original_cursor_ = CopyCursor( cur ) ;
		
		if ( ! impl_.original_cursor_ )  
			return sys::crash_error( sys::str_last_error() , FINFO ) ;
		
		return sys::NoError ;
	}


	sys::EErrorType CCursor::free () noexcept {
		if ( empty() ) 
			return sys::notify_error( "attempt to free empty CCursor" , sys::BAD_ATTEMPT , FINFO ) ;
	
		if ( ! set_cursor( impl_.original_cursor_ ) ) 
			return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;

		impl_.provider_.reset() ;
		return sys::NoError ;
	}

	sys::EErrorType CCursor::set_cursor_provider ( std::unique_ptr< ICursorProvider > new_provider ) noexcept
	{
		if ( ! new_provider ) 
			return sys::crash_error( "nullptr provided as new cursor provider" , FINFO ) ;
	
		impl_.provider_ = std::move( new_provider ) ;
		return sys::NoError ;
	}

	ICursorProvider& CCursor::get_cursor_provider () noexcept {
		return * impl_.provider_ ;
	}

	sys::EErrorType CCursor::corrupt () noexcept {
		assert( ! corrupted() ) ;
		
		impl_.is_corrupted_ = set_cursor( impl_.provider_ -> default_cursor() ) ;
		
		if ( ! corrupted() ) 
			return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
		
		return sys::NoError ;
	}

	sys::EErrorType CCursor::release () noexcept {
		assert( corrupted() ) ;
		impl_.is_corrupted_ = ! set_cursor_copy( impl_.original_cursor_ ) ; 
		
		if ( corrupted() ) 
			return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
		
		return sys::NoError ;
	}

	sys::EErrorType CCursor::move_to_pos ( POINT pos ) noexcept {
		if ( ! set_cursor( impl_.provider_ -> next_cursor( pos ) ) ) 
			return sys::notify_error( sys::str_last_error() , sys::BAD_ATTEMPT , FINFO ) ;
		return sys::NoError ; 
	}

	bool CCursor::empty () noexcept { return ! impl_.provider_ ; }
	bool CCursor::corrupted () noexcept { return impl_.is_corrupted_ ; }
}
