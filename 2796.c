bool IsValidOpName(StringPiece sp) {
  using ::tensorflow::strings::Scanner;

  Scanner scanner(sp);
  scanner.One(Scanner::UPPERLETTER).Any(Scanner::LETTER_DIGIT_UNDERSCORE);

  while (true) {
    if (!scanner.GetResult())  // Some error in previous iteration.
      return false;
    if (scanner.empty())  // No error, but nothing left, good.
      return true;

    // Absorb another name/namespace, starting with a '>'
    scanner.One(Scanner::RANGLE)
        .One(Scanner::UPPERLETTER)
        .Any(Scanner::LETTER_DIGIT_UNDERSCORE);
  }
}