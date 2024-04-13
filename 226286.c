TEST_F(QueryPlannerTest, NoKeepWithNToReturn) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    params.options |= QueryPlannerParams::SPLIT_LIMITED_SORT;
    addIndex(BSON("a" << 1));
    runQuerySortProjSkipNToReturn(fromjson("{a: 1}"), fromjson("{b: 1}"), BSONObj(), 0, 3);

    assertSolutionExists(
        "{ensureSorted: {pattern: {b: 1}, node: "
        "{or: {nodes: ["
        "{sort: {pattern: {b: 1}, limit: 3, node: {sortKeyGen: {node: "
        "{fetch: {node: {ixscan: {pattern: {a: 1}}}}}}}}}, "
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{fetch: {node: {ixscan: {pattern: {a: 1}}}}}}}}}]}}}}");
}