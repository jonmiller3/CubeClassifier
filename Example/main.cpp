//
//  main.cpp
//  Example
//
//  Created by Jonathan Miller on 8/16/13.
//  Copyright (c) 2013 UTFSM. All rights reserved.
//

#include <iostream>
#include <map>

#include "TTree.h"
#include "TH1F.h"
#include "TFile.h"
#include "TH2F.h"

#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"

const char *path="/Users/usmjonathanmiller/Dropbox/CubeClassifier/CubeClassifier/";


int main(int argc, const char * argv[])
{

    // insert code here...
    
    // this is an example of how to use cubeclassifer
    // as a tool (using ROOT as UI)
    
    
    std::cout << "Hello, World!\n";
    
    std::string filename="Test";
    
    
    
    TFile *rtfile = new TFile(Form("%sDefault%sfriend.root",path,filename.c_str()));
    TFile *vtfile = new TFile(Form("%sDefault%s.root",path,filename.c_str()));
    TTree *rttree = (TTree*)rtfile->Get("rtree");
    TTree *vttree = (TTree*)vtfile->Get(Form("treeDefault%s",filename.c_str()));
    
    // for this test just hardcode it
    int const mdim=5;
    
    // results tree
    float ratios[mdim];
    float ratiom[mdim];
    float numsig[mdim];
    float numdata[mdim];
    float nummc[mdim];
    int   kdim[mdim];
    int   elemnum;
    
    
    rttree->SetBranchAddress("numsig",numsig);
    rttree->SetBranchAddress("numdata",numdata);
    rttree->SetBranchAddress("nummc",nummc);
    
    rttree->SetBranchAddress("ratios",ratios);
    rttree->SetBranchAddress("ratiom",ratiom);
    
    // this is a bit pointless, the information is already there
    rttree->SetBranchAddress("kdim", kdim);
    
    rttree->SetBranchAddress("event",&elemnum);
    
    
    float v0,v1,v2,v3,v4;
    
    // who is this?
    vttree->SetBranchAddress("Variable7",&v0);
    vttree->SetBranchAddress("Variable4",&v1);
    vttree->SetBranchAddress("Variable5",&v2);
    vttree->SetBranchAddress("Variable6",&v3);
    
    
    vttree->AddFriend(rttree);

    // I should take advantage of the ease of ROOT
    
    // this is pretty hardcoded again
    TFile* outfile = new TFile(Form("%sDefault%sresult.root",path,filename.c_str()),"RECREATE");
    TTree *outtree = new TTree("ptree", "Result of Evalulation");
    outtree->AddFriend(Form("treeDefault%s",filename.c_str()),Form("%sDefault%s.root",path,filename.c_str()));
    outtree->AddFriend("rtree",Form("%sDefault%sfriend.root",path,filename.c_str()));
    
    
    float pnumsig;
    float pnumdata;
    float pnummc;
    float pratios;
    float pratiom;
    int pelemnum;
    int pkdim;
    
    TBranch *numsigb = outtree->Branch("numsig",&pnumsig,"numsig/F");
    TBranch *numdatab = outtree->Branch("numdata",&pnumdata,"numdata/F");
    TBranch *nummcb = outtree->Branch("nummc",&pnummc,"nummc/F");
    
    TBranch *ratiosb = outtree->Branch("ratios",&pratios,"ratios/F");
    TBranch *ratiosm = outtree->Branch("ratiom",&pratiom,"ratiom/F");
    
    TBranch *elemb   = outtree->Branch("event",&pelemnum,"event/I");
    
    TBranch *kb = outtree->Branch("kdim",&pkdim,"kdim/I");

    
    int ecounter=0;
    
    Long64_t nentries = vttree->GetEntries();
    
    
    for (Long64_t i=0; i<nentries; i++){
    //for (Long64_t i=0; i<20; i++){
        
        vttree->GetEntry(i);
        
        // I think that I can just pull out things together
        

        // it can take a k to limit
        float nsign=-1;
        float ndata=-1;
        float nmcar=-1;
        float rats=-1;
        float ratm=-1;
        for (int j=0; j<mdim; j++){

	  //std::cout << " lets get numdata "<<numdata[j]<<" "<<numsig[j]<<"\n"; 
            
            if (numdata[j]>0.00003&&numsig[j]>0.00003) {
                nsign=numsig[j];
                ndata=numdata[j];
                nmcar=nummc[j];
                rats=ratios[j];
                ratm=ratiom[j];
                pkdim=j;
		//std::cout << " we found one " <<j<<" "<<ndata<<"\n";
            }
            // selects best ratio/ec number
            // these should be inputs, maybe
            // even complicated inputs (a TF1)
            
        }
        
        
        pnumdata=ndata;
        pnumsig=nsign;
        pnummc=nmcar;
        pratiom=ratm;
        pratios=rats;
        
        pelemnum=elemnum;
        outtree->Fill();
        
        
        if (rats>0.9) {
            //std::cout<<" We found one "<<std::endl;
            //std::cout<<" this looks like "<<v0<<std::endl;
            //std::cout<<" this looks like "<<v1<<std::endl;
            //std::cout<<" this looks like "<<v2<<std::endl;
            //std::cout<<" this looks like "<<v3<<std::endl;
            //std::cout<<" this looks like "<<v4<<std::endl;

            
            //std::cout<<" this looks like "<<rats<<std::endl;
            //std::cout<<" this looks like "<<ndata<<std::endl;
            //std::cout<<" this looks like "<<nsign<<std::endl;
            ecounter++;
            
            
        }
        
        
    }
    
    outfile->Write();
    
    Printf("This is the total number of passing events %d",ecounter);
    
    return 0;
}


// this is for standalone ROOT use
void Test(std::string name, std::string cuts){
    
    TFile *_fileS = TFile::Open("DefaultSignalresult.root");
    TFile *_fileT = TFile::Open("DefaultTestresult.root");
    TFile *_fileB = TFile::Open("DefaultBackgroundresult.root");
    TTree *Stree = (TTree*)_fileS->Get("ptree");
    TTree *Ttree = (TTree*)_fileT->Get("ptree");
    TTree *Btree = (TTree*)_fileB->Get("ptree");

    TH1F* h1 = new TH1F("h1",Form("%s with %s",name.c_str(),cuts.c_str()),100,0,1);
    TH1F* h2 = new TH1F("h2",Form("%s with %s",name.c_str(),cuts.c_str()),100,0,1);
    TH1F* h3 = new TH1F("h3",Form("%s with %s",name.c_str(),cuts.c_str()),100,0,1);

    
    Stree->Draw(Form("treeDefaultSignal.%s>>h1",name.c_str()),cuts.c_str(),"");
    Ttree->Draw(Form("treeDefaultTest.%s>>h2",name.c_str()),cuts.c_str(),"");
    Btree->Draw(Form("treeDefaultBackground.%s>>h3",name.c_str()),cuts.c_str(),"");

    h1->SetLineWidth(2);
    h3->SetLineColor(4);

    cout<<" here is H1 "<<h1->Integral()<<" here is H2 "<<h2->Integral()<<" here is H3 "<<h3->Integral()<<endl;
    cout<<" here is the S/sqrt(S+B) "<<h1->Integral()/TMath::Sqrt(h1->Integral()+h3->Integral())<<endl;
    
    h1->DrawNormalized();
    h2->DrawNormalized("same");
    h3->DrawNormalized("same");
    
    return;
    
}

// this is for standalone ROOT use
void Test2(std::string name, std::string cuts,std::string title){
    
    TFile *_fileS = TFile::Open(Form("Default%sresult.root",title.c_str()));

    TTree *Stree = (TTree*)_fileS->Get("ptree");

    
    TH2F* h1 = new TH2F("h1",Form("%s with %s",name.c_str(),cuts.c_str()),100,0,1,100,0,1);
    
    Stree->Draw(Form("treeDefault%s.%s>>h1",title.c_str(),name.c_str()),cuts.c_str(),"");
    
    h1->DrawNormalized("colz");
    
    return;
    
}


#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif

// this is heavily based on the ROOT example
// it is something to compare with this classifier
void TMVATest(){
    
    TMVA::Tools::Instance();

    // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
    TString outfileName(Form("%sTMVA.root",path));
    TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
    
    // to get access to the GUI and all tmva macros
    TString tmva_dir(TString(gRootDir) + "/tmva");
    if(gSystem->Getenv("TMVASYS"))
        tmva_dir = TString(gSystem->Getenv("TMVASYS"));
    gROOT->SetMacroPath(tmva_dir + "/test/:" + gROOT->GetMacroPath() );
    gROOT->ProcessLine(".L TMVAGui.C");

    // Default MVA methods to be trained + tested
    std::map<std::string,int> Use;
    
    // --- Cut optimisation
    Use["Cuts"]            = 1;
    Use["CutsD"]           = 1;
    Use["CutsPCA"]         = 0;
    Use["CutsGA"]          = 0;
    Use["CutsSA"]          = 0;
    //
    // --- 1-dimensional likelihood ("naive Bayes estimator")
    Use["Likelihood"]      = 1;
    Use["LikelihoodD"]     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
    Use["LikelihoodPCA"]   = 1; // the "PCA" extension indicates PCA-transformed input variables (see option string\
    s)
    Use["LikelihoodKDE"]   = 0;
    Use["LikelihoodMIX"]   = 0;
    // --- Mutidimensional likelihood and Nearest-Neighbour methods
    Use["PDERS"]           = 1;
    Use["PDERSD"]          = 0;
    Use["PDERSPCA"]        = 0;
    Use["PDEFoam"]         = 1;
    Use["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
    Use["KNN"]             = 1; // k-nearest neighbour method
    //
    // --- Linear Discriminant Analysis
    Use["LD"]              = 1; // Linear Discriminant identical to Fisher
    Use["Fisher"]          = 0;
    Use["FisherG"]         = 0;
    Use["BoostedFisher"]   = 0; // uses generalised MVA method boosting
    Use["HMatrix"]         = 0;
    //
    // --- Function Discriminant analysis
    Use["FDA_GA"]          = 1; // minimisation of user-defined function using Genetics Algorithm
    Use["FDA_SA"]          = 0;
    Use["FDA_MC"]          = 0;
    Use["FDA_MT"]          = 0;
    Use["FDA_GAMT"]        = 0;
    Use["FDA_MCMT"]        = 0;
    //
    // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
    Use["MLP"]             = 0; // Recommended ANN
    Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
    Use["MLPBNN"]          = 0; // Recommended ANN with BFGS training method and bayesian regulator
    Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
    Use["TMlpANN"]         = 0; // ROOT's own ANN
    // --- Support Vector Machine
    Use["SVM"]             = 0;
    //
    // --- Boosted Decision Trees
    Use["BDT"]             = 1; // uses Adaptive Boost
    Use["BDTG"]            = 0; // uses Gradient Boost
    Use["BDTB"]            = 0; // uses Bagging
    Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
    Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
    //
    // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
    Use["RuleFit"]         = 0;
    // ---------------------------------------------------------------
    
    TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
                                               "Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

    // hardcoded
    factory->AddVariable( "Variable10", 'F' );
    factory->AddVariable( "Variable11", 'F' );
    factory->AddVariable( "Variable12", 'F' );
    factory->AddVariable( "Variable13", 'F' );

    // hardcoded (less important, this is for this test)
    TFile *inputS = TFile::Open(Form("%sDefaultSignal.root",path));
    TFile *inputB = TFile::Open(Form("%sDefaultBackground.root",path));

    // --- Register the training and test trees
    
    TTree *signal     = (TTree*)inputS->Get("treeDefaultSignal");
    TTree *background = (TTree*)inputB->Get("treeDefaultBackground");
    
    // global event weights per tree (see below for setting event-wise weights)
    Double_t signalWeight     = 1.0;
    Double_t backgroundWeight = 1.0;
    
    // You can add an arbitrary number of signal or background trees
    factory->AddSignalTree    ( signal,     signalWeight     );
    factory->AddBackgroundTree( background, backgroundWeight );

    // I don't have weights yet
    //factory->SetBackgroundWeightExpression( "weight" );
    
    // Apply additional cuts on the signal and background samples (can be different)
    TCut mycuts = ""; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
    TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";

    
    factory->PrepareTrainingAndTestTree( mycuts, mycutb,
                                        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );

    
    // ---------------- this is the different defititions
    
    if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variabl\
        es
        factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
                            "!H:!V:NTrees=50:nEventsMin=150:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
    
    // RuleFit -- TMVA implementation of Friedman's method
    if (Use["RuleFit"])
        factory->BookMethod( TMVA::Types::kRuleFit, "RuleFit",
                            "H:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );
    

    if (Use["BDT"])  // Adaptive Boost
        factory->BookMethod( TMVA::Types::kBDT, "BDT",
                            "!H:!V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
    
    
    if (Use["BDTB"]) // Bagging
        factory->BookMethod( TMVA::Types::kBDT, "BDTB",
                            "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
    

    if (Use["BDTD"]) // Decorrelation + Adaptive Boost
        factory->BookMethod( TMVA::Types::kBDT, "BDTD",
                            "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning:VarTransform=Decorrelate" );
    // CF(Clermont-Ferrand)ANN
    if (Use["CFMlpANN"])
        factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cy\
    cles:#nodes:#nodes:...
    
    // Tmlp(Root)ANN
    if (Use["TMlpANN"])
        factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...
    
    // Support Vector Machine
    if (Use["SVM"])
        factory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );
    

    // Boosted Decision Trees
    if (Use["BDTG"]) // Gradient Boost
        factory->BookMethod( TMVA::Types::kBDT, "BDTG",
                            "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:NNodesMax=5" );

    // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
    if (Use["MLP"])
        factory->BookMethod( TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );
    
    if (Use["MLPBFGS"])
        factory->BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );
    
    if (Use["MLPBNN"])
        factory->BookMethod( TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators
    
    if (Use["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
        factory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
                            "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );
    
    if (Use["FDA_MT"])
        factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
                            "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );
    
    if (Use["FDA_GAMT"])
        factory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
                            "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );
    
    if (Use["FDA_MCMT"])
        factory->BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
                            "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

    // Composite classifier: ensemble (tree) of boosted Fisher classifiers
    if (Use["BoostedFisher"])
        factory->BookMethod( TMVA::Types::kFisher, "BoostedFisher",
                            "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );
    
    // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or \
    SA)
    if (Use["FDA_MC"])
        factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
                            "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );
    
    if (Use["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
        factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
                            "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );
    

    // Linear discriminant (same as Fisher discriminant)
    if (Use["LD"])
        factory->BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );
    
    // Fisher discriminant (same as LD)
    if (Use["Fisher"])
        factory->BookMethod( TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );
    
    // Fisher with Gauss-transformed input variables
    if (Use["FisherG"])
        factory->BookMethod( TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

    if (Use["PDEFoamBoost"])
        factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoamBoost",
                            "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );
    
    // K-Nearest Neighbour classifier (KNN)
    if (Use["KNN"])
        factory->BookMethod( TMVA::Types::kKNN, "KNN",
                            "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );
    
    // H-Matrix (chi2-squared) method
    if (Use["HMatrix"])
        factory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

    if (Use["PDERSD"])
        factory->BookMethod( TMVA::Types::kPDERS, "PDERSD",
                            "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );
    
    if (Use["PDERSPCA"])
        factory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA",
                            "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );
    
    // Multi-dimensional likelihood estimator using self-adapting phase-space binning
    if (Use["PDEFoam"])
        factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
                            "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

    
    // Use a variable-dependent mix of splines and kernel density estimator
    if (Use["LikelihoodMIX"])
        factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX",
                            "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" );
    
    // Test the multi-dimensional probability density estimator
    // here are the options strings for the MinMax and RMS methods, respectively:
    //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
    //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
    
    if (Use["PDERS"])
        factory->BookMethod( TMVA::Types::kPDERS, "PDERS",
                            "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

    // PCA-transformed likelihood
    if (Use["LikelihoodPCA"])
        factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
                            "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" );
    
    // Use a kernel density estimator to approximate the PDFs
    if (Use["LikelihoodKDE"])
        factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
                            "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50" );

    
    if (Use["CutsSA"])
        factory->BookMethod( TMVA::Types::kCuts, "CutsSA",
                            "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );
    
    // Likelihood ("naive Bayes estimator")
    if (Use["Likelihood"])
        factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
                            "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50" );
    
    // Decorrelated likelihood
    if (Use["LikelihoodD"])
        factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD",
                            "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );

    
    if (Use["Cuts"])
        factory->BookMethod( TMVA::Types::kCuts, "Cuts",
                            "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );
    
    if (Use["CutsD"])
        factory->BookMethod( TMVA::Types::kCuts, "CutsD",
                            "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );
    
    if (Use["CutsPCA"])
        factory->BookMethod( TMVA::Types::kCuts, "CutsPCA",
                            "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );
    
    if (Use["CutsGA"])
        factory->BookMethod( TMVA::Types::kCuts, "CutsGA",
                            "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );
    

    // ---------------- this is the different defititions

    
    
    // Train MVAs using the set of training events
    factory->TrainAllMethods();
    
    // ---- Evaluate all MVAs using the set of test events
    factory->TestAllMethods();
    
    // ----- Evaluate and compare performance of all configured MVAs
    factory->EvaluateAllMethods();

    // Save the output
    outputFile->Close();
    
    std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
    std::cout << "==> TMVAClassification is done!" << std::endl;
    
    delete factory;
    
    
}

 
