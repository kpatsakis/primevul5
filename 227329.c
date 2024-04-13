HttpStateData::ReuseDecision::ReuseDecision(const StoreEntry *e, const Http::StatusCode code)
    : answer(HttpStateData::ReuseDecision::reuseNot), reason(nullptr), entry(e), statusCode(code) {}