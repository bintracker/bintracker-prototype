#ifndef Z80_CPU__H__
#define Z80_CPU__H__

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
	~z80cpu();

private:
//	int *memory;
    std::array<int, 0x10000> *memory;
	int instructionCycles;

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

	int regDummy;	//dummy register for generic instructions

	int *const regPtr[8] = {&regB, &regC, &regD, &regE, &regH, &regL, &regDummy, &regA};

	static const int conditionCodes[4];

	int parity_tbl[256];

//generic 16-bit register instructions

	int ld_r16_nn();
	int add_hl_r16();
	int inc_r16();
	int dec_r16();
	int pop_r16();
	int push_r16();

//generic 8-bit register instructions

	int inc_r8();
	int dec_r8();
	int ld_r8_n();
	int ld_r8_r8();
	int ld_r8_athl();
	int ld_athl_r8();
	int adx_a_r8();
	int sbx_a_r8();
	int and_r8();
	int xor_r8();
	int or_r8();
	int cp_r8();

//generic conditional instructions
	int jr_cc_n();
	int ret_cc();
	int jp_cc_nn();
	int call_cc_nn();

//misc generic instructions
	int rst_xx();

//regular (non-prefixed) instructions

	int nop();
	int ld_atbc_a();
	int rlca();
	int ex_af_af();
	int ld_a_atbc();
	int rrca();

	int djnz();
	int ld_atde_a();
	int rla();
	int jr();
	int ld_a_atde();
	int rra();

	int ld_atnn_hl();
	int daa();
	int ld_hl_atnn();
	int cpl();

	int ld_sp_nn();
	int ld_atnn_a();
	int inc_sp();
	int inc_athl();
	int dec_athl();
	int ld_athl_n();
	int scf();
	int add_hl_sp();
	int ld_a_atnn();
	int dec_sp();
	int ccf();

	int halt();

	int adx_a_athl();

	int sbx_a_athl();

	int and_athl();
	int xor_athl();

	int or_athl();
	int cp_athl();

	int jp_nn();
	int ret();
	int prefix_cb();
	int call_nn();
	int adx_a_n();

	int out_atn_a();
	int exx();
	int in_a_atn();
	int prefix_dd();
	int sbx_a_n();

	int ex_atsp_hl();
	int and_n();
	int jp_athl();
	int ex_de_hl();
	int prefix_ed();
	int xor_n();

	int pop_af();
	int di();
	int push_af();
	int or_n();
	int ld_sp_hl();
	int ei();
	int prefix_fd();
	int cp_n();


//prefix ED

	int in_r8_atc();
	int out_atc_r8();
	int sbc_hl_r16();
	int adc_hl_r16();
	int ld_atnn_r16();
	int ld_r16_atnn();

	int neg();
	int retn();
	int im0();
	int ld_i_a();
	int reti();
	int ld_r_a();

	int im1();
	int ld_a_i();
	int im2();
	int ld_a_r();

	int rrd();
	int rld();

	int sbc_hl_sp();
	int ld_atnn_sp();
	int adc_hl_sp();
	int ld_sp_atnn();

	int ldi();
	int cpi();
	int ini();
	int outi();
	int ldd();
	int cpd();
	int ind();
	int outd();

	int ldir();
	int cpir();
	int inir();
	int otir();
	int lddr();
	int cpdr();
	int indr();
	int otdr();

//prefix CB

	int rlc_r8();
	int rlc_athl();
	int rrc_r8();
	int rrc_athl();

	int rl_r8();
	int rl_athl();
	int rr_r8();
	int rr_athl();

	int sra_r8();
	int sra_athl();

	int slx_r8();
	int slx_athl();
	int srl_r8();
	int srl_athl();

 	int bit_x_r8();
	int bit_x_athl();

 	int res_x_r8();
	int res_x_athl();

	int set_x_r8();
	int set_x_athl();


//prefix DD
	int add_ix_r16();
	int ld_r8_atixpd();
	int ld_r8_ixh();
	int ld_r8_ixl();

	int ld_ix_nn();
	int ld_atnn_ix();
	int inc_ix();
	int inc_ixh();
	int dec_ixh();
	int ld_ixh_n();
	int add_ix_ix();
	int ld_ix_atnn();
	int dec_ix();
	int inc_ixl();
	int dec_ixl();
	int ld_ixl_n();

	int inc_atixpd();
	int dec_atixpd();
	int ld_atixpd_n();
	int add_ix_sp();

	int ld_ixh_r8();
	int ld_ixh_ixl();
	int ld_ixl_r8();
	int ld_ixl_ixh();

	int ld_atixpd_r8();

	int adx_a_ixh();
	int adx_a_ixl();
	int adx_a_atixpd();

	int sbx_a_ixh();
	int sbx_a_ixl();
	int sbx_a_atixpd();

	int and_ixh();
	int and_ixl();
	int and_atixpd();
	int xor_ixh();
	int xor_ixl();
	int xor_atixpd();

	int or_ixh();
	int or_ixl();
	int or_atixpd();
	int cp_ixh();
	int cp_ixl();
	int cp_atixpd();

	int prefix_ddcb();

	int pop_ix();
	int ex_atsp_ix();
	int push_ix();
	int jp_atix();

	int ld_sp_ix();


//prefix FD
	int add_iy_r16();
	int ld_r8_atiypd();
	int ld_r8_iyh();
	int ld_r8_iyl();

	int ld_iy_nn();
	int ld_atnn_iy();
	int inc_iy();
	int inc_iyh();
	int dec_iyh();
	int ld_iyh_n();
	int add_iy_iy();
	int ld_iy_atnn();
	int dec_iy();
	int inc_iyl();
	int dec_iyl();
	int ld_iyl_n();

	int inc_atiypd();
	int dec_atiypd();
	int ld_atiypd_n();
	int add_iy_sp();

	int ld_iyh_r8();
	int ld_iyh_iyl();
	int ld_iyl_r8();
	int ld_iyl_iyh();

	int ld_atiypd_r8();

	int adx_a_iyh();
	int adx_a_iyl();
	int adx_a_atiypd();

	int sbx_a_iyh();
	int sbx_a_iyl();
	int sbx_a_atiypd();

	int and_iyh();
	int and_iyl();
	int and_atiypd();
	int xor_iyh();
	int xor_iyl();
	int xor_atiypd();

	int or_iyh();
	int or_iyl();
	int or_atiypd();
	int cp_iyh();
	int cp_iyl();
	int cp_atiypd();

	int prefix_fdcb();

	int pop_iy();
	int ex_atsp_iy();
	int push_iy();
	int jp_atiy();

	int ld_sp_iy();

//prefix DDCB
	int rlc_atixpd();
	int rrc_atixpd();
	int rl_atixpd();
	int rr_atixpd();
	int sla_atixpd();
	int sra_atixpd();
	int sll_atixpd();
	int srl_atixpd();

	int bit_x_atixpd();
	int res_x_atixpd();
	int set_x_atixpd();

//prefix FDCB
	int rlc_atiypd();
	int rrc_atiypd();
	int rl_atiypd();
	int rr_atiypd();
	int sla_atiypd();
	int sra_atiypd();
	int sll_atiypd();
	int srl_atiypd();

	int bit_x_atiypd();
	int res_x_atiypd();
	int set_x_atiypd();


	using CpuFun = decltype(std::mem_fn(&z80cpu::nop));

	//prefix FDCB, rotate/shift
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

	//prefix DDCB, rotate/shift
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

	//prefix FD
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

		std::mem_fn(&z80cpu::nop),		//ld b,b
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_iyh),
		std::mem_fn(&z80cpu::ld_r8_iyl),
		std::mem_fn(&z80cpu::ld_r8_atiypd),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld c,c
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_iyh),
		std::mem_fn(&z80cpu::ld_r8_iyl),
		std::mem_fn(&z80cpu::ld_r8_atiypd),
		std::mem_fn(&z80cpu::ld_r8_r8),

		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld d,d
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_iyh),
		std::mem_fn(&z80cpu::ld_r8_iyl),
		std::mem_fn(&z80cpu::ld_r8_atiypd),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld e,e
		std::mem_fn(&z80cpu::ld_r8_iyh),
		std::mem_fn(&z80cpu::ld_r8_iyl),
		std::mem_fn(&z80cpu::ld_r8_atiypd),
		std::mem_fn(&z80cpu::ld_r8_r8),

		std::mem_fn(&z80cpu::ld_iyh_r8),
		std::mem_fn(&z80cpu::ld_iyh_r8),
		std::mem_fn(&z80cpu::ld_iyh_r8),
		std::mem_fn(&z80cpu::ld_iyh_r8),
		std::mem_fn(&z80cpu::nop),		//std::mem_fn(&z80cpu::ld_iyh_iyh(),
		std::mem_fn(&z80cpu::ld_iyh_iyl),
		std::mem_fn(&z80cpu::ld_r8_atiypd),
		std::mem_fn(&z80cpu::ld_iyh_r8),
		std::mem_fn(&z80cpu::ld_iyl_r8),
		std::mem_fn(&z80cpu::ld_iyl_r8),
		std::mem_fn(&z80cpu::ld_iyl_r8),
		std::mem_fn(&z80cpu::ld_iyl_r8),
		std::mem_fn(&z80cpu::ld_iyl_iyh),
		std::mem_fn(&z80cpu::nop),		//std::mem_fn(&z80cpu::ld_iyl_iyl(),
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
		std::mem_fn(&z80cpu::nop),		//ld a,a

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

	//prefix DD
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

		std::mem_fn(&z80cpu::nop),		//ld b,b
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_ixh),
		std::mem_fn(&z80cpu::ld_r8_ixl),
		std::mem_fn(&z80cpu::ld_r8_atixpd),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld c,c
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_ixh),
		std::mem_fn(&z80cpu::ld_r8_ixl),
		std::mem_fn(&z80cpu::ld_r8_atixpd),
		std::mem_fn(&z80cpu::ld_r8_r8),

		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld d,d
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_ixh),
		std::mem_fn(&z80cpu::ld_r8_ixl),
		std::mem_fn(&z80cpu::ld_r8_atixpd),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld e,e
		std::mem_fn(&z80cpu::ld_r8_ixh),
		std::mem_fn(&z80cpu::ld_r8_ixl),
		std::mem_fn(&z80cpu::ld_r8_atixpd),
		std::mem_fn(&z80cpu::ld_r8_r8),

		std::mem_fn(&z80cpu::ld_ixh_r8),
		std::mem_fn(&z80cpu::ld_ixh_r8),
		std::mem_fn(&z80cpu::ld_ixh_r8),
		std::mem_fn(&z80cpu::ld_ixh_r8),
		std::mem_fn(&z80cpu::nop),		//std::mem_fn(&z80cpu::ld_ixh_ixh(),
		std::mem_fn(&z80cpu::ld_ixh_ixl),
		std::mem_fn(&z80cpu::ld_r8_atixpd),
		std::mem_fn(&z80cpu::ld_ixh_r8),
		std::mem_fn(&z80cpu::ld_ixl_r8),
		std::mem_fn(&z80cpu::ld_ixl_r8),
		std::mem_fn(&z80cpu::ld_ixl_r8),
		std::mem_fn(&z80cpu::ld_ixl_r8),
		std::mem_fn(&z80cpu::ld_ixl_ixh),
		std::mem_fn(&z80cpu::nop),		//std::mem_fn(&z80cpu::ld_ixl_ixl(),
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
		std::mem_fn(&z80cpu::nop),		//ld a,a

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

	//prefix CB, rotate/shift
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

	//prefix CB, bit ops
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

	//prefix CB, res ops
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

	//prefix CB, set ops
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

	//prefix ED
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


//non-prefixed
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

		std::mem_fn(&z80cpu::nop),		//ld b,b
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_athl),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld c,c
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_athl),
		std::mem_fn(&z80cpu::ld_r8_r8),

		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld d,d
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_athl),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld e,e
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_athl),
		std::mem_fn(&z80cpu::ld_r8_r8),

		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld h,h
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_athl),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::ld_r8_r8),
		std::mem_fn(&z80cpu::nop),		//ld l,l
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
		std::mem_fn(&z80cpu::nop),		//ld a,a

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

#endif
