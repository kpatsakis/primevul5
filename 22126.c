void controller::notify(const std::string& msg) {
	if (cfg.get_configvalue_as_bool("notify-screen")) {
		LOG(level::DEBUG, "controller:notify: notifying screen");
		std::cout << "\033^" << msg << "\033\\";
		std::cout.flush();
	}
	if (cfg.get_configvalue_as_bool("notify-xterm")) {
		LOG(level::DEBUG, "controller:notify: notifying xterm");
		std::cout << "\033]2;" << msg << "\033\\";
		std::cout.flush();
	}
	if (cfg.get_configvalue_as_bool("notify-beep")) {
		LOG(level::DEBUG, "controller:notify: notifying beep");
		::beep();
	}
	if (cfg.get_configvalue("notify-program").length() > 0) {
		std::string prog = cfg.get_configvalue("notify-program");
		LOG(level::DEBUG, "controller:notify: notifying external program `%s'", prog);
		utils::run_command(prog, msg);
	}
}