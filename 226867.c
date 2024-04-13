static inline void sctp_copy_descendant(struct sock *sk_to,
					const struct sock *sk_from)
{
	size_t ancestor_size = sizeof(struct inet_sock);

	ancestor_size += sk_from->sk_prot->obj_size;
	ancestor_size -= offsetof(struct sctp_sock, pd_lobby);
	__inet_sk_copy_descendant(sk_to, sk_from, ancestor_size);
}