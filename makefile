CXX		= clang++
CXXFLAGS	= -O2 -std=c++11 -ffast-math -stdlib=libc++
LDFLAGS		= -stdlib=libc++ -L/usr/lib -lpugixml -lallegro -lallegro_main -lallegro_image -lallegro_font -lallegro_primitives -lallegro_dialog -lallegro_audio
INCLUDE		= -I. -I/usr/include/allegro5

DEPS		= bintracker.h helper_func.h gui/gui.h gui/worktune.h libmdal/pugixml.hpp libmdal/mdal.h data_as/mdatas.h\
		 sound_emul/sound_emul.h sound_emul/machines/vm.h sound_emul/machines/zxspectrum48.h sound_emul/machines/CPU/z80.h
OBJ		= bintracker.o helper_func.o\
 		 gui/elements.o gui/events.o gui/file_io.o gui/gui.o gui/input.o gui/keyhandler.o gui/mousehandler.o gui/status.o gui/work_tune.o\
		 libmdal/module.o libmdal/config.o libmdal/sequence.o libmdal/blocks.o libmdal/field.o libmdal/command.o\
		 data_as/data_as.o\
		 sound_emul/sound_emul.o\
		 sound_emul/machines/zxspectrum48.o sound_emul/machines/CPU/z80.o sound_emul/machines/CPU/z80instructions.o



bintracker: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE) $(LDFLAGS)

%.o: %.cpp $(DEPS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm $(OBJ)
