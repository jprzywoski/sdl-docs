/*
	a memaddshadedset function
      (or something like that )
*/

.text


/* memshset (char *dst, int center_shade,int fixed_shade, int length_2)
 dst is the *center* of the dst segment
 center shade is the initial color (at center) (!! 8:8 also)
 fixed_shade is the fixed point increment (8:8) 
 length_2 is the 1/2 length to set (to the left, and to the right)
*/

	
.globl memshset
.type   memshset,@function


memshset:
	pushl	%ebp
	movl	%esp,%ebp
	
	pushl	%edi		/* registres a sauvegarder (convention gcc) */
	pushl	%esi
	pushl	%ebx

	movl	8(%ebp),%edi      /* dst */
	movl	12(%ebp),%eax
	movl	16(%ebp),%ebx	  /* fixed point */
	movl	20(%ebp),%ecx	  /* half length */

	orl	%ecx,%ecx
	jz	2f

	movl	%edi,%esi
	subl	%ecx,%edi	/* %edi  left segment */
	addl	%ecx,%esi       /* %esi  right segment */
	incl	%esi		/* dont overlap at middle */
	
	movl	%ecx,%edx
	negl	%edx
	decl	%edx  /* for in-advance incl in loop */
1:
	addl	%ebx,%eax      /* increment color */
	incl	%edx
	
	addb	%ah,(%edi,%ecx) /* increment  left pixel */
	jc	3f              /* did overflow ? */
	
	addb	%ah,(%esi,%edx) /* increment right pixel */
	jc	4f

	decl	%ecx
	jnz	1b

2:	
	popl	%ebx
	popl	%esi
	popl	%edi

	leave
	ret
/* function goes on (several exit points): */
3:
	movb	$255,(%edi,%ecx)
	addb	%ah,(%esi,%edx)        /* we recopy all the code.. to avoid jmps   (hmm some asm guru here :	 is it really efficient ?.. */
	jc	4f
	decl	%ecx
	jnz	1b
	
	popl	%ebx
	popl	%esi
	popl	%edi

	leave
	ret

4:
	movb	$255,(%esi,%edx)
	decl	%ecx
	jnz	1b
	
	popl	%ebx
	popl	%esi
	popl	%edi

	leave
	ret
/* function really stops here  */
