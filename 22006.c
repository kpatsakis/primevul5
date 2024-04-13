PJ_DEF(pj_status_t) pj_stun_msg_add_unknown_attr(pj_pool_t *pool,
						 pj_stun_msg *msg,
						 unsigned attr_cnt,
						 const pj_uint16_t attr_type[])
{
    pj_stun_unknown_attr *attr = NULL;
    pj_status_t status;

    status = pj_stun_unknown_attr_create(pool, attr_cnt, attr_type, &attr);
    if (status != PJ_SUCCESS)
	return status;

    return pj_stun_msg_add_attr(msg, &attr->hdr);
}