# RISC‑V Simulator

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

A simple, educational RISC‑V instruction‑level simulator in C++. Assemble and execute RV32I code, with an optional standalone cache‑simulator module.

---

## 📂 Repository Layout

```text 
RISC_V_Simulator/
├── docs/                   # Design docs & lab reports (PDF/MD)
├── data/                   # Sample programs & testcases
│   ├── input.s
│   └── testcases.txt
├── include/                # (optional) Public headers
│   └── risc_v_simulator.hpp
├── src/                    # Implementation files
│   ├── simulator.cpp
│   ├── assembly_to_machine.cpp
│   └── cache_simulator.cpp
├── tests/                  # Unit‑tests or test scripts
│   └── run_all_tests.sh
├── Makefile                # Build & clean rules
├── README.md               # This file
└── LICENSE                 # MIT license
```

## 🚀 Usage
1. Prepare your files

- src/simulator.cpp — core simulator

- data/input.s — your RISC‑V assembly (one instruction per line; labels at start; in .data, use number, )

- Makefile — to build & clean

2. Build

make all  
- This produces: riscv_sim executable
- object files (e.g. assembly_to_machine.o)

3. Run

./riscv_sim data/input.s

4. Clean

make clean

## ⚙️ Input Format
- One instruction per line, properly spaced.

- Labels must start at column 1.

- In the .data section, list decimal integers separated by comma + space:
  
.data
arr: .word 10, 20, 30

## 🛠️ Features
- RV32I instruction set support (loads, stores, arithmetic, branches)

- Standalone cache simulator (cache_simulator.cpp) with configurable parameters

- Easily extensible to additional RISC‑V extensions or pipeline models

## 📝 Roadmap
- [ ] Add support for CSR and system instructions

- [ ] Integrate a pipeline timing model

- [ ] Create a CMake build option

- [ ] Automated unit tests (Catch2)

## 🤝 Contributing
- Fork the repo

- Create your feature branch
  git checkout -b feat/your-feature

- Commit your changes
  git commit -m "Add awesome feature"

- Push and open a Pull Request
  git push origin feat/your-feature
  
Please follow the existing code style and include tests where appropriate.

## 📄 License
This project is licensed under the MIT License. See LICENSE for details.
