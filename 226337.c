TEST_F(QueryPlannerTest, ExplodeOrForSort2) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    addIndex(BSON("d" << 1 << "c" << 1));

    runQuerySortProj(
        fromjson("{$or: [{a: 1, b: {$in: [1, 2]}}, {d: 3}]}"), BSON("c" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {c: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {bounds: {a: [[1,1,true,true]], b: [[1,1,true,true]],"
        "c: [['MinKey','MaxKey',true,true]]},"
        "pattern: {a:1, b:1, c:1}}},"
        "{ixscan: {bounds: {a: [[1,1,true,true]], b: [[2,2,true,true]],"
        "c: [['MinKey','MaxKey',true,true]]},"
        "pattern: {a:1, b:1, c:1}}},"
        "{ixscan: {bounds: {d: [[3,3,true,true]], "
        "c: [['MinKey','MaxKey',true,true]]},"
        "pattern: {d:1, c:1}}}]}}}}");
}