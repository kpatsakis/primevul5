static pj_status_t process_m_answer( pj_pool_t *pool,
				     pjmedia_sdp_media *offer,
				     pjmedia_sdp_media *answer,
				     pj_bool_t allow_asym)
{
    unsigned i;

    /* Check that the media type match our offer. */

    if (pj_strcmp(&answer->desc.media, &offer->desc.media)!=0) {
	/* The media type in the answer is different than the offer! */
	return PJMEDIA_SDPNEG_EINVANSMEDIA;
    }


    /* Check that transport in the answer match our offer. */

    /* At this point, transport type must be compatible, 
     * the transport instance will do more validation later.
     */
    if (pjmedia_sdp_transport_cmp(&answer->desc.transport, 
				  &offer->desc.transport) 
	!= PJ_SUCCESS)
    {
	return PJMEDIA_SDPNEG_EINVANSTP;
    }


    /* Check if remote has rejected our offer */
    if (answer->desc.port == 0) {
	
	/* Remote has rejected our offer. 
	 * Deactivate our media too.
	 */
	pjmedia_sdp_media_deactivate(pool, offer);

	/* Don't need to proceed */
	return PJ_SUCCESS;
    }

    /* Ticket #1148: check if remote answer does not set port to zero when
     * offered with port zero. Let's just tolerate it.
     */
    if (offer->desc.port == 0) {
	/* Don't need to proceed */
	return PJ_SUCCESS;
    }

    /* Process direction attributes */
    update_media_direction(pool, answer, offer);
 
    /* If asymetric media is allowed, then just check that remote answer has 
     * codecs that are within the offer. 
     *
     * Otherwise if asymetric media is not allowed, then we will choose only
     * one codec in our initial offer to match the answer.
     */
    if (allow_asym) {
	for (i=0; i<answer->desc.fmt_count; ++i) {
	    unsigned j;
	    pj_str_t *rem_fmt = &answer->desc.fmt[i];

	    for (j=0; j<offer->desc.fmt_count; ++j) {
		if (pj_strcmp(rem_fmt, &answer->desc.fmt[j])==0)
		    break;
	    }

	    if (j != offer->desc.fmt_count) {
		/* Found at least one common codec. */
		break;
	    }
	}

	if (i == answer->desc.fmt_count) {
	    /* No common codec in the answer! */
	    return PJMEDIA_SDPNEG_EANSNOMEDIA;
	}

	PJ_TODO(CHECK_SDP_NEGOTIATION_WHEN_ASYMETRIC_MEDIA_IS_ALLOWED);

    } else {
	/* Offer format priority based on answer format index/priority */
	unsigned offer_fmt_prior[PJMEDIA_MAX_SDP_FMT];

	/* Remove all format in the offer that has no matching answer */
	for (i=0; i<offer->desc.fmt_count;) {
	    unsigned pt;
	    pj_uint32_t j;
	    pj_str_t *fmt = &offer->desc.fmt[i];
	    

	    /* Find matching answer */
	    pt = pj_strtoul(fmt);

	    if (pt < 96) {
		for (j=0; j<answer->desc.fmt_count; ++j) {
		    if (pj_strcmp(fmt, &answer->desc.fmt[j])==0)
			break;
		}
	    } else {
		/* This is dynamic payload type.
		 * For dynamic payload type, we must look the rtpmap and
		 * compare the encoding name.
		 */
		const pjmedia_sdp_attr *a;
		pjmedia_sdp_rtpmap or_;

		/* Get the rtpmap for the payload type in the offer. */
		a = pjmedia_sdp_media_find_attr2(offer, "rtpmap", fmt);
		if (!a) {
		    pj_assert(!"Bug! Offer should have been validated");
		    return PJ_EBUG;
		}
		pjmedia_sdp_attr_get_rtpmap(a, &or_);

		/* Find paylaod in answer SDP with matching 
		 * encoding name and clock rate.
		 */
		for (j=0; j<answer->desc.fmt_count; ++j) {
		    a = pjmedia_sdp_media_find_attr2(answer, "rtpmap", 
						     &answer->desc.fmt[j]);
		    if (a) {
			pjmedia_sdp_rtpmap ar;
			pjmedia_sdp_attr_get_rtpmap(a, &ar);

			/* See if encoding name, clock rate, and channel
			 * count match 
			 */
			if (!pj_stricmp(&or_.enc_name, &ar.enc_name) &&
			    or_.clock_rate == ar.clock_rate &&
			    (pj_stricmp(&or_.param, &ar.param)==0 ||
			     (ar.param.slen==1 && *ar.param.ptr=='1')))
			{
			    /* Call custom format matching callbacks */
			    if (custom_fmt_match(pool, &or_.enc_name,
						 offer, i, answer, j, 0) ==
				PJ_SUCCESS)
			    {
				/* Match! */
				break;
			    }
			}
		    }
		}
	    }

	    if (j == answer->desc.fmt_count) {
		/* This format has no matching answer.
		 * Remove it from our offer.
		 */
		pjmedia_sdp_attr *a;

		/* Remove rtpmap associated with this format */
		a = pjmedia_sdp_media_find_attr2(offer, "rtpmap", fmt);
		if (a)
		    pjmedia_sdp_media_remove_attr(offer, a);

		/* Remove fmtp associated with this format */
		a = pjmedia_sdp_media_find_attr2(offer, "fmtp", fmt);
		if (a)
		    pjmedia_sdp_media_remove_attr(offer, a);

		/* Remove this format from offer's array */
		pj_array_erase(offer->desc.fmt, sizeof(offer->desc.fmt[0]),
			       offer->desc.fmt_count, i);
		--offer->desc.fmt_count;

	    } else {
		offer_fmt_prior[i] = j;
		++i;
	    }
	}

	if (0 == offer->desc.fmt_count) {
	    /* No common codec in the answer! */
	    return PJMEDIA_SDPNEG_EANSNOMEDIA;
	}

	/* Post process:
	 * - Resort offer formats so the order match to the answer.
	 * - Remove answer formats that unmatches to the offer.
	 */
	
	/* Resort offer formats */
	for (i=0; i<offer->desc.fmt_count; ++i) {
	    unsigned j;
	    for (j=i+1; j<offer->desc.fmt_count; ++j) {
		if (offer_fmt_prior[i] > offer_fmt_prior[j]) {
		    unsigned tmp = offer_fmt_prior[i];
		    offer_fmt_prior[i] = offer_fmt_prior[j];
		    offer_fmt_prior[j] = tmp;
		    str_swap(&offer->desc.fmt[i], &offer->desc.fmt[j]);
		}
	    }
	}

	/* Remove unmatched answer formats */
	{
	    unsigned del_cnt = 0;
	    for (i=0; i<answer->desc.fmt_count;) {
		/* The offer is ordered now, also the offer_fmt_prior */
		if (i >= offer->desc.fmt_count || 
		    offer_fmt_prior[i]-del_cnt != i)
		{
		    pj_str_t *fmt = &answer->desc.fmt[i];
		    pjmedia_sdp_attr *a;

		    /* Remove rtpmap associated with this format */
		    a = pjmedia_sdp_media_find_attr2(answer, "rtpmap", fmt);
		    if (a)
			pjmedia_sdp_media_remove_attr(answer, a);

		    /* Remove fmtp associated with this format */
		    a = pjmedia_sdp_media_find_attr2(answer, "fmtp", fmt);
		    if (a)
			pjmedia_sdp_media_remove_attr(answer, a);

		    /* Remove this format from answer's array */
		    pj_array_erase(answer->desc.fmt, 
				   sizeof(answer->desc.fmt[0]),
				   answer->desc.fmt_count, i);
		    --answer->desc.fmt_count;

		    ++del_cnt;
		} else {
		    ++i;
		}
	    }
	}
    }

    /* Looks okay */
    return PJ_SUCCESS;
}