static int sock_has_rx_data(struct socket *sock)
{
	if (unlikely(!sock))
		return 0;

	if (sock->ops->peek_len)
		return sock->ops->peek_len(sock);

	return skb_queue_empty(&sock->sk->sk_receive_queue);
}