#include "markov_chain.h"
#include "linked_list.h"
#include <string.h>



Node* add_to_database(MarkovChain *markov_chain, char *data_ptr){
  Node *node = get_node_from_database (markov_chain, data_ptr);
  if (node != NULL){
    return node;
  }
  MarkovNode *markov_node = malloc (sizeof (MarkovNode)*1);
  char *new_data = malloc (sizeof (char) * (strlen (data_ptr)+1));
  if (markov_node == NULL || new_data == NULL){
    return NULL;
  }
  strcpy (new_data, data_ptr);
  markov_node->data = new_data;
  markov_node->frequencies_list = NULL;
  markov_node->length = 0;
  if (add (markov_chain->database, markov_node) == 1){
    return NULL;
  }
  return markov_chain->database->last;
}


Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
  Node *wrapping_node_1 = markov_chain->database->first;
  if (wrapping_node_1 != NULL){
    while (wrapping_node_1 != NULL){
      if (strcmp (data_ptr, wrapping_node_1->data->data) == 0){
        return wrapping_node_1;
      }
      wrapping_node_1 = wrapping_node_1->next;
    }
  }
  return NULL;
}

/**
 * checks if the second node is already in the frequency list of the first node
 * @param first_node
 * @param second_node
 * @return true if it is already in the list and false otherwise
 */


bool second_node_exist (MarkovNode *first_node, MarkovNode *second_node){
  for (int i = 0; i < first_node->length ; i++){
    char *data = first_node->frequencies_list[i].word->data;
    if (strcmp (second_node->data, data) == 0){
      first_node->frequencies_list[i].frequency+=1;
      return true;
    }
  }
  return false;
}

bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node){
  if (first_node->length == 0){
    first_node->frequencies_list = malloc (sizeof (MarkovNodeFrequency));
    first_node->length = 1;
    first_node->frequencies_list->frequency = 0;
    if (first_node->frequencies_list == NULL){
      printf ("%s", "Allocation failure: the malloc failed");
      return false;
    }
  }
  else if (second_node_exist (first_node, second_node)){
    return true;
  }
  else if (first_node->length != 0){
    first_node->length +=1;
    MarkovNodeFrequency *new_list = realloc (first_node->frequencies_list,
                                            (first_node->length) * sizeof
                                            (MarkovNodeFrequency));
    if (new_list == NULL){
      printf ("%s", "Allocation failure: the reallocate failed");
      return false;
    }
    first_node->frequencies_list = new_list;
  }
  first_node->frequencies_list[first_node->length-1].word = second_node;
  first_node->frequencies_list[first_node->length-1].frequency = 1;
  return true;
}




void free_database(MarkovChain ** ptr_chain){
  if (ptr_chain != NULL){
    if ((*ptr_chain) != NULL){
      if ((*ptr_chain)->database != NULL){
        if ((*ptr_chain)->database->first != NULL){
          Node *point = (*ptr_chain)->database->first;
          while (point != NULL){
            if (point->data->frequencies_list != NULL){
              free (point->data->frequencies_list);
              point->data->frequencies_list = NULL;
            }
            if (point->data->data != NULL){
              free (point->data->data);
              point->data->data = NULL;
            }
            free (point->data);
            point->data = NULL;
            point = point->next;
          }
          free ((*ptr_chain)->database->first);
          (*ptr_chain)->database->first = NULL;
          free ((*ptr_chain)->database->last);
          (*ptr_chain)->database->last = NULL;
        }
        free ((*ptr_chain)->database);
        (*ptr_chain)->database = NULL;
      }
      free ((*ptr_chain));
      (*ptr_chain) = NULL;
    }
  }
}



/**
 * return a random number
 * @param max_number
 * @return
 */

int get_random_number (int max_number){
  return rand() % max_number;
}

/**
 * An helper method that check if the given node is the last word in the
 * sentence and if yes, it is calculate other node
 * @param count
 * @param num
 * @param node
 * @param size
 * @param length
 * @param prev
 * @return the random node
 */
MarkovNode* get_first_random_node_1(int count, int num, Node *node, int
size, int length, Node *prev){
  if (node->data->data[length-1] != '.'){
    return node->data;
  }
  count = 0;
  num = get_random_number (size);
  node = prev;
  while (count++ < num && node->next != NULL){
    node = node->next;
  }
  return get_first_random_node_1 (count, num, node, size, strlen
  (node->data->data), prev);
}


MarkovNode* get_first_random_node(MarkovChain *markov_chain){
  int num = get_random_number (markov_chain->database->size);
  int count = 0;
  Node *node = markov_chain->database->first;
  while (count++ < num && node->next != NULL){

    node = node->next;
  }
  return get_first_random_node_1 (0, 0, node,
                                  markov_chain->database->size,
                                  strlen (node->data->data),
                                  markov_chain->database->first);
}



MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr){
  int sum = 0;
  int num = 0;
  int index = 0;
  for (int i = 0; i < state_struct_ptr->length; i++)
  {
    sum += state_struct_ptr->frequencies_list[i].frequency;
  }
  if (sum != 0){
    num = get_random_number (sum);
  }
  while (num >= state_struct_ptr->frequencies_list[index].frequency){
    num -= state_struct_ptr->frequencies_list[index].frequency;
    index++;
  }
  return state_struct_ptr->frequencies_list[index].word;
}



void generate_tweet(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length){
  int count = 1;
  if (first_node == NULL){
    first_node = get_first_random_node (markov_chain);
  }
  MarkovNode *check = first_node;
  while (count <= max_length){
    if (check->data[strlen(check->data)-1] == '.'){
      printf (" %s\n", check->data);
      break;
    }
    if (count == max_length){
      printf (" %s\n", check->data);
    }
    else{
      printf (" %s", check->data);
    }
    count++;
    if (check->length == 0){
      break;
    }
    check = get_next_random_node (check);
  }
}