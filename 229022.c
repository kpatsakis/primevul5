TEST(InMatchExpression, StringMatchingWithNullCollatorUsesBinaryComparison) {
    BSONArray operand = BSON_ARRAY("string");
    BSONObj notMatch = BSON("a"
                            << "string2");
    InMatchExpression in("");
    std::vector<BSONElement> equalities{operand.firstElement()};
    ASSERT_OK(in.setEqualities(std::move(equalities)));
    ASSERT(!in.matchesSingleElement(notMatch["a"]));
}