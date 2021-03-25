//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: EdMedPhcCalorimeterSD.cc 100946 2016-11-03 11:28:08Z gcosmo $
//
/// \file EdMedPhcCalorimeterSD.cc
/// \brief Implementation of the EdMedPhcCalorimeterSD class

#include "EdMedPhcCalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EdMedPhcCalorimeterSD::EdMedPhcCalorimeterSD(
                            const G4String& name, 
                            const G4String& hitsCollectionName,
                            G4int nofCells)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr),
   fNofCells(nofCells)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EdMedPhcCalorimeterSD::~EdMedPhcCalorimeterSD() 
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EdMedPhcCalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection 
    = new EdMedPhcCalorHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce
  auto hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 

  // Create hits
  // fNofCells for cells + one more for total sums 
  for (G4int i=0; i<fNofCells+1; i++ ) {
    fHitsCollection->insert(new EdMedPhcCalorHit());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EdMedPhcCalorimeterSD::ProcessHits(G4Step* step, 
                                     G4TouchableHistory*)
{ 
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();
  auto EventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  auto analysisManager = G4AnalysisManager::Instance();
  if(0.0 < edep) {

    G4ThreeVector p1 = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector p2 = step->GetPostStepPoint()->GetPosition();
    G4double x1 = p1.x();
    G4double y1 = p1.y();
    G4double z1 = p1.z();
    //    G4double r1 = std::sqrt(x1*x1 + y1*y1);
    G4double x2 = p2.x();
    G4double y2 = p2.y();
    G4double z2 = p2.z();
    //    G4double r2 = std::sqrt(x2*x2 + y2*y2);
    G4double x0 = 0.5*(x1 + x2);
    G4double y0 = 0.5*(y1 + y2);
    G4double z0 = 0.5*(z1 + z2);
    //    G4double r0 = std::sqrt(x0*x0 + y0*y0);
    analysisManager->FillH1(0,z0+25.*cm,edep);
    //analysisManager->FillH1(1,z0+25.*cm,edep);
    //analysisManager->FillH1(2,z0+25.*cm,edep);
    analysisManager->FillNtupleDColumn(0, edep);
    analysisManager->FillNtupleDColumn(1, x0);
    analysisManager->FillNtupleDColumn(2, y0);
    analysisManager->FillNtupleDColumn(3, z0+25.*cm);
    analysisManager->FillNtupleDColumn(4, EventID);
    analysisManager->AddNtupleRow();
  }
  // step length
  G4double stepLength = 0.;
  // commenting out so that track length also applies to neutrals
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( edep==0. && stepLength == 0. ) return false;      

  auto touchable = (step->GetPreStepPoint()->GetTouchable());
    
  // Get calorimeter cell id 
  auto layerNumber = touchable->GetReplicaNumber(1);
  
  // Get hit accounting data for this cell
  auto hit = (*fHitsCollection)[layerNumber];
  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumber; 
    G4Exception("EdMedPhcCalorimeterSD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }         

  // Get hit for total accounting
  auto hitTotal 
    = (*fHitsCollection)[fHitsCollection->entries()-1];
  
  // Add values
  hit->Add(edep, stepLength);
  hitTotal->Add(edep, stepLength); 
      
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EdMedPhcCalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
    long unsigned int nofHits = fHitsCollection->entries();
    G4cout
      << G4endl 
      << "-------->Hits Collection: in this event they are " << nofHits 
      << " hits in the tracker chambers: " << G4endl;
    for ( long unsigned int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
