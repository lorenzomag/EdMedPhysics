{
  //Reading the input file
  TFile *file_input = TFile::Open("EdMedPhysics.root");

  //Reading the data (in TTree format
  TTree *EdMedPh = (TTree *) file_input->Get("EdMedPh;1");

  //Setting up your variables that you want to read from the data
  double Eabs;
  double X;
  double Y;
  double Z;

  // Connect these variables to the one in the TTree data 
  EdMedPh->SetBranchAddress("Eabs",&Eabs);
  EdMedPh->SetBranchAddress("X",&X); 
  EdMedPh->SetBranchAddress("Y",&Y);
  EdMedPh->SetBranchAddress("Z",&Z); // positions are recorded in mm

  // See how many entries (collections of E_abs,X,Y,Z) are written in the data file (each particle beam throw into the target can deposit energy multiple times at multiple steps inside the target, through the particle itself or with its shower of particles created)  
  int entries = EdMedPh->GetEntries();

  //Create your hystogram (100 bins between 0.0cm and 50.0cm)
  TH1F *histo_example =  new TH1F("histo_example","Histo example Edep@Z; Z(cm) ; E_{dep}",100,0.0,50.0);
  TH1F *histo_example_Z =  new TH1F("histo_example_Z","Histo example Z; Z(cm) ; N",100,0.0,50.0);
  TH1F *histo_example_Zcut =  new TH1F("histo_example_Zcut","Histo example Z (X^{2}+Y^{2} < 1mm) ; Z(cm) ; N",100,0.0,50.0);
  TH2F *plot_xy =  new TH2F("plot_xy","plot example Edep@Z; x(cm) ; y (cm)",100,0.0,50.0, 100,0.0,50.0);

  // loop through all the entries in the data file
  for (int i=0; i< entries; i++) {
    EdMedPh->GetEntry(i); // Read the single entry inside the data file (Now all the variables are linked to the values at this entry
    histo_example_Z->Fill(Z/10); // (Z is in mm!! we want it in cm)each value of Z is put into the histograms: This will give how many times there was an energy deposited in each range defined by this bin in the histogram
    histo_example->Fill(Z/10,Eabs); // same as before, but now each entry is weighted with the energy deposited at that moment. At the end this  will five the full energy deposited in each bin in Z.

    // We can make a cut on the data (NB variables in mm)
    if ( (X*X+Y*Y) < 1 ) {
      histo_example_Zcut->Fill(Z/10); // (Z is in mm!! we want it in cm)each value of Z is put into the histograms: This will give how many times there was an energy deposited in each range defined by this bin in the histogram
    }    
    if (Z<100){  //Z<100mm 
    //if (Z==350){ //add this line if you need Z=350mm
       plot_xy->Fill(X/10.,Y/10.); // convert mm to cm
    }
  }
  // now the looop through all the entries is finished
  

  // Create a Canvas where to plot the histogram created
  TCanvas *C1 = new TCanvas();
  histo_example_Z->Draw(); // Draw the histogram
  C1->Print("Z_picture.jpeg"); // Create a picture (just click on it in order to see it (or download it if you want to keep it)
  histo_example_Zcut->Draw(); // Draw the histogram
  C1->Print("Z_picture_CUT.jpeg"); // Create a picture (just click on it in order to see it (or download it if you want to keep it)
  histo_example->Draw(); // Draw the histogram
  C1->Print("Z_Edep_picture.jpeg"); // Create a picture (just click on it in order to see it (or download it if you want to keep it)

  // we can also draw them together
  histo_example_Z->SetLineColor(2); // change the color of the histogram to red
  histo_example_Z->SetTitle("Histo example Z (N_{events} RED) (E_{dep} BLACK); Z(cm);");
  histo_example_Z->Draw();
  histo_example->Draw("SAME");
  C1->Print("Z_picture_comparison.jpeg");

  TCanvas *C2 = new TCanvas();
  plot_xy->Draw("ColZ");
  C2->Print("xy_plot.jpeg");

  // we can also write the results in an output file
  TFile *file_output = new TFile("analysis_output.root","RECREATE");
  histo_example_Z->Write();
  histo_example_Zcut->Write();
  histo_example->Write();
  plot_xy->Write();

  file_output->Close();
}
