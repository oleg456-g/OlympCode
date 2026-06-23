import sys

PATH_LENGTH = 262

# Имитация структуры record из Pascal
class Path:
    def __init__(self):
        self.length = 0
        # Заполняем массив нулями, индексация от 1 до PATH_LENGTH
        self.numbers = [0] * (PATH_LENGTH + 1)

def F(X):
    if (X % 2) == 0:
        return X // 2
    else:
        return 3 * X + 1

def APPEND(V, P):
    P.length += 1
    P.numbers[P.length] = V

def PATHTO1(N, P):
    P.length = 0
    while N > 1:
        APPEND(N, P)
        N = F(N)
    APPEND(1, P)

def main():
    FLAG1 = False
    FLAG2 = False
    P1 = Path()
    P2 = Path()

    # Считываем N1 и N2 из стандартного ввода
    input_data = sys.stdin.read().split()
    if not input_data:
        return
    
    N1 = int(input_data[0])
    N2 = int(input_data[1])

    if N1 == N2:
        print(0, end="")
    else:
        if N1 > 1:
            I = F(N1)
        else:
            I = N1
            
        if N2 > 1:
            J = F(N2)
        else:
            J = N2

        if (I == N2) or (J == N1) or ((N1 == 4) and (N2 == 1)) or ((N1 == 1) and (N2 == 4)):
            print(1, end="")
        else:
            PATHTO1(N1, P1)
            PATHTO1(N2, P2)
            
            I = P1.length
            J = P2.length
            
            while (I > 0) and (J > 0) and (P1.numbers[I] == P2.numbers[J]):
                I -= 1
                J -= 1
                
            if (I >= 0) and (I < P1.length):
                V = P1.numbers[I + 1]
            elif (J >= 0) and (J < P2.length):
                V = P2.numbers[J + 1]
            else:
                V = 1
                
            RESULT = I + J
            
            if ((N2 == 1) and (N1 > 2)) or ((N1 == 1) and (N2 > 2)):
                RESULT -= 1
                FLAG2 = True
                
            print(RESULT) # writeln в Pascal переходит на новую строку
            
            if I > 1:
                print(P1.numbers[2], end="")
                for K in range(3, I + 1): # в Pascal "to I" включительно
                    if not (FLAG2 and (P1.numbers[K] == 2)):
                        print(f" {P1.numbers[K]}", end="")
                FLAG1 = True
                
            if (V > 1) and (V != N1) and (V != N2):
                if not (FLAG2 and (V == 2)):
                    if FLAG1:
                        print(" ", end="")
                    print(V, end="")
                    FLAG1 = True
                    
            if J > 1:
                if FLAG1:
                    if not (FLAG2 and (P2.numbers[J] == 2)):
                        print(" ", end="")
                if not (FLAG2 and (P2.numbers[J] == 2)):
                    print(P2.numbers[J], end="")
                
                # в Pascal "downto 2" включительно
                for K in range(J - 1, 1, -1):
                    if not (FLAG2 and (P2.numbers[K] <= 4)):
                        print(" ", end="")
                    if not (FLAG2 and (P2.numbers[K] == 2)):
                        print(P2.numbers[K], end="")

if __name__ == "__main__":
    main()