PJ_INLINE(void) rewrite_pt(pj_pool_t *pool, pj_str_t *attr_val,
			   const pj_str_t *old_pt, const pj_str_t *new_pt)
{
    int len_diff = (int)(new_pt->slen - old_pt->slen);

    /* Note that attribute value should be null-terminated. */
    if (len_diff > 0) {
	pj_str_t new_val;
	new_val.ptr = (char*)pj_pool_alloc(pool, attr_val->slen+len_diff+1);
	new_val.slen = attr_val->slen + len_diff;
	pj_memcpy(new_val.ptr + len_diff, attr_val->ptr, attr_val->slen + 1);
	*attr_val = new_val;
    } else if (len_diff < 0) {
	attr_val->slen += len_diff;
	pj_memmove(attr_val->ptr, attr_val->ptr - len_diff,
		   attr_val->slen + 1);
    }
    pj_memcpy(attr_val->ptr, new_pt->ptr, new_pt->slen);
}