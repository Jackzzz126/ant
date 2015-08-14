
PROTOS = $(wildcard ./*.proto)
TARGET = ./build/libproto.a

.PHONY:all
all:$(TARGET)

$(TARGET):$(PROTOS)
	@protoc ./*.proto --cpp_out=.
	@mv *.pb.h *.pb.cc ./build/

