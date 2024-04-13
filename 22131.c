void controller::execute_commands(char ** argv, unsigned int i) {
	if (v->formaction_stack_size() > 0)
		v->pop_current_formaction();
	for (; argv[i]; ++i) {
		LOG(level::DEBUG, "controller::execute_commands: executing `%s'", argv[i]);
		std::string cmd(argv[i]);
		if (cmd == "reload") {
			reload_all(true);
		} else if (cmd == "print-unread") {
			std::cout << strprintf::fmt(_("%u unread articles"), rsscache->get_unread_count()) << std::endl;
		} else {
			std::cerr << strprintf::fmt(_("%s: %s: unknown command"), argv[0], argv[i]) << std::endl;
			::std::exit(EXIT_FAILURE);
		}
	}
}