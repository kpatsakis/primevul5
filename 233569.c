static int svc_getname(struct socket *sock,struct sockaddr *sockaddr,
    int *sockaddr_len,int peer)
{
	struct sockaddr_atmsvc *addr;

	*sockaddr_len = sizeof(struct sockaddr_atmsvc);
	addr = (struct sockaddr_atmsvc *) sockaddr;
	memcpy(addr,peer ? &ATM_SD(sock)->remote : &ATM_SD(sock)->local,
	    sizeof(struct sockaddr_atmsvc));
	return 0;
}