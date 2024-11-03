#include "markov_chain.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ARGC_1 4
#define ARGC_2 5
#define SENTENCE 1000
#define BASE 10
#define MAX_WORD 100
#define MAX_LENGTH 20

/*
 * pointer to a func that receives data from a generic type and prints it
 * returns void.
 */
static void print_tweets (const void *data){
  const char *new_data= (const char*) data;
  printf(" %s", new_data);
}

/*
 * pointer to a func that gets 2 pointers of generic data type(same one)
 * and compare between them returns: - a positive value if the first is bigger
          - a negative value if the second is bigger
         - 0 if equal
 */
static int is_bigger_equal (const void *first, const void *second){
  const char *new_first = (const char*) first;
  const char *new_second = (const char*) second;
  while (*new_first != '\0' || *new_second != '\0'){
    if (*new_first < *new_second){
      return -1;
    }
    else if (*new_first > *new_second){
      return 1;
    }
    new_first++;
    new_second++;
  }
  return 0;
}


/*
 * a pointer to a function that gets a pointer of generic data type and
 * frees it.
 * returns void.
 */
static void data_free (void *argument){
  free (argument);
}

/*
 * a pointer to a function that  gets a pointer of generic data type and
 * returns a newly allocated copy of it
 * returns a generic pointer.
 */
static void *copy (const void* data){
  char *new_data = malloc(sizeof(char)*MAX_WORD);
  if (new_data == NULL){
    return NULL;
  }
  strcpy (new_data, (const char*) data);
  return new_data;
}

/*
 * a pointer to function that gets a pointer of generic data type and
 * returns:
 * - true if it's the last state.
 * - false otherwise.
 */
static bool last(const void *data){
  const char *new_data = (const char*) data;
  int length = strlen(new_data);
  if (new_data[length-1] == '.'){
    return true;
  }
  return false;
}


/**
 * check if a given line is all spaces
 * @param sentence
 * @return true if the given line isn't all spaces and false otherwise
 */
static bool check_space (const char *sentence){
  for (size_t i = 0; i < strlen(sentence) ; i++)
  {
    if (!isspace (sentence[i])){
      return false;
    }
  }
  return true;
}


/**
 * A function that behave like a strdup
 * @param str
 * @return string
 */
static char *strdupp (const char *str){
  char *dup = calloc (1, strlen (str)+1);
  if (!dup){
    return NULL;
  }
  strcpy (dup, str);
  return dup;
}


static int fill_database(FILE *fp, int words_to_read, MarkovChain
*markov_chain){
  char sentence[SENTENCE];
  int count = 0;
  char *word, *new_word;
  while (fgets(sentence, SENTENCE, fp) != NULL) {
    if (count >= words_to_read && words_to_read != ARGC_1){
      break;}
    if (check_space (sentence) == false){
      word = strtok (sentence, " \n\r\t");
      while (word != NULL){
        count += 1;
        if (count >= words_to_read && words_to_read != ARGC_1){
          break;}
        else{
          new_word = strdupp (word);
          Node *first_node = add_to_database (markov_chain, new_word);
          if (first_node == NULL){
            free (new_word);
            return 1;}
          word = strtok(NULL, " \n\t\r");
          free (new_word);
          if (word!= NULL){
            new_word = strdupp (word);
            if (count < words_to_read && words_to_read != ARGC_1){
              Node *second_node = add_to_database (markov_chain, new_word);
              if (second_node == NULL){
                free (new_word);
                return 1;}
              add_node_to_frequencies_list (first_node->data,second_node->data,
                                            markov_chain);
              free (new_word);}
            else{
              Node *second_node = add_to_database (markov_chain, new_word);
              if (second_node == NULL){
                free (new_word);
                return 1;}
              add_node_to_frequencies_list (first_node->data,second_node->data,
                                            markov_chain);
              free (new_word);}
          }
        }
      }
    }
  }
  return 0;
}


/**
 * Initialize the markov chain
 * @return an init markov chain
 */

static MarkovChain *markov_chain_create (){
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  LinkedList *linked_list = malloc (sizeof (LinkedList));
  if (markov_chain == NULL){
    return NULL;
  }
  if (linked_list == NULL){
    free(markov_chain);
    return NULL;
  }
  linked_list->first = NULL;
  linked_list->last = NULL;
  linked_list->size = 0;
  markov_chain->database = linked_list;
  markov_chain->print_func = print_tweets;
  markov_chain->comp_func = is_bigger_equal;
  markov_chain->free_data = data_free;
  markov_chain->copy_func = copy;
  markov_chain->is_last = last;
  return markov_chain;
}

int main (int argc, char *argv[]){
  MarkovChain *markov_chain = markov_chain_create();
  if (markov_chain == NULL){
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  int count = 0;
  int tweet_num = 1;
  if (argc != ARGC_1 && argc != ARGC_2){
    printf ("%s", "Usage: the argc isn't 3 or 4.\n");
    free_database (&markov_chain);
    return EXIT_FAILURE;
  }
  FILE *input_file = fopen(argv[3], "r");
  if (input_file == NULL){
    printf("%s", "Error: the path to file isn't valid.\n");
    free_database (&markov_chain);
    return EXIT_FAILURE;
  }
  if (argc == ARGC_2){
    int num = strtol (argv[4], NULL, BASE);
    if (fill_database (input_file, num, markov_chain) == 1){
      free_database (&markov_chain);
      return EXIT_FAILURE;
    }
  }
  else{
     if (fill_database (input_file, ARGC_1, markov_chain) == 1){
       free_database (&markov_chain);
       return EXIT_FAILURE;
       }
  }
  srand (strtol (argv[1], NULL, BASE));
  while (count++ < strtol (argv[2], NULL, BASE)){
     printf ("Tweet %d:", tweet_num++);
     MarkovNode *new_markov = get_first_random_node (markov_chain);
     generate_tweet (markov_chain, new_markov, MAX_LENGTH);
     }
  fclose (input_file);
  free_database (&markov_chain);
  return EXIT_SUCCESS;
}
