void param_converter() {



    TString califaCalFilename = "../../califa/califacalibparams_v1.root";

    UInt_t fDummyRunId=1;

    R3BCalifaCrystalCalPar *calPars;

    FairParRootFileIo* inputRoot = new FairParRootFileIo(kFALSE);

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();

    calPars = dynamic_cast<R3BCalifaCrystalCalPar*>(rtdb->getContainer("califaCrystalCalPar"));


    inputRoot->open(califaCalFilename,"in");
    rtdb->setFirstInput(inputRoot);

    Bool_t kParameterMerged = kFALSE;
    FairParAsciiFileIo* parOut = new FairParAsciiFileIo();

    parOut->open("../../califa/califacalibparams_v1.par","out");
    rtdb->setOutput(parOut);


    rtdb->addRun(1);
    rtdb->setInputVersion(fDummyRunId,(char*)"califaCrystalCalPar",1,1);

    rtdb->initContainers(kTRUE);
    rtdb->saveOutput();




}
