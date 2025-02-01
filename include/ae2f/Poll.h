/**
 * @file Poll.h
 * @author ae2f
 * @brief 
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <ae2f/Call.h>
#include <ae2f/Cast.h>

#ifndef ae2f_Async_h
#define ae2f_Async_h

#if ae2f_IS_WIN == ae2f_IS_LINUX
#error This library will not work gracefully under this OS, which means it has no clue for this lib.
#endif

#if ae2f_IS_WIN
#else
#include <sys/epoll.h>
#endif



#endif