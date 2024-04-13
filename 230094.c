static void nfs4_update_session(struct nfs4_session *session,
		struct nfs41_create_session_res *res)
{
	nfs4_copy_sessionid(&session->sess_id, &res->sessionid);
	/* Mark client id and session as being confirmed */
	session->clp->cl_exchange_flags |= EXCHGID4_FLAG_CONFIRMED_R;
	set_bit(NFS4_SESSION_ESTABLISHED, &session->session_state);
	session->flags = res->flags;
	memcpy(&session->fc_attrs, &res->fc_attrs, sizeof(session->fc_attrs));
	if (res->flags & SESSION4_BACK_CHAN)
		memcpy(&session->bc_attrs, &res->bc_attrs,
				sizeof(session->bc_attrs));
}