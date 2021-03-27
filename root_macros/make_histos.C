/* A function to read from a root file
   and create plots to visualise the data

   gary.smith@ed.ac.uk
   01 Feb 2021
   
   This program was written for the EdMedPhysics 
   projects in 2021 to assist with data analysis.
   
   Input:
   A root file which is the output from
   the Geant4 simulation.
   
   Output: 
   1) an output root file containing new histograms
   2) pdfs of the histograms
   
   How to run:
   
   From terminal command line
   $ root make_histos.C

   From the root prompt
   $ root
   [0] .x make_histos.C
   
*/
void make_histos(TString particle = "neutrons"){
  // gROOT->SetStyle("ATLAS");
  gStyle->SetMarkerSize(0.2);

  // The name of the file to input, 
  // ie the simulation output file.

  TString filename = "datasets/"+particle+".root";

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
  double X, Y, Z; // coordinates of the hit
  double z_cut = 50.;
  double EventID;
  // Connect these variables to the ones in the TTree:
  // &Edep e.g assigns the address of the variable above 
  // to the variable "Edep" in the tree. 
  tree->SetBranchAddress("Edep",&Edep); 
  tree->SetBranchAddress("X",&X); 
  tree->SetBranchAddress("Y",&Y);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("EventID", &EventID);
  // NB positions were recorded in mm
  
  // There is one entry per hit and
  // typically many per beam particle.
  int entries = tree->GetEntries();
  int nBins = 100;
  double z_min = tree->GetMinimum("Z") / 10.;
  double z_max = tree->GetMaximum("Z") / 10.;
  float half_bin_width = (z_max - z_min) / (2. * nBins);

  z_min -= half_bin_width;
  z_max += half_bin_width;
  nBins++;
  // Declare a 2D histogram
  // This will be used to plot the hits in the X-Y plane
  TH2F *hXY =  new TH2F("hXY","; X (cm) ; Y (cm)",
			100,-15.0,15.0, 100,-15.0,15.0);
  

  // This will be used to plot the hits in the Z-R plane
  TH2F *hZR =  new TH2F("hZR","; Z (cm) ; R (cm)",
			100,0.0,15.0, 100,0.0,5.0);
  
  // Declare a 3D histogram
  // Given this is a 3D histogram the number of bins 
  // per axis has been reduced 
  TH3F *hZXY =  new TH3F("hZXY","; Z (cm) ; X (cm); Y (cm)",
			 50,0.,z_cut,32,-15.0,15.0, 32,-15.0,15.0);
  TH1D *hZ = new TH1D("hZ", "Deposited Energy; Z (cm) ; Deposited Energy (MeV)",
                      nBins, z_min, z_max);
  // The following for loop is used to iterate though the hits.
  // This is where the histograms are filled and data
  // cuts  std::vector<double> Edep_per_event (n_events,0);
  int n_events = tree->GetMaximum("EventID") + 1;
  std::vector<double> Edep_per_event(n_events, 0);
  for (int i = 0; i < entries; i++)
  {

    // Get data for next energy deposit
    tree->GetEntry(i);

    Edep_per_event[EventID] += Edep;
    hZ->Fill(Z / 10, Edep);
    // Fill histograms with positions in cm
    // weighted by energy depostied
    hXY->Fill(X/10.,Y/10.,Edep);
    
    hZR->Fill(Z/10,sqrt(X*X+Y*Y)/10,Edep);
    
    // Apply a cut on the Z position
    // if( (Z/10) < 70 )
    hZXY->Fill(Z/10.,X/10.,Y/10.,Edep);
  }
  // Get value of bin with maximum counts
  // ie energy in Z bin with maximum energy
  double max_energy = hZ->GetMaximum();

  // Get the bin with the maximum energy deposited
  int bin_max = hZ->GetMaximumBin();

  //->GetXaxis()->GetBinCenter(binmax);
  double x_max = hZ->GetXaxis()->GetBinCenter(bin_max);

  // The following variables should be adjusted.

  // Here we are using the bin with the maximum energy to define
  // where the tumour is. In reality one would design the treatment
  // plan around the location of tumour.
  double z_tumour = x_max;
  double tumour_radius = 3;
  if (particle == "gammas"){
    tumour_radius = 2; 
  }
  else if (particle == "neutrons")
  {
    tumour_radius = 2;
  }
  
  double max_Edep = *std::max_element(Edep_per_event.begin(), Edep_per_event.end());
  
  TEllipse* tumour_xy = new TEllipse(0,0,tumour_radius,tumour_radius);
  TEllipse* tumour_xr = new TEllipse(z_tumour,0,tumour_radius,tumour_radius,0,180);
  
  tumour_xy->SetFillColorAlpha(0, 0);
  tumour_xy->SetLineWidth(3);
  tumour_xy->SetLineColor(2);
  tumour_xr->SetFillColorAlpha(0,0);
  tumour_xr->SetLineWidth(3);
  tumour_xr->SetLineColor(2);
  // tumour_xz->SetFillColorAlpha(0,0);
  // Create a canvas to draw on
  // and for saving pdfs
  TCanvas * canvas = new TCanvas();
  TGaxis::SetMaxDigits(2);
  canvas->SetLogz();

  // Dont show the stats box 
  // gStyle->SetOptStat(0);

  // For Draw Options see 5.8.2 at the link below 
  //https://root.cern.ch/root/htmldoc/guides/users-guide/Histograms.html#drawing-histograms

  // Draw as heat map ie with
  // counts in colors
  hXY->Draw("colz");
  tumour_xy->Draw("same");
  gPad->Update();
  canvas->SaveAs(particle+"_hXY.pdf");

  hZR->Draw("colz");
  tumour_xr->Draw("same");
  gPad->Update();
  canvas->SaveAs(particle+"_hZR.pdf");

  canvas->SetLogz(0);

  // NB The following option may take some time to draw 
  // and/or open as an image
  hZXY->SetFillColor(29);
  hZXY->Draw();
  hZXY->Draw("sameISO");
  gPad->Update();
  canvas->SaveAs(particle+"_hZXY.pdf");
  
  // Create a new file to save the histograms in.
  TFile * output_file = new TFile(particle+"_histos.root","RECREATE");
  output_file->cd();
  
  hXY->Write();
  hZR->Write();
  hZXY->Write();
  
  
  output_file->Close();

  input_file->Close();
 
  // quit root and return to the terminal command prompt
  gApplication->Terminate();

}

bool withinTumour(double x, double y, double z, double rad = 2, double o = 5)
{
  double pos = pow(x, 2) + pow(y, 2) + pow(z - o, 2);

  if (pos <= pow(rad, 2))
    return true;
  else
    return false;
}
