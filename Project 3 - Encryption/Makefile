.PHONY: all clean
CXX = g++
EXECS = vencrypt vdecrypt scrypt sbencrypt sbdecrypt

all: $(EXECS)

vencrypt: vencrypt.o
	@$(CXX) $^ -o $@

vdecrypt: vdecrypt.o
	@$(CXX) $^ -o $@

scrypt: scrypt.o
	@$(CXX) $^ -o $@

sbencrypt: sbencrypt.o lcgen.o
	@$(CXX) $^ -o $@

sbdecrypt: sbdecrypt.o lcgen.o
	@$(CXX) $^ -o $@

%.o: %.cpp
	@$(CXX) -c $< -o $@

clean:
	@rm $(EXECS) *.o
