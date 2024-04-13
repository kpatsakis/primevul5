TEST(RegexMatchExpression, MatchesArrayValue) {
    RegexMatchExpression regex("a", "b", "");
    ASSERT(regex.matchesBSON(BSON("a" << BSON_ARRAY("c"
                                                    << "b")),
                             NULL));
    ASSERT(!regex.matchesBSON(BSON("a" << BSON_ARRAY("d"
                                                     << "c")),
                              NULL));
}