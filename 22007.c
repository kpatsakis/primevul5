PJ_DEF(pj_status_t) pj_stun_binary_attr_create(pj_pool_t *pool,
					       int attr_type,
					       const pj_uint8_t *data,
					       unsigned length,
					       pj_stun_binary_attr **p_attr)
{
    pj_stun_binary_attr *attr;

    PJ_ASSERT_RETURN(pool && attr_type && p_attr, PJ_EINVAL);
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_binary_attr);
    *p_attr = attr;
    return pj_stun_binary_attr_init(attr, pool, attr_type, data, length);
}