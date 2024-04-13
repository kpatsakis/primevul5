static struct qrtr_sock *qrtr_port_lookup(int port)
{
	struct qrtr_sock *ipc;

	if (port == QRTR_PORT_CTRL)
		port = 0;

	rcu_read_lock();
	ipc = idr_find(&qrtr_ports, port);
	if (ipc)
		sock_hold(&ipc->sk);
	rcu_read_unlock();

	return ipc;
}