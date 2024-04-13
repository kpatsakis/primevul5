void ignore_signal(int sig) {
	LOG(level::WARN, "caught signal %d but ignored it", sig);
}