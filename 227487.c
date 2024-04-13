static void notify_timeout(struct tevent_context *ev,
			   struct tevent_timer *te,
			   struct timeval current_time,
			   void *private_data)
{
	struct smb2_request *req = talloc_get_type_abort(
		private_data, struct smb2_request);

	smb2_cancel(req);
}