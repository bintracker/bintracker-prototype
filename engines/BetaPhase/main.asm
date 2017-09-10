;BetaPhase - ZX Spectrum beeper engine - r0.4
;experimental pulse-interleaving synthesis without duty threshold comparison
;by utz 2016-2017, based on an original concept by Shiru


endPtn		equ #40

noUpd1		equ #1
noUpd2		equ #4
noUpd3		equ #80

phaseReset	equ #1
dutyModOn	equ #40

scaleDown	equ #0f		;rrca
scaleUp		equ #07		;rlca
dMod		equ #57		;ld d,a

mXor		equ #ac00		;xor (iy)h
mAnd		equ #a400		;and (iy)h
mOr		equ #b400		;or (iy)h
mNone		equ #b700		;or a

slideUp		equ #8000

rest		equ #0

ckick		equ #4
chat		equ #80


a0	 equ #1c
ais0	 equ #1e
b0	 equ #20
c1	 equ #22
cis1	 equ #24
d1	 equ #26
dis1	 equ #28
e1	 equ #2b
f1	 equ #2d
fis1	 equ #30
g1	 equ #33
gis1	 equ #36
a1	 equ #39
ais1	 equ #3c
b1	 equ #40
c2	 equ #44
cis2	 equ #48
d2	 equ #4c
dis2	 equ #50
e2	 equ #55
f2	 equ #5a
fis2	 equ #60
g2	 equ #65
gis2	 equ #6b
a2	 equ #72
ais2	 equ #79
b2	 equ #80
c3	 equ #87
cis3	 equ #8f
d3	 equ #98
dis3	 equ #a1
e3	 equ #ab
f3	 equ #b5
fis3	 equ #bf
g3	 equ #cb
gis3	 equ #d7
a3	 equ #e4
ais3	 equ #f1
b3	 equ #100
c4	 equ #10f
cis4	 equ #11f
d4	 equ #130
dis4	 equ #142
e4	 equ #155
f4	 equ #169
fis4	 equ #17f
g4	 equ #196
gis4	 equ #1ae
a4	 equ #1c7
ais4	 equ #1e2
b4	 equ #1ff
c5	 equ #21d
cis5	 equ #23e
d5	 equ #260
dis5	 equ #284
e5	 equ #2aa
f5	 equ #2d3
fis5	 equ #2fe
g5	 equ #32b
gis5	 equ #35b
a5	 equ #38f
ais5	 equ #3c5
b5	 equ #3fe
c6	 equ #43b
cis6	 equ #47b
d6	 equ #4bf
dis6	 equ #508
e6	 equ #554
f6	 equ #5a5
fis6	 equ #5fb
g6	 equ #656
gis6	 equ #6b7
a6	 equ #71d
ais6	 equ #789
b6	 equ #7fc
c7	 equ #876
cis7	 equ #8f6
d7	 equ #97f
dis7	 equ #a0f
e7	 equ #aa9
f7	 equ #b4b
fis7	 equ #bf7
g7	 equ #cad
gis7	 equ #d6e
a7	 equ #e3a



	org #8000
	

	di	
	exx
	push hl			;preserve HL' for return to BASIC
	push ix
	push iy
	ld (oldSP),sp
	ld hl,musicData
	ld (seqpntr),hl

;*******************************************************************************
rdseq
	exx
seqpntr equ $+1
	ld sp,0
	xor a
	pop de			;pattern pointer to DE
	or d
	ld (seqpntr),sp
	jr nz,rdptn0

;	jp exit		;uncomment to disable looping
	ld sp,mloop		;get loop point
	jr rdseq+3

;*******************************************************************************
rdptn0
	ld (ptnpntr),de
	ld e,0			;reset timer lo-byte
	
rdptn
	exx
	in a,(#fe)		;read kbd
	cpl
	and #1f
	jp nz,exit


ptnpntr equ $+1
	ld sp,0
	
	pop af			;
	jr z,rdseq
	
	jr c,skipUpdate1	;***ch1***
	ex af,af'

	pop af
	ld (preScale1A),a	;preScale1A|phase reset enable
	jr nc,_skipPhaseReset
	
	pop de			;pop phase offset
	ld hl,0
	
_skipPhaseReset
	ld a,#7a		;ld a,d
	jr nz,_setDutyMod
	
	ld a,#82		;add a,d
_setDutyMod
	ld (dutyMod),a
	

	pop bc			;mixMethod + preScale1B
	ld (preScale1B),bc
	
	pop bc			;freq divider
	
	ld a,b			;disable output on rests
	or c
 	jr nz,_skipPhaseReset2

	ld a,#af		;#af = xor a
	ld (mix1),a

_skipPhaseReset2
	
	ex af,af'

skipUpdate1			;***ch2***
	jp pe,skipUpdate2
	
	ld (_restoreHL),hl
	ex af,af'
	
	pop af			;mix2|phase reset enable
	ld (mix2),a	
	jr nc,_skipPhaseReset	;phase reset yes/no
	
	pop iy
	ld ix,0

_skipPhaseReset
	jr nz,_noDutyMod2
	
	pop af
	ld (dutyMod2),a
_noDutyMod2

	pop hl			;preScale2A/B
	ld (preScale2A),hl
				
	pop hl			;freq div
	ld (noteDiv2),hl
	
	ld a,h
	or l
	jr nz,_skipPhaseReset2	;disable output on rests
	
	ld a,#af		;#af = xor a
	ld (mix2),a
_skipPhaseReset2
	
	ex af,af'
_restoreHL equ $+1
	ld hl,0	

skipUpdate2			;***ch3***
	exx
	jp m,skipUpdate3
	ex af,af'
	
	pop hl			;postscale + slide amount
	ld a,h
	ld (postScale3),a
	ld a,l
	ld (slideAmount),a
	
	pop bc			;freq divider ch3 + slide dir
	
	ld hl,#809f		;sbc a,a \ add a,b
	ld a,#d6		;sub n
	sla b			;bit 7 set = slide up
	jr nc,_slideDown
	
	ld hl,#9188		;adc a,b \ sub c
	ld a,#ce		;add a,n

_slideDown
	ld (slideDirectionA),a
	ld (slideDirectionB),hl
	sra b			;restore freqdiv hi-byte
	
	ld hl,0			;phase reset
	ex af,af'
	
skipUpdate3
	ld d,a			;timer
	ld (ptnpntr),sp

noteDiv2 equ $+1
	ld sp,0	
	
;*******************************************************************************	
playNote
	exx			;4

	add hl,bc		;11		;ch1 (phaser/sid/noise)
	ex de,hl		;4
	add hl,bc		;11

	sbc a,a			;4		;sync for duty modulation
dutyMod	
	ld a,d			;4		;ld a,d = #7a (disable), add a,d = #82 (enable)
preScale1A
	nop			;4		;switch rrca/rlca/... *2 | ld d,a = #57 (enable sweep) | rlc d = #cb(02) for noise
preScale1B
	nop			;4		;also for rlc h... osc 2 off = noise? rlc l & prescale? or move it down | #(cb)02 for noise
mix1
	xor h			;4		;switch xor|or|and|or a|xor a (disable output)
	ret c			;5		;timing TODO: careful, this will fail if mix op doesn't reset carry

	out (#fe),a		;11___80 (ch3)
	
	ex de,hl		;4	
	ld a,0			;7		;timing

	
	add ix,sp		;15		;ch2 (phaser/noise)					
	add iy,sp		;15
	ld a,ixh		;8

preScale2A
	nop			;4
preScale2B
	nop			;4
mix2 equ $+1
	xor iyh			;8

	exx			;4
	out (#fe),a		;11___80 (ch1)	

	
	add hl,bc		;11		;ch3 (slide)
	jr nc,noSlideUpdate	;7/12
	
	ld a,c			;4
slideDirectionA
slideAmount equ $+1
	add a,0			;7		;add a,n = #ce, sub n = #d6
	ld c,a			;4
	
slideDirectionB
	adc a,b			;4		;sbc a,a	;adc a,b; sub c = #9188 | sbc a,a; add a,b = #809f
	sub c			;4		;add a,d
	ld b,a			;4
		
slideReturn
	ld a,h			;4
postScale3
	nop			;4		;switch
	out (#fe),a		;11___64 (ch2)
	
	dec e			;4
	jp nz,playNote		;10
				;224

	ld a,ixl				;duty modulator ch2
dutyMod2 equ $+1
	add a,0
	ld ixl,a
	
	ld a,ixh
	adc a,0
	ld ixh,a
			
	dec d
	jp nz,playNote
	
	ld (noteDiv2),sp
	jp rdptn

;*******************************************************************************	
noSlideUpdate
	jr _aa			;12
_aa	jp slideReturn		;10+12+12=34

;*******************************************************************************
exit
oldSP equ $+1
	ld sp,0
	pop iy
	pop ix
	pop hl
	exx
	ei
	ret
;*******************************************************************************
musicData
;	include "music.asm"