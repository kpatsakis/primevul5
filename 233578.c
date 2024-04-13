static int svc_create(struct net *net, struct socket *sock,int protocol)
{
	int error;

	if (net != &init_net)
		return -EAFNOSUPPORT;

	sock->ops = &svc_proto_ops;
	error = vcc_create(net, sock, protocol, AF_ATMSVC);
	if (error) return error;
	ATM_SD(sock)->local.sas_family = AF_ATMSVC;
	ATM_SD(sock)->remote.sas_family = AF_ATMSVC;
	return 0;
}