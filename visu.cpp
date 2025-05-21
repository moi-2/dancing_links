#include "def_dl.h"
#include <set>
#include <stdio.h>
#include <vector>

// Comparator for comparing the contents of the arrays with a given size
struct ArrayComparator {
  size_t array_size;
  ArrayComparator(size_t size) : array_size(size) {}

  bool operator()(const bool *lhs, const bool *rhs) const {
    for (size_t i = 0; i < array_size; ++i) {
      if (lhs[i] != rhs[i]) {
        return lhs[i] < rhs[i];
      }
    }
    return false;
  }
};

// DBG
//  Function to print a matrix
void printMatrix(const std::vector<std::vector<bool>> &matrix) {
  for (const auto &row : matrix) {
    for (bool val : row) {
      printf("%c", val ? '1' : '0');
    }
    printf("\n");
  }
}

// Comparator for comparing the contents of the matrices
struct MatrixComparator {
  bool operator()(const std::vector<std::vector<bool>> &lhs,
                  const std::vector<std::vector<bool>> &rhs) const {
    if (lhs.size() != rhs.size())
      return lhs.size() < rhs.size();
    for (size_t i = 0; i < lhs.size(); ++i) {
      if (lhs[i] != rhs[i])
        return lhs[i] < rhs[i];
    }
    return false; // They are equal
  }
};

// Function to rotate the matrix by 90 degrees
std::vector<std::vector<bool>>
rotateMatrix90(const std::vector<std::vector<bool>> &matrix) {
  if (matrix.empty())
    return {};

  size_t rows = matrix.size();
  size_t cols = matrix[0].size();

  std::vector<std::vector<bool>> rotatedMatrix(cols, std::vector<bool>(rows));

  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      rotatedMatrix[j][rows - 1 - i] = matrix[i][j];
    }
  }

  return rotatedMatrix;
}

int main(void) {
  // grid is 3 by 3 for now
  const std::vector<std::vector<bool>> board = {
      {1, 1, 1, 1, 1, 1, 1, 1 },
      {1, 1, 1, 1, 1, 1, 1, 1 },
      {1, 1, 1, 1, 1, 1, 1, 1 },
      {1, 1, 1, 0, 0, 1, 1, 1 },
      {1, 1, 1, 0, 0, 1, 1, 1 },
      {1, 1, 1, 1, 1, 1, 1, 0 },
      {1, 1, 1, 1, 1, 1, 1, 0 },
      {1, 1, 1, 1, 1, 0, 0, 0 },
  };
  // puzzle form are const for now
  const std::vector<std::vector<std::vector<bool>>> pieces = {
      {
          {1, 1},
          {1, 0},
          {1, 1},
      },
      {
          {1, 1, 1, 1, 1},
      },
      {
          {0, 1, 0},
          {1, 1, 1},
          {0, 1, 0},
      },
      {
          {1, 1, 0},
          {0, 1, 1},
          {0, 0, 1},
      },
      {
          {0, 1, 1},
          {0, 1, 0},
          {1, 1, 0},
      },
      {
          {0, 0, 1},
          {1, 1, 1},
          {0, 1, 0},
      },
      {
          {1, 1, 1},
          {0, 1, 1},
      },
      {
          {1, 0, 0},
          {1, 1, 1},
          {1, 0, 0},
      },
      {
          {0, 0, 1, 0},
          {1, 1, 1, 1},
      },
      {
          {0, 0, 0, 1},
          {1, 1, 1, 1},
      },
      {
          {0, 0, 1, 1},
          {1, 1, 1, 0},
      },
  };

  std::vector<std::set<std::vector<std::vector<bool>>, MatrixComparator>>
      pieces_with_rotations;

  // make pseudo piece that are rotated piece
  for (int i = 0; i < pieces.size(); i++) {
    pieces_with_rotations.push_back(std::set<std::vector<std::vector<bool>>, MatrixComparator>{pieces[i]});
    std::vector<std::vector<bool>> rotated_piece = rotateMatrix90(pieces[i]);
    pieces_with_rotations[i].insert(rotated_piece);
    rotated_piece = rotateMatrix90(rotated_piece);
    pieces_with_rotations[i].insert(rotated_piece);
    rotated_piece = rotateMatrix90(rotated_piece);
    pieces_with_rotations[i].insert(rotated_piece);
  }

  /*
  // Iterate over each set of vectors of vectors of bool
  for (const auto &piece_set : pieces_with_rotations) {
    // Iterate over each vector of vectors of bool in the set
    for (const auto &matrix : piece_set) {
      // Print the matrix
      printMatrix(matrix);
      printf("----\n");
    }
  }
  */

  int square_in_board = 0;
  for (int i = 0; i < board.size(); i++)
    for (int j = 0; j < board.size(); j++)
      if (board[i][j])
        square_in_board++;

  int matrix_row_size = pieces.size() + square_in_board;

  std::set<bool *, ArrayComparator> valid_pieces_pos(matrix_row_size);

  // get valid position
  for (int p_n = 0; p_n < pieces_with_rotations.size(); p_n++) {
    // pour chaque piece
    for (const std::vector<std::vector<bool>> &piece :
         pieces_with_rotations[p_n]) {
      // pour chaque rotation
      int base_index = pieces.size();
      for (int x = 0; x < board.size(); x++) {
        for (int y = 0; y < board[x].size(); y++) {
          // sur chaque case de board
          // ...

          bool *new_row = new bool[matrix_row_size]();
          // () make it filled with false
          new_row[p_n] = true;
          bool valid_piece = true;
          int index = base_index;

          for (int x_p = 0; x_p < piece.size(); x_p++) {
            for (int y_p = 0; y_p < piece[x_p].size(); y_p++) {

              if (piece[x_p][y_p]) {
                // si y a une piece
                if (board.size() <= (x + x_p) or
                    board[x + x_p].size() <= (y + y_p)) {
                  valid_piece = false;
                  break;
                } else {
                  if (board[x + x_p][y + y_p]) {
                    new_row[index] = true;
                    index++;
                  } else {
                    valid_piece = false;
                    break;
                  }
                }
              } else {
                // si on est sur un 0 de la piece on doit index++ si le board
                // est valid
                if (board.size() <= (x + x_p) or
                    board[x + x_p].size() <= (y + y_p)) {
                  continue;
                } else {
                  if (board[x + x_p][y + y_p])
                    index++;
                  continue;
                }
              }
            }
            if (!valid_piece)
              break;
            int square_left_in_row = 0;
            if (x + x_p < board.size())
              for (int s = y + piece[x_p].size(); s < board[x + x_p].size();
                   s++)
                if (board[x + x_p][s])
                  square_left_in_row++;

            int square_in_beg_next_row = 0;
            if (x + x_p + 1 < board.size() and x_p + 1 < piece.size())
              for (int s = 0; s < y; s++)
                if (board[x + x_p + 1][s])
                  square_in_beg_next_row++;

            index += square_left_in_row + square_in_beg_next_row;
          }
          if (board[x][y])
            base_index++;
          if (valid_piece)
            valid_pieces_pos.insert(new_row);
          else
            delete[] new_row;
        }
      }
    }
  }
  
  // Print the content of the set
  //for (const bool *array : valid_pieces_pos) {
  //  for (size_t i = 0; i < matrix_row_size; ++i) {
  //    printf("%b ", array[i]);
  //  }
  //  printf("\n");
  //}

  char pieces_rep[14] = {'#', '@', '%', '$', '0', '+', '=', 'o', 'u', 'x', '*', 'w', '~', '\"'};
  char rep = 'r';
  /*

  for (const bool *array : valid_pieces_pos) {
    for (size_t j = 0; j < pieces.size(); j++)
      if (array[j])
        rep = pieces_rep[j];
    int idx = pieces.size();
    for (int x = 0; x < board.size(); x++) {
      for (int y = 0; y < board.size(); y++) {
        if (board[x][y]) {
          if (array[idx]) {
            printf("%c ", rep);
          } else {
            printf(". ");
          }
          idx++;
        } else {
          printf("  ");
        }
      }
      printf("\n");
    }
    printf("---\n");
  }
  */

  // Create the 2D array
  size_t set_size = valid_pieces_pos.size();
  bool **valid_piece_arr = new bool *[set_size];
  for (size_t i = 0; i < set_size; ++i) {
    valid_piece_arr[i] = new bool[matrix_row_size];
  }

  // Copy the contents from the set to the 2D array
  size_t index = 0;
  for (const bool *array : valid_pieces_pos) {
    for (size_t i = 0; i < matrix_row_size; ++i) {
      valid_piece_arr[index][i] = array[i];
    }
    ++index;
  }

  bool ***def_test;
  printf("\nbegining...\n");
  getRows(&def_test, matrix_row_size, set_size, valid_piece_arr);
  printf("fninshed !\n");

  for (int i = 0; def_test[i] != NULL; i++) {
    for (int j = 0; def_test[i][j] != NULL; j++) {
      for (int k = 0; k < matrix_row_size; k++) {
        printf("%c ", def_test[i][j][k] ? '1' : '0');
      }
      printf("\n");
    }
    printf("---\n");
  }

  for (int s = 0; def_test[s] != NULL; s++) {
    for (int i = 0; def_test[s][i] != NULL; i++) {
      for (size_t j = 0; j < pieces.size(); j++)
        if (def_test[s][i][j])
          rep = pieces_rep[j];

      int idx = pieces.size();
      for (int x = 0; x < board.size(); x++) {
        for (int y = 0; y < board.size(); y++) {
          if (board[x][y]) {
            if (def_test[s][i][idx]) {
              printf("%c ", rep);
            } else {
              printf(". ");
            }
            idx++;
          } else {
            printf("  ");
          }
        }
        printf("\n");
      }
      printf("---\n");
    }
    printf("Other solution\n");
    printf("---\n");
  }

  /*
  for (int i = 0; i < res_to_print.size(); i++) {
    for (int j = 0; j < res_to_print[i].size(); j++) {
      for (int k = 0; k < res_to_print[i][j].size(); k++) {
        printf("%c ", res_to_print[i][j][k]);
      }
      printf("\n");
    }
      printf("---\n");
  }
  */
  

  return 0;
}
