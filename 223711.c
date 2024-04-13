Value ExpressionRegex::nextMatch(RegexExecutionState* regexState) const {
    int execResult = execute(regexState);

    // No match.
    if (execResult < 0) {
        return Value(BSONNULL);
    }

    // Use 'input' as StringData throughout the function to avoid copying the string on 'substr'
    // calls.
    StringData input = *(regexState->input);

    auto verifyBounds = [&input, this](auto startPos, auto limitPos, auto isCapture) {
        // If a capture group was not matched, then the 'startPos' and 'limitPos' will both be -1.
        // These bounds cannot occur for a match on the full string.
        if (startPos == -1 || limitPos == -1) {
            massert(31304,
                    str::stream() << "Unexpected error occurred while executing " << _opName
                                  << ". startPos: " << startPos << ", limitPos: " << limitPos,
                    isCapture && startPos == -1 && limitPos == -1);
            return;
        }

        massert(31305,
                str::stream() << "Unexpected error occurred while executing " << _opName
                              << ". startPos: " << startPos,
                (startPos >= 0 && static_cast<size_t>(startPos) <= input.size()));
        massert(31306,
                str::stream() << "Unexpected error occurred while executing " << _opName
                              << ". limitPos: " << limitPos,
                (limitPos >= 0 && static_cast<size_t>(limitPos) <= input.size()));
        massert(31307,
                str::stream() << "Unexpected error occurred while executing " << _opName
                              << ". startPos: " << startPos << ", limitPos: " << limitPos,
                startPos <= limitPos);
    };

    // The first and second entries of the 'capturesBuffer' will have the start and (end+1) indices
    // of the matched string, as byte offsets. '(limit - startIndex)' would be the length of the
    // captured string.
    verifyBounds(regexState->capturesBuffer[0], regexState->capturesBuffer[1], false);
    const int matchStartByteIndex = regexState->capturesBuffer[0];
    StringData matchedStr =
        input.substr(matchStartByteIndex, regexState->capturesBuffer[1] - matchStartByteIndex);

    // We iterate through the input string's contents preceding the match index, in order to convert
    // the byte offset to a code point offset.
    for (int byteIx = regexState->startBytePos; byteIx < matchStartByteIndex;
         ++(regexState->startCodePointPos)) {
        byteIx += getCodePointLength(input[byteIx]);
    }

    // Set the start index for match to the new one.
    regexState->startBytePos = matchStartByteIndex;

    std::vector<Value> captures;
    captures.reserve(regexState->numCaptures);

    // The next '2 * numCaptures' entries (after the first two entries) of 'capturesBuffer' will
    // hold the start index and limit pairs, for each of the capture groups. We skip the first two
    // elements and start iteration from 3rd element so that we only construct the strings for
    // capture groups.
    for (int i = 0; i < regexState->numCaptures; ++i) {
        const int start = regexState->capturesBuffer[2 * (i + 1)];
        const int limit = regexState->capturesBuffer[2 * (i + 1) + 1];
        verifyBounds(start, limit, true);

        // The 'start' and 'limit' will be set to -1, if the 'input' didn't match the current
        // capture group. In this case we put a 'null' placeholder in place of the capture group.
        captures.push_back(start == -1 && limit == -1 ? Value(BSONNULL)
                                                      : Value(input.substr(start, limit - start)));
    }

    MutableDocument match;
    match.addField("match", Value(matchedStr));
    match.addField("idx", Value(regexState->startCodePointPos));
    match.addField("captures", Value(captures));
    return match.freezeToValue();
}