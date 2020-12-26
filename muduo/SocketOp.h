#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace SocketOp{ 
    int createNonblockingOrDie(sa_family_t family);
    void closeOrDie(int sockfd);
}