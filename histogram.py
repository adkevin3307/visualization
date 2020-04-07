import matplotlib.pyplot as plt

with open('./Data/Scalar/histogram.txt', 'r') as f:
    for line in f:
        values = list(map(lambda x: int(x), line.split()))

_ = plt.hist(values, bins = 'auto')
plt.show()