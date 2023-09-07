#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  game_state_t* game = malloc(sizeof(game_state_t));
  game->num_rows = 18; 
  game->num_snakes = 1;
  snake_t* snek = malloc(sizeof(snake_t));
  snek->tail_row = 2;
  snek->head_col = 4;
  snek->head_row = 2;
  snek->tail_col = 2;
  snek->live = true;
  snake_t* snakeslst = malloc(sizeof(snake_t)*(game->num_snakes)); 
  snakeslst[0] = *snek;
  game->snakes = snakeslst;
  char** board = malloc(sizeof(char*)*(game->num_rows+1));
  char border[] = {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','\0'};
  char empty[] = {'#',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\0'};
  char snakie[] = {'#',' ','d','>','D',' ',' ',' ',' ','*',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','\0'};
  for(int i = 0; i<game->num_rows; i++){
    board[i] = malloc(sizeof(char)*21);
  }
  strcpy(board[0], border);
  strcpy(board[1], empty);
  strcpy(board[2], snakie);
  for (int i = 3; i < game->num_rows-1; i++){
    strcpy(board[i], empty);
  }
  strcpy(board[game->num_rows-1], border);
  free(snek);
  game->board = board;
  return game;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++){
    free((state->board)[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (int i = 0; i < state->num_rows; i++){
    fprintf(fp, (state->board)[i]);
    fprintf(fp, "\n");
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  if (c == 'a' | c == 'w'| c == 's'| c == 'd'){
    return true; 
  } else {
    return false; 
  }
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  if (c == 'W' | c == 'A'| c == 'S'| c == 'D' | c == 'x'){
    return true; 
  } else {
    return false; 
  }
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  if (is_head(c) | is_tail(c) | c == '>' | c == '<'| c == '^'| c == 'v'){
    return true; 
  } else {
    return false; 
  }
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^'){
    return 'w';
  } else if (c == '<'){
    return 'a';
  } else if (c == 'v') {
    return 's';
  } else if (c == '>') {
    return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W'){
    return '^';
  } else if (c == 'A'){
    return '<';
  } else if (c == 'S') {
    return 'v';
  } else if (c == 'D') {
    return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' | c == 's' | c == 'S') {
    return cur_row + 1;
  } else if (c == '^' | c == 'w' | c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' | c == 'd' | c == 'D') {
    return cur_col + 1;
  } else if (c == '<' | c == 'a' | c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  snake_t snake = state->snakes[snum];
  char headc = state->board[snake.head_row][snake.head_col];
  return state->board[get_next_row(state->snakes[snum].head_row, headc)]
                     [get_next_col(state->snakes[snum].head_col, headc)];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  snake_t snake = state->snakes[snum];
  char headc = state->board[snake.head_row][snake.head_col];
  state->board[get_next_row(state->snakes[snum].head_row, headc)]
              [get_next_col(state->snakes[snum].head_col, headc)] = state->board[state->snakes[snum].head_row][state->snakes[snum].head_col];
  state->board[state->snakes[snum].head_row][state->snakes[snum].head_col] = head_to_body(headc);
  state->snakes[snum].head_row = get_next_row(state->snakes[snum].head_row, headc);
  state->snakes[snum].head_col = get_next_col(state->snakes[snum].head_col, headc);
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  snake_t snake = state->snakes[snum];
  char tailc = state->board[snake.tail_row][snake.tail_col];
  state->board[get_next_row(state->snakes[snum].tail_row, tailc)]
              [get_next_col(state->snakes[snum].tail_col, tailc)]
           = body_to_tail(state->board
              [get_next_row(state->snakes[snum].tail_row, tailc)]
              [get_next_col(state->snakes[snum].tail_col, tailc)]);
  state->board[state->snakes[snum].tail_row][state->snakes[snum].tail_col] = ' ';
  state->snakes[snum].tail_row = get_next_row(state->snakes[snum].tail_row, tailc);
  state->snakes[snum].tail_col = get_next_col(state->snakes[snum].tail_col, tailc);
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  for (int i = 0; i < strlen(state->snakes); i++) {
    printf("Hello %d", strlen(state->snakes));
    if (is_snake(next_square(state,i)) | next_square(state,i) == '#') {
      state->board[state->snakes[i].head_row][state->snakes[i].head_col] = 'x';
      state->snakes[i].live = false;
    } else if (next_square(state, i) == ' ') {
      update_head(state, i);
      update_tail(state, i);
    } else if (next_square(state, i) == '*') {
      update_head(state, i);
      add_food(state);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    return NULL;
  }
  //init game, num_rows, and board
  game_state_t* game = malloc(sizeof(game_state_t));
  game->num_rows = 0;
  char** board = malloc(sizeof(char*));
  
  char A[500];
  int count = 0;
  while (fgets(A, 500, file)) {
    game->num_rows++;
    board = realloc(board, sizeof(char*) * game->num_rows);
    board[count] = malloc(strlen(A) * sizeof(char));
    char row[strlen(A)];
    for (int c = 0; c < strlen(A) - 1; c++) {
      row[c] = A[c];
    }
    row[strlen(A) - 1] = '\0';
    strcpy(board[count], row);
    count++;
  }

  game->board = board;
  game->num_snakes = 0;
  game->snakes = NULL;
  return game;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  snake_t snake = state->snakes[snum];
  char tail = board[snake.tail_row][snake.tail_col]; 
  int r = get_next_row(snake.tail_row, tail); 
  int c = get_next_col(snake.tail_col, tail); 
  while (!ishead(state->board[r][c])){
	char new = board[r][c];
	r = get_next_row(r, new); 
	c = get_next_col(c, new); 
  }
  snake.head_row = r; 
  snake.head_col = c; 
}


/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  for (int r = 0; r < state->num_rows; r++) {
    for (int c = 0; c < strlen(state->board[r]); c++) {
      //help me
    }
  }
  state->snakes = malloc(sizeof(snake_t) * state->num_snakes);
  snake_t *snek = malloc(sizeof(snake_t));
  state->snakes[0] = *snek;
  state->snakes[0].live = true;
  find_tail(state, 0);
  find_head(state, 0);
  return state;
}
