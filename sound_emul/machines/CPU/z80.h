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

    void reset() noexcept;

    void setPC(int startAddress) noexcept;
    void setSP(int address) noexcept;
    int getPC() noexcept;

    void execute_cycle() noexcept;
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
    void debugger_print_flags(const int flagRegister) noexcept;
    unsigned get_instruction_timing(int instructionPointer) noexcept;
    unsigned get_conditional_timing(const int instruction) noexcept;

    // generic 16-bit register instructions
    void ld_r16_nn() noexcept;
    void add_hl_r16() noexcept;
    void inc_r16() noexcept;
    void dec_r16() noexcept;
    void pop_r16() noexcept;
    void push_r16() noexcept;

    // generic 8-bit register instructions
    void inc_r8() noexcept;
    void dec_r8() noexcept;
    void ld_r8_n() noexcept;
    void ld_r8_r8() noexcept;
    void ld_r8_athl() noexcept;
    void ld_athl_r8() noexcept;
    void adx_a_r8() noexcept;
    void sbx_a_r8() noexcept;
    void and_r8() noexcept;
    void xor_r8() noexcept;
    void or_r8() noexcept;
    void cp_r8() noexcept;

    // generic conditional instructions
    void jr_cc_n() noexcept;
    void ret_cc() noexcept;
    void jp_cc_nn() noexcept;
    void call_cc_nn() noexcept;

    // misc generic instructions
    void rst_xx() noexcept;

    // regular (non-prefixed) instructions
    void nop() noexcept;
    void ld_atbc_a() noexcept;
    void rlca() noexcept;
    void ex_af_af() noexcept;
    void ld_a_atbc() noexcept;
    void rrca() noexcept;

    void djnz() noexcept;
    void ld_atde_a() noexcept;
    void rla() noexcept;
    void jr() noexcept;
    void ld_a_atde() noexcept;
    void rra() noexcept;

    void ld_atnn_hl() noexcept;
    void daa() noexcept;
    void ld_hl_atnn() noexcept;
    void cpl() noexcept;

    void ld_sp_nn() noexcept;
    void ld_atnn_a() noexcept;
    void inc_sp() noexcept;
    void inc_athl() noexcept;
    void dec_athl() noexcept;
    void ld_athl_n() noexcept;
    void scf() noexcept;
    void add_hl_sp() noexcept;
    void ld_a_atnn() noexcept;
    void dec_sp() noexcept;
    void ccf() noexcept;

    void halt() noexcept;

    void adx_a_athl() noexcept;

    void sbx_a_athl() noexcept;

    void and_athl() noexcept;
    void xor_athl() noexcept;

    void or_athl() noexcept;
    void cp_athl() noexcept;

    void jp_nn() noexcept;
    void ret() noexcept;
    void prefix_cb() noexcept;
    void call_nn() noexcept;
    void adx_a_n() noexcept;

    void out_atn_a() noexcept;
    void exx() noexcept;
    void in_a_atn() noexcept;
    void prefix_dd() noexcept;
    void sbx_a_n() noexcept;

    void ex_atsp_hl() noexcept;
    void and_n() noexcept;
    void jp_athl() noexcept;
    void ex_de_hl() noexcept;
    void prefix_ed() noexcept;
    void xor_n() noexcept;

    void pop_af() noexcept;
    void di() noexcept;
    void push_af() noexcept;
    void or_n() noexcept;
    void ld_sp_hl() noexcept;
    void ei() noexcept;
    void prefix_fd() noexcept;
    void cp_n() noexcept;


    // prefix ED
    void in_r8_atc() noexcept;
    void out_atc_r8() noexcept;
    void sbc_hl_r16() noexcept;
    void adc_hl_r16() noexcept;
    void ld_atnn_r16() noexcept;
    void ld_r16_atnn() noexcept;

    void neg() noexcept;
    void retn() noexcept;
    void im0() noexcept;
    void ld_i_a() noexcept;
    void reti() noexcept;
    void ld_r_a() noexcept;

    void im1() noexcept;
    void ld_a_i() noexcept;
    void im2() noexcept;
    void ld_a_r() noexcept;

    void rrd() noexcept;
    void rld() noexcept;

    void sbc_hl_sp() noexcept;
    void ld_atnn_sp() noexcept;
    void adc_hl_sp() noexcept;
    void ld_sp_atnn() noexcept;

    void ldi() noexcept;
    void cpi() noexcept;
    void ini() noexcept;
    void outi() noexcept;
    void ldd() noexcept;
    void cpd() noexcept;
    void ind() noexcept;
    void outd() noexcept;

    void ldir() noexcept;
    void cpir() noexcept;
    void inir() noexcept;
    void otir() noexcept;
    void lddr() noexcept;
    void cpdr() noexcept;
    void indr() noexcept;
    void otdr() noexcept;

    // prefix CB
    void rlc_r8() noexcept;
    void rlc_athl() noexcept;
    void rrc_r8() noexcept;
    void rrc_athl() noexcept;

    void rl_r8() noexcept;
    void rl_athl() noexcept;
    void rr_r8() noexcept;
    void rr_athl() noexcept;

    void sra_r8() noexcept;
    void sra_athl() noexcept;

    void slx_r8() noexcept;
    void slx_athl() noexcept;
    void srl_r8() noexcept;
    void srl_athl() noexcept;

    void bit_x_r8() noexcept;
    void bit_x_athl() noexcept;

    void res_x_r8() noexcept;
    void res_x_athl() noexcept;

    void set_x_r8() noexcept;
    void set_x_athl() noexcept;


    // prefix DD
    void add_ix_r16() noexcept;
    void ld_r8_atixpd() noexcept;
    void ld_r8_ixh() noexcept;
    void ld_r8_ixl() noexcept;

    void ld_ix_nn() noexcept;
    void ld_atnn_ix() noexcept;
    void inc_ix() noexcept;
    void inc_ixh() noexcept;
    void dec_ixh() noexcept;
    void ld_ixh_n() noexcept;
    void add_ix_ix() noexcept;
    void ld_ix_atnn() noexcept;
    void dec_ix() noexcept;
    void inc_ixl() noexcept;
    void dec_ixl() noexcept;
    void ld_ixl_n() noexcept;

    void inc_atixpd() noexcept;
    void dec_atixpd() noexcept;
    void ld_atixpd_n() noexcept;
    void add_ix_sp() noexcept;

    void ld_ixh_r8() noexcept;
    void ld_ixh_ixl() noexcept;
    void ld_ixl_r8() noexcept;
    void ld_ixl_ixh() noexcept;

    void ld_atixpd_r8() noexcept;

    void adx_a_ixh() noexcept;
    void adx_a_ixl() noexcept;
    void adx_a_atixpd() noexcept;

    void sbx_a_ixh() noexcept;
    void sbx_a_ixl() noexcept;
    void sbx_a_atixpd() noexcept;

    void and_ixh() noexcept;
    void and_ixl() noexcept;
    void and_atixpd() noexcept;
    void xor_ixh() noexcept;
    void xor_ixl() noexcept;
    void xor_atixpd() noexcept;

    void or_ixh() noexcept;
    void or_ixl() noexcept;
    void or_atixpd() noexcept;
    void cp_ixh() noexcept;
    void cp_ixl() noexcept;
    void cp_atixpd() noexcept;

    void prefix_ddcb() noexcept;

    void pop_ix() noexcept;
    void ex_atsp_ix() noexcept;
    void push_ix() noexcept;
    void jp_atix() noexcept;

    void ld_sp_ix() noexcept;


    // prefix FD
    void add_iy_r16() noexcept;
    void ld_r8_atiypd() noexcept;
    void ld_r8_iyh() noexcept;
    void ld_r8_iyl() noexcept;

    void ld_iy_nn() noexcept;
    void ld_atnn_iy() noexcept;
    void inc_iy() noexcept;
    void inc_iyh() noexcept;
    void dec_iyh() noexcept;
    void ld_iyh_n() noexcept;
    void add_iy_iy() noexcept;
    void ld_iy_atnn() noexcept;
    void dec_iy() noexcept;
    void inc_iyl() noexcept;
    void dec_iyl() noexcept;
    void ld_iyl_n() noexcept;

    void inc_atiypd() noexcept;
    void dec_atiypd() noexcept;
    void ld_atiypd_n() noexcept;
    void add_iy_sp() noexcept;

    void ld_iyh_r8() noexcept;
    void ld_iyh_iyl() noexcept;
    void ld_iyl_r8() noexcept;
    void ld_iyl_iyh() noexcept;

    void ld_atiypd_r8() noexcept;

    void adx_a_iyh() noexcept;
    void adx_a_iyl() noexcept;
    void adx_a_atiypd() noexcept;

    void sbx_a_iyh() noexcept;
    void sbx_a_iyl() noexcept;
    void sbx_a_atiypd() noexcept;

    void and_iyh() noexcept;
    void and_iyl() noexcept;
    void and_atiypd() noexcept;
    void xor_iyh() noexcept;
    void xor_iyl() noexcept;
    void xor_atiypd() noexcept;

    void or_iyh() noexcept;
    void or_iyl() noexcept;
    void or_atiypd() noexcept;
    void cp_iyh() noexcept;
    void cp_iyl() noexcept;
    void cp_atiypd() noexcept;

    void prefix_fdcb() noexcept;

    void pop_iy() noexcept;
    void ex_atsp_iy() noexcept;
    void push_iy() noexcept;
    void jp_atiy() noexcept;

    void ld_sp_iy() noexcept;

    // prefix DDCB
    void rlc_atixpd() noexcept;
    void rrc_atixpd() noexcept;
    void rl_atixpd() noexcept;
    void rr_atixpd() noexcept;
    void sla_atixpd() noexcept;
    void sra_atixpd() noexcept;
    void sll_atixpd() noexcept;
    void srl_atixpd() noexcept;

    void bit_x_atixpd() noexcept;
    void res_x_atixpd() noexcept;
    void set_x_atixpd() noexcept;

    // prefix FDCB
    void rlc_atiypd() noexcept;
    void rrc_atiypd() noexcept;
    void rl_atiypd() noexcept;
    void rr_atiypd() noexcept;
    void sla_atiypd() noexcept;
    void sra_atiypd() noexcept;
    void sll_atiypd() noexcept;
    void srl_atiypd() noexcept;

    void bit_x_atiypd() noexcept;
    void res_x_atiypd() noexcept;
    void set_x_atiypd() noexcept;


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
