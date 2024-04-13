static void sctp_sendmsg_update_sinfo(struct sctp_association *asoc,
				      struct sctp_sndrcvinfo *sinfo,
				      struct sctp_cmsgs *cmsgs)
{
	if (!cmsgs->srinfo && !cmsgs->sinfo) {
		sinfo->sinfo_stream = asoc->default_stream;
		sinfo->sinfo_ppid = asoc->default_ppid;
		sinfo->sinfo_context = asoc->default_context;
		sinfo->sinfo_assoc_id = sctp_assoc2id(asoc);

		if (!cmsgs->prinfo)
			sinfo->sinfo_flags = asoc->default_flags;
	}

	if (!cmsgs->srinfo && !cmsgs->prinfo)
		sinfo->sinfo_timetolive = asoc->default_timetolive;

	if (cmsgs->authinfo) {
		/* Reuse sinfo_tsn to indicate that authinfo was set and
		 * sinfo_ssn to save the keyid on tx path.
		 */
		sinfo->sinfo_tsn = 1;
		sinfo->sinfo_ssn = cmsgs->authinfo->auth_keynumber;
	}
}