BSONObj BitTestMatchExpression::getSerializedRightHandSide() const {
    std::string opString = "";

    switch (matchType()) {
        case BITS_ALL_SET:
            opString = "$bitsAllSet";
            break;
        case BITS_ALL_CLEAR:
            opString = "$bitsAllClear";
            break;
        case BITS_ANY_SET:
            opString = "$bitsAnySet";
            break;
        case BITS_ANY_CLEAR:
            opString = "$bitsAnyClear";
            break;
        default:
            MONGO_UNREACHABLE;
    }

    BSONArrayBuilder arrBob;
    for (auto bitPosition : _bitPositions) {
        arrBob.append(bitPosition);
    }
    arrBob.doneFast();

    return BSON(opString << arrBob.arr());
}