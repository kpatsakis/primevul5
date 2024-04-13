void controller::update_config() {
	v->set_regexmanager(&rxman);
	v->update_bindings();

	if (colorman.colors_loaded()) {
		v->set_colors(colorman.get_fgcolors(), colorman.get_bgcolors(), colorman.get_attributes());
		v->apply_colors_to_all_formactions();
	}

	if (cfg.get_configvalue("error-log").length() > 0) {
		logger::getInstance().set_errorlogfile(cfg.get_configvalue("error-log"));
	}

}