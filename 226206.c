TEST_F(QueryPlannerTest, NaturalSortAndHint) {
    addIndex(BSON("x" << 1));

    // Non-empty query, -1 sort, no hint.
    runQuerySortHint(fromjson("{x: {$exists: true}}"), BSON("$natural" << -1), BSONObj());
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: -1}}");

    // Non-empty query, 1 sort, no hint.
    runQuerySortHint(fromjson("{x: {$exists: true}}"), BSON("$natural" << 1), BSONObj());
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");

    // Non-empty query, -1 sort, -1 hint.
    runQuerySortHint(
        fromjson("{x: {$exists: true}}"), BSON("$natural" << -1), BSON("$natural" << -1));
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: -1}}");

    // Non-empty query, 1 sort, 1 hint.
    runQuerySortHint(
        fromjson("{x: {$exists: true}}"), BSON("$natural" << 1), BSON("$natural" << 1));
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");

    // Empty query, -1 sort, no hint.
    runQuerySortHint(BSONObj(), BSON("$natural" << -1), BSONObj());
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: -1}}");

    // Empty query, 1 sort, no hint.
    runQuerySortHint(BSONObj(), BSON("$natural" << 1), BSONObj());
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");

    // Empty query, -1 sort, -1 hint.
    runQuerySortHint(BSONObj(), BSON("$natural" << -1), BSON("$natural" << -1));
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: -1}}");

    // Empty query, 1 sort, 1 hint.
    runQuerySortHint(BSONObj(), BSON("$natural" << 1), BSON("$natural" << 1));
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}