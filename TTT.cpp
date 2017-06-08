#include "Programs/TTT.hpp"
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

TTT::node1 *fNode = new TTT::node1();	// Variable to hold the final node value with best score and best move
int bestMovePos = 0;
int colorValue = 0;

// Function to print the board
void TTT::printboard(State s)
{
	for(int r = 0 ; r < TTT::m_nrRows; r++)
		for(int c = 0; c < TTT::m_nrCols; c++)
			cout<<GetValueState(s, r, c);
}

// Function to calculate the evaluation value
double TTT::EvalState(const State s) const
{
	double blue = 0, blue_count = 0;
	double red = 0, red_count = 0;
	
	//checking row values
	for(int r = 0; r < m_nrRows; r++)
	{
		blue = 0, red = 0;
		for(int m = 0; m < m_nrCols; m++)
		{
			Value val = GetValueState(s,r,m);
			if((val == TTT::FIRST) || (val == TTT::EMPTY))
			blue++;
			if((val == TTT::SECOND) || (val == TTT::EMPTY))
			red++;
		}
		if(blue >= m_nrToWin)
			blue_count+=1;
		
		if(red >= m_nrToWin)
			red_count+=1;
	}

	//checking column values
	blue = 0, red = 0;
	for(int c = 0; c < m_nrCols; c++)
	{ 
		blue = 0, red = 0;
		for(int m = 0; m < m_nrRows; m++)
		{
			Value val = GetValueState(s,m,c);
			if((val == TTT::FIRST) || (val == TTT::EMPTY))
			blue++;
			if((val == TTT::SECOND) || (val == TTT::EMPTY))
			red++;
		}

		if(blue >= m_nrToWin)
			blue_count+=1;
		if(red >= m_nrToWin)
			red_count+=1;
	}
	
	//check diagonal
	for(int i = 0; i < (m_nrRows + m_nrCols) - 1 ; i++)
	{
		int r = 0, c = 0;
		if(i < m_nrRows)
		{
			r = (m_nrRows - 1) - i;
			c = 0;
		}
		else
		{
			r = 0;
			c = (i + 1) % m_nrCols;
		}
		blue = 0, red = 0;
		while(r < m_nrRows && c < m_nrCols)
		{	
			if ((GetValueState(s,r,c)== FIRST) || (GetValueState(s,r,c)== EMPTY))
				blue++;
			if ((GetValueState(s,r,c) == SECOND || (GetValueState(s,r,c)== EMPTY)))
				red++;
			if(blue >= m_nrToWin)
				blue_count += 1;
			if(red >= m_nrToWin)
				red_count += 1;
			r++, c++;
		}
	}
 
	//check anti diagonal
	for(int i = 0; i < (m_nrRows + m_nrCols) - 1 ; i++)
	{
		int r = 0 , c = 0;
		if(i < m_nrRows)
		{
			r = i;
			c = 0;
		}
		else
		{
			r = m_nrRows - 1;
			c = (i + 1) % m_nrCols;
		}
		blue = 0, red = 0;
		while(r >= 0 && c < m_nrCols)
		{
			if ((GetValueState(s,r,c)== FIRST) || (GetValueState(s,r,c)== EMPTY))
				blue++;
			if ((GetValueState(s,r,c) == SECOND || (GetValueState(s,r,c)== EMPTY)))
				red++;
			if(blue >= m_nrToWin)
				blue_count += 1;
			if(red >= m_nrToWin)
				red_count += 1;
			r--, c++;
		}
	}


	double eval_val = (blue_count - red_count);
	return eval_val;

    return 0;
}	

// Function to check for leaf nodes
bool TTT::CheckTerminality(State s) const
{
	for(int r = 0 ; r < m_nrRows; r++)
		for(int c = 0; c < m_nrCols; c++)
			if(GetValueState(s, r, c) == EMPTY)
				return false;
	return true;
}

// Function to create a copy of a node (deep copy)
TTT::node1* TTT::CopyNode(node1 *child)
{
	node1* child1 = new node1();

	memcpy(child1, child, sizeof(node1));

	return child1;
}

// Function to get the child nodes given a parent node
std::vector<TTT::node1*> TTT::GetChildNodes(node1 *root, Value player)
{
	std::vector<TTT::node1*> children;
	node1 *child = new node1();
	node1 *child1 = new node1();
	for(int r = 0 ; r < TTT::m_nrRows; r++)
	{
		for(int c = 0; c < TTT::m_nrCols; c++)
		{
			if(GetValueState(root->s, r, c) == EMPTY)
			{
				State new_temp = CopyState(root->s);			// Create a copy of the original state
				SetValueState(new_temp, r, c, player);			// Set the player value in the new board(state) to create a child node

			//	cout<<endl<<"NewTemp: ";printboard(new_temp);
				
				child->bestScore = root->bestScore;				// Set the score from parent
				child->bestMove = (TTT::m_nrCols * r + c);		// Set the move that was performed to get this board
				child->s = CopyState(new_temp);					// Copy the modified board
			//	cout<<endl<<"child: ";printboard(child->s);
				child1 = CopyNode(child);
				children.push_back(child1);
			}
		}
	}
	
	return children;
}

void TTT::BestMove(State   s, 
		   int     depth, 
		   int     remMoves, 
		   int     lastMove,
		   Value   player,
		   double *bestScore,
		   int    *bestMove)
{
    
	// Create the root node
	node1 *root_node = new node1();
	// Initialize the root node
	root_node->s = CopyState(s);
	root_node->bestScore = -1;			
	root_node->bestMove = -1;

	int player_num = 0;
	if(player == FIRST)
		player_num = 1;
	else if(player == SECOND)
		player_num = -1;

	// Check if next move will finish the game
	if((bestMovePos = CheckGameAdvantage(s, player_num)) != -1)
	{
		*bestScore = EvalState(s);
		*bestMove = bestMovePos;
		return;
	}

	// Call Neag Max function to calcuate teh best move and score
	node1 *finalNode = NegaMax(root_node, depth, -HUGE_VAL, HUGE_VAL, remMoves, lastMove, player_num);
	
	// Store the best move and best score
	*bestScore = finalNode->bestScore;
	*bestMove = finalNode->bestMove;
}

// Function to check if a player can win in the next move
// with the current board configuaration
int TTT::CheckGameAdvantage(State s, int color)
{
	int count = 0, countAgainst = 0, emptyCount = 0;
	int pos = 0, pos1_r = 0, pos1_c = 0, pos1_d = 0, pos1_ad = 0;

	bool emptyBoardFlag = true;

	// Check if the board is empty
	for(int r = 0 ; r < m_nrRows; r++)
	{
		for(int c = 0; c < m_nrCols; c++)
		{
			if(GetValueState(s, r, c) != EMPTY)
			{
				emptyBoardFlag = false;
			}
		}
	}
	if(emptyBoardFlag)
		return (m_nrRows*m_nrCols)/2; 

	// Set the for and against players depending on the color value
	Value colorCountFor, colorCountAgainst;
	if(color == 1)
	{
		colorCountFor = FIRST;
		colorCountAgainst = SECOND;
	}
	else
	{
		colorCountFor = SECOND;
		colorCountAgainst = FIRST;
	}

	// Check rows
	bool checkRowFlag = false;
	for(int r = 0; r < m_nrRows; r++)
	{
		count = 0; emptyCount = 0, countAgainst = 0;
		for(int c = 0; c < m_nrCols; c++)
		{
			if(GetValueState(s, r, c) == colorCountFor)
			{
				count++;
				countAgainst = 0;
			}
			else if(GetValueState(s, r, c) == colorCountAgainst)
			{
				countAgainst++;
				count = 0;
			}
			else if(GetValueState(s, r, c) == EMPTY)
			{
				pos = m_nrCols * r + c;
				emptyCount++;
			}
			if( ((count + emptyCount) == m_nrToWin) && (emptyCount == 1) )
				return pos;
			if(((countAgainst + emptyCount) == m_nrToWin) && (emptyCount == 1))
			{
				checkRowFlag = true;
				pos1_r = pos;
			}
		}
	}	
	
	// Check columns
	bool checkcolFlag = false;
	for(int c = 0; c < m_nrCols; c++)
	{
		count = 0; emptyCount = 0, countAgainst = 0;
		for(int r = 0; r < m_nrRows; r++)
		{
			if(GetValueState(s, r, c) == colorCountFor)
			{
				count++;
				countAgainst = 0;
			}
			else if(GetValueState(s, r, c) == colorCountAgainst)
			{
				countAgainst++;
				count = 0;
			}
			else if(GetValueState(s, r, c) == EMPTY)
			{
				pos = m_nrCols * r + c;
				emptyCount++;
			}
			if( ((count + emptyCount) == m_nrToWin) && (emptyCount == 1)) 
				return pos;
			if (((countAgainst + emptyCount) == m_nrToWin) && (emptyCount == 1))
			{
				checkcolFlag = true;
				pos1_c = pos;
			}
		}
	}

	//check diagonal
	bool checkDiagonalFlag = false;
	for(int i = 0; i < (m_nrRows + m_nrCols) - 1 ; i++)
	{
		int r = 0, c = 0, blue_dia = 0, red_dia = 0, empty_count = 0;
		if(i < m_nrRows)
		{
		   r = (m_nrRows - 1) - i;
		   c = 0;
		}
		else
		{
			r = 0;
			c = (i + 1) % m_nrCols;
		}
		count = 0; emptyCount = 0, countAgainst = 0;
		
		while(r < m_nrRows && c < m_nrCols)
		{
			if (GetValueState(s,r,c)== colorCountFor)
			{
				count++;
				countAgainst = 0;
			}
			else if (GetValueState(s,r,c) == colorCountAgainst)
			{
				countAgainst ++;
				count = 0;
			}
			else if (GetValueState(s,r,c)== EMPTY)
			{
				pos = m_nrCols * r + c;
				emptyCount++;
			}
			r++, c++;

			if(((count + emptyCount) >= m_nrToWin) && (emptyCount == 1))
				return pos;
			if(((countAgainst + emptyCount) >= m_nrToWin) && (emptyCount == 1))
			{
				checkDiagonalFlag = true;
				pos1_d = pos;
			}
		}
	}
	if(checkDiagonalFlag)
		return pos1_d;

	//check anti diagonal
	bool checkAntiDiagonalFlag = false;
	for(int i = 0; i < (m_nrRows + m_nrCols) - 1 ; i++)
	{
		int r = 0 , c = 0, blue_dia = 0, red_dia = 0, empty_count = 0;
		if(i < m_nrRows)
		{
			r = i;
			c = 0;
		}
		else
		{
			r = m_nrRows - 1;
			c = (i + 1) % m_nrCols;
		}
		count = 0; emptyCount = 0, countAgainst = 0;

		while(r >= 0 && c < m_nrCols)
		{
			if (GetValueState(s,r,c)== colorCountFor)
			{
				count++;
				countAgainst = 0;
			}
			else if (GetValueState(s,r,c) == colorCountAgainst)
			{
				countAgainst ++;
				count = 0;
			}
			else if (GetValueState(s,r,c)== EMPTY)
			{
				pos = m_nrCols * r + c;
				emptyCount++;
			}
			
			r--, c++;
			
			if(((count + emptyCount) >= m_nrToWin) && (emptyCount == 1))
				return pos;
			if(((countAgainst + emptyCount) >= m_nrToWin) && (emptyCount == 1))
			{
				checkAntiDiagonalFlag = true;
				pos1_ad = pos;
			}
		}
	}	
	if(checkRowFlag)
		return pos1_r;
	if(checkcolFlag)
		return pos1_c;
	if(checkDiagonalFlag)
		return pos1_d;
	if(checkAntiDiagonalFlag)
		return pos1_ad;

	return -1;
}

// Function to calculate the Best score and move using an evaluation function
TTT::node1* TTT::NegaMax(node1 *node, 
		    int     depth, 
		    double  alpha, 
		    double  beta, 
		    int     remMoves, 
		    int     lastMove,
		    int     color)
{
	// Base condition: terminate if the tree reaches the leaves
	// or if depth becomes 0
    if((depth == 0) || CheckTerminality(node->s))
	{
		fNode->bestScore = EvalState(node->s);
		fNode->bestMove = node->bestMove;
		return fNode;
	}

	// Set the correct player depending on the color
	// 1 - First
	// -1 - Second
	if(color == 1)
		colorValue = FIRST;
	else
		colorValue = SECOND;

	// Get children for the current board
	std::vector<node1*> child_nodes = GetChildNodes(node, (Value)colorValue);
	node1 *result_node;
	for(auto child = child_nodes.begin(); child != child_nodes.end(); child++)
	{
		result_node = NegaMax((*child), (depth-1), -beta, -alpha, (remMoves - 1), lastMove, -color); 
		result_node->bestScore = -result_node->bestScore;
		alpha = std::max(alpha, result_node->bestScore); 
		if(alpha >= beta)
			break;
	}
	return result_node;
}

bool TTT::CheckWin(const State s, const int r, const int c) const
{
    const Value val  = GetValueState(s, r, c);
    int   cons = 0, end;
    
    //check row
    cons = 0; end = std::min(m_nrCols, c + m_nrToWin);
    for(int i = std::max(0, c - m_nrToWin); i < end; ++i)
	if(GetValueState(s, r, i) != val)
	    cons = 0;
	else if((++cons) >= m_nrToWin)
	    return true;
    
    //check column
    cons = 0; end = std::min(m_nrRows, r + m_nrToWin);
    for(int i = std::max(0, r - m_nrToWin); i < end; ++i)
	if(GetValueState(s, i, c) != val)
	    cons = 0;
	else if((++cons) >= m_nrToWin)
	    return true;
    
    //check diagonal
    cons = 0; end = 1 + std::min(m_nrToWin - 1, std::min(m_nrRows - r - 1, m_nrCols - c - 1));
    for(int i = std::max(-m_nrToWin + 1, std::max(-r, -c)); i < end; ++i)
	if(GetValueState(s, r + i, c + i) != val)
	    cons = 0;
	else if((++cons) >= m_nrToWin)
	    return true;
    
    //check anti-diagonal
    cons = 0; end = 1 + std::min(m_nrToWin - 1, std::min(r, m_nrCols - c - 1));
    for(int i = std::max(-m_nrToWin + 1, std::max(r - m_nrRows + 1, -c)); i < end; ++i)
    {
	if(GetValueState(s, r - i, c + i) != val)
	    cons = 0;
	else if((++cons) >= m_nrToWin)
	    return true;
    }
    
    
    return false;
    
    
}


