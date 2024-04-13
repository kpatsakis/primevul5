filter_protocol_next(uint64_t token, uint64_t reqid, enum filter_phase phase)
{
	struct filter_session  *fs;

	/* session can legitimately disappear on a resume */
	if ((fs = tree_get(&sessions, reqid)) == NULL)
		return;

	filter_protocol_internal(fs, &token, reqid, phase, fs->lastparam);
}