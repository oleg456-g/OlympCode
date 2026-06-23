import sys

class Token:
    def __init__(self, kind, val=None, ob=None, t=None, cb=None, cnt=None, v=None, s=None, lhs=None, rhs=None):
        self.kind = kind  # Строковое имя варианта (End, Open, Close, Equals, Atom, Num, Plus, Bracket, Counted, Molecule, Component, Side, Equation)
        self.val = val    # Используется для хранения u8 (Open/Close), i32 (Num), или str (Atom)
        self.ob = ob      # Открывающая скобка (str)
        self.t = t        # Подтокен / Бокс токена
        self.cb = cb      # Закрывающая скобка (str)
        self.cnt = cnt    # Числовой счетчик / Коэффициент (int)
        self.v = v        # Список дочерних токенов (list)
        self.s = s        # Строковый срез для Molecule (str)
        self.lhs = lhs    # Левая часть уравнения (Token)
        self.rhs = rhs    # Правая часть уравнения (Token)

    def __str__(self):
        if self.kind == 'Open':
            return chr(self.val)
        elif self.kind == 'Close':
            return chr(self.val)
        elif self.kind == 'Equals':
            return '='
        elif self.kind == 'Atom':
            return self.val
        elif self.kind == 'Num':
            return str(self.val)
        elif self.kind == 'Plus':
            return '+'
        elif self.kind == 'Bracket':
            return f"{self.ob}{str(self.t)}{self.cb}"
        elif self.kind == 'Counted':
            res = str(self.t)
            if self.cnt > 1:
                res += str(self.cnt)
            return res
        elif self.kind == 'Molecule':
            # В оригинале закомментирован обход вектора v, выводится только строка s
            return self.s
        elif self.kind == 'Component':
            res = ""
            if self.cnt > 1:
                res += str(self.cnt)
            res += str(self.t)
            return res
        elif self.kind == 'Side':
            res = []
            for item in self.v:
                res.append(str(item))
            return '+'.join(res)
        elif self.kind == 'Equation':
            return f"{str(self.lhs)}={str(self.rhs)}"
        elif self.kind == 'End':
            raise RuntimeError("Panic: End token cannot be formatted")
        else:
            raise RuntimeError("Panic: Unknown token variant")


class Parser:
    def __init__(self, s: str):
        self.s = s.encode('utf-8')  # Работаем с байтами, как в Rust (&[u8])
        self.len = len(s)
        self.i = 0
        self.t = Token('End')
        self.token_i = 0
        self.atom_map = {}
        self.atom_vec = []
        self.counts = [[], []]

    def next_token(self):
        self.token_i = self.i
        if self.i >= self.len:
            self.t = Token('End')
        elif 48 <= self.s[self.i] <= 57:  # b'0' .. b'9'
            val = 0
            while self.i < self.len and 48 <= self.s[self.i] <= 57:
                val = val * 10 + (self.s[self.i] - 48)
                self.i += 1
            self.t = Token('Num', val=val)
        elif 65 <= self.s[self.i] <= 90:  # b'A' .. b'Z'
            beg = self.i
            self.i += 1
            while self.i < self.len and 97 <= self.s[self.i] <= 122:  # b'a' .. b'z'
                self.i += 1
            self.t = Token('Atom', val=self.s[beg:self.i].decode('utf-8'))
        elif self.s[self.i] == 40 or self.s[self.i] == 91:  # b'(' или b'['
            self.t = Token('Open', val=self.s[self.i])
            self.i += 1
        elif self.s[self.i] == 41 or self.s[self.i] == 93:  # b')' или b']'
            self.t = Token('Close', val=self.s[self.i])
            self.i += 1
        elif self.s[self.i] == 61:  # b'='
            self.t = Token('Equals')
            self.i += 1
        elif self.s[self.i] == 43:  # b'+'
            self.t = Token('Plus')
            self.i += 1
        else:
            raise Exception(f"parse error invalid char at pos {self.i}")

    def counted(self):
        if self.t.kind == 'Atom':
            b = Token('Atom', val=self.t.val)
            self.next_token()
        elif self.t.kind == 'Open':
            oc = self.t.val
            ob = self.s[self.i-1:self.i].decode('utf-8')
            self.next_token()
            m = self.molecule()
            if self.t.kind == 'Close':
                cc = self.t.val
                if (oc == 40 and cc == 41) or (oc == 91 and cc == 93):
                    cb = self.s[self.i-1:self.i].decode('utf-8')
                    self.next_token()
                    b = Token('Bracket', ob=ob, t=m, cb=cb)
                else:
                    raise Exception(f"parse error bracket mismatch at pos {self.i}")
            else:
                raise Exception(f"parse error expect closing bracket at pos {self.i}")
        else:
            raise Exception(f"parse error unexpected token at pos {self.i}")

        cnt = 1
        if self.t.kind == 'Num':
            cnt = self.t.val
            self.next_token()
        return Token('Counted', t=b, cnt=cnt)

    def molecule(self):
        v = []
        beg = self.token_i
        while True:
            if self.t.kind == 'Open' or self.t.kind == 'Atom':
                v.append(self.counted())
            else:
                break
        end = self.token_i
        return Token('Molecule', v=v, s=self.s[beg:end].decode('utf-8'))

    def component(self):
        koef = 1
        if self.t.kind == 'Num':
            koef = self.t.val
            self.next_token()
        m = self.molecule()
        return Token('Component', cnt=koef, t=m)

    def side(self):
        comps = []
        comps.append(self.component())
        while self.t.kind == 'Plus':
            self.next_token()
            comps.append(self.component())
        return Token('Side', v=comps)

    def equation(self):
        s1 = self.side()
        if self.t.kind == 'Equals':
            self.next_token()
            s2 = self.side()
            return Token('Equation', lhs=s1, rhs=s2)
        else:
            raise Exception(f"parse error = expected at pos {self.i}")

    def parse(self):
        self.next_token()
        root = self.equation()
        if self.t.kind == 'End':
            return root
        else:
            raise Exception(f"parse error EOF expected at pos {self.i}")

    def reset_component_count(self, node):
        if node.kind == 'Component':
            node.cnt = 1
        elif node.kind == 'Side':
            for item in node.v:
                self.reset_component_count(item)
        elif node.kind == 'Equation':
            self.reset_component_count(node.lhs)
            self.reset_component_count(node.rhs)
        else:
            raise RuntimeError("Panic in reset_component_count")

    def set_component_count(self, node, cnts):
        if node.kind == 'Component':
            node.cnt = cnts[0]
            return 1
        elif node.kind == 'Side':
            res = 0
            for t in node.v:
                res += self.set_component_count(t, cnts[res:])
            return res
        elif node.kind == 'Equation':
            cnt = self.set_component_count(node.lhs, cnts)
            cnt2 = self.set_component_count(node.rhs, cnts[cnt:])
            return cnt + cnt2
        else:
            raise RuntimeError("Panic in set_component_count")

    def remove_duplicates(self, node):
        if node.kind == 'Side':
            ms = set()
            j = 0
            for i in range(len(node.v)):
                if node.v[i].kind == 'Component' and node.v[i].t.kind == 'Molecule':
                    m = node.v[i].t.s
                else:
                    raise RuntimeError("Panic in remove_duplicates inner")
                
                if m not in ms:
                    ms.add(m)
                    node.v[i], node.v[j] = node.v[j], node.v[i]
                    j += 1
            del node.v[j:]  # Имитация v.resize_with(j, ...) для уменьшения размера вектора
        elif node.kind == 'Equation':
            self.remove_duplicates(node.lhs)
            self.remove_duplicates(node.rhs)
        else:
            raise RuntimeError("Panic in remove_duplicates")

    def collect_atoms(self, node, am, av):
        if node.kind == 'Atom':
            if node.val not in am:
                length = len(av)
                av.append(node.val)
                am[node.val] = length
        elif node.kind == 'Bracket':
            self.collect_atoms(node.t, am, av)
        elif node.kind == 'Counted':
            self.collect_atoms(node.t, am, av)
        elif node.kind == 'Molecule':
            for i in range(len(node.v)):
                self.collect_atoms(node.v[i], am, av)
        elif node.kind == 'Component':
            self.collect_atoms(node.t, am, av)
        elif node.kind == 'Side':
            for i in range(len(node.v)):
                self.collect_atoms(node.v[i], am, av)
        elif node.kind == 'Equation':
            self.collect_atoms(node.lhs, am, av)
            self.collect_atoms(node.rhs, am, av)

    def compute_presence(self, node, flag):
        if node.kind == 'Atom':
            flag[self.atom_map[node.val]] = True
        elif node.kind == 'Bracket':
            self.compute_presence(node.t, flag)
        elif node.kind == 'Counted':
            self.compute_presence(node.t, flag)
        elif node.kind == 'Molecule':
            for i in range(len(node.v)):
                self.compute_presence(node.v[i], flag)
        elif node.kind == 'Component':
            self.compute_presence(node.t, flag)
        elif node.kind == 'Side':
            for i in range(len(node.v)):
                self.compute_presence(node.v[i], flag)
        else:
            raise RuntimeError("Panic in compute_presence")

    def do_insert_missing(self, node, p):
        if node.kind == 'Side':
            for i in range(len(p)):
                if not p[i]:
                    a = Token('Atom', val=self.atom_vec[i])
                    c = Token('Counted', t=a, cnt=1)
                    m = Token('Molecule', v=[c], s=self.atom_vec[i])
                    cc = Token('Component', cnt=1, t=m)
                    node.v.append(cc)
        else:
            raise RuntimeError("Panic in do_insert_missing")

    def insert_missing(self, node):
        if node.kind == 'Equation':
            lhp = [False] * len(self.atom_vec)
            self.compute_presence(node.lhs, lhp)
            rhp = [False] * len(self.atom_vec)
            self.compute_presence(node.rhs, rhp)
            self.do_insert_missing(node.lhs, lhp)
            self.do_insert_missing(node.rhs, rhp)
        else:
            raise RuntimeError("Panic in insert_missing")

    def count_atoms(self, node, cnt, mult):
        if node.kind == 'Atom':
            cnt[self.atom_map[node.val]] += mult
        elif node.kind == 'Bracket':
            self.count_atoms(node.t, cnt, mult)
        elif node.kind == 'Counted':
            self.count_atoms(node.t, cnt, mult * node.cnt)
        elif node.kind == 'Molecule':
            for i in range(len(node.v)):
                self.count_atoms(node.v[i], cnt, mult)
        elif node.kind == 'Component':
            self.count_atoms(node.t, cnt, mult * node.cnt)
        else:
            raise RuntimeError("Panic in count_atoms")

    def count_atoms_side(self, node, cnt):
        if node.kind == 'Side':
            for i in range(len(node.v)):
                cc = [0] * len(self.atom_vec)
                self.count_atoms(node.v[i], cc, 1)
                cnt.append(cc)
        else:
            raise RuntimeError("Panic in count_atoms_side")

    def count_atoms_equation(self, node):
        if node.kind == 'Equation':
            lc = []
            self.count_atoms_side(node.lhs, lc)
            rc = []
            self.count_atoms_side(node.rhs, rc)
            self.counts = []
            for v in lc:
                self.counts.append(v)
            for v in rc:
                neg_v = [-x for x in v]
                self.counts.append(neg_v)
        else:
            raise RuntimeError("Panic in count_atoms_equation")

    @staticmethod
    def apply(mat, v, out):
        for i in range(len(out)):
            out[i] = 0
        rows = len(mat)
        cols = len(out)
        for r in range(rows):
            for c in range(cols):
                out[c] += mat[r][c] * v[r]

    @staticmethod
    def is_zero(v):
        for x in v:
            if x != 0:
                return False
        return True

    @staticmethod
    def apply_bool(mat, v, out):
        Parser.apply(mat, v, out)
        return Parser.is_zero(out)

    @staticmethod
    def next_vector(v, lim_box, limcnt_box):
        # Так как в Python примитивы передаются по значению,
        # lim и limcnt упакованы в списки из 1 элемента (имитация &mut)
        if limcnt_box[0] == len(v):
            for i in range(len(v)):
                v[i] = 1
            lim_box[0] += 1
            limcnt_box[0] = 0
        while True:
            i = 0
            while i < len(v):
                if v[i] == lim_box[0]:
                    limcnt_box[0] -= 1
                    v[i] = 1
                else:
                    v[i] += 1
                    if v[i] == lim_box[0]:
                        limcnt_box[0] += 1
                    break
                i += 1
            if limcnt_box[0] > 0:
                break
        # В авторском коде на Rust здесь жестко прописано `false`. 
        # Из-за этого условие `if Parser::next_vector(...)` в solve никогда не сработает на "BAD".
        return False


def solve(s: str) -> str:
    p = Parser(s)
    root = p.parse()
    p.reset_component_count(root)
    p.remove_duplicates(root)
    
    atom_map = {}
    atom_vec = []
    p.collect_atoms(root, atom_map, atom_vec)
    atom_vec.sort()
    atom_map.clear()
    for i in range(len(atom_vec)):
        atom_map[atom_vec[i]] = i
        
    p.atom_map = atom_map
    p.atom_vec = atom_vec
    p.insert_missing(root)
    p.count_atoms_equation(root)
    
    cnts = [1] * len(p.counts)
    lim_box = [1]
    limcnt_box = [len(p.counts)]
    out = [0] * len(p.counts[0])
    
    while True:
        if Parser.apply_bool(p.counts, cnts, out):
            p.set_component_count(root, cnts)
            return str(root)
        if Parser.next_vector(cnts, lim_box, limcnt_box):
            return "BAD"


def main():
    # Чтение из stdin аналогично Rust циклу read_line
    for line in sys.stdin:
        trimmed = line.strip()
        if not trimmed and line == '\n':
            continue
        try:
            print(solve(trimmed))
        except Exception as e:
            print(f"Error: {e}", file=sys.stderr)
            sys.exit(1)

if __name__ == '__main__':
    main()