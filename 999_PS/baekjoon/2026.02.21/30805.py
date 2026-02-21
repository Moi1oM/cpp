n=int(input())
n_l=list(map(int,input().split()))
m=int(input())
m_l=list(map(int,input().split()))
result=[]
while n_l and m_l:
    common = set(n_l) & set(m_l)
    if not common:
        break
    n_m = max(common)
    result.append(n_m)
    n_l = n_l[n_l.index(n_m) + 1:]
    m_l = m_l[m_l.index(n_m) + 1:]

if result:
    print(len(result))
    print(*(result))
else:
    print(0)