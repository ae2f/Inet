/**
 * @file Sock.h
 * @author ae2f
 * @brief 
 * @date 2025-02-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#if !ae2f_IS_WIN

#define ae2f_InetSock   int

/// @brief The socket was invalid
#define INVALID_SOCKET -1

#else
#define ae2f_InetSock SOCKET
#endif

#define ae2f_Socket     ae2f_InetSock