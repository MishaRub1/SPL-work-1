# **Reconstruction Simulation — SPL Assignment #1**

## Overview

This project is an object-oriented simulation system implemented in C++ as part of the Systems Programming Lab (SPL) course.
The simulator models reconstruction plans for different settlements in SPLand, allowing evaluation of various rebuilding strategies after a long war.


**The assignment focuses on practicing advanced C++ concepts, including:**

1. Class design and inheritance
2. The Rule of 5
3. Memory management and leak prevention
4. Custom selection policies and dynamic simulation steps


## **Features**

- Simulation Engine: Manages settlements, facilities, and reconstruction plans.
- Flexible Plans: Supports different selection policies (naive, balanced, economy-focused, sustainability-focused) to determine which facilities to build next.
- Settlement Types: Supports villages, cities, and metropolises, each with different construction capacities.
- Action System: Supports a variety of user commands, including adding settlements and facilities, creating plans, simulating time steps, changing policies, and logging actions.
- Backup & Restore: Ability to backup and restore the entire simulation state.
- Robust CLI Interface: Reads a configuration file and supports runtime commands.


## **Build & Run**

- Build:
Run make.
The executable will be created in the bin/ directory as simulation.

- Run:
Run ./bin/simulation <config_file_path>, replacing <config_file_path> with the path to your configuration file.


## **Important Notes**

The project is tested and must compile and run on CS LAB Unix machines.
Valgrind was used to verify no memory leaks.
Follows all course specifications strictly, including using provided skeleton headers and class interfaces.

**Authors**

Implemented as part of SPL course (2024/Fall) — Ben-Gurion University of the Negev.



