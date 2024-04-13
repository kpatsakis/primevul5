void atmsvc_exit(void)
{
	sock_unregister(PF_ATMSVC);
}