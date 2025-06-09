/**
 * @file Inet.h
 * @author ae2f
 * @brief 
 * @date 2025-02-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ae2f_Inet_h
#define ae2f_Inet_h


#include <ae2f/Call.h>
#include <ae2f/Cast.h>

#include "Inet/Sock.h"

#if ae2f_IS_WIN == ae2f_IS_LINUX
#error This library will not work gracefully under this OS, which means it has no clue for this lib.
#endif

#pragma region Library Import
#if ae2f_IS_WIN
#include <winsock2.h>

/// @brief
/// Inetet API Data
#define ae2f_InetMkData WSADATA

#else

#include <arpa/inet.h>
#include <unistd.h>

/// @brief
/// Dummy Inetet API Data
#define ae2f_InetMkData int8_t

#endif
#pragma endregion

#pragma region Library Starter & Cleaner

/// @brief
/// Library startup code. \n
/// Notice it is for Windows, not Linux.
///
/// For releasing, see @ref ae2f_InetDel.
///
/// Linux needs no startup for Inetet api. It will always success.
#define ae2f_InetMk(...) 0

#if ae2f_IS_WIN

/// @brief
/// Library startup code. \n
/// Notice it is for Windows, not Linux.
///
/// For releasing, see @ref ae2f_InetDel.
/// 
/// # see WSAStartup on Microsoft documentation
/// @param[in] wVersionRequired {WORD}
/// @param[out] lpWSAData {LPWSADATA}
#define ae2f_InetMk(wVersionRequired, lpWSAData) WSAStartup(wVersionRequired, lpWSAData)

#endif

#if ae2f_IS_WIN

/// @brief
/// Library release function.
#define ae2f_InetDel WSACleanup

#else

/// @brief
/// Library release function.
#define ae2f_InetDel() 0

#endif

#pragma endregion


#if ae2f_IS_WIN
#define close closesocket
#endif

#endif