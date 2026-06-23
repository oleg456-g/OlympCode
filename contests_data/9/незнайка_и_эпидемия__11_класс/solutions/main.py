import sys
import math

# Константа, эквивалентная u32::MAX в Rust
U32_MAX = 0xFFFFFFFF

def next_power_of_two(n: int) -> int:
    """Имитация метода u32::next_power_of_two() в Rust"""
    if n <= 0:
        return 1
    return 1 << (n - 1).bit_length()

def trailing_zeros(n: int) -> int:
    """Имитация метода u32::trailing_zeros() в Rust"""
    if n == 0:
        return 32
    return (n & -n).bit_length() - 1


class State:
    def __init__(self, n: int, m: int, uplink: list, downlink: list):
        self.n = n  # количество вершин
        self.m = m  # размер ДНК
        self.uplink = uplink
        self.downlink = downlink
        self.first_entry = [U32_MAX] * n
        self.depths = []
        self.depth_nodes = []
        self.rmq_idx = []

    def make_depths(self):
        # stk содержит кортежи: (node, index, depth)
        stk = [(0, 0, 0)]
        while len(stk) > 0:
            l = stk[-1]
            node = l[0]
            index = l[1]
            depth = l[2]
            
            if self.first_entry[node] == U32_MAX:
                self.first_entry[node] = len(self.depths)
                
            self.depths.append(l[2])
            self.depth_nodes.append(l[0])
            
            if index >= len(self.downlink[node]):
                stk.pop()
            else:
                # В Rust l.1 += 1 изменяет индекс в текущем элементе стека
                stk[-1] = (l[0], l[1] + 1, l[2])
                stk.append((self.downlink[node][index][0], 0, depth + 1))

    def build_rmq(self):
        width = 1
        dlen = len(self.depths)
        self.rmq_idx.append(list(range(dlen)))
        
        while True:
            newwidth = width * 2
            if newwidth > dlen:
                break

            prev = self.rmq_idx[-1]
            cur = []
            for i in range(dlen):
                idx = prev[i]
                if i + width < dlen and self.depths[prev[i + width]] < self.depths[idx]:
                    idx = prev[i + width]
                cur.append(idx)
                
            self.rmq_idx.append(cur)
            width = newwidth

    def query_rmq(self, from_idx: int, to_idx: int) -> int:
        width = to_idx - from_idx + 1
        if width == 1:
            return self.rmq_idx[0][from_idx]
        elif width == 2:
            return self.rmq_idx[1][from_idx]
        else:
            p2 = next_power_of_two(width)
            if width == p2:
                idx = trailing_zeros(p2)
                return self.rmq_idx[idx][from_idx]
            else:
                idx = trailing_zeros(p2) - 1
                p2 >>= 1
                minidx = self.rmq_idx[idx][from_idx]
                otheridx = self.rmq_idx[idx][to_idx + 1 - p2]
                if self.depths[otheridx] < self.depths[minidx]:
                    minidx = otheridx
                return minidx

    def query_lca(self, node1: int, node2: int) -> int:
        left = min(self.first_entry[node1], self.first_entry[node2])
        right = max(self.first_entry[node1], self.first_entry[node2])
        idx = self.query_rmq(left, right)
        return self.depth_nodes[idx]

    def collect_mutations(self, from_node: int, to_node: int, out: list):
        while from_node != to_node:
            out.append(self.uplink[from_node][1])
            assert self.uplink[from_node][0] != from_node
            from_node = self.uplink[from_node][0]

    def calc_common(self, mutts: list) -> int:
        if len(mutts) == 0:
            return self.m
            
        # Сортировка и удаление дубликатов (аналог mutts.sort_unstable() и mutts.dedup())
        mutts.sort()
        # Механизм дедупликации на месте, как в Rust
        if len(mutts) > 0:
            j = 0
            for i in range(1, len(mutts)):
                if mutts[i] != mutts[j]:
                    j += 1
                    mutts[j] = mutts[i]
            del mutts[j + 1:]

        longest = mutts[0]
        for i in range(1, len(mutts)):
            longest = max(longest, mutts[i] - mutts[i - 1] - 1)
            
        return max(longest, self.m - mutts[-1] - 1)

    def find_common(self, n1: int, n2: int) -> int:
        if n1 == n2:
            return self.m
            
        lca = self.query_lca(n1, n2)
        mutts = []
        if lca == n1:
            self.collect_mutations(n2, lca, mutts)
        elif lca == n2:
            self.collect_mutations(n1, lca, mutts)
        else:
            self.collect_mutations(n1, lca, mutts)
            self.collect_mutations(n2, lca, mutts)
            
        return self.calc_common(mutts)


def main():
    # Настройка чтения из стандартного ввода
    lines = sys.stdin.read().splitlines()
    if not lines:
        return
        
    line_iter = iter(lines)
    
    try:
        first_line = next(line_iter)
    except StopIteration:
        return

    tokens = first_line.split()
    if len(tokens) < 3:
        return
        
    m = int(tokens[0])
    n = int(tokens[1])
    k = int(tokens[2])

    uplink = [(0, 0)] * n
    downlink = [[] for _ in range(n)]

    for _ in range(n - 1):
        line = next(line_iter)
        tokens = line.split()
        s = int(tokens[0])
        d = int(tokens[1])
        b = int(tokens[2])
        
        uplink[d - 1] = (s - 1, b)
        downlink[s - 1].append((d - 1, b))

    st = State(n, m, uplink, downlink)
    st.make_depths()
    st.build_rmq()

    for _ in range(k):
        line = next(line_iter)
        tokens = line.split()
        p = int(tokens[0])
        q = int(tokens[1])
        print(st.find_common(p - 1, q - 1))


if __name__ == '__main__':
    main()