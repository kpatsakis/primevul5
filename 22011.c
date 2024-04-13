PJ_DEF(pj_status_t) pj_stun_msg_add_binary_attr(pj_pool_t *pool,
						pj_stun_msg *msg,
						int attr_type,
						const pj_uint8_t *data,
						unsigned length)
{
    pj_stun_binary_attr *attr = NULL;
    pj_status_t status;

    status = pj_stun_binary_attr_create(pool, attr_type,
					data, length, &attr);
    if (status != PJ_SUCCESS)
	return status;

    return pj_stun_msg_add_attr(msg, &attr->hdr);
}