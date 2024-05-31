// Copyright 2021 ETH Zurich and University of Bologna.
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

// Author: Domenic Wüthrich, ETH Zurich

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void matmul_4xVL(int8_t *c, const int8_t *a, const int8_t *b,
                 const unsigned int m_start, const unsigned int m_end,
                 const unsigned int N, const unsigned int P,
                 const unsigned int p_start, const unsigned int p_end) {

  unsigned int p = p_start;

  // Set the vector configuration
  uint32_t block_size_p;
  asm volatile("vsetvli %0, %1, e8, m4, ta, ma" : "=r"(block_size_p) : "r"(P));

  while (p < p_end) {
    // Calculate the vl
    uint32_t gvl;
    const uint32_t p_ = MIN(P - p, block_size_p);
    asm volatile("vsetvli %[gvl], %[vl], e8, m4, ta, ma"
                 : [gvl] "=r"(gvl)
                 : [vl] "r"(p_));
                //  : [vl] "r"(p_end - p));

    const int8_t *b_ = b + p;
    int8_t *c_ = c + p;

    for (unsigned int m = m_start; m < m_end; m += 4) {

      // Slice init
      asm volatile("vmv.v.i v0,  0");
      asm volatile("vmv.v.i v4,  0");
      asm volatile("vmv.v.i v8,  0");
      asm volatile("vmv.v.i v12, 0");

      const int8_t *a_ = a + m * N;
      const int8_t *a__ = a_;

      asm volatile("vle8.v v16, (%0);" ::"r"(b_));
      const int8_t *b__ = b_ + P;

      int8_t *c__ = c_ + m * P;

      int8_t t0, t1, t2, t3;

      t0 = *a__;
      a__ += N;
      t1 = *a__;
      a__ += N;
      t2 = *a__;
      a__ += N;
      t3 = *a__;

      unsigned int n = 0;

      while (n < N) {
        asm volatile("vle8.v v20, (%0);" ::"r"(b__));
        b__ += P;

        a__ = a_ + ++n;

        // if (n == 1) {
        //   asm volatile("vmul.vx v0, v16, %0" ::"r"(t0));
        //   t0 = *a__;
        //   a__ += N;
        //   asm volatile("vmul.vx v4, v16, %0" ::"r"(t1));
        //   t1 = *a__;
        //   a__ += N;
        //   asm volatile("vmul.vx v8, v16, %0" ::"r"(t2));
        //   t2 = *a__;
        //   a__ += N;
        //   asm volatile("vmul.vx v12, v16, %0" ::"r"(t3));
        //   t3 = *a__;
        // } else {
          asm volatile("vmacc.vx v0, %0, v16" ::"r"(t0));
          t0 = *a__;
          a__ += N;
          asm volatile("vmacc.vx v4, %0, v16" ::"r"(t1));
          t1 = *a__;
          a__ += N;
          asm volatile("vmacc.vx v8, %0, v16" ::"r"(t2));
          t2 = *a__;
          a__ += N;
          asm volatile("vmacc.vx v12, %0, v16" ::"r"(t3));
          t3 = *a__;
        // }

        a__ = a_ + ++n;

        if (n == N)
          break;

        asm volatile("vle8.v v16, (%0);" ::"r"(b__));
        b__ += P;

        asm volatile("vmacc.vx v0, %0, v20" ::"r"(t0));
        t0 = *a__;
        a__ += N;
        asm volatile("vmacc.vx v4, %0, v20" ::"r"(t1));
        t1 = *a__;
        a__ += N;
        asm volatile("vmacc.vx v8, %0, v20" ::"r"(t2));
        t2 = *a__;
        a__ += N;
        asm volatile("vmacc.vx v12, %0, v20" ::"r"(t3));
        t3 = *a__;
      }

      asm volatile("vmacc.vx v0, %0, v20" ::"r"(t0));
      asm volatile("vse8.v v0, (%0);" ::"r"(c__));
      c__ += P;
      asm volatile("vmacc.vx v4, %0, v20" ::"r"(t1));
      asm volatile("vse8.v v4, (%0);" ::"r"(c__));
      c__ += P;
      asm volatile("vmacc.vx v8, %0, v20" ::"r"(t2));
      asm volatile("vse8.v v8, (%0);" ::"r"(c__));
      c__ += P;
      asm volatile("vmacc.vx v12, %0, v20" ::"r"(t3));
      asm volatile("vse8.v v12, (%0);" ::"r"(c__));
    }

    p += gvl;
  }
}

// Matrices defined in data.S
extern int AVL __attribute__((aligned(4))); // Dimensions of the matrices
extern int8_t matrix1[] __attribute__((aligned(4)));
extern int8_t matrix2[] __attribute__((aligned(4)));
extern int8_t golden_o[] __attribute__((aligned(4)));
extern int8_t output[] __attribute__((aligned(4)));

void print_matrix(int8_t const *matrix, uint32_t num_rows, uint32_t num_columns) {
  printf("0x%8X\n", (unsigned int)matrix);
  for (uint32_t i = 0; i < num_rows; ++i) {
    for (uint32_t j = 0; j < num_columns; ++j) {
      printf("%2X ", (uint8_t)matrix[i * num_columns + j]);
    }
    printf("\n");
  }
}

int main() {
  
    // Print inputs
//   printf("Matrix size: %dx%d\n", AVL, AVL);
//   print_matrix(matrix1, AVL, AVL);
//   print_matrix(matrix2, AVL, AVL);

  
  // Issue fence instruction to start benchmark counter
  asm volatile("fence");
  asm volatile("fence");

  // Perform the operation
  matmul_4xVL(output, matrix1, matrix2, 0, AVL, AVL, AVL, 0, AVL);
  
  // Issue fence instruction to stop benchmark counter
  asm volatile("fence");

  // Check the result
  // int error = 0;
  // for (int indx = 0; indx < AVL*AVL; indx++) {
  //   if (output[indx] != golden_o[indx]) {
  //   //   printf("Error at index %d: Expected %X, Got %X\n", indx, (uint8_t)golden_o[indx], (uint8_t)output[indx]);
  //     error++;
  //   }
  // }

  // if (error>0) {
  //   printf("\n\nTest failed. Number of errors: %d\n\n", error);
  //   return 1;
  // } else {
  //   printf("\n\nTest succeeded!!\n\n");
  // }
  return 0;
}
