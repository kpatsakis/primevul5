PJ_DEF(pj_status_t) pj_stun_msgint_attr_create(pj_pool_t *pool,
					       pj_stun_msgint_attr **p_attr)
{
    pj_stun_msgint_attr *attr;

    PJ_ASSERT_RETURN(pool && p_attr, PJ_EINVAL);

    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_msgint_attr);
    INIT_ATTR(attr, PJ_STUN_ATTR_MESSAGE_INTEGRITY, 20);

    *p_attr = attr;

    return PJ_SUCCESS;
}