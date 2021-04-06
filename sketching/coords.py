


start = [18, 17]

count = 2
pos = start


def alg1():
    start = [18, 17]

    count = 99
    pos = start
    for x in range(30):
        count += 1
        if count > 2: count = 0

        if count == 0:
            pos[1] += 1
        if count == 1:
            pos[0] += 1
        if count == 2:
            pos[0] += 1

        print(pos)
        if pos == [31,24]: break

def alg2():
    start = [20, 19]
    count = 0
    pos = start

    for x in range(10):
        count+=1
        if count == 0:
            pos[1] += 1
        elif count == 1 or count == 2:
            pos[0] += 1
        else:
            count = 0
            pos[1] += 1
        print(pos)

alg2()
    