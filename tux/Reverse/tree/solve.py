PRE = [0x49, 0x40, 0x43, 0x34, 0x62, 0x7E, 0x33, 0x32, 0x57, 0x71, 0x60, 0x3E, 0x77, 0x70, 0x7A]
IN = [0x34, 0x43, 0x62, 0x40, 0x33, 0x7E, 0x32, 0x49, 0x60, 0x71, 0x3E, 0x57, 0x70, 0x77, 0x7A]


class Node:
    def __init__(self, val):
        self.val = val
        self.dec = None
        self.left = None
        self.right = None


def rebuild(pre, ino):
    if not pre:
        return None
    root = Node(pre[0])
    i = ino.index(pre[0])
    root.left = rebuild(pre[1:1 + i], ino[:i])
    root.right = rebuild(pre[1 + i:], ino[i + 1:])
    return root


def decrypt(root, depth):
    if root is None:
        return
    root.dec = root.val ^ (depth * 2 + 1)
    decrypt(root.left, depth + 1)
    decrypt(root.right, depth + 1)


def level_order(root):
    res = []
    q = [root]
    while q:
        n = q.pop(0)
        res.append(n.dec)
        if n.left:
            q.append(n.left)
        if n.right:
            q.append(n.right)
    return bytes(res).decode()


tree = rebuild(list(PRE), list(IN))
decrypt(tree, 0)
print(level_order(tree))
