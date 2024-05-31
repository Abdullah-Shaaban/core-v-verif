// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matheus Cavalcante <matheusd@iis.ee.ethz.ch>
//         Basile Bougenot <bbougenot@student.ethz.ch>

#include "vector_macros.h"

void TEST_CASE1(void) {
  uint32_t vxsat;
  VSET(4, e8, m1);
  VLOAD_8(v2, 133, 2, 220, 4);
  VLOAD_8(v3, 133, 2, 50, 4);
  __asm__ volatile("vsaddu.vv v4, v2, v3" ::);
  VCMP_U8(1, v4, 255, 4, 255, 8);
  read_vxsat(vxsat);
  check_vxsat(1, vxsat, 1);
  reset_vxsat;
}


void TEST_CASE3(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v2, 1, 0xFFFFFFFB, 3, 4);
  __asm__ volatile("vsaddu.vi v4, v2, 5" ::);
  VCMP_U32(3, v4, 6, 0xFFFFFFFF, 8, 9);
  read_vxsat(vxsat);
  check_vxsat(3, vxsat, 1);
  reset_vxsat;
}


void TEST_CASE5(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v2, 0xFFFFFFFD, 2, 3, 4);
  const uint32_t scalar = 5;
  __asm__ volatile("vsaddu.vx v4, v2, %[A]" ::[A] "r"(scalar));
  VCMP_U32(5, v4, 0xFFFFFFFF, 7, 8, 9);
  read_vxsat(vxsat);
  check_vxsat(5, vxsat, 1);
  reset_vxsat;
}


void TEST_CASE7(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v2, 1, 0x0000FFFF, 3, 4);
  VLOAD_32(v3, 0xA, 0xFFFF0000, 0x0, 0x0);
  VCLEAR(v4);
  __asm__ volatile("vsaddu.vv v4, v2, v3" ::);
  VCMP_U32(7, v4, 0xB, 0xFFFFFFFF, 3, 4);
  read_vxsat(vxsat);
  check_vxsat(7, vxsat, 0);
  reset_vxsat;
}

int main(void) {
  INIT_CHECK();
  // enable_vec();
  TEST_CASE1();
  TEST_CASE3();
  TEST_CASE5();
  TEST_CASE7();
  EXIT_CHECK();
}
