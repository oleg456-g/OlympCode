import sys
input = sys.stdin.readline

def parse_number(s):
    """Разбирает число в массив 50 разрядов (старший первый)."""
    a = [0] * 50
    j = 0  # количество разрядов
    i = 0
    while i < len(s):
        ch = s[i]
        if 'A' <= ch <= 'Z':
            val = ord(ch) - ord('A')
        elif 'a' <= ch <= 'z':
            val = ord(ch) - ord('a') + 26
        else:
            i += 1
            continue
        j += 1
        a[j - 1] = val  # временно в прямом порядке
        i += 1
        # Проверяем модификатор
        if i < len(s):
            if s[i] == '^':
                a[j - 1] += 52
                i += 1
            elif s[i] == '~':
                a[j - 1] += 104
                i += 1
            elif s[i] == '_':
                a[j - 1] += 156
                i += 1

    # Выравниваем: старший разряд в позицию 0
    b = [0] * 50
    for k in range(j):
        b[50 - j + k] = a[k]

    # Нормализация переносов (справа налево)
    for k in range(49, 0, -1):
        b[k - 1] += b[k] // 52
        b[k] %= 52

    return b

def main():
    n = int(input())
    numbers = []
    for _ in range(n):
        s = input().strip()
        numbers.append(parse_number(s))

    # Ищем нарушения порядка убывания
    # По условию ровно одна пара для обмена → либо одна соседняя пара,
    # либо две соседних нарушения (тогда ответ i и j+1)
    num1 = 0  # первое нарушение: A[num1] < A[num1+1]
    num2 = 0  # второе нарушение (если есть)

    for i in range(n - 1):
        if numbers[i] <= numbers[i + 1]:
            if num1 == 0:
                num1 = i + 1  # 1-индексация
            else:
                num2 = i + 1
                break  # больше двух не будет

    if num2 != 0:
        print(num1, num2 + 1)
    else:
        print(num1, num1 + 1)

main()
