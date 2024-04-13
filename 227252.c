std::ostream &operator <<(std::ostream &os, const HttpStateData::ReuseDecision &d)
{
    static const char *ReuseMessages[] = {
        "do not cache and do not share", // reuseNot
        "cache positively and share", // cachePositively
        "cache negatively and share", // cacheNegatively
        "do not cache but share" // doNotCacheButShare
    };

    assert(d.answer >= HttpStateData::ReuseDecision::reuseNot &&
           d.answer <= HttpStateData::ReuseDecision::doNotCacheButShare);
    return os << ReuseMessages[d.answer] << " because " << d.reason <<
           "; HTTP status " << d.statusCode << " " << *(d.entry);
}