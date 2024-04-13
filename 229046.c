TEST(InMatchExpression, ChangingCollationAfterAddingEqualitiesPreservesEqualities) {
    BSONObj obj1 = BSON(""
                        << "string1");
    BSONObj obj2 = BSON(""
                        << "string2");
    CollatorInterfaceMock collatorAlwaysEqual(CollatorInterfaceMock::MockType::kAlwaysEqual);
    CollatorInterfaceMock collatorReverseString(CollatorInterfaceMock::MockType::kReverseString);
    InMatchExpression in("");
    in.setCollator(&collatorAlwaysEqual);
    std::vector<BSONElement> equalities{obj1.firstElement(), obj2.firstElement()};
    ASSERT_OK(in.setEqualities(std::move(equalities)));
    ASSERT(in.getEqualities().size() == 1);
    in.setCollator(&collatorReverseString);
    ASSERT(in.getEqualities().size() == 2);
    ASSERT(in.getEqualities().count(obj1.firstElement()));
    ASSERT(in.getEqualities().count(obj2.firstElement()));
}