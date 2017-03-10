import numpy as np
import matplotlib.pyplot as plt
import imageio #Para hacer el gif
import os #Para crear directorio temporal
import shutil #Para eliminar directorio temporal

inp=np.genfromtxt('posiciones.txt', delimiter=' ')
eners=np.genfromtxt('energias.txt', delimiter=' ')
z = np.genfromtxt('tiempo.txt', dtype='string',usecols=0)
times = [float(z[0].split('u')[0]), float(z[2].split('u')[0]), float(z[4].split('u')[0])]

fig=plt.figure()
ax=plt.axes()
plt.grid()
plt.xlabel('Procesadores')
plt.ylabel('Tiempo (s)')
plt.title('Tiempo de usuario en funcion de numero de procesadores')
plt.xlim((0,5))
plt.plot([1,2,4], times, 'ro', ms=6)
plt.savefig('tiempo.pdf', format='pdf')
plt.close()

######  IMSHOW  #####
fig=plt.figure()
ax=plt.axes()
plt.grid()
plt.ylabel(r'Tiempo ($t$)')
plt.xlabel('Oscilador')
plt.imshow(inp, extent=[0,64,0,len(inp[:,0])], aspect='auto')
plt.colorbar()
plt.savefig('oscilador.pdf', format='pdf')
plt.close()

######  Energias  #####
fig=plt.figure()
ax=plt.axes()
plt.grid()
plt.xlabel('Tiempo')
plt.ylabel('Energias')
plt.xlim((0, eners[-1,-1]))
plt.title(r'Energias para modos fundamentales ($E_k$)')
for i in range(len(eners[0,:])-1):
    plt.plot(eners[:,-1], eners[:,i], label=r'$k=$'+str(i+1))
plt.legend(framealpha=0.5)
plt.savefig('energias.pdf', format='pdf')
plt.close()

######  GIF  #####
os.mkdir('temp')
with imageio.get_writer('./movimiento.gif', mode='I') as writer:
    for i in range(len(inp[:,0])):
        if (i%20==0):
            fig=plt.figure()
            plt.grid()
            plt.plot(inp[i,:], 'bo')
            plt.xlim((0,63))
            plt.ylim((-1,1))
            plt.savefig('./temp/'+str(i)+'.png', format='png')
            image=imageio.imread('./temp/'+str(i)+'.png')
            writer.append_data(image)
shutil.rmtree('temp')
