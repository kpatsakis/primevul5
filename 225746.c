mptcp_analysis_dsn_lookup(packet_info *pinfo , tvbuff_t *tvb,
    proto_tree *parent_tree, struct tcp_analysis* tcpd, struct tcpheader * tcph, mptcp_per_packet_data_t *mptcppd)
{
    struct mptcp_analysis* mptcpd = tcpd->mptcp_analysis;
    proto_item *item = NULL;
    mptcp_dss_mapping_t *mapping = NULL;
    guint32 relseq;
    guint64 rawdsn = 0;
    enum mptcp_dsn_conversion convert;

    if(!mptcp_analyze_mappings)
    {
        /* abort analysis */
        return;
    }

    /* for this to work, we need to know the original seq number from the SYN, not from a subsequent packet
    * hence, we abort if we didn't capture the SYN
    */
    if(!(tcpd->fwd->static_flags & ~TCP_S_BASE_SEQ_SET & (TCP_S_SAW_SYN | TCP_S_SAW_SYNACK))) {
        return;
    }

    /* if seq not relative yet, we compute it */
    relseq = (tcp_relative_seq) ? tcph->th_seq : tcph->th_seq - tcpd->fwd->base_seq;

    DISSECTOR_ASSERT(mptcpd);
    DISSECTOR_ASSERT(mptcppd);

    /* in case of a SYN, there is no mapping covering the DSN */
    if(tcph->th_flags & TH_SYN) {

        rawdsn = tcpd->fwd->mptcp_subflow->meta->base_dsn;
        convert = DSN_CONV_NONE;
    }
    /* if it's a non-syn packet without data (just used to convey TCP options)
     * then there would be no mappings */
    else if(relseq == 1 && tcph->th_seglen == 0) {
        rawdsn = tcpd->fwd->mptcp_subflow->meta->base_dsn + 1;
        convert = DSN_CONV_NONE;
    }
    else {

        wmem_list_frame_t *dss_it = NULL;
        wmem_list_t *results = NULL;
        guint32 ssn_low = relseq;
        guint32 seglen = tcph->th_seglen;

        results = wmem_itree_find_intervals(tcpd->fwd->mptcp_subflow->ssn2dsn_mappings,
                    wmem_packet_scope(),
                    ssn_low,
                    (seglen) ? ssn_low + seglen - 1 : ssn_low
                    );
        dss_it = wmem_list_head(results); /* assume it's always ok */
        if(dss_it) {
            mapping = (mptcp_dss_mapping_t *) wmem_list_frame_data(dss_it);
        }
        if(dss_it == NULL || mapping == NULL) {
            expert_add_info(pinfo, parent_tree, &ei_mptcp_mapping_missing);
            return;
        }
        else {
            mptcppd->mapping = mapping;
        }

        DISSECTOR_ASSERT(mapping);
        if(seglen) {
            /* Finds mappings that cover the sent data and adds them to the dissection tree */
            for(dss_it = wmem_list_head(results);
                dss_it != NULL;
                dss_it = wmem_list_frame_next(dss_it))
            {
                mapping = (mptcp_dss_mapping_t *) wmem_list_frame_data(dss_it);
                DISSECTOR_ASSERT(mapping);

                item = proto_tree_add_uint(parent_tree, hf_mptcp_related_mapping, tvb, 0, 0, mapping->frame);
                PROTO_ITEM_SET_GENERATED(item);
            }
        }

        convert = (mapping->extended_dsn) ? DSN_CONV_NONE : DSN_CONV_32_TO_64;
        DISSECTOR_ASSERT(mptcp_map_relssn_to_rawdsn(mapping, relseq, &rawdsn));
    }

    /* Make sure we have the 64bit raw DSN */
    if(mptcp_convert_dsn(rawdsn, tcpd->fwd->mptcp_subflow->meta,
        convert, FALSE, &tcph->th_mptcp->mh_rawdsn64)) {

        /* always display the rawdsn64 (helpful for debug) */
        item = proto_tree_add_uint64(parent_tree, hf_mptcp_rawdsn64, tvb, 0, 0, tcph->th_mptcp->mh_rawdsn64);

        /* converts to relative if required */
        if (mptcp_relative_seq
            && mptcp_convert_dsn(tcph->th_mptcp->mh_rawdsn64, tcpd->fwd->mptcp_subflow->meta, DSN_CONV_NONE, TRUE, &tcph->th_mptcp->mh_dsn)) {
            item = proto_tree_add_uint64(parent_tree, hf_mptcp_dsn, tvb, 0, 0, tcph->th_mptcp->mh_dsn);
            proto_item_append_text(item, " (Relative)");
        }

        /* register dsn->packet mapping */
        if(mptcp_intersubflows_retransmission
            && !PINFO_FD_VISITED(pinfo)
            && tcph->th_seglen > 0
          ) {
                mptcp_dsn2packet_mapping_t *packet = 0;
                packet = wmem_new0(wmem_file_scope(), mptcp_dsn2packet_mapping_t);
                packet->frame = pinfo->fd->num;
                packet->subflow = tcpd;

                wmem_itree_insert(tcpd->fwd->mptcp_subflow->dsn2packet_map,
                        tcph->th_mptcp->mh_rawdsn64,
                        tcph->th_mptcp->mh_rawdsn64 + (tcph->th_seglen - 1 ),
                        packet
                        );
        }
        PROTO_ITEM_SET_GENERATED(item);

        /* We can do this only if rawdsn64 is valid !
        if enabled, look for overlapping mappings on other subflows */
        if(mptcp_intersubflows_retransmission
            && tcph->th_have_seglen
            && tcph->th_seglen) {

            wmem_list_frame_t *subflow_it = NULL;

            /* results should be some kind of list in case 2 DSS are needed to cover this packet */
            for(subflow_it = wmem_list_head(mptcpd->subflows); subflow_it != NULL; subflow_it = wmem_list_frame_next(subflow_it)) {
                struct tcp_analysis *sf_tcpd = (struct tcp_analysis *)wmem_list_frame_data(subflow_it);
                struct mptcp_subflow *sf = mptcp_select_subflow_from_meta(sf_tcpd, tcpd->fwd->mptcp_subflow->meta);

                /* for current subflow */
                if (sf == tcpd->fwd->mptcp_subflow) {
                    /* skip, this is the current subflow */
                }
                /* in case there were retransmissions on other subflows */
                else  {
                    mptcp_add_duplicated_dsn(pinfo, parent_tree, tvb, sf,
                                             tcph->th_mptcp->mh_rawdsn64,
                                             tcph->th_mptcp->mh_rawdsn64 + tcph->th_seglen-1);
                }
            }
        }
    }
    else {
        /* could not get the rawdsn64, ignore and continue */
    }

}