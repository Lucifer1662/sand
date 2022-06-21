#pragma once

#include <memory.h>

#include <iostream>
#include <tuple>
#include <unordered_map>

#include "RenderContext.h"
#include "chunk.h"
#include "bss_thread_pool.h"

struct SDL_Renderer;

struct key_hash : public std::unary_function<std::tuple<int, int>, size_t> {
    size_t operator()(const std::tuple<int, int>& k) const {
        return std::get<0>(k) ^ std::get<1>(k);
    }
};

class World {
    std::unordered_map<std::tuple<int, int>, std::unique_ptr<Chunk>, key_hash> chunks;
    std::unordered_map<std::tuple<int, int>, std::unique_ptr<Chunk>, key_hash> newChunks;
    BS::thread_pool pool;
    bool finished = false;
    std::mutex lock;

   public:
    World();

    void draw(RenderContext& renderer);
    void update();
    bool finishedUpdate();
    void swapBuffers();
    Chunk& getChunk(int x, int y);
    Atom getAtom(int x, int y);
    void set(int x, int y, Atom atom);
};