Value ExpressionReplaceAll::_doEval(StringData input,
                                    StringData find,
                                    StringData replacement) const {
    // An empty string matches at every position, so replaceAll should insert 'replacement' at every
    // position when 'find' is empty. Handling this as a special case lets us assume 'find' is
    // nonempty in the usual case.
    if (find.size() == 0) {
        StringBuilder output;
        for (char c : input) {
            output << replacement << c;
        }
        output << replacement;
        return Value(output.stringData());
    }

    StringBuilder output;
    for (;;) {
        size_t startIndex = input.find(find);
        if (startIndex == std::string::npos) {
            output << input;
            break;
        }

        size_t endIndex = startIndex + find.size();
        output << input.substr(0, startIndex);
        output << replacement;
        // This step assumes 'find' is nonempty. If 'find' were empty then input.find would always
        // find a match at position 0, and the input would never shrink.
        input = input.substr(endIndex);
    }
    return Value(output.stringData());
}