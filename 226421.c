TEST_F(QueryPlannerTest, SortKeyMetaProjectionCovered) {
    addIndex(BSON("a" << 1));

    runQuerySortProj(
        BSONObj(), fromjson("{a: 1}"), fromjson("{_id: 0, a: 1, b: {$meta: 'sortKey'}}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: {$meta: 'sortKey'}}, node: "
        "{sort: {limit: 0, pattern: {a: 1}, node: "
        "{sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: {$meta: 'sortKey'}}, node: "
        "{sortKeyGen: {node: "
        "{ixscan: {pattern: {a: 1}}}}}}}");
}