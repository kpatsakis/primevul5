PJ_DEF(pj_str_t) pj_stun_get_err_reason(int err_code)
{
#if 0
    /* Find error using linear search */
    unsigned i;

    for (i=0; i<PJ_ARRAY_SIZE(stun_err_msg_map); ++i) {
	if (stun_err_msg_map[i].err_code == err_code)
	    return pj_str((char*)stun_err_msg_map[i].err_msg);
    }
    return pj_str(NULL);
#else
    /* Find error message using binary search */
    int first = 0;
    int n = PJ_ARRAY_SIZE(stun_err_msg_map);

    while (n > 0) {
	int half = n/2;
	int mid = first + half;

	if (stun_err_msg_map[mid].err_code < err_code) {
	    first = mid+1;
	    n -= (half+1);
	} else if (stun_err_msg_map[mid].err_code > err_code) {
	    n = half;
	} else {
	    first = mid;
	    break;
	}
    }


    if (stun_err_msg_map[first].err_code == err_code) {
	return pj_str((char*)stun_err_msg_map[first].err_msg);
    } else {
	return pj_str(NULL);
    }
#endif
}