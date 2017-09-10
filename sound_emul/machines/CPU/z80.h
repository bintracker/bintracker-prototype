#ifndef Z80__H__
#define Z80__H__

#include <functional>
#include <array>

using namespace std;


enum CpuType {NMOS, CMOS, BM1};

class z80cpu {

public:
	int inputPorts[0x10000];
	int inputPortsShort[0x100];
	int outputPorts[0x10000];
	int outputPortsShort[0x100];
	
	void reset();
	
	void setPC(int startAddress);
	void setSP(int address);
	int getPC();
	
	void execute_cycle();
	void execute_debug();
	int non_maskable_interrupt();
	int maskable_interrupt();

	z80cpu(int *mem, int z80Type);
	~z80cpu();
	
private:
	int *memory;
	
	int instructionCycles;
	
	int cpuType;
	
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
	
	
	using CpuFun = decltype(mem_fn(&z80cpu::nop));
	
	//prefix FDCB, rotate/shift
 	const array<CpuFun, 8> cpu_instructions_fdcb {{
 	
 		mem_fn(&z80cpu::rlc_atiypd),
 		mem_fn(&z80cpu::rrc_atiypd),

		mem_fn(&z80cpu::rl_atiypd),
		mem_fn(&z80cpu::rr_atiypd),

		mem_fn(&z80cpu::sla_atiypd),
		mem_fn(&z80cpu::sra_atiypd),

		mem_fn(&z80cpu::sll_atiypd),
		mem_fn(&z80cpu::srl_atiypd)
 	}};
	
	//prefix DDCB, rotate/shift
 	const array<CpuFun, 8> cpu_instructions_ddcb {{
 	
 		mem_fn(&z80cpu::rlc_atixpd),
 		mem_fn(&z80cpu::rrc_atixpd),

		mem_fn(&z80cpu::rl_atixpd),
		mem_fn(&z80cpu::rr_atixpd),

		mem_fn(&z80cpu::sla_atixpd),
		mem_fn(&z80cpu::sra_atixpd),

		mem_fn(&z80cpu::sll_atixpd),
		mem_fn(&z80cpu::srl_atixpd)
 	}};
	
	//prefix FD
	const array<CpuFun, 256> cpu_instructions_fd {{

		mem_fn(&z80cpu::nop),
	 	mem_fn(&z80cpu::ld_r16_nn),
		mem_fn(&z80cpu::ld_atbc_a),
		mem_fn(&z80cpu::inc_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rlca),
		mem_fn(&z80cpu::ex_af_af),
		mem_fn(&z80cpu::add_iy_r16),
		mem_fn(&z80cpu::ld_a_atbc),
		mem_fn(&z80cpu::dec_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rrca),
		
		mem_fn(&z80cpu::djnz),
		mem_fn(&z80cpu::ld_r16_nn),
		mem_fn(&z80cpu::ld_atde_a),
		mem_fn(&z80cpu::inc_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rla),
		mem_fn(&z80cpu::jr),
		mem_fn(&z80cpu::add_iy_r16),
		mem_fn(&z80cpu::ld_a_atde),
		mem_fn(&z80cpu::dec_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rra),		
	
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::ld_iy_nn),
		mem_fn(&z80cpu::ld_atnn_iy),
		mem_fn(&z80cpu::inc_iy),
		mem_fn(&z80cpu::inc_iyh),
		mem_fn(&z80cpu::dec_iyh),
		mem_fn(&z80cpu::ld_iyh_n),
		mem_fn(&z80cpu::daa),
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::add_iy_iy),
		mem_fn(&z80cpu::ld_iy_atnn),
		mem_fn(&z80cpu::dec_iy),
		mem_fn(&z80cpu::inc_iyl),
		mem_fn(&z80cpu::dec_iyl),
		mem_fn(&z80cpu::ld_iyl_n),
		mem_fn(&z80cpu::cpl),
		
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::ld_sp_nn),
		mem_fn(&z80cpu::ld_atnn_a),
		mem_fn(&z80cpu::inc_sp),
		mem_fn(&z80cpu::inc_atiypd),
		mem_fn(&z80cpu::dec_atiypd),
		mem_fn(&z80cpu::ld_atiypd_n),
		mem_fn(&z80cpu::scf),
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::add_iy_sp),
		mem_fn(&z80cpu::ld_a_atnn),
		mem_fn(&z80cpu::dec_sp),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::ccf),
		
		mem_fn(&z80cpu::nop),		//ld b,b
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_iyh),
		mem_fn(&z80cpu::ld_r8_iyl),
		mem_fn(&z80cpu::ld_r8_atiypd),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld c,c
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_iyh),
		mem_fn(&z80cpu::ld_r8_iyl),
		mem_fn(&z80cpu::ld_r8_atiypd),
		mem_fn(&z80cpu::ld_r8_r8),
		
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld d,d
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_iyh),
		mem_fn(&z80cpu::ld_r8_iyl),
		mem_fn(&z80cpu::ld_r8_atiypd),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld e,e
		mem_fn(&z80cpu::ld_r8_iyh),
		mem_fn(&z80cpu::ld_r8_iyl),
		mem_fn(&z80cpu::ld_r8_atiypd),
		mem_fn(&z80cpu::ld_r8_r8),
		
		mem_fn(&z80cpu::ld_iyh_r8),
		mem_fn(&z80cpu::ld_iyh_r8),
		mem_fn(&z80cpu::ld_iyh_r8),
		mem_fn(&z80cpu::ld_iyh_r8),
		mem_fn(&z80cpu::nop),		//mem_fn(&z80cpu::ld_iyh_iyh(),
		mem_fn(&z80cpu::ld_iyh_iyl),
		mem_fn(&z80cpu::ld_r8_atiypd),
		mem_fn(&z80cpu::ld_iyh_r8),
		mem_fn(&z80cpu::ld_iyl_r8),
		mem_fn(&z80cpu::ld_iyl_r8),
		mem_fn(&z80cpu::ld_iyl_r8),
		mem_fn(&z80cpu::ld_iyl_r8),
		mem_fn(&z80cpu::ld_iyl_iyh),
		mem_fn(&z80cpu::nop),		//mem_fn(&z80cpu::ld_iyl_iyl(),
		mem_fn(&z80cpu::ld_r8_atiypd),
		mem_fn(&z80cpu::ld_iyl_r8),
		
		mem_fn(&z80cpu::ld_atiypd_r8),
		mem_fn(&z80cpu::ld_atiypd_r8),
		mem_fn(&z80cpu::ld_atiypd_r8),
		mem_fn(&z80cpu::ld_atiypd_r8),
		mem_fn(&z80cpu::ld_atiypd_r8),
		mem_fn(&z80cpu::ld_atiypd_r8),
		mem_fn(&z80cpu::halt),
		mem_fn(&z80cpu::ld_atiypd_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_iyh),
		mem_fn(&z80cpu::ld_r8_iyl),
		mem_fn(&z80cpu::ld_r8_atiypd),
		mem_fn(&z80cpu::nop),		//ld a,a
		
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_iyh),
		mem_fn(&z80cpu::adx_a_iyl),
		mem_fn(&z80cpu::adx_a_atiypd),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_iyh),
		mem_fn(&z80cpu::adx_a_iyl),
		mem_fn(&z80cpu::adx_a_atiypd),
		mem_fn(&z80cpu::adx_a_r8),
		
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_iyh),
		mem_fn(&z80cpu::sbx_a_iyl),
		mem_fn(&z80cpu::sbx_a_atiypd),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_iyh),
		mem_fn(&z80cpu::sbx_a_iyl),
		mem_fn(&z80cpu::sbx_a_atiypd),
		mem_fn(&z80cpu::sbx_a_r8),
		
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_iyh),
		mem_fn(&z80cpu::and_iyl),
		mem_fn(&z80cpu::and_atiypd),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_iyh),
		mem_fn(&z80cpu::xor_iyl),
		mem_fn(&z80cpu::xor_atiypd),
		mem_fn(&z80cpu::xor_r8),
		
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_iyh),
		mem_fn(&z80cpu::or_iyl),
		mem_fn(&z80cpu::or_atiypd),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_iyh),
		mem_fn(&z80cpu::cp_iyl),
		mem_fn(&z80cpu::cp_atiypd),
		mem_fn(&z80cpu::cp_r8),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_r16),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::jp_nn),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_r16),
		mem_fn(&z80cpu::adx_a_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::ret),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::prefix_ddcb),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::call_nn),
		mem_fn(&z80cpu::adx_a_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_r16),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::out_atn_a),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_r16),
		mem_fn(&z80cpu::sbx_a_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::exx),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::in_a_atn),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_dd),
		mem_fn(&z80cpu::sbx_a_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_iy),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ex_atsp_iy),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_iy),
		mem_fn(&z80cpu::and_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::jp_atiy),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ex_de_hl),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_ed),
		mem_fn(&z80cpu::xor_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_af),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::di),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_af),
		mem_fn(&z80cpu::or_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::ld_sp_iy),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ei),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_fd),
		mem_fn(&z80cpu::cp_n),
		mem_fn(&z80cpu::rst_xx)		
	}};
	
	//prefix DD
	const array<CpuFun, 256> cpu_instructions_dd {{

		mem_fn(&z80cpu::nop),
	 	mem_fn(&z80cpu::ld_r16_nn),
		mem_fn(&z80cpu::ld_atbc_a),
		mem_fn(&z80cpu::inc_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rlca),
		mem_fn(&z80cpu::ex_af_af),
		mem_fn(&z80cpu::add_ix_r16),
		mem_fn(&z80cpu::ld_a_atbc),
		mem_fn(&z80cpu::dec_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rrca),
		
		mem_fn(&z80cpu::djnz),
		mem_fn(&z80cpu::ld_r16_nn),
		mem_fn(&z80cpu::ld_atde_a),
		mem_fn(&z80cpu::inc_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rla),
		mem_fn(&z80cpu::jr),
		mem_fn(&z80cpu::add_ix_r16),
		mem_fn(&z80cpu::ld_a_atde),
		mem_fn(&z80cpu::dec_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rra),		
	
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::ld_ix_nn),
		mem_fn(&z80cpu::ld_atnn_ix),
		mem_fn(&z80cpu::inc_ix),
		mem_fn(&z80cpu::inc_ixh),
		mem_fn(&z80cpu::dec_ixh),
		mem_fn(&z80cpu::ld_ixh_n),
		mem_fn(&z80cpu::daa),
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::add_ix_ix),
		mem_fn(&z80cpu::ld_ix_atnn),
		mem_fn(&z80cpu::dec_ix),
		mem_fn(&z80cpu::inc_ixl),
		mem_fn(&z80cpu::dec_ixl),
		mem_fn(&z80cpu::ld_ixl_n),
		mem_fn(&z80cpu::cpl),
		
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::ld_sp_nn),
		mem_fn(&z80cpu::ld_atnn_a),
		mem_fn(&z80cpu::inc_sp),
		mem_fn(&z80cpu::inc_atixpd),
		mem_fn(&z80cpu::dec_atixpd),
		mem_fn(&z80cpu::ld_atixpd_n),
		mem_fn(&z80cpu::scf),
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::add_ix_sp),
		mem_fn(&z80cpu::ld_a_atnn),
		mem_fn(&z80cpu::dec_sp),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::ccf),
		
		mem_fn(&z80cpu::nop),		//ld b,b
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_ixh),
		mem_fn(&z80cpu::ld_r8_ixl),
		mem_fn(&z80cpu::ld_r8_atixpd),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld c,c
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_ixh),
		mem_fn(&z80cpu::ld_r8_ixl),
		mem_fn(&z80cpu::ld_r8_atixpd),
		mem_fn(&z80cpu::ld_r8_r8),
		
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld d,d
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_ixh),
		mem_fn(&z80cpu::ld_r8_ixl),
		mem_fn(&z80cpu::ld_r8_atixpd),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld e,e
		mem_fn(&z80cpu::ld_r8_ixh),
		mem_fn(&z80cpu::ld_r8_ixl),
		mem_fn(&z80cpu::ld_r8_atixpd),
		mem_fn(&z80cpu::ld_r8_r8),
		
		mem_fn(&z80cpu::ld_ixh_r8),
		mem_fn(&z80cpu::ld_ixh_r8),
		mem_fn(&z80cpu::ld_ixh_r8),
		mem_fn(&z80cpu::ld_ixh_r8),
		mem_fn(&z80cpu::nop),		//mem_fn(&z80cpu::ld_ixh_ixh(),
		mem_fn(&z80cpu::ld_ixh_ixl),
		mem_fn(&z80cpu::ld_r8_atixpd),
		mem_fn(&z80cpu::ld_ixh_r8),
		mem_fn(&z80cpu::ld_ixl_r8),
		mem_fn(&z80cpu::ld_ixl_r8),
		mem_fn(&z80cpu::ld_ixl_r8),
		mem_fn(&z80cpu::ld_ixl_r8),
		mem_fn(&z80cpu::ld_ixl_ixh),
		mem_fn(&z80cpu::nop),		//mem_fn(&z80cpu::ld_ixl_ixl(),
		mem_fn(&z80cpu::ld_r8_atixpd),
		mem_fn(&z80cpu::ld_ixl_r8),
		
		mem_fn(&z80cpu::ld_atixpd_r8),
		mem_fn(&z80cpu::ld_atixpd_r8),
		mem_fn(&z80cpu::ld_atixpd_r8),
		mem_fn(&z80cpu::ld_atixpd_r8),
		mem_fn(&z80cpu::ld_atixpd_r8),
		mem_fn(&z80cpu::ld_atixpd_r8),
		mem_fn(&z80cpu::halt),
		mem_fn(&z80cpu::ld_atixpd_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_ixh),
		mem_fn(&z80cpu::ld_r8_ixl),
		mem_fn(&z80cpu::ld_r8_atixpd),
		mem_fn(&z80cpu::nop),		//ld a,a
		
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_ixh),
		mem_fn(&z80cpu::adx_a_ixl),
		mem_fn(&z80cpu::adx_a_atixpd),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_ixh),
		mem_fn(&z80cpu::adx_a_ixl),
		mem_fn(&z80cpu::adx_a_atixpd),
		mem_fn(&z80cpu::adx_a_r8),
		
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_ixh),
		mem_fn(&z80cpu::sbx_a_ixl),
		mem_fn(&z80cpu::sbx_a_atixpd),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_ixh),
		mem_fn(&z80cpu::sbx_a_ixl),
		mem_fn(&z80cpu::sbx_a_atixpd),
		mem_fn(&z80cpu::sbx_a_r8),
		
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_ixh),
		mem_fn(&z80cpu::and_ixl),
		mem_fn(&z80cpu::and_atixpd),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_ixh),
		mem_fn(&z80cpu::xor_ixl),
		mem_fn(&z80cpu::xor_atixpd),
		mem_fn(&z80cpu::xor_r8),
		
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_ixh),
		mem_fn(&z80cpu::or_ixl),
		mem_fn(&z80cpu::or_atixpd),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_ixh),
		mem_fn(&z80cpu::cp_ixl),
		mem_fn(&z80cpu::cp_atixpd),
		mem_fn(&z80cpu::cp_r8),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_r16),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::jp_nn),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_r16),
		mem_fn(&z80cpu::adx_a_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::ret),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::prefix_ddcb),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::call_nn),
		mem_fn(&z80cpu::adx_a_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_r16),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::out_atn_a),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_r16),
		mem_fn(&z80cpu::sbx_a_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::exx),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::in_a_atn),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_dd),
		mem_fn(&z80cpu::sbx_a_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_ix),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ex_atsp_ix),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_ix),
		mem_fn(&z80cpu::and_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::jp_atix),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ex_de_hl),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_ed),
		mem_fn(&z80cpu::xor_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_af),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::di),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_af),
		mem_fn(&z80cpu::or_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::ld_sp_ix),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ei),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_fd),
		mem_fn(&z80cpu::cp_n),
		mem_fn(&z80cpu::rst_xx)		
	}};
	
	//prefix CB, rotate/shift
	const array<CpuFun, 64> cpu_instructions_cb {{
	
		mem_fn(&z80cpu::rlc_r8),
		mem_fn(&z80cpu::rlc_r8),
		mem_fn(&z80cpu::rlc_r8),
		mem_fn(&z80cpu::rlc_r8),
		mem_fn(&z80cpu::rlc_r8),
		mem_fn(&z80cpu::rlc_r8),
		mem_fn(&z80cpu::rlc_athl),
		mem_fn(&z80cpu::rlc_r8),
		mem_fn(&z80cpu::rrc_r8),
		mem_fn(&z80cpu::rrc_r8),
		mem_fn(&z80cpu::rrc_r8),
		mem_fn(&z80cpu::rrc_r8),
		mem_fn(&z80cpu::rrc_r8),
		mem_fn(&z80cpu::rrc_r8),
		mem_fn(&z80cpu::rrc_athl),
		mem_fn(&z80cpu::rrc_r8),

		mem_fn(&z80cpu::rl_r8),
		mem_fn(&z80cpu::rl_r8),
		mem_fn(&z80cpu::rl_r8),
		mem_fn(&z80cpu::rl_r8),
		mem_fn(&z80cpu::rl_r8),
		mem_fn(&z80cpu::rl_r8),
		mem_fn(&z80cpu::rl_athl),
		mem_fn(&z80cpu::rl_r8),
		mem_fn(&z80cpu::rr_r8),
		mem_fn(&z80cpu::rr_r8),
		mem_fn(&z80cpu::rr_r8),
		mem_fn(&z80cpu::rr_r8),
		mem_fn(&z80cpu::rr_r8),
		mem_fn(&z80cpu::rr_r8),
		mem_fn(&z80cpu::rr_athl),
		mem_fn(&z80cpu::rr_r8),
	
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_athl),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::sra_r8),
		mem_fn(&z80cpu::sra_r8),
		mem_fn(&z80cpu::sra_r8),
		mem_fn(&z80cpu::sra_r8),
		mem_fn(&z80cpu::sra_r8),
		mem_fn(&z80cpu::sra_r8),
		mem_fn(&z80cpu::sra_athl),
		mem_fn(&z80cpu::sra_r8),
	
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::slx_athl),
		mem_fn(&z80cpu::slx_r8),
		mem_fn(&z80cpu::srl_r8),
		mem_fn(&z80cpu::srl_r8),
		mem_fn(&z80cpu::srl_r8),
		mem_fn(&z80cpu::srl_r8),
		mem_fn(&z80cpu::srl_r8),
		mem_fn(&z80cpu::srl_r8),
		mem_fn(&z80cpu::srl_athl),
		mem_fn(&z80cpu::srl_r8)
	}};

	//prefix CB, bit ops
	const array<CpuFun, 8> cpu_instructions_cb_bit {{
	
		mem_fn(&z80cpu::bit_x_r8),
		mem_fn(&z80cpu::bit_x_r8),
		mem_fn(&z80cpu::bit_x_r8),
		mem_fn(&z80cpu::bit_x_r8),
		mem_fn(&z80cpu::bit_x_r8),
		mem_fn(&z80cpu::bit_x_r8),
		mem_fn(&z80cpu::bit_x_athl),
		mem_fn(&z80cpu::bit_x_r8)
	}};
	
	//prefix CB, res ops
	const array<CpuFun, 8> cpu_instructions_cb_res {{
	
		mem_fn(&z80cpu::res_x_r8),
		mem_fn(&z80cpu::res_x_r8),
		mem_fn(&z80cpu::res_x_r8),
		mem_fn(&z80cpu::res_x_r8),
		mem_fn(&z80cpu::res_x_r8),
		mem_fn(&z80cpu::res_x_r8),
		mem_fn(&z80cpu::res_x_athl),
		mem_fn(&z80cpu::res_x_r8)
	}};
	
	//prefix CB, set ops
	const array<CpuFun, 8> cpu_instructions_cb_set {{
	
		mem_fn(&z80cpu::set_x_r8),
		mem_fn(&z80cpu::set_x_r8),
		mem_fn(&z80cpu::set_x_r8),
		mem_fn(&z80cpu::set_x_r8),
		mem_fn(&z80cpu::set_x_r8),
		mem_fn(&z80cpu::set_x_r8),
		mem_fn(&z80cpu::set_x_athl),
		mem_fn(&z80cpu::set_x_r8)
	}};
	
	//prefix ED
	const array<CpuFun, 124> cpu_instructions_ed {{

		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::sbc_hl_r16),
		mem_fn(&z80cpu::ld_atnn_r16),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::retn),
		mem_fn(&z80cpu::im0),
		mem_fn(&z80cpu::ld_i_a),
		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::adc_hl_r16),
		mem_fn(&z80cpu::ld_r16_atnn),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::reti),
		mem_fn(&z80cpu::im0),
		mem_fn(&z80cpu::ld_r_a),
	
		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::sbc_hl_r16),
		mem_fn(&z80cpu::ld_atnn_r16),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::retn),
		mem_fn(&z80cpu::im1),
		mem_fn(&z80cpu::ld_a_i),
		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::adc_hl_r16),
		mem_fn(&z80cpu::ld_r16_atnn),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::reti),
		mem_fn(&z80cpu::im2),
		mem_fn(&z80cpu::ld_a_r),
	
		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::sbc_hl_r16),
		mem_fn(&z80cpu::ld_atnn_hl),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::retn),
		mem_fn(&z80cpu::im0),
		mem_fn(&z80cpu::rrd),
		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::adc_hl_r16),
		mem_fn(&z80cpu::ld_hl_atnn),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::reti),
		mem_fn(&z80cpu::im0),
		mem_fn(&z80cpu::rld),
	
		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::sbc_hl_sp),
		mem_fn(&z80cpu::ld_atnn_sp),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::retn),
		mem_fn(&z80cpu::im1),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::in_r8_atc),
		mem_fn(&z80cpu::out_atc_r8),
		mem_fn(&z80cpu::adc_hl_sp),
		mem_fn(&z80cpu::ld_sp_atnn),
		mem_fn(&z80cpu::neg),
		mem_fn(&z80cpu::reti),
		mem_fn(&z80cpu::im2),
		mem_fn(&z80cpu::nop),
	
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
	
		mem_fn(&z80cpu::ldi),
		mem_fn(&z80cpu::cpi),
		mem_fn(&z80cpu::ini),
		mem_fn(&z80cpu::outi),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::ldd),
		mem_fn(&z80cpu::cpd),
		mem_fn(&z80cpu::ind),
		mem_fn(&z80cpu::outd),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
	
		mem_fn(&z80cpu::ldir),
		mem_fn(&z80cpu::cpir),
		mem_fn(&z80cpu::inir),
		mem_fn(&z80cpu::otir),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::nop),
		mem_fn(&z80cpu::lddr),
		mem_fn(&z80cpu::cpdr),
		mem_fn(&z80cpu::indr),
		mem_fn(&z80cpu::otdr)
	}};

	
//non-prefixed	
	const array<CpuFun, 256> cpu_instructions {{
		mem_fn(&z80cpu::nop),
	 	mem_fn(&z80cpu::ld_r16_nn),
		mem_fn(&z80cpu::ld_atbc_a),
		mem_fn(&z80cpu::inc_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rlca),
		mem_fn(&z80cpu::ex_af_af),
		mem_fn(&z80cpu::add_hl_r16),
		mem_fn(&z80cpu::ld_a_atbc),
		mem_fn(&z80cpu::dec_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rrca),
		
		mem_fn(&z80cpu::djnz),
		mem_fn(&z80cpu::ld_r16_nn),
		mem_fn(&z80cpu::ld_atde_a),
		mem_fn(&z80cpu::inc_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rla),
		mem_fn(&z80cpu::jr),
		mem_fn(&z80cpu::add_hl_r16),
		mem_fn(&z80cpu::ld_a_atde),
		mem_fn(&z80cpu::dec_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::rra),
		
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::ld_r16_nn),
		mem_fn(&z80cpu::ld_atnn_hl),
		mem_fn(&z80cpu::inc_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::daa),
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::add_hl_r16),
		mem_fn(&z80cpu::ld_hl_atnn),
		mem_fn(&z80cpu::dec_r16),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::cpl),
		
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::ld_sp_nn),
		mem_fn(&z80cpu::ld_atnn_a),
		mem_fn(&z80cpu::inc_sp),
		mem_fn(&z80cpu::inc_athl),
		mem_fn(&z80cpu::dec_athl),
		mem_fn(&z80cpu::ld_athl_n),
		mem_fn(&z80cpu::scf),
		mem_fn(&z80cpu::jr_cc_n),
		mem_fn(&z80cpu::add_hl_sp),
		mem_fn(&z80cpu::ld_a_atnn),
		mem_fn(&z80cpu::dec_sp),
		mem_fn(&z80cpu::inc_r8),
		mem_fn(&z80cpu::dec_r8),
		mem_fn(&z80cpu::ld_r8_n),
		mem_fn(&z80cpu::ccf),
		
		mem_fn(&z80cpu::nop),		//ld b,b
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_athl),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld c,c
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_athl),
		mem_fn(&z80cpu::ld_r8_r8),
		
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld d,d
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_athl),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld e,e
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_athl),
		mem_fn(&z80cpu::ld_r8_r8),
		
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld h,h
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_athl),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::nop),		//ld l,l
		mem_fn(&z80cpu::ld_r8_athl),
		mem_fn(&z80cpu::ld_r8_r8),
		
		mem_fn(&z80cpu::ld_athl_r8),
		mem_fn(&z80cpu::ld_athl_r8),
		mem_fn(&z80cpu::ld_athl_r8),
		mem_fn(&z80cpu::ld_athl_r8),
		mem_fn(&z80cpu::ld_athl_r8),
		mem_fn(&z80cpu::ld_athl_r8),
		mem_fn(&z80cpu::halt),
		mem_fn(&z80cpu::ld_athl_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_r8),
		mem_fn(&z80cpu::ld_r8_athl),
		mem_fn(&z80cpu::nop),		//ld a,a
		
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_athl),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_r8),
		mem_fn(&z80cpu::adx_a_athl),
		mem_fn(&z80cpu::adx_a_r8),
		
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_athl),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_r8),
		mem_fn(&z80cpu::sbx_a_athl),
		mem_fn(&z80cpu::sbx_a_r8),
		
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::and_athl),
		mem_fn(&z80cpu::and_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_r8),
		mem_fn(&z80cpu::xor_athl),
		mem_fn(&z80cpu::xor_r8),

		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::or_athl),
		mem_fn(&z80cpu::or_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_r8),
		mem_fn(&z80cpu::cp_athl),
		mem_fn(&z80cpu::cp_r8),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_r16),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::jp_nn),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_r16),
		mem_fn(&z80cpu::adx_a_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::ret),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::prefix_cb),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::call_nn),
		mem_fn(&z80cpu::adx_a_n),
		mem_fn(&z80cpu::rst_xx),
				
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_r16),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::out_atn_a),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_r16),
		mem_fn(&z80cpu::sbx_a_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::exx),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::in_a_atn),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_dd),
		mem_fn(&z80cpu::sbx_a_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_r16),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ex_atsp_hl),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_r16),
		mem_fn(&z80cpu::and_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::jp_athl),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ex_de_hl),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_ed),
		mem_fn(&z80cpu::xor_n),
		mem_fn(&z80cpu::rst_xx),
		
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::pop_af),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::di),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::push_af),
		mem_fn(&z80cpu::or_n),
		mem_fn(&z80cpu::rst_xx),
		mem_fn(&z80cpu::ret_cc),
		mem_fn(&z80cpu::ld_sp_hl),
		mem_fn(&z80cpu::jp_cc_nn),
		mem_fn(&z80cpu::ei),
		mem_fn(&z80cpu::call_cc_nn),
		mem_fn(&z80cpu::prefix_fd),
		mem_fn(&z80cpu::cp_n),
		mem_fn(&z80cpu::rst_xx)
	}};
};

#endif
