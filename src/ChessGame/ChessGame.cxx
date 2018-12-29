#include "../Event/Event.hxx"
#include "../Event/EventStack.hxx"

#include "StockfishConnector.hxx"
#include "GameException.hxx"

#include "ChessGame.hxx"


ChessGame::ChessGame()
{
}

void ChessGame::start()
{
    stockfishConnectord.startCommunication();
}

Vector2i ChessGame::uciFormatToPosition(const std::string& position)
{
    int x(0), y(0);
    bool found(false);
    for (x = 0; x < 8 && !found; x++)
    {
        for (y = 0; y < 8 && !found; y++)
        {
            if (uciGrid[x][y].compare(position) == 0)
            {
                found = true;
            }
        }
    }

    if (!found)
    {
        throw GameException("Oups, something went wrong...");
    }

    return {x - 1, y - 1};
}

const std::string& ChessGame::positionToUciFormat(const Vector2i& position)
{
    if (boardAt(position.x, position.y) == OUT_OF_BOUND)
    {
        throw GameException("Oups, something went wrong...");
    }

    return uciGrid[position.x][position.y];
};

const int& ChessGame::boardAt(int x, int y)
{
    if (0 <= x && x < 8 &&
        0 <= y && y < 8)
    {
        return board[x][y];
    }
    else
    {
        return OUT_OF_BOUND;
    }
}

void ChessGame::computePAWNNextPositions()
{
    // Moving one square
    if (boardAt(position.x, position.y + 1) == EMPTY)
    {
        allowedNextPositions[position.x][position.y + 1] = true;
    }

    // Moving two squares
    if (position.y == 1 &&
        boardAt(position.x, position.y + 1) == EMPTY &&
        boardAt(position.x, position.y + 2) == EMPTY)
    {
        allowedNextPositions[position.x][position.y + 2] = true;
    }

    // Moving in diagonal
    if (boardAt(position.x - 1, position.y + 1) < 0)
    {
        allowedNextPositions[position.x - 1][position.y + 1] = true;
    }
    if (boardAt(position.x + 1, position.y + 1) < 0)
    {
        allowedNextPositions[position.x + 1][position.y + 1] = true;
    }
}

void ChessGame::computeROOKNextPositions()
{
    int dx, dy;
    // Moving forward
    for (dy = 1; position.y + dy < 8; dy++)
    {
        if (boardAt(position.x, position.y + dy) <= 0)
        {
            allowedNextPositions[position.x][position.y + dy] = true;
        }
        if (boardAt(position.x, position.y + dy) != EMPTY)
        {
            break;
        }
    }

    // Moving backward
    for (dy = 1; position.y - dy >= 0; dy++)
    {
        if (boardAt(position.x, position.y - dy) <= 0)
        {
            allowedNextPositions[position.x][position.y - dy] = true;
        }
        if (boardAt(position.x, position.y - dy) != EMPTY)
        {
            break;
        }
    }

    // Moving to the right
    for (dx = 1; position.x + dx < 8; dx++)
    {
        if (boardAt(position.x + dx, position.y) <= 0)
        {
            allowedNextPositions[position.x + dx][position.y] = true;
        }
        if (boardAt(position.x + dx, position.y) != EMPTY)
        {
            break;
        }
    }

    // Moving to the left
    for (dx = 1; position.x - dx >= 0; dx++)
    {
        if (boardAt(position.x - dx, position.y) <= 0)
        {
            allowedNextPositions[position.x - dx][position.y] = true;
        }
        if (boardAt(position.x - dx, position.y) != EMPTY)
        {
            break;
        }
    }
}

void ChessGame::computeKNIGHTNextPositions()
{
    // Moving forward/right
    if (boardAt(position.x + 1, position.y + 2) <= 0)
    {
        allowedNextPositions[position.x + 1][position.y + 2] = true;
    }
    if (boardAt(position.x + 2, position.y + 1) <= 0)
    {
        allowedNextPositions[position.x + 2][position.y + 1] = true;
    }

    // Moving forward/left
    if (boardAt(position.x - 1, position.y + 2) <= 0)
    {
        allowedNextPositions[position.x - 1][position.y + 2] = true;
    }
    if (boardAt(position.x - 2, position.y + 1) <= 0)
    {
        allowedNextPositions[position.x - 2][position.y + 1] = true;
    }

    // Moving backward/right
    if (boardAt(position.x + 1, position.y - 2) <= 0)
    {
        allowedNextPositions[position.x + 1][position.y - 2] = true;
    }
    if (boardAt(position.x + 2, position.y - 1) <= 0)
    {
        allowedNextPositions[position.x + 2][position.y - 1] = true;
    }

    // Moving backward/left
    if (boardAt(position.x - 1, position.y - 2) <= 0)
    {
        allowedNextPositions[position.x - 1][position.y - 2] = true;
    }
    if (boardAt(position.x - 2, position.y - 1) <= 0)
    {
        allowedNextPositions[position.x - 2][position.y - 1] = true;
    }
}

void ChessGame::computeBISHOPNextPositions()
{
    int dd;
    // Moving forward/right
    for (dd = 1; position.x + dd < 8 && position.y + dd < 8; dd++)
    {
        if (boardAt(position.x + dd, position.y + dd) <= 0)
        {
            allowedNextPositions[position.x + dd][position.y + dd] = true;
        }
        if (boardAt(position.x + dd, position.y + dd) != EMPTY)
        {
            break;
        }
    }

    // Moving forward/left
    for (dd = 1; position.x - dd >= 0 && position.y + dd < 8; dd++)
    {
        if (boardAt(position.x - dd, position.y + dd) <= 0)
        {
            allowedNextPositions[position.x - dd][position.y + dd] = true;
        }
        if (boardAt(position.x - dd, position.y + dd) != EMPTY)
        {
            break;
        }
    }

    // Moving backward/right
    for (dd = 1; position.x + dd < 8 && position.y - dd < 8; dd++)
    {
        if (boardAt(position.x + dd, position.y - dd) <= 0)
        {
            allowedNextPositions[position.x + dd][position.y - dd] = true;
        }
        if (boardAt(position.x + dd, position.y - dd) != EMPTY)
        {
            break;
        }
    }

    // Moving backward/left
    for (dd = 1; position.x - dd >= 0 && position.y - dd < 8; dd++)
    {
        if (boardAt(position.x - dd, position.y - dd) <= 0)
        {
            allowedNextPositions[position.x - dd][position.y - dd] = true;
        }
        if (boardAt(position.x - dd, position.y - dd) != EMPTY)
        {
            break;
        }
    }
}

void ChessGame::computeKINGNextPositions()
{
    // Move forward forward/right forward/left
    if (boardAt(position.x, position.y + 1) <= 0)
    {
        allowedNextPositions[position.x][position.y + 1] = true;
    }
    if (boardAt(position.x + 1, position.y + 1) <= 0)
    {
        allowedNextPositions[position.x + 1][position.y + 1] = true;
    }
    if (boardAt(position.x - 1, position.y + 1) <= 0)
    {
        allowedNextPositions[position.x - 1][position.y + 1] = true;
    }

    // Move right left
    if (boardAt(position.x + 1, position.y) <= 0)
    {
        allowedNextPositions[position.x + 1][position.y] = true;
    }
    if (boardAt(position.x - 1, position.y) <= 0)
    {
        allowedNextPositions[position.x - 1][position.y] = true;
    }

    // Move backward backward/right backward/left
    if (boardAt(position.x, position.y - 1) <= 0)
    {
        allowedNextPositions[position.x][position.y - 1] = true;
    }
    if (boardAt(position.x + 1, position.y - 1) <= 0)
    {
        allowedNextPositions[position.x + 1][position.y - 1] = true;
    }
    if (boardAt(position.x - 1, position.y - 1) <= 0)
    {
        allowedNextPositions[position.x - 1][position.y - 1] = true;
    }
}

void ChessGame::computeAllowedNextPositions()
{
    // Reset matrix
    resetAllowedNextPositions();

    // Get the selected piece
    const int& piece = boardAt(selectedPiecePosition.x, selectedPiecePosition.y);

    // If the new selected piece position is {-1, -1} which means that nothing is
    // selected, or if the new selected piece is one of AI's pieces (< 0), or if
    // the selected position corresponds to an EMPTY space (== 0), we only reset
    // the allowedNextPositions matrix
    if ((selectedPiecePosition.x == -1 && selectedPiecePosition.y == -1) ||
         piece < 0 ||
         piece == EMPTY)
    {
        return;
    }

    // In other cases, one user's piece has been selected, we compute the new
    // matrix according to this piece
    switch (piece)
    {
        case PAWN:
            computePAWNNextPositions();
            break;
        case ROOK:
            computeROOKNextPositions();
            break;
        case KNIGHT:
            computeKNIGHTNextPositions();
            break;
        case BISHOP:
            computeBISHOPNextPositions();
            break;
        case QUEEN:
            // The rule for the QUEEN is a combination of the rule for the BISHOP and
            // ROOK
            computeBISHOPNextPositions();
            computeROOKNextPositions();
            break;
        case KING:
            computeKINGNextPositions();
            break;
    }
}

void ChessGame::resetAllowedNextPositions()
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            allowedNextPositions[x][y] = false;
        }
    }
}

void ChessGame::setNewSelectedPiecePosition(Vector2i newSelectedPiecePosition)
{
    if (state == USER_TURN)
    {
        // Register last user clicked position
        oldSelectedPiecePosition = selectedPiecePosition;

        // And set the new selected piece position
        selectedPiecePosition = newSelectedPiecePosition;

        // If the new selected piece is an allowed move, it surely means that the user
        // wants to move a piece: it will be performed at the next "perform" method
        // call
        if (allowedNextPositions[selectedPiecePosition.x][selectedPiecePosition.y] == true)
        {
            return;
        }

        // In other cases, compute the new allowedNextPositions matrix
        computeAllowedNextPositions();
    }
}

void ChessGame::perform()
{
    if (state == USER_TURN)
    {
        // If the selected position is an allowed move, it surely means that the
        // user wants to move a piece
        if (selectedPiecePosition.x != -1 && selectedPiecePosition.y != -1 &&
            allowedNextPositions[selectedPiecePosition.x][selectedPiecePosition.y] == true)
        {
            // Set the currently moving piece
            movingPiece = boardAt(oldSelectedPiecePosition.x, oldSelectedPiecePosition.y);
            movingPieceStartPosition = oldSelectedPiecePosition;
            movingPieceEndPosition = selectedPiecePosition;
            movingPiecePosition = {
                (float)movingPieceStartPosition.x, (float)movingPieceStartPosition.y
            };

            // Remove the piece from its old position
            board[oldSelectedPiecePosition.x][oldSelectedPiecePosition.y] = EMPTY;

            // Store the last user move as an UCI string
            lastUserMove.clear();
            lastUserMove.append(positionToUciFormat(oldSelectedPiecePosition));
            lastUserMove.append(positionToUciFormat(selectedPiecePosition));

            // Unselect piece
            oldSelectedPiecePosition = {-1, -1};
            selectedPiecePosition = {-1, -1};

            // Reset suggested user move
            suggestedUserMoveStartPosition = {-1, -1};
            suggestedUserMoveEndPosition = {-1, -1};

            // Reset allowedNextPositions matrix
            resetAllowedNextPositions();

            // Transition to the next state
            state = USER_MOVING;
            clockd.restart();
        }
    }
    else if (state == USER_MOVING || state == AI_MOVING)
    {
        // If a piece has been taken, remove it from the board and send event
        if (boardAt(movingPieceEndPosition.x, movingPieceEndPosition.y) != EMPTY &&
             sqrt(pow((movingPieceEndPosition.x - movingPiecePosition.x) * 4, 2) +
                  pow((movingPieceEndPosition.y - movingPiecePosition.y) * 4, 2)) < 3.2)
        {
            Event event;
            event.type = Event::PieceTakenEvent;
            event.piece.position = movingPieceEndPosition;
            event.piece.piece = boardAt(movingPieceEndPosition.x, movingPieceEndPosition.y);
            EventStack::pushEvent(event);

            board[movingPieceEndPosition.x][movingPieceEndPosition.y] = EMPTY;
        }

        float elapsedTime = clockd.getElapsedTime();
        if (elapsedTime < 1.0)
        {
            movingPiecePosition = {
                elapsedTime * movingPieceEndPosition.x + (1 - elapsedTime) * movingPieceStartPosition.x,
                elapsedTime * movingPieceEndPosition.y + (1 - elapsedTime) * movingPieceStartPosition.y
            };

            // Seng moving piece event
            Event event;
            event.type = Event::PieceMovingEvent;
            event.movingPiece.currentPosition = movingPiecePosition;
            event.movingPiece.startPosition = movingPieceStartPosition;
            event.movingPiece.endPosition = movingPieceEndPosition;
            EventStack::pushEvent(event);
        }
        else
        {
            // Add the moving piece to its end position
            board[movingPieceEndPosition.x][movingPieceEndPosition.y] = movingPiece;

            // Seng piece stops event
            Event event;
            event.type = Event::PieceStopsEvent;
            event.movingPiece.currentPosition = movingPiecePosition;
            event.movingPiece.startPosition = movingPieceStartPosition;
            event.movingPiece.endPosition = movingPieceEndPosition;
            EventStack::pushEvent(event);

            // Reset attributes
            movingPiece = EMPTY;
            movingPiecePosition = {-1, -1};
            movingPieceStartPosition = {-1, -1};
            movingPieceEndPosition = {-1, -1};

            // Transition to waiting state if it's the next turn is the AI turn, USER_TURN otherwise.
            state = state == USER_MOVING ? WAITING : USER_TURN;
            clockd.restart();
        }
    }
    else if (state == WAITING)
    {
        if (clockd.getElapsedTime() >= 1.0)
        {
            state = AI_TURN;
            clockd.restart();
        }
    }
    else if (state == AI_TURN)
    {
        // Get AI decision according to the last user move
        std::string aiMove = stockfishConnectord.getNextAIMove(lastUserMove);

        // If the AI tried to move one user's pawn, stop the game
        Vector2i aiMoveStartPosition = uciFormatToPosition(aiMove.substr(0, 2));
        if (boardAt(aiMoveStartPosition.x, aiMoveStartPosition.y) >= 0)
        {
            throw GameException("A forbiden move has been performed!");
        }

        // Set the currently moving piece
        movingPiece = boardAt(aiMoveStartPosition.x, aiMoveStartPosition.y);
        movingPieceStartPosition = aiMoveStartPosition;
        movingPieceEndPosition = uciFormatToPosition(aiMove.substr(2, 2));;
        movingPiecePosition = {
            (float)aiMoveStartPosition.x, (float)aiMoveStartPosition.y
        };

        // Remove the piece from its old position
        board[aiMoveStartPosition.x][aiMoveStartPosition.y] = EMPTY;

        // Get suggested user next move if available
        if (stockfishConnectord.suggestedUserMove.compare("(none)") != 0)
        {
            std::string startPosition_str = stockfishConnectord.suggestedUserMove.substr(0, 2);
            std::string endPosition_str = stockfishConnectord.suggestedUserMove.substr(2, 2);

            suggestedUserMoveStartPosition = uciFormatToPosition(startPosition_str);
            suggestedUserMoveEndPosition = uciFormatToPosition(endPosition_str);
        }
        else
        {
            suggestedUserMoveStartPosition = {-1, -1};
            suggestedUserMoveEndPosition = {-1, -1};
        }

        // Transition to AI_MOVING state
        state = AI_MOVING;
        clockd.restart();
    }
}

ChessGame::~ChessGame()
{
}
