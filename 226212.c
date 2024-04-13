TEST_F(QueryPlannerTest, ExplodeOrForSort) {
    addIndex(BSON("a" << 1 << "c" << 1));
    addIndex(BSON("b" << 1 << "c" << 1));

    runQuerySortProj(fromjson("{$or: [{a: 1}, {a: 2}, {b: 2}]}"), BSON("c" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {c: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {bounds: {a: [[1,1,true,true]], "
        "c: [['MinKey','MaxKey',true,true]]},"
        "pattern: {a:1, c:1}}},"
        "{ixscan: {bounds: {a: [[2,2,true,true]], "
        "c: [['MinKey','MaxKey',true,true]]},"
        "pattern: {a:1, c:1}}},"
        "{ixscan: {bounds: {b: [[2,2,true,true]], "
        "c: [['MinKey','MaxKey',true,true]]},"
        "pattern: {b:1, c:1}}}]}}}}");
}