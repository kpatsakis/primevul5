pair<const EntityDescriptor*,const RoleDescriptor*> DynamicMetadataProvider::getEntityDescriptor(const Criteria& criteria) const
{
    Category& log = Category::getInstance(SAML_LOGCAT ".MetadataProvider.Dynamic");

    bool writeLocked = false;

    pair<const EntityDescriptor*,const RoleDescriptor*> entity = AbstractMetadataProvider::getEntityDescriptor(criteria);

    cachemap_t::iterator cit;
    if (entity.first) {
        cit = m_cacheMap.find(entity.first->getEntityID());
    }
    else if (criteria.entityID_ascii) {
        auto_ptr_XMLCh widetemp(criteria.entityID_ascii);
        cit = m_cacheMap.find(widetemp.get());
    }
    else if (criteria.entityID_unicode) {
        cit = m_cacheMap.find(criteria.entityID_unicode);
    }
    else if (criteria.artifact) {
        auto_ptr_XMLCh widetemp(criteria.artifact->getSource().c_str());
        cit = m_cacheMap.find(widetemp.get());
    }
    else {
        cit = m_cacheMap.end();
    }
    if (cit != m_cacheMap.end()) {
        if (time(nullptr) <= cit->second)
            return entity;
    }

    string name;
    if (criteria.entityID_ascii) {
        name = criteria.entityID_ascii;
    }
    else if (criteria.entityID_unicode) {
        auto_ptr_char temp(criteria.entityID_unicode);
        name = temp.get();
    }
    else if (criteria.artifact) {
        name = criteria.artifact->getSource();
    }
    else {
        return entity;
    }

    if (entity.first)
        log.info("metadata for (%s) is beyond caching interval, attempting to refresh", name.c_str());
    else
        log.info("resolving metadata for (%s)", name.c_str());

    try {
        auto_ptr<EntityDescriptor> entity2(resolve(criteria));

        if (criteria.entityID_unicode && !XMLString::equals(criteria.entityID_unicode, entity2->getEntityID())) {
            log.error("metadata instance did not match expected entityID");
            return entity;
        }
        else if (criteria.artifact) {
            auto_ptr_char temp2(entity2->getEntityID());
            const string hashed(SecurityHelper::doHash("SHA1", temp2.get(), strlen(temp2.get()), true));
            if (hashed != name) {
                log.error("metadata instance did not match expected entityID");
                return entity;
            }
        }
        else {
            auto_ptr_XMLCh temp2(name.c_str());
            if (!XMLString::equals(temp2.get(), entity2->getEntityID())) {
                log.error("metadata instance did not match expected entityID");
                return entity;
            }
        }

        try {
            SchemaValidators.validate(entity2.get());
        }
        catch (exception& ex) {
            log.error("metadata instance failed manual validation checking: %s", ex.what());
            throw MetadataException("Metadata instance failed manual validation checking.");
        }

        doFilters(*entity2);

        time_t now = time(nullptr);
        time_t cmp = now;
        if (cmp < (std::numeric_limits<int>::max() - 60))
            cmp += 60;
        if (entity2->getValidUntil() && entity2->getValidUntilEpoch() < cmp)
            throw MetadataException("Metadata was already invalid at the time of retrieval.");

        log.info("caching resolved metadata for (%s)", name.c_str());

        time_t cacheExp = (entity2->getValidUntil() ? entity2->getValidUntilEpoch() : SAMLTIME_MAX) - now;
        if (entity2->getCacheDuration())
            cacheExp = min(cacheExp, entity2->getCacheDurationEpoch());
            
        cacheExp *= m_refreshDelayFactor;

        if (cacheExp > m_maxCacheDuration)
            cacheExp = m_maxCacheDuration;
        else if (cacheExp < m_minCacheDuration)
            cacheExp = m_minCacheDuration;

        log.info("next refresh of metadata for (%s) no sooner than %u seconds", name.c_str(), cacheExp);

        m_lock->unlock();
        m_lock->wrlock();
        writeLocked = true;

        emitChangeEvent(*entity2);

        m_cacheMap[entity2->getEntityID()] = now + cacheExp;

        cacheExp = SAMLTIME_MAX;
        unindex(entity2->getEntityID(), true);  // actually frees the old instance with this ID
        indexEntity(entity2.get(), cacheExp);
        entity2.release();

        m_lastUpdate = now;
    }
    catch (exception& e) {
        log.error("error while resolving entityID (%s): %s", name.c_str(), e.what());
        if (!writeLocked) {
            m_lock->unlock();
            m_lock->wrlock();
            writeLocked = true;
        }
        if (entity.first)
            m_cacheMap[entity.first->getEntityID()] = time(nullptr) + m_minCacheDuration;
        else if (criteria.entityID_unicode)
            m_cacheMap[criteria.entityID_unicode] = time(nullptr) + m_minCacheDuration;
        else {
            auto_ptr_XMLCh widetemp(name.c_str());
            m_cacheMap[widetemp.get()] = time(nullptr) + m_minCacheDuration;
        }
        log.warn("next refresh of metadata for (%s) no sooner than %u seconds", name.c_str(), m_minCacheDuration);
        return entity;
    }

    if (writeLocked) {
        m_lock->unlock();
        m_lock->rdlock();
    }

    return getEntityDescriptor(criteria);
}
