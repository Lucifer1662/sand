CC=emcc
CFLAGS= -s USE_SDL=2 -s USE_SDL_GFX=2 
OPTIMIZATION = -O2
OUTPUT = example.html
DEPS = $(wildcard src/*.h)
CPPS = $(wildcard src/*.cpp)
OBJS = $(addprefix temp/,$(CPPS:.cpp=.o))


temp/%.o: %.cpp
	$(CC) $(OPTIMIZATION) $^ $(CFLAGS) -c -o $@

build/$(OUTPUT): $(OBJS) $(DEPS)
	$(CC) $(OPTIMIZATION) $(OBJS) $(CFLAGS) -o build/$(OUTPUT)


clean: 
	rmdir /Q /S temp &
	rmdir /Q /S build &
	md temp\src &
	md build &
	copy serve.json build\serve.json