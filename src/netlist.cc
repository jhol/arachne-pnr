/* Copyright (C) 2015 Cotton Seed
   
   This file is part of arachne-pnr.  Arachne-pnr is free software;
   you can redistribute it and/or modify it under the terms of the GNU
   General Public License version 2 as published by the Free Software
   Foundation.
   
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include "netlist.hh"

#include "casting.hh"
#include "port.hh"
#include "util.hh"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>

void
Design::set_top(Model *t)
{
  assert(m_top == nullptr);
  m_top = t;
}

Design::Design()
  : m_top(nullptr)
{
}

Design::~Design()
{
  for (const auto &p : m_models)
    delete p.second;
  m_models.clear();
}

void
Design::create_standard_models()
{
  Model *lc = new Model(this, "ICESTORM_LC");
  lc->add_port("I0", Direction::IN, Value::ZERO);
  lc->add_port("I1", Direction::IN, Value::ZERO);
  lc->add_port("I2", Direction::IN, Value::ZERO);
  lc->add_port("I3", Direction::IN, Value::ZERO);
  lc->add_port("CIN", Direction::IN, Value::ZERO);
  lc->add_port("CLK", Direction::IN, Value::ZERO);
  lc->add_port("CEN", Direction::IN, Value::ONE);
  lc->add_port("SR", Direction::IN, Value::ZERO);
  lc->add_port("LO", Direction::OUT);
  lc->add_port("O", Direction::OUT);
  lc->add_port("COUT", Direction::OUT);
  
  lc->set_param("LUT_INIT", BitVector(1, 0));
  lc->set_param("NEG_CLK", BitVector(1, 0));
  lc->set_param("CARRY_ENABLE", BitVector(1, 0));
  lc->set_param("DFF_ENABLE", BitVector(1, 0));
  lc->set_param("SET_NORESET", BitVector(1, 0));
  lc->set_param("SET_ASYNC", BitVector(1, 0));
  lc->set_param("ASYNC_SR", BitVector(1, 0));
  
  Model *io = new Model(this, "SB_IO");
  io->add_port("PACKAGE_PIN", Direction::INOUT);
  io->add_port("LATCH_INPUT_VALUE", Direction::IN, Value::ZERO);
  io->add_port("CLOCK_ENABLE", Direction::IN, Value::ONE);
  io->add_port("INPUT_CLK", Direction::IN, Value::ZERO);
  io->add_port("OUTPUT_CLK", Direction::IN, Value::ZERO);
  io->add_port("OUTPUT_ENABLE", Direction::IN, Value::ZERO);
  io->add_port("D_OUT_0", Direction::IN, Value::ZERO);
  io->add_port("D_OUT_1", Direction::IN, Value::ZERO);
  io->add_port("D_IN_0", Direction::OUT, Value::ZERO);
  io->add_port("D_IN_1", Direction::OUT, Value::ZERO);
  
  io->set_param("PIN_TYPE", BitVector(6, 0)); // 000000
  io->set_param("PULLUP", BitVector(1, 0));  // default NO pullup
  io->set_param("NEG_TRIGGER", BitVector(1, 0));
  io->set_param("IO_STANDARD", "SB_LVCMOS");
  
  Model *gb = new Model(this, "SB_GB");
  gb->add_port("USER_SIGNAL_TO_GLOBAL_BUFFER", Direction::IN);
  gb->add_port("GLOBAL_BUFFER_OUTPUT", Direction::OUT);
  
  Model *gb_io = new Model(this, "SB_GB_IO");
  gb_io->add_port("PACKAGE_PIN", Direction::INOUT);
  gb_io->add_port("GLOBAL_BUFFER_OUTPUT", Direction::OUT);
  gb_io->add_port("LATCH_INPUT_VALUE", Direction::IN, Value::ZERO);
  gb_io->add_port("CLOCK_ENABLE", Direction::IN, Value::ONE);
  gb_io->add_port("INPUT_CLK", Direction::IN, Value::ZERO);
  gb_io->add_port("OUTPUT_CLK", Direction::IN, Value::ZERO);
  gb_io->add_port("OUTPUT_ENABLE", Direction::IN, Value::ZERO);
  gb_io->add_port("D_OUT_0", Direction::IN, Value::ZERO);
  gb_io->add_port("D_OUT_1", Direction::IN, Value::ZERO);
  gb_io->add_port("D_IN_0", Direction::OUT, Value::ZERO);
  gb_io->add_port("D_IN_1", Direction::OUT, Value::ZERO);
  
  gb_io->set_param("PIN_TYPE", BitVector(6, 0)); // 000000
  gb_io->set_param("PULLUP", BitVector(1, 0));  // default NO pullup
  gb_io->set_param("NEG_TRIGGER", BitVector(1, 0));
  gb_io->set_param("IO_STANDARD", "SB_LVCMOS");
  
  Model *lut = new Model(this, "SB_LUT4");
  lut->add_port("O", Direction::OUT);
  lut->add_port("I0", Direction::IN, Value::ZERO);
  lut->add_port("I1", Direction::IN, Value::ZERO);
  lut->add_port("I2", Direction::IN, Value::ZERO);
  lut->add_port("I3", Direction::IN, Value::ZERO);
  
  lut->set_param("LUT_INIT", BitVector(1, 0));
  
  Model *carry = new Model(this, "SB_CARRY");
  carry->add_port("CO", Direction::OUT);
  carry->add_port("I0", Direction::IN, Value::ZERO);
  carry->add_port("I1", Direction::IN, Value::ZERO);
  carry->add_port("CI", Direction::IN, Value::ZERO);
  
  for (int neg_clk = 0; neg_clk <= 1; ++neg_clk)
    for (int cen = 0; cen <= 1; ++cen)
      for (int sr = 0; sr <= 4; ++sr)
        {
          std::string name = "SB_DFF";
          if (neg_clk)
            name.push_back('N');
          if (cen)
            name.push_back('E');
          switch(sr)
            {
            case 0:  break;
            case 1:
              name.append("SR");
              break;
            case 2:
              name.append("R");
              break;
            case 3:
              name.append("SS");
              break;
            case 4:
              name.append("S");
              break;
            default:
              abort();
            }
        
          Model *dff = new Model(this, name);
          dff->add_port("Q", Direction::OUT);
          dff->add_port("C", Direction::IN, Value::ZERO);
          if (cen)
            dff->add_port("E", Direction::IN, Value::ONE);
          switch(sr)
            {
            case 0:  break;
            case 1:
            case 2:
              dff->add_port("R", Direction::IN, Value::ZERO);
              break;
            case 3:
            case 4:
              dff->add_port("S", Direction::IN, Value::ZERO);
              break;
            default:
              abort();
            }
          dff->add_port("D", Direction::IN, Value::ZERO);
        }
  
  for (int nr = 0; nr <= 1; ++nr)
    for (int nw = 0; nw <= 1; ++nw)
      {
        std::string name = "SB_RAM40_4K";
        
        if (nr)
          name.append("NR");
        if (nw)
          name.append("NW");
        Model *bram = new Model(this, name);
        
        for (int i = 0; i <= 15; ++i)
          bram->add_port(fmt("RDATA[" << i << "]"), Direction::OUT);
        for (int i = 0; i <= 10; ++i)
          bram->add_port(fmt("RADDR[" << i << "]"), Direction::IN, Value::ZERO);
        
        for (int i = 0; i <= 10; ++i)
          bram->add_port(fmt("WADDR[" << i << "]"), Direction::IN, Value::ZERO);
        for (int i = 0; i <= 15; ++i)
          bram->add_port(fmt("MASK[" << i << "]"), Direction::IN, Value::ZERO);
        for (int i = 0; i <= 15; ++i)
          bram->add_port(fmt("WDATA[" << i << "]"), Direction::IN, Value::ZERO);
        
        bram->add_port("RCLKE", Direction::IN, Value::ONE);
        
        if (nr)
          bram->add_port("RCLKN", Direction::IN, Value::ZERO);
        else
          bram->add_port("RCLK", Direction::IN, Value::ZERO);
        bram->add_port("RE", Direction::IN, Value::ZERO);
        
        bram->add_port("WCLKE", Direction::IN, Value::ONE);
        if (nw)
          bram->add_port("WCLKN", Direction::IN, Value::ZERO);
        else
          bram->add_port("WCLK", Direction::IN, Value::ZERO);
        bram->add_port("WE", Direction::IN, Value::ZERO);
        
        for (int i = 0; i <= 15; ++i)
          bram->set_param(fmt("INIT_" << hexdigit(i, 'A')), BitVector(256, 0));
        bram->set_param("READ_MODE", BitVector(2, 0));
        bram->set_param("WRITE_MODE", BitVector(2, 0));
      }

  Model *pll_core = new Model(this, "SB_PLL40_CORE");
  pll_core->add_port("REFERENCECLK", Direction::IN, Value::ZERO);
  pll_core->add_port("RESETB", Direction::IN, Value::ZERO);
  pll_core->add_port("BYPASS", Direction::IN, Value::ZERO);
  pll_core->add_port("EXTFEEDBACK", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[0]", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[1]", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[2]", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[3]", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[4]", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[5]", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[6]", Direction::IN, Value::ZERO);
  pll_core->add_port("DYNAMICDELAY[7]", Direction::IN, Value::ZERO);
  pll_core->add_port("LATCHINPUTVALUE", Direction::IN, Value::ZERO);
  pll_core->add_port("SCLK", Direction::IN, Value::ZERO);
  pll_core->add_port("SDI", Direction::IN, Value::ZERO);
  pll_core->add_port("SDO", Direction::IN, Value::ZERO);
  pll_core->add_port("LOCK", Direction::OUT);
  pll_core->add_port("PLLOUTGLOBAL", Direction::OUT);
  pll_core->add_port("PLLOUTCORE", Direction::OUT);
  
  pll_core->set_param("FEEDBACK_PATH", "SIMPLE");
  pll_core->set_param("DELAY_ADJUSTMENT_MODE_FEEDBACK", "FIXED");
  pll_core->set_param("FDA_FEEDBACK", BitVector(4, 0));
  pll_core->set_param("DELAY_ADJUSTMENT_MODE_RELATIVE", "FIXED");
  pll_core->set_param("FDA_RELATIVE", BitVector(4, 0));
  pll_core->set_param("SHIFTREG_DIV_MODE", BitVector(1, 0));
  pll_core->set_param("PLLOUT_SELECT", "GENCLK");
  pll_core->set_param("DIVR", BitVector(4, 0));
  pll_core->set_param("DIVF", BitVector(7, 0));
  pll_core->set_param("DIVQ", BitVector(3, 0));
  pll_core->set_param("FILTER_RANGE", BitVector(3, 0));
  pll_core->set_param("EXTERNAL_DIVIDE_FACTOR", BitVector(32, 1));
  pll_core->set_param("ENABLE_ICEGATE", BitVector(1, 0));
  
  Model *pll_pad = new Model(this, "SB_PLL40_PAD");
  pll_pad->add_port("PACKAGEPIN", Direction::IN);
  pll_pad->add_port("RESETB", Direction::IN, Value::ZERO);
  pll_pad->add_port("BYPASS", Direction::IN, Value::ZERO);
  pll_pad->add_port("EXTFEEDBACK", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[0]", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[1]", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[2]", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[3]", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[4]", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[5]", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[6]", Direction::IN, Value::ZERO);
  pll_pad->add_port("DYNAMICDELAY[7]", Direction::IN, Value::ZERO);
  pll_pad->add_port("LATCHINPUTVALUE", Direction::IN, Value::ZERO);
  pll_pad->add_port("SCLK", Direction::IN, Value::ZERO);
  pll_pad->add_port("SDI", Direction::IN, Value::ZERO);
  pll_pad->add_port("SDO", Direction::IN, Value::ZERO);
  pll_pad->add_port("LOCK", Direction::OUT);
  pll_pad->add_port("PLLOUTGLOBAL", Direction::OUT);
  pll_pad->add_port("PLLOUTCORE", Direction::OUT);
  
  pll_pad->set_param("FEEDBACK_PATH", "SIMPLE");
  pll_pad->set_param("DELAY_ADJUSTMENT_MODE_FEEDBACK", "FIXED");
  pll_pad->set_param("FDA_FEEDBACK", BitVector(4, 0));
  pll_pad->set_param("DELAY_ADJUSTMENT_MODE_RELATIVE", "FIXED");
  pll_pad->set_param("FDA_RELATIVE", BitVector(4, 0));
  pll_pad->set_param("SHIFTREG_DIV_MODE", BitVector(1, 0));
  pll_pad->set_param("PLLOUT_SELECT", "GENCLK");
  pll_pad->set_param("DIVR", BitVector(4, 0));
  pll_pad->set_param("DIVF", BitVector(7, 0));
  pll_pad->set_param("DIVQ", BitVector(3, 0));
  pll_pad->set_param("FILTER_RANGE", BitVector(3, 0));
  pll_pad->set_param("EXTERNAL_DIVIDE_FACTOR", BitVector(32, 1));
  pll_pad->set_param("ENABLE_ICEGATE", BitVector(1, 0));

  Model *pll_2_pad = new Model(this, "SB_PLL40_2_PAD");
  pll_2_pad->add_port("PACKAGEPIN", Direction::IN);
  pll_2_pad->add_port("RESETB", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("BYPASS", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("EXTFEEDBACK", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[0]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[1]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[2]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[3]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[4]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[5]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[6]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("DYNAMICDELAY[7]", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("LATCHINPUTVALUE", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("SCLK", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("SDI", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("SDO", Direction::IN, Value::ZERO);
  pll_2_pad->add_port("LOCK", Direction::OUT);
  pll_2_pad->add_port("PLLOUTGLOBALA", Direction::OUT);
  pll_2_pad->add_port("PLLOUTCOREA", Direction::OUT);
  pll_2_pad->add_port("PLLOUTGLOBALB", Direction::OUT);
  pll_2_pad->add_port("PLLOUTCOREB", Direction::OUT);
  
  pll_2_pad->set_param("FEEDBACK_PATH", "SIMPLE");
  pll_2_pad->set_param("DELAY_ADJUSTMENT_MODE_FEEDBACK", "FIXED");
  pll_2_pad->set_param("FDA_FEEDBACK", BitVector(4, 0));
  pll_2_pad->set_param("DELAY_ADJUSTMENT_MODE_RELATIVE", "FIXED");
  pll_2_pad->set_param("FDA_RELATIVE", BitVector(4, 0));
  pll_2_pad->set_param("SHIFTREG_DIV_MODE", BitVector(1, 0));
  pll_2_pad->set_param("PLLOUT_SELECT_PORTA", "GENCLK");
  pll_2_pad->set_param("PLLOUT_SELECT_PORTB", "GENCLK");
  pll_2_pad->set_param("DIVR", BitVector(4, 0));
  pll_2_pad->set_param("DIVF", BitVector(7, 0));
  pll_2_pad->set_param("DIVQ", BitVector(3, 0));
  pll_2_pad->set_param("FILTER_RANGE", BitVector(3, 0));
  pll_2_pad->set_param("EXTERNAL_DIVIDE_FACTOR", BitVector(32, 1));
  pll_2_pad->set_param("ENABLE_ICEGATE_PORTA", BitVector(1, 0));
  pll_2_pad->set_param("ENABLE_ICEGATE_PORTB", BitVector(1, 0));

  Model *pll_2f_core = new Model(this, "SB_PLL40_2F_CORE");
  pll_2f_core->add_port("REFERENCECLK", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("RESETB", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("BYPASS", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("EXTFEEDBACK", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[0]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[1]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[2]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[3]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[4]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[5]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[6]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("DYNAMICDELAY[7]", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("LATCHINPUTVALUE", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("SCLK", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("SDI", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("SDO", Direction::IN, Value::ZERO);
  pll_2f_core->add_port("LOCK", Direction::OUT);
  pll_2f_core->add_port("PLLOUTGLOBALA", Direction::OUT);
  pll_2f_core->add_port("PLLOUTCOREA", Direction::OUT);
  pll_2f_core->add_port("PLLOUTGLOBALB", Direction::OUT);
  pll_2f_core->add_port("PLLOUTCOREB", Direction::OUT);
  
  pll_2f_core->set_param("FEEDBACK_PATH", "SIMPLE");
  pll_2f_core->set_param("DELAY_ADJUSTMENT_MODE_FEEDBACK", "FIXED");
  pll_2f_core->set_param("FDA_FEEDBACK", BitVector(4, 0));
  pll_2f_core->set_param("DELAY_ADJUSTMENT_MODE_RELATIVE", "FIXED");
  pll_2f_core->set_param("FDA_RELATIVE", BitVector(4, 0));
  pll_2f_core->set_param("SHIFTREG_DIV_MODE", BitVector(1, 0));
  pll_2f_core->set_param("PLLOUT_SELECT_PORTA", "GENCLK");
  pll_2f_core->set_param("PLLOUT_SELECT_PORTB", "GENCLK");
  pll_2f_core->set_param("DIVR", BitVector(4, 0));
  pll_2f_core->set_param("DIVF", BitVector(7, 0));
  pll_2f_core->set_param("DIVQ", BitVector(3, 0));
  pll_2f_core->set_param("FILTER_RANGE", BitVector(3, 0));
  pll_2f_core->set_param("EXTERNAL_DIVIDE_FACTOR", BitVector(32, 1));
  pll_2f_core->set_param("ENABLE_ICEGATE_PORTA", BitVector(1, 0));
  pll_2f_core->set_param("ENABLE_ICEGATE_PORTB", BitVector(1, 0));

  Model *pll_2f_pad = new Model(this, "SB_PLL40_2F_PAD");
  pll_2f_pad->add_port("PACKAGEPIN", Direction::IN);
  pll_2f_pad->add_port("RESETB", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("BYPASS", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("EXTFEEDBACK", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[0]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[1]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[2]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[3]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[4]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[5]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[6]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("DYNAMICDELAY[7]", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("LATCHINPUTVALUE", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("SCLK", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("SDI", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("SDO", Direction::IN, Value::ZERO);
  pll_2f_pad->add_port("LOCK", Direction::OUT);
  pll_2f_pad->add_port("PLLOUTGLOBALA", Direction::OUT);
  pll_2f_pad->add_port("PLLOUTCOREA", Direction::OUT);
  pll_2f_pad->add_port("PLLOUTGLOBALB", Direction::OUT);
  pll_2f_pad->add_port("PLLOUTCOREB", Direction::OUT);
  
  pll_2f_pad->set_param("FEEDBACK_PATH", "SIMPLE");
  pll_2f_pad->set_param("DELAY_ADJUSTMENT_MODE_FEEDBACK", "FIXED");
  pll_2f_pad->set_param("FDA_FEEDBACK", BitVector(4, 0));
  pll_2f_pad->set_param("DELAY_ADJUSTMENT_MODE_RELATIVE", "FIXED");
  pll_2f_pad->set_param("FDA_RELATIVE", BitVector(4, 0));
  pll_2f_pad->set_param("SHIFTREG_DIV_MODE", BitVector(1, 0));
  pll_2f_pad->set_param("PLLOUT_SELECT_PORTA", "GENCLK");
  pll_2f_pad->set_param("PLLOUT_SELECT_PORTB", "GENCLK");
  pll_2f_pad->set_param("DIVR", BitVector(4, 0));
  pll_2f_pad->set_param("DIVF", BitVector(7, 0));
  pll_2f_pad->set_param("DIVQ", BitVector(3, 0));
  pll_2f_pad->set_param("FILTER_RANGE", BitVector(3, 0));
  pll_2f_pad->set_param("EXTERNAL_DIVIDE_FACTOR", BitVector(32, 1));
  pll_2f_pad->set_param("ENABLE_ICEGATE_PORTA", BitVector(1, 0));
  pll_2f_pad->set_param("ENABLE_ICEGATE_PORTB", BitVector(1, 0));

  Model *warmboot = new Model(this, "SB_WARMBOOT");
  warmboot->add_port("BOOT", Direction::IN, Value::ZERO);
  warmboot->add_port("S1", Direction::IN, Value::ZERO);
  warmboot->add_port("S0", Direction::IN, Value::ZERO);
  
  Model *tbuf = new Model(this, "$_TBUF_");
  tbuf->add_port("A", Direction::IN);
  tbuf->add_port("E", Direction::IN);
  tbuf->add_port("Y", Direction::OUT);
}

Model *
Design::find_model(const std::string &n) const
{
  return lookup_or_default(m_models, n, (Model *)nullptr);
}

void
Design::prune()
{
  for (const auto &p : m_models)
    p.second->prune();
}

#ifndef NDEBUG
void
Design::check() const
{
  for (const auto &p : m_models)
    p.second->check(this);
}
#endif

void
Design::write_blif(std::ostream &s) const
{
  assert(m_top);
  m_top->write_blif(s);
}

void
Design::write_verilog(std::ostream &s) const
{
  assert(m_top);
  m_top->write_verilog(s);
}

void
Design::dump() const
{
  write_blif(*logs);
}

Models::Models(const Design *d)
{
  lut4 = d->find_model("SB_LUT4");
  carry = d->find_model("SB_CARRY");
  lc = d->find_model("ICESTORM_LC");
  io = d->find_model("SB_IO");
  gb = d->find_model("SB_GB");
  gb_io = d->find_model("SB_GB_IO");
  ram = d->find_model("SB_RAM40_4K");
  ramnr = d->find_model("SB_RAM40_4KNR");
  ramnw = d->find_model("SB_RAM40_4KNW");
  ramnrnw = d->find_model("SB_RAM40_4KNRNW");
  warmboot = d->find_model("SB_WARMBOOT");
  tbuf = d->find_model("$_TBUF_");
}
