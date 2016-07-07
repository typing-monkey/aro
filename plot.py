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
#for i in range(8):
     #for freq in range(1024):
          #for j in range(128):
               #a[freq][i*128+j] = intensity[freq*128+j]
     #intensity = intensity[1024*128:]
                
plt.imshow(a,cmap="Blues")
plt.xlabel('Time')
plt.ylabel('Frequency')
plt.colorbar()
plt.clim(700,8000)
plt.savefig('1.png')
#plt.pause(0.05)

index = 1
while True:
     for freq in range(1024):
          a[freq] = a[freq][64:]
          a[freq].extend([0]*64)
     for freq in range(1024):     
          for i in range(64):
               a[freq][-1*(64-i)] = intensity[freq*128+i+count]
     index += 1
     plt.imshow(a,cmap="Blues",vmin = 700, vmax = 8000,origin='lower')
     plt.xlim()
     #plt.clim(700,8000)
     plt.savefig(str(index)+'.png')
     #plt.colorbar()
     plt.pause(0.05)
     count = (count + 64) % 128
     if count == 0:
          intensity = intensity[1024*128:]
        
    