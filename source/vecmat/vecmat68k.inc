;/*
;THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
;SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
;END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
;ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
;IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
;SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
;FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
;CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
;AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.  
;COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
;*/
;
;
; Header file for vector/matrix library
;
;

	IF (&TYPE('_VECMAT_INC') = 'UNDEFINED') THEN
_VECMAT_INC equ 1

	include	'fix.inc'

	machine	MC68040

;Structures

vms_vector	RECORD	0
x	ds	fix
y	ds	fix
z	ds	fix
	ORG	x
xyz	ds	fix
	ds	fix
	ds	fix
size	equ	*
	ENDR


vms_svec	RECORD	0
sv_x	ds.w	1
sv_y	ds.w	1
sv_z	ds.w	1
	ORG	sv_x
sv_xyz	ds.w	3
size	equ	*
	ENDR


vms_angvec	RECORD	0
pitch	ds	fixang
bank	ds	fixang
head	ds	fixang
size	equ	*
	ENDR


vms_matrix	RECORD	0
m1	ds	fix
m4	ds	fix
m7	ds	fix
m2	ds	fix
m5	ds	fix
m8	ds	fix
m3	ds	fix
m6	ds	fix
m9	ds	fix
	ORG	m1
rvec	ds	vms_vector
uvec	ds	vms_vector
fvec	ds	vms_vector
size	equ	*
	ENDR


;Macros

;copies one vector to another, using the register specified.  If none
;specified, uses d0.l
	MACRO
&labl	vm_copy	&src,&dest,&reg=d0
	LCLC	&s1, &s2, &s3, &s4
	IF &POS('(',&src) THEN
&s1	SETC	&SUBSTR(&src,1,&POS('(',&src)-1)
&s2	SETC	&SUBSTR(&src,&POS('(',&src),&LEN(&src))
	IF (&LEN(&s1) = 0) THEN
&s1	SETC	'vms_vector'
	ENDIF
	ELSE
&s1	SETC	&src
&s2	SETC	''
	ENDIF
	IF &POS('(',&dest) THEN
&s3	SETC	&SUBSTR(&dest,1,&POS('(',&dest)-1)
&s4	SETC	&SUBSTR(&dest,&POS('(',&dest),&LEN(&dest))
	IF (&LEN(&s3) = 0) THEN
&s3	SETC	'vms_vector'
	ENDIF
	ELSE
&s3	SETC	&dest
&s4	SETC	''	
	ENDIF
	move.l	&CONCAT(&s1,'.x',&s2), &reg
	move.l	&reg, &CONCAT(&s3,'.x',&s4)
	move.l	&CONCAT(&s1,'.y',&s2), &reg
	move.l	&reg, &CONCAT(&s3,'.y',&s4)
	move.l	&CONCAT(&s1,'.z',&s2), &reg
	move.l	&reg, &CONCAT(&s3,'.z',&s4)
	ENDM

;copies one angvec to another, using the register specified.  If none
;specified, uses eax (and ax). Note the trick to get the word part of
;the register without knowing what the register is.
;@vm_acopy	macro	dest,src,reg:=<eax>
;@	mov	reg,fix ptr [src].p	;copy two at once
;@	mov	fix ptr [dest].p,reg
;@	db	66h	;size override, use short
;@	mov	reg,fix ptr [src].h ;copy last angle
;@	db	66h	;size override, use short
;@	mov	fix ptr [dest].h,reg
;@	endm


	IF (&TYPE('_NO_VECMAT_IMPORTS') = 'UNDEFINED') THEN

;Global contants
	import	_vmd_zero_vector:DATA
	import	_vmd_identity_matrix:DATA

;Routines

;register usage appears here, but see VECMAT.H for other info

	import	vm_vec_add		;d0.l=dest, a0,a1=srcs
	import	vm_vec_sub		;d0.l=dest, a0,a1=srcs
	import	vm_vec_add2		;a1=dest, a0=source
	import	vm_vec_sub2		;a1=dest, a0=source
	import	vm_vec_avg		;d0.l=dest, a0,a1=srcs
	import	vm_vec_avg4		;d0.l=dest, a0,a1,a2,a3=srcs
	import	vm_vec_scale		;a2=vec, d2.l=scale
	import	vm_vec_copy_scale	;a1=dest, a2=src, d2.l=scale
	import	vm_vec_scale2		;a1=vec, d1.l=n, d2.l=d
	import	vm_vec_mag		;a0=vec, returns d0.l=mag
	import	vm_vec_dist		;a0,a1=vecs, returns d0.l=dist
	import	vm_vec_mag_quick	;a0=vec, returns d0.l=approx dist
	import	vm_vec_dist_quick	;a0,a1=vecs, returns d0.l=approx dist
	import	vm_vec_normalize	;a0=vec, returns d2.l=mag
	import	vm_vec_normalize_quick	;a0=vec, returns d2.l=mag
	import	vm_vec_copy_normalize	;a1=dest, a0=src
	import	vm_vec_copy_normalize_quick	;a1=dest, a0=src
	import	vm_vec_normalized_dir	;a1=dest, a0=endpoint, d1.l=startpoint
	import	vm_vec_normalized_dir_quick	;a1=dest, a0=endpoint, d1.l=startpoint
	import	vm_vec_dotprod		;a0,a1=vecs, ret d0.l=dotprod
	import	vm_vec_crossprod	;d0.l=dest, a0,a1=srcs
	import	vm_vec_normal		;a2=dest, d0.l,a0,a1=srcs
	import	vm_vec_perp		;a2=dest, d0.l,a0,a1=srcs
	import	vm_vec_delta_ang	;a0,a1=vecs, d0.l=(optional) fvec, ret ax=angle
	import	vm_vec_delta_ang_norm	;a0,a1=vec, ret ax=angle
	import	vm_angles_2_matrix	;a1=dest, a0=angvec
	import	vm_vector_2_matrix	;a1=dest, a0=fwdvec, d0.l=upvec, ebx=rightvec
	import	vm_vector_2_matrix_norm	;a1=dest, a0=fwdvec, d0.l=upvec, ebx=rightvec
	import	vm_vec_rotate		;d0.l=dest, a0=src, a1=matrix
	import	vm_transpose_matrix	;a1=matrix (transpose in place)
	import	vm_copy_transpose_matrix ;a1=dest, a0=src
	import	vm_matrix_x_matrix	;d0.l=dest, a0,a1=srcs
	import	vm_vec_ang_2_matrix	;a0=vector, d0.l=angle, a1=matrix
	import	vm_dist_to_plane	;ebx=norm, a1=plane pnt, a0=check pnt, ret d0.l=dist
	import	vm_extract_angles_matrix ;a1=angles, a0=matrix
	import	vm_vec_scale_add	;a1=dest, ebx=src1, a0=src2, ecx=scale
	import	vm_vec_scale_add2	;a1=dest, a0=src, ecx=scale
	import	vm_extract_angles_vector	;a1=angvec, a0=vec TRASHES a0
	import	vm_extract_angles_vector_normalized	;a1=angvec, a0=vec

	ENDIF	;_NO_VECMAT_IMPORTS

	ENDIF	;_VECMAT_INC

