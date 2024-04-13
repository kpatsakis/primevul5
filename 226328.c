TEST_F(QueryPlannerTest, IndexBoundsUnindexedSort) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{$or: [{a: 1}, {a: 2}]}"), BSON("b" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {b:1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {filter: {$or:[{a:1},{a:2}]}, dir: 1}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {b:1}, limit: 0, node: {sortKeyGen: {node: {fetch: "
        "{filter: null, node: {ixscan: {filter: null, "
        "pattern: {a:1}, bounds: {a: [[1,1,true,true], [2,2,true,true]]}}}}}}}}}");
}