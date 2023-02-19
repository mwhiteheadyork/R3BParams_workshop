
typedef struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_TPAT_t unpacktpat;
    EXT_STR_h101_WRMASTER_t wrmaster;
    EXT_STR_h101_CALIFA_t califa;
    EXT_STR_h101_WRCALIFA_t wrcalifa;
    EXT_STR_h101_AMS_onion_t ams;


} EXT_STR_h101;

void unpack_offline()
{

    const Int_t nev = 1E6;
    Int_t fRunId = 1;

    const Int_t expId = 444; // select experiment: 444, 455 or 467

    // Create input -----------------------------------------
    TString filename = "/media/gabri/EXP_DATA/ProtonBeamData/lmd/main0023_0001.lmd";

    // Output file ------------------------------------------
    TString outputFileName = "main0023_mapped.root";
    TString ParFile = "exp_par.root"; // Output file for params


    Bool_t NOTstoremappeddata = false; // if true, don't store mapped data in the root file

    // UCESB configuration ----------------------------------
    TString ntuple_options = "RAW";
    TString ucesb_dir = getenv("UCESB_DIR");
    TString upexps_dir = "/home/gabri/CODE/upexps/202006_s444/";

    TString ucesb_path;

    if (expId == 444)
    {
        ucesb_path = upexps_dir + "202006_s444 --allow-errors --input-buffer=100Mi";
    }

    ucesb_path.ReplaceAll("//", "/");


    // Calibration files ------------------------------------
    TString dir = gSystem->Getenv("VMCWORKDIR");


    TString cal_par_filename = "../../../workshop_expParams.root";
    cal_par_filename.ReplaceAll("//", "/");




    // Create source using ucesb for input ------------------
    EXT_STR_h101 ucesb_struct;

    FairRunOnline* run = new FairRunOnline();
    R3BEventHeader *EvntHeader = new R3BEventHeader();
    run->SetEventHeader(EvntHeader);
    run->SetSink(new FairRootFileSink(outputFileName));


    R3BUcesbSource* source =
        new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);


    // Definition of reader ---------------------------------
    R3BUnpackReader* unpackreader =
        new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct, offsetof(EXT_STR_h101, unpack));

    R3BWhiterabbitMasterReader* unpackWRMaster;
    R3BCalifaFebexReader* unpackcalifa;
    R3BWhiterabbitCalifaReader* unpackWRCalifa;
    R3BAmsReader *unpackams;

    unpackams = new R3BAmsReader((EXT_STR_h101_AMS_onion *)&ucesb_struct.ams,
                                 offsetof(EXT_STR_h101, ams));

    unpackWRMaster = new R3BWhiterabbitMasterReader((EXT_STR_h101_WRMASTER*)&ucesb_struct.wrmaster, offsetof(EXT_STR_h101, wrmaster), 0x300);
    unpackcalifa =
            new R3BCalifaFebexReader((EXT_STR_h101_CALIFA*)&ucesb_struct.califa, offsetof(EXT_STR_h101, califa));

    unpackWRCalifa = new R3BWhiterabbitCalifaReader(
            (EXT_STR_h101_WRCALIFA*)&ucesb_struct.wrcalifa, offsetof(EXT_STR_h101, wrcalifa), 0xa00, 0xb00);



    // Add readers ------------------------------------------
    source->AddReader(unpackreader);

    source->AddReader(new R3BTrloiiTpatReader(&ucesb_struct.unpacktpat, offsetof(EXT_STR_h101, unpacktpat)));


   	unpackWRMaster->SetOnline(NOTstoremappeddata);
    source->AddReader(unpackWRMaster);

    unpackcalifa->SetOnline(NOTstoremappeddata);
    source->AddReader(unpackcalifa);

    unpackWRCalifa->SetOnline(NOTstoremappeddata);
    source->AddReader(unpackWRCalifa);


    // Create online run ------------------------------------

    run->SetSource(source);

    // Initialize -------------------------------------------
    run->Init();

    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    // Run --------------------------------------------------
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);



    std::cout << "Macro finished succesfully." << std::endl;
    std::cout << "Output file is " << outputFileName << std::endl;
    gApplication->Terminate();
}
