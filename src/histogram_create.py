import pandas as pd
import matplotlib.pyplot as plt
df = pd.read_csv('/home/osboxes/darknet-nnpack-dreml/output.csv')
df.head(1)

df[' Tensor Value'].plot(kind='hist')
plt.title('Tensor Values for Figure #25000')
plt.xlabel('Tensor Value')
plt.show()
#print(df[' Tensor Value'])
