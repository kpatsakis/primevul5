static void tcp_dis_handler(struct smb2_transport *t, void *p)
{
	struct smb2_tree *tree = (struct smb2_tree *)p;
	smb2_transport_dead(tree->session->transport,
			NT_STATUS_LOCAL_DISCONNECT);
	t = NULL;
	tree = NULL;
}