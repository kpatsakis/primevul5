void ctrl_c_action(int sig) {
	LOG(level::DEBUG,"caught signal %d",sig);
	if (SIGINT == sig) {
		ctrl_c_hit = 1;
	} else {
		stfl::reset();
		utils::remove_fs_lock(lock_file);
		::exit(EXIT_FAILURE);
	}
}