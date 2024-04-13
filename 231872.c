static pj_status_t match_offer(pj_pool_t *pool,
			       pj_bool_t prefer_remote_codec_order,
                               pj_bool_t answer_with_multiple_codecs,
			       const pjmedia_sdp_media *offer,
			       const pjmedia_sdp_media *preanswer,
			       const pjmedia_sdp_session *preanswer_sdp,
			       pjmedia_sdp_media **p_answer)
{
    unsigned i;
    pj_bool_t master_has_codec = 0,
	      master_has_other = 0,
	      found_matching_codec = 0,
	      found_matching_telephone_event = 0,
	      found_matching_other = 0;
    unsigned pt_answer_count = 0;
    pj_str_t pt_answer[PJMEDIA_MAX_SDP_FMT];
    pj_str_t pt_offer[PJMEDIA_MAX_SDP_FMT];
    pjmedia_sdp_media *answer;
    const pjmedia_sdp_media *master, *slave;
    unsigned nclockrate = 0, clockrate[PJMEDIA_MAX_SDP_FMT];
    unsigned ntel_clockrate = 0, tel_clockrate[PJMEDIA_MAX_SDP_FMT];

    /* If offer has zero port, just clone the offer */
    if (offer->desc.port == 0) {
	answer = sdp_media_clone_deactivate(pool, offer, preanswer,
					    preanswer_sdp);
	*p_answer = answer;
	return PJ_SUCCESS;
    }

    /* If the preanswer define zero port, this media is being rejected,
     * just clone the preanswer.
     */
    if (preanswer->desc.port == 0) {
	answer = pjmedia_sdp_media_clone(pool, preanswer);
	*p_answer = answer;
	return PJ_SUCCESS;
    }

    /* Set master/slave negotiator based on prefer_remote_codec_order. */
    if (prefer_remote_codec_order) {
	master = offer;
	slave  = preanswer;
    } else {
	master = preanswer;
	slave  = offer;
    }
    
    /* With the addition of telephone-event and dodgy MS RTC SDP, 
     * the answer generation algorithm looks really shitty...
     */
    for (i=0; i<master->desc.fmt_count; ++i) {
	unsigned j;
	
	if (pj_isdigit(*master->desc.fmt[i].ptr)) {
	    /* This is normal/standard payload type, where it's identified
	     * by payload number.
	     */
	    unsigned pt;

	    pt = pj_strtoul(&master->desc.fmt[i]);
	    
	    if (pt < 96) {
		/* For static payload type, it's enough to compare just
		 * the payload number.
		 */

		master_has_codec = 1;

		/* We just need to select one codec if not allowing multiple.
		 * Continue if we have selected matching codec for previous 
		 * payload.
		 */
		if (!answer_with_multiple_codecs && found_matching_codec)
		    continue;

		/* Find matching codec in local descriptor. */
		for (j=0; j<slave->desc.fmt_count; ++j) {
		    unsigned p;
		    p = pj_strtoul(&slave->desc.fmt[j]);
		    if (p == pt && pj_isdigit(*slave->desc.fmt[j].ptr)) {
			unsigned k;

			found_matching_codec = 1;
			pt_offer[pt_answer_count] = slave->desc.fmt[j];
			pt_answer[pt_answer_count++] = slave->desc.fmt[j];

			/* Take note of clock rate for tel-event. Note: for
			 * static PT, we assume the clock rate is 8000.
			 */
			for (k=0; k<nclockrate; ++k)
			    if (clockrate[k] == 8000)
				break;
			if (k == nclockrate)
			    clockrate[nclockrate++] = 8000;
			break;
		    }
		}

	    } else {
		/* This is dynamic payload type.
		 * For dynamic payload type, we must look the rtpmap and
		 * compare the encoding name.
		 */
		const pjmedia_sdp_attr *a;
		pjmedia_sdp_rtpmap or_;
		pj_bool_t is_codec = 0;

		/* Get the rtpmap for the payload type in the master. */
		a = pjmedia_sdp_media_find_attr2(master, "rtpmap", 
						 &master->desc.fmt[i]);
		if (!a) {
		    pj_assert(!"Bug! Offer should have been validated");
		    return PJMEDIA_SDP_EMISSINGRTPMAP;
		}
		pjmedia_sdp_attr_get_rtpmap(a, &or_);

		if (pj_stricmp2(&or_.enc_name, "telephone-event")) {
		    master_has_codec = 1;
		    if (!answer_with_multiple_codecs && found_matching_codec)
			continue;
		    is_codec = 1;
		}
		
		/* Find paylaod in our initial SDP with matching 
		 * encoding name and clock rate.
		 */
		for (j=0; j<slave->desc.fmt_count; ++j) {
		    a = pjmedia_sdp_media_find_attr2(slave, "rtpmap", 
						     &slave->desc.fmt[j]);
		    if (a) {
			pjmedia_sdp_rtpmap lr;
			pjmedia_sdp_attr_get_rtpmap(a, &lr);

			/* See if encoding name, clock rate, and
			 * channel count  match 
			 */
			if (!pj_stricmp(&or_.enc_name, &lr.enc_name) &&
			    or_.clock_rate == lr.clock_rate &&
			    (pj_stricmp(&or_.param, &lr.param)==0 ||
			     (lr.param.slen==0 && or_.param.slen==1 && 
						 *or_.param.ptr=='1') || 
			     (or_.param.slen==0 && lr.param.slen==1 && 
						  *lr.param.ptr=='1'))) 
			{
			    /* Match! */
			    if (is_codec) {
				pjmedia_sdp_media *o_med, *a_med;
				unsigned o_fmt_idx, a_fmt_idx;
				unsigned k;

				o_med = (pjmedia_sdp_media*)offer;
				a_med = (pjmedia_sdp_media*)preanswer;
				o_fmt_idx = prefer_remote_codec_order? i:j;
				a_fmt_idx = prefer_remote_codec_order? j:i;

				/* Call custom format matching callbacks */
				if (custom_fmt_match(pool, &or_.enc_name,
						     o_med, o_fmt_idx,
						     a_med, a_fmt_idx,
						     ALLOW_MODIFY_ANSWER) !=
				    PJ_SUCCESS)
				{
				    continue;
				}
				found_matching_codec = 1;

				/* Take note of clock rate for tel-event */
				for (k=0; k<nclockrate; ++k)
				    if (clockrate[k] == or_.clock_rate)
					break;
				if (k == nclockrate)
				    clockrate[nclockrate++] = or_.clock_rate;
			    } else {
			    	unsigned k;

				/* Keep track of tel-event clock rate,
				 * to prevent duplicate.
				 */
				for (k=0; k<ntel_clockrate; ++k)
				    if (tel_clockrate[k] == or_.clock_rate)
					break;
				if (k < ntel_clockrate)
				    continue;
				
				tel_clockrate[ntel_clockrate++] = or_.clock_rate;
				found_matching_telephone_event = 1;
			    }

			    pt_offer[pt_answer_count] = 
						prefer_remote_codec_order?
						offer->desc.fmt[i]:
						offer->desc.fmt[j];
			    pt_answer[pt_answer_count++] = 
						prefer_remote_codec_order? 
						preanswer->desc.fmt[j]:
						preanswer->desc.fmt[i];
			    break;
			}
		    }
		}
	    }

	} else {
	    /* This is a non-standard, brain damaged SDP where the payload
	     * type is non-numeric. It exists e.g. in Microsoft RTC based
	     * UA, to indicate instant messaging capability.
	     * Example:
	     *	- m=x-ms-message 5060 sip null
	     */
	    master_has_other = 1;
	    if (found_matching_other)
		continue;

	    for (j=0; j<slave->desc.fmt_count; ++j) {
		if (!pj_strcmp(&master->desc.fmt[i], &slave->desc.fmt[j])) {
		    /* Match */
		    found_matching_other = 1;
		    pt_offer[pt_answer_count] = prefer_remote_codec_order?
						offer->desc.fmt[i]:
						offer->desc.fmt[j];
		    pt_answer[pt_answer_count++] = prefer_remote_codec_order? 
						   preanswer->desc.fmt[j]:
						   preanswer->desc.fmt[i];
		    break;
		}
	    }
	}
    }

    /* See if all types of master can be matched. */
    if (master_has_codec && !found_matching_codec) {
	return PJMEDIA_SDPNEG_NOANSCODEC;
    }

    /* If this comment is removed, negotiation will fail if remote has offered
       telephone-event and local is not configured with telephone-event

    if (offer_has_telephone_event && !found_matching_telephone_event) {
	return PJMEDIA_SDPNEG_NOANSTELEVENT;
    }
    */

    if (master_has_other && !found_matching_other) {
	return PJMEDIA_SDPNEG_NOANSUNKNOWN;
    }

    /* Seems like everything is in order. */

    /* Remove unwanted telephone-event formats. */
    if (found_matching_telephone_event) {
	pj_str_t first_televent_offer = {0};
	pj_str_t first_televent_answer = {0};
	unsigned matched_cnt = 0;

	for (i=0; i<pt_answer_count; ) {
	    const pjmedia_sdp_attr *a;
	    pjmedia_sdp_rtpmap r;
	    unsigned j;

	    /* Skip static PT, as telephone-event uses dynamic PT */
	    if (!pj_isdigit(*pt_answer[i].ptr) || pj_strtol(&pt_answer[i])<96)
	    {
		++i;
		continue;
	    }

	    /* Get the rtpmap for format. */
	    a = pjmedia_sdp_media_find_attr2(preanswer, "rtpmap",
					     &pt_answer[i]);
	    pj_assert(a);
	    pjmedia_sdp_attr_get_rtpmap(a, &r);

	    /* Only care for telephone-event format */
	    if (pj_stricmp2(&r.enc_name, "telephone-event")) {
		++i;
		continue;
	    }

	    if (first_televent_offer.slen == 0) {
		first_televent_offer = pt_offer[i];
		first_televent_answer = pt_answer[i];
	    }

	    for (j=0; j<nclockrate; ++j) {
		if (r.clock_rate==clockrate[j])
		    break;
	    }

	    /* This tel-event's clockrate is unwanted, remove the tel-event */
	    if (j==nclockrate) {
		pj_array_erase(pt_answer, sizeof(pt_answer[0]),
			       pt_answer_count, i);
		pj_array_erase(pt_offer, sizeof(pt_offer[0]),
			       pt_answer_count, i);
		pt_answer_count--;
	    } else {
		++matched_cnt;
		++i;
	    }
	}

	/* Tel-event is wanted, but no matched clock rate (to the selected
	 * audio codec), just put back any first matched tel-event formats.
	 */
	if (!matched_cnt) {
	    pt_offer[pt_answer_count] = first_televent_offer;
	    pt_answer[pt_answer_count++] = first_televent_answer;
	}
    }

    /* Build the answer by cloning from preanswer, and reorder the payload
     * to suit the offer.
     */
    answer = pjmedia_sdp_media_clone(pool, preanswer);
    for (i=0; i<pt_answer_count; ++i) {
	unsigned j;
	for (j=i; j<answer->desc.fmt_count; ++j) {
	    if (!pj_strcmp(&answer->desc.fmt[j], &pt_answer[i]))
		break;
	}
	pj_assert(j != answer->desc.fmt_count);
	str_swap(&answer->desc.fmt[i], &answer->desc.fmt[j]);
    }
    
    /* Remove unwanted local formats. */
    for (i=pt_answer_count; i<answer->desc.fmt_count; ++i) {
	pjmedia_sdp_attr *a;

	/* Remove rtpmap for this format */
	a = pjmedia_sdp_media_find_attr2(answer, "rtpmap", 
					 &answer->desc.fmt[i]);
	if (a) {
	    pjmedia_sdp_media_remove_attr(answer, a);
	}

	/* Remove fmtp for this format */
	a = pjmedia_sdp_media_find_attr2(answer, "fmtp", 
					 &answer->desc.fmt[i]);
	if (a) {
	    pjmedia_sdp_media_remove_attr(answer, a);
	}
    }
    answer->desc.fmt_count = pt_answer_count;

#if PJMEDIA_SDP_NEG_ANSWER_SYMMETRIC_PT
    apply_answer_symmetric_pt(pool, answer, pt_answer_count,
			      pt_offer, pt_answer);
#endif

    /* Update media direction. */
    update_media_direction(pool, offer, answer);

    *p_answer = answer;
    return PJ_SUCCESS;
}