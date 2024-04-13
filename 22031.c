PJ_DEF(pj_status_t) pj_stun_sockaddr_attr_init( pj_stun_sockaddr_attr *attr,
						int attr_type, 
						pj_bool_t xor_ed,
						const pj_sockaddr_t *addr,
						unsigned addr_len)
{
    unsigned attr_len;

    PJ_ASSERT_RETURN(attr && addr_len && addr, PJ_EINVAL);
    PJ_ASSERT_RETURN(addr_len == sizeof(pj_sockaddr_in) ||
		     addr_len == sizeof(pj_sockaddr_in6), PJ_EINVAL);

    attr_len = pj_sockaddr_get_addr_len(addr) + 4;
    INIT_ATTR(attr, attr_type, attr_len);

    pj_memcpy(&attr->sockaddr, addr, addr_len);
    attr->xor_ed = xor_ed;

    return PJ_SUCCESS;
}