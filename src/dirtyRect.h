

struct Rect {
    int bl_x, bl_y, tr_x, tr_y;

    void assign(int bl_x, int bl_y, int tr_x, int tr_y) {
        this->bl_x = bl_x;
        this->bl_y = bl_y;
        this->tr_x = tr_x;
        this->tr_y = tr_y;
    }
};

struct DirtyRect : public Rect {
    bool isDirty;

    void addPoint(int x, int y, Rect& bounds) {
        if (!isDirty) {
            assign(x-1, y-1, x+1, y+1);
            isDirty = true;
        } else {
            if (this->bl_x > x-1) this->bl_x = x-1;
            if (this->tr_x < x+1) this->tr_x = x+1;
            if (this->bl_y > y-1) this->bl_y = y-1;
            if (this->tr_y < y+1) this->tr_y = y+1;
        }

        bl_x = std::max(bounds.bl_x, bl_x);
        bl_y = std::max(bounds.bl_y, bl_y);
        tr_x = std::min(bounds.tr_x, tr_x);
        tr_y = std::min(bounds.tr_y, tr_y);
    }

    void reset(){
        isDirty = false;
    }

    bool containsY(int y){
        return this->bl_y <= y && this->tr_y >= y;
    }

    bool containsX(int x){
        return this->bl_x <= x && this->tr_x >= x;
    }
};