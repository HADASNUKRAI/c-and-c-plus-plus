#include "markov_chain.h"
#include "linked_list.h"
#include <string.h>



Node* add_to_database(MarkovChain *markov_chain, void *data_ptr){
  Node *node = get_node_from_database (markov_chain, data_ptr);
  if (node != NULL){
//      markov_chain->free_data (data_ptr);
    return node;
  }
  MarkovNode *markov_node = malloc (sizeof (MarkovNode)*1);
  void *new_data = markov_chain->copy_func(data_ptr);
  if( new_data == NULL || markov_node == NULL){
//      markov_chain->free_data (data_ptr);
    return NULL;
  }
  markov_node->data = new_data;
  markov_node->frequencies_list = NULL;
  markov_node->length = 0;
  if (add (markov_chain->database, markov_node) == 1){
    return NULL;
  }
//  markov_chain->free_data (data_ptr);
  return markov_chain->database->last;
}


Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
  Node *wrapping_node_1 = markov_chain->database->first;
  if (wrapping_node_1 != NULL){
    while (wrapping_node_1 != NULL){
      int res = markov_chain->comp_func(data_ptr, wrapping_node_1->data->data);
      if (res == 0){
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

bool second_node_exist (MarkovNode *first_node, MarkovNode *second_node,
                        MarkovChain *markov_chain){
  for (int i = 0; i < first_node->length ; i++){
    void *first_data = first_node->frequencies_list[i].word->data;
    int answer = markov_chain->comp_func(second_node->data, first_data);
    if (answer == 0){
      first_node->frequencies_list[i].frequency+=1;
      return true;
    }
  }
  return false;
}

bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
  if (first_node->length == 0){
    first_node->frequencies_list = malloc (sizeof (MarkovNodeFrequency));
    first_node->length = 1;
    first_node->frequencies_list->frequency = 0;
    if (first_node->frequencies_list == NULL){
      printf ("%s", "Allocation failure: the malloc failed");
      return false;
    }
  }
  else if (second_node_exist (first_node, second_node, markov_chain)){
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
              (*ptr_chain)->free_data (point->data->frequencies_list);
              point->data->frequencies_list = NULL;
            }
            if (point->data->data != NULL){
              (*ptr_chain)->free_data(point->data->data);
              point->data->data = NULL;
            }
            (*ptr_chain)->free_data (point->data);
            Node *next_node = point->next;
            point->data = NULL;
            free (point);
            point = next_node;
          }
        }
        (*ptr_chain)->free_data ((*ptr_chain)->database);
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
size, Node *prev, MarkovChain *markov_chain){
  if (markov_chain->is_last (node->data->data) == false){
    return node->data;
  }
  count = 0;
  num = get_random_number (size);
  node = prev;
  while (count++ < num && node->next != NULL){
    node = node->next;
  }
  return get_first_random_node_1 (count, num, node, size, prev, markov_chain);
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
                                  markov_chain->database->first, markov_chain);
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
    if (markov_chain->is_last(check->data)){
      markov_chain->print_func (check->data);
      printf("\n");
      break;
    }
    if (count == max_length){
      markov_chain->print_func (check->data);
      printf("\n");
    }
    else{
      markov_chain->print_func (check->data);
    }
    count++;
    if (check->length == 0){
      break;
    }
    check = get_next_random_node (check);
  }
}