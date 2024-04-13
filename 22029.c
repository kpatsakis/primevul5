PJ_DEF(pj_status_t) pj_stun_unknown_attr_create(pj_pool_t *pool,
						unsigned attr_cnt,
						const pj_uint16_t attr_array[],
						pj_stun_unknown_attr **p_attr)
{
    pj_stun_unknown_attr *attr;
    unsigned i;

    PJ_ASSERT_RETURN(pool && attr_cnt < PJ_STUN_MAX_ATTR && p_attr, PJ_EINVAL);

    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_unknown_attr);
    INIT_ATTR(attr, PJ_STUN_ATTR_UNKNOWN_ATTRIBUTES, attr_cnt * 2);

    attr->attr_count = attr_cnt;
    for (i=0; i<attr_cnt; ++i) {
	attr->attrs[i] = attr_array[i];
    }

    /* If the number of unknown attributes is an odd number, one of the
     * attributes MUST be repeated in the list.
     */
    /* No longer necessary
    if ((attr_cnt & 0x01)) {
	attr->attrs[attr_cnt] = attr_array[attr_cnt-1];
    }
    */

    *p_attr = attr;

    return PJ_SUCCESS;
}