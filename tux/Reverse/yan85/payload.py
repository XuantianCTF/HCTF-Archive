from pwn import *

p = process('./just_vm')  # 替换为你的目标程序路径

payload = bytes([43, 250, 107, 217]) # 或者 b"\x2b\xfa\x6b\xd9"

p.send(payload)

p.interactive() 
