#pragma once
#include "stdafx.h"
#include "Win32Project5.h"

#include <memory>
#include <exception>
#include <stdexcept>

#include <cassert>

#include <Windows.h>

#include "ICursorProvider.hxx"
#include "CColoredProvider.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

namespace cursor 
{	
	namespace 
	{
		std::size_t calculate_cursor_num( POINT prev_pos , POINT next_pos ) noexcept ;
	}

	std::unique_ptr<ICursorProvider> CColoredProvider::create ( HINSTANCE instance ) noexcept try
	{
		return std::unique_ptr< ICursorProvider >( new CColoredProvider( instance ) ) ;
	} catch ( std::exception const& e ) {
		sys::notify_error( e.what() , sys::BAD_ATTEMPT , FINFO ) ;
		return nullptr ;
	}

	HCURSOR CColoredProvider::default_cursor () {
		return CopyCursor( table_[ DEFAULT_CURSOR_IDX ] ) ; 
	}

	HCURSOR CColoredProvider::next_cursor( POINT new_position ) {
		auto cursor_no = calculate_cursor_num( last_pos_ , new_position ) ;
		last_pos_ = new_position ;
		return CopyCursor( table_[ cursor_no ] ) ;
	}

	CColoredProvider::CColoredProvider ( HINSTANCE instance )
	{
		std::size_t idx = 0 ;

		for ( auto each : { IDC_CURSOR1 , IDC_CURSOR2 , IDC_CURSOR3 ,
							IDC_CURSOR4 , IDC_CURSOR5 , IDC_CURSOR6 ,
							IDC_CURSOR7 , IDC_CURSOR8 } )
		{
			table_[ idx ] = LoadCursor( instance , MAKEINTRESOURCE( each ) ) ;
			if ( ! table_[ idx ] ) { 
				throw std::runtime_error{ "CColoredProvider : error loading cursor" } ;
			}
			++ idx ;
		}
	}

	namespace 
	{
		std::size_t calculate_cursor_num( POINT prev_pos , POINT next_pos ) noexcept
		{
			const FLOAT cos_pi_d_6 = 0.96592582628f,
						cos_pi_d_3 = 0.5f ;
			LONG dx = next_pos.x - prev_pos.x ,
				 dy = next_pos.y - prev_pos.y ;
			float relation = std::abs( next_pos.x - prev_pos.x )
								/ std::sqrt( dx * dx + dy * dy ) ;

			if( dy < 0 )  {
				if( dx > 0 ) {
					if( relation > cos_pi_d_6 ) return 0 ; // I quad  
					if( relation > cos_pi_d_3 ) return 1 ;
					return 2 ;
				}
				if( relation < cos_pi_d_3 ) return 2 ; // II quad
				if( relation < cos_pi_d_6 ) return 3 ;
				return 4 ;
			}
			if( dx < 0 ) {
				if( relation > cos_pi_d_6 ) return 4 ;  // III quad
				if( relation > cos_pi_d_3 ) return 5 ;
				return 6 ;
			}
			if( relation < cos_pi_d_3 ) return 6 ; // IV quad
			if( relation < cos_pi_d_6 ) return 7 ;
			return 0 ;
		}
	}
}