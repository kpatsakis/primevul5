Value ExpressionReplaceOne::_doEval(StringData input,
                                    StringData find,
                                    StringData replacement) const {
    size_t startIndex = input.find(find);
    if (startIndex == std::string::npos) {
        return Value(StringData(input));
    }

    // An empty string matches at every position, so replaceOne should insert the replacement text
    // at position 0. input.find correctly returns position 0 when 'find' is empty, so we don't need
    // any special case to handle this.
    size_t endIndex = startIndex + find.size();
    StringBuilder output;
    output << input.substr(0, startIndex);
    output << replacement;
    output << input.substr(endIndex);
    return Value(output.stringData());
}