#include "bucket_tool.h"

namespace convoy {
namespace architect {

bool BucketTool::colors_similar(Color c1, Color c2) {
    int dr = c1.r - c2.r;
    int dg = c1.g - c2.g;
    int db = c1.b - c2.b;
    int da = c1.a - c2.a;
    return (dr*dr + dg*dg + db*db + da*da) <= (tolerance_ * tolerance_);
}

void BucketTool::flood_fill(Canvas* canvas, int x, int y, Color target, Color fill) {
    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<bool>> visited(canvas->height(), std::vector<bool>(canvas->width(), false));

    q.push({x, y});
    visited[y][x] = true;

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();

        Color current = canvas->get_pixel(cx, cy);
        if (!colors_similar(current, target)) {
            continue;
        }

        canvas->set_pixel(cx, cy, fill);

        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};
        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (nx >= 0 && nx < (int)canvas->width() && ny >= 0 && ny < (int)canvas->height() && !visited[ny][nx]) {
                visited[ny][nx] = true;
                q.push({nx, ny});
            }
        }
    }
}

void BucketTool::on_mouse_down(Canvas* canvas, int x, int y) {
    Color target = canvas->get_pixel(x, y);
    flood_fill(canvas, x, y, target, foreground_);
}

void BucketTool::on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) {
}

void BucketTool::on_mouse_up(Canvas* canvas, int x, int y) {
}

} // namespace architect
} // namespace convoy
