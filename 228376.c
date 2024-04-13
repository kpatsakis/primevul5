Http::Http(ECChannel *lpChannel, std::shared_ptr<ECConfig> lpConfig) :
	m_lpChannel(lpChannel), m_lpConfig(std::move(lpConfig))
{}