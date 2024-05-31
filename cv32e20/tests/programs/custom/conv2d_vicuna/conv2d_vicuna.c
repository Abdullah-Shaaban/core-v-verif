// Copyright 2020 ETH Zurich and University of Bologna.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Author: Matteo Perotti

#include <stdint.h>
#include <stdio.h>


// Define Matrix dimensions:
// o = i ° f, with i=[MxN], f=[FxF], o=[MxN]
// The filter is a square matrix, and F is odd
#define M 32
#define N 32
#define F 3

// Matrices defined in data.S
extern int8_t image[] __attribute__((aligned(4)));    // [ (M+2) * (N+2) ]
extern int8_t filter[] __attribute__((aligned(4)));   // [ F*F ]
extern int8_t output[] __attribute__((aligned(4)));   // [ M*N ]
extern int8_t golden_o[] __attribute__((aligned(4))); // [ M*N ]

void print_matrix(int8_t const *matrix, uint8_t num_rows, uint8_t num_columns) {
  printf("0x%8X\n", (uint32_t)matrix);
  for (uint8_t i = 0; i < num_rows; ++i) {
    for (uint8_t j = 0; j < num_columns; ++j) {
      printf("%3X ", (uint8_t)matrix[i * num_columns + j]);
    }
    printf("\n");
  }
}

extern void conv3x3(int8_t *output, int8_t *image, int8_t *filter, int img_size);

int main() {

  // Print input matrices
//   printf("Filter:\n");
//   print_matrix(filter, F, F);
//   printf("Image:\n");
//   print_matrix(image, M+2, N+2);

  // Prepare to call the convolution function
  int img_size = (M+2) * (N+2); // Padded

  // Execute convolution
  conv3x3(output, image, filter, img_size);

  // Verify correctness
  printf("Verifying result...\n");
  int error = 0;
  for (uint32_t indx = 0; indx < M * N; indx++) {
    if (output[indx] != golden_o[indx]) {
      // printf("Error at index %d: %d != %d\n", indx, output[indx], golden_o[indx]);
      error +=1;
    }
  }
  if (error != 0) {
    printf("Test failed. Number of errors: %d\n", error);
    return 1;
  }
  return 0;
}