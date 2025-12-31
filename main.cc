#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4Version.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"

// Employ your PhysicsList if necessary
#include "PhysicsList.hh"
// alterntive: #include "QBBC.hh"


#include <ctime>


int main(int argc, char** argv)
{
  // --- UI interface if no argument
  G4UIExecutive* ui = (argc == 1) ? new G4UIExecutive(argc, argv) : nullptr;

  // --- Run manager (MT if available)
  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

#ifdef G4MULTITHREADED
// runManager->SetNumberOfThreads(4);  // choose the value
#endif


  // --- Seed RNG (riproducibile? metti un seed fisso; altrimenti usa time)
  G4long seed = static_cast<G4long>(std::time(nullptr));
  CLHEP::HepRandom::setTheSeed(seed);

  // --- User init
  runManager->SetUserInitialization(new DetectorConstruction());

  // Physics: employ your list (suggested for RX) or comment and use QBBC
  runManager->SetUserInitialization(new PhysicsList());
  //runManager->SetUserInitialization(new QBBC());
  //runManager->SetUserInitialization(new FTFP_BERT());
  //runManager->SetUserInitialization(new QGSP_BIC_HP()); //it does not exist in Geant4_10

  runManager->SetUserInitialization(new ActionInitialization());

  // --- Visualization (only if interactive)
  G4VisManager* visManager = nullptr;
  if (ui) {
    visManager = new G4VisExecutive();
    visManager->Initialize();
  }

  // --- Initialize kernel
  runManager->Initialize();

  // --- UI manager
  auto UImanager = G4UImanager::GetUIpointer();

  if (!ui) {
    // === Batch mode ===
    if (argc < 2) {
      G4cerr << "[main] Nessun macro file specificato per batch mode.\n";
      return 1;
    }
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }
  else {
    // === Interactive mode ===
    // Upload an init vis macro if there is (alternatively this row can be %)
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // --- Cleanup
  delete visManager;  
  delete runManager;

  return 0;
}
