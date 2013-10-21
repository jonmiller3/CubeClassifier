#!/usr/bin/env python
#from cmath import sqrt as ssqrt
#from cmath import cos as scos
from math import fabs as fabs     # more restrictive than abs() (in a good way)

#from cmath import sin as ssin

# from math import pow as spower   # slower than **
#from numpy import *
#from scipy import interpolate, integrate
# from scipy.linalg.matfuncs import expm2 as expm2
#from scipy.stats import lognorm
from warnings import warn

import scipy
import numpy
import cmath
import math
from ROOT import *


def create_example1_sim(dimension,noiselevel,nback,nsig,nmc):
    print " the noise level is a modification of ex 1 ",noiselevel
    print " the dimension is part of the standard approach ",dimension
    print " example 1 is constant mean difference, identity covariance"
    
    # determine mean
    u=numpy.ones(dimension)
    v=-1*u
    VI=numpy.identity(dimension)
    cval=scipy.spatial.distance.mahalanobis(u, v, VI)
    cval=2.7/cval
    
    u=u*cval
    v=-1*u

    f = TFile( 'example1Signal.root', 'RECREATE' )

    treeS = TTree( 'T', 'Signal Tree' )
    pdist=numpy.array(dimension)
    
    for vnum in Range(0,dimension):
        treeS.Branch("variable"+vnum,pdist[vnum],"variable"+vnum+"/F")
    
    for data in Range(0,nsig):
        pdist=numpy.random.multivariate_normal(u, VI)
        treeS.Fill()

    treeS.Write()
    f.Write()
    f.Close()


    f = TFile( 'example1Background.root', 'RECREATE' )
    # this does it for background
    # except I need to do the TFile part
    treeB = TTree( 'T', 'Background Tree' )
    ndist=numpy.array(dimension)
    
    for vnum in Range(0,dimension):
        treeB.Branch("variable"+vnum,ndist[vnum],"variable"+vnum+"/F")
    
    for data in Range(0,nback):
    
        ndist=numpy.random.multivariate_normal(v, VI)
    
        treeB.Fill()

    treeB.Write()
    f.Write()
    f.Close()


    f = TFile( 'example1MC.root', 'RECREATE' )

    treeM = TTree( 'T', 'Monte Carlo Tree' )
    mlist=numpy.array(dimension)

    for vnum in Range(0,dimension):
        treeM.Branch("variable"+vnum,mdist[vnum],"variable"+vnum+"/F")

    for data in Range(0,nmc):
    
        mdist=numpy.random.multivariate_normal(v, VI)
    
        treeM.Fill()

    treeM.Write()
    f.Write()
    f.Close()



def create_example2_sim(dimension,noiselevel,nback,nsig,nmc):
        print " the noise level is a modification of ex 2 ",noiselevel
        print " the dimension is part of the standard approach ",dimension
        print " example 2 is decreasing mean difference, block-diagnol interchangable covariance"

        lnum=[]
        for num in Range(0,dimension):
            if num<50:
                lnum.append(math.sqrt(50-num))
            else:
                lnum.append(0)
        u=numpy.array(lnum)
        v=-1*u

        lcov=[]
        covarray=numpy.identity(50)
        c8elem=numpy.ones((50,50))
        c8ident=.8*covarray
        covarray=covarray+c8elem-c8ident
    
        for d in Range(0,dimension/50):
            lcov.append(covarray)
        VI=scipy.linalg.block_diag(lcov)
        cval=scipy.spatial.distance.mahalanobis(u, v, VI)
        cval=2.7/cval

    u=u*cval
    v=-1*u

    f = TFile( 'example2Signal.root', 'RECREATE' )

    treeS = TTree( 'T', 'Signal Tree' )
    pdist=numpy.array(dimension)

    for vnum in Range(0,dimension):
            treeS.Branch("variable"+vnum,pdist[vnum],"variable"+vnum+"/F")

    for data in Range(0,nsig):
        pdist=numpy.random.multivariate_normal(u, VI)
        treeS.Fill()

    treeS.Write()
    f.Write()
    f.Close()


    f = TFile( 'example2Background.root', 'RECREATE' )
    # this does it for background
    # except I need to do the TFile part
    treeB = TTree( 'T', 'Background Tree' )
    ndist=numpy.array(dimension)

    for vnum in Range(0,dimension):
        treeB.Branch("variable"+vnum,ndist[vnum],"variable"+vnum+"/F")

    for data in Range(0,nback):

        ndist=numpy.random.multivariate_normal(v, VI)
    
        treeB.Fill()

    treeB.Write()
    f.Write()
    f.Close()


    f = TFile( 'example2MC.root', 'RECREATE' )

    treeM = TTree( 'T', 'Monte Carlo Tree' )
    mlist=numpy.array(dimension)

    for vnum in Range(0,dimension):
        treeM.Branch("variable"+vnum,mdist[vnum],"variable"+vnum+"/F")

    for data in Range(0,nmc):
    
        mdist=numpy.random.multivariate_normal(v, VI)
    
        treeM.Fill()

    treeM.Write()
    f.Write()
    f.Close()

