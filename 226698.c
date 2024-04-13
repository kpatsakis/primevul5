static void qrtr_reset_ports(void)
{
	struct qrtr_sock *ipc;
	int id;

	mutex_lock(&qrtr_port_lock);
	idr_for_each_entry(&qrtr_ports, ipc, id) {
		/* Don't reset control port */
		if (id == 0)
			continue;

		sock_hold(&ipc->sk);
		ipc->sk.sk_err = ENETRESET;
		ipc->sk.sk_error_report(&ipc->sk);
		sock_put(&ipc->sk);
	}
	mutex_unlock(&qrtr_port_lock);
}