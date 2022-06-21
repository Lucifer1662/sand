#pragma once
#include <array>
#include <iostream>

#include "atoms.h"
#include "dirtyRect.h"
#include <array>

const int ChunkSize = 200;

class ChunkProxy;
struct RenderContext;
class World;
struct AtomUpdater;



struct Atoms : public std::array<Atom, ChunkSize * ChunkSize> {
    DirtyRect dirtyRect;
};

class Chunk {
    Rect region;

    Atoms atoms1;
    Atoms atoms2;
    Atoms* atoms;
    Atoms* futureAtoms;
    std::array<Chunk*, 9> neighbors;

   public:
    Chunk() = default;
    Chunk(const Chunk&) = delete;
    Chunk(Chunk&&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    Atom get(int x, int y);
    Atom getFut(int x, int y);

    size_t index(int x, int y);
    void set(int x, int y, Atom atom);
    void setSelf(int x, int y, Atom atom);
    void dirty(int x, int y);

    void init(int x, int y);

    void update(AtomUpdater& atomUpdater,int bl_x, int bl_y, int tr_x, int tr_y);

    void update(World& world);

    void draw(RenderContext& renderer);

    void swapBuffers();

    Chunk* neighborChunk(int x, int y, World& world);
};