PJ_DEF(pj_status_t) pj_stun_msg_init( pj_stun_msg *msg,
				      unsigned msg_type,
				      pj_uint32_t magic,
				      const pj_uint8_t tsx_id[12])
{
    PJ_ASSERT_RETURN(msg && msg_type, PJ_EINVAL);

    msg->hdr.type = (pj_uint16_t) msg_type;
    msg->hdr.length = 0;
    msg->hdr.magic = magic;
    msg->attr_count = 0;

    if (tsx_id) {
	pj_memcpy(&msg->hdr.tsx_id, tsx_id, sizeof(msg->hdr.tsx_id));
    } else {
	struct transaction_id
	{
	    pj_uint32_t	    proc_id;
	    pj_uint32_t	    random;
	    pj_uint32_t	    counter;
	} id;
	static pj_uint32_t pj_stun_tsx_id_counter;

	if (!pj_stun_tsx_id_counter)
	    pj_stun_tsx_id_counter = pj_rand();

	id.proc_id = pj_getpid();
	id.random = pj_rand();
	id.counter = pj_stun_tsx_id_counter++;

	pj_memcpy(&msg->hdr.tsx_id, &id, sizeof(msg->hdr.tsx_id));
    }

    return PJ_SUCCESS;
}