static int svc_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct atm_vcc *vcc;

	if (sk)  {
		vcc = ATM_SD(sock);
		pr_debug("svc_release %p\n", vcc);
		clear_bit(ATM_VF_READY, &vcc->flags);
		/* VCC pointer is used as a reference, so we must not free it
		   (thereby subjecting it to re-use) before all pending connections
		   are closed */
		svc_disconnect(vcc);
		vcc_release(sock);
	}
	return 0;
}