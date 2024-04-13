void controller::reload_range(unsigned int start, unsigned int end, unsigned int size, bool unattended) {

	std::vector<unsigned int> v;
	for (unsigned int i=start; i<=end; ++i)
		v.push_back(i);

	auto extract = [](std::string& s, const std::string& url) {
		size_t p = url.find("//");
		p = (p == std::string::npos) ? 0 : p+2;
		std::string suff(url.substr(p));
		p = suff.find('/');
		s = suff.substr(0, p);
	};

	std::sort(v.begin(), v.end(), [&](unsigned int a, unsigned int b) {
		std::string domain1, domain2;
		extract(domain1, feeds[a]->rssurl());
		extract(domain2, feeds[b]->rssurl());
		std::reverse(domain1.begin(), domain1.end());
		std::reverse(domain2.begin(), domain2.end());
		return domain1 < domain2;
	});

	curl_handle easyhandle;

	for (auto i : v) {
		LOG(level::DEBUG, "controller::reload_range: reloading feed #%u", i);
		this->reload(i, size, unattended, &easyhandle);
	}
}