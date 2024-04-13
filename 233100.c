sc_get_driver(void)
{
	struct sc_card_driver *iso_drv = sc_get_iso7816_driver();

	if (!iso_ops)
		iso_ops = iso_drv->ops;

	iasecc_ops = *iso_ops;

	iasecc_ops.match_card = iasecc_match_card;
	iasecc_ops.init = iasecc_init;
	iasecc_ops.finish = iasecc_finish;
	iasecc_ops.read_binary = iasecc_read_binary;
	/*	write_binary: ISO7816 implementation works	*/
	/*	update_binary: ISO7816 implementation works	*/
	iasecc_ops.erase_binary = iasecc_erase_binary;
	/*	resize_binary	*/
	/* 	read_record: Untested	*/
	/*	write_record: Untested	*/
	/*	append_record: Untested	*/
	/*	update_record: Untested	*/
	iasecc_ops.select_file = iasecc_select_file;
	/*	get_response: Untested	*/
	/*	get_challenge: ISO7816 implementation works	*/
	iasecc_ops.logout = iasecc_logout;
	/*	restore_security_env	*/
	iasecc_ops.set_security_env = iasecc_set_security_env;
	iasecc_ops.decipher = iasecc_decipher;
	iasecc_ops.compute_signature = iasecc_compute_signature;
	iasecc_ops.create_file = iasecc_create_file;
	iasecc_ops.delete_file = iasecc_delete_file;
	/*	list_files	*/
	iasecc_ops.check_sw = iasecc_check_sw;
	iasecc_ops.card_ctl = iasecc_card_ctl;
	iasecc_ops.process_fci = iasecc_process_fci;
	/*	construct_fci: Not needed	*/
	iasecc_ops.pin_cmd = iasecc_pin_cmd;
	/*	get_data: Not implemented	*/
	/*	put_data: Not implemented	*/
	/*	delete_record: Not implemented	*/

	iasecc_ops.read_public_key = iasecc_read_public_key;

	return &iasecc_drv;
}