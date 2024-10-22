//*********************************************************************************\\
// 
// Collection of generally useful utilities across several different categories.
//
//*********************************************************************************\\

#pragma once

#include <random>
#include <time.h>
#include <assert.h>
#include <codecvt>          //For wstring conversion
#include <comdef.h>         //_com_error, ::Wide..
#include <string>
#include <functional>

//================================================================================\\
// Typedefs
//================================================================================\\



//================================================================================\\
// Function Pointers
//================================================================================\\

//Handy define macro related to the below to cut down on repetive code
#define cast_void_ptr(data) static_cast<void*>(data)

/*
    So as to define and leverage the features of std::function a bit clearer,
    this defines a "locked in at binding" type of function. For example,
    std::function<void> can take any function with a void return so long as the
    parameters are bound at creation.

    If the parameters need to change frequently, consider "DynamicFunc".
*/
template<class RET_TYPE>
using BindingFunc = std::function<RET_TYPE()>;

/*
    Like BindingFunc, this is a more defined use of std::function, but instead
    should be used for functions that take in frequently changing arguments
    over "bind and forget" arguments, whilst not strictly preventing it
    either.

    Symantically: DynamicFunc<ReturnType, Arg00, Arg01, etc...>
*/
template <class RET_TYPE, typename... Args>
using DynamicFunc = std::function<RET_TYPE(Args...)>;

/*
    Generic and versatile std::function definitions. Suitable for mixed function signature whilst
    being able to maintain the benefits of array storage. Does incur additional cost on execution
    due to indirection so may be less suitable if function pointer count is low.

    As a general guideline, here are some details regarding each type signature and use(s):

    Type A: Single void* param that can be used as both an input and output slot (or both with
    custom "cargo" containers).
    Type B: Double void* with I/O -> Slots 0/1 as a general guideline. Mix and match as required, if
    required.

    Definitions can be expanded as needed (i.e. having 3+ slots).
*/
using VoidFuncA = std::function<void(void*)>;
using VoidFuncB = std::function<void(void*, void*)>;

/*
    Support function for converting suitable functions into acceptable functions for use with VoidFuncA.
    Parameter can be input/output or both.

    - Function Signture: void Func(DataType ioData);
    - Binding Signature: VoidFuncA func = BindToVoidFuncA<MyClass, IODataType>(&MyClass::Func, &myClass)
    - Calling Function: func(static_cast<void*>(ioData));

    When binding signature, bind just the type, not pointers or references (you are binding the indirection,
    not the original function).
*/
template<typename CLASS_TYPE, typename IO_TYPE>
std::function<void(void*)> BindToVoidFuncA(void(CLASS_TYPE::* func)(IO_TYPE*), CLASS_TYPE* obj)
{
    //Create wrapping lambda function
    return [obj, func](void* ioData)
    {
        //Recast the data to appropriate type
        IO_TYPE* castIOData = static_cast<IO_TYPE*>(ioData);
        //Call member function
        (obj->*func)(castIOData);
    };
}
/*
    Support function for converting suitable functions into acceptable functions for use with VoidFuncB.
    Advised that parameters typically assume I/O -> slot 0/1 logic in setup, but it's acceptable to mix & match so long
    as you're tracking appropriately.

    - Function Signture: void Func(InboundType* in, OutboundType* out);
    - Binding Signature: VoidFuncB func = BindToVoidFuncB<MyClass, InboundType, OutboundType>(&MyClass::Func, &myClass)
    - Calling Function: func(static_cast<void*>(input), static_cast<void*>(output));

    When binding signature, bind just the type(s), not pointers or references (you are binding the indirection,
    not the original function).
*/
template<typename CLASS_TYPE, typename INPUT_TYPE, typename OUTPUT_TYPE>
std::function<void(void*, void*)> BindToVoidFuncB(void(CLASS_TYPE::* func)(INPUT_TYPE*, OUTPUT_TYPE*), CLASS_TYPE* obj)
{
    // Create and return a wrapping lambda function
    return [obj, func](void* input, void* output)
    {
        // Cast input and output pointers to the appropriate types
        INPUT_TYPE* castInput = static_cast<INPUT_TYPE*>(input);
        OUTPUT_TYPE* castOutput = static_cast<OUTPUT_TYPE*>(output);

        // Call the member function, passing the cast pointers (as the function expects pointers)
        (obj->*func)(castInput, castOutput);
    };
}

//================================================================================\\
// Constants
//================================================================================\\

//Helpful string compositions based on types (alphabetics, symbols etc)
static const std::string g_ALPHABETICS_LOWER = "abcdefghijklmnopqrstuvwxyz";
static const std::string g_ALPHABETICS_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const std::string g_SYMBOLS = "`¬!£$%^&*()_+-=_+[]{};'#:@~,./<>?|" + 34 + 92;	//32 = ", 92 = \


//================================================================================\\
// Resource Releasing
//================================================================================\\

//General call to free a given STL based container
template<class T>
inline void FreeSTLContainer(T& container)
{
	T empty;
	std::swap(container, empty);
}

//Releases COM Object resources
#define ReleaseCOM(x) \
{	if(x){	x->Release();	x = 0;	} }	



//================================================================================\\
// Conversions
//================================================================================\\

//WString to string converter, returning converted string
inline std::string WStringToString(const std::wstring& wstr)
{
    int wstrSize = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string newStr(wstrSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &newStr[0], wstrSize, NULL, NULL);
    return newStr;
}
//String to wstring converter, returning converted wstring
inline std::wstring StringtoWString(const std::string& str)
{
    int strSize = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring newWStr(strSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &newWStr[0], strSize);
    return newWStr;
}

/*
    Macros for some of the core recast types + some reminders on use cases:
    - Static - Converting related types, implicit to explicit casts, pointers to
        related classes and enum to integral type. Fastest with no error checks
        during runtime, only at compile time.
    - Dynamic - Safe conversion of ptrs/ref with inheritance tree and RTTI
        validity of downcasting (ideal for polymorphic classes that need RTTI).
        Slower due to run-time checks over compile time checks.
    - Const - Passing a const value to a non-const function
    - Reinterpret - Low-level, unsafe casts between unrelated types, ideally
        for manipulation of data at a binary level.
*/
#define recast_static(type, value) static_cast<type>(value)
#define recast_dynamic(type, value) dynamic_cast<type>(value)
#define recast_const(type, value) const_cast<type>(value)
#define recast_reint(type, value) reinterpret_cast<type>(value)



//================================================================================\\
// Manipulations
//================================================================================\\

//In the case where we need just the file name, and not the proceeding or preceeding information
inline void StripPathAndExtension(std::string& fileName, std::string* pPath = nullptr, std::string* pExt = nullptr)
{
    if (pPath)
        *pPath = "";
    if (pExt)
        *pExt = "";
    //filename only
    std::string::size_type n = fileName.find_last_of("\\/");
    if (n != std::string::npos)
    {
        if (pPath)
            pPath->append(fileName.c_str(), n + 1);
        fileName.erase(0, n + 1);
    }
    n = fileName.find_last_of(".");
    assert(n != std::string::npos);
    if (pExt)
        *pExt = fileName.substr(n);
    fileName.erase(n, fileName.length());
}

