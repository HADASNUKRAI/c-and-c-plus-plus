#include "markov_chain.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ARGC_1 4
#define ARGC_2 5
#define SENTENCE 1000
#define BASE 10


/**
 * check if a given line is all spaces
 * @param sentence
 * @return true if the given line isn't all spaces and false otherwise
 */
bool check_space (char *sentence){
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
char *strdupp (const char *str){
  char *dup = calloc (1, strlen (str)+1);
  if (!dup){
    return NULL;
  }
  strcpy (dup, str);
  return dup;
}


int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain){
  char sentence[SENTENCE];
  int count = 0;
  char *word;
  while (fgets(sentence, SENTENCE, fp) != NULL) {
    if (count >= words_to_read && words_to_read != ARGC_1){
      break;
    }
    if (check_space (sentence) == false){
      word = strtok (sentence, " \n\r\t");
      while (word != NULL){
        count += 1;
        if (count >= words_to_read && words_to_read != ARGC_1){
          break;
        }
        else{
          char *new_word = strdupp (word);
          Node *first_node = add_to_database (markov_chain, new_word);
          word = strtok(NULL, " \n\t\r");
          if (word!= NULL){
            new_word = strdupp (word);
            if (count < words_to_read && words_to_read != ARGC_1){
              Node *second_node = add_to_database (markov_chain, new_word);
              add_node_to_frequencies_list (first_node->data,
                                            second_node->data);
              if (first_node == NULL || second_node == NULL){
                return 1;
              }
            }
            else{
              Node *second_node = add_to_database (markov_chain, new_word);
              add_node_to_frequencies_list (first_node->data,
                                            second_node->data);
              if (first_node == NULL || second_node == NULL){
                return 1;
              }
            }
          }
        }
      }
    }
    else{
      continue;
    }
  }
  return 0;
}


/**
 * Initialize the markov chain
 * @return an init markov chain
 */

MarkovChain *markov_chain_create (){
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  LinkedList *linked_list = malloc (sizeof (LinkedList));
  linked_list->first = NULL;
  linked_list->last = NULL;
  linked_list->size = 0;
  markov_chain->database = linked_list;
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








