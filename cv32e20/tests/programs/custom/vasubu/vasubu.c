// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matheus Cavalcante <matheusd@iis.ee.ethz.ch>
//         Basile Bougenot <bbougenot@student.ethz.ch>

#include "vector_macros.h"

void TEST_CASE1(void) {
  VSET(4, e32, m1);
  VLOAD_32(v2, 5, 10, 15, 20);
  VLOAD_32(v3, 1, 2, 3, 4);
  __asm__ volatile("vasubu.vv v4, v2, v3" ::);
  VCMP_U32(1, v4, 2, 4, 6, 8);
}

void TEST_CASE3(void) {
  VSET(4, e16, m1);
  VLOAD_16(v2, 5, 10, 15, 20);
  const uint32_t scalar = 5;
  __asm__ volatile("vasubu.vx v4, v2, %[A]" ::[A] "r"(scalar));
  VCMP_U16(3, v4, 0, 3, 5, 8);
}

int main(void) {
  INIT_CHECK();
  // enable_vec();
  TEST_CASE1();
  TEST_CASE3();
  EXIT_CHECK();
}