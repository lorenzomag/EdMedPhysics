void make_histograms(){
  
  // The name of the file to input, 
  // ie the simulation output file.
  char filename[128] = "EdMedPhysics.root";

  // Declare a TFile object to read in data from.
  // Use the filename string from above.
  TFile * input_file = TFile::Open(filename);
  
  // A TTree is an ntuple with variables 
  // stored with different values per event.
  //TTree * tree = (TTree *) input_file->Get("EdMedPh;1");
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
  
  // Declare a 2D histogram
  // This will be used to plot the hits in the X-Y plane
  TH2F *hXY =  new TH2F("hXY","; X (cm) ; Y (cm)",
			100,-15.0,15.0, 100,-15.0,15.0);
  

  // This will be used to plot the hits in the Z-R plane
  TH2F *hZR =  new TH2F("hZR","; Z (cm) ; R (cm)",
			100,0.0,15.0, 100,0.0,5.0);
  
  for (int i = 0; i < entries; i++) {
    
    // Get data for next energy deposit
    tree->GetEntry(i); 
    
    // Fill histograms with positions in cm
    // weighted by energy depostied
    hXY->Fill(X/10.,Y/10.,Edep);
    
    hZR->Fill(Z/10,sqrt(X*X+Y*Y)/10,Edep);
    
  }
  
  
  
  // Create a canvas to draw on
  // and later to save as a pdf
  TCanvas * canvas = new TCanvas();
  
  // Dont show show the stats box 
  gStyle->SetOptStat(0);

  hXY->Draw("colz");
  canvas->SaveAs("hXY.pdf");

  hZR->Draw("colz");
  canvas->SaveAs("hZR.pdf");
  
  TFile * output_file = new TFile("my_analysis.root","RECREATE");
  output_file->cd();
  hXY->Write();
  hZR->Write();
  output_file->Close();
  
  input_file->Close();
 
  // quit root and return to the terminal command prompt
  gApplication->Terminate();

}
