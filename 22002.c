PJ_DEF(pj_status_t) pj_stun_msg_add_sockaddr_attr(pj_pool_t *pool,
						  pj_stun_msg *msg,
						  int attr_type, 
						  pj_bool_t xor_ed,
						  const pj_sockaddr_t *addr,
						  unsigned addr_len)
{
    pj_stun_sockaddr_attr *attr;
    pj_status_t status;

    status = pj_stun_sockaddr_attr_create(pool, attr_type, xor_ed,
					         addr, addr_len, &attr);
    if (status != PJ_SUCCESS)
	return status;

    return pj_stun_msg_add_attr(msg, &attr->hdr);
}