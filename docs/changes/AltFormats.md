# Alternate JSON File Formats

## Formats

**High Candidates**
- [MessagePack](https://msgpack.org/index.html) ([this version in particular](https://github.com/mikeloomisgg/cppack))
- [TML](https://github.com/cppfw/tml)
- [Cap'n proto](https://capnproto.org/)

**Medium Candidates**
- [Internet Object](https://docs.internetobject.org/)
- [Apache Avro](https://avro.apache.org/)

**Low Candidates**
- [Protocol Buffers](https://protobuf.dev/)
- [FlatBuffers](https://flatbuffers.dev/)

## Hurdles

### Blender Exporter Support

The biggest hurdle will be the matter of the Blender explorter. More specifically, adding export support for the file.

How do you even install python modules on blender?

Would it be a problem to ship a blender addon with a python module in it?

## Format Pros & Cons

### MessagePack

**Pros**
- Header-only C++ library (see [this version in particular](https://github.com/mikeloomisgg/cppack))
- (Seemingly) fast

**Cons**

### TML

**Pros**

**Cons**

### Cap'n Proto

**Pros**
- (Seemingly) fastest

**Cons**
- Complicated as fuck
