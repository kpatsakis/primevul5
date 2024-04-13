TEST(InMatchExpression, StringMatchingRespectsCollation) {
    BSONArray operand = BSON_ARRAY("string");
    BSONObj match = BSON("a"
                         << "string2");
    CollatorInterfaceMock collator(CollatorInterfaceMock::MockType::kAlwaysEqual);
    InMatchExpression in("");
    in.setCollator(&collator);
    std::vector<BSONElement> equalities{operand.firstElement()};
    ASSERT_OK(in.setEqualities(std::move(equalities)));
    ASSERT(in.matchesSingleElement(match["a"]));
}