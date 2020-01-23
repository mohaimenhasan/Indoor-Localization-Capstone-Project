import numpy as np
import functions as fn
import time as t

#constants from help.txt
packet=0
sdr=1
time=2
firstdata=3

win=30
N_iter=20
N_start=2

T=1
N_ant=2 #antennas
N_sig=1 #incoming signals
fc=2472e6 #center freq
df=312.5e3 #freq spacing
d=6.05e-2 #ant spacing

#subcarriers
all_scIdx=np.array([-26, -25, -24, -23, -22, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26]) #sc indices
N_sc=all_scIdx.size #no. of sc

scIdx=12 #MUSIC implementation
scCol = np.flatnonzero(all_scIdx==scIdx) + firstdata + N_sc*np.arange(N_ant)

freq=fc+scIdx*df
grid=np.linspace(-np.pi/2,np.pi/2,num=200)

fName_r = 'Data/-45degrees@2m.txt'
#fName_r = 'Data/0degrees@5m.txt'

############################### Load Data ###############################
data=np.genfromtxt(fName_r,delimiter=',',dtype=complex)
data[:,0]=data[:,0]-1 #pythonic indexing


############################ Start Simulation ###########################
aoaEst=np.empty(N_iter)
for iterIdx in np.arange(N_start,N_start+N_iter,1):
	print(iterIdx)
	t.sleep(0.5)
	pktIdx = np.arange(iterIdx*T,(iterIdx+win)*T,T)
	pkt_frame = data[ np.isin(data[:,0],pktIdx) ]
	csi_frame = pkt_frame[:,scCol]
	print(csi_frame)
	'''
	csi_frame = np.array([[ 1.86875212+0.01552772j,  4.17763901+2.30442023j],
 [ 0.406362  +1.84510744j,  2.03030539-4.35728741j],
 [-0.86664146-1.92564297j, -3.15464592-3.30734324j],
 [ 2.05936813-0.11371767j,  2.45318985+4.07948208j],
 [-2.10229969-0.2052629j , -3.41149998-3.36714077j],
 [-0.25000322+2.12480283j,  3.90513897-3.34793377j],
 [-1.02577949+0.85562164j,  0.30754495-5.04648113j],
 [ 1.41946244-0.32172877j,  4.2670846 -2.54550767j],
 [-0.08538542-1.53742421j, -1.37571263+5.00399113j]], dtype=complex)
	'''
	
	
	fn.musicAoA( fn.estCovMat(csi_frame.T),freq,d,N_ant,N_sig,grid)
	





