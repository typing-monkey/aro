import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

fig = plt.figure()
chunk_size = 128


f = open("test2.dat","r")
intensity = np.fromfile(f,dtype=np.int32)
a = [0]*1024

count = 0
x = [i for i in range(1024)]

time = 0

while True:
    for freq in range(1024):
        a[freq] = intensity[freq*128+time]
      
    plt.clf()
    plt.title('kotekan spectrum polarizaiton 1')
    plt.xlim(0,1023)
    #plt.ylim(10000,25000)
    plt.xlabel('Frequency')
    plt.ylabel('Intensity')     
    plt.plot(x,a)
    plt.pause(0.05)
    time = (time + 1) % 128
    if time == 0:
        intensity = intensity[1024*128:]
