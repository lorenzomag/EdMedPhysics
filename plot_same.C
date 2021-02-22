/* A function to read from two root files
   and plot a histogram from both of them 
   on the same canvas.

   gary.smith@ed.ac.uk
   22 Feb 2021
   
   This program was written for the EdMedPhysics 
   projects in 2021 to assist with data analysis.
   
   Input:
   Two root files - the output from
   the Geant4 simulation.
   
   Output: 
   A pdf file of the histograms plotted on the
   same canvas.

   How to run:
   
   From terminal command line
   $ root edit_histo.C
   
   From the root prompt
   $ root
   [0] .x edit_histo.C
   
   Or
   
   [0] .L edit_histo.C
   [1] edit_histo() 
   
*/

void plot_same(){

  // The names of the files to input.
  // Rename the simulation output files
  // and change the names here as appropriate.
  
  char filename_1[128] = "Protons.root";
  char filename_2[128] = "Gammas.root";

  // Note that you may have to scale the number 
  // beam particle to see plots from different
  // beam settings on the same scales.
  
  // Use the filename character arrays from above.
  TFile * input_file_1 = new TFile(filename_1,"");
  TFile * input_file_2 = new TFile(filename_2,"");
  
  // Connect to a histogram in the file.
  // https://root.cern.ch/doc/master/classTH1D-members.html
  TH1D * Edep_vs_Z_1 = (TH1D*)input_file_1->Get("Edep_vs_z"); 
  TH1D * Edep_vs_Z_2 = (TH1D*)input_file_2->Get("Edep_vs_z"); 
  
  // Fix the histogram title and axis labels.
  Edep_vs_Z_1->SetTitle(";Z in phantom (mm);Accumulated energy deposited (MeV)");
  Edep_vs_Z_2->SetTitle(";Z in phantom (mm);Accumulated energy deposited (MeV)");
  
  // Dont draw stats box
  gStyle->SetOptStat(0);
  
  // Dont show the title. 
  // To include the title, 
  // comment in the line below by putting // at the start of the line.
  gStyle->SetOptTitle(0);
  
  // Set max no. of digits on axes.
  // Display large numbers as exponentials 
  // Comment in the line below to implement by removing the //
  //TGaxis::SetMaxDigits(2);
  
  // Create a canvas to draw on
  // and later to save as a pdf
  TCanvas canvas;
  
  // Draw histogram as a simple histogram 
  // This will only apply to the pdf image.
  
  Edep_vs_Z_1->SetLineColor(kRed);
  Edep_vs_Z_2->SetLineColor(kBlue);

  Edep_vs_Z_1->Draw("hist");  
  Edep_vs_Z_2->Draw("hist same");
  //Edep_vs_Z_2->Draw("hist");

  // Save the canvas as a pdf...
  canvas.SaveAs("my_overplotted_histos.pdf");

  //  input_file_1->Close();
  input_file_2->Close();

  // Quit root and return to the terminal command prompt.
  gApplication->Terminate();
  
  // "Wouldn't you like to be a pepper too?"
}
