#include "chunk.h"

#include <iostream>

#include "RenderContext.h"
#include "atomUpdater.h"
#include "world.h"

void Chunk::init(int x, int y) {
    this->region.bl_x = x;
    this->region.bl_y = y;
    this->region.tr_x = x + ChunkSize - 1;
    this->region.tr_y = y + ChunkSize - 1;

    std::cout << region.bl_x << "," << region.bl_y << "," << region.tr_x << "," << region.tr_y << std::endl;

    atoms = &atoms1;
    futureAtoms = &atoms2;
    memset(atoms1.data(), 0, atoms1.size() * sizeof(Atom));
    memset(atoms2.data(), 0, atoms2.size() * sizeof(Atom));
    memset(neighbors.data(), 0, neighbors.size() * sizeof(Chunk*));

    atoms1.dirtyRect.isDirty = false;
    atoms2.dirtyRect.isDirty = false;
}

size_t Chunk::index(int x, int y) {
    if (x < this->region.bl_x || this->region.tr_x < x ||
        y < this->region.bl_y || this->region.tr_y < y) {
        std::cout << "Out of bounds: " << x << ", " << y
                  << " - in : " << this->region.bl_x << this->region.bl_y
                  << std::endl;
        return 0;
    }

    return (x - this->region.bl_x) + (y - this->region.bl_y) * ChunkSize;
}

Atom Chunk::get(int x, int y) { return (*atoms)[index(x, y)]; }
Atom Chunk::getFut(int x, int y) { return (*futureAtoms)[index(x, y)]; }

void Chunk::set(int x, int y, Atom atom) { (*futureAtoms)[index(x, y)] = atom; }
void Chunk::setSelf(int x, int y, Atom atom) { (*atoms)[index(x, y)] = atom; }

void Chunk::dirty(int x, int y) {
    futureAtoms->dirtyRect.addPoint(x, y, region);
}

void Chunk::update(AtomUpdater& atomUpdater, int bl_x, int bl_y, int tr_x,
                   int tr_y) {
    for (int y = bl_y; y <= tr_y; y++) {
        for (int x = bl_x; x <= tr_x; x++) {
            atomUpdater.update(x, y);
        }
    }
}

#define BOTTOM_PTR neighborChunk(0, -1, world)
#define TOP_PTR neighborChunk(0, 1, world)
#define LEFT_PTR neighborChunk(-1, 0, world)
#define RIGHT_PTR neighborChunk(1, 0, world)
#define BOTTOM_RIGHT_PTR neighborChunk(1, -1, world)
#define BOTTOM_LEFT_PTR neighborChunk(-1, -1, world)
#define TOP_RIGHT_PTR neighborChunk(1, 1, world)
#define TOP_LEFT_PTR neighborChunk(-1, 1, world)

void Chunk::update(World& world) {
    if (!this->atoms->dirtyRect.isDirty) return;

    auto bl_x_off = std::max(region.bl_x + 1, this->atoms->dirtyRect.bl_x);
    auto bl_y_off = std::max(region.bl_y + 1, this->atoms->dirtyRect.bl_y);
    auto tr_x_off = std::min(region.tr_x - 1, this->atoms->dirtyRect.tr_x);
    auto tr_y_off = std::min(region.tr_y - 1, this->atoms->dirtyRect.tr_y);

    {  // center
        AtomUpdater atomUpdater(
            {this, this, this, /**/ this, this, this, /**/ this, this, this});
        update(atomUpdater, bl_x_off, bl_y_off, tr_x_off, tr_y_off);
    }

    // bottom
    if (atoms->dirtyRect.containsY(this->region.bl_y)) {
        auto bottom = BOTTOM_PTR;
        AtomUpdater atomUpdater({bottom, bottom, bottom, /**/ this, this, this,
                                 /**/ this, this, this});

        update(atomUpdater, bl_x_off, region.bl_y, tr_x_off, region.bl_y);
    }

    // top
    if (atoms->dirtyRect.containsY(this->region.tr_y)) {
        auto top = TOP_PTR;
        AtomUpdater atomUpdater(
            {this, this, this, /**/ this, this, this, /**/ top, top, top});

        update(atomUpdater, bl_x_off, region.tr_y, tr_x_off, region.tr_y);
    }

    // left
    if (atoms->dirtyRect.containsX(this->region.bl_x)) {
        auto left = LEFT_PTR;
        AtomUpdater atomUpdater(
            {left, this, this, /**/ left, this, this, /**/ left, this, this});

        update(atomUpdater, region.bl_x, bl_y_off, region.bl_x, tr_y_off);
    }

    // right
    if (atoms->dirtyRect.containsX(this->region.tr_x)) {
        auto right = RIGHT_PTR;
        AtomUpdater atomUpdater({this, this, right, /**/ this, this, right,
                                 /**/ this, this, right});

        update(atomUpdater, region.tr_x, bl_y_off, region.tr_x, tr_y_off);
    }

    // bottom right
    if (atoms->dirtyRect.tr_x == region.tr_x &&
        atoms->dirtyRect.bl_y == region.bl_y) {
        auto right = RIGHT_PTR;
        auto bottom = BOTTOM_PTR;
        auto bottom_right = BOTTOM_RIGHT_PTR;
        AtomUpdater atomUpdater({bottom, bottom, bottom_right, /**/ this, this,
                                 right,
                                 /**/ this, this, right});

        update(atomUpdater, region.tr_x, region.bl_y, region.tr_x, region.bl_y);
    }

    // bottom left
    if (atoms->dirtyRect.bl_x == region.bl_x &&
        atoms->dirtyRect.bl_y == region.bl_y) {
        auto left = LEFT_PTR;
        auto bottom = BOTTOM_PTR;
        auto bottom_left = BOTTOM_LEFT_PTR;
        AtomUpdater atomUpdater({bottom_left, bottom, bottom, /**/ left, this,
                                 this,
                                 /**/ left, this, this});

        update(atomUpdater, region.bl_x, region.bl_y, region.bl_x, region.bl_y);
    }

    // top right
    if (atoms->dirtyRect.tr_x == region.tr_x &&
        atoms->dirtyRect.tr_y == region.tr_y) {
        auto right = RIGHT_PTR;
        auto top = TOP_PTR;
        auto top_right = TOP_RIGHT_PTR;
        AtomUpdater atomUpdater({this, this, right, /**/ this, this, right,
                                 /**/ top, top, top_right});

        update(atomUpdater, region.tr_x, region.tr_y, region.tr_x, region.tr_y);
    }

    // top left
    if (atoms->dirtyRect.bl_x == region.bl_x &&
        atoms->dirtyRect.tr_y == region.tr_y) {
        auto left = LEFT_PTR;
        auto top = TOP_PTR;
        auto top_left = TOP_LEFT_PTR;
        AtomUpdater atomUpdater({left, this, this, /**/ left, this, this,
                                 /**/ top_left, top, top});

        update(atomUpdater, region.bl_x, region.tr_y, region.bl_x, region.tr_y);
    }
}

void Chunk::draw(RenderContext& renderer) {
    if (atoms->dirtyRect.isDirty) {
        renderer.setColour(0x00, 0x80, 0xFF, 0xFF);
        renderer.renderRectangle(atoms->dirtyRect.bl_x, atoms->dirtyRect.bl_y,
                                 atoms->dirtyRect.tr_x, atoms->dirtyRect.tr_y);
    }

    renderer.setColour(0xFF, 0x80, 0xFF, 0xFF);
    renderer.renderRectangle(region.bl_x, region.bl_y, region.tr_x,
                             region.tr_y);

    for (int y = this->region.bl_y; y <= this->region.tr_y; y++) {
        for (int x = this->region.bl_x; x <= this->region.tr_x; x++) {
            auto a = get(x, y);
            if (a == Sand) {
                renderer.setColour(0x00, 0x80, 0x00, 0xFF);
                renderer.renderPoints(x, y);
            } else if (a == Stone) {
                renderer.setColour(0xFF, 0x80, 0x00, 0xFF);
                renderer.renderPoints(x, y);
            }
        }
    }
}

void Chunk::swapBuffers() {
    if (atoms->dirtyRect.isDirty) {
        for (int y = atoms->dirtyRect.bl_y; y <= atoms->dirtyRect.tr_y; y++) {
            // for (int x = atoms->dirtyRect.bl_x; x <= atoms->dirtyRect.tr_x;
            //      x++) {
            //     setSelf(x, y, getFut(x, y));
            // }
            auto start = index(atoms->dirtyRect.bl_x, y);
            auto end = index(atoms->dirtyRect.tr_x, y) + 1;
            memcpy(atoms->data() + start, futureAtoms->data() + start,
                   end - start);
        }
    }

    atoms->dirtyRect.reset();

    auto temp = atoms;
    atoms = futureAtoms;
    futureAtoms = temp;
}

Chunk* Chunk::neighborChunk(int x, int y, World& world) {
    auto& n = neighbors[(x+1) + (y+1) * 3];
    if (n) return n;

    n = &world.getChunk((region.bl_x+1) + x * ChunkSize,
                        (region.bl_y+1) + y * ChunkSize);
    
    return n;
}