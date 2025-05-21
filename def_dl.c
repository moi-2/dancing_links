#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "def_dl.h"

#define DEBUG false

typedef struct node node;

struct node {
  struct node *up;
  struct node *down;
  struct node *right;
  struct node *left;
  struct node *column;
  int size;
  char col_num;
#if DEBUG
  char position[2];
#endif
};

void coverCol(node *c) {
  // remove column from header list
  c->right->left = c->left;
  c->left->right = c->right;
  for (node *i = c->down; i != c; i = i->down) {
    for (node *j = i->right; j != i; j = j->right) {
      // remove node in the same row as node from column c
      j->down->up = j->up;
      j->up->down = j->down;
      // update column size
      j->column->size -= 1;
    }
  }
}

void uncoverCoverCol(node *c) {
  for (node *i = c->up; i != c; i = i->up) {
    for (node *j = i->left; j != i; j = j->left) {
      j->column->size += 1;
      j->down->up = j;
      j->up->down = j;
    }
  }
  c->right->left = c;
  c->left->right = c;
}

// append bool array to 2D bool array
bool **appendOneSolution(bool **array, int *length, bool *newElement) {
  // Increase the length by one
  *length += 1;

  if (*length == 1) {
    array = (bool **)malloc(sizeof(bool *));
  } else {

    // Reallocate memory to hold the new element
    array = (bool **)realloc(array, (*length) * sizeof(bool *));
    if (*array == NULL) {
      printf("Memory reallocation failed\n");
      exit(1); // Exit if realloc fails
    }
  }
  // Add the new element to the end of the array
  array[*length - 1] = newElement;

  return array;
}

//append 2D bool to 3D bool
//newElement => 2D bool
bool ****appendSolution(bool ****array, int *length, bool **newElement) {
  // Increase the length by one
  *length += 1;

  if (*length == 1) {
    *array = (bool ***)malloc(sizeof(bool **));
  } else {

    // Reallocate memory to hold the new element
    *array = (bool ***)realloc(*array, (*length) * sizeof(bool **));
    if (*array == NULL) {
      printf("Memory reallocation failed\n");
      exit(1); // Exit if realloc fails
    }
  }
  // Add the new element to the end of the array
  (*array)[*length - 1] = newElement;

  return array;
}

void solve(bool ****result_row, int *res_len, node *root, int k, node *Ok[],
           int num_of_cols) {
  if (root->right == root) {
    bool **one_solution;
    int solution_len = 0;
    for (int i = 0; i < k; i++) {
      node *j = Ok[i];
      bool *res_piece = (bool *)calloc(num_of_cols, sizeof(bool));
      while (true) {
        // append result
        res_piece[j->column->col_num] = true;
#if DEBUG
        printf("%c", j->column->col_num + 'A');
#endif
        j = j->right;
        if (j == Ok[i])
          break;
      }
      one_solution = appendOneSolution(one_solution, &solution_len, res_piece);
#if DEBUG
      printf("\n");
#endif
    }
    one_solution = appendOneSolution(one_solution, &solution_len, NULL);
    result_row = appendSolution(result_row, res_len, one_solution);
#if DEBUG
    printf("---\n");
#endif
    return;
  }
  // choose a column
  node *c = root->right;

  coverCol(c);
  for (node *r = c->down; r != c; r = r->down) {
    // set Ok <- r
    Ok[k] = r;
    for (node *j = r->right; j != r; j = j->right) {
      coverCol(j->column);
    }
    solve(result_row, res_len, root, k + 1, Ok, num_of_cols);
    // set r <- Ok and c <- (r->column)
    r = Ok[k];
    c = r->column;
    for (node *j = r->left; j != r; j = j->left) {
      uncoverCoverCol(j->column);
    }
  }
  uncoverCoverCol(c);
  return;
}

void createNodes(bool ****result_row, node *root, int num_of_rows,
                 int num_of_cols, bool **matrix) {
  root = (node *)malloc(sizeof(node));

  node *pre_node = root;
  node *columns[num_of_cols];
  node *last_nodes_above[num_of_cols];

  // make column LinkedList
  for (int i = 0; i < num_of_cols; i++) {
    node *col = (node *)malloc(sizeof(node));
    col->col_num = i;
    col->size = 0;

    // link columns
    col->left = pre_node;
    pre_node->right = col;

    pre_node = col;
    columns[i] = col;
    last_nodes_above[i] = col;
  }
  // make linked list circular
  root->left = columns[num_of_cols - 1];
  columns[num_of_cols - 1]->right = root;

  pre_node = NULL;

  for (int i = 0; i < num_of_rows; i++) {
    node *first_node_of_row = NULL;
    for (int j = 0; j < num_of_cols; j++) {
      if (matrix[i][j]) {
        // create node
        node *new_node = (node *)malloc(sizeof(node));
#if DEBUG
        new_node->position[0] = i;
        new_node->position[1] = j;
#endif
        // save first node of row to link it with last of row
        if (first_node_of_row == NULL)
          first_node_of_row = new_node;
        new_node->column = columns[j];
        columns[j]->size += 1;
        // link up down
        new_node->up = last_nodes_above[j];
        last_nodes_above[j]->down = new_node;

        // if not first in row make links
        if (pre_node != NULL) {
          new_node->left = pre_node;
          pre_node->right = new_node;
        }

        // save new_node for next loop
        pre_node = new_node;
        // update wich node is above in column j
        last_nodes_above[j] = new_node;
      }
    }
    // if at least one node in row make LinkedList circular
    if (pre_node != NULL) {
      // pre_node is last node of row here
      pre_node->right = first_node_of_row;
      first_node_of_row->left = pre_node;
    }
    pre_node = NULL;
  }
  // make all last nodes of each column linked with column to make it circular
  for (int j = 0; j < num_of_cols; j++) {
    last_nodes_above[j]->down = columns[j];
    columns[j]->up = last_nodes_above[j];
  }

  node *oui[num_of_rows];
  int res_len = 0;
  solve(result_row, &res_len, root, 0, oui, num_of_cols);
  result_row = appendSolution(result_row, &res_len, NULL);

  return;
}

void getRows(bool ****result_solutions, int m_size_row, int m_size_col,
             bool **matrix) {
  node *root;
  createNodes(result_solutions, root, m_size_col, m_size_row, matrix);
}

/*int main(void) {
  const int m_size_row = 4;
  const int m_size_col = 5;
  bool matrix[4][5] = {{true, false, false, true, false},
                       {true, false, false, true, false},
                       {true, true, false, true, false},
                       {false, false, true, false, true}};

    // Pointer to pointers (double pointer)
    bool **ptr = (bool **)malloc(m_size_col * sizeof(bool *));

    // Convert array of arrays to pointer of pointers
    for (int i = 0; i < m_size_row; i++) {
        ptr[i] = matrix[i];
    }

  node *root;
  createNodes(root, m_size_row, m_size_col, ptr);
}
*/
