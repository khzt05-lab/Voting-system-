# UIT Student Election Management System

A beginner-friendly C++17 console application for the University of Information Technology student election project.

## Election flow

1. The admin registers students. Batch and year are detected from each `TNT - XXXX` roll number.
2. Every registered student automatically becomes a candidate for their own class or major EC election.
3. Students vote only inside their own class or major.
4. The admin calculates the Class/Major EC winners.
5. Those winners automatically become the only Batch Representative candidates.
6. Every registered student in the batch may vote for the Batch Representative.
7. Results, votes, students, and election phases stay saved after the program restarts.

The admin never manually enters election candidates.

## UIT mappings

| Roll number | Batch | Year |
|---|---:|---|
| 1600-1799 | 9 | 5th Year |
| 1800-1999 | 10 | 4th Year |
| 2000-2199 | 11 | 3rd Year |
| 2200-2399 | 12 | 2nd Year |
| 2400-2600 | 13 | 1st Year |

Years 1-3 use Classes A-E. Years 4-5 use the seven UIT majors listed in the application.

## Files

- `main.cpp` starts the application.
- `headers/Student.h` and `src/Student.cpp` store student data and validate roll numbers.
- `headers/FileHandler.h` and `src/FileHandler.cpp` handle persistent text files.
- `headers/Admin.h` and `src/Admin.cpp` handle admin authentication.
- `headers/VotingSystem.h` and `src/VotingSystem.cpp` contain menus and the two election stages.
- `data/` contains students, votes, results, and election status.

The original `Candidate` files are no longer used because candidates are now generated from registered students and EC winners.

## Compile and run on Windows

Open the project folder in VS Code, open a terminal, and run:

```powershell
g++ -std=c++17 -Wall -Wextra -I headers main.cpp src/Admin.cpp src/Student.cpp src/FileHandler.cpp src/VotingSystem.cpp -o VotingSystem.exe
.\VotingSystem.exe
```

You can also double-click `compile.bat`.

Run the roll-number boundary tests with:

```powershell
g++ -std=c++17 -I headers tests/roll_number_tests.cpp src/Student.cpp -o roll_tests.exe
.\roll_tests.exe
```

On the first Admin Login, the program asks you to create an administrator username and password.
They are saved only on that computer in `data/admin_credentials.txt`, which Git ignores. Do not
share or commit that local file. This simple storage is suitable for a classroom console project,
not a real public election system.

## Test scenario

First use **Admin Login > Manage Students** to add at least four dummy Batch 12, Class C
students with roll numbers `TNT - 2301` through `TNT - 2304`. Then test as follows:

1. Admin logs in and starts the Batch 12 Class EC election.
2. Students `TNT - 2301` through `TNT - 2304` log in and vote.
3. A student tries to vote twice; the program rejects the second vote.
4. Admin calculates the Class C EC winner.
5. Admin starts the Batch 12 Representative election.
6. Batch 12 students vote again in the second election.
7. Admin calculates the Batch Representative winner.
8. Restart the program and confirm that results and voting status remain available.

For realistic Batch Representative testing, add students to more than one class so multiple EC winners become candidates.
