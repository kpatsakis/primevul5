TEST_F(QueryPlannerTest, InvalidUtf8CodePointDoesNotLeadToInvalidIndexBoundsInvariantFailure) {
    params.options &= ~QueryPlannerParams::INCLUDE_COLLSCAN;
    addIndex(BSON("a" << 1));

    // This UTF-8 is encoded correctly in the sense that it maps to a sequence of code points. The
    // code point 0x110000 is considered invalid. This does not result in an error because it does
    // not trigger a bounds building invariant.
    auto invalidCodePoint = std::string{"\xf4\x90\x80\x80"};
    auto findCommandWithInvalidCodepoint = BSON("find"
                                                << "testns"
                                                << "filter"
                                                << BSON("a" << BSON("$regex" << invalidCodePoint)));
    runQueryAsCommand(findCommandWithInvalidCodepoint);
}