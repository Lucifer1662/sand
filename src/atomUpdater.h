#include <vector>

#include "atoms.h"
class Chunk;

class AtomUpdater {
   public:
    using Chunks = std::vector<Chunk *>;

   private:
    Chunks chunks;
    int x, y;

    int index(int x, int y){
        return x+y*3;
    }

    Atom get(int x, int y);

    void set(int x, int y, Atom atom);

    void dirty(int x, int y);

    void setDirty(int x, int y, Atom atom);

   public:
    AtomUpdater(Chunks&& chunks);

    void update(int x, int y);
};
