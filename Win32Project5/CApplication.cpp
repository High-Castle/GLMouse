#pragma once

#include "stdafx.h"
#include "Win32Project5.h"

#include <cassert>
#include <memory>

#include <utility>

#include <cstdio>
#include <cstring>

#include <Windows.h>
#include <Windowsx.h>
#include <lmcons.h>

#include "CError.hxx"
#include "CLogging.hxx"

#include "CCursor.hxx"
#include "ICursorProvider.hxx"
#include "CColoredProvider.hxx"

#include "CCurrentUserAutostart.hxx"
#include "CSingleInstanceMutex.hxx"

#include "CTrayMenu.hxx"
#include "CTrayObject.hxx"
#include "CMouse_LL_Hook.hxx"
#include "CMessageOnlyWindow.hxx"

#include "CApplication.hxx"

#define HC_TOOLS_CPP_FILE
#include "hc_action_macro.hxx"

#define FINFO_CPP_FILE
#include "finfo_macro.hxx"

app::CApplication::CImpl app::CApplication::impl_ ;
namespace app
{
	sys::EErrorType CApplication::init ( HINSTANCE hInstance , WNDPROC win_proc ) noexcept
	{
		sys::EErrorType err_at_now = sys::NoError ;
		impl_.instance = hInstance ;

		HC_TOOLS_SCOPE 
		{
			HC_TOOLS_ACTION( {
				char user_name [ UNLEN + 1 ] ;
				char user_token[ UNLEN + sizeof impl_.app_token ] ;
				{ 
					DWORD token_sz = sizeof user_name ;
					GetUserNameA( user_name , &token_sz ) ;
					sprintf_s( user_token , sizeof user_token , 
						"%s%s" , user_name , impl_.app_token ) ; 
				}
				err_at_now = sys::CSingleInstanceMutex::acquire( user_token ) ;
				return err_at_now == sys::NoError ;
			} , { sys::CSingleInstanceMutex::release() ; } ) ;

			HC_TOOLS_ACTION( {
				char exe_path [ MAX_PATH + 1 ] ;
				GetModuleFileNameA( NULL , exe_path , MAX_PATH + 1 ) ;
				err_at_now = sys::CCurrentUserAutostart::init( impl_.app_name , exe_path ) ;
				return err_at_now == sys::NoError ;
			} , { sys::CCurrentUserAutostart::free() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = CMessageOnlyWindow::init( instance() , win_proc ) ; 
				return err_at_now == sys::NoError ;
			} , { CMessageOnlyWindow::free() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = cursor::CCursor::init( LoadCursor( nullptr , IDC_ARROW ) ,
				cursor::CColoredProvider::create( instance() ) ) ;
				return err_at_now == sys::NoError ;
			} , { cursor::CCursor::free() ; } ) ;
			cursor::CCursor::corrupt() ;

			HC_TOOLS_ACTION( { 
				err_at_now = CTrayObject::init( CMessageOnlyWindow::handle() , instance() ) ;
				return err_at_now == sys::NoError ;
			} , { CTrayObject::free() ; } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = CTrayObject::show_tray_icon() ;
				return err_at_now == sys::NoError ;
			} , { } ) ;

			HC_TOOLS_ACTION( { 
				err_at_now = CTrayMenu::init( sys::CCurrentUserAutostart::state() ) ;
				return err_at_now == sys::NoError ;
			} , { } ) ;

			HC_TOOLS_LEAVE_SCOPE
		}
		HC_TOOLS_END_SCOPE

			if ( err_at_now == sys::NoError )
			{
				impl_.inited = true ;
				return sys::NoError ;
			}
		return sys::CrashError ;
	}

	HINSTANCE CApplication::instance () noexcept {
		return impl_.instance ;
	}

	bool CApplication::inited () noexcept {
		return impl_.inited ;
	}

	sys::EErrorType CApplication::free () noexcept 
	{
		CTrayMenu::free() ;
		CTrayObject::free() ;
		cursor::CCursor::free() ;
		sys::CCurrentUserAutostart::free() ; 
		CMessageOnlyWindow::free() ;
		sys::CSingleInstanceMutex::release() ;
		return sys::NoError ;
	}
}



