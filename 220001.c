int headerIsSourceHeuristic(Header h)
{
    indexEntry entry = findEntry(h, RPMTAG_DIRNAMES, RPM_STRING_ARRAY_TYPE);
    return entry && entry->info.count == 1 && entry->data && !*(const char *)entry->data;
}