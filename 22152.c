void controller::load_configfile(const std::string& filename) {
	if (cfgparser.parse(filename, true)) {
		update_config();
	} else {
		v->show_error(strprintf::fmt(_("Error: couldn't open configuration file `%s'!"), filename));
	}
}