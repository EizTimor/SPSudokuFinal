/*
 * turns_list.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */

#ifndef TURNS_LIST_H_
#define TURNS_LIST_H_

/*
 * Structure: moveNode
 * ------------------
 * 	A structure used to represent a node in the turns linked list
 *
 * 	row : the row of the changed cell.
 * 	col : the column of the changed cell.
 * 	prev_val : the previous value of the changed cell.
 * 	new_val : the new value of the changed cell.
 * 	next : a pointer to the next element in the list.
 * 	prev : a pointer to the previous element in the list.
 */
typedef struct move_node{
	int row;
	int col;
	int prev_val;
	int new_val;
	struct move_node* next;
	struct move_node* prev;
} MoveNode;

/*
 * Structure: movesList
 * ------------------
 * 	A structure used to represent a linked list
 *
 * 	top : a pointer to the first element of the list
 * 	length : an integer representing the length of the list
 */
typedef struct {
	MoveNode* top;
	int length;
} MovesList;

/*
 * Structure: turnNode
 * ------------------
 * 	A structure used to represent a node in the turns linked list
 *
 * 	changes : a movesList for all the moves in a specific turn.
 * 	next : a pointer to the next element in the list.
 * 	prev : a pointer to the previous element in the list.
 */
typedef struct turn_node {
	MovesList* changes;
	struct turn_node* next;
	struct turn_node* prev;
} TurnNode;

/*
 * Structure: turnsList
 * ------------------
 * 	A structure used to represent a linked list
 *
 * 	top : a pointer to the first element of the list.
 * 	current : a pointer to the last move we've done.
 * 	length : an integer representing the length of the list.
 */
typedef struct {
	TurnNode* top;
	TurnNode* current;
	int length;
} TurnsList;

/*
 * Function: create_moves_list
 * ----------------------
 * 	Creates a new instance of a moves list.
 *
 * 	returns: a pointer to a new empty moves list.
 */
MovesList* create_moves_list();

/*
 * Function: insert_move
 * ----------------------
 * 	Receives a moves list, and 4 integers representing the row, column, previos value and new value of a cell,
 * 	and inserts the information to the moves list.
 *
 * 	moves : the moves list to add the information into.
 * 	row : the row of the changed cell.
 * 	col : the column of the changed cell.
 * 	prev_val : the value of the cell prior to the change.
 * 	new_val : the value of the cell after the change.
 *
 * 	returns: ?
 */
void insert_move(MovesList* moves, int row, int col, int prev_val, int new_val);

/*
 * Function: destroy_moves_list
 * ------------------------
 * 	Used to destroy a movesList structure and free the memory allocated to it.
 *
 * 	moves : a pointer to the movesList structure.
 *
 */
void destroy_moves_list(MovesList* moves);

/*
 * Function: create_turns_list
 * ----------------------
 * 	Creates a new instance of a turns list.
 *
 * 	returns: a pointer to a new empty turns list.
 */
TurnsList* create_turns_list();

/*
 * Function: insert_turn
 * ----------------------
 * 	Receives a turns list, and a changes list,
 * 	and inserts the information to the moves list.
 *
 *	turns : the turns list to add the information into.
 * 	moves : the moves list of the specific turn.
 *
 * 	returns: ?
 */
void insert_turn(TurnsList* turns, MovesList* changes);

/*
 * Function: clean_from_current
 * ----------------------
 * 	Receives a turns list, and clear all turns starting from current (include).
 *
 *	turns : the turns list to add the information into.
 *
 * 	returns: ?
 */
void clean_from_current(TurnsList* turns);

/*
 * Function: destroy_turns_list
 * ------------------------
 * 	Used to destroy a turnsList structure and free the memory allocated to it.
 *
 * 	moves : a pointer to the turnsList structure.
 *
 */
void destroy_turns_list(TurnsList* turns);

#endif /* TURNS_LIST_H_ */
