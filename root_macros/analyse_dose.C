/*

  A function for analysing dose.
  22 02 21
  gary.smith@ed.ac.uk
  
  The program was written to assist with the UoE
  medical physics course projects. It can be used
  as a skeleton for developing an analysis of dose.

  Input:
  A root file which is the output from
  the Geant4 simulation.
   
  Output: 
  1) an output root file containing a new histogram
  2) a pdf of the histogram
  
  How to run:
  
  From the terminal command line
  $ root analyse_dose.C
  
  From the root prompt
  $ root
  [0] .x analyse_dose.C

 */

#include <algorithm>
bool withinTumour(double x, double y, double z, double rad=2, double o=5);




void analyse_dose(TString particle = "neutrons")
{
  gROOT->SetStyle("ATLAS");
  gStyle->SetMarkerSize(0.2);
  // The name of the file to input, 
  // ie the simulation output file.
  cout << "Processing " << particle << endl;

  TString filename = "datasets/" + particle + ".root";

  // Declare a TFile object to read in data from.
  // Use the filename string from above.
  TFile * input_file = TFile::Open(filename);
  
  // A TTree is an ntuple with variables 
  // stored with different values per entry.
  TTree * tree = (TTree *) input_file->Get("EdMedPh");

  // Declare variables to read from the tree.
  // The same variable names are used here as in the
  // input file but they could be anything.
  double Edep; // energy deposited during the hit
  double X, Y, Z; // positions of the hit
  double EventID;
  // Connect these variables to the ones in the TTree:
  // &Edep e.g assigns the address of the variable above 
  // to the variable "Edep" in the tree. 
  tree->SetBranchAddress("Edep",&Edep); 
  tree->SetBranchAddress("X",&X); 
  tree->SetBranchAddress("Y",&Y);
  tree->SetBranchAddress("Z",&Z); 
  tree->SetBranchAddress("EventID",&EventID);
  // NB positions were recorded in mm
  
  // There is one entry per hit and
  // typically many per beam particle.
  int entries = tree->GetEntries();
  int n_events = tree->GetMaximum("EventID") + 1;
   
  // Set binning for histogram
  int    nBins = 100;
  double z_min = tree->GetMinimum("Z")/10. ;
  double z_max = tree->GetMaximum("Z")/10. ;
  float  half_bin_width = (z_max - z_min)/(2.*nBins);
  
  z_min -= half_bin_width;
  z_max += half_bin_width;
  nBins++;
  
  // 1D histogram
  // This will be used to plot the energy-weighted hits vs Z position 
  TH1D *hZ =  new TH1D("hZ","Deposited Energy; Z (cm) ; Deposited Energy (MeV)",
		       nBins,z_min,z_max);
  

  std::vector<double> Edep_per_event (n_events,0);
  for (int i = 0; i < entries; i++) {
    
    // Get data for next energy deposit
    tree->GetEntry(i); 

    Edep_per_event[EventID] += Edep;

    // energy weighted hit position
    hZ->Fill(Z/10,Edep);

  }



  double max_Edep = *std::max_element(Edep_per_event.begin(), Edep_per_event.end());
  int n_bins = (int) max_Edep * 2;

  TH1D *hDose = new TH1D("hDose",";Deposited Energy (MeV);Counts",
            n_bins,0.,max_Edep);
  

  //----------------------------------------
  
  // Get value of bin with maximum counts
  // ie energy in Z bin with maximum energy
  double max_energy = hZ->GetMaximum();

  // Get the bin with the maximum energy deposited  
  int   bin_max = hZ->GetMaximumBin();
 
  //->GetXaxis()->GetBinCenter(binmax);
  double x_max = hZ->GetXaxis()->GetBinCenter(bin_max);

  cout << endl;
  cout << " Data for " << particle <<  endl;
  cout << " Maximum deposited energy = " << max_energy << " MeV, at Z = " << x_max << " cm " << endl;

  // The following variables should be adjusted.

  // Here we are using the bin with the maximum energy to define 
  // where the tumour is. In reality one would design the treatment
  // plan around the location of tumour.
  double z_tumour  = x_max;
  double tumour_radius = 3;
  if (particle == "gammas")
  {
    tumour_radius = 2;
  }
  else if (particle == "neutrons")
  {
    tumour_radius = 4;
  }
  double tumor_dose = 0;
  double healthy_dose = 0;
  for (int i = 0; i < entries; i++)
  {

    // Get data for next energy deposit
    tree->GetEntry(i);

    if (withinTumour(X / 10, Y / 10, Z / 10,tumour_radius,z_tumour))
    {
      tumor_dose += Edep;
    }
    else
    {
      healthy_dose += Edep;
    }
  }


  TAxis *axis = hZ->GetXaxis();
  double integral = hZ->Integral(z_min, z_max);

  cout << endl;
  cout << " Total Dose = " << integral << endl;
  cout << endl;
    
  // Create a canvas to draw on
  // and later for saving a pdfs
  TCanvas * canvas = new TCanvas();
  
  // Dont show the stats box 
  // gStyle->SetOptStat(0);

  // For Draw Options see 5.8.2 at the link below 
  //https://root.cern.ch/root/htmldoc/guides/users-guide/Histograms.html#drawing-histograms

  // Use scientific units on axis if
  // more than 2 digits.
  // Comment in out or modify as needed.
  TGaxis::SetMaxDigits(2);

  hZ->Draw("hist");
  TF1* m1 = new TF1("m1", "expo", x_max+2, 50);
  
  hZ->Fit("m1","R");
  
  m1->Draw("same");
  m1->SetLineStyle(9);
  m1->SetLineColor(46);

  TF1* fitFunc = hZ->GetFunction("m1");
  double p0 = fitFunc->GetParameter(0);
  double p1 = fitFunc->GetParameter(1);

  canvas->Update();
  double hist_top = canvas->GetFrame()->GetY2();
  double xv[] = {z_tumour, z_tumour};
  double yv[] = {hist_top *1/4, hist_top * 3/4};
  double exv[] = {tumour_radius,tumour_radius};
  double eyv[] = {hist_top/4,hist_top/4};
  TGraphErrors* t_range = new TGraphErrors(2, xv, yv,exv,eyv);
  t_range->SetFillStyle(3351);
  t_range->SetFillColor(35);
  t_range->SetTitle("Range of tumour");

  t_range->Draw("same2");
  TString eq = TString::Format("#splitline{Fit to exponential decrease}{exp(%5.2f-%5.2f z)}", p0, abs(p1));
  m1->SetTitle(eq);
  // auto tZ = new TLatex;
  // tZ->SetNDC();
  // tZ->DrawLatex(0.73, 0.58, eq);

  // auto lZ = new TLegend(0.55, 0.78, 0.98, 0.95, "", "NDC");
  // lZ->SetBorderSize(1);
  // lZ->SetTextFont(42);
  // lZ->AddEntry("hZ", "Deposited Energy", "lpf");
  // lZ->AddEntry("m1", eq, "lf");
  // lZ->AddEntry("t_range","Range of tumour","lf");
  // lZ->Draw();

  canvas->BuildLegend();

  gStyle->SetOptStat(1000010);

  canvas->SaveAs(particle+"_hZ.pdf");

  // Create a new file to save the histograms in.
  TFile * output_file = new TFile(particle+"_dose.root","RECREATE");
  output_file->cd();
  
  // hZ->Write();

  
  double total_Edep = 0;
  int non_null = 0;
  for (int i=0; i<n_events; i++){
    double edep = Edep_per_event[i];
    if (edep > 0){
      // cout << edep << endl;
      total_Edep += edep;
      non_null += 1;
      hDose->Fill(edep);
    } 
  }

  gStyle->SetOptStat(1000010);
  hDose->Draw("hist");

  canvas->SaveAs(particle+"_hDose.pdf");
  hDose->Write();


  cout<< "Total Edep: " << total_Edep << endl;
  cout << "Average Edep per event: " << total_Edep / n_events << endl;
  cout << "Average Edep for non_null events: " << total_Edep / non_null << endl;
  cout << "Tumor dose about "<<z_tumour<<": " << tumor_dose << endl;
  cout << "Healty dose: " << healthy_dose << endl;


  cout << max_Edep << endl;
  output_file->Close();
  input_file->Close();
 
  // quit root and return to the terminal command prompt
  cout << hist_top << endl;
  gApplication->Terminate();
}

bool withinTumour(double x, double y, double z, double rad=2, double o=5)
{
  double pos = pow(x, 2) + pow(y, 2) + pow(z - o, 2);

  if (pos <= pow(rad, 2))
    return true;
  else
    return false;
}
