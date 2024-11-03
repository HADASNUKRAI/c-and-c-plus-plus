#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define ARGC 3
#define SENTENCE 1000
#define BASE 10

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case
    // there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case there
    // is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;


/*
 * pointer to a func that receives data from a generic type and prints it
 * returns void.
 */
static void printing_tweets (const void *data){
  Cell *new_data= (Cell*) data;
  if (new_data->ladder_to != EMPTY){
    printf (" [%d]-ladder to %d ", new_data->number,
            new_data->ladder_to);
  }
  else if (new_data->snake_to != EMPTY){
    printf (" [%d]-snake to %d ", new_data->number,
            new_data->snake_to);
  }
  else{
    printf (" [%d] ", new_data->number);
  }
}



/*
 * pointer to a func that gets 2 pointers of generic data type(same one)
 * and compare between them returns: - a positive value if the first is bigger
          - a negative value if the second is bigger
         - 0 if equal
 */
static int is_equal (const void *first, const void *second){
  const int *new_first = (const int*) first;
  const int *new_second = (const int*) second;
  if (*new_first < *new_second){
    return -1;
  }
  else if (*new_first > *new_second){
    return 1;
  }
  return 0;
}


/*
 * a pointer to a function that gets a pointer of generic data type and
 * frees it.
 * returns void.
 */
static void free_the_data (void *argument){
  free (argument);
}


/*
 * a pointer to a function that  gets a pointer of generic data type and
 * returns a newly allocated copy of it
 * returns a generic pointer.
 */
static void *copy_function (const void* data){
  Cell *new_data = malloc(sizeof(Cell));
  if (new_data == NULL){
    return NULL;
  }
  memcpy (new_data, data, sizeof(Cell));
  return new_data;
}


/*
 * a pointer to function that gets a pointer of generic data type and
 * returns:
 * - true if it's the last state.
 * - false otherwise.
 */
static bool check_last(const void *data){
  Cell *new_data = (Cell *) data;
  if (new_data->number == BOARD_SIZE){
    return true;
  }
  return false;
}







/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};



/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
  printf("%s", error_msg);
  if (database != NULL)
  {
    free_database(database);
  }
  return EXIT_FAILURE;
}


static int create_board(Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    cells[i] = malloc(sizeof(Cell));
    if (cells[i] == NULL)
    {
      for (int j = 0; j < i; j++) {
        free(cells[j]);
      }
      handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
      return EXIT_FAILURE;
    }
    *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
  {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to)
    {
      cells[from - 1]->ladder_to = to;
    }
    else
    {
      cells[from - 1]->snake_to = to;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
  Cell* cells[BOARD_SIZE];
  if(create_board(cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database(markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database(markov_chain,cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
      to_node = get_node_from_database(markov_chain, cells[index_to])
          ->data;
      add_node_to_frequencies_list (from_node, to_node, markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell*) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database(markov_chain, cells[index_to])
            ->data;
        add_node_to_frequencies_list (from_node, to_node, markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free(cells[i]);
  }
  return EXIT_SUCCESS;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */

static MarkovChain  *create_chain (){
  MarkovChain *chain = malloc (sizeof (MarkovChain));
  LinkedList *linked_list = malloc (sizeof (LinkedList));
  if (chain == NULL){
    return NULL;
  }
  if (linked_list == NULL){
    free(chain);
    return NULL;
  }
  linked_list->first = NULL;
  linked_list->last = NULL;
  linked_list->size = 0;
  chain->database = linked_list;
  chain->print_func = printing_tweets;
  chain->comp_func = is_equal;
  chain->free_data = free_the_data;
  chain->copy_func = copy_function;
  chain->is_last = check_last;
  return chain;
}


static void generate_walk (MarkovChain *markov_chain, int max){
  int count = 1;
  MarkovNode *check = markov_chain->database->first->data;
  while (count <= max){
    if (markov_chain->is_last(check->data)){
      markov_chain->print_func (check->data);
      printf("\n");
      break;
    }
    if (count == max){
      markov_chain->print_func (check->data);
      printf("->\n");
    }
    else{
      markov_chain->print_func (check->data);
      printf("->");
    }
    count++;
    if (check->length == 0){
      break;
    }
    check = get_next_random_node (check);
  }
}



int main(int argc, char *argv[])
{
//  char argv_1[] = "1";
//  char argv_2[] = "1";
//  argc = 3;
  MarkovChain *m_chain = create_chain();
  if (m_chain == NULL){
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  int count = 0;
  int walk = 1;
  if (argc != ARGC){
    printf ("%s", "Usage: the argc isn't 3.\n");
    free_database (&m_chain);
    return EXIT_FAILURE;
  }
  if (fill_database (m_chain) == EXIT_FAILURE){
    free_database (&m_chain);
    return EXIT_FAILURE;
  }
  srand (strtol (argv[1], NULL, BASE));
  while (count++ < strtol (argv[2], NULL, BASE)){
    printf ("Random Walk %d:", walk++);
    generate_walk (m_chain, MAX_GENERATION_LENGTH);
  }
  free_database (&m_chain);
  return EXIT_SUCCESS;

}