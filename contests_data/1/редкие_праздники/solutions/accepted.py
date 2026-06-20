import sys

# ------------------------------------------------------------
#  Високосный календарь – решение «по блокам по 400 лет»
# ------------------------------------------------------------

def is_leap(year: int) -> bool:
    """Возвращает True, если year – високосный."""
    return year % 400 == 0 or (year % 4 == 0 and year % 100 != 0)


def solve() -> None:
    data = sys.stdin.read().strip().split()
    if not data:
        return
    L, R = map(int, data)

    # 1) Считаем количество високосных лет в одном полном 400‑летнем блоке
    leaps_in_block = sum(1 for y in range(400) if is_leap(y))   # всегда 97

    # Если отрезок короче 400 лет, просто перебираем всё
    if R - L + 1 < 400:
        ans = sum(1 for y in range(L, R + 1) if is_leap(y))
        print(ans)
        return

    # 2) Границы первого и последнего полного блока
    first_full_start = ((L + 399) // 400) * 400          # ceil(L/400) * 400
    last_full_start  = ((R - 399) // 400) * 400          # floor((R-399)/400) * 400

    ans = 0

    # 3) Префикс – годы до начала первого полного блока
    for y in range(L, min(R + 1, first_full_start)):
        if is_leap(y):
            ans += 1

    # 4) Полные 400‑летние блоки
    if first_full_start <= last_full_start:
        blocks = (last_full_start - first_full_start) // 400 + 1
        ans += blocks * leaps_in_block

    # 5) Суффикс – годы после последнего полного блока
    suffix_begin = max(L, last_full_start + 400)   # первый год после последнего полного блока
    for y in range(suffix_begin, R + 1):
        if is_leap(y):
            ans += 1

    print(ans)


if __name__ == "__main__":
    solve()
