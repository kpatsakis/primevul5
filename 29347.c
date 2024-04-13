DynamicMetadataProvider::~DynamicMetadataProvider()
{
    clearDescriptorIndex(true);

    if (m_cleanup_thread) {
        m_shutdown = true;
        m_cleanup_wait->signal();
        m_cleanup_thread->join(nullptr);
        delete m_cleanup_thread;
        delete m_cleanup_wait;
        m_cleanup_thread = nullptr;
        m_cleanup_wait = nullptr;
    }
}
