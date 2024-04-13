void controller::start_reload_all_thread(std::vector<int> * indexes) {
	LOG(level::INFO,"starting reload all thread");
	std::thread t(downloadthread(this, indexes));
	t.detach();
}