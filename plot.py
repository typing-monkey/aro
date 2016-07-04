import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

fig = plt.figure()
chunk_size = 65536/256/2


f = open("test2.dat","r")
intensity = np.fromfile(f,dtype=np.int32)
a=[[0]*1024]*1024
freq = 0


count = 0
for i in range(8):
     for freq in range(1024):
          for j in range(128):
               a[freq][i*128+j] = intensity[i*1024*128+freq*128+j]
intensity = intensity[1024*128*8:]
plt.imshow(a,cmap="Blues")
plt.xlabel('Time')
plt.ylabel('Frequency')     
plt.colorbar()
plt.pause(0.05)

while True:
     for freq in range(1024):
          a[freq] = a[freq][32:]
          a[freq].extend([0]*32)
     for freq in range(1024):     
          for i in range(32):
               a[freq][-1*(32-i)] = intensity[freq*128+i+count]
     
     plt.imshow(a,cmap="Blues") 
     plt.pause(0.05)
     count = (count + 32) % 128
     if count == 0:
          intensity = intensity[1024*128:]
        
    