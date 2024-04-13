Lockable* DynamicMetadataProvider::lock()
{
    m_lock->rdlock();
    return this;
}
