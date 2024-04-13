PJ_DEF(pj_status_t) pj_stun_binary_attr_init( pj_stun_binary_attr *attr,
					      pj_pool_t *pool,
					      int attr_type,
					      const pj_uint8_t *data,
					      unsigned length)
{
    PJ_ASSERT_RETURN(attr_type, PJ_EINVAL);

    INIT_ATTR(attr, attr_type, length);

    attr->magic = PJ_STUN_MAGIC;

    if (data && length) {
	attr->length = length;
	attr->data = (pj_uint8_t*) pj_pool_alloc(pool, length);
	pj_memcpy(attr->data, data, length);
    } else {
	attr->data = NULL;
	attr->length = 0;
    }

    return PJ_SUCCESS;
}