void edit_histo(double x_max = -1.){
  
  // The name of the file to input.
  // Ie the simulation output file name.
  char filename[128] = "EdMedPhysics.root";

  // Declare a TFile object to read in data from.
  // Use the filename character array from above.
  TFile * input_file = new TFile(filename,"append");

  // Connect to a histogram in the file.
  // https://root.cern.ch/doc/master/classTH1D-members.html
  TH1D * Edep_vs_Z_edited = (TH1D*)input_file->Get("Edep_vs_z");  
  
  Edep_vs_Z_edited->SetName("Edep_vs_Z_edited");

  // Fix the histogram axis labels.
  Edep_vs_Z_edited->SetTitle("Edep_vs_Z;Z in phantom (mm);Accumulated energy deposited (MeV)");
  
  // Fix the x-axis range.
  // Stay in the while loop until a valid answer is given.
  // (NB there is an option to skip this by setting the value as an 
  // input argument to this function - see function declaration at top)
  while(x_max < 0 && x_max <= 60.){
    
    // Print a question to the user. IE to yourself.
    std::cout << std::endl;
    std::cout << " What do you want to set the x range maximum to (in cm)? " << std::endl;
    std::cout << " Enter a value between 0 and 50, e.g. for 12 cm enter 12." << std::endl;
    std::cout << " \t";
    
    // Read the users answer in.
    std::cin >> x_max;
    
    // Scale from cm to mm.
    x_max *= 10.; 
  }
  std::cout << std::endl;
  
  // Set the x-axis range to the value specified above.
  Edep_vs_Z_edited->GetXaxis()->SetRangeUser(-5,x_max);

  // The stats box can be modified.
  // (https://root.cern.ch/doc/master/classTPaveStats.html)
  //
  // Draw stats box with: Entries and Integral only.
  // Entries is the number of hits in which some energy was deposited.
  // Intergal is total energy deposited in the phantom during the entire run.
  gStyle->SetOptStat(1000010);
  
  // To not draw stats box,
  // uncomment the line below.
  //gStyle->SetOptStat(0);
  
  // Dont show the title. 
  // To include the title, 
  // comment in the line below by putting // at the start of the line.
  gStyle->SetOptTitle(0);

  // Create a canvas to draw on
  // and later to save as a pdf
  TCanvas canvas;
  // Draw histogram as a simple histogram 
  // with no error bars.
  // This will only apply to the pdf image
  Edep_vs_Z_edited->Draw("hist");
  
  
  // Save the canvas as a pdf
  canvas.SaveAs("my_histogram.pdf");
  // NB Several other file types can be specified
  // including image files (.png,.eps,.jpg); or even a .root or .C file.
  
  // Add the edited histogram to the existing root file.
  input_file->Write();
  
   std::cout << std::endl;
   std::cout << " " << filename << " has been updated" << std::endl;
   std::cout << std::endl;

  input_file->Close();
  
  // quit root and return to the terminal command prompt
  gApplication->Terminate();
  
  // "Wouldn't you like to be a pepper too?"
}
