//*********************************************************************************\\
// 
// Generalist collection of debugging tools that are useful in most situations,
// console/VS or otherwise.
//
//*********************************************************************************\\

#pragma once

#include <windows.h>
#include <sstream>
#include <cassert>

//
//Asserts, Debugging & Error Handling
//

//Passes information out to output window via stringstream.
#define DBOUT( s )                          \
{                                           \
   std::ostringstream os_;                  \
   os_ << s << "\n";                        \
   OutputDebugString( os_.str().c_str() );  \
}
//Alternate method that uses wstringstream.
#define WDBOUT(s)                            \
{								             \
   std::wostringstream os_;		             \
   os_ << s << L"\n";				         \
   OutputDebugStringW( os_.str().c_str() );  \
}

//Assert wrapper to define a message alongside a test expressions for verbose testings.
#define msg_assert(expr, msg) assert(( (void)(msg), (expr) ))