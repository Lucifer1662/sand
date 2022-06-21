#include "atomUpdater.h"

#include "atoms.h"
#include "chunk.h"

AtomUpdater::AtomUpdater(Chunks&& chunks)
    : chunks(std::move(chunks)) {}

void AtomUpdater::update(int x, int y) {
    this->x = x;
    this->y = y;
    auto a = get(0, 0);
    if (a == Sand) {
        auto b = get(0, -1);
        if (b == Air) {
            setDirty(0, 0, Air);
            setDirty(0, -1, Sand);
        } else {
            auto l = get(-1, -1);
            auto r = get(1, -1);

            if (l == Air && r == Air) {
                auto adjustment = -1 + 2 * (rand() % 2);
                setDirty(0, 0, Air);
                setDirty(adjustment, -1, Sand);
            } else if (l == Air) {
                setDirty(0, 0, Air);
                setDirty(-1, -1, Sand);
            } else if (r == Air) {
                setDirty(0, 0, Air);
                setDirty(1, -1, Sand);
            } else {
                set(0, 0, Sand);
            }
        }
    } else if (a == Stone) {
        set(0,0, Stone);
    }
}

Atom AtomUpdater::get(int x_off, int y_off) {
    return chunks[index(x_off+1, y_off+1)]->get(this->x + x_off, this->y + y_off);
}

void AtomUpdater::set(int x_off, int y_off, Atom atom) {
    chunks[index(x_off+1, y_off+1)]->set(this->x + x_off, this->y + y_off, atom);
}

void AtomUpdater::dirty(int x_off, int y_off) {
    chunks[index(x_off+1, y_off+1)]->dirty(this->x + x_off, this->y + y_off);
}

void AtomUpdater::setDirty(int x, int y, Atom atom) {
    set(x,y,atom);
    dirty(x,y);
}


