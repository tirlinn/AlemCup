#include <stdio.h>
#include <stdlib.h>

struct cell
{
    int box, path, goal, delay;
};

struct bot
{
    int x, y, f_a, f_r;
};

struct pos
{
    int x, y;
};

void fill_map(struct cell** map, char* line, int i, int j);
void fill_entity(struct cell** map, struct bot* Deidara, struct bot* Saken, char ent_type, int p_id, int x, int y, int param_1, int param_2, int player_id, int h, int w);
void fill_bomb(struct cell** map, char ent_type, int p_id, int x, int y, int param_1, int param_2, int h, int w);
void fill_path(struct cell** map, struct bot* Deidara, int h, int w);
void get_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb);
void fill_goal(struct cell** map, struct bot* Deidara, int h, int w);
void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w);
void final_map(struct cell** map, struct bot* Deidara, int h, int w, struct pos* aim);
void find_dir(struct cell** map, struct bot* Deidara, struct pos* aim, int h, int w, int* dir);
void freentf(struct cell** map, int h, int w, struct pos* aim);

int main(void)
{
    struct bot* Deidara = malloc(sizeof(struct bot));
    Deidara->f_r = 2;
    struct bot* Saken = malloc(sizeof(struct bot));
    Saken->f_r = 2;
    while (1)
    {
        int h, w, player_id, tick;
        scanf("%d%d%d%d", &w, &h, &player_id, &tick);
        fprintf(stderr, "h - %d, w - %d, player_id - %d, tick - %d\n", h, w, player_id, tick);

        // read map
        struct cell** map = malloc(sizeof(struct cell*) * h);
        for (int i = 0; i < h; i++)
        {
            map[i] = malloc(sizeof(struct cell) * w);
            char* line = malloc(sizeof(char) * (w + 1));
            scanf("%s", line);
            for (int j = 0; j < w; j++)
            {
                fill_map(map, line, i, j);
            }
            free(line);
        }

        // number of entities
        int n;
        scanf("%d", &n);
        getchar();
        fprintf(stderr, "number of entities - %d\n", n);

        // read entities
        for (int i = 0; i < 2 * (n - 1) + 1; i++)
        {
            char ent_type;
            int p_id, x, y, param_1, param_2;
            scanf("%c", &ent_type);
            if (ent_type == 'f')
            {
                scanf("%c", &ent_type);
                scanf("%c", &ent_type);
                if (ent_type == 'r')
                    ent_type = 'l';
            }
            scanf("%d%d%d%d%d", &p_id, &x, &y, &param_1, &param_2);
            fill_entity(map, Deidara, Saken, ent_type, p_id, x, y, param_1, param_2, player_id, h, w);
        }

        fill_goal(map, Deidara, h, w);

        fill_path(map, Deidara, h, w);

        struct pos* aim = malloc(sizeof(struct pos));
        aim->y = Deidara->y;
        aim->x = Deidara->x;

        final_map(map, Deidara, h, w, aim);

        int dir = 5;

        find_dir(map, Deidara, aim, h, w, &dir);

        const char actions[][10] = { "left", "right", "up", "down", "bomb", "stay" };

        // bot action
        printf("%s\n", actions[dir]);
        fflush(stdout);

        freentf(map, h, w, aim);
    }
    free(Saken);
    free(Deidara);
    return 0;
}

void fill_entity(struct cell** map, struct bot* Deidara, struct bot* Saken, char ent_type, int p_id, int x, int y, int param_1, int param_2, int player_id, int h, int w)
{
    switch (ent_type)
    {
    case 'p':
        if (p_id == player_id)
        {
            Deidara->x = x;
            Deidara->y = y;
            Deidara->f_a = param_1;
            fprintf(stderr, "Deidara: x - %d, y - %d, magazine - %d\n", Deidara->x, Deidara->y, Deidara->f_a);
        }
        else
        {
            Saken->x = x;
            Saken->y = y;
            Saken->f_a = param_1;
            fprintf(stderr, "Saken: x - %d, y - %d, magazine - %d\n", Saken->x, Saken->y, Saken->f_a);
        }
        break;

    case 'b':
        fill_bomb(map, ent_type, p_id, x, y, param_1, param_2, h, w);
        fprintf(stderr, "bomb: x - %d, y - %d, tick - %d, radius - %d\n", x, y, param_1, param_2);
        break;

    case 'a':
        fprintf(stderr, "Debug a\n");
        break;

    case 'l':
        fprintf(stderr, "Debug l\n");
        break;

    default: break;
    }
}

void fill_bomb(struct cell** map, char ent_type, int p_id, int x, int y, int param_1, int param_2, int h, int w)
{
    map[y][x].box = 50;
    map[y][x].goal = -150;

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;
        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal = -150;
            map[y][tmp_x].box = -1 * param_1;
        }
        else
            break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;
        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal = -150;
            map[y][tmp_x].box = -1 * param_1;
        }
        else
            break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;
        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].goal = -150;
            map[tmp_y][x].box = -1 * param_1;
        }
        else
            break;
    }

    for (int i = 1; i <= param_2; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;
        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].goal = -150;
            map[tmp_y][x].box = -1 * param_1;
        }
        else
            break;
    }
}

void fill_map(struct cell** map, char* line, int i, int j)
{
    switch (line[j])
    {
    case '.':
        map[i][j].box = 1;
        break;

    case ';':
        map[i][j].box = 10;
        break;

    case '!':
        map[i][j].box = 100;
        break;
    default:
        break;
    }
    map[i][j].path = 1000;
    map[i][j].goal = 0;
    map[i][j].delay = 0;
}

void fill_goal(struct cell** map, struct bot* Deidara, int h, int w)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].box == 10)
                fill_box(map, Deidara, i, j, h, w);
        }
    }
}

void fill_box(struct cell** map, struct bot* Deidara, int y, int x, int h, int w)
{
    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x + i;
        if (tmp_x >= w) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal += 6;
        }
        else
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_x = x - i;
        if (tmp_x < 0) break;

        if (map[y][tmp_x].box == 1)
        {
            map[y][tmp_x].goal += 6;
        }
        else
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_y = y + i;
        if (tmp_y >= h) break;

        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].goal += 6;
        }
        else
            break;
    }

    for (int i = 1; i <= Deidara->f_r; i++)
    {
        int tmp_y = y - i;
        if (tmp_y < 0) break;

        if (map[tmp_y][x].box == 1)
        {
            map[tmp_y][x].goal += 6;
        }
        else
            break;
    }
}

void fill_path(struct cell** map, struct bot* Deidara, int h, int w)
{
    int step = 0;
    char prev = 0;
    int x = Deidara->x;
    int y = Deidara->y;
    int bomb = 0;
    get_point(map, y, x, step, prev, h, w, bomb);
}

void get_point(struct cell** map, int y, int x, int step, int prev, int h, int w, int bomb)
{
    //if (step >= 30) return;
    if (step == 0)
    {
        map[y][x].path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box == 1 && step < map[y][x].path)
    {
        map[y][x].path = step++;
        map[y][x].delay = 0;
        bomb = 0;
    }
    else if (map[y][x].box < 0 && step < map[y][x].path)
    {
        bomb++;
        if (map[y][x].box != -1 * step)
        {
            map[y][x].path = step++;
            map[y][x].delay = 0;
        }
        else
        {
            map[y][x].path = bomb + step++;
            map[y][x].delay = bomb;
        }
    }
    else
        return;

    if (prev != 'L' && x + 1 < w)
        get_point(map, y, x + 1, step, 'R', h, w, bomb);
    if (prev != 'U' && y + 1 < h)
        get_point(map, y + 1, x, step, 'D', h, w, bomb);
    if (prev != 'R' && x - 1 >= 0)
        get_point(map, y, x - 1, step, 'L', h, w, bomb);
    if (prev != 'D' && y - 1 >= 0)
        get_point(map, y - 1, x, step, 'U', h, w, bomb);
}

void final_map(struct cell** map, struct bot* Deidara, int h, int w, struct pos* aim)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i][j].goal == 0)
                map[i][j].goal = -135;
            map[i][j].goal -= map[i][j].path;
            if (map[i][j].goal > map[aim->y][aim->x].goal)
            {
                aim->y = i;
                aim->x = j;
            }
        }
    }
}

void find_dir(struct cell** map, struct bot* Deidara, struct pos* aim, int h, int w, int* dir)
{
    int y = aim->y;
    int x = aim->x;
    int wait = 0;
    struct pos next = { -1, -1 };
    if (y == Deidara->y && x == Deidara->x)
    {
        *dir = 4;
        return;
    }

    while (y != Deidara->y || x != Deidara->x)
    {
        if (x - 1 >= 0)
        {
            if (map[y][x - 1].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 1;
                x--;
                continue;
            }
        }
        if (y - 1 >= 0)
        {
            if (map[y - 1][x].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 3;
                y--;
                continue;
            }
        }
        if (x + 1 < w)
        {
            if (map[y][x + 1].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 0;
                x++;
                continue;
            }
        }
        if (y + 1 < h)
        {
            if (map[y + 1][x].path == map[y][x].path - 1 - map[y][x].delay)
            {
                if (map[y][x].delay != 0)
                {
                    next.y = y;
                    next.x = x;
                    wait = map[y][x].delay;
                }
                *dir = 2;
                y++;
                continue;
            }
        }
    }

    if (wait != 0 && wait == -map[next.y][next.x].box)
    {
        *dir = 5;
    }
}

void freentf(struct cell** map, int h, int w, struct pos* aim)
{
    // print maps
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "box: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%4d ", map[i][j].box);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "path: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%4d ", map[i][j].path);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < h; i++)
    {
        fprintf(stderr, "goal: ");
        for (int j = 0; j < w; j++)
        {
            fprintf(stderr, "%5d ", map[i][j].goal);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    //free part
    for (int i = 0; i < w; i++)
    {
        free(map[i]);
    }
    free(aim);
    free(map);
}