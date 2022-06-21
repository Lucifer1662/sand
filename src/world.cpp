#include "world.h"

#include "chunk.h"

void World::draw(RenderContext& renderer) {
    for (auto& it : chunks) {
        auto& c = it.second;
        c->draw(renderer);
    }
}

void World::update() {

    for (auto& it : chunks) {
        auto& c = it.second;
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
    // std::cout << "in:" << x << ", " << y << std::endl;
    if(x < 0)
        x -= ChunkSize;

    if(y < 0)
        y -= ChunkSize;

    x = (x / ChunkSize) * ChunkSize;
    y = (y / ChunkSize) * ChunkSize;
    // std::cout << "out:" << x << ", " << y << std::endl;


    {
    auto it = chunks.find({x, y});
    if (it != chunks.end()) return *it->second;
    }

    {
    auto it = newChunks.find({x, y});
    if (it != newChunks.end()) return *it->second;
    }

    std::cout << "blah" << x << ", " << y << std::endl;
    auto c = new Chunk();
    c->init(x,y);
    newChunks.emplace(std::make_pair(x,y), c);
    std::cout << "Init Chunk" << x << ", " << y << std::endl;
    return *c;
}

Atom World::getAtom(int x, int y) {
    auto& c = getChunk(x, y);
    return c.get(x, y);
}

void World::set(int x, int y, Atom atom) {
    auto& c = getChunk(x, y);
    c.set(x, y, atom);
    c.dirty(x, y);
}
