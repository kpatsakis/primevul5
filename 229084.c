TEST(RegexMatchExpression, RegexAcceptsUCPOption) {
    RegexMatchExpression regex("a", "(*UCP)(\\w|\u304C)", "");
    ASSERT(regex.matchesBSON(BSON("a"
                                  << "k")));
    ASSERT(regex.matchesBSON(BSON("a"
                                  << "\u304B")));
    ASSERT(regex.matchesBSON(BSON("a"
                                  << "\u304C")));
}