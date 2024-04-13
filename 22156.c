bool controller::trylock_reload_mutex() {
	if (reload_mutex.try_lock()) {
		LOG(level::DEBUG, "controller::trylock_reload_mutex succeeded");
		return true;
	}
	LOG(level::DEBUG, "controller::trylock_reload_mutex failed");
	return false;
}