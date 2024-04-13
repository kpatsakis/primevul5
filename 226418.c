TEST_F(QueryPlannerTest, CannotSnapshotWithGeoNear) {
    // Snapshot is skipped with geonear queries.
    addIndex(BSON("a"
                  << "2d"));
    runQuerySnapshot(fromjson("{a: {$near: [0,0]}}"));

    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists("{geoNear2d: {a: '2d'}}");
}