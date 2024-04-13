 * recovery timer fires then FAST_IO_FAIL is returned. The caller
 * should pass this error value to the scsi eh.
 */
int iscsi_block_scsi_eh(struct scsi_cmnd *cmd)
{
	struct iscsi_cls_session *session =
			starget_to_session(scsi_target(cmd->device));
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&session->lock, flags);
	while (session->state != ISCSI_SESSION_LOGGED_IN) {
		if (session->state == ISCSI_SESSION_FREE) {
			ret = FAST_IO_FAIL;
			break;
		}
		spin_unlock_irqrestore(&session->lock, flags);
		msleep(1000);
		spin_lock_irqsave(&session->lock, flags);
	}