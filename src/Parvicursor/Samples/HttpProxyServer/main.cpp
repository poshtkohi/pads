#include "HttpProxyServer.h"


using namespace Samples::AsyncSockets;
//---------------------------------------
//extern Int32 _connection_num_;
static void StopRequested_signal_handler(int sig)
{
    printf("\nCTRL+C was pressed.\n");
    //printf("The number of connections is: %d\n", _connection_num_);
    printf("The proxy server program is terminating.\nPlease wait ...\n");
    exit(0);
}

static void SIGSEGV_handler(int sig)
{
    printf("\nSIGSEGV_handler\n");
    exit(-1);
}
//---------------------------------------
int main(int argc, char* argv[])
{
    my_signal(SIGINT, StopRequested_signal_handler);
    //signal(SIGSEGV, SIGSEGV_handler);
    return HttpProxyServer::Parvicursor_main(argc, argv);
}
//---------------------------------------
