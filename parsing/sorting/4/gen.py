#!/usr/bin/env python3

import os
import random

random.seed(777333)

MAX_ID = 100000
MAX_SCORE = 100000

def write_test(test_id, participants):
    path = f"tests/{test_id:03d}.in"
    with open(path, "w", encoding="utf-8") as f:
        f.write(str(len(participants)) + "\n")
        for pid, score in participants:
            f.write(f"{pid} {score}\n")

def gen():
    os.makedirs("tests", exist_ok=True)

    tid = 1

    # --------------------------------------------------
    # 1-2. Примеры из условия
    # --------------------------------------------------

    write_test(tid, [
        (101, 80),
        (305, 90),
        (200, 14)
    ])
    tid += 1

    write_test(tid, [
        (20, 80),
        (30, 90),
        (25, 90)
    ])
    tid += 1

    # --------------------------------------------------
    # 3-20. Маленькие и граничные
    # --------------------------------------------------

    tests = [
        [(1, 1)],
        [(1, 1), (100000, 100000)],
        [(1, 100000), (100000, 1)],
        [(1, 50), (2, 50)],
        [(2, 50), (1, 50)],
        [(1, 1), (2, 1), (3, 1)],
        [(3, 1), (2, 1), (1, 1)],
        [(1, 10), (2, 20), (3, 30), (4, 40)],
        [(4, 40), (3, 30), (2, 20), (1, 10)],
        [(1, 10), (2, 20), (3, 10), (4, 20)],
        [(100000, 100000), (99999, 100000), (1, 100000)],
        [(1, 100000), (2, 100000), (3, 100000), (4, 100000)],
        [(4, 100000), (3, 100000), (2, 100000), (1, 100000)],
        [(1, 1), (2, 2), (3, 1), (4, 2), (5, 1)],
        [(5, 1), (4, 2), (3, 1), (2, 2), (1, 1)],
        [(1, 777)],
        [(100000, 1)],
        [(50000, 50000)]
    ]

    for t in tests:
        write_test(tid, t)
        tid += 1

    # --------------------------------------------------
    # 21-50
    # UB-тесты: все баллы одинаковые
    # --------------------------------------------------

    for n in range(64, 94):  # 30 тестов
        participants = [(i, 777) for i in range(n, 0, -1)]
        write_test(tid, participants)
        tid += 1

    # --------------------------------------------------
    # 51-80
    # Средние тесты, все баллы одинаковые
    # --------------------------------------------------

    for _ in range(30):
        n = random.randint(100, 1000)

        ids = random.sample(range(1, MAX_ID + 1), n)

        participants = [(x, 50000) for x in ids]

        write_test(tid, participants)
        tid += 1

    # --------------------------------------------------
    # 81-100
    # Средние случайные
    # --------------------------------------------------

    for _ in range(20):
        n = random.randint(1000, 5000)

        ids = random.sample(range(1, MAX_ID + 1), n)

        participants = []

        for x in ids:
            participants.append(
                (x, random.randint(1, MAX_SCORE))
            )

        write_test(tid, participants)
        tid += 1

    # --------------------------------------------------
    # 101-105
    # Максимальные все баллы одинаковые
    # --------------------------------------------------

    for _ in range(5):
        participants = [(i, 77777) for i in range(MAX_ID, 0, -1)]

        write_test(tid, participants)
        tid += 1

    # --------------------------------------------------
    # 106-110
    # Максимальные случайные id
    # одинаковый score
    # --------------------------------------------------

    for _ in range(5):
        ids = list(range(1, MAX_ID + 1))
        random.shuffle(ids)

        participants = [(x, 12345) for x in ids]

        write_test(tid, participants)
        tid += 1

    # --------------------------------------------------
    # 111-115
    # Максимальные случайные
    # --------------------------------------------------

    for _ in range(5):
        ids = list(range(1, MAX_ID + 1))
        random.shuffle(ids)

        participants = []

        for x in ids:
            participants.append(
                (x, random.randint(1, MAX_SCORE))
            )

        write_test(tid, participants)
        tid += 1

    # --------------------------------------------------
    # 116-120
    # Несколько больших групп равных баллов
    # --------------------------------------------------

    for _ in range(5):
        ids = list(range(1, MAX_ID + 1))
        random.shuffle(ids)

        scores_pool = [1, 10, 100, 1000, 10000]

        participants = []

        for x in ids:
            participants.append(
                (x, random.choice(scores_pool))
            )

        write_test(tid, participants)
        tid += 1

    assert tid == 121
    print("Generated", tid - 1, "tests")

if __name__ == "__main__":
    gen()