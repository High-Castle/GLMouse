#ifdef HC_TOOLS_CPP_FILE
#ifndef HC_TOOLS_ACTION_MACRO_HXX
#define HC_TOOLS_ACTION_MACRO_HXX
#include <utility>

namespace tools
{
	template < unsigned long LineN , class F , class AF >
	struct action_commit_or_rollback final
	{    
		template < class F_ , class AF_ >
		action_commit_or_rollback( bool& common_perform , F_&& func , AF_&& afunc ) :
			afunc_( std::forward< AF_ >( afunc ) ),
			common_perform_( common_perform )
		{
			result_ = func() ;
		}

		~ action_commit_or_rollback () 
		{
			if ( perform_exec_ && common_perform_ )
				afunc_() ; // noexcept
		}
		bool result ( ) const noexcept { return result_ ; }
		void discard () noexcept { perform_exec_ = false ; }
	private :
		AF afunc_ ;
		bool & common_perform_ ;
		bool result_ , perform_exec_ = true ;
	} ;

	template < unsigned long LineN , class ... FArg > 
	action_commit_or_rollback< LineN , FArg... > make_action_helper ( bool& cp , FArg&&... f )
	{
		return action_commit_or_rollback< LineN , FArg... >{ cp , std::forward< FArg >( f ) ...  } ;
	}
}

//*/

#define HC_TOOLS_ASSEMBLE_EXP( NAME0 , NAME1 ) NAME0 ## NAME1
#define HC_TOOLS_ASSEMBLE( NAME0 , NAME1 ) HC_TOOLS_ASSEMBLE_EXP( NAME0 , NAME1 )
#define HC_TOOLS_LINE_NAME HC_TOOLS_ASSEMBLE( obj_ , __LINE__ )

#define HC_TOOLS_ACTION( FUNC , AFUNC ) \
    auto HC_TOOLS_LINE_NAME = tools::make_action_helper<__LINE__>( HC_TOOLS_common_perform_26DO_1996NOT_1USE_1THIS_0ID_ , [ & ] () -> bool FUNC , [ & ] () AFUNC ) ; if ( ! HC_TOOLS_LINE_NAME.result() ) { HC_TOOLS_LINE_NAME.discard() ; return ; }

#define HC_TOOLS_SCOPE { bool HC_TOOLS_common_perform_26DO_1996NOT_1USE_1THIS_0ID_ = true ; [ & ] () 

#define HC_TOOLS_END_SCOPE () ; } 

#define HC_TOOLS_LEAVE_SCOPE HC_TOOLS_common_perform_26DO_1996NOT_1USE_1THIS_0ID_ = false ;

#else

#error This header can be included only once and only in source file

#endif
#endif