rem Compile all .cpp files in the src directory with optimization flags
g++ ./src/*.cpp -O3 -ffast-math -march=native -I .\src\ -I .\sdl\include\SDL2\ -L .\sdl\lib -lSDL2main -lSDL2 -o .\bin\pgol_v2.exe

rem Wait for 60 seconds before proceeding
timeout /t 60 /nobreak