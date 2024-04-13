static void update_media_direction(pj_pool_t *pool,
				   const pjmedia_sdp_media *remote,
				   pjmedia_sdp_media *local)
{
    pjmedia_dir old_dir = PJMEDIA_DIR_ENCODING_DECODING,
	        new_dir;

    /* Get the media direction of local SDP */
    if (pjmedia_sdp_media_find_attr2(local, "sendonly", NULL))
	old_dir = PJMEDIA_DIR_ENCODING;
    else if (pjmedia_sdp_media_find_attr2(local, "recvonly", NULL))
	old_dir = PJMEDIA_DIR_DECODING;
    else if (pjmedia_sdp_media_find_attr2(local, "inactive", NULL))
	old_dir = PJMEDIA_DIR_NONE;

    new_dir = old_dir;

    /* Adjust local media direction based on remote media direction */
    if (pjmedia_sdp_media_find_attr2(remote, "inactive", NULL) != NULL) {
	/* If remote has "a=inactive", then local is inactive too */

	new_dir = PJMEDIA_DIR_NONE;

    } else if(pjmedia_sdp_media_find_attr2(remote, "sendonly", NULL) != NULL) {
	/* If remote has "a=sendonly", then set local to "recvonly" if
	 * it is currently "sendrecv". Otherwise if local is NOT "recvonly",
	 * then set local direction to "inactive".
	 */
	switch (old_dir) {
	case PJMEDIA_DIR_ENCODING_DECODING:
	    new_dir = PJMEDIA_DIR_DECODING;
	    break;
	case PJMEDIA_DIR_DECODING:
	    /* No change */
	    break;
	default:
	    new_dir = PJMEDIA_DIR_NONE;
	    break;
	}

    } else if(pjmedia_sdp_media_find_attr2(remote, "recvonly", NULL) != NULL) {
	/* If remote has "a=recvonly", then set local to "sendonly" if
	 * it is currently "sendrecv". Otherwise if local is NOT "sendonly",
	 * then set local direction to "inactive"
	 */
    
	switch (old_dir) {
	case PJMEDIA_DIR_ENCODING_DECODING:
	    new_dir = PJMEDIA_DIR_ENCODING;
	    break;
	case PJMEDIA_DIR_ENCODING:
	    /* No change */
	    break;
	default:
	    new_dir = PJMEDIA_DIR_NONE;
	    break;
	}

    } else {
	/* Remote indicates "sendrecv" capability. No change to local 
	 * direction 
	 */
    }

    if (new_dir != old_dir) {
	pjmedia_sdp_attr *a = NULL;

	remove_all_media_directions(local);

	switch (new_dir) {
	case PJMEDIA_DIR_NONE:
	    a = pjmedia_sdp_attr_create(pool, "inactive", NULL);
	    break;
	case PJMEDIA_DIR_ENCODING:
	    a = pjmedia_sdp_attr_create(pool, "sendonly", NULL);
	    break;
	case PJMEDIA_DIR_DECODING:
	    a = pjmedia_sdp_attr_create(pool, "recvonly", NULL);
	    break;
	default:
	    /* sendrecv */
	    break;
	}
	
	if (a) {
	    pjmedia_sdp_media_add_attr(local, a);
	}
    }
}