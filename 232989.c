unsigned int udp_poll(struct file *file, struct socket *sock, poll_table *wait)
{
	unsigned int mask = datagram_poll(file, sock, wait);
	struct sock *sk = sock->sk;

	if (!skb_queue_empty(&udp_sk(sk)->reader_queue))
		mask |= POLLIN | POLLRDNORM;

	sock_rps_record_flow(sk);

	/* Check for false positives due to checksum errors */
	if ((mask & POLLRDNORM) && !(file->f_flags & O_NONBLOCK) &&
	    !(sk->sk_shutdown & RCV_SHUTDOWN) && first_packet_length(sk) == -1)
		mask &= ~(POLLIN | POLLRDNORM);

	return mask;

}