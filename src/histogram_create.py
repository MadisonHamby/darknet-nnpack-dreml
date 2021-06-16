import pandas as pd
import matplotlib.pyplot as plt
df = pd.read_csv('/home/osboxes/darknet-nnpack-dreml/output.csv')
df.head(1)

df[' Tensor Value'].plot(kind='hist')
max_col = df[' Tensor Value'].max()
min_col = df[' Tensor Value'].min()
plt.title('Tensor Values for Figure #25999 - Fixed-Point')
plt.xlabel('Tensor Value')
print("Minimum value is: ", min_col)
print("Maximum value is:", max_col)
#plt.xlim((-10, 10))
plt.show()
#print(df[' Tensor Value'])
