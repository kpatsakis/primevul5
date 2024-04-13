PJ_DEF(pj_status_t) pjmedia_sdp_neg_fmt_match(pj_pool_t *pool,
					      pjmedia_sdp_media *offer,
					      unsigned o_fmt_idx,
					      pjmedia_sdp_media *answer,
					      unsigned a_fmt_idx,
					      unsigned option)
{
    const pjmedia_sdp_attr *attr;
    pjmedia_sdp_rtpmap o_rtpmap, a_rtpmap;
    unsigned o_pt;
    unsigned a_pt;

    o_pt = pj_strtoul(&offer->desc.fmt[o_fmt_idx]);
    a_pt = pj_strtoul(&answer->desc.fmt[a_fmt_idx]);

    if (o_pt < 96 || a_pt < 96) {
	if (o_pt == a_pt)
	    return PJ_SUCCESS;
	else
	    return PJMEDIA_SDP_EFORMATNOTEQUAL;
    }

    /* Get the format rtpmap from the offer. */
    attr = pjmedia_sdp_media_find_attr2(offer, "rtpmap", 
					&offer->desc.fmt[o_fmt_idx]);
    if (!attr) {
	pj_assert(!"Bug! Offer haven't been validated");
	return PJ_EBUG;
    }
    pjmedia_sdp_attr_get_rtpmap(attr, &o_rtpmap);

    /* Get the format rtpmap from the answer. */
    attr = pjmedia_sdp_media_find_attr2(answer, "rtpmap", 
					&answer->desc.fmt[a_fmt_idx]);
    if (!attr) {
	pj_assert(!"Bug! Answer haven't been validated");
	return PJ_EBUG;
    }
    pjmedia_sdp_attr_get_rtpmap(attr, &a_rtpmap);

    if (pj_stricmp(&o_rtpmap.enc_name, &a_rtpmap.enc_name) != 0 ||
	(o_rtpmap.clock_rate != a_rtpmap.clock_rate) ||
	(!(pj_stricmp(&o_rtpmap.param, &a_rtpmap.param) == 0 ||
	   (a_rtpmap.param.slen == 0 && o_rtpmap.param.slen == 1 &&
	    *o_rtpmap.param.ptr == '1') ||
	   (o_rtpmap.param.slen == 0 && a_rtpmap.param.slen == 1 &&
	    *a_rtpmap.param.ptr=='1'))))
    {
	return PJMEDIA_SDP_EFORMATNOTEQUAL;
    }

    return custom_fmt_match(pool, &o_rtpmap.enc_name,
			    offer, o_fmt_idx, answer, a_fmt_idx, option);
}