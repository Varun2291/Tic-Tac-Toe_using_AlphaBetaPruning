#ifndef AI__TTT_HPP_
#define AI__TTT_HPP_

#include <cstring>
#include <vector>

//define the maximum value for the evaluation function,
//which should return a value in the range [-MAX_VALUE, MAX_VALUE]
#define MAX_VALUE 100000000000

class TTT
{
public:
//initial values for nrRows, nrCols, and nrToWin
//these values are changed to the values specified by the user when the program is run
    TTT(void) : m_nrRows(3),
		m_nrCols(3),
		m_nrToWin(3)
    {
    }
    
    
//Variables representing the number of rows, number of columns, and number of consecutive elements needed for a win
    int m_nrRows;
    int m_nrCols;
    int m_nrToWin;
    
    
//EMPTY value denotes an empty square
//FIRST value denotes a square occupied by the first player
//SECOND value denotes a square occupied by the second player
    enum Value
	{
	    EMPTY = 0,
	    FIRST = 1,
	    SECOND= 2
	};
    
//State of the tic-tac-toe is presented as a one-dimensional array (similar to puzzle board)
//There are also functions (see GetValueState, SetValueState) to get and set the state values
//at different rows and columns
    typedef unsigned char *State;
 
//This is a utility function which converts row, col into id
    int IdFromRowCol(const int row, const int col) const
    {
	return row * m_nrCols + col;
    }
   
//extract row from id 
    int RowFromId(const int id) const
    {
	return id / m_nrCols;
    }
 
//extract column from id   
    int ColFromId(const int id) const
    {
	return id % m_nrCols;
    }
    

//This is a utility function which returns the value in position (row, col)
    Value GetValueState(const State s, const int row, const int col) const
    {
	return (Value) s[row * m_nrCols + col];
    }
    
//This is a utility function which sets the square in position (row, col) to val
//For example, SetValue(s, 1, 2, FIRST) sets the square in position (1, 2) to the value FIRST 
// (marks the move by the first player)
    void SetValueState(State s, const int row, const int col, const Value val) const
    {
	s[row * m_nrCols + col] = val;
    }
    
//This is a utility function which checks if the player that made the move in position (r, c) actually won the game.
//Note that (r, c) corresponds to the last move made. The function returns true iff this last move made is a winning move.
    bool CheckWin(const State s, const int r, const int c) const;
    
    
//This is the function that you need to write 
//(write it in the src/Programs/TTT.cpp file; here it's just the definition of the function).
    double EvalState(const State s) const;
    
    
//This function uses minimax with alpha-beta prunning and the evaluation function to select the best move
//s is the board configuration
//depth is the maximum search depth
//remMoves is the number of remaining moves (the game ends in a tie if remMoves is zero)
//lastMove indicates the last move made by a player
//bestScore is an output variable, which will contain the highest score for the player
//bestMove is an output variable, which will contain the best move for the player	  
    void BestMove(State   s, 
		  int     depth, 
		  int     remMoves, 
		  int     lastMove,
		  Value   player,
		  double *bestScore,
		  int    *bestMove) ; 
    
//This is a utility function, which allocates memory for a new state		
    State NewState(void) const
    {
	return new unsigned char[m_nrRows * m_nrCols];
    }
    
    
//This is a utility function, which deletes the memory allocated to a state
    void DeleteState(State s) const
    {
	if(s)
	    delete [] s;
    }
    
//This is a utility function which sets every square to 0
    void ClearState(State s) const
    {
	memset(s, 0, m_nrRows * m_nrCols * sizeof(unsigned char));
    }
    
//This is a utility function which assigns copies the values of the state src to the state dest
    void CopyState(const State src, State dest) const
    {
	memcpy(dest, src, m_nrRows * m_nrCols * sizeof(unsigned char));
    }
    
//This is a utility function which creates a new state as an exact copy of the state s
    State CopyState(const State s) const
    {
	State dest = NewState();
	CopyState(s, dest);
	return dest;
    }

	typedef struct Node
	{
		State s;
		double bestScore;
		int bestMove;
	};
	typedef struct Node node1;

	std::vector<node1*> GetChildNodes(node1 *root, Value player);
	bool CheckTerminality(State s) const;
	void printboard(State s);
	node1* CopyNode(node1 *child);
	int CheckGameAdvantage(State s, int color);


	//This function uses minimax with alpha-beta prunning and the evaluation function. 
	//s is the board configuration
	//depth is the maximum search depth
	//remMoves is the number of remaining moves (the game ends in a tie if remMoves is zero)
	//lastMove indicates the last move made by a player
	//color indicates the turn (1 for FIRST and -1 for SECOND player)
    node1* NegaMax(node1 *node, 
		   int     depth, 
		   double  alpha, 
		   double  beta, 
		   int     remMoves, 
		   int     lastMove,
		   int     color);

};


#endif

