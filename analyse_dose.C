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

void analyse_dose(){
  
  // The name of the file to input, 
  // ie the simulation output file.
  char filename[128] = "EdMedPhysics.root";

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
  
  // Connect these variables to the ones in the TTree:
  // &Edep e.g assigns the address of the variable above 
  // to the variable "Edep" in the tree. 
  tree->SetBranchAddress("Edep",&Edep); 
  tree->SetBranchAddress("X",&X); 
  tree->SetBranchAddress("Y",&Y);
  tree->SetBranchAddress("Z",&Z); 
  // NB positions were recorded in mm
  
  // There is one entry per hit and
  // typically many per beam particle.
  int entries = tree->GetEntries();  

  // Set binning for histogram
  int    nBins = 50.;
  double z_min = 0.;
  double z_max = 15.;
  float  half_bin_width = (z_max - z_min)/(2.*nBins);
  
  z_min -= half_bin_width;
  z_max += half_bin_width;
  nBins++;
  
  // 1D histogram
  // This will be used to plot the energy-weighted hits vs Z position 
  TH1D *hZ =  new TH1D("hZ","; Z (cm) ; Accumulated Energy (MeV)",
		       nBins,z_min,z_max);
  

  for (int i = 0; i < entries; i++) {
    
    // Get data for next energy deposit
    tree->GetEntry(i); 

    // energy weighted hit position
    hZ->Fill(Z/10,Edep);

  }

  //----------------------------------------
  
  // Get value of bin with maximum counts
  // ie energy in Z bin with maximum energy
  double max_energy = hZ->GetMaximum();

  // Get the bin with the maximum energy deposited  
  int   bin_max = hZ->GetMaximumBin();
 
  //->GetXaxis()->GetBinCenter(binmax);
  double x_max = hZ->GetXaxis()->GetBinCenter(bin_max);

  cout << endl;
  cout << " Maximum deposited energy = " << max_energy << " MeV, at Z = " << x_max << " cm " << endl;

  // The following variables should be adjusted.

  // Here we are using the bin with the maximum energy to define 
  // where the tumour is. In reality one would design the treatment
  // plan around the location of tumour.
  double z_tumour  = x_max;
  // Start with an arbirtary location - modify value as needed.
  double z_healthy = 6.0;

  float dose_healthy = hZ->GetBinContent(hZ->GetXaxis()->FindBin(z_healthy)); 
  float dose_tumour  = hZ->GetBinContent(hZ->GetXaxis()->FindBin(z_tumour));   

  cout << endl;
  cout << " Dose = " << dose_healthy << " MeV, at z = " << z_healthy << " cm " << endl;
  cout << " Dose = " << dose_tumour  << " MeV, at z = " << z_tumour  << " cm " << endl;
  cout << endl;
    
  // Create a canvas to draw on
  // and later for saving a pdfs
  TCanvas * canvas = new TCanvas();
  
  // Dont show the stats box 
  gStyle->SetOptStat(0);

  // For Draw Options see 5.8.2 at the link below 
  //https://root.cern.ch/root/htmldoc/guides/users-guide/Histograms.html#drawing-histograms

  hZ->Draw("hist");
  canvas->SaveAs("hZ.pdf");

  // Create a new file to save the histograms in.
  TFile * output_file = new TFile("dose.root","RECREATE");
  output_file->cd();
  
  hZ->Write();

  output_file->Close();
  input_file->Close();
 
  // quit root and return to the terminal command prompt
  gApplication->Terminate();

}
