import numpy as np
import pandas as pd
import scipy.signal as ss
import matplotlib
matplotlib.use('QT4Agg')
import matplotlib.pyplot as plt
plt.ion()

# ----------------------------------------------------------------------------------------------

def estCovMat(X):

	dim = X.ndim

	if dim==1: #this needs to be confirmed
		return np.matmul(X,np.conj(X).T)
	elif dim==2:
		return np.matmul( X,np.conj(X).T )
	elif dim==3:
		return np.matmul( X,np.swapaxes(np.conj(X),-1,-2) )

# ----------------------------------------------------------------------------------------------

def musicAoA(cov,f,d,N_ant,N_sig,grid):
	''' MUSIC algorithm for AoA estimation

	Inputs:
		cov - covariance matrix N_dimxN_dim
		f - frequency
		d - antenna spacing
		N_ant - no. of antennas
		N_sig - no. of incoming signals
		grid - on which to evaluate MUSIC

	Output:
		theta - top N peaks
	'''
	
	N_dim = cov.shape[0]
	kd = 2*np.pi*f*d/3e8 
	
	#find noise subspace
	w,v = np.linalg.eig(cov)
	inds = np.argsort(np.abs(w))
	Qn = np.conj( v[:,inds[:N_dim-N_sig]] ) #noise subspace
	
	#setup steering vectors (N_dim for each grid point)
	s=np.empty((grid.size,N_dim),dtype=complex)
	for _n in np.arange(N_dim):
		s[:,_n] = np.exp(-1j*kd*np.sin(grid)*_n)
	
	P = 1/np.linalg.norm( np.matmul(np.conj(s),Qn), axis=1  )**2
	P = P.flatten()

	peaks,properties = ss.find_peaks(P)
	print("OUR STUFF")
	#print(csi_frame.T)
	print("GRID")
	print(grid)
	print("AoA is at: " + str(grid[peaks[0]] * 180/np.pi) + " degrees")

	#plot spectrum
	print("try to plot")
	plt.clf()
	plt.plot(grid*180/np.pi,10*np.log10(P))
	plt.ylabel('Pseudo-spectrum (dB)')
	plt.xlabel('Angle (deg)')
	plt.grid(linestyle=':',linewidth=1.2)
	plt.show()
	plt.pause(0.001)
	print("done")
	

	return 10*np.log10(P)

# ----------------------------------------------------------------------------------------------
		
	










