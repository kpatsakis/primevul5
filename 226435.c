TEST_F(QueryPlannerTest, CantUseHashedIndexToProvideSortWithIndexablePred) {
    addIndex(BSON("x"
                  << "hashed"));
    runQuerySortProj(BSON("x" << BSON("$in" << BSON_ARRAY(0 << 1))), BSON("x" << 1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{sort: {pattern: {x: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{fetch: {node: "
        "{ixscan: {pattern: {x: 'hashed'}}}}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {x: 1}, limit: 0, node: {sortKeyGen: {node:"
        "{cscan: {dir: 1, filter: {x: {$in: [0, 1]}}}}}}}}");
}