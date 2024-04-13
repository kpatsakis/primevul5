static pjmedia_sdp_media *sdp_media_clone_deactivate(
				    pj_pool_t *pool,
                                    const pjmedia_sdp_media *rem_med,
                                    const pjmedia_sdp_media *local_med,
                                    const pjmedia_sdp_session *local_sess)
{
    pjmedia_sdp_media *res;

    res = pjmedia_sdp_media_clone_deactivate(pool, rem_med);
    if (!res)
	return NULL;

    if (!res->conn && (!local_sess || !local_sess->conn)) {
	if (local_med && local_med->conn)
	    res->conn = pjmedia_sdp_conn_clone(pool, local_med->conn);
	else {
	    res->conn = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_conn);
	    res->conn->net_type = pj_str("IN");
	    res->conn->addr_type = pj_str("IP4");
	    res->conn->addr = pj_str("127.0.0.1");
	}
    }

    return res;
}