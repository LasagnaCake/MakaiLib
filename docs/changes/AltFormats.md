# Alternate JSON File Formats

## Formats

**High Candidates**
- [MessagePack](https://msgpack.org/index.html)
- [Cap'n proto](https://capnproto.org/)
- [Apache Avro](https://avro.apache.org/)

**Medium Candidates**
- [TML](https://github.com/cppfw/tml)
- [Internet Object](https://docs.internetobject.org/)

**Low Candidates**
- [Protocol Buffers](https://protobuf.dev/)
- [FlatBuffers](https://flatbuffers.dev/)

## Hurdles

### Blender Exporter Support

The biggest hurdle will be the matter of the Blender exporter. More specifically, adding export support for the file.

How do you even install python modules on blender?

Would it be a problem to ship a blender add-on with a python module in it?

## Format Pros & Cons

Performance profiling pulled from:
- [https://stackoverflow.com/a/20806438](https://stackoverflow.com/a/20806438)

### MessagePack

**Pros**
- Header-only C++ library (see [this version in particular](https://github.com/mikeloomisgg/cppack))
- (Seemingly) fast
- Has libraries for Python
- Middle-of-the-Road performance for simplicity (see library previously mentioned)

**Cons**

### Apache Avro

**Pros**

**Cons**
- Some complexity overhead
- Not as fast as MessagePack

### Cap'n Proto

**Pros**
- (Seemingly) fastest
- Faster than MessagePack

**Cons**
- High complexity overhead

### TML

**Pros**

**Cons**
- (Seemingly) not as fast as other alternatives
- C++ only
