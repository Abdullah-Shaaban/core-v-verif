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
#include "printf.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void matmul(int32_t *c, const int32_t *a, const int32_t *b,
                            const unsigned int N, const unsigned int P,
                            unsigned int vl) {
  // Set VL
  asm volatile("vsetvli zero, %0, e32, m2, ta, ma" ::"r"(vl));

  // Temporary variables
  int32_t t0, t1, t2, t3, t4, t5, t6, t7;
  int32_t *a_ = (int32_t *)a;
  int32_t *b_ = (int32_t *)b;
  int32_t *c_ = (int32_t *)c;

  int32_t *a__ = a_;

  // Compute the multiplication
  unsigned int n = 0;

  t0 = *a__, a__ += N;
  t1 = *a__, a__ += N;
  t2 = *a__, a__ += N;
  t3 = *a__, a__ += N;
  t4 = *a__, a__ += N;
  t5 = *a__, a__ += N;
  t6 = *a__, a__ += N;
  t7 = *a__;

  // Calculate pointer to the matrix A
  a__ = a_ + ++n;

  asm volatile("vle32.v v16, (%0);" ::"r"(b_));
  b_ += P;
  asm volatile("vmv.v.i v0,  0");
  asm volatile("vmacc.vx v0, %0, v16" ::"r"(t0));
  t0 = *a__, a__ += N;
  asm volatile("vmv.v.i v2,  0");
  asm volatile("vmacc.vx v2, %0, v16" ::"r"(t1));
  t1 = *a__, a__ += N;
  asm volatile("vmv.v.i v4,  0");
  asm volatile("vmacc.vx v4, %0, v16" ::"r"(t2));
  t2 = *a__, a__ += N;
  asm volatile("vmv.v.i v6,  0");
  asm volatile("vmacc.vx v6, %0, v16" ::"r"(t3));
  t3 = *a__, a__ += N;

  // Load one row of B
  asm volatile("vle32.v v18, (%0);" ::"r"(b_));
  b_ += P;

  asm volatile("vmv.v.i v8,  0");
  asm volatile("vmacc.vx v8, %0, v16" ::"r"(t4));
  t4 = *a__, a__ += N;
  asm volatile("vmv.v.i v10,  0");
  asm volatile("vmacc.vx v10, %0, v16" ::"r"(t5));
  t5 = *a__, a__ += N;
  asm volatile("vmv.v.i v12,  0");
  asm volatile("vmacc.vx v12, %0, v16" ::"r"(t6));
  t6 = *a__, a__ += N;
  asm volatile("vmv.v.i v14,  0");
  asm volatile("vmacc.vx v14, %0, v16" ::"r"(t7));
  t7 = *a__;

  // Calculate pointer to the matrix A
  a__ = a_ + ++n;

  while (n < N) {
    // Load one row of B
    asm volatile("vle32.v v16, (%0);" ::"r"(b_));
    b_ += P;

    asm volatile("vmacc.vx v0, %0, v18" ::"r"(t0));
    t0 = *a__, a__ += N;
    asm volatile("vmacc.vx v2, %0, v18" ::"r"(t1));
    t1 = *a__, a__ += N;
    asm volatile("vmacc.vx v4, %0, v18" ::"r"(t2));
    t2 = *a__, a__ += N;
    asm volatile("vmacc.vx v6, %0, v18" ::"r"(t3));
    t3 = *a__, a__ += N;
    asm volatile("vmacc.vx v8, %0, v18" ::"r"(t4));
    t4 = *a__, a__ += N;
    asm volatile("vmacc.vx v10, %0, v18" ::"r"(t5));
    t5 = *a__, a__ += N;
    asm volatile("vmacc.vx v12, %0, v18" ::"r"(t6));
    t6 = *a__, a__ += N;
    asm volatile("vmacc.vx v14, %0, v18" ::"r"(t7));
    t7 = *a__;

    // Calculate pointer to the matrix A
    a__ = a_ + ++n;

    // Load one row of B
    asm volatile("vle32.v v18, (%0);" ::"r"(b_));
    b_ += P;

    asm volatile("vmacc.vx v0, %0, v16" ::"r"(t0));
    t0 = *a__, a__ += N;
    asm volatile("vmacc.vx v2, %0, v16" ::"r"(t1));
    t1 = *a__, a__ += N;
    asm volatile("vmacc.vx v4, %0, v16" ::"r"(t2));
    t2 = *a__, a__ += N;
    asm volatile("vmacc.vx v6, %0, v16" ::"r"(t3));
    t3 = *a__, a__ += N;
    asm volatile("vmacc.vx v8, %0, v16" ::"r"(t4));
    t4 = *a__, a__ += N;
    asm volatile("vmacc.vx v10, %0, v16" ::"r"(t5));
    t5 = *a__, a__ += N;
    asm volatile("vmacc.vx v12, %0, v16" ::"r"(t6));
    t6 = *a__, a__ += N;
    asm volatile("vmacc.vx v14, %0, v16" ::"r"(t7));
    t7 = *a__;

    // Calculate pointer to the matrix A
    a__ = a_ + ++n;
  }

  asm volatile("vmacc.vx v0, %0, v18" ::"r"(t0));
  asm volatile("vse32.v v0, (%0);" ::"r"(c_));
  c_ += P;
  asm volatile("vmacc.vx v2, %0, v18" ::"r"(t1));
  asm volatile("vse32.v v2, (%0);" ::"r"(c_));
  c_ += P;
  asm volatile("vmacc.vx v4, %0, v18" ::"r"(t2));
  asm volatile("vse32.v v4, (%0);" ::"r"(c_));
  c_ += P;
  asm volatile("vmacc.vx v6, %0, v18" ::"r"(t3));
  asm volatile("vse32.v v6, (%0);" ::"r"(c_));
  c_ += P;
  asm volatile("vmacc.vx v8, %0, v18" ::"r"(t4));
  asm volatile("vse32.v v8, (%0);" ::"r"(c_));
  c_ += P;
  asm volatile("vmacc.vx v10, %0, v18" ::"r"(t5));
  asm volatile("vse32.v v10, (%0);" ::"r"(c_));
  c_ += P;
  asm volatile("vmacc.vx v12, %0, v18" ::"r"(t6));
  asm volatile("vse32.v v12, (%0);" ::"r"(c_));
  c_ += P;
  asm volatile("vmacc.vx v14, %0, v18" ::"r"(t7));
  asm volatile("vse32.v v14, (%0);" ::"r"(c_));
  c_ += P;
}

// Matrices defined in data.S
extern int AVL __attribute__((aligned(4))); // Dimensions of the matrices
extern int8_t matrix1[] __attribute__((aligned(4)));
extern int8_t matrix2[] __attribute__((aligned(4)));
extern int8_t golden_o[] __attribute__((aligned(4)));
extern int8_t output[] __attribute__((aligned(4)));

int main() {
  
  // gcc sometimes uses vector instructions to assign the date. So, we allow time before starting the benchmark 
    printf("Starting the benchmark...\n");
    
    // Issue fence instruction to start benchmark counter
    asm volatile("fence");
    asm volatile("fence");

    // Perform the operation
    matmul(matrix1, matrix2, output, AVL);
    
    // Issue fence instruction to stop benchmark counter
    asm volatile("fence");

    // Check the result
    int error = 0;
    for (int i = 0; i < AVL; i++) {
        for (int j = 0; j < AVL; j++) {
            if (output[i*AVL + j] != golden_o[i*AVL + j]) {
                error += 1;
                // printf("Error at [%d][%d]: c_vec[%d][%d]= %d, but c_ref[%d][%d] = %d\n", i, j, i, j, c_vec[i][j], i, j, c_ref[i][j]);
            }
        }
    }

    if (error>0) {
        printf("Test failed. Number of errors: %d\n", error);
        return 1;
    } else {
        return 0;
    }

  return 0;
}
