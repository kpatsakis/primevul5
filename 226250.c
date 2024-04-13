TEST_F(QueryPlannerTest, IndexBoundsIndexedSort) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{$or: [{a: 1}, {a: 2}]}"), BSON("a" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {a:1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {filter: {$or:[{a:1},{a:2}]}, dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {filter: null, "
        "pattern: {a:1}, bounds: {a: [[1,1,true,true], [2,2,true,true]]}}}}}");
}