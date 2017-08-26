	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_returnH
	.p2align	4, 0x90
_returnH:                               ## @returnH
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp2:
	.cfi_def_cfa_register %rbp
	movb	$72, %al
	movsbl	%al, %eax
	popq	%rbp
	retq
	.cfi_endproc

	.globl	_subtractintint
	.p2align	4, 0x90
_subtractintint:                        ## @subtractintint
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp3:
	.cfi_def_cfa_offset 16
Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp5:
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %esi
	subl	-8(%rbp), %esi
	movl	%esi, %eax
	popq	%rbp
	retq
	.cfi_endproc

	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp6:
	.cfi_def_cfa_offset 16
Ltmp7:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp8:
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movl	$80, %eax
	movl	%eax, %ecx
	movl	$0, -4(%rbp)
	movl	%edi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rcx, %rdi
	callq	_malloc
	movl	$12, %edx
	movl	%edx, %edi
	movq	%rax, -24(%rbp)
	callq	_malloc
	movl	$8, %edx
	movl	%edx, %edi
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movl	$1, (%rax)
	movq	-32(%rbp), %rax
	movl	$1, 4(%rax)
	movq	-32(%rbp), %rax
	movl	$1, 8(%rax)
	callq	_malloc
	movl	$1, %edi
	movl	$2, %esi
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movl	$1, (%rax)
	movq	-40(%rbp), %rax
	movl	$1, 4(%rax)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	%rax, (%rcx)
	callq	_subtractintint
	movl	$2, %edi
	movl	$1, %esi
	movl	%eax, -72(%rbp)         ## 4-byte Spill
	callq	_subtractintint
	movl	$2, %edi
	movl	$1, %esi
	movl	-72(%rbp), %edx         ## 4-byte Reload
	addl	%eax, %edx
	movl	%edx, -44(%rbp)
	callq	_subtractintint
	movl	$2, %edi
	addl	$1, %eax
	movl	%eax, %esi
	callq	_subtractintint
	movl	$1, %edi
	movl	$2, %esi
	movl	%eax, -48(%rbp)
	movl	-48(%rbp), %eax
	movl	%eax, -76(%rbp)         ## 4-byte Spill
	callq	_subtractintint
	movl	$2, %edi
	movl	$1, %esi
	movl	%eax, -80(%rbp)         ## 4-byte Spill
	callq	_subtractintint
	movl	$2, %edi
	movl	$1, %esi
	movl	-80(%rbp), %edx         ## 4-byte Reload
	addl	%eax, %edx
	movl	-76(%rbp), %eax         ## 4-byte Reload
	cmpl	%edx, %eax
	sete	%r8b
	andb	$1, %r8b
	movzbl	%r8b, %edx
	movl	%edx, -52(%rbp)
	callq	_subtractintint
	movl	%eax, -56(%rbp)
LBB2_1:                                 ## =>This Inner Loop Header: Depth=1
	movl	$2, %edi
	movl	$1, %esi
	movl	-56(%rbp), %eax
	movl	%eax, -84(%rbp)         ## 4-byte Spill
	callq	_subtractintint
	movl	-84(%rbp), %esi         ## 4-byte Reload
	cmpl	%eax, %esi
	jge	LBB2_6
## BB#2:                                ##   in Loop: Header=BB2_1 Depth=1
	movl	$2, %edi
	movl	$1, %esi
	callq	_subtractintint
	cmpl	$0, %eax
	je	LBB2_4
## BB#3:                                ##   in Loop: Header=BB2_1 Depth=1
	jmp	LBB2_4
LBB2_4:                                 ##   in Loop: Header=BB2_1 Depth=1
	jmp	LBB2_5
LBB2_5:                                 ##   in Loop: Header=BB2_1 Depth=1
	movl	$2, %edi
	movl	$1, %esi
	callq	_subtractintint
	addl	-56(%rbp), %eax
	movl	%eax, -56(%rbp)
	jmp	LBB2_1
LBB2_6:
	movl	$0, -64(%rbp)
LBB2_7:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$10, -64(%rbp)
	jge	LBB2_12
## BB#8:                                ##   in Loop: Header=BB2_7 Depth=1
	movl	$2, %edi
	movl	$1, %esi
	callq	_subtractintint
	cmpl	$0, %eax
	je	LBB2_10
## BB#9:                                ##   in Loop: Header=BB2_7 Depth=1
	jmp	LBB2_10
LBB2_10:                                ##   in Loop: Header=BB2_7 Depth=1
	jmp	LBB2_11
LBB2_11:                                ##   in Loop: Header=BB2_7 Depth=1
	movl	-64(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -64(%rbp)
	jmp	LBB2_7
LBB2_12:
	movq	-24(%rbp), %rax
	movq	(%rax), %rdi
	callq	_printBoxBox1
	movq	-40(%rbp), %rdi
	callq	_printRectRect1
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	callq	_free
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	callq	_free
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	callq	_free
	xorl	%eax, %eax
	addq	$96, %rsp
	popq	%rbp
	retq
	.cfi_endproc


.subsections_via_symbols
