 * This can be called from a LLD or iscsi_transport.
 */
struct iscsi_cls_session *
iscsi_create_session(struct Scsi_Host *shost, struct iscsi_transport *transport,
		     int dd_size, unsigned int target_id)
{
	struct iscsi_cls_session *session;

	session = iscsi_alloc_session(shost, transport, dd_size);
	if (!session)
		return NULL;

	if (iscsi_add_session(session, target_id)) {
		iscsi_free_session(session);
		return NULL;