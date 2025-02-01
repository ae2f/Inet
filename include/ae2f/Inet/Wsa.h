/**
 * @file Wsa.h
 * @author ae2f
 * @brief 
 * It is a port of WSA (Windows Sockets API)
 * @date 2025-02-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdint.h>
#include "../Inet.h"
#if ae2f_IS_WIN
#define ae2f_Inet_Wsa_h

#endif


#ifndef ae2f_Inet_Wsa_h
#define ae2f_Inet_Wsa_h

#define SOCKET          ae2f_InetSock
#define WSAAPI

#define closesocket close

/// @brief
/// Library startup code. \n
/// Notice it is for Windows, not Linux.
///
/// For releasing, see @ref ae2f_InetDel.
///
/// Linux needs no startup for Inetet api. It will always success.
#define WSAStartup ae2f_InetMk 

#define WSACleanup ae2f_InetDel

#define DWORD uint32_t
#define HANDLE void*

typedef struct _WSABUF {
  uint32_t len;
  char  *buf;
} WSABUF;

typedef struct WSAEVENT_imp *WSAEVENT;

typedef struct _WSAOVERLAPPED {
  DWORD    Internal;
  DWORD    InternalHigh;
  DWORD    Offset;
  DWORD    OffsetHigh;
  WSAEVENT hEvent;
} WSAOVERLAPPED, *LPWSAOVERLAPPED;

typedef void LPWSAOVERLAPPED_COMPLETION_ROUTINE(
  DWORD dwError,
  DWORD cbTransferred,
  LPWSAOVERLAPPED lpOverlapped,
  DWORD dwFlags
);

/// @brief 
/// @param[in] s 
/// @param[in, out] lpBuffers 
/// @param[in] dwBufferCount 
/// @param[out] lpNumberOfBytesRecvd 
/// @param[in, out] lpFlags 
/// @param[in] lpOverlapped 
/// @param[in] lpCompletionRoutine 
/// @return 
inline int WSAAPI WSARecv(
  SOCKET                             s,
  WSABUF*                           lpBuffers,
  DWORD                              dwBufferCount,
  DWORD*                            lpNumberOfBytesRecvd,
  DWORD*                            lpFlags,
  LPWSAOVERLAPPED                    lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) { return 0; }

/// @brief
/// @param[in] s 
/// @param[in] lpBuffers 
/// @param[in] dwBufferCount 
/// @param[out] lpNumberOfBytesSent 
/// @param[in] dwFlags 
/// @param[in] lpOverlapped 
/// @param[in] lpCompletionRoutine 
/// @return 
int WSAAPI WSASend(
  SOCKET                             s,
  WSABUF*                           lpBuffers,
  DWORD                              dwBufferCount,
  DWORD*                            lpNumberOfBytesSent,
  DWORD                              dwFlags,
  LPWSAOVERLAPPED                    lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) { return 0; }

#endif