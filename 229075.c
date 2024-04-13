void BitTestMatchExpression::debugString(StringBuilder& debug, int level) const {
    _debugAddSpace(debug, level);

    debug << path() << " ";

    switch (matchType()) {
        case BITS_ALL_SET:
            debug << "$bitsAllSet:";
            break;
        case BITS_ALL_CLEAR:
            debug << "$bitsAllClear:";
            break;
        case BITS_ANY_SET:
            debug << "$bitsAnySet:";
            break;
        case BITS_ANY_CLEAR:
            debug << "$bitsAnyClear:";
            break;
        default:
            MONGO_UNREACHABLE;
    }

    debug << " [";
    for (size_t i = 0; i < _bitPositions.size(); i++) {
        debug << _bitPositions[i];
        if (i != _bitPositions.size() - 1) {
            debug << ", ";
        }
    }
    debug << "]";

    MatchExpression::TagData* td = getTag();
    if (td) {
        debug << " ";
        td->debugString(&debug);
    }
}