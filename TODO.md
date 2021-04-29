- Check for discovered checks in en-passant moves
- The king_danger_squares bitboard is buggy and needs to be fixed (
  e.g. "n1n5/PPPk4/8/8/8/8/4Kp1p/5NqN w - - 0 1" will allow capture of the f2
  pawn although that leaves the king in check)

Failed tests:

- Custom8
- Custom9
- Custom10
- Custom11
- Custom12
