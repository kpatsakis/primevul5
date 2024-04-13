static bool test_valid_request(struct torture_context *torture,
			       struct smb2_tree *tree)
{
	bool ret = true;
	NTSTATUS status;
	struct smb2_handle dh;
	struct smb2_notify n;
	struct smb2_request *req;
	uint32_t max_buffer_size;

	torture_comment(torture, "TESTING VALIDITY OF CHANGE NOTIFY REQUEST\n");

	smb2_transport_credits_ask_num(tree->session->transport, 256);

	smb2_util_unlink(tree, FNAME);

	status = smb2_util_roothandle(tree, &dh);
	CHECK_STATUS(status, NT_STATUS_OK);

	max_buffer_size =
		smb2cli_conn_max_trans_size(tree->session->transport->conn);

	n.in.recursive		= 0x0000;
	n.in.buffer_size	= max_buffer_size;
	n.in.file.handle	= dh;
	n.in.completion_filter	= FILE_NOTIFY_CHANGE_ALL;
	n.in.unknown		= 0x00000000;
	req = smb2_notify_send(tree, &n);

	while (!req->cancel.can_cancel && req->state <= SMB2_REQUEST_RECV) {
		if (tevent_loop_once(torture->ev) != 0) {
			break;
		}
	}

	status = torture_setup_simple_file(torture, tree, FNAME);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &n);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(n.out.num_changes, 1);
	CHECK_VAL(n.out.changes[0].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(n.out.changes[0].name, FNAME);

	/*
	 * if the change response doesn't fit in the buffer
	 * NOTIFY_ENUM_DIR is returned.
	 */
	n.in.buffer_size	= 0x00000000;
	req = smb2_notify_send(tree, &n);

	while (!req->cancel.can_cancel && req->state <= SMB2_REQUEST_RECV) {
		if (tevent_loop_once(torture->ev) != 0) {
			break;
		}
	}

	status = torture_setup_simple_file(torture, tree, FNAME);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &n);
	CHECK_STATUS(status, STATUS_NOTIFY_ENUM_DIR);

	/*
	 * if the change response fits in the buffer we get
	 * NT_STATUS_OK again
	 */
	n.in.buffer_size	= max_buffer_size;
	req = smb2_notify_send(tree, &n);

	while (!req->cancel.can_cancel && req->state <= SMB2_REQUEST_RECV) {
		if (tevent_loop_once(torture->ev) != 0) {
			break;
		}
	}

	status = torture_setup_simple_file(torture, tree, FNAME);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &n);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(n.out.num_changes, 3);
	CHECK_VAL(n.out.changes[0].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(n.out.changes[0].name, FNAME);
	CHECK_VAL(n.out.changes[1].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(n.out.changes[1].name, FNAME);
	CHECK_VAL(n.out.changes[2].action, NOTIFY_ACTION_MODIFIED);
	CHECK_WIRE_STR(n.out.changes[2].name, FNAME);

	/* if the first notify returns NOTIFY_ENUM_DIR, all do */
	status = smb2_util_close(tree, dh);
	CHECK_STATUS(status, NT_STATUS_OK);
	status = smb2_util_roothandle(tree, &dh);
	CHECK_STATUS(status, NT_STATUS_OK);

	n.in.recursive		= 0x0000;
	n.in.buffer_size	= 0x00000001;
	n.in.file.handle	= dh;
	n.in.completion_filter	= FILE_NOTIFY_CHANGE_ALL;
	n.in.unknown		= 0x00000000;
	req = smb2_notify_send(tree, &n);

	while (!req->cancel.can_cancel && req->state <= SMB2_REQUEST_RECV) {
		if (tevent_loop_once(torture->ev) != 0) {
			break;
		}
	}

	status = torture_setup_simple_file(torture, tree, FNAME);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &n);
	CHECK_STATUS(status, STATUS_NOTIFY_ENUM_DIR);

	n.in.buffer_size        = max_buffer_size;
	req = smb2_notify_send(tree, &n);
	while (!req->cancel.can_cancel && req->state <= SMB2_REQUEST_RECV) {
		if (tevent_loop_once(torture->ev) != 0) {
			break;
		}
	}

	status = torture_setup_simple_file(torture, tree, FNAME);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &n);
	CHECK_STATUS(status, STATUS_NOTIFY_ENUM_DIR);

	/* if the buffer size is too large, we get invalid parameter */
	n.in.recursive		= 0x0000;
	n.in.buffer_size	= max_buffer_size + 1;
	n.in.file.handle	= dh;
	n.in.completion_filter	= FILE_NOTIFY_CHANGE_ALL;
	n.in.unknown		= 0x00000000;
	req = smb2_notify_send(tree, &n);
	status = smb2_notify_recv(req, torture, &n);
	CHECK_STATUS(status, NT_STATUS_INVALID_PARAMETER);

done:
	return ret;
}