PJ_DEF(pj_stun_msg*) pj_stun_msg_clone( pj_pool_t *pool,
					const pj_stun_msg *src)
{
    pj_stun_msg *dst;
    unsigned i;

    PJ_ASSERT_RETURN(pool && src, NULL);

    dst = PJ_POOL_ZALLOC_T(pool, pj_stun_msg);
    pj_memcpy(dst, src, sizeof(pj_stun_msg));

    /* Duplicate the attributes */
    for (i=0, dst->attr_count=0; i<src->attr_count; ++i) {
	dst->attr[dst->attr_count] = pj_stun_attr_clone(pool, src->attr[i]);
	if (dst->attr[dst->attr_count])
	    ++dst->attr_count;
    }

    return dst;
}