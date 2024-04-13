static pj_status_t process_answer(pj_pool_t *pool,
				  pjmedia_sdp_session *local_offer,
				  pjmedia_sdp_session *answer,
				  pj_bool_t allow_asym,
				  pjmedia_sdp_session **p_active)
{
    unsigned omi = 0; /* Offer media index */
    unsigned ami = 0; /* Answer media index */
    pj_bool_t has_active = PJ_FALSE;
    pjmedia_sdp_session *offer;
    pj_status_t status;

    /* Check arguments. */
    PJ_ASSERT_RETURN(pool && local_offer && answer && p_active, PJ_EINVAL);

    /* Duplicate local offer SDP. */
    offer = pjmedia_sdp_session_clone(pool, local_offer);

    /* Check that media count match between offer and answer */
    // Ticket #527, different media count is allowed for more interoperability,
    // however, the media order must be same between offer and answer.
    // if (offer->media_count != answer->media_count)
    //	   return PJMEDIA_SDPNEG_EMISMEDIA;

    /* Now update each media line in the offer with the answer. */
    for (; omi<offer->media_count; ++omi) {
	if (ami == answer->media_count) {
	    /* The answer has less media than the offer */
	    pjmedia_sdp_media *am;

	    /* Generate matching-but-disabled-media for the answer */
	    am = sdp_media_clone_deactivate(pool, offer->media[omi],
	                                    offer->media[omi], offer);
	    answer->media[answer->media_count++] = am;
	    ++ami;

	    /* Deactivate our media offer too */
	    pjmedia_sdp_media_deactivate(pool, offer->media[omi]);

	    /* No answer media to be negotiated */
	    continue;
	}

	status = process_m_answer(pool, offer->media[omi], answer->media[ami],
				  allow_asym);

	/* If media type is mismatched, just disable the media. */
	if (status == PJMEDIA_SDPNEG_EINVANSMEDIA) {
	    pjmedia_sdp_media_deactivate(pool, offer->media[omi]);
	    continue;
	}
	/* No common format in the answer media. */
	else if (status == PJMEDIA_SDPNEG_EANSNOMEDIA) {
	    pjmedia_sdp_media_deactivate(pool, offer->media[omi]);
	    pjmedia_sdp_media_deactivate(pool, answer->media[ami]);
	} 
	/* Return the error code, for other errors. */
	else if (status != PJ_SUCCESS) {
	    return status;
	}

	if (offer->media[omi]->desc.port != 0)
	    has_active = PJ_TRUE;

	++ami;
    }

    *p_active = offer;

    return has_active ? PJ_SUCCESS : PJMEDIA_SDPNEG_ENOMEDIA;
}