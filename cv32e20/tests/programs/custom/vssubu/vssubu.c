// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matheus Cavalcante <matheusd@iis.ee.ethz.ch>
//         Basile Bougenot <bbougenot@student.ethz.ch>

#include "vector_macros.h"

void TEST_CASE1(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v2, 0xCffffff0, 15, 20, 1583);
  VLOAD_32(v3, 0xfffffff0, 3, 25, 500);
  __asm__ volatile("vssubu.vv v4, v2, v3" ::);
  VCMP_U32(1, v4, 0, 12, 0, 1083);
  read_vxsat(vxsat);
  check_vxsat(1, vxsat, 1);
  reset_vxsat;
}

void TEST_CASE2(void) {
  uint32_t vxsat;
  VSET(4, e16, m1);
  VLOAD_16(v2, 0xFFFF, 0x7FFC, 15, 20);
  VLOAD_16(v3, 1, 0xF0, 3, 25);
  __asm__ volatile("vssubu.vv v4, v2, v3" ::);
  VCMP_U16(2, v4, 0xFFFE, 0x7F0C, 12, 0);
  read_vxsat(vxsat);
  check_vxsat(2, vxsat, 1);
  reset_vxsat;
}

void TEST_CASE3(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v2, 444, 44543528, 15, 5);
  const int32_t scalar = 5;
  __asm__ volatile("vssubu.vx v4, v2, %[A]" ::[A] "r"(scalar));
  VCMP_U32(3, v4, 439, 44543523, 10, 0);
  read_vxsat(vxsat);
  check_vxsat(3, vxsat, 0);
  reset_vxsat;
}


int main(void) {
  INIT_CHECK();
  // enable_vec();
  TEST_CASE1();
  TEST_CASE2();
  TEST_CASE3();
  EXIT_CHECK();
}
