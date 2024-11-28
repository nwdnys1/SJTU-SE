def f(n):
    if n == 1:
        return 1
    else:
         #print n
         return n*f(n-1)
    
print(f(100))