#include "board.h"
#include "piece.h"
#include "empty.h"

#include "queen.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "pawn.h"
#include "king.h"

#include <utility>
#include <memory>
#include <iostream>

using namespace std;


Board::Board() {
    auto empty = make_shared<Empty>();
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            pieces[i][j] = empty;
        }
    }

    pieces[0][0] = make_shared<Rook>('w');
    pieces[1][0] = make_shared<Knight>('w');
    pieces[2][0] = make_shared<Bishop>('w');
    pieces[3][0] = make_shared<Queen>('w');
    pieces[4][0] = make_shared<King>('w');
    pieces[5][0] = make_shared<Bishop>('w');
    pieces[6][0] = make_shared<Knight>('w');
    pieces[7][0] = make_shared<Rook>('w');

    for (int i=0; i<8; i++) {
        pieces[i][1] = make_shared<Pawn>('w');
        pieces[i][6] = make_shared<Pawn>('b');
    }

    pieces[0][7] = make_shared<Rook>('b');
    pieces[1][7] = make_shared<Knight>('b');
    pieces[2][7] = make_shared<Bishop>('b');
    pieces[3][7] = make_shared<Queen>('b');
    pieces[4][7] = make_shared<King>('b');
    pieces[5][7] = make_shared<Bishop>('b');
    pieces[6][7] = make_shared<Knight>('b');
    pieces[7][7] = make_shared<Rook>('b');
}

Board::~Board() {}

// <int, int> = <col, row>
shared_ptr<Piece> Board::getPiece(std::pair<int, int> position){
    return pieces[position.first][position.second];
}

/*
vector<vector<shared_ptr<Piece>>> Board::getPieces() {
    return pieces;
}
*/

// void Board::movePieceTo(Piece *piece, pair<int, int> position){
bool Board::movePieceTo(shared_ptr<Piece> piece, pair<int, int> from, pair<int, int> to){
    // get valid moves of the piece
    bool isValid = false;
    auto validMovesVec = piece->getValidMoves(from, *this);
    for (auto pointPair : validMovesVec) {
        if (pointPair.first == to.first && pointPair.second == to.second) {
            if (getPiece(pointPair)->getColor() != piece->getColor()) {
                isValid = true;
                break;
            }
        }
    }

    if (!isValid) { return false; }

    lastEatenPiece = getPiece(to);
    lastMovedPiece = piece;
    lastMoveStart = from;
    lastMoveEnd = to;
    // uses setPiece twice: once to place the piece in the end spot,
    // a second time to set the starting point to null.

    /*
    auto targetPiece = getPiece(to);
    if (targetPiece->getColor() == '0') {
        setPiece(piece, to);
    } else if (piece->getColor() != targetPiece->getColor()) {
        // setPiece(make_shared<Empty>(), to);
        setPiece(piece, to);
        // TODO: what to do with the captured piece?
    }
    */
    
    // Check for castling
    if ((piece->getType() == 'k' || piece->getType() == 'K') && !piece->hasMoved()) {
        // auto potentialCastle = to;
        pair<int, int> potentialRook;
        pair<int, int> potentialRookMove;
        if (to.first == 2) {
            potentialRook = make_pair(0, to.second);
            potentialRookMove = make_pair(3, to.second);
        } else if (to.first == 6) {
            potentialRook = make_pair(7, to.second);
            potentialRookMove = make_pair(5, to.second);
        }
        
        auto pRook = getPiece(potentialRook);
        if (piece->getColor() == pRook->getColor()) {
            if (pRook->getType() == 'r' || pRook->getType() == 'R') {
                setPiece(pRook, potentialRookMove);
                setPiece(make_shared<Empty>(), potentialRook);
                pRook->move();
            } else {
                // Do nothing if the move "eats" a piece of the same colour, that is not a castle.
                return false;
            }
        }
    }
    
    // Deny a pawn moving diagonally if it's not capturing an opponent piece
    if ((piece->getType() == 'p' || piece->getType() == 'P')) {
        if (from.first != to.first) {
            auto potentialCapture = getPiece(to);
            char captureColor = potentialCapture->getColor();
            if (captureColor == 0 || captureColor == piece->getColor()) {
                return false;
            }
        }
    }


    /* auto potentialMove = getPiece(to);
    if (piece->getColor() == potentialMove->getColor()) {
        if (potentialMove->getType() == 'r' || potentialMove->getType() == 'R') {
            setPiece(potentialMove, from);
            potentialMove->move();
        } else {
            // Do nothing if the move "eats" a piece of the same colour, that is not a castle.
            return false;
        }
    } else {
        setPiece(make_shared<Empty>(), from);
    } */
    setPiece(make_shared<Empty>(), from);
    setPiece(piece, to);
    piece->move();

    return true;
}

void Board::undo() {
    setPiece(lastMovedPiece, lastMoveStart);
    setPiece(lastEatenPiece, lastMoveEnd);
    lastMovedPiece->undoMove();
}

void Board::setPiece(shared_ptr<Piece> piece, pair<int, int> position) {
    pieces[position.first][position.second] = piece;
}

bool Board::isValidMove(Piece &piece, pair<int, int> position) {
    return true;
}

void Board::clear() {
    auto empty = make_shared<Empty>();
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            pieces[i][j] = empty;
        }
    }
}

/*
BoardIterator Board::begin() {
    return BoardIterator{make_shared<Board>((*this)), make_pair(0,0)};
}

BoardIterator Board::end() {
    return;
}
*/