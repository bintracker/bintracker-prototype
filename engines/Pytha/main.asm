;Pytha Beeper Engine
;by utz 06'2017 * www.irrlichtproject.de
;2 channels of tone, triangle/rectangle/saw/noise waveforms

USE_LOOP equ 1
USE_DRUMS equ 1
;include "equates.h"

loop_point_patch equ $8023

noise	 equ $1237
rest	 equ $0
c0	 equ $83
cis0	 equ $8a
d0	 equ $93
dis0	 equ $9b
e0	 equ $a4
f0	 equ $ae
fis0	 equ $b9
g0	 equ $c4
gis0	 equ $cf
a0	 equ $dc
ais0	 equ $e9
b0	 equ $f6
c1	 equ $105
cis1	 equ $115
d1	 equ $125
dis1	 equ $136
e1	 equ $149
f1	 equ $15c
fis1	 equ $171
g1	 equ $187
gis1	 equ $19e
a1	 equ $1b7
ais1	 equ $1d1
b1	 equ $1ed
c2	 equ $20a
cis2	 equ $229
d2	 equ $24a
dis2	 equ $26d
e2	 equ $292
f2	 equ $2b9
fis2	 equ $2e2
g2	 equ $30e
gis2	 equ $33d
a2	 equ $36e
ais2	 equ $3a2
b2	 equ $3da
c3	 equ $414
cis3	 equ $452
d3	 equ $494
dis3	 equ $4da
e3	 equ $524
f3	 equ $572
fis3	 equ $5c5
g3	 equ $61c
gis3	 equ $679
a3	 equ $6dc
ais3	 equ $744
b3	 equ $7b3
c4	 equ $828
cis4	 equ $8a4
d4	 equ $928
dis4	 equ $9b3
e4	 equ $a47
f4	 equ $ae4
fis4	 equ $b89
g4	 equ $c39
gis4	 equ $cf3
a4	 equ $db8
ais4	 equ $e89
b4	 equ $f66
c5	 equ $1051
cis5	 equ $1149
d5	 equ $1250
dis5	 equ $1367
e5	 equ $148e
f5	 equ $15c7
fis5	 equ $1713
g5	 equ $1872
gis5	 equ $19e6
a5	 equ $1b70
ais5	 equ $1d12
b5	 equ $1ecc
c6	 equ $20a1
cis6	 equ $2292
d6	 equ $24a0
dis6	 equ $26ce
e6	 equ $291c
f6	 equ $2b8e
fis6	 equ $2e25
g6	 equ $30e4
gis6	 equ $33cc
a6	 equ $36e0
ais6	 equ $3a24
b6	 equ $3d99
c7	 equ $4142
cis7	 equ $4524
d7	 equ $4940
dis7	 equ $4d9b
e7	 equ $5239
f7	 equ $571c
fis7	 equ $5c4b
g7	 equ $61c7
gis7	 equ $6798
a7	 equ $6dc1

	org #8000

	di
	ld c,#fe
	exx
	push hl			;preserve HL' for return to BASIC
	ld (oldSP),sp
	ld hl,music_data
	ld (seqpntr),hl
	ld ixl,0		;timer lo
	ld c,#fe

;*******************************************************************************
read_seq
seqpntr equ $+1
	ld sp,0
	xor a
	pop iy
	or iyh
	ld (seqpntr),sp
	jr nz,read_ptn0

IF USE_LOOP = 1	
	ld sp,mloop		;get loop point
	jr read_seq+3
ENDIF
;*******************************************************************************
exit
oldSP equ $+1
	ld sp,0
	pop hl
	exx
	ei
	ret

;*******************************************************************************
read_ptn0
	ld sp,iy			;set pattern pointer

read_ptn
	in a,(#fe)			;read kbd
	cpl
	and #1f
	jr nz,exit


	pop af				;flags|speed
	jp m,read_seq
	ld ixh,a			;speed
IF USE_DRUMS = 1
	jp pe,drum
ENDIF
drum_return
	jr z,no_ch1_reload
	
	ex af,af'			;load data ch1
	pop af
	jr c,note_only_ch1 
	
	ld b,a				;offset
	
	ld hl,0
	ld a,h	
	jr nz,set_mod_ch1		;if Z then disable modulator
	
	ld a,4				;inc b
set_mod_ch1
	ld (mod_enable1),a
	jp po,set_noise1
	
	ld hl,#04cb			;rlc h = noise enable
set_noise1
	ld (noise1),hl
	
	pop hl				;waveform
	ld (waveform1),hl
	
note_only_ch1
	ld hl,0
	pop de				;freq divider
	
	ex af,af'
	
no_ch1_reload
	exx
	jr c,no_ch2_reload
			
	pop af				;load data ch2
	jr c,note_only_ch2 
	
	ld b,a				;offset
	
	ld hl,0
	ld a,h	
	jr nz,set_mod_ch2		;if Z then disable modulator
	
	ld a,4				;inc b
set_mod_ch2
	ld (mod_enable2),a
	jp po,set_noise2
	
	ld hl,#04cb			;rlc h = noise enable
set_noise2
	ld (noise2),hl
	
	pop hl				;waveform
	ld (waveform2),hl
	
note_only_ch2
	ld hl,0
	pop de				;freq divider

no_ch2_reload

;*******************************************************************************
;TODO update release with new (correct) timing, update equates.h, update demo song
sound_loop
	exx			;4
	
	add hl,de		;11	;ch1 accu	
	ld a,h			;4
	add a,b			;4	;apply modulator
	or h			;4
	rlca			;4
waveform1
	sbc a,a			;4	;replace with rrca for saw wave
	xor h			;4	;replace with nop for saw/square wave
	rrca			;4
	out (#fe),a		;11__64
	rrca			;4
	out (c),a		;12__16
noise1
	ds 2			;8	;noise w/ rlc h
	rrca			;4
	nop			;4
	exx			;4
	out (c),a		;12__32
	
_ch2	
	add hl,de		;11	;ch2 accu
	
	ld a,h			;4
	add a,b			;4	;apply modulator
	or h			;4
	ret c			;5	;timing
	rlca			;4
waveform2
	sbc a,a			;4	;replace with rrca for saw wave
	xor h			;4	;replace with nop for saw/square wave
	rrca			;4
	out (#fe),a		;12__64
	rrca			;4
	out (c),a		;12__16
noise2
	ds 2			;8	;noise w/ rlc h
	rrca			;4
	dec ixl			;8
	out (c),a		;12__32
	
	jp nz,sound_loop	;10
				;216

mod_enable2	
	inc b				;replace this with nop to disable modulation
	exx

mod_enable1
	inc b				;replace this with nop to disable modulation
	exx
	
	dec ixh
	jp nz,sound_loop
	
	exx
	jp read_ptn
	
;*******************************************************************************
IF USE_DRUMS = 1
drum
	ex af,af'
	ld (deRest),de
	ld (hlRest),hl
	ld (bcRest),bc
	
	pop de
	ld a,d
	rlca
	jr c,drum2
	
drum1					;kick, 
	ld d,a				;D = start pitch
	ld a,e
	ld (slideSpeed),a
	ld e,0
	ld h,e
	ld l,e
	ld c,#3				;length
	
xlllp
	add hl,de
	jr c,noUpd
	ld a,e
slideSpeed equ $+1
	sub #10				;speed
	ld e,a
	sbc a,a
	add a,d
	ld d,a
noUpd
	ld a,h
	and #ff				;border
	out (#fe),a
	djnz xlllp
	dec c
	jr nz,xlllp
				;45680 (/224 = 203.9)
	
	ld ixl,#34			;correct speed offset

drum_end
deRest equ $+1
	ld de,0
hlRest equ $+1
	ld hl,0
bcRest equ $+1
	ld bc,0
	ex af,af'	
	jp drum_return		
	

	
drum2						;noise
	rlca
	ld a,e
	ld (dvol),a
	ld hl,1					;#1 (snare) <- 1011 -> #1237 (hat)
	jr z,d2init
	
	ld hl,#1237
	
d2init				
	ld bc,#ff03				;length
sloop
	add hl,hl		;11
	sbc a,a			;4
	xor l			;4
	ld l,a			;4

dvol equ $+1	
	cp #80			;7		;volume
	sbc a,a			;4
	
	and #ff			;7		;border
	out (#fe),a		;11
	djnz sloop		;13/7 : 65 * 256 * B : B=3 -> 49920 (/224 = 222.8)

	dec c			;4
	jr nz,sloop		;12 : (16 - 6) * B : B=3 -> +30
				;			+load/wrap
				;49903 w/ b=#ff (/224 = 222.8)
	ld ixl,#21		;correct speed offset
	jr drum_end
ENDIF
	
	
;*******************************************************************************
music_data
;	include "music.asm"
