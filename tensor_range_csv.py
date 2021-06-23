import pandas as pd
import csv

df = pd.read_csv('/home/osboxes/darknet-nnpack-dreml/output.csv')

max_col = df[' Tensor Value'].max()
min_col = df[' Tensor Value'].min()

# min in left column, max on right
fields=[min_col,max_col]
with open(r'tensor_output.csv', 'a') as f:
    writer = csv.writer(f)
    writer.writerow(fields)
