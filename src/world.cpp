#include "world.h"

#include <emscripten.h>

#include "chunk.h"

void World::draw(RenderContext& renderer) {
    for (auto& it : chunks) {
        auto& c = it.second;
        c->draw(renderer, renderer.forceDraw());
    }
}

void World::update() {
    for (auto& it : chunks) {
        Chunk* c = it.second.get();
        c->update(*this);
    }
}

void World::swapBuffers() {
    for (auto& it : chunks) {
        auto& c = it.second;
        c->swapBuffers();
    }

    for (auto& it : newChunks) {
        chunks[it.first] = std::move(it.second);
    }

    newChunks.clear();
}

Chunk& World::getChunk(int x, int y) {
    if (x < 0) x -= ChunkSize - 1;

    if (y < 0) y -= ChunkSize - 1;

    x = (x / ChunkSize) * ChunkSize;
    y = (y / ChunkSize) * ChunkSize;

    {
        auto it = chunks.find({x, y});
        if (it != chunks.end()) return *it->second;
    }

    {
        auto it = newChunks.find({x, y});
        if (it != newChunks.end()) return *it->second;
    }

    auto c = new Chunk();
    c->init(x, y);
    newChunks.emplace(std::make_pair(x, y), c);
    std::cout << "Init Chunk" << x << ", " << y << std::endl;
    return *c;
}

Atom World::getAtom(int x, int y) {
    auto& c = getChunk(x, y);
    return c.get(x, y);
}

void World::set(int x, int y, Atom atom) {
    auto& c = getChunk(x, y);
    if (x < c.region.bl_x || c.region.tr_x < x || y < c.region.bl_y ||
        c.region.tr_y < y) {
        std::cout << "WORLD OUT of bounds: " << x << ", " << y
                  << " - in : " << c.region.bl_x << c.region.bl_y << std::endl;
        return;
    }
    c.set(x, y, atom);
    c.dirty(x, y);
}
