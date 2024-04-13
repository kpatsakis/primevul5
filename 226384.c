TEST_F(QueryPlannerTest, SortKeyMetaProjection) {
    addIndex(BSON("a" << 1));

    runQuerySortProj(BSONObj(), fromjson("{a: 1}"), fromjson("{b: {$meta: 'sortKey'}}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{proj: {spec: {b: {$meta: 'sortKey'}}, node: "
        "{sort: {limit: 0, pattern: {a: 1}, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}}}");
    assertSolutionExists(
        "{proj: {spec: {b: {$meta: 'sortKey'}}, node: "
        "{sortKeyGen: {node: {fetch: {filter: null, node: "
        "{ixscan: {pattern: {a: 1}}}}}}}}}");
}