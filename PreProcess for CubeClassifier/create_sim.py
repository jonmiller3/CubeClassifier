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
from scipy import spatial
from scipy import linalg
import numpy
import cmath
import math
import array
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
    pdist=numpy.zeros(dimension,dtype=float)
    print "test",pdist
    
    treeS.Branch("variable",pdist,"variable["+str(dimension)+"]/D")
    #for vnum in range(0,dimension):
    #    treeS.Branch("variable"+str(vnum),pdist[vnum],"variable"+str(vnum)+"/F")
    
    pdist=numpy.random.multivariate_normal(u, VI)
    print "test",pdist
    for data in range(0,nsig):
        dist=numpy.random.multivariate_normal(u, VI)
        for ele in range(0,dimension):
            pdist[ele]=dist[ele]
            if (data<2):
                print "type",type(dist[ele]),dist[ele],type(pdist[ele])

        treeS.Fill()

    treeS.Write()
    f.Write()
    f.Close()


    f = TFile( 'example1Background.root', 'RECREATE' )
    # this does it for background
    # except I need to do the TFile part
    treeB = TTree( 'T', 'Background Tree' )
    ndist=numpy.zeros(dimension,dtype=float)


    treeB.Branch("variable",pdist,"variable["+str(dimension)+"]/F")

    #for vnum in range(0,dimension):
    #    treeB.Branch("variable"+str(vnum),ndist[vnum],"variable"+str(vnum)+"/F")
    
    for data in range(0,nback):
        dist=numpy.random.multivariate_normal(v, VI)
        for ele in range(0,dimension):
            ndist[ele]=dist[ele]
    
        treeB.Fill()

    treeB.Write()
    f.Write()
    f.Close()


    f = TFile( 'example1MC.root', 'RECREATE' )

    treeM = TTree( 'T', 'Monte Carlo Tree' )

    mdist=numpy.zeros(dimension,dtype=float)

    treeM.Branch("variable",pdist,"variable["+str(dimension)+"]/F")

    #for vnum in range(0,dimension):
    #    treeM.Branch("variable"+str(vnum),mdist[vnum],"variable"+str(vnum)+"/F")

    for data in range(0,nmc):
        dist=numpy.random.multivariate_normal(v, VI)
        for ele in range(0,dimension):
            mdist[ele]=dist[ele]
    
        treeM.Fill()

    treeM.Write()
    f.Write()
    f.Close()



def create_example2_sim(dimension,noiselevel,nback,nsig,nmc,minvars):
    print " the noise level is a modification of ex 2 ",noiselevel
    print " the dimension is part of the standard approach ",dimension
    print " example 2 is decreasing mean difference, block-diagnol interchangable covariance"

    lnum=[]
    for num in range(0,dimension):
        if num<minvars:
            lnum.append(math.sqrt(minvars-num))
        else:
            lnum.append(0)
    u=numpy.array(lnum)
    v=-1*u

    lcov=[]
    covarray=numpy.identity(minvars)
    c8elem=numpy.ones((minvars,minvars))
    c8ident=.8*covarray
    covarray=covarray+c8elem-c8ident
    
    for d in range(0,dimension/minvars):
        lcov.append(covarray)
    VI=scipy.linalg.block_diag(lcov)
    cval=scipy.spatial.distance.mahalanobis(u, v, VI)
    cval=2.7/cval

    u=u*cval
    v=-1*u

    f = TFile( 'example2Signal.root', 'RECREATE' )

    treeS = TTree( 'T', 'Signal Tree' )
    pdist=numpy.zeros(dimension,dtype=float)
    print "test",pdist

    treeS.Branch("variable",pdist,"variable["+str(dimension)+"]/D")
    #for vnum in range(0,dimension):
    #    treeS.Branch("variable"+str(vnum),pdist[vnum],"variable"+str(vnum)+"/F")

    pdist=numpy.random.multivariate_normal(u, VI)
    print "test",pdist
    for data in range(0,nsig):
        dist=numpy.random.multivariate_normal(u, VI)
        for ele in range(0,dimension):
            pdist[ele]=dist[ele]
            if (data<2):
                print "type",type(dist[ele]),dist[ele],type(pdist[ele])
    
        treeS.Fill()

    treeS.Write()
    f.Write()
    f.Close()


    f = TFile( 'example2Background.root', 'RECREATE' )
    # this does it for background
    # except I need to do the TFile part
    treeB = TTree( 'T', 'Background Tree' )
    

    ndist=numpy.zeros(dimension,dtype=float)


    treeB.Branch("variable",pdist,"variable["+str(dimension)+"]/F")

    #for vnum in range(0,dimension):
    #    treeB.Branch("variable"+str(vnum),ndist[vnum],"variable"+str(vnum)+"/F")

    for data in range(0,nback):
        dist=numpy.random.multivariate_normal(v, VI)
        for ele in range(0,dimension):
            ndist[ele]=dist[ele]
    
        treeB.Fill()

    treeB.Write()
    f.Write()
    f.Close()


    f = TFile( 'example2MC.root', 'RECREATE' )

    treeM = TTree( 'T', 'Monte Carlo Tree' )
    mdist=numpy.zeros(dimension,dtype=float)

    treeM.Branch("variable",pdist,"variable["+str(dimension)+"]/F")

    #for vnum in range(0,dimension):
    #    treeM.Branch("variable"+str(vnum),mdist[vnum],"variable"+str(vnum)+"/F")

    for data in range(0,nmc):
        dist=numpy.random.multivariate_normal(v, VI)
        for ele in range(0,dimension):
            mdist[ele]=dist[ele]

        treeM.Fill()

    treeM.Write()
    f.Write()
    f.Close()

# initially I am not interested in doing hdlss
create_example1_sim(8,0,20000,20000,20000)
create_example2_sim(8,0,20000,20000,20000,4)

