# Makefile of the library "external/whispercpp"
# Anastasia Shchupak 2025,2026

include ../../makefile.defs

CPPFLAGS = -I ../../melder -I ../../fon -I ../../sys

WHISPER_AND_GGML_OBJECTS = \
	whisper.o \
	diarize.o \
	ggml.o \
	ggml-type-traits.o \
	ggml-memory-pool.o \
	ggml-backend.o \
	ggml-backend-dl.o \
	ggml-backend-reg.o \
	ggml-alloc.o \
	ggml-threading.o \
	ggml-quants.o

GGML_CPU_OBJECTS = \
	ggml-cpu.o \
	ggml-cpu-cpp.o \
	traits.o \
	quants.o \
	unary-ops.o \
	binary-ops.o \
	ops.o \
	vec.o

MODEL_OBJECTS = \
	model-ggml-segmentation.o \
	model-ggml-embedding.o

WHISPER_OBJECTS = $(WHISPER_AND_GGML_OBJECTS) $(GGML_CPU_OBJECTS) $(MODEL_OBJECTS)

.PHONY: all clean

all: libwhisper.a

clean:
	$(RM) $(WHISPER_OBJECTS)
	$(RM) libwhisper.a

libwhisper.a: $(WHISPER_OBJECTS)
	touch libwhisper.a
	rm libwhisper.a
	$(AR) cq libwhisper.a $(WHISPER_OBJECTS)
	$(RANLIB) libwhisper.a

$(WHISPER_OBJECTS): *.h
