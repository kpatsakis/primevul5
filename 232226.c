void wsrep_sst_received (wsrep_t* const      wsrep,
                         const wsrep_uuid_t& uuid,
                         wsrep_seqno_t const seqno,
                         const void* const   state,
                         size_t const        state_len)
{
    wsrep_get_SE_checkpoint(local_uuid, local_seqno);

    if (memcmp(&local_uuid, &uuid, sizeof(wsrep_uuid_t)) ||
        local_seqno < seqno || seqno < 0)
    {
        wsrep_set_SE_checkpoint(uuid, seqno);
        local_uuid = uuid;
        local_seqno = seqno;
    }
    else if (local_seqno > seqno)
    {
        WSREP_WARN("SST postion is in the past: %lld, current: %lld. "
                   "Can't continue.",
                   (long long)seqno, (long long)local_seqno);
        unireg_abort(1);
    }

    wsrep_init_sidno(uuid);

    if (wsrep)
    {
        int const rcode(seqno < 0 ? seqno : 0);
        wsrep_gtid_t const state_id = {
            uuid, (rcode ? WSREP_SEQNO_UNDEFINED : seqno)
        };

        wsrep->sst_received(wsrep, &state_id, state, state_len, rcode);
    }
}