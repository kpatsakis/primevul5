static void smb2cli_req_cancel_done(struct tevent_req *subreq)
{
	/* we do not care about the result */
	TALLOC_FREE(subreq);
}
