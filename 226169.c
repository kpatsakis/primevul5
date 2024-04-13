TEST_F(QueryPlannerTest, InWithoutSort) {
    addIndex(BSON("a" << 1 << "b" << 1));
    // No sort means we don't bother to blow up the bounds.
    runQuerySortProjSkipNToReturn(fromjson("{a: {$in: [1, 2]}}"), BSONObj(), BSONObj(), 0, 1);

    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {a: 1, b: 1}}}}}");
}