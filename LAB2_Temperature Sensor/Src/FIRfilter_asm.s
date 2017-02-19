	AREA text, CODE, READONLY
	EXPORT FIR_asm

;FIR filter
;variables:
;--------
;R0 : output
;R1	:	filer values
;R2	:	input source
;R3	:	input array length
;--------


FIR_asm

	;VLDR.f32	S0,	[R0,#0x04]	; input array
	;VLDR.f32	S1,	[R1,#0x04]	; filter array
	VLDR.f32 	S5,	[R1, #0x00]	;filter values [0.1, 0.15, 0.5, 0.15, 0.1]
	VLDR.f32 	S1,	[R1, #0x04]
	VLDR.f32 	S2,	[R1, #0x08]
	VLDR.f32 	S3,	[R1, #0x0C]
	VLDR.f32 	S4,	[R1, #0x10]
	;SUB	R3, R3, #0X04
	
iteration
	;S9 = output source, S8 = input source
	VLDR.f32	S9,	[R0, #0x00]
	;VMOV.f32	S9,	[R0, #0x00]
	
	VLDR.f32	S8,	[R2, #0x00]
	;VMOV.f32	S8,	[R2, #0x00]
	VMUL.f32	S9, S5, S8	;S9 = S0*S8
	
	VLDR.f32	S8,	[R2, #0x04]
	;VMOV.f32	S8,	[R2, #0x04]
	VMLA.f32	S9, S1, S8	;S9 = S9 + S1*S8
	
	VLDR.f32	S8,	[R2, #0x08]
	;VMOV.f32	S8,	[R2, #0x08]
	VMLA.f32	S9, S2, S8	;S9 = S9 + S2*S8
	
	VLDR.f32	S8,	[R2, #0x0C]
	;VMOV.f32	S8,	[R2, #0x0C]
	VMLA.f32	S9, S3, S8	;S9 = S9 + S3*S8
	
	VLDR.f32	S8,	[R2, #0x10]
	;VMOV.f32	S8,	[R2, #0x10]
	VMLA.f32	S9, S4, S8	;S9 = S9 + S4*S8
	
	VSTR.f32	S9, [R0, #0x00]	;store S9 into output

	
	add R0, R0, #0x04
	add R2, R2, #0x04
	sub R3, R3, #0x01	;count where it is
	
	CMP	R3, #4
	BNE iteration
	;BGE	iteration
	
	sub R0, R0, #64
	sub R1, R1, #64
	
	BX LR

	END 