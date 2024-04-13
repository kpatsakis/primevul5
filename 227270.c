findPreviouslyCachedEntry(StoreEntry *newEntry) {
    assert(newEntry->mem_obj);
    return newEntry->mem_obj->request ?
           storeGetPublicByRequest(newEntry->mem_obj->request) :
           storeGetPublic(newEntry->mem_obj->storeId(), newEntry->mem_obj->method);
}