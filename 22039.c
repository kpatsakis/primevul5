PJ_DEF(pj_status_t) pj_stun_uint_attr_create(pj_pool_t *pool,
					     int attr_type,
					     pj_uint32_t value,
					     pj_stun_uint_attr **p_attr)
{
    pj_stun_uint_attr *attr;

    PJ_ASSERT_RETURN(pool && p_attr, PJ_EINVAL);

    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_uint_attr);
    INIT_ATTR(attr, attr_type, 4);
    attr->value = value;

    *p_attr = attr;

    return PJ_SUCCESS;
}