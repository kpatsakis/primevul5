PJ_DEF(pj_status_t) pj_stun_msg_create_response(pj_pool_t *pool,
						const pj_stun_msg *req_msg,
						unsigned err_code,
						const pj_str_t *err_msg,
						pj_stun_msg **p_response)
{
    unsigned msg_type = req_msg->hdr.type;
    pj_stun_msg *response = NULL;
    pj_status_t status;

    PJ_ASSERT_RETURN(pool && p_response, PJ_EINVAL);

    PJ_ASSERT_RETURN(PJ_STUN_IS_REQUEST(msg_type), 
		     PJNATH_EINSTUNMSGTYPE);

    /* Create response or error response */
    if (err_code)
	msg_type |= PJ_STUN_ERROR_RESPONSE_BIT;
    else
	msg_type |= PJ_STUN_SUCCESS_RESPONSE_BIT;

    status = pj_stun_msg_create(pool, msg_type, req_msg->hdr.magic, 
				req_msg->hdr.tsx_id, &response);
    if (status != PJ_SUCCESS) {
	return status;
    }

    /* Add error code attribute */
    if (err_code) {
	status = pj_stun_msg_add_errcode_attr(pool, response, 
					      err_code, err_msg);
	if (status != PJ_SUCCESS) {
	    return status;
	}
    }

    *p_response = response;
    return PJ_SUCCESS;
}