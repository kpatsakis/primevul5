TEST(RegexMatchExpression, MatchesScalar) {
    RegexMatchExpression regex("a", "b", "");
    ASSERT(regex.matchesBSON(BSON("a"
                                  << "b"),
                             NULL));
    ASSERT(!regex.matchesBSON(BSON("a"
                                   << "c"),
                              NULL));
}