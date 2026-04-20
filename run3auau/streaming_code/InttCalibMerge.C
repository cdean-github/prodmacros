#include <TFile.h>
#include <TString.h>
#include <iostream>
#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>
#include <vector>
#include <numeric>
#include <cmath>
TString base_dir = "/sphenix/data/data02/sphnxpro/production/run3auau/physics/new_nocdbtag_v001/";
int LoadHotMap(int runnumber, TFile* hotmap_files[8], TTree* hotmap_multiple[8], TTree* hotmap_single[8])
{
    for (int intt = 0; intt <= 7; ++intt)
    {
        TString intt_dir = base_dir + Form("DST_STREAMING_EVENT_intt%d", intt);
        int lower = (runnumber / 100) * 100;
        int upper = lower + 100;
        TString subdir = Form("run_%08d_%08d", lower, upper);
        TString full_path = intt_dir + "/" + subdir + "/hist/";
        TString filename = Form("CALIB_HOTMAP_DST_STREAMING_EVENT_intt%d_run3auau_new_nocdbtag_v001-%08d-00000.root", intt, runnumber);
        TString file_path = full_path + filename;
        TFile *file = TFile::Open(file_path);
        if (file)
        {
            std::cout << "Opened: " << file_path << std::endl;
            hotmap_files[intt] = file;
            TTree *multiple_tree = dynamic_cast<TTree *>(file->Get("Multiple"));
            if (multiple_tree)
            {
                // std::cout << "  ↳ Contains TTree: Multiple" << std::endl;
                hotmap_multiple[intt] = multiple_tree;
            }
            else
            {
                std::cout << "  ↳ TTree 'Multiple' not found" << std::endl;
            }

            TTree *single_tree = dynamic_cast<TTree *>(file->Get("Single"));
            if (single_tree)
            {
                // std::cout << "  ↳ Contains TTree: Single" << std::endl;
                hotmap_single[intt] = single_tree;
                TString dmean_branch = Form("Dmean%d", intt);
                TString dsigma_branch = Form("Dsigma%d", intt);
                if (single_tree->GetBranch(dmean_branch) && single_tree->GetBranch(dsigma_branch))
                {
                    // std::cout << "    ↳ Found branches: " << dmean_branch << ", " << dsigma_branch << std::endl;
                }
                else
                {
                    std::cout << "    ↳ Missing one or both branches: " << dmean_branch << ", " << dsigma_branch << std::endl;
                }
            }
            else
            {
                std::cout << "  ↳ TTree 'Single' not found" << std::endl;
            }
        }
        else
        {
            std::cerr << "Could not open: " << file_path << std::endl;
            hotmap_files[intt] = nullptr;
            hotmap_multiple[intt] = nullptr;
            hotmap_single[intt] = nullptr;
        }
    }
    for (int i = 0; i < 8; ++i) {
        if (!hotmap_files[i]) {
            std::cout << "One or more HOTMAP files could not be opened. No merged file will be created." << std::endl;
            return -1;
        }
    }
    return 0;
}

int LoadBcoMap(int runnumber, TFile* bcomap_files[8], TTree* bcomap_multiple[8], TTree* bcomap_single[8])
{

    for (int intt = 0; intt <= 7; ++intt)
    {
        TString intt_dir = base_dir + Form("DST_STREAMING_EVENT_intt%d", intt);
        int lower = (runnumber / 100) * 100;
        int upper = lower + 100;
        TString subdir = Form("run_%08d_%08d", lower, upper);
        TString full_path = intt_dir + "/" + subdir + "/hist/";
        TString filename = Form("CALIB_BCOMAP_DST_STREAMING_EVENT_intt%d_run3auau_new_nocdbtag_v001-%08d-00000.root", intt, runnumber);
        TString file_path = full_path + filename;
        TFile* file = TFile::Open(file_path);
        if (file)
        {
             std::cout << "Opened: " << file_path << std::endl;
            bcomap_files[intt] = file;
            TTree* multiple_tree = dynamic_cast<TTree*>(file->Get("Multiple"));
            if (multiple_tree)
            {
                bcomap_multiple[intt] = multiple_tree;
            }
            else
            {
                std::cout << "  ↳ TTree 'Multiple' not found" << std::endl;
            }

            TTree* single_tree = dynamic_cast<TTree*>(file->Get("Single"));
            if (single_tree)
            {
                bcomap_single[intt] = single_tree;
            }
            else
            {
                std::cout << "  ↳ TTree 'Single' not found" << std::endl;
            }
        }
        else
        {
            std::cerr << "Could not open: " << file_path << std::endl;
            bcomap_files[intt] = nullptr;
            bcomap_multiple[intt] = nullptr;
            bcomap_single[intt] = nullptr;
        }
    }
    for (int i = 0; i < 8; ++i) {
        if (!bcomap_files[i]) {
            std::cout << "One or more BCOMAP files could not be opened. No merged file will be created." << std::endl;
            return -1;
        }
    }
    return 0;
}

void MergeHotMap(std::string outputname, TTree* hotmap_multiple[8], TTree* hotmap_single[8])
{
    CDBTTree *cdbttree = new CDBTTree(outputname);
    int size = 0;
    int evt = 0;
    for (int intt = 0; intt < 8; ++intt)
    {
        TTree *tree = hotmap_multiple[intt];
        if (!tree)
            continue;


        int IID;
        int Ichannel;
        int Ichip;
        int Ifelix_channel;
        int Ifelix_server;
        int Iflag;
        tree->SetBranchAddress("IID", &IID);
        tree->SetBranchAddress("Ichannel", &Ichannel);
        tree->SetBranchAddress("Ichip", &Ichip);
        tree->SetBranchAddress("Ifelix_channel", &Ifelix_channel);
        tree->SetBranchAddress("Ifelix_server", &Ifelix_server);
        tree->SetBranchAddress("Iflag", &Iflag);

        Long64_t nentries = tree->GetEntries();
        for (Long64_t i = 0; i < nentries; ++i)
        {
            tree->GetEntry(i);
            // std::cout << "  Entry " << i << ": IID=" << IID
            //           << ", Ichannel=" << Ichannel
            //           << ", Ichip=" << Ichip
            //           << ", Ifelix_channel=" << Ifelix_channel
            //           << ", Ifelix_server=" << Ifelix_server
            //           << ", Iflag=" << Iflag << std::endl;
            cdbttree->SetIntValue(size, "felix_server", Ifelix_server);
            cdbttree->SetIntValue(size, "felix_channel", Ichannel);
            cdbttree->SetIntValue(size, "chip", Ichip);
            cdbttree->SetIntValue(size, "channel", Ifelix_channel);
            cdbttree->SetIntValue(size, "flag", Iflag);
            ++size;
        }
    }
    for (int intt = 0; intt < 8; ++intt)
    {

        TTree *single = hotmap_single[intt];
        if (!single)
            continue;


        std::string dmean_branch = Form("Dmean%d", intt);
        std::string dsigma_branch = Form("Dsigma%d", intt);
        double dmean;
        double dsigma = 0;
        single->SetBranchAddress(dmean_branch.c_str(), &dmean);
        single->SetBranchAddress(dsigma_branch.c_str(), &dsigma);
        single->SetBranchAddress("Ievent", &evt);
        single->GetEntry(0);
        cdbttree->SetSingleDoubleValue(dmean_branch, dmean);
        cdbttree->SetSingleDoubleValue(dsigma_branch, dsigma);
    }
    cdbttree->SetSingleIntValue("size", size);
    cdbttree->SetSingleIntValue("event", evt);
    cdbttree->Commit();
    cdbttree->CommitSingle();
    cdbttree->WriteCDBTTree();
    std::cout << "Saved hotmap merged file to: " << outputname << std::endl;
}

void MergeBcoMap(std::string outputname, TTree* bcomap_multiple[8], TTree* bcomap_single[8])
{
    CDBTTree *cdbttree = new CDBTTree(outputname);
    int size = 0;
    int ievent = 0;
    int runmode = 0;

    std::vector<int> bco_diffs;

    for (int intt = 0; intt < 8; ++intt)
    {
        TTree *tree = bcomap_multiple[intt];
        if (!tree)
            continue;

        int IID;
        int Ibco_diff;
        int Ifelix_channel;
        int Ifelix_server;
        tree->SetBranchAddress("Ibco_diff", &Ibco_diff);
        tree->SetBranchAddress("Ifelix_channel", &Ifelix_channel);
        tree->SetBranchAddress("Ifelix_server", &Ifelix_server);

        Long64_t nentries = tree->GetEntries();
        for (Long64_t i = 0; i < nentries; ++i)
        {
            tree->GetEntry(i);
            if (!(Ifelix_channel == 4 && Ifelix_server == 2))
            { // Maksked in Online. Do not use it for calculation
                bco_diffs.push_back(Ibco_diff);
            }

            cdbttree->SetIntValue(size, "felix_server", Ifelix_server);
            cdbttree->SetIntValue(size, "felix_channel", Ifelix_channel);
            cdbttree->SetIntValue(size, "bco_diff", Ibco_diff);
            ++size;
        }
    }

    double mean = std::accumulate(bco_diffs.begin(), bco_diffs.end(), 0.0) / bco_diffs.size();
    double sum_sq = 0.0;
    for (const auto& val : bco_diffs)
        sum_sq += (val - mean) * (val - mean);
    double stddev = std::sqrt(sum_sq / bco_diffs.size());

    for (int intt = 0; intt < 8; ++intt)
    {
        TTree *single = bcomap_single[intt];
        if (!single) continue;


        int events = 0;
        int runmode = 0;
        int isize = 0;
        single->SetBranchAddress("Ievents", &events);
        single->SetBranchAddress("Irunmode", &runmode);
        single->SetBranchAddress("Isize", &isize);
        single->GetEntry(0);

        cdbttree->SetSingleDoubleValue("StdDev", stddev);
        cdbttree->SetSingleIntValue("events", events);
        cdbttree->SetSingleIntValue("runmode", runmode);
        cdbttree->SetSingleIntValue("size", isize);
    }

    cdbttree->Commit();
    cdbttree->CommitSingle();
    cdbttree->WriteCDBTTree();
    std::cout << "Saved bcomap merged file to: " << outputname << std::endl;
}

void InttCalibMerge(int runnumber=69304,
		    const std::string& histdir="/sphenix/data/data02/sphnxpro/production/run3auau/physics/new_nocdbtag_v001/")
{
  base_dir = histdir;
    TFile *hotmap_files[8] = {nullptr};
    TFile *bcomap_files[8] = {nullptr};
    TTree *hotmap_multiple[8] = {nullptr};
    TTree *hotmap_single[8] = {nullptr};
    TTree *bcomap_multiple[8] = {nullptr};
    TTree *bcomap_single[8] = {nullptr};
    if (LoadHotMap(runnumber, hotmap_files, hotmap_multiple, hotmap_single) == -1 ||
        LoadBcoMap(runnumber, bcomap_files, bcomap_multiple, bcomap_single) == -1)
        return;

    std::string hotoutname = Form("CALIB_INTT_HOTMAP_MERGED-%08d.root", runnumber);
    std::string bcooutname = Form("CALIB_INTT_BCOMAP_MERGED-%08d.root", runnumber);
    MergeHotMap(hotoutname, hotmap_multiple, hotmap_single);
    MergeBcoMap(bcooutname, bcomap_multiple, bcomap_single);
    // Close and delete opened files (if any)
    for (int i = 0; i < 8; ++i)
    {
        if (hotmap_files[i])
        {
            hotmap_files[i]->Close();
            delete hotmap_files[i];
        }
        if (bcomap_files[i])
        {
            bcomap_files[i]->Close();
            delete bcomap_files[i];
        }
    }
}
