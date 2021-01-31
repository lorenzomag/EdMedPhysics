void make_plots(){
  
  // The name of the file to input, 
  // ie the simulation output file.
  string filename = "EdMedPhysics.root";

  // Declare a TFile object to read in data from.
  // Use the filename string from above.
  TFile * input_file = TFile::Open(filename);
  
  // A TTree is an ntuple with variables 
  // stored with different values per event.
  TTree * tree = (TTree *) file_input->Get("EdMedPh;1");

  // Declare variables to read from the tree.
  // The same variable names are used here as in the
  // input file but they could be anything.
  double Edep; // absorbed energy
  double X, Y, Z; // positions of the hits
  
  // Connect these variables to the ones in the TTree:
  // &Edep e.g assigns the address of the variable above 
  // to the variable "Edep" in the tree. Simples.
  tree->SetBranchAddress("Edep",&Edep); 
  tree->SetBranchAddress("X",&X); 
  tree->SetBranchAddress("Y",&Y);
  tree->SetBranchAddress("Z",&Z); // positions are recorded in mm

  
}
