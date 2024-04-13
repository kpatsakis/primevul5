PJ_DEF(pj_status_t)  pj_stun_msg_add_uint64_attr(pj_pool_t *pool,
					         pj_stun_msg *msg,
					         int attr_type,
					         const pj_timestamp *value)
{
    pj_stun_uint64_attr *attr = NULL;
    pj_status_t status;

    status = pj_stun_uint64_attr_create(pool, attr_type, value, &attr);
    if (status != PJ_SUCCESS)
	return status;

    return pj_stun_msg_add_attr(msg, &attr->hdr);
}