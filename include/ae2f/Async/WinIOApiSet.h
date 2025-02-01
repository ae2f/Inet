#include <ae2f/Call.h>

#if ae2f_IS_WIN
#define ae2f_Async_WinIOApiSet_h
#endif

#ifndef ae2f_Async_WinIOApiSet_h
#define ae2f_Async_WinIOApiSet_h

#define _In_
#define _In_opt_
#define WINAPI

#include <stdint.h>
#include <ae2f/Cast.h>

#define HANDLE void*
#define DWORD uint32_t

#define INVALID_HANDLE_VALUE ae2f_reinterpret_cast(void*, -1)

/// @brief 
/// @param[in] FileHandle 
/// @param[in] ExistingCompletionPort 
/// @param[in] CompletionKey 
/// @param[in] NumberOfConcurrentThreads 
/// @return  
HANDLE WINAPI CreateIoCompletionPort(
  _In_     HANDLE    FileHandle,
  _In_opt_ HANDLE    ExistingCompletionPort,
  _In_     uint32_t* CompletionKey,
  _In_     DWORD     NumberOfConcurrentThreads
) {}

#endif