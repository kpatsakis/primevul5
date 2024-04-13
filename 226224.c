TEST_F(QueryPlannerTest, CantUseHashedIndexToProvideSort) {
    addIndex(BSON("x"
                  << "hashed"));
    runQuerySortProj(BSONObj(), BSON("x" << 1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists(
        "{sort: {pattern: {x: 1}, limit: 0, node: {sortKeyGen:"
        "{node: {cscan: {dir: 1, filter: {}}}}}}}");
}