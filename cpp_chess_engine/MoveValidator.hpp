#ifndef MOVE_VALIDATOR_HPP
#define MOVE_VALIDATOR_HPP

class ChessBoard; // Forward declaration

// Abstract interface for move validators.
class MoveValidator {
public:
    virtual ~MoveValidator() {}
    // Validate a move from square 'from_idx' to square 'to_idx' on the given board.
    virtual bool validate(int from_idx, int to_idx, const ChessBoard& board) const = 0;
};

#endif // MOVE_VALIDATOR_HPP
