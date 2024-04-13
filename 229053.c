DEATH_TEST(EqOp, InvalidEooOperand, "Invariant failure _rhs") {
    BSONObj operand;
    EqualityMatchExpression eq("", operand.firstElement());
}