PJ_DEF(pj_status_t) pj_stun_string_attr_init( pj_stun_string_attr *attr,
					      pj_pool_t *pool,
					      int attr_type,
					      const pj_str_t *value)
{
    if (value && value->slen) {
	INIT_ATTR(attr, attr_type, value->slen);
	attr->value.slen = value->slen;
	pj_strdup(pool, &attr->value, value);
    } else {
	INIT_ATTR(attr, attr_type, 0);
    }
    return PJ_SUCCESS;
}