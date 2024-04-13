static int qrtr_port_assign(struct qrtr_sock *ipc, int *port)
{
	u32 min_port;
	int rc;

	mutex_lock(&qrtr_port_lock);
	if (!*port) {
		min_port = QRTR_MIN_EPH_SOCKET;
		rc = idr_alloc_u32(&qrtr_ports, ipc, &min_port, QRTR_MAX_EPH_SOCKET, GFP_ATOMIC);
		if (!rc)
			*port = min_port;
	} else if (*port < QRTR_MIN_EPH_SOCKET && !capable(CAP_NET_ADMIN)) {
		rc = -EACCES;
	} else if (*port == QRTR_PORT_CTRL) {
		min_port = 0;
		rc = idr_alloc_u32(&qrtr_ports, ipc, &min_port, 0, GFP_ATOMIC);
	} else {
		min_port = *port;
		rc = idr_alloc_u32(&qrtr_ports, ipc, &min_port, *port, GFP_ATOMIC);
		if (!rc)
			*port = min_port;
	}
	mutex_unlock(&qrtr_port_lock);

	if (rc == -ENOSPC)
		return -EADDRINUSE;
	else if (rc < 0)
		return rc;

	sock_hold(&ipc->sk);

	return 0;
}