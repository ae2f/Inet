#include "./Inet.h"

#ifdef _WIN32

#define ae2f_InetNBlock(sock, res)  \
{ \
    u_long mode = 1; \
    if (ioctlsocket(sock, FIONBIO, &mode) != 0) { \
        *(res) = -1; \
    } \
    *(res) = 0; \
}

#define errno WSAGetLastError()
#define EWOULDBLOCK   WSAEWOULDBLOCK
#define EAGAIN        WSAEWOULDBLOCK

#else

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define ae2f_InetNBlock(fd, res)                                                      \
  if (res) {                                                                   \
    int v_flags;                                                               \
    v_flags = fcntl(fd, F_GETFL, 0);                                           \
                                                                               \
    if (v_flags == -1) {                                                       \
      *(res) = -1;                                                             \
    } else {                                                                   \
                                                                               \
      /* Add non-blocking flag */                                              \
      v_flags |= O_NONBLOCK;                                                   \
      if (fcntl(fd, F_SETFL, v_flags) == -1) {                                 \
        *(res) = -1;                                                            \
      } else                                                                   \
        *(res) = 0;                                                            \
    }                                                                          \
  }

#endif
