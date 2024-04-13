static void* clone_string_attr(pj_pool_t *pool, const void *src)
{
    const pj_stun_string_attr *asrc = (const pj_stun_string_attr*)src;
    pj_stun_string_attr *dst = PJ_POOL_ALLOC_T(pool, pj_stun_string_attr);

    pj_memcpy(dst, src, sizeof(pj_stun_attr_hdr));
    pj_strdup(pool, &dst->value, &asrc->value);

    return (void*)dst;
}