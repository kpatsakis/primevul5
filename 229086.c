DEATH_TEST(GtOp, InvalidEooOperand, "Invariant failure _rhs") {
    BSONObj operand;
    GTMatchExpression gt("", operand.firstElement());
}