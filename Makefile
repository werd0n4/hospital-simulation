standard = 17
libraries = -pthread -lncurses
target = hospital.o
flags = -g -std=c++$(standard)
files = main.cpp OperatingRoom.cpp Examination.cpp Bed.* Doctor.cpp Cleaner.cpp Reception.cpp Rehabilitation.* Patient.*

$(target): $(files) 
	g++ main.cpp -o $(target) $(libraries) $(flags) 

all: $(target)

clean:
	rm -f *.o $(target)

dump: #pokaż kod w postaci assemblera
	g++ main.cpp -o $(target) $(libraries) $(flags) -S