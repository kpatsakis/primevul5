TEST_F(QueryPlannerTest, IndexBoundsUnindexedSortHint) {
    addIndex(BSON("a" << 1));
    runQuerySortHint(fromjson("{$or: [{a: 1}, {a: 2}]}"), BSON("b" << 1), BSON("a" << 1));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sort: {pattern: {b:1}, limit: 0, node: {sortKeyGen: {node: {fetch: "
        "{filter: null, node: {ixscan: {filter: null, "
        "pattern: {a:1}, bounds: {a: [[1,1,true,true], [2,2,true,true]]}}}}}}}}}");
}