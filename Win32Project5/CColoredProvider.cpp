#pragma once

#include <memory>
#include <exception>

#include <Windows.h>

#include "stdafx.h"
#include "Win32Project5.h"

#include "ICursorProvider.hxx"
#include "CColoredProvider.hxx"

namespace cursor 
{
	enum { DIRECTION_TABLE_SZ = 8 ,
		   DEFAULT_CURSOR_IDX = 4 } ;
	
	namespace 
	{
		std::size_t calculate_cursor_num( POINT prev_pos , POINT next_pos ) noexcept ;
	}

	HCURSOR CColoredProvider::default_cursor () { 
		return CopyCursor( table_[ DEFAULT_CURSOR_IDX ] ) ; 
	}

	HCURSOR CColoredProvider::next_cursor( POINT new_position )  {
		auto cursor_no = calculate_cursor_num( last_pos_ , new_position ) ;
		last_pos_ = new_position ;
		return CopyCursor( table_[ cursor_no ] ) ;
	}

	CColoredProvider::CColoredProvider ()
	{
		std::size_t idx = 0 ;
		for ( auto each : { IDC_CURSOR1 , IDC_CURSOR2 , IDC_CURSOR3 ,
							IDC_CURSOR4 , IDC_CURSOR5 , IDC_CURSOR6 ,
							IDC_CURSOR7 , IDC_CURSOR8 } )
		{
			table_[ idx ] = LoadCursor( nullptr , MAKEINTRESOURCE( each ) ) ;
			if ( ! table_[ idx ] ) throw std::system_error{  EAGAIN } ;
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