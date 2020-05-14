// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------
#ifndef VIRUS_SIMULATIONS_H_
#define VIRUS_SIMULATIONS_H_

#include "biodynamo.h"

namespace bdm {
  struct MoveModule : public BaseBiologyModule {
    BDM_STATELESS_BM_HEADER(MoveModule, BaseBiologyModule, 1);
    MoveModule() : BaseBiologyModule(gAllEventIds) {}

    /// Empty default event constructor, because MoveModule does not have state.
    template <typename TEvent, typename TBm>
    MoveModule(const TEvent& event, TBm* other, uint64_t new_oid = 0)
        : BaseBiologyModule(event, other, new_oid) {}

    void Run(SimObject* so) override {
      if (auto* cell = dynamic_cast<Cell*>(so)) {
        auto* random = Simulation::GetActive()->GetRandom();
        // create an array of 3 random numbers between -2 and 2
        Double3 cell_movements =
            random->template UniformArray<3>(-2, 2);
        // update the cell mass location, ie move the cell
        cell->UpdatePosition(cell_movements);
      }
    }
  };

inline int Simulate(int argc, const char** argv) {

  auto set_param = [](Param* param) {
      param->export_visualization_ = true; // allows export of visualisation files
      param->visualization_export_interval_ = 2; // export visualisation files every 2 steps
      param->visualize_sim_objects_["Cell"] = std::set<std::string>{ "" };
      param->bound_space_ = true;
      param->min_bound_ = 0;
      param->max_bound_ = 1000;  // cube of 100*100*100
  };
  Simulation simulation(argc, argv, set_param);

  auto* rm = simulation.GetResourceManager();
  auto* random = simulation.GetRandom();

  // Define initial model - in this example: single cell at origin

  int min = 700;
  int max = 800;

  size_t nb_of_cells=200; // number of cells in the simulation
  double x_coord, y_coord;

  for (size_t i = 0; i < nb_of_cells; ++i) {
    // our modelling will be a cell cube of 100*100*100
    // random double between 0 and 100
    x_coord = random->Uniform(min, max);
    y_coord = random->Uniform(min, max);

    // creating the cell at position x, y, z
    Cell* cell = new Cell({x_coord, y_coord, 0});
    // set cell parameters
    cell->SetDiameter(7.5);

    cell->AddBiologyModule(new MoveModule());

    rm->push_back(cell);  // put the created cell in our cells structure
  }

  // Run simulation for one timestep
  simulation.GetScheduler()->Simulate(500);

  std::cout << "Simulation completed successfully!" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // HELLO_WORLD_H_
