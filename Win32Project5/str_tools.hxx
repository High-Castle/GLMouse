#pragma once

#include <cstring>
#include <cstddef>

namespace tools {
	template < std::size_t SZ >
	bool set_str( char ( &dst )[ SZ ] , const char * src ) noexcept
	{
		if ( ! src ) {
			std::strncpy( dst , "[CError - error : src is nullptr]" ,  SZ ) ;
			return false ;
		}

		if ( dst <= src && src < dst + SZ ) {
			std::strncpy( dst , "[CError - error : overlapping dst and src ranges]" ,  SZ ) ;
			assert( ! "tools::set_str : OVERLAPPING.!" ) ;
			return false ;
		}

		std::strncpy( dst , src , SZ ) ;
		dst[ SZ - 1 ] = '\0' ;
		return true ;
	}
}
