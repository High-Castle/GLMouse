#ifdef FINFO_CPP_FILE
	#ifndef FIFO_MACRO_HXX
		#define FIFO_MACRO_HXX

		#define FINFO __func__ , __FILE__ , __LINE__

	#else

		#error This header can be included only once and only in source file

	#endif
#endif