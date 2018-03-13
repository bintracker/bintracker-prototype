// Copyright 2017-2018 utz. See LICENSE for details.

#ifndef SOUND_EMUL_MACHINES_CPU_Z80_H_
#define SOUND_EMUL_MACHINES_CPU_Z80_H_

#include <functional>
#include <array>


enum class Z80Type {NMOS, CMOS, BM1};

class z80cpu {
 public:
    std::array<int8_t, 0x10000> inputPorts;
    std::array<int8_t, 0x100> inputPortsShort;
    std::array<int8_t, 0x10000> outputPorts;
    std::array<int8_t, 0x100> outputPortsShort;

    void reset();

    void setPC(int startAddress);
    void setSP(int address);
    int getPC();

    void execute_cycle();
    void execute_debug();
    int non_maskable_interrupt();
    int maskable_interrupt();

    z80cpu(std::array<int, 0x10000> *mem, Z80Type z80Type);
    ~z80cpu() = default;

 private:
    std::array<int, 0x10000> *memory;
    unsigned instructionCycles;

    Z80Type cpuType;

    int regA;
    int regB;
    int regC;
    int regD;
    int regE;
    int regF;
    int regH;
    int regL;
    int regAs;
    int regBs;
    int regCs;
    int regDs;
    int regEs;
    int regFs;
    int regHs;
    int regLs;
    int regI;
    int regR;
    int regIXH;
    int regIXL;
    int regIYH;
    int regIYL;
    int regSP;
    int regPC;

    int regQ;
    int regMEMPTR;
    bool regIFF1;
    bool regIFF2;
    bool interruptsEnabled;
    int interruptMode;

    int regDummy;   // dummy register for generic instructions

    const std::array<int *const, 8> regPtr = {{&regB, &regC, &regD, &regE, &regH, &regL, &regDummy, &regA}};
    static const std::array<const int, 4> conditionCodes;
    // used as lookup by z80 instructions that set the parity flag
    static const std::array<int, 256> parityTable;
    static const std::array<unsigned, 256> instructionTimingsRegular;
    static const std::array<unsigned, 256> instructionTimingsDD_FD;
    static const std::array<unsigned, 256> instructionTimingsED;

    static std::array<int, 256> init_parity_table();
    void debugger_print_flags(const int flagRegister);
    unsigned get_instruction_timing(int instructionPointer);
    unsigned get_conditional_timing(const int instruction);

    // generic 16-bit register instructions
    void ld_r16_nn();
    void add_hl_r16();
    void inc_r16();
    void dec_r16();
    void pop_r16();
    void push_r16();

    // generic 8-bit register instructions
    void inc_r8();
    void dec_r8();
    void ld_r8_n();
    void ld_r8_r8();
    void ld_r8_athl();
    void ld_athl_r8();
    void adx_a_r8();
    void sbx_a_r8();
    void and_r8();
    void xor_r8();
    void or_r8();
    void cp_r8();

    // generic conditional instructions
    void jr_cc_n();
    void ret_cc();
    void jp_cc_nn();
    void call_cc_nn();

    // misc generic instructions
    void rst_xx();

    // regular (non-prefixed) instructions
    void nop();
    void ld_atbc_a();
    void rlca();
    void ex_af_af();
    void ld_a_atbc();
    void rrca();

    void djnz();
    void ld_atde_a();
    void rla();
    void jr();
    void ld_a_atde();
    void rra();

    void ld_atnn_hl();
    void daa();
    void ld_hl_atnn();
    void cpl();

    void ld_sp_nn();
    void ld_atnn_a();
    void inc_sp();
    void inc_athl();
    void dec_athl();
    void ld_athl_n();
    void scf();
    void add_hl_sp();
    void ld_a_atnn();
    void dec_sp();
    void ccf();

    void halt();

    void adx_a_athl();

    void sbx_a_athl();

    void and_athl();
    void xor_athl();

    void or_athl();
    void cp_athl();

    void jp_nn();
    void ret();
    void prefix_cb();
    void call_nn();
    void adx_a_n();

    void out_atn_a();
    void exx();
    void in_a_atn();
    void prefix_dd();
    void sbx_a_n();

    void ex_atsp_hl();
    void and_n();
    void jp_athl();
    void ex_de_hl();
    void prefix_ed();
    void xor_n();

    void pop_af();
    void di();
    void push_af();
    void or_n();
    void ld_sp_hl();
    void ei();
    void prefix_fd();
    void cp_n();


    // prefix ED
    void in_r8_atc();
    void out_atc_r8();
    void sbc_hl_r16();
    void adc_hl_r16();
    void ld_atnn_r16();
    void ld_r16_atnn();

    void neg();
    void retn();
    void im0();
    void ld_i_a();
    void reti();
    void ld_r_a();

    void im1();
    void ld_a_i();
    void im2();
    void ld_a_r();

    void rrd();
    void rld();

    void sbc_hl_sp();
    void ld_atnn_sp();
    void adc_hl_sp();
    void ld_sp_atnn();

    void ldi();
    void cpi();
    void ini();
    void outi();
    void ldd();
    void cpd();
    void ind();
    void outd();

    void ldir();
    void cpir();
    void inir();
    void otir();
    void lddr();
    void cpdr();
    void indr();
    void otdr();

    // prefix CB
    void rlc_r8();
    void rlc_athl();
    void rrc_r8();
    void rrc_athl();

    void rl_r8();
    void rl_athl();
    void rr_r8();
    void rr_athl();

    void sra_r8();
    void sra_athl();

    void slx_r8();
    void slx_athl();
    void srl_r8();
    void srl_athl();

    void bit_x_r8();
    void bit_x_athl();

    void res_x_r8();
    void res_x_athl();

    void set_x_r8();
    void set_x_athl();


    // prefix DD
    void add_ix_r16();
    void ld_r8_atixpd();
    void ld_r8_ixh();
    void ld_r8_ixl();

    void ld_ix_nn();
    void ld_atnn_ix();
    void inc_ix();
    void inc_ixh();
    void dec_ixh();
    void ld_ixh_n();
    void add_ix_ix();
    void ld_ix_atnn();
    void dec_ix();
    void inc_ixl();
    void dec_ixl();
    void ld_ixl_n();

    void inc_atixpd();
    void dec_atixpd();
    void ld_atixpd_n();
    void add_ix_sp();

    void ld_ixh_r8();
    void ld_ixh_ixl();
    void ld_ixl_r8();
    void ld_ixl_ixh();

    void ld_atixpd_r8();

    void adx_a_ixh();
    void adx_a_ixl();
    void adx_a_atixpd();

    void sbx_a_ixh();
    void sbx_a_ixl();
    void sbx_a_atixpd();

    void and_ixh();
    void and_ixl();
    void and_atixpd();
    void xor_ixh();
    void xor_ixl();
    void xor_atixpd();

    void or_ixh();
    void or_ixl();
    void or_atixpd();
    void cp_ixh();
    void cp_ixl();
    void cp_atixpd();

    void prefix_ddcb();

    void pop_ix();
    void ex_atsp_ix();
    void push_ix();
    void jp_atix();

    void ld_sp_ix();


    // prefix FD
    void add_iy_r16();
    void ld_r8_atiypd();
    void ld_r8_iyh();
    void ld_r8_iyl();

    void ld_iy_nn();
    void ld_atnn_iy();
    void inc_iy();
    void inc_iyh();
    void dec_iyh();
    void ld_iyh_n();
    void add_iy_iy();
    void ld_iy_atnn();
    void dec_iy();
    void inc_iyl();
    void dec_iyl();
    void ld_iyl_n();

    void inc_atiypd();
    void dec_atiypd();
    void ld_atiypd_n();
    void add_iy_sp();

    void ld_iyh_r8();
    void ld_iyh_iyl();
    void ld_iyl_r8();
    void ld_iyl_iyh();

    void ld_atiypd_r8();

    void adx_a_iyh();
    void adx_a_iyl();
    void adx_a_atiypd();

    void sbx_a_iyh();
    void sbx_a_iyl();
    void sbx_a_atiypd();

    void and_iyh();
    void and_iyl();
    void and_atiypd();
    void xor_iyh();
    void xor_iyl();
    void xor_atiypd();

    void or_iyh();
    void or_iyl();
    void or_atiypd();
    void cp_iyh();
    void cp_iyl();
    void cp_atiypd();

    void prefix_fdcb();

    void pop_iy();
    void ex_atsp_iy();
    void push_iy();
    void jp_atiy();

    void ld_sp_iy();

    // prefix DDCB
    void rlc_atixpd();
    void rrc_atixpd();
    void rl_atixpd();
    void rr_atixpd();
    void sla_atixpd();
    void sra_atixpd();
    void sll_atixpd();
    void srl_atixpd();

    void bit_x_atixpd();
    void res_x_atixpd();
    void set_x_atixpd();

    // prefix FDCB
    void rlc_atiypd();
    void rrc_atiypd();
    void rl_atiypd();
    void rr_atiypd();
    void sla_atiypd();
    void sra_atiypd();
    void sll_atiypd();
    void srl_atiypd();

    void bit_x_atiypd();
    void res_x_atiypd();
    void set_x_atiypd();


    using CpuFun = decltype(std::mem_fn(&z80cpu::nop));

    // prefix FDCB, rotate/shift
    const std::array<CpuFun, 8> cpu_instructions_fdcb {{
            std::mem_fn(&z80cpu::rlc_atiypd),
            std::mem_fn(&z80cpu::rrc_atiypd),
            std::mem_fn(&z80cpu::rl_atiypd),
            std::mem_fn(&z80cpu::rr_atiypd),
            std::mem_fn(&z80cpu::sla_atiypd),
            std::mem_fn(&z80cpu::sra_atiypd),
            std::mem_fn(&z80cpu::sll_atiypd),
            std::mem_fn(&z80cpu::srl_atiypd)
    }};

    // prefix DDCB, rotate/shift
    const std::array<CpuFun, 8> cpu_instructions_ddcb {{
            std::mem_fn(&z80cpu::rlc_atixpd),
            std::mem_fn(&z80cpu::rrc_atixpd),
            std::mem_fn(&z80cpu::rl_atixpd),
            std::mem_fn(&z80cpu::rr_atixpd),
            std::mem_fn(&z80cpu::sla_atixpd),
            std::mem_fn(&z80cpu::sra_atixpd),
            std::mem_fn(&z80cpu::sll_atixpd),
            std::mem_fn(&z80cpu::srl_atixpd)
    }};

    // prefix FD
    const std::array<CpuFun, 256> cpu_instructions_fd {{
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::ld_r16_nn),
            std::mem_fn(&z80cpu::ld_atbc_a),
            std::mem_fn(&z80cpu::inc_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rlca),
            std::mem_fn(&z80cpu::ex_af_af),
            std::mem_fn(&z80cpu::add_iy_r16),
            std::mem_fn(&z80cpu::ld_a_atbc),
            std::mem_fn(&z80cpu::dec_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rrca),

            std::mem_fn(&z80cpu::djnz),
            std::mem_fn(&z80cpu::ld_r16_nn),
            std::mem_fn(&z80cpu::ld_atde_a),
            std::mem_fn(&z80cpu::inc_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rla),
            std::mem_fn(&z80cpu::jr),
            std::mem_fn(&z80cpu::add_iy_r16),
            std::mem_fn(&z80cpu::ld_a_atde),
            std::mem_fn(&z80cpu::dec_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rra),

            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::ld_iy_nn),
            std::mem_fn(&z80cpu::ld_atnn_iy),
            std::mem_fn(&z80cpu::inc_iy),
            std::mem_fn(&z80cpu::inc_iyh),
            std::mem_fn(&z80cpu::dec_iyh),
            std::mem_fn(&z80cpu::ld_iyh_n),
            std::mem_fn(&z80cpu::daa),
            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::add_iy_iy),
            std::mem_fn(&z80cpu::ld_iy_atnn),
            std::mem_fn(&z80cpu::dec_iy),
            std::mem_fn(&z80cpu::inc_iyl),
            std::mem_fn(&z80cpu::dec_iyl),
            std::mem_fn(&z80cpu::ld_iyl_n),
            std::mem_fn(&z80cpu::cpl),

            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::ld_sp_nn),
            std::mem_fn(&z80cpu::ld_atnn_a),
            std::mem_fn(&z80cpu::inc_sp),
            std::mem_fn(&z80cpu::inc_atiypd),
            std::mem_fn(&z80cpu::dec_atiypd),
            std::mem_fn(&z80cpu::ld_atiypd_n),
            std::mem_fn(&z80cpu::scf),
            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::add_iy_sp),
            std::mem_fn(&z80cpu::ld_a_atnn),
            std::mem_fn(&z80cpu::dec_sp),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::ccf),

            std::mem_fn(&z80cpu::nop),      // ld b,b
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_iyh),
            std::mem_fn(&z80cpu::ld_r8_iyl),
            std::mem_fn(&z80cpu::ld_r8_atiypd),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld c,c
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_iyh),
            std::mem_fn(&z80cpu::ld_r8_iyl),
            std::mem_fn(&z80cpu::ld_r8_atiypd),
            std::mem_fn(&z80cpu::ld_r8_r8),

            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld d,d
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_iyh),
            std::mem_fn(&z80cpu::ld_r8_iyl),
            std::mem_fn(&z80cpu::ld_r8_atiypd),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld e,e
            std::mem_fn(&z80cpu::ld_r8_iyh),
            std::mem_fn(&z80cpu::ld_r8_iyl),
            std::mem_fn(&z80cpu::ld_r8_atiypd),
            std::mem_fn(&z80cpu::ld_r8_r8),

            std::mem_fn(&z80cpu::ld_iyh_r8),
            std::mem_fn(&z80cpu::ld_iyh_r8),
            std::mem_fn(&z80cpu::ld_iyh_r8),
            std::mem_fn(&z80cpu::ld_iyh_r8),
            std::mem_fn(&z80cpu::nop),      // std::mem_fn(&z80cpu::ld_iyh_iyh(),
            std::mem_fn(&z80cpu::ld_iyh_iyl),
            std::mem_fn(&z80cpu::ld_r8_atiypd),
            std::mem_fn(&z80cpu::ld_iyh_r8),
            std::mem_fn(&z80cpu::ld_iyl_r8),
            std::mem_fn(&z80cpu::ld_iyl_r8),
            std::mem_fn(&z80cpu::ld_iyl_r8),
            std::mem_fn(&z80cpu::ld_iyl_r8),
            std::mem_fn(&z80cpu::ld_iyl_iyh),
            std::mem_fn(&z80cpu::nop),      // std::mem_fn(&z80cpu::ld_iyl_iyl(),
            std::mem_fn(&z80cpu::ld_r8_atiypd),
            std::mem_fn(&z80cpu::ld_iyl_r8),

            std::mem_fn(&z80cpu::ld_atiypd_r8),
            std::mem_fn(&z80cpu::ld_atiypd_r8),
            std::mem_fn(&z80cpu::ld_atiypd_r8),
            std::mem_fn(&z80cpu::ld_atiypd_r8),
            std::mem_fn(&z80cpu::ld_atiypd_r8),
            std::mem_fn(&z80cpu::ld_atiypd_r8),
            std::mem_fn(&z80cpu::halt),
            std::mem_fn(&z80cpu::ld_atiypd_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_iyh),
            std::mem_fn(&z80cpu::ld_r8_iyl),
            std::mem_fn(&z80cpu::ld_r8_atiypd),
            std::mem_fn(&z80cpu::nop),      // ld a,a

            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_iyh),
            std::mem_fn(&z80cpu::adx_a_iyl),
            std::mem_fn(&z80cpu::adx_a_atiypd),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_iyh),
            std::mem_fn(&z80cpu::adx_a_iyl),
            std::mem_fn(&z80cpu::adx_a_atiypd),
            std::mem_fn(&z80cpu::adx_a_r8),

            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_iyh),
            std::mem_fn(&z80cpu::sbx_a_iyl),
            std::mem_fn(&z80cpu::sbx_a_atiypd),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_iyh),
            std::mem_fn(&z80cpu::sbx_a_iyl),
            std::mem_fn(&z80cpu::sbx_a_atiypd),
            std::mem_fn(&z80cpu::sbx_a_r8),

            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_iyh),
            std::mem_fn(&z80cpu::and_iyl),
            std::mem_fn(&z80cpu::and_atiypd),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_iyh),
            std::mem_fn(&z80cpu::xor_iyl),
            std::mem_fn(&z80cpu::xor_atiypd),
            std::mem_fn(&z80cpu::xor_r8),

            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_iyh),
            std::mem_fn(&z80cpu::or_iyl),
            std::mem_fn(&z80cpu::or_atiypd),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_iyh),
            std::mem_fn(&z80cpu::cp_iyl),
            std::mem_fn(&z80cpu::cp_atiypd),
            std::mem_fn(&z80cpu::cp_r8),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_r16),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::jp_nn),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_r16),
            std::mem_fn(&z80cpu::adx_a_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::ret),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::prefix_ddcb),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::call_nn),
            std::mem_fn(&z80cpu::adx_a_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_r16),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::out_atn_a),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_r16),
            std::mem_fn(&z80cpu::sbx_a_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::exx),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::in_a_atn),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_dd),
            std::mem_fn(&z80cpu::sbx_a_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_iy),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ex_atsp_iy),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_iy),
            std::mem_fn(&z80cpu::and_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::jp_atiy),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ex_de_hl),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_ed),
            std::mem_fn(&z80cpu::xor_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_af),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::di),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_af),
            std::mem_fn(&z80cpu::or_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::ld_sp_iy),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ei),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_fd),
            std::mem_fn(&z80cpu::cp_n),
            std::mem_fn(&z80cpu::rst_xx)
    }};

    // prefix DD
    const std::array<CpuFun, 256> cpu_instructions_dd {{
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::ld_r16_nn),
            std::mem_fn(&z80cpu::ld_atbc_a),
            std::mem_fn(&z80cpu::inc_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rlca),
            std::mem_fn(&z80cpu::ex_af_af),
            std::mem_fn(&z80cpu::add_ix_r16),
            std::mem_fn(&z80cpu::ld_a_atbc),
            std::mem_fn(&z80cpu::dec_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rrca),

            std::mem_fn(&z80cpu::djnz),
            std::mem_fn(&z80cpu::ld_r16_nn),
            std::mem_fn(&z80cpu::ld_atde_a),
            std::mem_fn(&z80cpu::inc_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rla),
            std::mem_fn(&z80cpu::jr),
            std::mem_fn(&z80cpu::add_ix_r16),
            std::mem_fn(&z80cpu::ld_a_atde),
            std::mem_fn(&z80cpu::dec_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rra),

            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::ld_ix_nn),
            std::mem_fn(&z80cpu::ld_atnn_ix),
            std::mem_fn(&z80cpu::inc_ix),
            std::mem_fn(&z80cpu::inc_ixh),
            std::mem_fn(&z80cpu::dec_ixh),
            std::mem_fn(&z80cpu::ld_ixh_n),
            std::mem_fn(&z80cpu::daa),
            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::add_ix_ix),
            std::mem_fn(&z80cpu::ld_ix_atnn),
            std::mem_fn(&z80cpu::dec_ix),
            std::mem_fn(&z80cpu::inc_ixl),
            std::mem_fn(&z80cpu::dec_ixl),
            std::mem_fn(&z80cpu::ld_ixl_n),
            std::mem_fn(&z80cpu::cpl),

            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::ld_sp_nn),
            std::mem_fn(&z80cpu::ld_atnn_a),
            std::mem_fn(&z80cpu::inc_sp),
            std::mem_fn(&z80cpu::inc_atixpd),
            std::mem_fn(&z80cpu::dec_atixpd),
            std::mem_fn(&z80cpu::ld_atixpd_n),
            std::mem_fn(&z80cpu::scf),
            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::add_ix_sp),
            std::mem_fn(&z80cpu::ld_a_atnn),
            std::mem_fn(&z80cpu::dec_sp),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::ccf),

            std::mem_fn(&z80cpu::nop),      // ld b,b
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_ixh),
            std::mem_fn(&z80cpu::ld_r8_ixl),
            std::mem_fn(&z80cpu::ld_r8_atixpd),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld c,c
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_ixh),
            std::mem_fn(&z80cpu::ld_r8_ixl),
            std::mem_fn(&z80cpu::ld_r8_atixpd),
            std::mem_fn(&z80cpu::ld_r8_r8),

            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld d,d
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_ixh),
            std::mem_fn(&z80cpu::ld_r8_ixl),
            std::mem_fn(&z80cpu::ld_r8_atixpd),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld e,e
            std::mem_fn(&z80cpu::ld_r8_ixh),
            std::mem_fn(&z80cpu::ld_r8_ixl),
            std::mem_fn(&z80cpu::ld_r8_atixpd),
            std::mem_fn(&z80cpu::ld_r8_r8),

            std::mem_fn(&z80cpu::ld_ixh_r8),
            std::mem_fn(&z80cpu::ld_ixh_r8),
            std::mem_fn(&z80cpu::ld_ixh_r8),
            std::mem_fn(&z80cpu::ld_ixh_r8),
            std::mem_fn(&z80cpu::nop),      // std::mem_fn(&z80cpu::ld_ixh_ixh(),
            std::mem_fn(&z80cpu::ld_ixh_ixl),
            std::mem_fn(&z80cpu::ld_r8_atixpd),
            std::mem_fn(&z80cpu::ld_ixh_r8),
            std::mem_fn(&z80cpu::ld_ixl_r8),
            std::mem_fn(&z80cpu::ld_ixl_r8),
            std::mem_fn(&z80cpu::ld_ixl_r8),
            std::mem_fn(&z80cpu::ld_ixl_r8),
            std::mem_fn(&z80cpu::ld_ixl_ixh),
            std::mem_fn(&z80cpu::nop),      // std::mem_fn(&z80cpu::ld_ixl_ixl(),
            std::mem_fn(&z80cpu::ld_r8_atixpd),
            std::mem_fn(&z80cpu::ld_ixl_r8),

            std::mem_fn(&z80cpu::ld_atixpd_r8),
            std::mem_fn(&z80cpu::ld_atixpd_r8),
            std::mem_fn(&z80cpu::ld_atixpd_r8),
            std::mem_fn(&z80cpu::ld_atixpd_r8),
            std::mem_fn(&z80cpu::ld_atixpd_r8),
            std::mem_fn(&z80cpu::ld_atixpd_r8),
            std::mem_fn(&z80cpu::halt),
            std::mem_fn(&z80cpu::ld_atixpd_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_ixh),
            std::mem_fn(&z80cpu::ld_r8_ixl),
            std::mem_fn(&z80cpu::ld_r8_atixpd),
            std::mem_fn(&z80cpu::nop),      // ld a,a

            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_ixh),
            std::mem_fn(&z80cpu::adx_a_ixl),
            std::mem_fn(&z80cpu::adx_a_atixpd),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_ixh),
            std::mem_fn(&z80cpu::adx_a_ixl),
            std::mem_fn(&z80cpu::adx_a_atixpd),
            std::mem_fn(&z80cpu::adx_a_r8),

            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_ixh),
            std::mem_fn(&z80cpu::sbx_a_ixl),
            std::mem_fn(&z80cpu::sbx_a_atixpd),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_ixh),
            std::mem_fn(&z80cpu::sbx_a_ixl),
            std::mem_fn(&z80cpu::sbx_a_atixpd),
            std::mem_fn(&z80cpu::sbx_a_r8),

            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_ixh),
            std::mem_fn(&z80cpu::and_ixl),
            std::mem_fn(&z80cpu::and_atixpd),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_ixh),
            std::mem_fn(&z80cpu::xor_ixl),
            std::mem_fn(&z80cpu::xor_atixpd),
            std::mem_fn(&z80cpu::xor_r8),

            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_ixh),
            std::mem_fn(&z80cpu::or_ixl),
            std::mem_fn(&z80cpu::or_atixpd),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_ixh),
            std::mem_fn(&z80cpu::cp_ixl),
            std::mem_fn(&z80cpu::cp_atixpd),
            std::mem_fn(&z80cpu::cp_r8),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_r16),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::jp_nn),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_r16),
            std::mem_fn(&z80cpu::adx_a_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::ret),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::prefix_ddcb),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::call_nn),
            std::mem_fn(&z80cpu::adx_a_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_r16),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::out_atn_a),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_r16),
            std::mem_fn(&z80cpu::sbx_a_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::exx),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::in_a_atn),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_dd),
            std::mem_fn(&z80cpu::sbx_a_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_ix),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ex_atsp_ix),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_ix),
            std::mem_fn(&z80cpu::and_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::jp_atix),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ex_de_hl),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_ed),
            std::mem_fn(&z80cpu::xor_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_af),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::di),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_af),
            std::mem_fn(&z80cpu::or_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::ld_sp_ix),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ei),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_fd),
            std::mem_fn(&z80cpu::cp_n),
            std::mem_fn(&z80cpu::rst_xx)
    }};

    // prefix CB, rotate/shift
    const std::array<CpuFun, 64> cpu_instructions_cb {{
            std::mem_fn(&z80cpu::rlc_r8),
            std::mem_fn(&z80cpu::rlc_r8),
            std::mem_fn(&z80cpu::rlc_r8),
            std::mem_fn(&z80cpu::rlc_r8),
            std::mem_fn(&z80cpu::rlc_r8),
            std::mem_fn(&z80cpu::rlc_r8),
            std::mem_fn(&z80cpu::rlc_athl),
            std::mem_fn(&z80cpu::rlc_r8),
            std::mem_fn(&z80cpu::rrc_r8),
            std::mem_fn(&z80cpu::rrc_r8),
            std::mem_fn(&z80cpu::rrc_r8),
            std::mem_fn(&z80cpu::rrc_r8),
            std::mem_fn(&z80cpu::rrc_r8),
            std::mem_fn(&z80cpu::rrc_r8),
            std::mem_fn(&z80cpu::rrc_athl),
            std::mem_fn(&z80cpu::rrc_r8),

            std::mem_fn(&z80cpu::rl_r8),
            std::mem_fn(&z80cpu::rl_r8),
            std::mem_fn(&z80cpu::rl_r8),
            std::mem_fn(&z80cpu::rl_r8),
            std::mem_fn(&z80cpu::rl_r8),
            std::mem_fn(&z80cpu::rl_r8),
            std::mem_fn(&z80cpu::rl_athl),
            std::mem_fn(&z80cpu::rl_r8),
            std::mem_fn(&z80cpu::rr_r8),
            std::mem_fn(&z80cpu::rr_r8),
            std::mem_fn(&z80cpu::rr_r8),
            std::mem_fn(&z80cpu::rr_r8),
            std::mem_fn(&z80cpu::rr_r8),
            std::mem_fn(&z80cpu::rr_r8),
            std::mem_fn(&z80cpu::rr_athl),
            std::mem_fn(&z80cpu::rr_r8),

            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_athl),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::sra_r8),
            std::mem_fn(&z80cpu::sra_r8),
            std::mem_fn(&z80cpu::sra_r8),
            std::mem_fn(&z80cpu::sra_r8),
            std::mem_fn(&z80cpu::sra_r8),
            std::mem_fn(&z80cpu::sra_r8),
            std::mem_fn(&z80cpu::sra_athl),
            std::mem_fn(&z80cpu::sra_r8),

            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::slx_athl),
            std::mem_fn(&z80cpu::slx_r8),
            std::mem_fn(&z80cpu::srl_r8),
            std::mem_fn(&z80cpu::srl_r8),
            std::mem_fn(&z80cpu::srl_r8),
            std::mem_fn(&z80cpu::srl_r8),
            std::mem_fn(&z80cpu::srl_r8),
            std::mem_fn(&z80cpu::srl_r8),
            std::mem_fn(&z80cpu::srl_athl),
            std::mem_fn(&z80cpu::srl_r8)
    }};

    // prefix CB, bit ops
    const std::array<CpuFun, 8> cpu_instructions_cb_bit {{
            std::mem_fn(&z80cpu::bit_x_r8),
            std::mem_fn(&z80cpu::bit_x_r8),
            std::mem_fn(&z80cpu::bit_x_r8),
            std::mem_fn(&z80cpu::bit_x_r8),
            std::mem_fn(&z80cpu::bit_x_r8),
            std::mem_fn(&z80cpu::bit_x_r8),
            std::mem_fn(&z80cpu::bit_x_athl),
            std::mem_fn(&z80cpu::bit_x_r8)
    }};

    // prefix CB, res ops
    const std::array<CpuFun, 8> cpu_instructions_cb_res {{
            std::mem_fn(&z80cpu::res_x_r8),
            std::mem_fn(&z80cpu::res_x_r8),
            std::mem_fn(&z80cpu::res_x_r8),
            std::mem_fn(&z80cpu::res_x_r8),
            std::mem_fn(&z80cpu::res_x_r8),
            std::mem_fn(&z80cpu::res_x_r8),
            std::mem_fn(&z80cpu::res_x_athl),
            std::mem_fn(&z80cpu::res_x_r8)
    }};

    // prefix CB, set ops
    const std::array<CpuFun, 8> cpu_instructions_cb_set {{
            std::mem_fn(&z80cpu::set_x_r8),
            std::mem_fn(&z80cpu::set_x_r8),
            std::mem_fn(&z80cpu::set_x_r8),
            std::mem_fn(&z80cpu::set_x_r8),
            std::mem_fn(&z80cpu::set_x_r8),
            std::mem_fn(&z80cpu::set_x_r8),
            std::mem_fn(&z80cpu::set_x_athl),
            std::mem_fn(&z80cpu::set_x_r8)
    }};

    // prefix ED
    const std::array<CpuFun, 124> cpu_instructions_ed {{
            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::sbc_hl_r16),
            std::mem_fn(&z80cpu::ld_atnn_r16),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::retn),
            std::mem_fn(&z80cpu::im0),
            std::mem_fn(&z80cpu::ld_i_a),
            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::adc_hl_r16),
            std::mem_fn(&z80cpu::ld_r16_atnn),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::reti),
            std::mem_fn(&z80cpu::im0),
            std::mem_fn(&z80cpu::ld_r_a),

            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::sbc_hl_r16),
            std::mem_fn(&z80cpu::ld_atnn_r16),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::retn),
            std::mem_fn(&z80cpu::im1),
            std::mem_fn(&z80cpu::ld_a_i),
            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::adc_hl_r16),
            std::mem_fn(&z80cpu::ld_r16_atnn),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::reti),
            std::mem_fn(&z80cpu::im2),
            std::mem_fn(&z80cpu::ld_a_r),

            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::sbc_hl_r16),
            std::mem_fn(&z80cpu::ld_atnn_hl),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::retn),
            std::mem_fn(&z80cpu::im0),
            std::mem_fn(&z80cpu::rrd),
            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::adc_hl_r16),
            std::mem_fn(&z80cpu::ld_hl_atnn),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::reti),
            std::mem_fn(&z80cpu::im0),
            std::mem_fn(&z80cpu::rld),

            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::sbc_hl_sp),
            std::mem_fn(&z80cpu::ld_atnn_sp),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::retn),
            std::mem_fn(&z80cpu::im1),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::in_r8_atc),
            std::mem_fn(&z80cpu::out_atc_r8),
            std::mem_fn(&z80cpu::adc_hl_sp),
            std::mem_fn(&z80cpu::ld_sp_atnn),
            std::mem_fn(&z80cpu::neg),
            std::mem_fn(&z80cpu::reti),
            std::mem_fn(&z80cpu::im2),
            std::mem_fn(&z80cpu::nop),

            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),

            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),

            std::mem_fn(&z80cpu::ldi),
            std::mem_fn(&z80cpu::cpi),
            std::mem_fn(&z80cpu::ini),
            std::mem_fn(&z80cpu::outi),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::ldd),
            std::mem_fn(&z80cpu::cpd),
            std::mem_fn(&z80cpu::ind),
            std::mem_fn(&z80cpu::outd),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),

            std::mem_fn(&z80cpu::ldir),
            std::mem_fn(&z80cpu::cpir),
            std::mem_fn(&z80cpu::inir),
            std::mem_fn(&z80cpu::otir),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::lddr),
            std::mem_fn(&z80cpu::cpdr),
            std::mem_fn(&z80cpu::indr),
            std::mem_fn(&z80cpu::otdr)
    }};


    // non-prefixed
    const std::array<CpuFun, 256> cpu_instructions {{
            std::mem_fn(&z80cpu::nop),
            std::mem_fn(&z80cpu::ld_r16_nn),
            std::mem_fn(&z80cpu::ld_atbc_a),
            std::mem_fn(&z80cpu::inc_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rlca),
            std::mem_fn(&z80cpu::ex_af_af),
            std::mem_fn(&z80cpu::add_hl_r16),
            std::mem_fn(&z80cpu::ld_a_atbc),
            std::mem_fn(&z80cpu::dec_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rrca),

            std::mem_fn(&z80cpu::djnz),
            std::mem_fn(&z80cpu::ld_r16_nn),
            std::mem_fn(&z80cpu::ld_atde_a),
            std::mem_fn(&z80cpu::inc_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rla),
            std::mem_fn(&z80cpu::jr),
            std::mem_fn(&z80cpu::add_hl_r16),
            std::mem_fn(&z80cpu::ld_a_atde),
            std::mem_fn(&z80cpu::dec_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::rra),

            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::ld_r16_nn),
            std::mem_fn(&z80cpu::ld_atnn_hl),
            std::mem_fn(&z80cpu::inc_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::daa),
            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::add_hl_r16),
            std::mem_fn(&z80cpu::ld_hl_atnn),
            std::mem_fn(&z80cpu::dec_r16),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::cpl),

            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::ld_sp_nn),
            std::mem_fn(&z80cpu::ld_atnn_a),
            std::mem_fn(&z80cpu::inc_sp),
            std::mem_fn(&z80cpu::inc_athl),
            std::mem_fn(&z80cpu::dec_athl),
            std::mem_fn(&z80cpu::ld_athl_n),
            std::mem_fn(&z80cpu::scf),
            std::mem_fn(&z80cpu::jr_cc_n),
            std::mem_fn(&z80cpu::add_hl_sp),
            std::mem_fn(&z80cpu::ld_a_atnn),
            std::mem_fn(&z80cpu::dec_sp),
            std::mem_fn(&z80cpu::inc_r8),
            std::mem_fn(&z80cpu::dec_r8),
            std::mem_fn(&z80cpu::ld_r8_n),
            std::mem_fn(&z80cpu::ccf),

            std::mem_fn(&z80cpu::nop),      // ld b,b
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_athl),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld c,c
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_athl),
            std::mem_fn(&z80cpu::ld_r8_r8),

            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld d,d
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_athl),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld e,e
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_athl),
            std::mem_fn(&z80cpu::ld_r8_r8),

            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld h,h
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_athl),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::nop),      // ld l,l
            std::mem_fn(&z80cpu::ld_r8_athl),
            std::mem_fn(&z80cpu::ld_r8_r8),

            std::mem_fn(&z80cpu::ld_athl_r8),
            std::mem_fn(&z80cpu::ld_athl_r8),
            std::mem_fn(&z80cpu::ld_athl_r8),
            std::mem_fn(&z80cpu::ld_athl_r8),
            std::mem_fn(&z80cpu::ld_athl_r8),
            std::mem_fn(&z80cpu::ld_athl_r8),
            std::mem_fn(&z80cpu::halt),
            std::mem_fn(&z80cpu::ld_athl_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_r8),
            std::mem_fn(&z80cpu::ld_r8_athl),
            std::mem_fn(&z80cpu::nop),      // ld a,a

            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_athl),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_r8),
            std::mem_fn(&z80cpu::adx_a_athl),
            std::mem_fn(&z80cpu::adx_a_r8),

            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_athl),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_r8),
            std::mem_fn(&z80cpu::sbx_a_athl),
            std::mem_fn(&z80cpu::sbx_a_r8),

            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::and_athl),
            std::mem_fn(&z80cpu::and_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_r8),
            std::mem_fn(&z80cpu::xor_athl),
            std::mem_fn(&z80cpu::xor_r8),

            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::or_athl),
            std::mem_fn(&z80cpu::or_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_r8),
            std::mem_fn(&z80cpu::cp_athl),
            std::mem_fn(&z80cpu::cp_r8),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_r16),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::jp_nn),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_r16),
            std::mem_fn(&z80cpu::adx_a_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::ret),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::prefix_cb),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::call_nn),
            std::mem_fn(&z80cpu::adx_a_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_r16),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::out_atn_a),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_r16),
            std::mem_fn(&z80cpu::sbx_a_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::exx),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::in_a_atn),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_dd),
            std::mem_fn(&z80cpu::sbx_a_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_r16),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ex_atsp_hl),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_r16),
            std::mem_fn(&z80cpu::and_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::jp_athl),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ex_de_hl),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_ed),
            std::mem_fn(&z80cpu::xor_n),
            std::mem_fn(&z80cpu::rst_xx),

            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::pop_af),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::di),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::push_af),
            std::mem_fn(&z80cpu::or_n),
            std::mem_fn(&z80cpu::rst_xx),
            std::mem_fn(&z80cpu::ret_cc),
            std::mem_fn(&z80cpu::ld_sp_hl),
            std::mem_fn(&z80cpu::jp_cc_nn),
            std::mem_fn(&z80cpu::ei),
            std::mem_fn(&z80cpu::call_cc_nn),
            std::mem_fn(&z80cpu::prefix_fd),
            std::mem_fn(&z80cpu::cp_n),
            std::mem_fn(&z80cpu::rst_xx)
    }};
};

#endif  // SOUND_EMUL_MACHINES_CPU_Z80_H_
