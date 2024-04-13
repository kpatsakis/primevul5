static void x25_remove_socket(struct sock *sk)
{
	write_lock_bh(&x25_list_lock);
	sk_del_node_init(sk);
	write_unlock_bh(&x25_list_lock);
}