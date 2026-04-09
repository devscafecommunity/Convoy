#include "pivot_tool.h"

namespace convoy { namespace architect {

void PivotTool::on_mouse_down(Canvas* /*canvas*/, int x, int y) {
    pivot_ = {static_cast<float>(x), static_cast<float>(y)};
}
void PivotTool::on_mouse_drag(Canvas* /*c*/, int, int, int to_x, int to_y) {
    pivot_ = {static_cast<float>(to_x), static_cast<float>(to_y)};
}
void PivotTool::on_mouse_up(Canvas* /*c*/, int x, int y) {
    pivot_ = {static_cast<float>(x), static_cast<float>(y)};
}

}}
