#
# *  opti_cvrt.s
# *
# *  conversions functions between graphic modes
# *
# *  (trifouilli extrait de l'optilib en version pre-beta)
# *
#

	
.global opti_memwset
	

	
.global SOpti_cvrt_RGBA_8888_RGB_565
.global SOpti_cvrt_bRGBA_8888_RGB_565
.global SOpti_cvrt_mRGBA_8888_RGB_565
.global SOpti_cvrt_lABGR_8888_RGB_565

.global SOpti_cvrt_BGRA_8888_RGB_565
.global SOpti_cvrt_bBGRA_8888_RGB_565
.global SOpti_cvrt_mBGRA_8888_RGB_565
.global SOpti_cvrt_lARGB_8888_RGB_565
	
.global SOpti_cvrt_RGB_565_ARGB_8888
.global SOpti_cvrt_RGB_565_bBGRA_8888
.global SOpti_cvrt_RGB_565_mBGRA_8888
.global SOpti_cvrt_RGB_565_lARGB_8888	

	
.text

opti_memwset:
	pushl	%ebp
	movl	%esp,%ebp
	
	pushl	%edi		# registres a sauvegarder (convention gcc)
	#pushl	%esi
	pushl	%ebx
	
	movl	8(%ebp),%edi
	movl	12(%ebp),%eax
	movl	16(%ebp),%ecx

	orl	%ecx,%ecx
	jz	7f		# nothing


	testl	$2,%edi		# w or l aligned ?
	jz	5f		# l
	movw	%ax,(%edi)	# write 1st word to align
	decl	%ecx
	addl	$2,%edi
	
5:	
	movl	%ecx,%ebx
	shrl	$2,%ecx		# number of 8bytes
	jz	2f		# count < 4

	movl	%eax,%edx
	shll	$16,%eax
	movw	%dx,%ax		
	
	leal	-2(%edi,%ecx,8),%edx
	
	decl	%ecx
.align 16, 0x90
1:
	movl	%eax,(%edi,%ecx,8)
	movl	%eax,4(%edi,%ecx,8)
	decl	%ecx		
	jns	1b		
	
	movl	%edx,%edi

2:
	andl	$3,%ebx
	jz	7f

	
.align 16, 0x90
3:
	movw	%ax,(%edi,%ebx,2)
	decl	%ebx
	jnz	3b
7:

	popl	%ebx		# registres a sauvegarder (convention gcc)
	#popl	%esi
	popl	%edi

	leave
	ret
		


SOpti_cvrt_ABGR_8888_RGB_565:
SOpti_cvrt_bRGBA_8888_RGB_565:
SOpti_cvrt_mRGBA_8888_RGB_565:
SOpti_cvrt_lABGR_8888_RGB_565:	
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%esi
	pushl	%edi
	pushl	%ebx
	movl	8(%ebp),%esi       # load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx        # and number of pixels

		 
	subl	$2,%edi		#        will be re-added in the loop (for pairability)
	
.align 16, 0x90			#	align loop start for efficiency;fill with nops(90)
1:
	movl	(%esi),%eax	# 
	addl    $4,%esi		# 

	movb    %al,%dl		#       R
	andl    $0x00ffff00,%eax#        keep only BG
	
	shll    $8,%eax		#        ABGR => BGR0  ( BG00 in fact)
	andb    $0xf8,%dl       #        zeroe 3 low bits of R
	
	bswapl  %eax            #      Rearange BGR0 into 0RGB (00GB)

	
	shrb	$2,%ah          #       shift G (8bits ->6bits)
	addl	$2,%edi		#        next dest. pixel

	shrl	$3,%eax		#       shift B (8bits -> 5bits) and put GB on position

	orb     %dl,%ah		#        mix R with GB
	
	movw    %ax,(%edi)	
	nop
	
	decl	%ecx		
	jnz	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi	
	leave
	ret

SOpti_cvrt_ARGB_8888_RGB_565:	
SOpti_cvrt_bBGRA_8888_RGB_565:	
SOpti_cvrt_mBGRA_8888_RGB_565:	
SOpti_cvrt_lARGB_8888_RGB_565:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%esi
	pushl	%edi
	pushl	%ebx
	
	movl	8(%ebp),%esi       # load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx        # and number of pixels

		 
	subl	$2,%edi		#        will be re-added in the loop (for pairability)
	
.align 16, 0x90			#	align loop start for efficiency; fill with nops(90)
1:
	movl	(%esi),%eax	
	addl    $4,%esi		

	movl    %eax,%edx	#       
	andl    $0x0000ffff,%eax#        keep only GB
	
	shrl	$16,%edx	#       R into dl

	shrb	$2,%ah          #       shift G (8bits ->6bits)
	addl	$2,%edi		#        next dest. pixel

	shrl	$3,%eax		#       shift B (8bits -> 5bits) and put GB on position
	andb    $0xf8,%dl       #        zeroe 3 low bits of R
	
	orb     %dl,%ah		#        mix R with GB
	
	movw    %ax,(%edi)	
	nop
	
	decl	%ecx		
	jnz	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi
		
	leave
	ret

	



SOpti_cvrt_RGB_565_ARGB_8888:
SOpti_cvrt_RGB_565_bBGRA_8888:
SOpti_cvrt_RGB_565_mBGRA_8888:
SOpti_cvrt_RGB_565_lARGB_8888:	
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%esi
	pushl	%edi
	pushl	%ebx
		
	movl	8(%ebp),%esi        # load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx        # and number of pixels

.align 16, 0x90
1:
	movw	(%esi,%ecx,2),%ax	
	movl	%eax,%ebx	
	
	shll	$3,%eax		#     shift  B and ~G
	andl	$0xf800,%ebx	# 	isolate R
	
	shll	$8,%ebx		# 	set R in position
	andl	$0xffff,%eax	#      clear upper word for future R
	
	shlb	$2,%ah		# 	shift G
	
	orl	%eax,%ebx	# 	mix 0R GB

	movl	%ebx,(%edi,%ecx,4)	
	nop			
	
	
	decl	%ecx		
	jnz	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi
	leave
	ret





