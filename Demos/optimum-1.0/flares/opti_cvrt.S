

/*
# *  conversions functions between graphic modes
# *
# *  (trifouilli extrait de l'optilib en version pre-beta)
# *
#
*/
	

	
.text




.global SOpti_cvrt_PAL_8_RGB_565

	
	.type	 SOpti_cvrt_PAL_8_RGB_565,@function



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
