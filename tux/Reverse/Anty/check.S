.intel_syntax noprefix

.global check_flag_asm
.text

/* 
 * bool check_flag_asm(const char* input);
 */
check_flag_asm:
    .cfi_startproc
    push    rbp
    mov     rbp, rsp
    push    r12
    push    rbx

    sub     rsp, 0x100

    .byte   0xEB, 0x01
    .byte   0xB8

    // Start code
    mov     r12, [rdi]

    /*
     * 结合进位标志 (CF) 与位运算校验
    */
    mov     rax, 0x1122334455667788
    xor     r12, rax

    mov     rbx, 0x5555555555555555
    add     r12, rbx        /* 触发标志位变化 */
    pushfq                  /* 保存 EFLAGS 到栈顶 */
    pop     rax             /* 提取 EFLAGS 到 rax */
    and     rax, 1          /* 提取 CF (Carry Flag) 标志位 */

    /* 验证算术结果 */
    mov     rax, 0xBAA15C9468878A15
    cmp     r12, rax
    jne     .Lfail

    /* ======================================================================
     * 技巧 4: 栈平衡对抗修正
     * ====================================================================== */
    add     rsp, 0x100
    pop     rbx
    pop     r12
    pop     rbp
    mov     rax, 1          /* 返回 true (1) */
    ret

.Lfail:
    add     rsp, 0x100
    pop     rbx
    pop     r12
    pop     rbp
    xor     rax, rax        /* 返回 false (0) */
    ret

    .cfi_endproc
