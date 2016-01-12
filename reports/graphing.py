#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt

N = 6
menMeans = (20, 35, 30, 35, 27, 28)
menStd =   (2, 3, 4, 1, 2, 5)

ind = np.arange(N)  # the x locations for the groups
width = 0.35       # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(ind, menMeans, width, color='r', yerr=menStd)

womenMeans = (25, 32, 34, 20, 25, 65)
womenStd =   (3, 5, 2, 3, 3, 6)
rects2 = ax.bar(ind+width, womenMeans, width, color='y', yerr=womenStd)

# add some
ax.set_ylabel('Scores')
ax.set_title('Scores by group and gender')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('G1', 'G2', 'G3', 'G4', 'G5') )

ax.legend( (rects1[0], rects2[0]), ('Men', 'Women') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)

plt.show()


import csv

search_for = ['clEnqueueNDRangeKernel']

with open('negative.csv') as inf, open('out.csv','w') as outf:
    reader = csv.reader(inf)
    writer = csv.writer(outf, delimiter='\t', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    for row in reader:
        if row[0] in search_for:
            print('Found: {}'.format(row))
	    print row[1]
	    

