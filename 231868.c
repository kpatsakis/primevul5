PJ_DEF(pj_status_t) pjmedia_sdp_neg_modify_local_offer( pj_pool_t *pool,
				    pjmedia_sdp_neg *neg,
				    const pjmedia_sdp_session *local)
{
    return pjmedia_sdp_neg_modify_local_offer2(pool, neg, 0, local);
}