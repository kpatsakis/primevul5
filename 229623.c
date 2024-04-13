HeaderLookupTable_t::initCache()
{
    idCache.resize(TOTAL_KEYWORDS);
    for (int j = MIN_HASH_VALUE; j <= MAX_HASH_VALUE; ++j) { //MAX_HASH_VALUE is exported by gperf
        if (HttpHeaderDefinitionsTable[j].name[0] != '\0') { //some slots are empty
            idCache[static_cast<int>(HttpHeaderDefinitionsTable[j].id)] =
                & HttpHeaderDefinitionsTable[j];
        }
    }
    //check after the fact. The cache array must be full
    for (auto e : idCache) {
        assert(e->name);
    }
}