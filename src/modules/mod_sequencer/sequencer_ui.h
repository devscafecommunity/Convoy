#pragma once

namespace convoy { namespace sequencer {

class SequencerUI {
public:
    void render();

private:
    int  fps_           = 12;
    int  frame_count_   = 4;
    int  current_frame_ = 0;
    bool playing_       = false;
};

}}
