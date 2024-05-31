// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matteo Perotti <mperotti@iis.ee.ethz.ch>

#include "vector_macros.h"

// Naive test
void TEST_CASE1(void) {
  // VSET(16, e8, m1);
  // VLOAD_8(v16, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8);
  // VLOAD_8(v8, 1);
  // asm volatile("vredsum.vs v24, v16, v8");
  // VCMP_U8(1, v24, 73);

  volatile uint8_t Vv16[4] = {1, 2, 3, 4}; 
  volatile uint8_t Vv8[4]  = {0, 0, 0, 0};
  VSET(4, e8, m1);
  asm volatile ("vl""e8"".v ""v16"", (%0)  \n":: [V] "r"(Vv16));
  asm volatile ("vl""e8"".v ""v8"", (%0)  \n":: [V] "r"(Vv8));
  VSET(4, e8, m1);
  asm volatile("vredsum.vs v24, v16, v8");
  uint8_t sum;
  asm volatile("vmv.x.s %[sum], v24" : [sum] "=r"(sum));
  if (sum != 10) {
    printf("vredsum.vs or vmv.x.s failed for SEW=8. sum = %d instead of 10\n", sum);
  }
  VCMP_U8(1, v24, 10);

  // VSET(16, e32, m4);
  // VLOAD_32(v16, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8);
  // VLOAD_32(v8, 1);
  // asm volatile("vredsum.vs v24, v16, v8");
  // VCMP_U32(3, v24, 73);
}

void TEST_CASE2(void) {
  volatile uint16_t Vv16[4] = {2, 3, 4, 5}; 
  volatile uint16_t Vv8[4]  = {0, 0, 0, 0};
  VSET(4, e16, m1);
  asm volatile ("vl""e16"".v ""v16"", (%0)  \n":: [V] "r"(Vv16));
  asm volatile ("vl""e16"".v ""v8"", (%0)  \n":: [V] "r"(Vv8));
  VSET(4, e16, m1);
  asm volatile("vredsum.vs v24, v16, v8");
  VCMP_U16(2, v24, 14);
}

void TEST_CASE3(void) {
  volatile uint32_t Vv16[4]; 
  volatile uint32_t Vv8[4];
  for (int i = 0; i < 4; i++) {
    Vv16[i] = i + 3;
    Vv8[i] = 0;
  }
  
  VSET(4, e32, m2);
  asm volatile ("vl""e32"".v ""v16"", (%0)  \n":: [V] "r"(Vv16));
  asm volatile ("vl""e32"".v ""v8"", (%0)  \n":: [V] "r"(Vv8));
  VSET(4, e32, m2);
  asm volatile("vredsum.vs v24, v16, v8");
  VCMP_U32(3, v24, 18);
}

int main(void) {
  INIT_CHECK();
  // enable_vec();

  TEST_CASE1();
  TEST_CASE2();
  TEST_CASE3();

  EXIT_CHECK();
}
