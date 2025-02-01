#include <ae2f/Call.h>

#if ae2f_IS_WIN
#include <ae2f/Inet/Wsa.h>
#include <ae2f/Async/WinIOApiSet.h>

enum __SENDRECV {
  SEND, RECV
};

#include <thread>
#include <vector>
#include <stdio.h>

#ifndef _WIN32

// for async
#include <sys/epoll.h>

#endif

constexpr unsigned int 
MAX_WORKER_THREAD = 4, 
PACKET_SIZE = 1024,
CLIENT_MAX = SOMAXCONN;

#if !ae2f_IS_WIN

#define wtf char

typedef struct _OVERLAPPED {
    uint32_t* Internal;
    uint32_t* InternalHigh;
      union {
        struct {
          uint32_t Offset;
          uint32_t OffsetHigh;
        } DUMMYSTRUCTNAME;
        void* Pointer;
      } DUMMYUNIONNAME;
      HANDLE    hEvent;
} OVERLAPPED, *LPOVERLAPPED;

#endif

struct LappedIO {
    OVERLAPPED overlapped;
    WSABUF wsa_buf;
    char buffer[PACKET_SIZE];
    bool PRO_TYPE;
};

struct ClientInfo {
    ae2f_Socket client;
    LappedIO RecvIO, SendIO;
};

#define IOPORTDEF

class IOCompletionPort {
    HANDLE IOCPHANDLE;
    
    union __THREAD {
       // std::thread td[1];
       uint8_t buf[sizeof(std::thread)];
       uint8_t td[sizeof(std::thread)];

       public:
       inline __THREAD() : buf{0, } {}
       ~__THREAD() { ((std::thread*)buf)->~thread(); }
    }; 
    
    std::vector<__THREAD> Threads;
    std::thread AcceptThread;
    ae2f_Socket SvrSock;

    void IOPORTDEF Send(ClientInfo* clientinfo, const char* pMsg);
    void IOPORTDEF Recv(ClientInfo* clientinfo);

    void IOPORTDEF WorkTd(size_t idx);
    void IOPORTDEF AcceptTd();

public:

    inline IOPORTDEF IOCompletionPort() {}
    inline IOPORTDEF ~IOCompletionPort();

    void IOPORTDEF BindAndListen(int port);
    void IOPORTDEF StartSvr();
};

#define IOPORTDEF IOCompletionPort::
#define MADEWORD2N2 0b1000000010

IOPORTDEF ~IOCompletionPort() {
  close(SvrSock);
  ae2f_InetDel();
}

void IOPORTDEF BindAndListen(int port) {
  int opt;
  ae2f_InetMkData data;
  ae2f_InetMk(MADEWORD2N2, &data);
  SvrSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  setsockopt(SvrSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  IOCPHANDLE = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, MAX_WORKER_THREAD);
  sockaddr_in SvrAddr = {0};
  SvrAddr.sin_family = AF_INET;
  SvrAddr.sin_port = htons(port);
  SvrAddr.sin_addr.s_addr = INADDR_ANY;

  bind(SvrSock, (const sockaddr*)&SvrAddr, sizeof(SvrAddr));
  listen(SvrSock, SOMAXCONN);
}

void IOPORTDEF StartSvr() {
  Threads.resize(MAX_WORKER_THREAD);

  for(size_t i = 0; i < MAX_WORKER_THREAD; i++) {
    new(Threads[i].td) std::thread([this, i]() { this->WorkTd(i); });
  } AcceptThread = std::thread([this]() { this->AcceptTd(); });
  AcceptThread.join();
}


#include <string.h>

/// @todo Understand
/// WSASend
void IOPORTDEF Send(ClientInfo* clientinfo, const char* pMsg) {
  memcpy(clientinfo->SendIO.buffer, pMsg, PACKET_SIZE);
  clientinfo->SendIO.PRO_TYPE = SEND;
  clientinfo->SendIO.wsa_buf.buf = clientinfo->SendIO.buffer;
  clientinfo->SendIO.wsa_buf.len = PACKET_SIZE - 1;

  switch(strnlen(clientinfo->SendIO.wsa_buf.buf, PACKET_SIZE)) {
    case PACKET_SIZE:
    puts("Exception: the string might be longer than you've thought");
    break;

    default: {
      WSASend(
        clientinfo->client, &clientinfo->SendIO.wsa_buf, 1, 0, 0, (LPWSAOVERLAPPED)&clientinfo->SendIO.overlapped, 0
      );
    }
  }
}

/// @todo 
/// WSARecv 
void IOPORTDEF Recv(ClientInfo* clientinfo) {
  uint32_t flags = 0, bytesRead = 0;
  clientinfo->RecvIO.PRO_TYPE = RECV;
  clientinfo->RecvIO.wsa_buf.buf = clientinfo->RecvIO.buffer;
  clientinfo->RecvIO.wsa_buf.len = bytesRead = PACKET_SIZE;

  // WSARecv...
  bytesRead = recv(
    clientinfo->client, 
    clientinfo->RecvIO.wsa_buf.buf, 
    clientinfo->RecvIO.wsa_buf.len, 
    0
  );

}

#include <assert.h>

/// @todo
/// GetQueuedCompletionStatus 
void IOPORTDEF WorkTd(size_t idx) {
  uint32_t bytesTransferred = 0;
  ClientInfo* clientinfo = 0;
  LPOVERLAPPED overlapped = 0;

  while((volatile int)1) {
    // GetQueuedCompletionStatus
    LappedIO* lappedIO = (LappedIO*)overlapped;
    switch(lappedIO->PRO_TYPE) {
      case SEND: {
        this->Send(clientinfo, lappedIO->buffer);
        this->Recv(clientinfo);
      } break;
      case RECV: {
        puts("I am sending:\n");
        puts(lappedIO->buffer);
        puts("\n");
      } break;
      default: assert(0);
    }
  }
}
/// @todo
/// CreateIoCompletionPort
/// 
void IOPORTDEF AcceptTd() {

  while((volatile int)1) {
    ae2f_InetSock client = accept(SvrSock, 0, 0);

    // 적혀 있던 거: ClientInfo* clientinfo = new ClientInfo();
    // 문제 요약: 안 끝나는 반복문 안의 누수임
    // Now solved
    union {
      ClientInfo info[1];
      uint8_t buf[sizeof(ClientInfo)];
    } clientinfo = {.buf = {0, }};

    // No need:
    // memset(clientinfo, sizeof(clientinfo));

    clientinfo.info->client = client;
    CreateIoCompletionPort((HANDLE)client, IOCPHANDLE, (uint32_t*)clientinfo.info, 0);
    this->Recv(clientinfo.info);
  }
}
#else
int main() {}
#endif