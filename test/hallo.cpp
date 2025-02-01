#include <ae2f/Inet/Wsa.h>
#include <ae2f/Async/WinIOApiSet.h>

#if ae2f_IS_LINUX

enum __SENDRECV {
  SEND, RECV
};

#include <thread>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

constexpr unsigned int 
MAX_WORKER_THREAD = 4, 
PACKET_SIZE = 1024,
CLIENT_MAX = SOMAXCONN;

struct LappedIO {
    int bytesTransferred;
    char buffer[PACKET_SIZE];
    __SENDRECV PRO_TYPE;
};

struct ClientInfo {
    int client;
    LappedIO RecvIO, SendIO;
};

class IOCompletionPort {
    int epoll_fd;
    int SvrSock;
    union __THREAD {
       // std::thread td[1];
       uint8_t buf[sizeof(std::thread)];

       public:
       inline __THREAD() : buf{0, } {}
       ~__THREAD() { ((std::thread*)buf)->~thread(); }
    }; 

    std::vector<__THREAD> Threads;
    std::thread AcceptThread;

    void Send(ClientInfo* clientinfo, const char* pMsg);
    void Recv(ClientInfo* clientinfo);
    void WorkTd();
    void AcceptTd();

public:
    IOCompletionPort() {}
    ~IOCompletionPort();

    void BindAndListen(int port);
    void StartSvr();
};

IOCompletionPort::~IOCompletionPort() {
    close(SvrSock);
}

void IOCompletionPort::BindAndListen(int port) {
    SvrSock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(SvrSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int flags = fcntl(SvrSock, F_GETFL, 0); // Getting Current Flag
    fcntl(SvrSock, F_SETFL, flags | O_NONBLOCK); // Add a non-block Flag


    epoll_fd = epoll_create1(0);
    sockaddr_in SvrAddr = {0};
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(port);
    SvrAddr.sin_addr.s_addr = INADDR_ANY;

    bind(SvrSock, (const sockaddr*)&SvrAddr, sizeof(SvrAddr));
    listen(SvrSock, SOMAXCONN);
}

void IOCompletionPort::StartSvr() {
    Threads.resize(MAX_WORKER_THREAD);

    for(size_t i = 0; i < MAX_WORKER_THREAD; i++) {
        
        new(Threads[i].buf) std::thread([this]() { this->WorkTd(); });
    }
    AcceptThread = std::thread([this]() { this->AcceptTd(); });
    AcceptThread.join();
}

void IOCompletionPort::Send(ClientInfo* clientinfo, const char* pMsg) {
    memcpy(clientinfo->SendIO.buffer, pMsg, PACKET_SIZE);
    clientinfo->SendIO.PRO_TYPE = SEND;

    ssize_t bytesSent = send(clientinfo->client, clientinfo->SendIO.buffer, PACKET_SIZE, 0);
    if (bytesSent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        
        perror("send failed");
    }
}

void IOCompletionPort::Recv(ClientInfo* clientinfo) {
    ssize_t bytesRead = recv(clientinfo->client, clientinfo->RecvIO.buffer, PACKET_SIZE, 0);
    if (bytesRead > 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        clientinfo->RecvIO.PRO_TYPE = RECV;
        clientinfo->RecvIO.bytesTransferred = bytesRead;
    } else if (bytesRead == 0) {
        // Connection closed
        close(clientinfo->client);
    } else {
        perror("recv failed");
    }
}

void IOCompletionPort::WorkTd() {
    struct epoll_event evs[CLIENT_MAX];
    while (true) {
        int evcount = epoll_wait(epoll_fd, evs, CLIENT_MAX, -1);
        for(size_t i = 0; i < evcount; i++) {
            ClientInfo* clientInfo = (ClientInfo*)evs[i].data.ptr;
            if(evs[i].events & EPOLLIN) {
                this->Recv(clientInfo);
            } else if (evs[i].events & EPOLLOUT) {
                this->Send(clientInfo, clientInfo->SendIO.buffer);
            }
        }
    }
}

void IOCompletionPort::AcceptTd() {
    while (true) {
        int client = accept(SvrSock, nullptr, nullptr);
        if (client < 0) {
            perror("accept failed");
            continue;
        }

        fcntl(client, F_SETFL, O_NONBLOCK);

        ClientInfo clientinfo[1];
        clientinfo->client = client;

        struct epoll_event ev;
        ev.events = (((uint64_t)EPOLLIN) | ((uint64_t)EPOLLOUT));
        ev.data.ptr = clientinfo;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &ev);

        this->Recv(clientinfo);
    }
}

int main() {

    IOCompletionPort iocp;
    iocp.BindAndListen(200);
    iocp.StartSvr();
    return 0;
}
#else
int main() {
    puts("You are not on linux");
    return 0;
}
#endif