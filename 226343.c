TEST_F(QueryPlannerTest, MergeSortEvenIfSameIndex) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProj(fromjson("{$or: [{a:1}, {a:7}]}"), fromjson("{b:1}"), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    // TODO the second solution should be mergeSort rather than just sort
}