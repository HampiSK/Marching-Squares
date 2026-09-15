#include <array>
#include <vector>

#ifdef near
    #undef near
#endif

#ifdef far
    #undef far
#endif

#include <raylib.h>
#include <raymath.h>

struct Bubble
{
    float radius;
    Vector2 position;
    Vector2 velocity;

    Bubble() :
        radius(GetRandomValue(20, 60)),
        position(Vector2{ (float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight()) }),
        velocity(Vector2{ (float)GetRandomValue(-2, 2), (float)GetRandomValue(-2, 2) }) {}
};

struct Grid
{
    int columnSize;
    int rowSize;
    int cellSize;
    std::vector<float> data;

    Grid(int columnSize, int rowSize, int cellSize) :
        columnSize(columnSize), rowSize(rowSize), cellSize(cellSize), data(std::vector<float>(columnSize * rowSize, 0.0f)) {}
};

static void drawUsingMarchingSquares(Grid &grid)
{
    for (int i = 0; i < grid.columnSize - 1; ++i)
    {
        const int x = i * grid.cellSize;
        for (int j = 0; j < grid.rowSize - 1; ++j)
        {
            const int y = j * grid.cellSize;

            const float topLeft = grid.data[j * grid.columnSize + i];
            const float topRight = grid.data[j * grid.columnSize + i + 1];
            const float bottomRight = grid.data[(j + 1) * grid.columnSize + i + 1];
            const float bottomLeft = grid.data[(j + 1) * grid.columnSize + i];

            constexpr float FIELD_THRESHOLD = 1.0f;
            const int c1 = topLeft < FIELD_THRESHOLD ? 0 : 1;
            const int c2 = topRight < FIELD_THRESHOLD ? 0 : 1;
            const int c3 = bottomRight < FIELD_THRESHOLD ? 0 : 1;
            const int c4 = bottomLeft < FIELD_THRESHOLD ? 0 : 1;
            const int state = c1 * 8 + c2 * 4 + c3 * 2 + c4;

            const Vector2 edgeTop{ Lerp(x, x + grid.cellSize, (FIELD_THRESHOLD - topLeft) / (topRight - topLeft)), y };
            const Vector2 edgeRight{ x + grid.cellSize, Lerp(y, y + grid.cellSize, (FIELD_THRESHOLD - topRight) / (bottomRight - topRight)) };
            const Vector2 edgeBottom{ Lerp(x, x + grid.cellSize, (FIELD_THRESHOLD - bottomLeft) / (bottomRight - bottomLeft)), y + grid.cellSize };
            const Vector2 edgeLeft{ x, Lerp(y, y + grid.cellSize, (FIELD_THRESHOLD - topLeft) / (bottomLeft - topLeft)) };

            constexpr float LINE_THICKNESS = 5.0f;
            switch (state)
            {
                case 1: DrawLineEx(edgeBottom, edgeLeft, LINE_THICKNESS, WHITE); break;
                case 2: DrawLineEx(edgeRight, edgeBottom, LINE_THICKNESS, WHITE); break;
                case 3: DrawLineEx(edgeRight, edgeLeft, LINE_THICKNESS, WHITE); break;
                case 4: DrawLineEx(edgeTop, edgeRight, LINE_THICKNESS, WHITE); break;
                case 5:
                    DrawLineEx(edgeTop, edgeLeft, LINE_THICKNESS, WHITE);
                    DrawLineEx(edgeRight, edgeBottom, LINE_THICKNESS, WHITE);
                break;
                case 6: DrawLineEx(edgeTop, edgeBottom, LINE_THICKNESS, WHITE); break;
                case 7: DrawLineEx(edgeTop, edgeLeft, LINE_THICKNESS, WHITE); break;
                case 8: DrawLineEx(edgeTop, edgeLeft, LINE_THICKNESS, WHITE); break;
                case 9: DrawLineEx(edgeTop, edgeBottom, LINE_THICKNESS, WHITE); break;
                case 10:
                    DrawLineEx(edgeTop, edgeRight, LINE_THICKNESS, WHITE);
                    DrawLineEx(edgeBottom, edgeLeft, LINE_THICKNESS, WHITE);
                break;
                case 11: DrawLineEx(edgeTop, edgeRight, LINE_THICKNESS, WHITE); break;
                case 12: DrawLineEx(edgeRight, edgeLeft, LINE_THICKNESS, WHITE); break;
                case 13: DrawLineEx(edgeRight, edgeBottom, LINE_THICKNESS, WHITE); break;
                case 14: DrawLineEx(edgeBottom, edgeLeft, LINE_THICKNESS, WHITE); break;
                default: break;
            }
        }
    }
}

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, GAME_NAME);
    SetTargetFPS(60);

    constexpr float CELL_SIZE = 10.0f;
    Grid grid = Grid(1 + GetScreenWidth() / CELL_SIZE, 1 + GetScreenHeight() / CELL_SIZE, CELL_SIZE);
    std::array<Bubble, 18> bubbles;

    while (!WindowShouldClose())
    {
        // Uppdate bubbles
        for (Bubble &bubble : bubbles)
        {
            bubble.position.x += bubble.velocity.x;
            bubble.position.y += bubble.velocity.y;

            if (bubble.position.x - bubble.radius < 0.0f || bubble.position.x + bubble.radius > (float)GetScreenWidth()) bubble.velocity.x = -bubble.velocity.x;
            if (bubble.position.y - bubble.radius < 0.0f || bubble.position.y + bubble.radius > (float)GetScreenHeight()) bubble.velocity.y = -bubble.velocity.y;
        }

        // Sample the 'meatball' scalar field at every grid point
        for (int i = 0; i < grid.columnSize - 1; ++i)
        {
            const int x = i * grid.cellSize;
            for (int j = 0; j < grid.rowSize - 1; ++j)
            {
                const int y = j * grid.cellSize;

                float sum = 0.0f;
                for (const Bubble &bubble : bubbles)
                {
                    const float dx = x - bubble.position.x;
                    const float dy = y - bubble.position.y;
                    sum += (bubble.radius * bubble.radius) / (dx * dx + dy * dy);
                }
                grid.data[j * grid.columnSize + i] = sum;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        drawUsingMarchingSquares(grid);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
