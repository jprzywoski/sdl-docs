/***************************************
*        
* $RCSfile: opti_cvrt.S,v $ 
* ( $Revision: 1.1.1.1 $ $Date: 1999/10/08 08:52:30 $ )
* $Author: slouken $ / Optimum
*
* http://optimum.citeweb.net/index.html 
*                                       
****************************************/

/*
# *  conversions functions between graphic modes
# *
# *  (trifouilli extrait de l'optilib en version pre-beta)
# *
#
*/
	

	
.text

.global SOpti_cvrt_RGB_888_RGB_565
.global SOpti_cvrt_BGR_888_RGB_565



.global SOpti_cvrt_ABGR_8888_RGB_565
.global SOpti_cvrt_bRGBA_8888_RGB_565
.global SOpti_cvrt_mRGBA_8888_RGB_565
.global SOpti_cvrt_lABGR_8888_RGB_565

.global SOpti_cvrt_ARGB_8888_RGB_565
.global SOpti_cvrt_bBGRA_8888_RGB_565
.global SOpti_cvrt_mBGRA_8888_RGB_565
.global SOpti_cvrt_lARGB_8888_RGB_565
	
.global SOpti_cvrt_RGB_565_ARGB_8888
.global SOpti_cvrt_RGB_565_bBGRA_8888
.global SOpti_cvrt_RGB_565_mBGRA_8888
.global SOpti_cvrt_RGB_565_lARGB_8888	

.global SOpti_cvrt_PAL_8_RGB_565
.global SOpti_cvrt_PAL_8_RGBA_8888

	.type	 SOpti_cvrt_RGB_888_RGB_565,@function
	.type	 SOpti_cvrt_BGR_888_RGB_565,@function
	
	.type	 SOpti_cvrt_ABGR_8888_RGB_565,@function
	.type	 SOpti_cvrt_bRGBA_8888_RGB_565,@function
	.type	 SOpti_cvrt_mRGBA_8888_RGB_565,@function
	.type	 SOpti_cvrt_lABGR_8888_RGB_565,@function
	
	.type	 SOpti_cvrt_ARGB_8888_RGB_565,@function
	.type	 SOpti_cvrt_bBGRA_8888_RGB_565,@function
	.type	 SOpti_cvrt_mBGRA_8888_RGB_565,@function
	.type	 SOpti_cvrt_lARGB_8888_RGB_565,@function
	
	.type	 SOpti_cvrt_RGB_565_ARGB_8888,@function
	.type	 SOpti_cvrt_RGB_565_bBGRA_8888,@function
	.type	 SOpti_cvrt_RGB_565_mBGRA_8888,@function
	.type	 SOpti_cvrt_RGB_565_lARGB_8888,@function
	
	.type	 SOpti_cvrt_PAL_8_RGB_565,@function
	.type	 SOpti_cvrt_PAL_8_RGBA_888,@function

SOpti_cvrt_RGB_888_RGB_565:	

	pushl	%ebp
	movl	%esp,%ebp

	pushl	%esi
	pushl	%edi
	pushl	%ebx
	
	movl	8(%ebp),%esi       // load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx        // and number of pixels

		 
	subl	$2,%edi		//        will be re-added in the loop (for pairability)
	
.align 16, 0x90			//	align loop start for efficiency; fill with nops(90)
1:
	movb	(%esi),%ah
	movb	2(%esi),%bl
	
	movb	1(%esi),%al
	shrb	$3,%bl
	
	shrb    $3,%ah
	addl    $3,%esi

	andb	$0xfc,%al
	addl	$2,%edi
	
	shll	$3,%eax
	
	orb	%bl,%al

	movw    %ax,(%edi)	
	nop
	
	decl	%ecx		
	jnz	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi
		
	leave
	ret

SOpti_cvrt_BGR_888_RGB_565:	

	pushl	%ebp
	movl	%esp,%ebp

	pushl	%esi
	pushl	%edi
	pushl	%ebx
	
	movl	8(%ebp),%esi       // load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx        // and number of pixels

		 
	subl	$2,%edi		//        will be re-added in the loop (for pairability)
	
.align 16, 0x90			//	align loop start for efficiency; fill with nops(90)
1:
	movb	2(%esi),%ah
	movb	(%esi),%bl
	
	movb	1(%esi),%al
	shrb	$3,%bl
	
	shrb    $3,%ah
	addl    $3,%esi

	andb	$0xfc,%al
	addl	$2,%edi
	
	shll	$3,%eax
	
	orb	%bl,%al

	movw    %ax,(%edi)	
	nop
	
	decl	%ecx		
	jnz	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi
		
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
	movl	8(%ebp),%esi       //#  load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx       //# and number of pixels

		 
	subl	$2,%edi		//        will be re-added in the loop (for pairability)
	
.align 16, 0x90			//	align loop start for efficiency;fill with nops(90)
1:
	movl	(%esi),%eax	# 
	addl    $4,%esi		# 

	movb    %al,%dl		//       R
	andl    $0x00ffff00,%eax//        keep only BG
	
	shll    $8,%eax		//        ABGR => BGR0  ( BG00 in fact)
	andb    $0xf8,%dl       //        zeroe 3 low bits of R
	
	bswapl  %eax            //      Rearange BGR0 into 0RGB (00GB)

	
	shrb	$2,%ah          //       shift G (8bits ->6bits)
	addl	$2,%edi		//        next dest. pixel

	shrl	$3,%eax		//       shift B (8bits -> 5bits) and put GB on position

	orb     %dl,%ah		//        mix R with GB
	
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
	
	movl	8(%ebp),%esi       // load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx        // and number of pixels

		 
	subl	$2,%edi		//        will be re-added in the loop (for pairability)
	
.align 16, 0x90			//	align loop start for efficiency; fill with nops(90)
1:
	movl	(%esi),%eax	
	addl    $4,%esi		

	movl    %eax,%edx	//       
	andl    $0x0000ffff,%eax//        keep only GB
	
	shrl	$16,%edx	//       R into dl

	shrb	$2,%ah          //       shift G (8bits ->6bits)
	addl	$2,%edi		//        next dest. pixel

	shrl	$3,%eax		//       shift B (8bits -> 5bits) and put GB on position
	andb    $0xf8,%dl       //        zeroe 3 low bits of R
	
	orb     %dl,%ah		//        mix R with GB
	
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
		
	movl	8(%ebp),%esi        // load src and dst ptr,
	movl   12(%ebp),%edi 
	movl   16(%ebp),%ecx        // and number of pixels

.align 16, 0x90
1:
	movw	(%esi,%ecx,2),%ax	
	movl	%eax,%ebx	
	
	shll	$3,%eax		//     shift  B and ~G
	andl	$0xf800,%ebx	// 	isolate R
	
	shll	$8,%ebx		// 	set R in position
	andl	$0xffff,%eax	//      clear upper word for future R
	
	shlb	$2,%ah		// 	shift G
	
	orl	%eax,%ebx	// 	mix 0R GB

	movl	%ebx,(%edi,%ecx,4)	
	nop			
	
	
	decl	%ecx		
	jnz	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi
	leave
	ret




SOpti_cvrt_PAL_8_RGB_565:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%esi
	pushl	%edi
	pushl	%ebx
		
	movl	8(%ebp),%esi        # load src and dst ptr,
	movl	12(%ebp),%edi 
	movl	16(%ebp),%ecx	    # number of pixels
	movl	20(%ebp),%ebx	    # palette
	
	addl	%ecx,%esi		# partir de la fin pour partir du debut (hem)
	movl	%ecx,%eax
	shll	$1,%eax
	addl	%eax,%edi
	shrl	$2,%ecx			# 4 pixels par tour
	negl	%ecx
.align 16,0x90
1:	
	movl	(%esi,%ecx,4),%edx
	xorl	%eax,%eax
	
	pushl	%esi
	movb	%dh,%al
	
	movw	(%ebx,%eax,2),%si
	movb	%dl,%al

	shll	$16,%esi
	andl	$0xffff0000,%edx

	movw	(%ebx,%eax,2),%si
	nop

	shrl	$16,%edx
	movl	%esi,(%edi,%ecx,8)	###

	movb	%dh,%al
	andl	$0xff,%edx

	movw	(%ebx,%eax,2),%ax
	popl	%esi

	shll	$16,%eax
	nop
	
	movw	(%ebx,%edx,2),%ax
	nop

	movl	%eax,4(%edi,%ecx,8)
	nop
	
	incl	%ecx		
	js	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi
	leave
	ret



SOpti_cvrt_PAL_8_RGBA_8888:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%esi
	pushl	%edi
	pushl	%ebx
		
	movl	8(%ebp),%esi        # load src and dst ptr,
	movl	12(%ebp),%edi 
	movl	16(%ebp),%ecx	    # number of pixels
	movl	20(%ebp),%ebx	    # palette
	
	addl	%ecx,%esi		# partir de la fin pour partir du debut (hem)
	movl	%ecx,%eax
	shll	$2,%eax
	addl	%eax,%edi
	negl	%ecx
	xorl	%edx,%edx
	
.align 16,0x90
1:	
	movb	(%esi,%ecx),%dl
	nop
	
	movl	(%ebx,%edx,4),%eax
	incl	%ecx		

	movl	%eax,(%edi,%ecx,4)
	js	1b		
	
	popl	%ebx
	popl	%edi
	popl	%esi
	leave
	ret
